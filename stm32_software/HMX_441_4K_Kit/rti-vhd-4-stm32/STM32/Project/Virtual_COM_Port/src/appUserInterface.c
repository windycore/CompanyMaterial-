// ---------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>> COPYRIGHT NOTICE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ---------------------------------------------------------------------------
//
//
//
// This is an unpublished work.
// ---------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>>>>> WARRANTEE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ---------------------------------------------------------------------------
/* ---------------------------- END CUSTOM AREA --------------------------- */
#include "appUserInterface.h"
#include "si_timer.h"
#include "Util.h"
#include "utilDataMgrAPI.h"
#include "dvPC9539.h"
#include "si_logger.h"
#include "appSystemMgrAPI.h"
#include "si_tx_component.h"
#include "mcu.h"
#include "main.h"
#include "dvLcd.h"
#include "Uart.h"
#include "appPowerMgrAPI.h"
#include "Si_app_adapter_rx.h"
#include "Sk_avr_repeater.h"
#include "KeyTasks.h"
#include "si_drv_tpi_system.h"
#include "halPowerCtrlAPI.h"
#include "LedReceiveEvent.h"
#include "OSAL.h"
#include "IRTasks.h"
#include "LcdHandle.h"
// ***************************************************************************
// *************** START OF PRIVATE TYPE AND SYMBOL DEFINITIONS **************
// ***************************************************************************
// This file does not contain any non-configurable definitions.
// ***************************************************************************
// ****************** START OF PRIVATE CONFIGURATION SECTION *****************
// ***************************************************************************
#define PRD_TYPE_TXT   "HDBaseT Matrix"
#define PRD_NAME_TXT   "HMX 442LP4K"
// define the time to exit if no key or mesage input
#define DEFAULT_UI_CLEAR mS_TO_TICKS(6000L)
#define DEFAULT_UI_ENTER mS_TO_TICKS(3000L)
char LCD1_IDLE_TEXT[17] = PRD_TYPE;
char LCD2_IDLE_TEXT[17] = PRD_NAME;
char m_acProduct_Type[17]= PRD_TYPE;
char m_acProduct_Name[17]= PRD_NAME;
/* -------------- BEGIN CUSTOM AREA [010] appUserInterface.c -----------------
 * CONFIGURATION: This custom area contains configurable includes and defines.
 * Add custom includes, defines and types here */
//static KEY_EVENT sLastKeEvent;
typedef enum
{
    /* ---------------- BEGIN CUSTOM AREA [010] utilUITypes.h --------------------
     * CONFIGURATION: Add user input events here */
    seNONE,
    seSAVE,
    seLOAD,
    seINVLAID,
} eSENSESTATUS;
/* ---------------------------- END CUSTOM AREA --------------------------- */
// ***************************************************************************
// ******************* END OF PRIVATE CONFIGURATION SECTION ******************
// ***************************************************************************
// current and next selected states of user interface
// ---------------------------------------------------------------------------
static eUI_STATE        m_eState;
static eUI_STATE        m_eNextState;
static eUartLed_STATE        m_eUartLedState;
// ---------------------------------------------------------------------------
// flag indicating that OSD graphics need to be updated (redrawn)
// ---------------------------------------------------------------------------
static BOOL             m_bUpdateUI;
//static BOOL             m_bUIEnabled = TRUE;
// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
static DWORD           m_wLedToFlashing;   // 用于记录那些灯要闪烁
//
//  -True: 正在闪烁, 计时器里判断如果TRUE 在闪烁,时间到了更新,不退出
//                                                                     如果FASLE,  CLEAR 时间到了就退出到IDEAL
//             一般在RENDER 中显示完后根据情况把他设立起来
//                        在RENDER 中闪烁中达到次数清楚
//
// etUI_CLEAR 时间到闪烁退出, 闪烁频率 etUI_FLASHING
// 总共闪烁时间由 etUI_CLEAR 决定
static BOOL            m_bLedFlashingEN = FALSE;  // 1: 开始闪烁
//
//
//  闪烁次数
static BYTE             m_cLedFlashCNT = 0;  // 闪烁次数, //一般在RENDER 中++
//--------------------------------------------------------------------
// etUI_CLEAR 时间到字符闪烁退出, 闪烁频率 etUI_FLASHING
// 总共闪烁时间由 etUI_CLEAR 决定
static BOOL            m_bUIFlashingEN = FALSE;  // 字符闪烁计时器使能
static BOOL            m_bCharFlashing = FALSE;  // 0: 正常显示1:  显示空
//--------------------------------------------
// uiSWITCH_VIDEO
static BYTE   m_cSwitchInputPort = 0;            // 切换下 那个输入口被选中
static BOOL   m_abSwitchOutPortSelect[MAX_OUT];  // 切换 下那些输出口被选中
static BOOL   m_bSwitchEnter = FALSE;            // 有效条件下enter 按键按下
//---------------------------------------------
// uiSWITCH_INFO
static BYTE   m_cSwitcherInfoPort = 0;
//---------------------------------------------
//uiEDID_EXTERN_MANAGE
static BYTE   m_cEdidManageInPort = 0;
static BYTE   m_cEdidManageOutPort = 0;
//----------------------------------------------
//uiUART_MESSAGE
static BYTE m_bUartMessageSend = FALSE; // 是否需要发送数据
static BYTE m_cUartMessage1[LCD_MAX_CHAR];
static BYTE m_cUartMessage2[LCD_MAX_CHAR];
//-----------------------------------------------
//uiSYSTEM_SWITCHPORT_INFO
static BYTE m_bOutPortSwitcherInf;
//---------------------------------------------------
//ePrevSwitchKey
static eKEY ePrevSwitchKey[8] = {keNONE,keNONE,keNONE,keNONE,keNONE,keNONE,keNONE,keNONE};
extern void dvLCDLineTXTDraw(BYTE  cLine,BYTE cPostion,...);
#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
extern BYTE KeyInput[8];
static WORD preLinkstatus = 0;
#endif

extern BOOL  System_UnLock_flag;
extern BYTE   u8IChannellock[MAX_OUT];

#ifdef MODIFY_IR_SWITCH_2017_2_21
extern bool g_bLEDUpdateMainLoopFlag;
extern bool g_bFlashSpillTimeFlag;
extern WORD g_wFlashSpillTimeDelay;
extern WORD g_LEDFlashBlankTimeCnt;//LED闪烁时间间隔
extern void LEDFlash(bool bLignt);
//哪一组输出口需要闪烁
#if (MACHINETYPE == MUH4K862)
bool g_bFlashOutPortSelect[8] = {false, false, false, false, false, false, false, false};
#elif (MACHINETYPE==MHUB4K431)
bool g_bFlashOutPortSelect[4] ={false, false, false, false};
#endif
#endif
//====================================================================
eKEY Get_PrevSwitchKey(BYTE tIdex)
{
    return ePrevSwitchKey[tIdex];
}

void Set_PrevSwitchKey(BYTE tIdex,eKEY tprekey)
{
    ePrevSwitchKey[tIdex]=tprekey;
}

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
void Set_PreLinkStatus(WORD u16Value)
{
    preLinkstatus = u16Value;
}

WORD Get_PreLinkStatus(void)
{
    return preLinkstatus;
}

#endif

// ---------------------------------------------------------------------------
// Process state changes on timeouts. If we are in a UI state that has a
// timeout and the timeout occurs, then transition to the idle state.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
static void ProcessStateChangeOnTimeout(void)
{
	#ifdef MODIFY_IR_SWITCH_2017_2_21
	BYTE i = 0;
	if(g_bFlashSpillTimeFlag)
	{
		g_LEDFlashBlankTimeCnt = 0;
		g_bFlashSpillTimeFlag = false;
    	g_bLEDUpdateMainLoopFlag = true;
    	for (i = 0; i < MAX_OUT; i++)
			g_bFlashOutPortSelect[i] = false;
		m_eNextState = uiSPLASH;
		m_bUpdateUI = TRUE;
	}
	#endif
	
    if (SiiPlatformTimerExpired(etUI_CLEAR))
    {
        m_bUIFlashingEN = FALSE;
        m_bLedFlashingEN  = FALSE;
        m_cLedFlashCNT = 0;
        switch (m_eState)
        {
        case uiIDLE:
            break;
        case uiSWITCH_VIDEO:
        case uiEDID_EXTERN_MANAGE:
            /*
                        if (m_bLedFlashing)
                        {
                            m_bUpdateUI = TRUE;
                        }
                        else
            */
        {
        /*
        	#ifdef MODIFY_IR_SWITCH_2017_2_21
        	BYTE i = 0;
        	g_bLEDTimeOverFlag = false;
        	g_bLEDUpdateMainLoopFlag = true;
        	for (i = 0; i < MAX_OUT; i++)
					g_bFlashOutPortSelect[i] = false;
        	#endif
        	*/
        	#ifdef MODIFY_IR_SWITCH_2017_2_21
        	if(g_bLEDUpdateMainLoopFlag)
        	{
        	#endif
            	m_eNextState = uiSPLASH;
            #ifdef MODIFY_IR_SWITCH_2017_2_21
            }
            #endif
            m_bUpdateUI = TRUE;
        }
        break;
        case uiSWITCH_INFO:
            m_eNextState = uiSPLASH;
            m_bUpdateUI = TRUE;
            break;
        case uiSYSTEM_LOCK_INFO:
        case uiSYSTEM_NAME:
        case uiSYSTEM_VERSION:
        case uiSYSTEM_MESSAGE_ONOFF_INFO:
            m_eNextState = uiIDLE;
            m_bUpdateUI = TRUE;
            break;
        case uiSPLASH:
        default:
            m_eNextState = uiIDLE;
            m_bUpdateUI = TRUE;
            break;
        }
    }
    // 字符闪烁
    //
    //   |---------------------3.5s-------------------------------------->
    //   on ---------->  off -------->  on  ----> off-------->on ---> off
    //   |    nex loop    |    1s       |    1s   |       1s    |
    //   0             1             2          3           4          5
    //
    if ( m_bLedFlashingEN)
    {
        if (SiiPlatformTimerExpired(etUI_FLASHING))
        {
            SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(600L),0xFF);
            m_bUpdateUI = TRUE;
            m_cLedFlashCNT++;
            if (m_cLedFlashCNT>4)
            {
                m_bLedFlashingEN = FALSE;
                //  如果闪烁完后, 不退出本状态, 任然进入本状态的render,
                //  本状态根据是否 m_bLedFlashingEN 来判断是否需要闪烁,会进入死循环,一直闪烁下去。
                m_eNextState = uiSPLASH;
            }
        }
    }
    else    if(m_bUIFlashingEN)
    {
        if (SiiPlatformTimerExpired(etUI_FLASHING))
        {
            SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(600L),0xFF);
            m_bUpdateUI = TRUE;
        }
    }
    /*
    // Led 闪烁, 去掉公用一个计时器
    if (m_bUIFlashingEN)
    {
        if (SiiPlatformTimerExpired(etLED_FLASHING))
        {
            SiiPlatformTimerSet(etLED_FLASHING, mS_TO_TICKS(1000L),0xFF);
            m_bUpdateUI = TRUE;
        }
    }
    */
}
// ---------------------------------------------------------------------------
// Determine if event should be filter out and not handled by normal event
// processing.
//
// Params:
//  eKey : Event key input
//
// Return:
//  TRUE= Event should be ignored, FALSE= Event should be handled by normal
//  event processing.
// ---------------------------------------------------------------------------
//#pragma argsused
#if 0
static BOOL EventFilter(eKEY eKey)
{
    BOOL        bFiltered = FALSE;
    return bFiltered;
}
// ---------------------------------------------------------------------------
// Default event handler if not handled by normal event processing.
//
// Params:
//  eState : New user interface state
//
// Return: none
// ---------------------------------------------------------------------------
static void EventDefault(eUI_STATE eState)
{
}
// ---------------------------------------------------------------------------
// Handles the keEXIT event.
//
// Params:
//  eState : New user interface state
//
// Return:
//  Update new user interface state
// ---------------------------------------------------------------------------
static eUI_STATE EventExit(eUI_STATE eState)
{
    return uiIDLE; // TODO
}
// ---------------------------------------------------------------------------
// Handles the keProgramTable event.
//
// Params:
//  eState : New user interface state
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Handles the keAUTO event.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Handles the keTIMER event.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
static void EventKeyTimer(void)
{
}
#endif
// ---------------------------------------------------------------------------
// Perform processing for entering a new UI state
//
// Params:
//  eState : Current state of the UI
//
// Return: none
// ---------------------------------------------------------------------------
//  it is not good ideal add KEY_EVENT param for this function
// 新进入一个状态才调用
static void UIStateEnter(eUI_STATE eState,
                         KEY_EVENT sKeyEvent)
{
    LED_EVENT sLedEvent;
    Event_t bEvent;
    /* -------------- BEGIN CUSTOM AREA [020] appUserInterface.c -------------- */
    dvLCD_On(TRUE);
    switch (eState)
    {
    case uiSPLASH:
        dvLCDLineDraw(0, 0, (PBYTE)LCD1_IDLE_TEXT);
        dvLCDLineDraw(1, 0, (PBYTE)LCD2_IDLE_TEXT);
        dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
        if (sKeyEvent.eKey == keCLEAR)
        {
            dvLedOnSet(1 << elnCLEAR);
        }
        if (sKeyEvent.eKey == keENTER)
        {
            //Event_t   bEvent;
        bEvent.Head.opcode=sLEDx_ON;
        bEvent.Head.DestAddr=mqLED;
        bEvent.Head.argCount=1;
        bEvent.args[0]=elnENTER;
           utilExecOlnyMsgSend(mqLED,
                            bEvent);
               /****************/   
            dvLedOnSet(1 << elnENTER);
        }
        SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
        break;
    case uiSWITCH_VIDEO:
    case uiUART_SWITCH_VIDEO2:
    case uiUART_SWITCH_IR:
        if (m_cSwitchInputPort<MAX_OUT)
        {
            dvLedAllOnSet(1 << (elnIN1 + m_cSwitchInputPort));
            /*   test  massege
                    sLedEvent.opcode=sLEDx_ON;
                 sLedEvent.args[0]=elnIN1+m_cSwitchInputPort;
                 sLedEvent.argCount=1;
                  //  utilExecMsgSend(mqLED,
                    //                    (BYTE *)&sLedEvent);

                //  osal_msg_send(mqLED, (BYTE *)&sLedEvent);
            */
            dvLCDLineTXTDraw(1, 0, "%2x", m_cSwitchInputPort+1);
        }
        dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
        SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
        break;
    case uiSWITCH_INFO:
        dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
        break;
    case uiEDID_EXTERN_MANAGE:
        dvLCDLineDraw(0, 0,  "EDID Manage");
        dvLCDLineDraw(1, 0,  "EDIDM");
        dvLedAllOnSet(1 << (elnEDID));
        m_wLedToFlashing |= (1 << (elnEDID));
        break;
    case uiEDID_INTERN_MANAGE:
        dvLCDLineDraw(0, 0, "Select IntEDID");
        dvLCDLineDraw(1, 0, "EDID/");
        dvLedAllOnSet(1 << (elnEDID));
        break;
    case uiSYSTEM_INCONNECT_INFO:
    case uiUART_INCONNECT_INFO:
        dvLCDLineDraw(0, 0, "In   01 02 03 04");
        dvLCDLineDraw(1, 0, "Connect  N N N N");
        //dvLedAllOnSet(1 << (elnEDID));
        break;
    case uiSYSTEM_OUTCONNECT_INFO:
    case uiUART_OUTCONNECT_INFO:
        dvLCDLineDraw(0, 0, "Out  01 02 03 04");
        dvLCDLineDraw(1, 0, "Connect  N N N N");
        //dvLedAllOnSet(1 << (elnEDID));
        break;
    case uiSYSTEM_SWITCH_INFO:
    case uiUART_SWITCH_INFO:
    case uiUART_SWITCH_ALL_PORT_INFO:
        dvLCDLineDraw(0, 0, "Out  01 02 03 04");
        dvLCDLineDraw(1, 0, "In   01 01 01 01");
        //dvLedAllOnSet(1 << (elnEDID));
        break;
    case uiSYSTEM_IN_HDCP_INFO:
    case uiUART_IN_HDCP_INFO:
        dvLCDLineDraw(0, 0, "In   01 02 03 04");
        dvLCDLineDraw(1, 0, "HDCP  N  N  N  N");
        //dvLedAllOnSet(1 << (elnEDID));
        break;
    case uiUART_IN_HDCPENABLE_INFO:
        dvLCDLineDraw(0, 0, "In   01 02 03 04");
        dvLCDLineDraw(1, 0, "HDCPEN  Y Y Y Y ");
        //dvLedAllOnSet(1 << (elnEDID));
        break;
    case uiUART_OUT_HDMIAUDIO_INFO:
        dvLCDLineDraw(0, 0, "Out  01 02 03 04");
        dvLCDLineDraw(1, 0, "Audio Y  Y  Y  Y");
        //dvLedAllOnSet(1 << (elnEDID));
        break;
    case uiSYSTEM_OUT_HDCP_INFO:
    case uiUART_OUT_HDCP_INFO:
        dvLCDLineDraw(0, 0, "Out  01 02 03 04");
        dvLCDLineDraw(1, 0, "HDCP  N  N  N  N");
        //dvLedAllOnSet(1 << (elnEDID));
        break;
    case uiSYSTEM_OUT_RESOLUTION_INFO:
    case uiUART_OUT_RESOLUTION_INFO:
        dvLCDLineDraw(0, 0, "Resolution");
        dvLCDLineDraw(1, 0, "Out 1  0000x0000");
        m_cEdidManageOutPort = 0;
        m_bUIFlashingEN = TRUE;
        m_bCharFlashing = TRUE;
        //dvLedAllOnSet(1 << (elnEDID));
        break;
    case uiSYSTEM_LOCK_INFO:
        //dvLCDLineDraw(1, 0, "System Locked!");
        break;
    case uiSYSTEM_NAME:
        //dvLCDLineDraw(1, 0, sys_name);
        break;
    case uiSYSTEM_VERSION:
        // dvLCDLineDraw(1, 0, Version);
        break;
    case uiSYSTEM_MESSAGE_ONOFF_INFO:
        //dvLCDLineDraw(1, 0, "/:MessageOff;");
        //dvLCDLineDraw(1, 0, "/:MessageOn;");
        break;
    case uiUART_MESSAGE:
        // to render draw
        break;
    case uiIDLE:
        dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
        dvLCDLineDraw(1, 0, LCD2_IDLE_TEXT);
//        dvLCDLineDraw(0, 0, " ");
//       dvLCDLineDraw(1, 0, " ");
        dvLedAllOffSet(0xffffffff); // all led off, except power led
        m_bLedFlashingEN = FALSE;
        m_bUIFlashingEN = FALSE;
        dvLCD_On(FALSE);
        break;
    case uiUART_SYSTEM_TYPE:
        dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
        //dvLCDLineDraw(1, 0, LCD2_IDLE_TEXT);
        break;
    default:
        break;
    }
    /* ---------------------------- END CUSTOM AREA --------------------------- */
}
// ---------------------------------------------------------------------------
// Perform event handling processing for current UI state
//
// Params:
//  eState    : current UI state
//  sKeyEvent : event information structure
//
// Return: none
// ---------------------------------------------------------------------------
// 只有动作, 记录值
static void UIStateEvent(eUI_STATE eState,
                         KEY_EVENT sKeyEvent)
{
    BYTE i;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    //BYTE acTest[]="To Remote Message";
    switch (sKeyEvent.eKey)
    {
            // 不在某个状态下按键事件需要进入下一个状态,并且需要保存一些下一状态需要得变量
        case keStandBy:
            if(get_standby_flag()==1)
            {
                set_clear_standby_flag(0);
                Set_Tm1623ValueChangeFlag(true);
            }
            else
            {
                appPowerMgrPowerStateSet(pwrSTANDBY);
                Set_Tm1623ValueChangeFlag(true);
            }
            break;
        case keIN1:
        case keIN2:
        case keIN3:
        case keIN4:
        case keIN5:
        case keIN6:
        case keIN7:
        case keIN8:
	        if (uiEDID_INTERN_MANAGE != eState &&
            uiEDID_EXTERN_MANAGE != eState &&
            uiSWITCH_VIDEO != eState&&
            uiUART_SWITCH_VIDEO2 != eState&&
            uiUART_SWITCH_IR != eState)
            {
                m_eNextState = uiSWITCH_VIDEO;
                m_cSwitchInputPort = sKeyEvent.eKey - keIN1;
                m_bUpdateUI = TRUE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                for (i = 0; i < MAX_OUT; i++)
                {
                    m_abSwitchOutPortSelect[i] = FALSE;
                }
              Event_t   bEvent;
            bEvent.Head.opcode=sLEDx_ON;
            bEvent.Head.DestAddr=mqLED;
            bEvent.args[0]=m_cSwitchInputPort;
            bEvent.Head.argCount=1;
               utilExecOlnyMsgSend(mqLED,
                                bEvent);
                   /****************/   
                m_bLedFlashingEN = FALSE; // 在闪烁中(还没有退出状态)有新按键退出闪烁
                m_bSwitchEnter = FALSE;
                //m_cLedFlashCNT = 0;
            }
            break;
        case keOUT1:
        case keOUT2:
        case keOUT3:
        case keOUT4:
        case keOUT5:
        case keOUT6:
        case keOUT7:
        case keOUT8:
        //-----------------------------
        	#ifdef MODIFY_IR_SWITCH_2017_2_21
        	BYTE cOutPort;
            if (uiEDID_INTERN_MANAGE != eState &&
                    uiEDID_EXTERN_MANAGE != eState &&
                    //uiSWITCH_VIDEO != eState&&
                    uiUART_SWITCH_VIDEO2 != eState&&
                    uiUART_SWITCH_IR != eState)           
            {
        		m_eNextState = uiSWITCH_VIDEO;
                cOutPort = sKeyEvent.eKey - keOUT1;
                m_abSwitchOutPortSelect[cOutPort] = TRUE;
                g_bFlashOutPortSelect[cOutPort] = true;
                m_bUpdateUI = TRUE;
                m_bSwitchEnter = FALSE;
                m_bLedFlashingEN = FALSE;
                //SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                //SiiPlatformTimerSet(etUI_CLEAR, 10000L,0xFF);
                //g_LEDFlashOverTimeCnt = 2000;
                g_bLEDUpdateMainLoopFlag = false;
				g_LEDFlashBlankTimeCnt = 400;//马上开始闪烁
				g_wFlashSpillTimeDelay = 8000;//8s
            }
            #else
            if (uiEDID_INTERN_MANAGE != eState &&
            uiEDID_EXTERN_MANAGE != eState &&
            uiSWITCH_VIDEO != eState&&
            uiUART_SWITCH_VIDEO2 != eState&&
            uiUART_SWITCH_IR != eState)
            {
                //AV:01->01
                //IR:01->01
                m_cSwitcherInfoPort = sKeyEvent.eKey - keOUT1;
                m_eNextState = uiSWITCH_INFO;
                m_bUpdateUI = TRUE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
            }
            #endif
      //---------------------------------
            break;
      //---------------------------------
      #ifdef MODIFY_IR_SWITCH_2017_2_21
      	case keALL:
		    if (uiEDID_INTERN_MANAGE != eState &&
            uiEDID_EXTERN_MANAGE != eState &&
            //uiSWITCH_VIDEO != eState&&
            uiUART_SWITCH_VIDEO2 != eState&&
            uiUART_SWITCH_IR != eState) 
      		{
      			m_eNextState = uiSWITCH_VIDEO;
                for (i = 0; i < MAX_OUT; i++)
                {
                    m_abSwitchOutPortSelect[i] = TRUE;
                    g_bFlashOutPortSelect[i] = true;
                }
                m_bUpdateUI = TRUE;
                m_bSwitchEnter = FALSE;
                m_bLedFlashingEN = FALSE;
                //SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                //SiiPlatformTimerSet(etUI_CLEAR, 10000L,0xFF);
                //g_LEDFlashOverTimeCnt = 2000;
                g_bLEDUpdateMainLoopFlag = false;
				g_LEDFlashBlankTimeCnt = 400;//马上开始闪烁
				g_wFlashSpillTimeDelay = 8000;//8s
            }
      		break;
      #endif
      //---------------------------------
        case keEDID:
            if (uiEDID_EXTERN_MANAGE != eState )
            {
                m_eNextState = uiEDID_EXTERN_MANAGE;
                m_bUpdateUI = TRUE;
                m_cEdidManageOutPort = 0xff;
                m_cEdidManageInPort = 0xff;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
            }
            break;
        case keEDID_LONG:
            m_eNextState = uiEDID_INTERN_MANAGE;
            m_bUpdateUI = TRUE;
            m_cEdidManageInPort = 0xff;
            m_cEdidManageOutPort = 0xff;
            SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
            break;
        case keENTER_LONG:
        {
            m_eNextState = uiSYSTEM_INCONNECT_INFO;
            m_bUpdateUI = TRUE;
            SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
        }
        break;
        case keCLEAR:        	
			#ifdef MODIFY_IR_SWITCH_2017_2_21
			g_LEDFlashBlankTimeCnt = 0;
			g_bLEDUpdateMainLoopFlag = true;
			for (i = 0; i < MAX_OUT; i++)
				g_bFlashOutPortSelect[i] = false;
			#endif
            m_eNextState = uiSPLASH;
            m_bUpdateUI = TRUE;
            break;
        case ke1TO1:
        case ke1TO2:
        case ke1TO3:
        case ke1TO4:
        #if (MAX_OUT>4)
        case ke1TO5:
        case ke1TO6:
        case ke1TO7:
        case ke1TO8:
        #endif
            {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                BYTE cOuputPort = sKeyEvent.eKey - ke1TO1;
                if(psSystemSettingData->acIChannellock[0]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                                 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                 
                }
                else
                {
                    appUartSwitchInfoSet(0, 1, &cOuputPort,TRUE);
                    appSystemSwitcherPortSet(0, 1, &cOuputPort);
		      appSystemIRSwitcherPortSet(0, 1, &cOuputPort);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
            break;
        case ke2TO1:
        case ke2TO2:
        case ke2TO3:
        case ke2TO4:
        #if (MAX_OUT>4)
        case ke2TO5:
        case ke2TO6:
        case ke2TO7:
        case ke2TO8:
        #endif
        {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke2TO1;
                if(psSystemSettingData->acIChannellock[1]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                                 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                    
                }
                else
                {
                  appUartSwitchInfoSet(1, 1, &cOuputPort,TRUE);
                      appSystemSwitcherPortSet(1, 1, &cOuputPort);
            		appSystemIRSwitcherPortSet(1, 1, &cOuputPort);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
        break;
        case ke3TO1:
        case ke3TO2:
        case ke3TO3:
        case ke3TO4:
        #if (MAX_OUT>4)
        case ke3TO5:
        case ke3TO6:
        case ke3TO7:
        case ke3TO8:
        #endif
        {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke3TO1;
                if(psSystemSettingData->acIChannellock[2]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                                 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                    
                }
                else
                {
                  appUartSwitchInfoSet(2, 1, &cOuputPort,TRUE);
                      appSystemSwitcherPortSet(2, 1, &cOuputPort);
            		appSystemIRSwitcherPortSet(2, 1, &cOuputPort);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
        break;
        case ke4TO1:
        case ke4TO2:
        case ke4TO3:
        case ke4TO4:
        #if (MAX_OUT>4)
        case ke4TO5:
        case ke4TO6:
        case ke4TO7:
        case ke4TO8:
        #endif
        {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke4TO1;
                if(psSystemSettingData->acIChannellock[3]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                                 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                
                }
                else
                {
                  appUartSwitchInfoSet(3, 1, &cOuputPort,TRUE);
                      appSystemSwitcherPortSet(3, 1, &cOuputPort);
            		appSystemIRSwitcherPortSet(3, 1, &cOuputPort);
                }
                    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
        break;
        #if (MAX_OUT>4)
        case ke5TO1:
        case ke5TO2:
        case ke5TO3:
        case ke5TO4:
        case ke5TO5:
        case ke5TO6:
        case ke5TO7:
        case ke5TO8:
        {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke5TO1;
                if(psSystemSettingData->acIChannellock[4]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                                 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                
                }
                else
                {
                  appUartSwitchInfoSet(4, 1, &cOuputPort,TRUE);
                      appSystemSwitcherPortSet(4, 1, &cOuputPort);
            		appSystemIRSwitcherPortSet(4, 1, &cOuputPort);
                }
                    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
        break;
        #endif
        #if (MAX_OUT>5)
        case ke6TO1:
        case ke6TO2:
        case ke6TO3:
        case ke6TO4:
        case ke6TO5:
        case ke6TO6:
        case ke6TO7:
        case ke6TO8:
            {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke6TO1;
                if(psSystemSettingData->acIChannellock[5]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                                 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                
                }
                else
                {
                  appUartSwitchInfoSet(5, 1, &cOuputPort,TRUE);
                      appSystemSwitcherPortSet(5, 1, &cOuputPort);
            		appSystemIRSwitcherPortSet(5, 1, &cOuputPort);
                }
                    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
            break;
        #endif
        #if (MAX_OUT>6)
        case ke7TO1:
        case ke7TO2:
        case ke7TO3:
        case ke7TO4:
        case ke7TO5:
        case ke7TO6:
        case ke7TO7:
        case ke7TO8:
            {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke7TO1;
                if(psSystemSettingData->acIChannellock[6]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                                 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                
                }
                else
                {
                  appUartSwitchInfoSet(6, 1, &cOuputPort,TRUE);
                      appSystemSwitcherPortSet(6, 1, &cOuputPort);
            		appSystemIRSwitcherPortSet(6, 1, &cOuputPort);
                }
                    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
            break;
        #endif
        #if (MAX_OUT>7)
        case ke8TO1:
        case ke8TO2:
        case ke8TO3:
        case ke8TO4:
        case ke8TO5:
        case ke8TO6:
        case ke8TO7:
        case ke8TO8:
        {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke8TO1;
                if(psSystemSettingData->acIChannellock[7]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                                 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                    
                }
                else
                {
                    appUartSwitchInfoSet(7, 1, &cOuputPort,TRUE);
                    appSystemSwitcherPortSet(7, 1, &cOuputPort);
		      appSystemIRSwitcherPortSet(7, 1, &cOuputPort);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
        break;
        #endif
        case ke1TOALL:
        case ke2TOALL:
        case ke3TOALL:
        case ke4TOALL:
        #ifdef MODIFY_IR_SWITCH_2017_2_21
        #if (MAX_OUT>4)
        case ke5TOALL:
        case ke6TOALL:
        case ke7TOALL:
        case ke8TOALL:
        #endif
        #endif
        {
            BYTE acOutPortList[MAX_OUT] ;
            for(i=0; i<MAX_OUT; i++)
            {
                acOutPortList[i]=i;
            }
            BYTE cInPort = sKeyEvent.eKey - ke1TOALL;
            #ifdef MODIFY_IR_SWITCH_2017_2_21
            appUartSwitchInfoSet(cInPort, MAX_OUT, acOutPortList,TRUE);
            appSystemSwitcherPortSet(cInPort, MAX_OUT, acOutPortList);
	     appSystemIRSwitcherPortSet(cInPort, MAX_OUT, acOutPortList);
            #else
            appUartSwitchInfoSet(cInPort, 4, acOutPortList,TRUE);
            appSystemSwitcherPortSet(cInPort, 4, acOutPortList);
            #endif
        }
        break;
        #if (MACHINETYPE==MUH4K862)||(MACHINETYPE==MHUB4K431)
        case ke1DelayTO1:
        case ke1DelayTO2:
        case ke1DelayTO3:
        case ke1DelayTO4:
        case ke1DelayTO5:
        case ke1DelayTO6:
        case ke1DelayTO7:
        case ke1DelayTO8:
            {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                BYTE cOuputPort = sKeyEvent.eKey - ke1DelayTO1;
                #if 0
                if(psSystemSettingData->bSystemLock==TRUE)
                {
                    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                    appUIStateChange(uiSYSTEM_LOCK_INFO);
                    break;
                }
                #endif
                if(psSystemSettingData->acIChannellock[0]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                 
                }
                else
                {
                    Set_PrevSwitchKey(cOuputPort,sKeyEvent.eKey);
                    //appUartSwitchInfoSet(0, 1, &cOuputPort,TRUE);
                    //appSystemSwitcherPortSet(0, 1, &cOuputPort);
                    //appUIStateChange(uiSPLASH);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
            break;
        case ke2DelayTO1:
        case ke2DelayTO2:
        case ke2DelayTO3:
        case ke2DelayTO4:
        case ke2DelayTO5:
        case ke2DelayTO6:
        case ke2DelayTO7:
        case ke2DelayTO8:
        {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke2DelayTO1;
                if(psSystemSettingData->acIChannellock[1]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                     UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                    
                }
                else
                {
                    Set_PrevSwitchKey(cOuputPort,sKeyEvent.eKey);
                    //appUartSwitchInfoSet(1, 1, &cOuputPort,TRUE);
                    //appSystemSwitcherPortSet(1, 1, &cOuputPort);
                    //appUIStateChange(uiSPLASH);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
        break;
        case ke3DelayTO1:
        case ke3DelayTO2:
        case ke3DelayTO3:
        case ke3DelayTO4:
        case ke3DelayTO5:
        case ke3DelayTO6:
        case ke3DelayTO7:
        case ke3DelayTO8:
        {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke3DelayTO1;
                if(psSystemSettingData->acIChannellock[2]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                    
                }
                else
                {
                    Set_PrevSwitchKey(cOuputPort,sKeyEvent.eKey);
                    //appUartSwitchInfoSet(2, 1, &cOuputPort,TRUE);
                    //appSystemSwitcherPortSet(2, 1, &cOuputPort);
                    //appUIStateChange(uiSPLASH);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
        break;
        case ke4DelayTO1:
        case ke4DelayTO2:
        case ke4DelayTO3:
        case ke4DelayTO4:
        case ke4DelayTO5:
        case ke4DelayTO6:
        case ke4DelayTO7:
        case ke4DelayTO8:
        {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke4DelayTO1;
                if(psSystemSettingData->acIChannellock[3]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                
                }
                else
                {
                    Set_PrevSwitchKey(cOuputPort,sKeyEvent.eKey);
                    //appUartSwitchInfoSet(3, 1, &cOuputPort,TRUE);
                    //appSystemSwitcherPortSet(3, 1, &cOuputPort);
                    //appUIStateChange(uiSPLASH);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
        break;
        #if (MAX_OUT>4)
        case ke5DelayTO1:
        case ke5DelayTO2:
        case ke5DelayTO3:
        case ke5DelayTO4:
        case ke5DelayTO5:
        case ke5DelayTO6:
        case ke5DelayTO7:
        case ke5DelayTO8:
        {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke5DelayTO1;
                if(psSystemSettingData->acIChannellock[4]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                
                }
                else
                {
                    Set_PrevSwitchKey(cOuputPort,sKeyEvent.eKey);
                    //appUartSwitchInfoSet(4, 1, &cOuputPort,TRUE);
                    //appSystemSwitcherPortSet(4, 1, &cOuputPort);
                    //appUIStateChange(uiSPLASH);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
        break;
        #endif
        #if (MAX_OUT>5)
        case ke6DelayTO1:
        case ke6DelayTO2:
        case ke6DelayTO3:
        case ke6DelayTO4:
        case ke6DelayTO5:
        case ke6DelayTO6:
        case ke6DelayTO7:
        case ke6DelayTO8:
            {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke6DelayTO1;
                if(psSystemSettingData->acIChannellock[5]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                
                }
                else
                {
                    Set_PrevSwitchKey(cOuputPort,sKeyEvent.eKey);
                    //appUartSwitchInfoSet(5, 1, &cOuputPort,TRUE);
                    //appSystemSwitcherPortSet(5, 1, &cOuputPort);
                    //appUIStateChange(uiSPLASH);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
            break;
        #endif
        #if (MAX_OUT>6)
        case ke7DelayTO1:
        case ke7DelayTO2:
        case ke7DelayTO3:
        case ke7DelayTO4:
        case ke7DelayTO5:
        case ke7DelayTO6:
        case ke7DelayTO7:
        case ke7DelayTO8:
            {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke7DelayTO1;
                if(psSystemSettingData->acIChannellock[6]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                
                }
                else
                {
                    Set_PrevSwitchKey(cOuputPort,sKeyEvent.eKey);
                    //appUartSwitchInfoSet(6, 1, &cOuputPort,TRUE);
                    //appSystemSwitcherPortSet(6, 1, &cOuputPort);
                    //appUIStateChange(uiSPLASH);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
            break;
        #endif
        #if (MAX_OUT>7)
        case ke8DelayTO1:
        case ke8DelayTO2:
        case ke8DelayTO3:
        case ke8DelayTO4:
        case ke8DelayTO5:
        case ke8DelayTO6:
        case ke8DelayTO7:
        case ke8DelayTO8:
            {
                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                 BYTE cOuputPort = sKeyEvent.eKey - ke8DelayTO1;
                if(psSystemSettingData->acIChannellock[7]&BIT(cOuputPort))
                {
                    sprintf(USART1_tx_buff,"This input is locked!\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                    
                }
                else
                {
                    Set_PrevSwitchKey(cOuputPort,sKeyEvent.eKey);
                    //appUartSwitchInfoSet(7, 1, &cOuputPort,TRUE);
                    //appSystemSwitcherPortSet(7, 1, &cOuputPort);
                    //appUIStateChange(uiSPLASH);
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
            }
            break;
        #endif
        
        case keENTER:
            {
                for(i=0;i<MAX_OUT;i++)
                {
                    if(Get_PrevSwitchKey(i)<ke1DelayTO1||Get_PrevSwitchKey(i)>(ke1DelayTO1+8*MAX_OUT-1))
                        continue;
                    switch(Get_PrevSwitchKey(i))
                    {
                        case ke1DelayTO1:
                        case ke1DelayTO2:
                        case ke1DelayTO3:
                        case ke1DelayTO4:
                        case ke1DelayTO5:
                        case ke1DelayTO6:
                        case ke1DelayTO7:
                        case ke1DelayTO8:
                            {
                                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                                BYTE cOuputPort = Get_PrevSwitchKey(i)- ke1DelayTO1;
                                //appUartSwitchInfoSet(0, 1, &cOuputPort,TRUE);
                                appUartSwitchSatusSet(0, 1, &cOuputPort);
                                #ifdef PRD_IR_OUT
                                appSystemIRSwitcherPortSet(0, 1, &cOuputPort);
                                #endif
                                appSystemSwitcherPortSet(0, 1, &cOuputPort);
                                sprintf(USART1_tx_buff,"AV:01->%02d\r\n",cOuputPort+1);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                                //eState = uiSPLASH;
                            }
                            break;
                        case ke2DelayTO1:
                        case ke2DelayTO2:
                        case ke2DelayTO3:
                        case ke2DelayTO4:
                        case ke2DelayTO5:
                        case ke2DelayTO6:
                        case ke2DelayTO7:
                        case ke2DelayTO8:
                            {
                                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                                BYTE cOuputPort = Get_PrevSwitchKey(i)- ke2DelayTO1;
                                //appUartSwitchInfoSet(1, 1, &cOuputPort,TRUE);
                                appUartSwitchSatusSet(1, 1, &cOuputPort);
                                #ifdef PRD_IR_OUT
                                appSystemIRSwitcherPortSet(1, 1, &cOuputPort);
                                #endif
                                appSystemSwitcherPortSet(1, 1, &cOuputPort);
                                sprintf(USART1_tx_buff,"AV:02->%02d\r\n",cOuputPort+1);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                                //eState = uiSPLASH;
                            }
                            break;
                        case ke3DelayTO1:
                        case ke3DelayTO2:
                        case ke3DelayTO3:
                        case ke3DelayTO4:
                        case ke3DelayTO5:
                        case ke3DelayTO6:
                        case ke3DelayTO7:
                        case ke3DelayTO8:
                            {
                                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                                BYTE cOuputPort = Get_PrevSwitchKey(i)- ke3DelayTO1;
                                //appUartSwitchInfoSet(2, 1, &cOuputPort,TRUE);
                                appUartSwitchSatusSet(2, 1, &cOuputPort);
                                #ifdef PRD_IR_OUT
                                appSystemIRSwitcherPortSet(2, 1, &cOuputPort);
                                #endif
                                appSystemSwitcherPortSet(2, 1, &cOuputPort);
                                sprintf(USART1_tx_buff,"AV:03->%02d\r\n",cOuputPort+1);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                                //eState = uiSPLASH;
                            }
                            break;
                        case ke4DelayTO1:
                        case ke4DelayTO2:
                        case ke4DelayTO3:
                        case ke4DelayTO4:
                        case ke4DelayTO5:
                        case ke4DelayTO6:
                        case ke4DelayTO7:
                        case ke4DelayTO8:
                            {
                                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                                BYTE cOuputPort = Get_PrevSwitchKey(i)- ke4DelayTO1;
                                //appUartSwitchInfoSet(3, 1, &cOuputPort,TRUE);
                                appUartSwitchSatusSet(3, 1, &cOuputPort);
                                #ifdef PRD_IR_OUT
                                appSystemIRSwitcherPortSet(3, 1, &cOuputPort);
                                #endif
                                appSystemSwitcherPortSet(3, 1, &cOuputPort);
                                sprintf(USART1_tx_buff,"AV:04->%02d\r\n",cOuputPort+1);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                                //eState = uiSPLASH;
                            }
                            break;
                        #if (MAX_OUT>4)
                        case ke5DelayTO1:
                        case ke5DelayTO2:
                        case ke5DelayTO3:
                        case ke5DelayTO4:
                        case ke5DelayTO5:
                        case ke5DelayTO6:
                        case ke5DelayTO7:
                        case ke5DelayTO8:
                            {
                                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                                BYTE cOuputPort = Get_PrevSwitchKey(i)- ke5DelayTO1;
                                //appUartSwitchInfoSet(4, 1, &cOuputPort,TRUE);
                                appUartSwitchSatusSet(4, 1, &cOuputPort);
                                #ifdef PRD_IR_OUT
                                appSystemIRSwitcherPortSet(4, 1, &cOuputPort);
                                #endif
                                appSystemSwitcherPortSet(4, 1, &cOuputPort);
                                sprintf(USART1_tx_buff,"AV:05->%02d\r\n",cOuputPort+1);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                                //eState = uiSPLASH;
                            }
                            break;
                        #endif
                        #if (MAX_OUT>5)
                        case ke6DelayTO1:
                        case ke6DelayTO2:
                        case ke6DelayTO3:
                        case ke6DelayTO4:
                        case ke6DelayTO5:
                        case ke6DelayTO6:
                        case ke6DelayTO7:
                        case ke6DelayTO8:
                            {
                                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                                BYTE cOuputPort = Get_PrevSwitchKey(i)- ke6DelayTO1;
                                //appUartSwitchInfoSet(5, 1, &cOuputPort,TRUE);
                                appUartSwitchSatusSet(5, 1, &cOuputPort);
                                #ifdef PRD_IR_OUT
                                appSystemIRSwitcherPortSet(5, 1, &cOuputPort);
                                #endif
                                appSystemSwitcherPortSet(5, 1, &cOuputPort);
                                sprintf(USART1_tx_buff,"AV:06->%02d\r\n",cOuputPort+1);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                                //eState = uiSPLASH;
                            }
                            break;
                        #endif
                        #if (MAX_OUT>6)
                        case ke7DelayTO1:
                        case ke7DelayTO2:
                        case ke7DelayTO3:
                        case ke7DelayTO4:
                        case ke7DelayTO5:
                        case ke7DelayTO6:
                        case ke7DelayTO7:
                        case ke7DelayTO8:
                            {
                                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                                BYTE cOuputPort = Get_PrevSwitchKey(i)- ke7DelayTO1;
                                //appUartSwitchInfoSet(6, 1, &cOuputPort,TRUE);
                                appUartSwitchSatusSet(6, 1, &cOuputPort);
                                #ifdef PRD_IR_OUT
                                appSystemIRSwitcherPortSet(6, 1, &cOuputPort);
                                #endif
                                appSystemSwitcherPortSet(6, 1, &cOuputPort);
                                sprintf(USART1_tx_buff,"AV:07->%02d\r\n",cOuputPort+1);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                                //eState = uiSPLASH;
                            }
                            break;
                        #endif
                        #if (MAX_OUT>7)
                        case ke8DelayTO1:
                        case ke8DelayTO2:
                        case ke8DelayTO3:
                        case ke8DelayTO4:
                        case ke8DelayTO5:
                        case ke8DelayTO6:
                        case ke8DelayTO7:
                        case ke8DelayTO8:
                            {
                                PSYSTEM_SETTING_DATA   psSystemSettingData;  
                                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                                BYTE cOuputPort = Get_PrevSwitchKey(i)- ke8DelayTO1;
                                //appUartSwitchInfoSet(7, 1, &cOuputPort,TRUE);
                                appUartSwitchSatusSet(7, 1, &cOuputPort);
                                #ifdef PRD_IR_OUT
                                appSystemIRSwitcherPortSet(7, 1, &cOuputPort);
                                #endif
                                appSystemSwitcherPortSet(7, 1, &cOuputPort);
                                sprintf(USART1_tx_buff,"AV:08->%02d\r\n",cOuputPort+1);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                                //eState = uiSPLASH;
                            }
                            break;
                        #endif
                        default:
                            break;
                    }
                    Set_PrevSwitchKey(i,keNONE);
                }
            }
            break;           
        #endif
        default:
            #if 0//(MACHINETYPE==MUH4K862)
            for(i=0;i<MAX_OUT;i++)
            {
                Set_PrevSwitchKey(i,sKeyEvent.eKey);
            }
            #endif
            break;
    }
    switch (eState) // 在各自状态中按键消息处理
    {
        case uiSPLASH:
        {
            if (keENTER == sKeyEvent.eKey||
                    keCLEAR == sKeyEvent.eKey)
            {
                m_bUpdateUI = TRUE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
            }
        }
        break;
        case uiSWITCH_VIDEO:
        {
            BYTE cOutPort;
            //-------------------------------------------------------
            #ifdef MODIFY_IR_SWITCH_2017_2_21
            BYTE  cPortNumber = 0;
            BYTE  cPortList[MAX_OUT];
            if (keIN1 == sKeyEvent.eKey ||
                keIN2 == sKeyEvent.eKey ||
                keIN3 == sKeyEvent.eKey ||
                keIN4 == sKeyEvent.eKey||
                keIN5 == sKeyEvent.eKey||
                keIN6 == sKeyEvent.eKey||
                keIN7 == sKeyEvent.eKey||
                keIN8 == sKeyEvent.eKey
                )
            {					
                m_cSwitchInputPort = sKeyEvent.eKey - keIN1;
                m_bUpdateUI = TRUE;
                //SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                //for (i = 0; i < MAX_OUT; i++)
                //{
                    //m_abSwitchOutPortSelect[i] = FALSE;
                //}
              Event_t   bEvent;
            bEvent.Head.opcode=sLEDx_ON;
            bEvent.Head.DestAddr=mqLED;
            bEvent.args[0]=m_cSwitchInputPort;
            bEvent.Head.argCount=1;
               utilExecOlnyMsgSend(mqLED,
                                bEvent);
                   /****************/   
                m_bLedFlashingEN = FALSE; // 在闪烁中(还没有退出状态)有新按键退出闪烁
                m_bSwitchEnter = FALSE;
                //m_cLedFlashCNT = 0;
				//SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01); 
				LEDFlash(true);
                //sKeyEvent.eKey = keENTER;

                for (i = 0; i < MAX_OUT; i++)
                {
                    if (m_abSwitchOutPortSelect[i] == TRUE)
                    {
                        cPortList[cPortNumber] = i;
                        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                        //KeyInput[cPortList[cPortNumber]]=m_cSwitchInputPort;
                        //Set_Tm1623ValueChangeFlag(true);
                        SiiPlatformTimerSet(etLED_UPDATE,mS_TO_TICKS(2000L),0xFF); // use systick: 1ms per tick
                        #endif
                        cPortNumber++;
                     }
                }

                if (cPortNumber > 0)
                {
                    //  appSystemSwitcherPortSet(m_cSwitchInputPort, cPortNumber, cPortList);
                    Event_t   bEvent;
                    bEvent.Head.opcode=eLEDChannelSwitch;
                    bEvent.Head.DestAddr=mqLED;
                    bEvent.args[0]=m_cSwitchInputPort;
                    bEvent.args[1]=cPortNumber;
                    memcpy(&bEvent.args[2],cPortList,i);
                    bEvent.Head.argCount=cPortNumber+2;
                    utilExecOlnyMsgSend(mqLED,
                    bEvent);
                    /****************/   
                    //***********************************************
                    bEvent.Head.opcode=eSwitchxTox;
                    bEvent.Head.DestAddr=mqSWITCH;
                    bEvent.args[0]=m_cSwitchInputPort;
                    bEvent.args[1]=cPortNumber;
                    memcpy(&bEvent.args[2],cPortList,i);
                    bEvent.Head.argCount=cPortNumber+2;
                    utilExecOlnyMsgSend(mqSWITCH,
                    bEvent);
                    //***************************************************
                    #ifdef PRD_IR_OUT
                    appSystemIRSwitcherPortSet(m_cSwitchInputPort, cPortNumber, cPortList);
                    #endif
                    //m_bUpdateUI = TRUE;
                    //SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                    //m_bSwitchEnter = TRUE;
                    //m_cLedFlashCNT = 0;
                    #ifdef MODIFY_IR_SWITCH_2017_2_21
	        		g_LEDFlashBlankTimeCnt = 0;
	        		g_bLEDUpdateMainLoopFlag = true;
					for (i = 0; i < MAX_OUT; i++)
						g_bFlashOutPortSelect[i] = false;
					#endif
                    m_eNextState = uiSPLASH; // wf8421 to exit this event
                }
            }
            #else
            if (keOUT1 == sKeyEvent.eKey ||
                    keOUT2 == sKeyEvent.eKey ||
                    keOUT3 == sKeyEvent.eKey ||
                    keOUT4 == sKeyEvent.eKey||
                    keOUT5 == sKeyEvent.eKey||
                    keOUT6 == sKeyEvent.eKey||
                    keOUT7 == sKeyEvent.eKey||
                    keOUT8 == sKeyEvent.eKey
               )
            {
                cOutPort = sKeyEvent.eKey - keOUT1;
                m_abSwitchOutPortSelect[cOutPort] = TRUE;
                m_bUpdateUI = TRUE;
                m_bSwitchEnter = FALSE;
                m_bLedFlashingEN = FALSE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
            }
            if (keALL == sKeyEvent.eKey)
            {
                for (i = 0; i < MAX_OUT; i++)
                {
                    m_abSwitchOutPortSelect[i] = TRUE;
                }
                m_bUpdateUI = TRUE;
                m_bSwitchEnter = FALSE;
                m_bLedFlashingEN = FALSE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
            }
           
            if (keENTER == sKeyEvent.eKey)
            {
                BYTE  cPortNumber = 0;
                BYTE  cPortList[MAX_OUT];
             //     psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
        //          dvLedOnSet(1 << elnENTER);          //点亮ENTER灯
        //  memset(USART1_tx_buff,0,USART1_TX_BUFF_MAX);
                //begin switch
                for (i = 0; i < MAX_OUT; i++)
                {
                    if (m_abSwitchOutPortSelect[i] == TRUE)
                    {
                        cPortList[cPortNumber] = i;
                        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                        //KeyInput[cPortList[cPortNumber]]=m_cSwitchInputPort;
                        //Set_Tm1623ValueChangeFlag(true);
                        SiiPlatformTimerSet(etLED_UPDATE,mS_TO_TICKS(2000L),0xFF); // use systick: 1ms per tick
                        #endif
                        cPortNumber++;
                     }
                }
         //   UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
        //    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
                if (cPortNumber > 0)
                {
                    //  appSystemSwitcherPortSet(m_cSwitchInputPort, cPortNumber, cPortList);
                    Event_t   bEvent;
                    bEvent.Head.opcode=eLEDChannelSwitch;
                    bEvent.Head.DestAddr=mqLED;
                    bEvent.args[0]=m_cSwitchInputPort;
                    bEvent.args[1]=cPortNumber;
                    memcpy(&bEvent.args[2],cPortList,i);
                    bEvent.Head.argCount=cPortNumber+2;
                    utilExecOlnyMsgSend(mqLED,
                    bEvent);
                    /****************/   
                    //***********************************************
                    bEvent.Head.opcode=eSwitchxTox;
                    bEvent.Head.DestAddr=mqSWITCH;
                    bEvent.args[0]=m_cSwitchInputPort;
                    bEvent.args[1]=cPortNumber;
                    memcpy(&bEvent.args[2],cPortList,i);
                    bEvent.Head.argCount=cPortNumber+2;
                    utilExecOlnyMsgSend(mqSWITCH,
                    bEvent);
                    //***************************************************
                    #ifdef PRD_IR_OUT
                    appSystemIRSwitcherPortSet(m_cSwitchInputPort, cPortNumber, cPortList);
                    #endif
                    //m_bUpdateUI = TRUE;
                    //SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                    //m_bSwitchEnter = TRUE;
                    //m_cLedFlashCNT = 0;
                    m_eNextState = uiSPLASH; // wf8421 to exit this event
                }
            }
            #endif
        //-----------------------------------------------------------
        }
        break;
        case uiEDID_EXTERN_MANAGE:
        {
            if (keOUT1 == sKeyEvent.eKey ||
                    keOUT2 == sKeyEvent.eKey ||
                    keOUT3 == sKeyEvent.eKey ||
                    keOUT4 == sKeyEvent.eKey||
                    keOUT5 == sKeyEvent.eKey||
                    keOUT6 == sKeyEvent.eKey||
                    keOUT7 == sKeyEvent.eKey||
                    keOUT8 == sKeyEvent.eKey
               )
            {
                m_cEdidManageOutPort = sKeyEvent.eKey - keOUT1;
                m_bUpdateUI = TRUE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bLedFlashingEN = FALSE;
            }
            if (keIN1 == sKeyEvent.eKey ||
                    keIN2 == sKeyEvent.eKey ||
                    keIN3 == sKeyEvent.eKey ||
                    keIN4 == sKeyEvent.eKey||
                    keIN5 == sKeyEvent.eKey||
                    keIN6 == sKeyEvent.eKey||
                    keIN7 == sKeyEvent.eKey||
                    keIN8 == sKeyEvent.eKey
               )
            {
                m_cEdidManageInPort = sKeyEvent.eKey - keIN1;
                m_bUpdateUI = TRUE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bLedFlashingEN = FALSE;
            }
            if (keALL== sKeyEvent.eKey)
            {
                m_cEdidManageInPort = MAX_IN;
                m_bUpdateUI = TRUE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bLedFlashingEN = FALSE;
            }
            if (keENTER == sKeyEvent.eKey &&
                    0xff != m_cEdidManageOutPort &&
                    0xff != m_cEdidManageInPort&&
                    !m_bLedFlashingEN) // 闪烁下不对ENTER起作用
            {
                if(m_cEdidManageInPort == MAX_IN)
                {
                    for(i = 0; i< MAX_IN; i++)
                    {
                        appSystemInputPortEdidSet(i, (eEDIDSELECT)(m_cEdidManageOutPort + edsEDID_EXTERN1));
                    }
                }
                else
                {
                    appSystemInputPortEdidSet(m_cEdidManageInPort,(eEDIDSELECT)(m_cEdidManageOutPort+ edsEDID_EXTERN1));
                }
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // to Flashing
                m_bUpdateUI = TRUE;
                m_wLedToFlashing = 0;
                //m_cLedFlashCNT = 0;
                //m_eNextState = uiIDLE; // wf8421 to exit this event
            }
            if (keEDID == sKeyEvent.eKey )
            {
                // 重新开始
            }
        }
        break;
        case uiEDID_INTERN_MANAGE: // 内部EDID 管理
        {
            //select intEDID
            //     INPUT  OUT1,OUT2
            //EDID/3/1
            if (keIN1 == sKeyEvent.eKey ||
                    keIN2 == sKeyEvent.eKey ||
                    keIN3 == sKeyEvent.eKey ||
                    keIN4 == sKeyEvent.eKey||
                    keIN5 == sKeyEvent.eKey||
                    keIN6 == sKeyEvent.eKey||
                    keIN7 == sKeyEvent.eKey||
                    keIN8 == sKeyEvent.eKey
               )
            {
                m_cEdidManageInPort = sKeyEvent.eKey - keIN1;
                m_bUpdateUI = TRUE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bLedFlashingEN = FALSE;
            }
            if (keOUT1 == sKeyEvent.eKey ||
                    keOUT2 == sKeyEvent.eKey)
            {
                //Range 0 ~ 4
                if (keOUT1 == sKeyEvent.eKey)
                {
                    if (m_cEdidManageOutPort < edsEDID_EXTERN1 - edsEDID_INTERN1 - 1)
                    {
                        m_cEdidManageOutPort++;
                    }
                    else
                    {
                        m_cEdidManageOutPort = 0;
                    }
                    m_bLedFlashingEN = FALSE;
                }
                else
                {
                    if(m_cEdidManageOutPort == 0xff)
                    {
                        m_cEdidManageOutPort = edsEDID_EXTERN1 - edsEDID_INTERN1 - 1;
                    }
                    else if (m_cEdidManageOutPort > 0)
                    {
                        m_cEdidManageOutPort--;
                    }
                    else
                    {
                        m_cEdidManageOutPort = edsEDID_EXTERN1 - edsEDID_INTERN1 - 1;
                    }
                    m_bLedFlashingEN = FALSE;
                }
                m_bUpdateUI = TRUE;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
            }
            if (keENTER == sKeyEvent.eKey &&
                    0xff != m_cEdidManageOutPort &&
                    0xff != m_cEdidManageInPort&&
                    !m_bLedFlashingEN) // 闪烁下不对ENTER起作用
            {
                appSystemInputPortEdidSet(m_cEdidManageInPort, (eEDIDSELECT)(m_cEdidManageOutPort + edsEDID_INTERN1));
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
                //m_eNextState = uiIDLE; // wf8421 to exit this event
            }
        }
        break;
        case uiSYSTEM_INCONNECT_INFO:
            //dvLCDLineDraw(0, 0, "In      01 02 03 04");
            //dvLCDLineDraw(1, 0, "Connect N N N N");
            if (keIN3 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_OUT_RESOLUTION_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            if (keIN4 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_OUTCONNECT_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            break;
        case uiSYSTEM_OUTCONNECT_INFO:
            //dvLCDLineDraw(0, 0, "Out     01 02 03 04");
            //dvLCDLineDraw(1, 0, "Connect N  N  N  N");
            if (keIN3 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_INCONNECT_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            if (keIN4 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_SWITCH_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            break;
        case uiSYSTEM_SWITCH_INFO:
            //dvLCDLineDraw(0, 0, "Out  01 02 03 04");
            //dvLCDLineDraw(1, 0, "In   N  N  N  N");
            if (keIN3 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_OUTCONNECT_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            if (keIN4 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_IN_HDCP_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            break;
        case uiSYSTEM_IN_HDCP_INFO:
            //dvLCDLineDraw(0, 0, "In   01 02 03 04");
            //dvLCDLineDraw(1, 0, "Hdcp N  N  N  N");
            if (keIN3 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_SWITCH_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            if (keIN4 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_OUT_HDCP_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            break;
        case uiSYSTEM_OUT_HDCP_INFO:
            //dvLCDLineDraw(0, 0, "Out  01 02 03 04");
            //dvLCDLineDraw(1, 0, "Hdcp N  N  N  N");
            if (keIN3 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_IN_HDCP_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            if (keIN4 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_OUT_RESOLUTION_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            break;
        case uiSYSTEM_OUT_RESOLUTION_INFO:
            //dvLCDLineDraw(0, 0, "Resolution");
            //dvLCDLineDraw(1, 0, "Out 1 0000x0000");
            if (keIN3 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_OUT_HDCP_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            if (keIN4 == sKeyEvent.eKey)
            {
                m_eNextState = uiSYSTEM_INCONNECT_INFO;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            if (keIN1 == sKeyEvent.eKey)
            {
                if (m_cEdidManageOutPort > 0)
                {
                    m_cEdidManageOutPort--;
                }
                else
                {
                    m_cEdidManageOutPort = MAX_OUT - 1;
                }
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            if (keIN2 == sKeyEvent.eKey)
            {
                if (m_cEdidManageOutPort < MAX_OUT - 1)
                {
                    m_cEdidManageOutPort++;
                }
                else
                {
                    m_cEdidManageOutPort = 0;
                }
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                m_bUpdateUI = TRUE;
            }
            break;
        case uiIDLE:
            if (keENTER == sKeyEvent.eKey)
            {
                m_eNextState = uiSPLASH;
            }
            break;
        default:
            break;
    }
}
// ---------------------------------------------------------------------------
// Perform rendering for current UI state
//
// Params:
//  eState : UI state to render
//
// Return: none
// ---------------------------------------------------------------------------
//   it is not good ideal add KEY_EVENT param for this function
//
// --------------------------------------------------------------------------
static void UIStateRender(eUI_STATE eState,
                          KEY_EVENT sKeyEvent)
{
    BYTE i = 0;
    BYTE cDrawTXT[100] = { 0 };
    BYTE acUartMsg[100] = { 0 };
    BYTE cCount = 0;
    BYTE cCount2 = 0;
    //Event_t   bEvent;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    POUTPUTPORT_MANAGER_DATA  psOutputPortMgrData;
    memset(cDrawTXT,0,20);
    memset(acUartMsg,0,40);
    switch (eState)
    {
    case uiSPLASH:
    {
        if (keENTER == sKeyEvent.eKey)
        {
            dvLedAllOffSet(0xFFFFFFFF);
            dvLedOnSet(1<<elnENTER);
        }
        if ( keCLEAR == sKeyEvent.eKey)
        {
            dvLedAllOffSet(0xFFFFFFFF);
            dvLedOnSet(1<<elnCLEAR);
        }
    }
    break;
    case uiSWITCH_VIDEO:
    case uiUART_SWITCH_VIDEO2:
    case uiUART_SWITCH_IR:
    {
        DWORD wLed = 0;
        BYTE acOutPort[100];
        BYTE j = 0;
        //闪烁完后，如果CLEAR 时间还没到，还会进入这里
        if (!m_bSwitchEnter) // 这里还不能自己用ENTER 按键来判断，
        {
            dvLedOffSet(1 << elnENTER);
            //cCount += SiiSprintf(&cDrawTXT[cCount], "AV:%2x->", m_cSwitchInputPort);
            for (i = 0; i < MAX_OUT; i++)
            {
                if (m_abSwitchOutPortSelect[i] == TRUE)
                {
                    wLed |= (1 << (elnOUT1 + i));
                    cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%2x,", i + 1);
                }
            }
            if (sKeyEvent.eKey == keALL)
            {
                dvLedOnSet(1 << elnALL);
            }
            if(uiSWITCH_VIDEO == eState)  //[kyq add: 20151026 17:13:55]
            {
                if (wLed > 0)
                {
                    cDrawTXT[cCount - 1] = 0;
                    dvLCDLineDraw(1, 0, cDrawTXT);
                    dvLedOnSet(wLed);
                    if(sKeyEvent.cType==HALUICTRL_TYPE_KEYPAD)
                    {
                        SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01);    //expect only one
                    }
                }
                else
                {
                    dvLedAllOnSet(1 << (elnIN1 + m_cSwitchInputPort));
                    dvLCDLineDraw(0, 0, (PBYTE)LCD1_IDLE_TEXT);
                    dvLCDLineTXTDraw(1, 0, "%2x", m_cSwitchInputPort + 1);
                    SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                }
            }
            // kyq 20150925
            if (uiUART_SWITCH_VIDEO2 == eState||
                    uiUART_SWITCH_IR == eState )
            {
                m_bSwitchEnter = TRUE; // UART COMMAND LET GO TO ENTER PROCESS
                m_bLedFlashingEN = FALSE;
                //[kyq Begin: 20151026 17:14:21]
                dvLedAllOnSet(1 << (elnIN1 + m_cSwitchInputPort));
                dvLedOnSet(wLed);
                //[kyq End: 20151026 17:14:30]
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // 总共闪烁时间3S
                appUIUpdateSet();
            }
        }
        // Enter key pressed
        else if (!m_bLedFlashingEN)
        {
            //m_cLedFlashCNT = 0;
            BOOL bSelectAll = FALSE;
            m_bLedFlashingEN = TRUE;
            m_cLedFlashCNT = 0;
            dvLedOnSet(1 << elnENTER);
            cCount = 0;
            j = 0;
            for (i = 0; i < MAX_OUT; i++)
            {
                if (m_abSwitchOutPortSelect[i] == TRUE)
                {
                    wLed |= (1 << (elnOUT1 + i));
                    j++;
                }
            }
            if (j >= MAX_OUT)
            {
                bSelectAll = TRUE;
            }
            if (!bSelectAll)
            {
                if (m_cSwitchInputPort>=MAX_OUT)
                {
                    if (uiUART_SWITCH_IR == eState)
                    {
                        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "IR:OFF->");
                    }
                    else
                    {
                        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "AV:OFF->");
                    }
                }
                else
                {
                    if (uiUART_SWITCH_IR == eState)
                    {
                        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "IR:%2x->",m_cSwitchInputPort + 1);
                    }
                    else
                    {
                        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "AV:%2x->", m_cSwitchInputPort + 1);
                    }
                }
                j = 0;
                for (i = 0; i < MAX_OUT; i++)
                {
                    if (m_abSwitchOutPortSelect[i] == TRUE)
                    {
                        //wLed |= (1 << (elnOUT1 + i));
                        j += SiiSprintf((char*)&acOutPort[j], "%2x,", i + 1); // 01,02,03,04,
                    }
                }
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%s", acOutPort);
                cDrawTXT[cCount - 1] = 0;
            }
            else
            {
                if(uiUART_SWITCH_IR == eState)
                {
                    if (m_cSwitchInputPort >= MAX_OUT)
                    {
                        //cCount += SiiSprintf(&cDrawTXT[cCount], "IR:OFF To All"); // IR not add . Gui maybe wrong accept
                        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "IR:OFF->All");
                    }
                    else
                    {
                        //cCount += SiiSprintf(&cDrawTXT[cCount], "IR:%2x%s", m_cSwitchInputPort + 1, " To All");
                        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "IR:%2x->All", m_cSwitchInputPort + 1);
                    }
                }
                else
                {
                    if (m_cSwitchInputPort >= MAX_OUT)
                    {
                        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "OFF To All.");
                    }
                    else
                    {
                        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%2x%s", m_cSwitchInputPort + 1, " To All.");
                    }
                }
            }
            if (wLed > 0)
            {
                //BOOL bUartReturnMessage_EN;
                dvLCDLineDraw(1, 0, cDrawTXT);
                //if (cCount <= 11) // 去掉最后一个","
                if (!bSelectAll)
                {
                    cCount--;
                }
                UART1_SendData2PC(cDrawTXT, strlen(cDrawTXT));
                dvLedOnSet(wLed);
                m_eNextState = uiSPLASH; // wf8421 to exit this event
            }
            else
            {
                dvLedAllOnSet(1 << (elnIN1 + m_cSwitchInputPort));
                dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
                dvLCDLineTXTDraw(1, 0, "%2x", m_cSwitchInputPort);
                //SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
            }
            SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // 总共闪烁时间3S
            // 马上开始闪烁
            SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF);
            m_wLedToFlashing = dvLedOnGet();
        }
        else
        {
            dvLedToggleSet(m_wLedToFlashing);
        }
    }
    break;
    case uiUART_SWITCH_VIDEO:
    {
        DWORD wLed = 0;
        if (m_bUartMessageSend)
        {
            if (m_cUartMessage1[0] != 0)
            {
                dvLCDLineDraw(0, 0, m_cUartMessage1);
            }
            else
            {
                dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
            }
            if (m_cUartMessage2[0] != 0)
            {
                dvLCDLineDraw(1, 0, m_cUartMessage2);
            }
            else
            {
                dvLCDLineDraw(1, 0, LCD2_IDLE_TEXT);
            }
            if (m_bUartMessageSend)
            {
                UART1_SendData2PC(m_cUartMessage2, strlen(m_cUartMessage2));
                //UART1_SendData2PC("\r\n",2);
            }
            m_bUartMessageSend = FALSE;
        }
        switch (m_eUartLedState)
        {
        case ulChannelSwitch:                              // switch one channel

#if 0
             /****************/
        //Event_t   bEvent;
        bEvent.Head.opcode=eLEDChannelSwitch;
        bEvent.Head.DestAddr=mqLED;
        bEvent.args[0]=m_cSwitchInputPort;
           for (i = 1; i < MAX_OUT+1; i++)
          bEvent.args[i]=m_abSwitchOutPortSelect[i-1] ; 
        bEvent.Head.argCount=sizeof(m_cSwitchInputPort)+MAX_OUT;
           utilExecOlnyMsgSend(mqLED,
                            bEvent);
            /****************/  
#else           
  //move LedReceiveEvent.c
            if (!m_bLedFlashingEN)
            {
                m_bLedFlashingEN = TRUE;
                m_cLedFlashCNT = 0;
                dvLedOffSet(1 << elnENTER);
                for (i = 0; i < MAX_OUT; i++)
                {
                    if (m_abSwitchOutPortSelect[i] == TRUE)
                    {
                        wLed |= (1 << (elnOUT1 + i));
                    }
                }
                dvLedAllOnSet(1 << (elnIN1 + m_cSwitchInputPort));
                dvLedOnSet(wLed);
                dvLedOnSet(1 << elnENTER);
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // 总共闪烁时间3S
                // 马上开始闪烁
                SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF);
                m_wLedToFlashing = dvLedOnGet();
            }
            else
            {
                dvLedToggleSet(m_wLedToFlashing);
            }
#endif
            break;
        case ulAllThrough:                            // all through  move LedReceiveEvent.c
   /*
            if (!m_bLedFlashingEN)
            {
                m_bLedFlashingEN = TRUE;
                m_cLedFlashCNT = 0;
                dvLedOffSet(1 << elnENTER);
                for (i = 0; i < MAX_OUT; i++)           //输入输出灯全亮
                {
                    wLed |= (1 << (elnOUT1 + i)) | (1 << (elnIN1 + i));
                }
                dvLedOnSet(wLed);
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // 总共闪烁时间3S
                // 马上开始闪烁
                SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF);
                m_wLedToFlashing = dvLedOnGet();
            }
            else
            {
                dvLedToggleSet(m_wLedToFlashing);
            }   */
            break;
         
        case ulAllClose:                              // all channel closed
        case ulAllOpen:                              // all channel open
            if (!m_bLedFlashingEN)
            {
                m_bLedFlashingEN = TRUE;
                m_cLedFlashCNT = 0;
                dvLedOffSet(1 << elnENTER);
                for (i = 0; i < MAX_OUT; i++)           //输出灯全亮
                {
                    wLed |= (1 << (elnOUT1 + i));
                }
                dvLedOnSet(wLed);
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // 总共闪烁时间3S
                SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF);
                m_wLedToFlashing = dvLedOnGet();
            }
            else
            {
                dvLedToggleSet(m_wLedToFlashing);
            }
            break;
        case   ulToAll:                                 // one channel to all channel
            if (!m_bLedFlashingEN)
            {
                m_bLedFlashingEN = TRUE;
                m_cLedFlashCNT = 0;
                dvLedOffSet(1 << elnENTER);
                for (i = 0; i < MAX_OUT; i++)           //输出灯全亮
                {
                    wLed |= (1 << (elnOUT1 + i));
                }
                dvLedAllOnSet(1 << (elnIN1 + m_cSwitchInputPort));       //亮对应的输入灯
                dvLedOnSet(wLed);
                dvLedOnSet(1 << elnENTER);
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // 总共闪烁时间3S
                // 马上开始闪烁
                SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF);
                m_wLedToFlashing = dvLedOnGet();
            }
            else
            {
                dvLedToggleSet(m_wLedToFlashing);
            }
            break;
        case  ulThrough:                              // through one channel
            if (!m_bLedFlashingEN)
            {
                m_bLedFlashingEN = TRUE;
                m_cLedFlashCNT = 0;
                dvLedOffSet(1 << elnENTER);
                for (i = 0; i < MAX_OUT; i++)
                {
                    if (m_abSwitchOutPortSelect[i] == TRUE)
                    {
                        wLed |= (1 << (elnOUT1 + i));
                    }
                }
                dvLedAllOnSet(1 << (elnIN1 + m_cSwitchInputPort));       //亮对应的输入灯
                dvLedOnSet(wLed);
                dvLedOnSet(1 << elnENTER);
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // 总共闪烁时间3S
                // 马上开始闪烁
                SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF);
                m_wLedToFlashing = dvLedOnGet();
            }
            else
            {
                dvLedToggleSet(m_wLedToFlashing);
            }
            break;
        case  ulClose:                                // close one channel
        case  ulOpen:
            if (!m_bLedFlashingEN)
            {
                m_bLedFlashingEN = TRUE;
                m_cLedFlashCNT = 0;
                dvLedOffSet(1 << elnENTER);
                for (i = 0; i < MAX_OUT; i++)
                {
                    if (m_abSwitchOutPortSelect[i] == TRUE)
                    {
                        wLed |= (1 << (elnOUT1 + i));
                    }
                }
                dvLedOnSet(wLed);
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // 总共闪烁时间3S
                // 马上开始闪烁
                SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF);
                m_wLedToFlashing = dvLedOnGet();
            }
            else
            {
                dvLedToggleSet(m_wLedToFlashing);
            }
            break;
        default :
            break;
        }
    }
    break;
    case uiSWITCH_INFO:
    {
        BYTE cInPort = 0;
        BYTE cIRInPort = 0;
        //POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
        cInPort = psOutputPortMgrData->acInPortSelect[m_cSwitcherInfoPort];
#ifdef PRD_IR_OUT
        cIRInPort = psOutputPortMgrData->acIRInPortSelect[m_cSwitcherInfoPort];
#endif
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                          FALSE);
        dvLedAllOnSet(1 << (elnOUT1 + m_cSwitcherInfoPort));
        if (cInPort >= MAX_IN)
        {
            cCount += SiiSprintf((char*)&cDrawTXT[cCount], "AV:%s->%2x", "OFF", m_cSwitcherInfoPort + 1);
        }
        else
        {
            cCount += SiiSprintf((char*)&cDrawTXT[cCount], "AV:%2x->%2x", cInPort + 1, m_cSwitcherInfoPort + 1);
            dvLedOnSet(1 << (elnIN1 + cInPort));
        }
#ifndef PRD_IR_OUT
        dvLCDLineDraw(1, 0, cDrawTXT);
        UART1_SendData2PC(cDrawTXT, cCount);
#endif
#ifdef PRD_IR_OUT
        dvLCDLineDraw(0, 0, cDrawTXT);
        UART1_SendData2PC(cDrawTXT, cCount);
        cCount = 0;
        memset(cDrawTXT,0,20);
        if (cIRInPort >= MAX_IN)
        {
            cCount += SiiSprintf((char*)&cDrawTXT[cCount], "IR:%s->%2x", "OFF", m_cSwitcherInfoPort + 1);
        }
        else
        {
            cCount += SiiSprintf((char*)&cDrawTXT[cCount], "IR:%2x->%2x", cIRInPort + 1, m_cSwitcherInfoPort + 1);
        }
        dvLCDLineDraw(1, 0, cDrawTXT);
        UART1_SendData2PC(cDrawTXT, cCount);
#endif
        SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
    }
    break;
    case uiEDID_EXTERN_MANAGE: //
    {
        if (!m_bLedFlashingEN)
        {
            // EDIDM1B2
            dvLedOnSet(1 << (elnEDID)); // 闪烁状态下按IN,OUT 键, 在刚好闪烁黑的情况下需要重新亮
            dvLedOffSet(1 << (elnENTER)); // 闪烁状态下按IN,OUT 键, 没有按下ENTER 键不亮
            cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%s", "EDIDM");
            if (m_cEdidManageOutPort != 0xff)
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%xB", m_cEdidManageOutPort + 1);
                              dvLedOffSet((1 << elnOUT1) | (1 << elnOUT2) | (1 << elnOUT3) | (1 << elnOUT4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif
                          );
                dvLedOnSet(1 << (elnOUT1 + m_cEdidManageOutPort));
                //m_wLedToFlashing |= 1 << (elnOUT1 + m_cEdidManageOutPort);
            }
            else
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%cB", ' ');
            }
            if (m_cEdidManageInPort != 0xff)
            {
                if (m_cEdidManageInPort == MAX_IN)
                {
                    cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%s", "ALL");
                    //dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4));
             //       dvLedOnSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4) | (1 << elnALL));
                dvLedOnSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif
                     | (1 << elnALL));
                    //dvLedOnSet(1 << elnALL);
                    //m_wLedToFlashing |= 1 << (elnIN1 + m_cEdidManageInPort);
                }
                else
                {
                    cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%x", m_cEdidManageInPort + 1);
                         dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif

                    | (1 << elnALL)
 );
               //     dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4) );
                    dvLedOnSet(1 << (elnIN1 + m_cEdidManageInPort));
                    //m_wLedToFlashing |= 1 << (elnIN1 + m_cEdidManageInPort);
                }
            }
            else
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%c", ' ');
            }
            if (keENTER == sKeyEvent.eKey)
            {
                dvLedOnSet(1 << (elnENTER));
                //m_wLedToFlashing |= (1 << (elnENTER));
                m_bLedFlashingEN = TRUE;
                m_cLedFlashCNT = 0;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
                SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF);
                m_wLedToFlashing = dvLedOnGet();
                UART1_SendData2PC(cDrawTXT, cCount);
                m_eNextState = uiSPLASH; // wf8421 to exit this event
            }
            dvLCDLineDraw(1, 0, cDrawTXT);
        }
        else
        {
            //m_cLedFlashCNT++;
            dvLedToggleSet(m_wLedToFlashing);
        }
    }
    break;
    case uiEDID_INTERN_MANAGE:  // 内部EDID 显示 // 0K
    {
        if (!m_bLedFlashingEN)
        {
            //select intEDID
            //EDID/3/1
            dvLedOnSet(1 << (elnEDID)); // 闪烁状态下按IN,OUT 键, 在刚好闪烁黑的情况下需要重新亮
            dvLedOffSet(1 << (elnENTER));// 闪烁状态下按IN,OUT 键, 没有按下ENTER 键不亮
            cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%s", "EDID/");
            if (m_cEdidManageInPort != 0xff)
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%x", m_cEdidManageInPort + 1);
                dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
                    
                #if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
                #endif

                #if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
                #endif

                          );
                dvLedOnSet(1 << (elnIN1 + m_cEdidManageInPort));
            }
            else
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%c", ' ');
            }
            if (m_cEdidManageOutPort != 0xff)
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "/%x",  m_cEdidManageOutPort + 1);
            }
            else
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "/%c", ' ');
            }
            //*
            //在本状态中闪烁灭灯时候按下IN1~IN4,再按ENTER, output 灯不亮, 如果需要亮,要增加变量记录
            if (keOUT1 == sKeyEvent.eKey)
            {
                dvLedOffSet((1 << elnOUT1) | (1 << elnOUT2) | (1 << elnOUT3) | (1 << elnOUT4)
                #if (MAX_OUT>4)
               |(1 << elnOUT5) |(1 << elnOUT6) 
                #endif

                #if(MAX_OUT>6)
                  |(1 << elnOUT7)|(1 << elnOUT8) 
                #endif
                );
                dvLedOnSet(1 << (elnOUT1));
            }
            else if (keOUT2 == sKeyEvent.eKey)
            {
                dvLedOffSet((1 << elnOUT1) | (1 << elnOUT2) | (1 << elnOUT3) | (1 << elnOUT4)
                #if (MAX_OUT>4)
               |(1 << elnOUT5) |(1 << elnOUT6) 
                #endif

                #if(MAX_OUT>6)
                  |(1 << elnOUT7)|(1 << elnOUT8) 
                #endif
                );
                dvLedOnSet(1 << (elnOUT2));
            }
            //*/
            else if (keENTER == sKeyEvent.eKey)
            {
                dvLedOnSet(1 << (elnENTER));
                m_bLedFlashingEN = TRUE; // 开始闪烁
                m_cLedFlashCNT = 0;
                SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF); // delay 0ms 开始
                SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF); // delay 0ms 开始
                m_wLedToFlashing = dvLedOnGet();
                UART1_SendData2PC(cDrawTXT, cCount);
                m_eNextState = uiSPLASH; // wf8421 to exit this event
            }
            dvLCDLineDraw(1, 0, cDrawTXT);
        }
        else
        {
            // 闪烁3次
            //m_cLedFlashCNT++;
            dvLedToggleSet(m_wLedToFlashing);
        }
    }
    break;
    case uiSYSTEM_INCONNECT_INFO:
    case uiUART_INCONNECT_INFO:
        //dvLCDLineDraw(0, 0, "In      01 02 03 04");
        //dvLCDLineDraw(1, 0, "Connect N N N N");
        SiiSprintf((char*)m_cUartMessage1, "In   01 02 03 04");
        SiiSprintf((char*)m_cUartMessage2, "Connect  N N N N");
        cCount=9;
        for (i = 0; i < 4; i++)
        {
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", dvSiiDrvAdaptRxConnectGet(i) ? 'Y' : 'N');
        }
        if (uiUART_INCONNECT_INFO == eState)
        {
            //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
         //   UART1_SendData2PC(m_cUartMessage1, cCount);
	   UART1_SendData2PC(m_cUartMessage1, strlen(m_cUartMessage1));
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
#if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
        SiiSprintf((char*)m_cUartMessage1, "In   05 06 07 08");
        SiiSprintf((char*)m_cUartMessage2, "Connect  N N N N");
#else
        SiiSprintf((char*)m_cUartMessage1, "In   05 06");
        SiiSprintf((char*)m_cUartMessage2, "Connect  N N");
#endif
        cCount=9;
        for (i =4; i < MAX_IN; i++)
        {
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", dvSiiDrvAdaptRxConnectGet(i) ? 'Y' : 'N');
        }
        if (uiUART_INCONNECT_INFO == eState)
        {
            //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
            UART1_SendData2PC(m_cUartMessage1, strlen(m_cUartMessage1));
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
#endif
        if (sKeyEvent.eKey >= keIN3 &&
                sKeyEvent.eKey <= keIN4)
        {
                              dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif
 );
            dvLedOnSet(1 << (elnIN1 + (sKeyEvent.eKey - keIN1)));
        }
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
        break;
    case uiSYSTEM_OUTCONNECT_INFO:
    case uiUART_OUTCONNECT_INFO:
    {
        //dvLCDLineDraw(0, 0, "Out     01 02 03 04");
        //dvLCDLineDraw(1, 0, "Connect N  N  N  N");
        BYTE cSii9533Instance;
        txStatus_t txStat;
        //BOOL bHPD;
        SiiSprintf((char*)m_cUartMessage1, "Out  01 02 03 04");
        SiiSprintf((char*)m_cUartMessage2, "Connect  N N N N");
        cCount=9;
        for (i = 0; i < 4; i++)
        {
            cSii9533Instance = utilOutputPortToInstance(i);
            txStat = SiiTxStatusGet(cSii9533Instance);
            //bHPD = SiiTxStatusGet(cSii9533Instance).isHotPlugDetected; //[kyq add: 20150728 15:38:23]
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", (txStat.isHotPlugDetected) ? 'Y' : 'N');
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
        if (uiUART_OUTCONNECT_INFO == eState)
        {
            //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
              UART1_SendData2PC(m_cUartMessage1, strlen(m_cUartMessage1));
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
#if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
        SiiSprintf((char*)m_cUartMessage1, "Out  05 06 07 08");
        SiiSprintf((char*)m_cUartMessage2, "Connect  N N N N");
#else
        SiiSprintf((char*)m_cUartMessage1, "Out  05 06");
        SiiSprintf((char*)m_cUartMessage2, "Connect  N N");
#endif
        cCount=9;
        for (i = 4; i < MAX_OUT; i++)
        {
            cSii9533Instance = utilOutputPortToInstance(i);
            txStat = SiiTxStatusGet(cSii9533Instance);
            //bHPD = SiiTxStatusGet(cSii9533Instance).isHotPlugDetected; //[kyq add: 20150728 15:38:23]
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", (txStat.isHotPlugDetected) ? 'Y' : 'N');
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
        if (uiUART_OUTCONNECT_INFO == eState)
        {
            //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
            UART1_SendData2PC(m_cUartMessage1, strlen(m_cUartMessage1));
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
#endif
        if (sKeyEvent.eKey >= keIN3 &&
                sKeyEvent.eKey <= keIN4)
        {
                dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif
                          );
            dvLedOnSet(1 << (elnIN1 + (sKeyEvent.eKey - keIN1)));
        }
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
    }
    break;
    case uiSYSTEM_SWITCH_INFO:
    case uiUART_SWITCH_INFO:
    {
        //dvLCDLineDraw(0, 0, "Out  01 02 03 04");
        //dvLCDLineDraw(1, 0, "In   N  N  N  N");
        BYTE cInPort = 0;
        POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
        SiiSprintf((char*)m_cUartMessage1, "Out  01 02 03 04");
        SiiSprintf((char*)m_cUartMessage2, "In   01 01 01 01");
        cCount=4;
        for (i = 0; i < 4; i++)
        {
            cInPort = psOutputPortMgrData->acInPortSelect[i];
            if (cInPort >= MAX_IN)
            {
                cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], " %s", "00");
            }
            else
            {
                cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], " %2d", cInPort + 1);
            }
        }
        if (uiUART_SWITCH_INFO == eState)
        {
            // cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
            UART1_SendData2PC(m_cUartMessage1, cCount);
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
#if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
        SiiSprintf((char*)m_cUartMessage1, "Out  05 06 07 08");
        SiiSprintf((char*)m_cUartMessage2, "In   01 01 01 01");
#else
        SiiSprintf((char*)m_cUartMessage1, "Out  05 06");
        SiiSprintf((char*)m_cUartMessage2, "In   01 01");
#endif
        cCount=4;
        for (i = 4; i < MAX_OUT; i++)
        {
            cInPort = psOutputPortMgrData->acInPortSelect[i];
            if (cInPort >= MAX_IN)
            {
                cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], " %s", "00");
            }
            else
            {
                cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], " %2d", cInPort + 1);
            }
        }
        if (uiUART_SWITCH_INFO == eState)
        {
            // cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
            UART1_SendData2PC(m_cUartMessage1, cCount);
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
#endif
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,FALSE);
        if (sKeyEvent.eKey >= keIN3 &&
                sKeyEvent.eKey <= keIN4)
        {
                dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif
                          );
            dvLedOnSet(1 << (elnIN1 + (sKeyEvent.eKey - keIN1)));
        }
        SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,0xFF);
    }
    break;
    case uiSYSTEM_IN_HDCP_INFO:
    case uiUART_IN_HDCP_INFO:
        //dvLCDLineDraw(0, 0, "In   01 02 03 04");
        //dvLCDLineDraw(1, 0, "Hdcp N  N  N  N");
        SiiSprintf((char*)m_cUartMessage1, "In   01 02 03 04");
        SiiSprintf((char*)m_cUartMessage2, "HDCP  N  N  N  N");
        cCount=4;
        for (i = 0; i <4; i++)
        {
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", dvSiiDrvAdaptRxHDCPGet(i) ? 'Y' : 'N');
        }
        //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
        if (uiUART_IN_HDCP_INFO == eState)
        {
            UART1_SendData2PC(m_cUartMessage1, cCount);
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
#if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
        SiiSprintf((char*)m_cUartMessage1, "In   05 06 07 08");
        SiiSprintf((char*)m_cUartMessage2, "HDCP  N  N  N  N");
#else
        SiiSprintf((char*)m_cUartMessage1, "In   05 06");
        SiiSprintf((char*)m_cUartMessage2, "HDCP  N  N");
#endif
        cCount=4;
        for (i = 4; i < MAX_IN; i++)
        {
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", dvSiiDrvAdaptRxHDCPGet(i) ? 'Y' : 'N');
        }
        //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
        if (uiUART_IN_HDCP_INFO == eState)
        {
            UART1_SendData2PC(m_cUartMessage1, cCount);
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
#endif
        if (sKeyEvent.eKey >= keIN3 &&
                sKeyEvent.eKey <= keIN4)
        {
                dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif
        );
            dvLedOnSet(1 << (elnIN1 + (sKeyEvent.eKey - keIN1)));
        }
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
        break;
    case uiUART_IN_HDCPENABLE_INFO:
    {
        PINPUTPORT_MANAGER_DATA       psInputPortMgrData;
        psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnINPUTPORT_MAMAGER_DATA);
        SiiSprintf((char*)m_cUartMessage1, "In   01 02 03 04");
        SiiSprintf((char*)m_cUartMessage2, "HDCPEN  Y Y Y Y ");
        cCount = 8;
        for (i = 0; i < 4; i++)
        {
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
        if (uiUART_IN_HDCPENABLE_INFO == eState)
        {
            //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
            UART1_SendData2PC(m_cUartMessage1, cCount);
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
#if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
        SiiSprintf((char*)m_cUartMessage1, "In   05 06 07 08");
        SiiSprintf((char*)m_cUartMessage2, "HDCPEN  Y Y Y Y ");
#else
        SiiSprintf((char*)m_cUartMessage1, "In   05 06");
        SiiSprintf((char*)m_cUartMessage2, "HDCPEN  Y Y");
#endif
        cCount = 8;
        for (i = 4; i < MAX_IN; i++)
        {
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
        if (uiUART_IN_HDCPENABLE_INFO == eState)
        {
            //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
            UART1_SendData2PC(m_cUartMessage1, cCount);
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
#endif
        if (sKeyEvent.eKey >= keIN3 &&
                sKeyEvent.eKey <= keIN4)
        {
                dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif
                          );
            dvLedOnSet(1 <<(elnIN1 +(sKeyEvent.eKey - keIN1)));
        }
        utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA,FALSE);
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
    }
    break;
    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    case uiUART_OUT_LINK_INFO:
        {
            //dvLCDLineDraw(0, 0, "Out  01 02 03 04");
            //dvLCDLineDraw(1, 0, "Hdcp N  N  N  N");
            WORD OutLinkStatus;
            SiiSprintf((char*)m_cUartMessage1, "Out  01 02 03 04");
            SiiSprintf((char*)m_cUartMessage2, "LINK  N  N  N  N");
            cCount = 4;
            #if (MACHINETYPE == MUH4K862)
            if(dvPC9539Read(epi9539_LINK_POECTRL,inport1,(uint8_t*)&OutLinkStatus)!=rcSUCCESS)
            {
                
            }
            #else
            if(dvPC9539Read(epi9539_9679RST,inport1,(uint8_t*)&OutLinkStatus)!=rcSUCCESS)
            {
                
            }
            #endif
            //Uart_printf("OutLinkStatus==0x%4x\r\n",OutLinkStatus);
            if((OutLinkStatus&0xFF)!=(preLinkstatus&0xFF))
            {
                preLinkstatus = OutLinkStatus;
            }
            for (i = 0; i < 4; i++)
            {
                #if (MACHINETYPE == MHUB4K431)
                cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", (OutLinkStatus&(1<<i)||(i==3)) ? 'N' : 'Y');
                #else
                cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", OutLinkStatus&(1<<i) ? 'N' : 'Y');
                #endif
            }
            dvLCDLineDraw(0, 0, m_cUartMessage1);
            dvLCDLineDraw(1, 0, m_cUartMessage2);
            if (uiUART_OUT_LINK_INFO == eState)
            {
                UART1_SendData2PC(m_cUartMessage1, cCount);
                UART1_SendData2PC(m_cUartMessage2, cCount);
            }
        #if(MAX_OUT>4)  //8 channel
            #if(MAX_OUT>6)
            SiiSprintf((char*)m_cUartMessage1, "Out  05 06 07 08");
            SiiSprintf((char*)m_cUartMessage2, "LINK  Y  Y  Y  Y");
            #else
            SiiSprintf((char*)m_cUartMessage1, "Out  05 06");
            SiiSprintf((char*)m_cUartMessage2, "LINK  Y  Y");
            #endif
            cCount = 4;
            for (i = 0; i < 4; i++)
            {
                #if (MACHINETYPE == MUH4K862)
                cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", ((OutLinkStatus&(1<<(i+4)))||((i+4)==7||(i+4)==8)) ? 'N' : 'Y');
                #else
                cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", OutLinkStatus&(1<<(i+4)) ? 'N' : 'Y');
                #endif
            }
            dvLCDLineDraw(0, 0, m_cUartMessage1);
            dvLCDLineDraw(1, 0, m_cUartMessage2);
            //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
            if (uiUART_OUT_LINK_INFO == eState)
            {
                UART1_SendData2PC(m_cUartMessage1, cCount);
                UART1_SendData2PC(m_cUartMessage2, cCount);
            }
        #endif
            #if 0
            if (sKeyEvent.eKey >= keIN3 &&
                    sKeyEvent.eKey <= keIN4)
            {
                 dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
                #if (MAX_IN>4)
                   |(1 << elnIN5) |(1 << elnIN6) 
                #endif
                #if(MAX_IN>6)
                      |(1 << elnIN7)|(1 << elnIN8) 
                #endif
                    );
                dvLedOnSet(1 <<(elnIN1 +(sKeyEvent.eKey - keIN1)));
            }
            #endif
            SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
            SiiPlatformTimerSet(etLINK_SCAN,mS_TO_TICKS(5000L),0xFF); // use systick: 1ms per tick
        }
        break;
    #endif
    case uiSYSTEM_OUT_HDCP_INFO:
    case uiUART_OUT_HDCP_INFO:
    {
        //dvLCDLineDraw(0, 0, "Out  01 02 03 04");
        //dvLCDLineDraw(1, 0, "Hdcp N  N  N  N");
        BOOL bOutHDCPEN;
        SiiSprintf((char*)m_cUartMessage1, "Out  01 02 03 04");
        SiiSprintf((char*)m_cUartMessage2, "HDCP  N  N  N  N");
        cCount = 4;
        for (i = 0; i < 4; i++)
        {
            bOutHDCPEN = dvSii9533TXHDCPGet(utilOutputPortToInstance(i));
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", bOutHDCPEN ? 'Y' : 'N');
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
        //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
        if (uiUART_OUT_HDCP_INFO == eState)
        {
            UART1_SendData2PC(m_cUartMessage1, cCount);
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
#if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
        SiiSprintf((char*)m_cUartMessage1, "Out  05 06 07 08");
        SiiSprintf((char*)m_cUartMessage2, "HDCPEN  Y Y Y Y ");
#else
        SiiSprintf((char*)m_cUartMessage1, "Out  05 06");
        SiiSprintf((char*)m_cUartMessage2, "HDCPEN  Y Y");
#endif
        cCount = 4;
        for (i =4; i < MAX_OUT; i++)
        {
            bOutHDCPEN = dvSii9533TXHDCPGet(utilOutputPortToInstance(i));
            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", bOutHDCPEN ? 'Y' : 'N');
        }
        dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
        //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
        if (uiUART_OUT_HDCP_INFO == eState)
        {
            UART1_SendData2PC(m_cUartMessage1, cCount);
            UART1_SendData2PC(m_cUartMessage2, cCount);
        }
#endif
        if (sKeyEvent.eKey >= keIN3 &&
                sKeyEvent.eKey <= keIN4)
        {
                         dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif
 );
            dvLedOnSet(1 <<(elnIN1 +(sKeyEvent.eKey - keIN1)));
        }
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
    }
    break;
    case uiSYSTEM_OUT_RESOLUTION_INFO:
    case uiUART_OUT_RESOLUTION_INFO:
    {
        BYTE cSii9533Instance;
        INT nHRes = 0;
        INT nVRes = 0;
        INT nCLK = 0;
        bool_t bInterlace;
        SiiSprintf((char*)m_cUartMessage1, "Resolution");
        SiiSprintf((char*)m_cUartMessage2, "Out 1  0000x0000");
        if (uiUART_OUT_RESOLUTION_INFO == eState)
        {
            UART1_SendData2PC(m_cUartMessage1, strlen(m_cUartMessage1));
            for (i = 0; i < MAX_IN; i++)
            {
                cCount=4;
                cSii9533Instance = utilOutputPortToInstance(i);
                dvSii9533RXResolutionGet(cSii9533Instance, &nHRes, &nVRes, &nCLK,&bInterlace);
                if (nVRes>100)
                {
                    if(nHRes>999)
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%d %4dx%d%c", i + 1, nHRes, nVRes, bInterlace ? 'i' : 'p');
                    else
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%d  %3dx%d%c", i + 1, nHRes, nVRes, bInterlace ? 'i' : 'p');
                }
                else
                {
                    cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%d 0000x0000", i + 1);
                }
                if(i==0)
                {
                    dvLCDLineDraw(1, 0, m_cUartMessage2);       //lcd上显示
                }
                UART1_SendData2PC(m_cUartMessage2, cCount);
                //UART1_SendData2PC("\r\n",2);
            }
            appUIStateChange(uiSYSTEM_OUT_RESOLUTION_INFO);
            SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
        }
        else
        {
            cSii9533Instance = utilOutputPortToInstance(m_cEdidManageOutPort);
            dvSii9533RXResolutionGet(cSii9533Instance, &nHRes, &nVRes, &nCLK,&bInterlace);
            // outport resolution
            cCount=4;
            if(nVRes>100)
            {
                if (m_bCharFlashing)
                {
                    if(nHRes>999)
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c %4dx%d%c", ' ', nHRes, nVRes,bInterlace?'i':'p');
                    else
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c  %3dx%d%c", ' ', nHRes, nVRes,bInterlace?'i':'p');    
                    m_bCharFlashing = FALSE;
                }
                else
                {
                    if(nHRes>999)
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%d %4dx%d%c", m_cEdidManageOutPort + 1, nHRes, nVRes,bInterlace?'i':'p');//[kyq edid: 20151026 17:35:20]
                    else
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%d  %3dx%d%c", m_cEdidManageOutPort + 1, nHRes, nVRes,bInterlace?'i':'p');//[kyq edid: 20151026 17:35:20]        
                    m_bCharFlashing = TRUE;
                }
            }
            else
            {
                if (m_bCharFlashing)
                {
                    cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  0000x0000");
                    m_bCharFlashing = FALSE;
                }
                else
                {
                    cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%d 0000x0000", m_cEdidManageOutPort + 1);//[kyq edid: 20151026 17:35:20]
                    m_bCharFlashing = TRUE;
                }
            }
            dvLCDLineDraw(1, 0, m_cUartMessage2);
            if (sKeyEvent.eKey >= keIN1 &&
                    sKeyEvent.eKey <= keIN4)
            {
                 dvLedOffSet((1 << elnIN1) | (1 << elnIN2) | (1 << elnIN3) | (1 << elnIN4)
#if (MAX_IN>4)
               |(1 << elnIN5) |(1 << elnIN6) 
#endif

#if(MAX_IN>6)
                  |(1 << elnIN7)|(1 << elnIN8) 
#endif
                          );
                dvLedOnSet(1 << (elnIN1 + (sKeyEvent.eKey - keIN1)));
            }
            /*
            else if (keENTER == sKeyEvent.eKey)
            {
                dvLedOnSet(1 << (elnENTER));
            }
            */
        }
        SiiPlatformTimerSet(etUI_FLASHING, mS_TO_TICKS(500L),0xFF);
    }
    break;
    case uiUART_MESSAGE:
        if (m_cUartMessage1[0] != 0)
        {
            dvLCDLineDraw(0, 0, m_cUartMessage1);
        }
        else
        {
            dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
        }
        if (m_cUartMessage2[0] != 0)
        {
            dvLCDLineDraw(1, 0, m_cUartMessage2);
        }
        else
        {
            dvLCDLineDraw(1, 0, LCD2_IDLE_TEXT);
        }
        if (m_bUartMessageSend)
        {
            UART1_SendData2PC(m_cUartMessage2, strlen(m_cUartMessage2));
            //UART1_SendData2PC("\r\n",2);
        }
        m_bUartMessageSend = FALSE;
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
        break;
    case uiSYSTEM_LOCK_INFO:
    {
        BOOL bLocked;
        //PSYSTEM_SETTING_DATA    psSystemSettingData;
        psSystemSettingData  = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        bLocked = psSystemSettingData->bSystemLock;
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                          FALSE);
        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%s",
                             bLocked ?  "System Locked!" : "System UnLock!");
        dvLCDLineDraw(1, 0, cDrawTXT);
        // cCount += SiiSprintf(&cDrawTXT[cCount], "%s", "\r\n");
        UART1_SendData2PC(cDrawTXT, cCount);
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
    }
    break;
    case uiSYSTEM_NAME:
    {
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        dvLCDLineDraw(1, 0, psSystemSettingData->acProduct_Name);
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                          FALSE);
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
    }
    break;
    case uiSYSTEM_VERSION:
        dvLCDLineDraw(1, 0, g_FW_Version);
        UART1_SendData2PC(g_FW_Version, strlen(g_FW_Version));
        //UART1_SendData2PC("\r\n",2);
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
        break;
    case uiSYSTEM_MESSAGE_ONOFF_INFO:
        //dvLCDLineDraw(1, 0, "/:MessageOff;");
        //dvLCDLineDraw(1, 0, "/:MessageOn;");
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%s",
                             psSystemSettingData->bUartReturnMessage_EN ?
                             "/:MessageOn;" : "/:MessageOff;");
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                          FALSE);
        dvLCDLineDraw(1, 0, cDrawTXT);
        //cCount += SiiSprintf(&cDrawTXT[cCount], "%s", "\r\n");
        UART1_SendData2PC(cDrawTXT, cCount);
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
        break;
    case uiUART_SWITCH_PORT_INFO:       //status指令查询单个输入输出对应的反馈
    {
        BYTE cInPort = 0;
#if 0 //def PRD_IR_OUT
        BYTE cIRInPort = 0;
        BYTE cCnt = 0;
        BYTE acUartMsg2[40] =
        { 0 };
#endif
        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
        if (m_bOutPortSwitcherInf < MAX_OUT)
        {
            cInPort = psOutputPortMgrData->acInPortSelect[m_bOutPortSwitcherInf];
            if (cInPort >= MAX_IN)
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "AV:%s->%2d",
                                     "OFF", m_bOutPortSwitcherInf+ 1);
            }
            else
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "AV:%2d->%2d",
                                     cInPort + 1, m_bOutPortSwitcherInf + 1);
            }
#ifndef PRD_IR_OUT
            dvLCDLineDraw(1, 0, cDrawTXT);
            //cCount2 += SiiSprintf(&acUartMsg[cCount2], "%s", "\r\n");
#endif
            UART1_SendData2PC(cDrawTXT, cCount);
#ifdef PRD_IR_OUT
            dvLCDLineDraw(0, 0, cDrawTXT);
            cCount = 0;
            memset(cDrawTXT, 0, 20);
            cInPort = psOutputPortMgrData->acIRInPortSelect[m_bOutPortSwitcherInf];
            if (cInPort >= MAX_IN)
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "IR:%s->%2d",
                                     "OFF", m_bOutPortSwitcherInf + 1);
            }
            else
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], "IR:%2d->%2d",
                                     cInPort + 1, m_bOutPortSwitcherInf + 1);
            }
            dvLCDLineDraw(1, 0, cDrawTXT);
            UART1_SendData2PC(cDrawTXT, cCount);
#endif
        }

        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                          FALSE);
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
    }
    break;
    case uiUART_SWITCH_ALL_PORT_INFO:                   //status指令查询所有输入输出对应的反馈
    {
        //dvLCDLineDraw(0, 0, "Out  01 02 03 04");
        //dvLCDLineDraw(1, 0, "In   N  N  N  N");
        BYTE cInPort = 0;
#ifdef PRD_IR_OUT
        BYTE cIRInPort = 0;
        BYTE cCnt = 0;
        BYTE acUartMsg2[40] =
        { 0 };
#endif
        POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
        for (i = 0; i < MAX_OUT; i++)
        {
            cCount2=0;
            cCount=0;
#ifdef PRD_IR_OUT
            cCnt=0;
#endif
            cInPort = psOutputPortMgrData->acInPortSelect[i];
            if (cInPort >= MAX_IN)
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], " %s", "00");
                cCount2 += SiiSprintf((char*)&acUartMsg[cCount2], "AV:%s->%2d",
                                      "OFF", i + 1);
            }
            else
            {
                cCount += SiiSprintf((char*)&cDrawTXT[cCount], " %2d", cInPort + 1);
                cCount2 += SiiSprintf((char*)&acUartMsg[cCount2], "AV:%2d->%2d",
                                      cInPort + 1, i + 1);
            }
#ifdef PRD_IR_OUT
            cIRInPort = psOutputPortMgrData->acIRInPortSelect[i];
            if (cIRInPort >= MAX_IN)
            {
                cCnt += SiiSprintf((char*)&acUartMsg2[cCnt], "IR:%s->%2d\r\n",
                                   "OFF", i + 1);
            }
            else
            {
                cCnt += SiiSprintf((char*)&acUartMsg2[cCnt], "IR:%2d->%2d\r\n",
                                   cIRInPort+ 1, i + 1);
            }
#endif
            UART1_SendData2PC(acUartMsg, cCount2);
#ifdef PRD_IR_OUT
            UART1_SendData2PC(acUartMsg2, cCnt);
#endif
            dvLCDLineDraw(1, 4, cDrawTXT);
        }
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                          FALSE);
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
    }
    break;
    case uiUART_OUT_HDMIAUDIO_INFO:
        SiiSprintf((char*)m_cUartMessage1, "Out  01 02 03 04\r\n");
        SiiSprintf((char*)m_cUartMessage2, "Audio  Y  Y  Y  Y\r\n");
        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
        cCount = 5;

    for(i=0;i<4;i++)
        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c",
                             psOutputPortMgrData->abHDMIAudio_EN[i] ? 'Y' : 'N'
                        
                            );
       dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
        //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
        UART1_SendData2PC(m_cUartMessage1, cCount);
        UART1_SendData2PC(m_cUartMessage2, cCount);

    #if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
                SiiSprintf((char*)m_cUartMessage1, "Out  05 06 07 08\r\n");
        SiiSprintf(m_cUartMessage2, "Audio  Y  Y  Y  Y\r\n");
#else
               SiiSprintf((char*)m_cUartMessage1, "Out  05 06\r\n");
        SiiSprintf((char*)m_cUartMessage2, "Audio  Y  Y\r\n");
#endif

    //    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
        cCount = 5;
    for(i=4;i<MAX_OUT;i++)
        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c",
                             psOutputPortMgrData->abHDMIAudio_EN[i] ? 'Y' : 'N'
                        
                            );
     
       dvLCDLineDraw(0, 0, m_cUartMessage1);
        dvLCDLineDraw(1, 0, m_cUartMessage2);
        //cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
        UART1_SendData2PC(m_cUartMessage1, cCount);
        UART1_SendData2PC(m_cUartMessage2, cCount);
#endif

        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, FALSE);
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
        break;
    case uiUART_IP_INFO:
        psSystemSettingData  = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        cCount += SiiSprintf((char*)&cDrawTXT[cCount], "%d.%d.%d.%d",
                             psSystemSettingData->acIP_Address[0],
                             psSystemSettingData->acIP_Address[1],
                             psSystemSettingData->acIP_Address[2],
                             psSystemSettingData->acIP_Address[3]
                            );
        cCount2 += SiiSprintf((char*)acUartMsg, "IP:%d.%d.%d.%d",
                              psSystemSettingData->acIP_Address[0],
                              psSystemSettingData->acIP_Address[1],
                              psSystemSettingData->acIP_Address[2],
                              psSystemSettingData->acIP_Address[3]
                             );
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                          FALSE);
        dvLCDLineDraw(0, 0, "IP:");
        dvLCDLineDraw(1, 0, cDrawTXT);
        UART1_SendData2PC(acUartMsg, cCount2);
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
        break;
    case uiUART_SYSTEM_TYPE:
        psSystemSettingData  = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        dvLCDLineDraw(1, 0, psSystemSettingData->acProduct_Name);
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                          FALSE);
        UART1_SendData2PC(psSystemSettingData->acProduct_Name, strlen(psSystemSettingData->acProduct_Name));
        SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(DEFAULT_UI_CLEAR),0xFF);
        break;
    default:
        break;
    }
}
// ---------------------------------------------------------------------------
// Perform processing for exiting a UI state
//
// Params:
//  eState : UI state being exited
//
// Return: none
// ---------------------------------------------------------------------------
// it is not good ideal add KEY_EVENT param for this function
//
static void UIStateExit(eUI_STATE eState,
                        KEY_EVENT sKeyEvent)
{
    BYTE i;
    // 各自状态用到的变量退出时候看是否清除
    switch (eState)
    {
        /*
        uiSPLASH,
        uiSWITCH_VIDEO,
        uiSWITCH_INFO,
        uiEDID_EXTERN_MANAGE,
        uiEDID_INTERN_MANAGE,





        uiTEST,
        */
    case uiSPLASH:
        dvLCDLineDraw(0, 0, " ");
        dvLCDLineDraw(1, 0, " ");
        dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
        break;
    case uiSWITCH_VIDEO:
        dvLCDLineDraw(0, 0, " ");
        dvLCDLineDraw(1, 0, " ");
        dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
        for (i = 0; i < MAX_OUT; i++)
        {
            m_abSwitchOutPortSelect[i] = FALSE;
        }
        m_bSwitchEnter = FALSE;
        m_bLedFlashingEN = FALSE;
        m_cLedFlashCNT = 0;
        break;
    case uiSWITCH_INFO:
        dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
        break;
    case uiEDID_EXTERN_MANAGE:
        dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
        m_cEdidManageInPort = 0xff;
        m_cEdidManageOutPort = 0xff;
        break;
    case uiEDID_INTERN_MANAGE:
        dvLedAllOffSet(0xFFFFFFFF);// all led off, except power led
        m_cEdidManageInPort = 0xff;
        m_cEdidManageOutPort = 0xff;
        break;
    case  uiSYSTEM_INCONNECT_INFO:    // 输入是否接信号
    case  uiUART_INCONNECT_INFO:
    case  uiSYSTEM_OUTCONNECT_INFO: // 输出是否接信号
    case  uiUART_OUTCONNECT_INFO:
    case  uiSYSTEM_SWITCH_INFO:          // 所有输入和输出的对应关系
    case  uiUART_SWITCH_INFO:             //  所有端口的切换关系
    case  uiSYSTEM_IN_HDCP_INFO:
    case  uiUART_IN_HDCP_INFO:
    case  uiSYSTEM_OUT_HDCP_INFO:
    case  uiUART_OUT_HDCP_INFO:
    case  uiSYSTEM_OUT_RESOLUTION_INFO:
    case  uiUART_OUT_RESOLUTION_INFO:
    case  uiUART_MESSAGE:
        // uiSYSTEM_INFO,
    case  uiSYSTEM_LOCK_INFO:
        //uiSYSTEM_UNLOCK,
    case  uiSYSTEM_NAME:
    case  uiSYSTEM_VERSION:
    case  uiSYSTEM_MESSAGE_ONOFF_INFO:
    case  uiUART_SWITCH_PORT_INFO:   // 串口查询端口切换关系
    case  uiUART_OUT_HDMIAUDIO_INFO:
    case  uiUART_IN_HDCPENABLE_INFO:    // 用户设定HDCP 使能状态
    case  uiUART_IP_INFO:
    case  uiUART_SYSTEM_TYPE:
    case uiIDLE:
        dvLCDLineDraw(0, 0, " ");  //[kyq add: 20150907 20:51:01]
        dvLCDLineDraw(1, 0, " ");
    default:
        break;
    }
    m_wLedToFlashing = 0;
    m_bLedFlashingEN = FALSE;
    m_bUIFlashingEN = FALSE;
    dvLedAllOffSet(0xFFFFFFFF);// all led off, except power led
}
//#pragma argsused
// ---------------------------------------------------------------------------
// This API is accessed by the Data Manager component to allow initialization
// and resetting of this component抯 persistent data node by handling external
// events triggered by the persistent data storage.
//
// Params:
//  eNodeID : Node identifier that is saved internally to access this node
//  eEvent  : Identifier of the external event. Event can be  nveRESETALL or
//            nveINITIALIZE
//
// Return: none
// ---------------------------------------------------------------------------
void appUINodeCallback(eNODE_NAME eNodeID,
                       eNV_EVENT  eEvent)
{
}
#if 0
//----------------------------------------------------------------------------
// Enable or disable the UI
//
// Params:
//  bEnable  : TRUE to Enable the UI, FALSE to disable
//
// Return: <none>
//----------------------------------------------------------------------------
void appUIEnable(BOOL bEnable)
{
    /* -------------- BEGIN CUSTOM AREA [090] appUserInterface.c -------------- */
    /* ---------------------------- END CUSTOM AREA --------------------------- */
    m_bUIEnabled = bEnable;
}
#endif
// ---------------------------------------------------------------------------
// Return TURE if current UI state is IDLE
//
// Params: none
//
// Return: TRUE if current UI state is IDLE
//
// Notes:
// ---------------------------------------------------------------------------
BOOL appUIStateIsIdle(void)
{
    return (m_eState == uiIDLE);
}
// ---------------------------------------------------------------------------
// Change the state of the user interface to the specified value.
//
// Params:
//  eState : state to transition to
//
// Return: none
//
// Notes:
//  This API must ONLY be called from within the User Interface component,
//  and only from appUIxxxEvent routines.
// ---------------------------------------------------------------------------
void appUIStateChange(eUI_STATE eState)
{
    m_eNextState = eState;
    m_bUpdateUI = TRUE;
}
// some caller is wrong parame
void appUartSwitchLedStateChange(BYTE IN, BYTE OUT, eUartLed_STATE State)
{
    BYTE i;
    for (i = 0; i < MAX_OUT; i++)
    {
        m_abSwitchOutPortSelect[i] = FALSE;
    }
    m_eUartLedState= State;
    m_cSwitchInputPort=IN-1;
    m_abSwitchOutPortSelect[OUT-1] = TRUE;
    m_bLedFlashingEN = FALSE; // add by kyq 20150929
}
//  IN -  输入口, begin 0
//  cPortCount: 输出端口数,从 1 开始
//  acPortList:   输出端口号 从 0开始
//  bAVorIR: 1: 视频和IR 切换? 0: IR 切换
//
void appUartSwitchInfoSet(BYTE cInPort, BYTE cPortCount,PBYTE acOutPortList,BOOL bAVorIR)
{
    BYTE i;
    for (i = 0; i < MAX_OUT; i++)
    {
        m_abSwitchOutPortSelect[i] = FALSE;
    }
    //m_eUartLedState= State;
    m_cSwitchInputPort = cInPort;
    for (i = 0; i < cPortCount; i++)
    {
        m_abSwitchOutPortSelect[acOutPortList[i]] = TRUE;
    }
    //m_abSwitchOutPortSelect[OUT-1] = TRUE;
    m_bSwitchEnter = FALSE; // KYQ 20151026
    m_bLedFlashingEN = FALSE; //[kyq ADD: 20151026 17:02:11]
    if (bAVorIR)
    {
        appUIStateChange(uiUART_SWITCH_VIDEO2);
    }
    else
    {
        appUIStateChange(uiUART_SWITCH_IR);
    }
}

//  IN -  输入口, begin 0
//  cPortCount: 输出端口数,从 1 开始
//  acPortList:   输出端口号 从 0开始
//
void appUartSwitchSatusSet(BYTE cInPort, BYTE cPortCount,PBYTE acOutPortList)
{
    BYTE i;
    for (i = 0; i < MAX_OUT; i++)
    {
        m_abSwitchOutPortSelect[i] = FALSE;
    }
    //m_eUartLedState= State;
    m_cSwitchInputPort = cInPort;
    for (i = 0; i < cPortCount; i++)
    {
        m_abSwitchOutPortSelect[acOutPortList[i]] = TRUE;
    }
    //m_abSwitchOutPortSelect[OUT-1] = TRUE;
    m_bSwitchEnter = FALSE; // KYQ 20151026
    m_bLedFlashingEN = FALSE; //[kyq ADD: 20151026 17:02:11]
    appUIStateChange(uiSPLASH);
}
// mars add---------------------------------------------------------------------------
// Get the state of the user interface.
//
// Params: none
//
// Return: eState : state to transition to
// ---------------------------------------------------------------------------
eUI_STATE appUIStateGet(void)
{
    return m_eState;
}
// ---------------------------------------------------------------------------
// Set a flag indicating that the user interface graphics need to be updated
//
// Params: none
//
// Return: none
//
// Notes:
//  This API must ONLY be called from within the User Interface component,
//  and only from appUIxxxEvent routines.
// ---------------------------------------------------------------------------
void appUIUpdateSet(void)
{
    m_bUpdateUI = TRUE;
}
// ---------------------------------------------------------------------------
// Enable or disable the UI
//
// Params:
//  bEnable : TRUE to enable the UI (reset the current state), FALSE to disable
//
// Return: none
//
// Notes:
// ---------------------------------------------------------------------------
void appUIEnable(BOOL bEnable)
{
    KEY_EVENT sKeyEvent;
    sKeyEvent.eKey = keNONE;
    // If disabling, exit the current state
    if (!bEnable)
    {
        UIStateExit(m_eState, sKeyEvent);
    }
    // Otherwise, re-enter the current state and flag the UI to be updated
    else
    {
        UIStateEnter(m_eState, sKeyEvent);
        m_bUpdateUI = TRUE;
    }
}
void appUIExit(void)
{
    KEY_EVENT sKeyEvent;
    sKeyEvent.eKey = keNONE;
    // If disabling, exit the current state
    //if (!bEnable)
    {
        UIStateExit(m_eState, sKeyEvent);
        m_eState = uiIDLE;
    }
}
eUI_STATE appUINextStateGet(void)
{
    return m_eNextState;
}

// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************
// ---------------------------------------------------------------------------
// This API sets up the internal state of this component.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void appUIInitialize(void)
{
    //ePOWERSTATE         eLastBootState;
    //BYTE                cLogoSel = 0;
    PSYSTEM_SETTING_DATA  psSystemSettingData;
    BOOL  bIRKeyEn;
    // initialize user interface state
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    bIRKeyEn = !psSystemSettingData->bSystemLock;
    memcpy(LCD1_IDLE_TEXT, psSystemSettingData->acLCDLine1DefaultTXT, 17);
    memcpy(LCD2_IDLE_TEXT, psSystemSettingData->acLCDLine2DefaultTXT, 17);
// kyq 20151201
    memcpy(m_acProduct_Type,psSystemSettingData->acProduct_Type, 17);
    memcpy(m_acProduct_Name,psSystemSettingData->acProduct_Name, 17);
 //   memcpy(m_acProduct_Name,psSystemSettingData->acProduct_Name,strlen( psSystemSettingData->acProduct_Name));
    memcpy(u8IChannellock,psSystemSettingData->acIChannellock, MAX_OUT);
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
    m_eState = uiIDLE;
    m_bUpdateUI = FALSE;
    //m_FAN_ERR_Timer = 0;
    /* -------------- BEGIN CUSTOM AREA [060] appUserInterface.c -------------- */
    // Set initial state transition
    // See how we got here from the boot code and only display the splash
    // screen if this was a cold start.
#if 0
    if(pwrSTANDBY == appPowerMgrLastPowerStateGet()||
            pwrSOFTOFF ==  appPowerMgrLastPowerStateGet())
    {
        m_eNextState = uiSYSTEM_VERSION;
    }
    else
#endif
    {
        m_eNextState = uiSPLASH;
    }
    memset(m_cUartMessage1, 0, LCD_MAX_CHAR);
    memset(m_cUartMessage2, 0, LCD_MAX_CHAR);
    // to init LCD1_IDLE_TEXT,LCD2_IDLE_TEXT from NVRAM
    // TODO
    System_UnLock_flag = !bIRKeyEn;
    appUIIRKeyEnableSet(bIRKeyEn);
    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    InitLinkStatue();
    #endif
    /* ---------------------------- END CUSTOM AREA --------------------------- */
}
// ---------------------------------------------------------------------------
// This API performs periodic processing for the user interface.  It checks
// for user input (e.g. keypad or infrared) and updates the state of the user
// interface appropriately.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void appUIProcess(void)
{
    KEY_EVENT sKeyEvent;
    BOOL      bStateChanged = FALSE;
    // check for state change on timeout
    ProcessStateChangeOnTimeout();
    /* ---------------------------- END CUSTOM AREA --------------------------- */
    // check if user input control has a new keypress
    if (utilExecMsgPresent(mqUSER_INPUT))
    {
        utilExecMsgRead(mqUSER_INPUT,
                        (BYTE *)&sKeyEvent);
        //DEBUG_PRINT(MSG_ERR, "***USER_INPUT = %d\r\n",  sKeyEvent.eKey);
        /* Add code here to perform a post read operation on the user input queue */
        utilExecMsgClear(mqUSER_INPUT);
        /* ---------------------------- END CUSTOM AREA --------------------------- */
    }
    else
    {
        sKeyEvent.eKey = keNONE;
    }
    // 当前状态下按键处理
    if (keNONE != sKeyEvent.eKey)
    {
        UIStateEvent(m_eState,
                     sKeyEvent);
        //SiiPlatformTimerSet(etUI_CLEAR, DEFAULT_UI_CLEAR,,0xFF); // test only
        // any key press will exit DEMO mode
        appSystemSwitchDemoModeEnableSet(FALSE);
        //    m_bUpdateUI = TRUE;
    }
    bStateChanged = (m_eNextState != m_eState);
    /* -------------- BEGIN CUSTOM AREA [077] appUserInterface.c -----------------
     * Add code here to force a state change if required even though the event
     * handler didn't change the state. */
    /* ---------------------------- END CUSTOM AREA --------------------------- */
    if (bStateChanged)
    {
        // execute exit code for current state
        UIStateExit(m_eState, sKeyEvent);
        // execute entry code for next state
        UIStateEnter(m_eNextState, sKeyEvent);
        /* -------------- BEGIN CUSTOM AREA [078] appUserInterface.c -----------------
         * Add code here to force a state change if required even though the event
         * handler didn't change the state. */
        /* ---------------------------- END CUSTOM AREA --------------------------- */
        // update state information and set flag to redraw graphics
        m_eState = m_eNextState;
        m_bUpdateUI = TRUE;
    }
    // if the user interface graphics need to be updated,
    if (m_bUpdateUI)
    {
        m_bUpdateUI = FALSE;
        // call the rendering routine for the current state
        UIStateRender(m_eState, sKeyEvent);
    }
    
    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    QueryLinkStatue();
    #endif
    /* -------------- BEGIN CUSTOM AREA [079] appUserInterface.c -------------- */
    /* ---------------------------- END CUSTOM AREA --------------------------- */
}
void appUIIRKeyEnableSet(BOOL bEnable)
{
 /****************/
        Event_t   bEvent;
        bEvent.Head.opcode=eKeyLock;
        bEvent.Head.DestAddr=mqKEY;
        bEvent.args[0]=bEnable;
        bEvent.Head.argCount=1;
           utilExecOlnyMsgSend(mqKEY,
                            bEvent);
/****************/  
/****************/
        bEvent.Head.opcode=eIRLock;
        bEvent.Head.DestAddr=mqIR;
        bEvent.args[0]=bEnable;
        bEvent.Head.argCount=1;
           utilExecOlnyMsgSend(mqIR,
                            bEvent);
/****************/  
    //halUICtrlKeyEnalbe(bEnable);  //msg send
    //halUICtrlIRKeyEnalbe(bEnable);  //msg send
}
// 把串口命令中响应后的LCD 显示设置进来
void appUIUartMessageSet(PBYTE pcLine1Message, PBYTE pcLine2Message, BOOL bSendMessage)
{
    assert_param(strlen(pcLine1Message) <= 16);
    assert_param(strlen(pcLine2Message) <= 16);
    memset(m_cUartMessage2, 0, LCD_MAX_CHAR);
    memset(m_cUartMessage1, 0, LCD_MAX_CHAR);
    if (pcLine1Message != NULL)
    {
        memcpy(m_cUartMessage1, pcLine1Message, (strlen(pcLine1Message)>LCD_MAX_CHAR-1)?(LCD_MAX_CHAR-1):strlen(pcLine1Message));
    }
    if (pcLine2Message != NULL)
    {
        memcpy(m_cUartMessage2, pcLine2Message, (strlen(pcLine2Message)>LCD_MAX_CHAR-1)?(LCD_MAX_CHAR-1):strlen(pcLine2Message));
    }
    m_bUartMessageSend = bSendMessage;
}
// 显示某个端口的切换信息
// MAX_OUT为显示所有输出端口信息
void appUIUartMessageSwitcherPortSet(BYTE cOutPort)
{
    m_bOutPortSwitcherInf = cOutPort;
    if (m_bOutPortSwitcherInf < MAX_OUT)
    {
        appUIStateChange(uiUART_SWITCH_PORT_INFO);    // 显示单个端口
    }
    else
    {
        appUIStateChange(uiUART_SWITCH_ALL_PORT_INFO);    // 显示所有端口
    }
}
// DEMO 时候设置显示状态
void appUIDemoSwitcherInfoSet(BYTE cOutPort)
{
    m_cSwitcherInfoPort = cOutPort;
    m_eNextState = uiSWITCH_INFO;
    m_bUpdateUI = TRUE;
}

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
#define LINK_POLL_PERIOD    (5000L)
void InitLinkStatue(void)
{
    WORD OutLinkStatus=0;
    #if (MACHINETYPE == MUH4K862)
    if(dvPC9539Read(epi9539_LINK_POECTRL,inport1,(uint8_t*)&OutLinkStatus)!=rcSUCCESS)
    {
        return;
    }
    #else
    if(dvPC9539Read(epi9539_9679RST,inport1,(uint8_t*)&OutLinkStatus)!=rcSUCCESS)
    {
        return;
    }
    #endif
    if(OutLinkStatus&0xFF!=preLinkstatus&0xFF)
    {
        preLinkstatus = OutLinkStatus;
        //appUIStateChange(uiUART_OUT_LINK_INFO);
    }
    
    #if (/*(MACHINETYPE == MUH4K862)||*/(MACHINETYPE==MHUB4K431))
    SiiPlatformTimerSet(etLINK_SCAN,mS_TO_TICKS(LINK_POLL_PERIOD*12),0xFF ); // use systick: 1ms per tick
    #else
    SiiPlatformTimerSet(etLINK_SCAN,mS_TO_TICKS(LINK_POLL_PERIOD*6),0xFF ); // use systick: 1ms per tick
    #endif
}
 
void QueryLinkStatue(void)
{
    WORD OutLinkStatus=0;
    
    if(!SiiPlatformTimerExpired(etLINK_SCAN))
    {
        return; // time not enougth
    }
    
    #if (MACHINETYPE == MUH4K862)
    if(dvPC9539Read(epi9539_LINK_POECTRL,inport1,(uint8_t*)&OutLinkStatus)!=rcSUCCESS)
    {
        return;
    }
    #else
    if(dvPC9539Read(epi9539_9679RST,inport1,(uint8_t*)&OutLinkStatus)!=rcSUCCESS)
    {
        return;
    }
    #endif
    if((OutLinkStatus&0xFF)!=(preLinkstatus&0xFF))
    {
        preLinkstatus = OutLinkStatus;
        appUIStateChange(uiUART_OUT_LINK_INFO);
    }
    
    SiiPlatformTimerSet(etLINK_SCAN,mS_TO_TICKS(LINK_POLL_PERIOD),0xFF); // use systick: 1ms per tick
}
#endif

