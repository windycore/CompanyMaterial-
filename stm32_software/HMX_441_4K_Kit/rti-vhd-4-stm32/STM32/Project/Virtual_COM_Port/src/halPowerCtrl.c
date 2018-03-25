//
// $RCSfile: halPowerCtrl.c,v $
// $Revision: 1.0 $
// $Author:  $
// $Date:  20150805 9:57:45]
//
#include "sk_avr_repeater.h"
#include "halPowerCtrlAPI.h"
#include "dvPC9539.h"
#include "appPowerMgrAPI.h"
#include "appSystemMgrAPI.h"
#include <string.h>
#include "dvCPLD.h"
#include "dvVS010TX.h"
#include "bsp_Gpio.h"
#include "KeyTasks.h"
#include "uart.h"
#include "dvLcd.h"
#include "main.h"
#include "si_app_adapter_rx.h"
#include "IRTasks.h"
#include "SwitchVideoHandle.h"

// The last boot power state.
//static ePOWERSTATE      m_eLastBootPowerState;
// BOOL TRUE if the last suspend exit was due to user input.
//static BOOL             m_bUserLastSuspendExit;
extern uint8_t  USART1_RX_BUFF[];
extern vu8      *USART1_rxpoint;  
extern BOOL   g_bUSART1_DataComplete;
static BYTE standby_flag =0;
// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
//#pragma warn -use
#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
extern void Set_PreLinkStatus(WORD u16Value);
extern WORD Get_PreLinkStatus(void);
#endif

#if 0
// ---------------------------------------------------------------------------
// This API waits for the external power to stabilize. We use this when coming
// out of a low power state when we have just turned on the switched power.
//
// Params:
//  wDelay : Delay time in uS
//
// Return: none
// ---------------------------------------------------------------------------
static void PowerCtrlPowerUpDelay(WORD wDelay)
{
}
#endif
#if ((MACHINETYPE == MUH4K862)|(MACHINETYPE == MHUB4K431))
extern BYTE cmdlist[][13];
extern int Get_Cmdlist_Len(void);
#endif
extern void SetStartupFlag(bool_t tbOn);

// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************
// ---------------------------------------------------------------------------
// This API sets up the internal state of component..
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void halPowerCtrlInitialize(void)
{
    // Call this one time to reat the last boot state from the temporary
    // register it was saved in.
    //m_eLastBootPowerState = dvPowerCtrlGetLastBootPowerState();
    // Grab the reason we last exited the low power suspend loop.
    //m_bUserLastSuspendExit = dvPowerCtrlGetLastSuspendExit();
}
//#pragma argsused
// ---------------------------------------------------------------------------
// This API is used to prepare the driver layer for a power state change.
// Initially this API has just a single call to the chip driver when preparing
// to enter the ultra low power mode. Upon returning from that call
// dvPowerctrl will be called and it will not return to this API. Only a valid
// wake event will bring the system back to the pwrON power state through the
// boot code.
//
// Params:
//  cRouteId       : Route identifier - for future use.
//  eNewPowerState : The power state to change to..
//
// Return: none
// ---------------------------------------------------------------------------
void halPowerCtrlPowerSet(ePOWERSTATE eNewPowerState)
{
    KEY_EVENT sKeyEvent;
    unsigned char *UartStr;
    BOOL  bExit = FALSE;
    BYTE  i;
    BYTE  cCount;
//------------------------------------------
    //PPOWER_MANAGER_DATA psPowerMgrData;
    PSYSTEM_SETTING_DATA         psSystemSettingData;
    //unsigned char *pp;
    //BYTE IP_add[4];
    //BYTE acBraudRate[MAX_OUT];
    //unsigned int jiucuo; 
    //BYTE m_cUartMessage1[LCD_MAX_CHAR];
    //BYTE m_cUartMessage2[LCD_MAX_CHAR];
    BYTE m_cUartMessage1[100];
    BYTE m_cUartMessage2[100];
    BYTE cInstance = 0;
    BYTE cSii9679LoopCNT = 0;
    BYTE cSii9533 = 0;
    
    //for(i = 0; i< MAX_OUT; i++)
    //{
    //   acBraudRate[i] = psSystemSettingData->acRemoteBaudRate[i];
    //}
    //utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
    
    switch (eNewPowerState)
    {
/* ---------------- BEGIN CUSTOM AREA [020] halPowerCtrl.c ---------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
    case pwrON:
        // We are transitioning from a low power state where we had turned
        // off the external power. Turn it back on here so the rest of the
        // initialization can complete.
        dvCpldClose();
        dvVS010TXPowerOn(ON);
        //power_LED(PON);
        //set_clear_standby_flag(0);
        dvLedOnSet(1 << elnGREEN);
        dvLedOffSet(1 << elnRED);
        BSP_GPIO_FanPower_EN(TRUE);
        BSP_GPIO_AP3012_EN(TRUE);
		appSystemSwitcherOutportForceRecoverAll();//// Video and IR  huang
        // here to send uart message for power on
        //BL_ON;
        // Before we let the system wake up see how we got here.
        //psPowerMgrData->eLastPwrState;
        delay_nms(200); // 500
        dvCpldOpen();
        //delay_nms(200); // 500
        #if 1
        InitRemoteCtrlMcu();
        #endif
        //psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        #if !((MACHINETYPE == MHUB4K431)||(MACHINETYPE == MUH4K862))
        for (i = 0; i < MAX_OUT; i++)
        {
            /*dvCpldSendDataToRemote(i+1,
                                     acBraudRate[i],
                                     NULL,
                                     0);
                */
            //psSystemSettingData->acRemoteBaudRate[i] = 3;
            dvCpldRemoteBraundSet(i + 1);
        }
        #endif
        //utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
        break;
    case pwrSTANDBY:
        dvLedOnSet(1 << elnRED);
        dvLedOffSet(1 << elnGREEN);
        dvLCD_On(FALSE);
        BSP_GPIO_FanPower_EN(FALSE);
        BSP_GPIO_AP3012_EN(FALSE);
        dvVS010TXPowerOn(ON);
        appSystemSwitcherOutportForceCloseAll();
        //set_clear_standby_flag(1);
		// here to send uart message for power standby
        SetStartupFlag(TRUE);
        while (TRUE)
        {
            // IR, KEY, UART can week up
            IWDG_ReloadCounter();
            halUICtrlEventHandler();
            //  halIRKeyProcess();
            AppTaskMsgLoop();
        #if 1
    		dvSii9533Process(cSii9533);
    	   	cSii9533++;
    	   	if (cSii9533 == DevNum)
    	   	{
    		   	cSii9533 = 0;
    	   	}
    		if (cSii9679LoopCNT++ > 3)
    		{
    			for (cInstance = 0; cInstance < INSTANCE_9679; cInstance++)
    			{
    				dvSiiDrvAdaptTaskExecute(cInstance);
    			}
    			cSii9679LoopCNT = 0;
    		}
        #endif
            if (utilExecMsgPresent(mqUSER_INPUT))			//有消息要处理
            {
                utilExecMsgRead(mqUSER_INPUT,
                                (BYTE *)&sKeyEvent);
                utilExecMsgClear(mqUSER_INPUT);
                if (sKeyEvent.eKey != keNONE)
                {
                    if(sKeyEvent.eKey != keStandBy)
                    {
                        bExit = TRUE;
                        //set_clear_standby_flag(1);
                        Set_Tm1623ValueChangeFlag(true);
                    }
                    else
                    {
                        bExit = TRUE;
                        sKeyEvent.eKey = keNONE;
                        set_clear_standby_flag(1);
                        Set_Tm1623ValueChangeFlag(true);
                    }
                    //APP_Printf("  bExit = TRUE; Key wake up!\n");
                }
            }

    /***************************************************************************************************************/
    			
            if (g_bUSART1_DataComplete)
            {
                memset(m_cUartMessage1, 0, LCD_MAX_CHAR);
                memset(m_cUartMessage2, 0, LCD_MAX_CHAR);
                cCount = 0;
                //jiucuo = 0;
                UartStr = USART1_RX_BUFF;
                
                // here some bug
                while (UartStr < USART1_rxpoint)
                {
                    if (UartStr == 0)
                        UartStr++;
                    else
                        break;
                }
                //UartStr = USART1_RX_BUFF;
                // here some bug
            #ifdef PRD_VHD4_CMD_END
                if (StrCmp2(UartStr, "%9962.~", (unsigned char *)USART1_rxpoint))
            #else
                if (StrCmp2(UartStr, "%9962.", (unsigned char *)USART1_rxpoint))
            #endif
                {
                    UART1_SendBytes("STANDBY\r\n", strlen("STANDBY\r\n"));
                }
//[kyq Begin: 20151014 15:49:44]
            #ifdef PRD_VHD4_CMD_END
                else if(StrCmp(UartStr, "%9975.~",  (unsigned char *)USART1_rxpoint))
            #else
                else if(StrCmp(UartStr, "%9975.",  (unsigned char *)USART1_rxpoint))
            #endif
                {
                    BYTE cInPort = 0;
                    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
                    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
                    SiiSprintf((char*)m_cUartMessage1, "Out  01 02 03 04");
                    SiiSprintf((char*)m_cUartMessage2, "In   01 01 01 01");
                    cCount = 4;
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
			        UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
            #if(MAX_OUT>4)  //8 channel
                #if(MAX_OUT>6)
                    SiiSprintf((char*)m_cUartMessage1, "Out  05 06 07 08");
                    SiiSprintf((char*)m_cUartMessage2, "In   01 01 01 01");
                #else
                    SiiSprintf((char*)m_cUartMessage1, "Out  05 06");
                    SiiSprintf((char*)m_cUartMessage2, "In   01 01");
                #endif
                    cCount = 4;
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
			        UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
            #endif
                    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, FALSE);
        
                    
                }
            #ifdef PRD_VHD4_CMD_END
               else if(StrCmp(UartStr, "%9978.~",  (unsigned char *)USART1_rxpoint))
            #else
                else if(StrCmp(UartStr, "%9978.",  (unsigned char *)USART1_rxpoint))
            #endif
                {
                    //查询输入端的HDCP兼容状态
                    PINPUTPORT_MANAGER_DATA       psInputPortMgrData;
                    psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnINPUTPORT_MAMAGER_DATA);
                    SiiSprintf((char*)m_cUartMessage1, "In   01 02 03 04");
        		SiiSprintf((char*)m_cUartMessage2, "HDCPEN  Y Y Y Y ");
                    cCount = 8;
                    for (i = 0; i < 4; i++)
                    {
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
                    }
		      		UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
            #if(MAX_IN>4)  //8 channel
                #if(MAX_IN>6)
			        SiiSprintf((char*)m_cUartMessage1, "In   05 06 07 08");
			        SiiSprintf((char*)m_cUartMessage2, "HDCPEN  Y Y Y Y ");
                #else
			        SiiSprintf((char*)m_cUartMessage1, "In   05 06");
			        SiiSprintf((char*)m_cUartMessage2, "HDCPEN  Y Y");
                #endif

                    cCount = 8;
                    for (i =4; i < MAX_IN; i++)
                    {
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
                    }
		      		UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
            #endif
                    utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA, FALSE);
             
                }
            #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
            #ifdef PRD_VHD4_CMD_END
               else if(StrCmp(UartStr, "%9979.~",  (unsigned char *)USART1_rxpoint))
            #else
                else if(StrCmp(UartStr, "%9979.",  (unsigned char *)USART1_rxpoint))
            #endif
                {
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
                    if((OutLinkStatus&0xFF)!=(Get_PreLinkStatus()&0xFF))
                    {
                        //preLinkstatus = OutLinkStatus;
                        Set_PreLinkStatus(OutLinkStatus);
                    }
                    for (i = 0; i < 4; i++)
                    {
                        #if (MACHINETYPE == MHUB4K431)
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", (OutLinkStatus&(1<<i)||(i==3)) ? 'N' : 'Y');
                        #else
                        cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", OutLinkStatus&(1<<i) ? 'N' : 'Y');
                        #endif
                    }
                    UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
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
                    UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
                #endif
                    SiiPlatformTimerSet(etLINK_SCAN,mS_TO_TICKS(5000L),0xFF); // use systick: 1ms per tick
                }
            #endif
            #ifdef PRD_VHD4_CMD_END
                else if (StrCmp(UartStr, "/^Version;~", (unsigned char *)USART1_rxpoint) != NULL) // Software Version
            #else
                else if (StrCmp(UartStr, "/^Version;", (unsigned char *)USART1_rxpoint) != NULL) // Software Version
            #endif
                {
                    UART1_SendData2PC(g_FW_Version, strlen(g_FW_Version));
                }
            #ifdef PRD_VHD4_CMD_END
                else if (StrCmp(UartStr, "%9961.~", (unsigned char *)USART1_rxpoint))
            #else
                else if (StrCmp(UartStr, "%9961.", (unsigned char *)USART1_rxpoint))
            #endif
                {
                    BOOL bLocked;
                    //PSYSTEM_SETTING_DATA    psSystemSettingData;
                    psSystemSettingData  = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
                    bLocked = psSystemSettingData->bSystemLock;
                    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                                      FALSE);
                    cCount += SiiSprintf((char*)m_cUartMessage1, "%s",
                                         bLocked ?	"System Locked!" : "System UnLock!");
                    // cCount += SiiSprintf(&cDrawTXT[cCount], "%s", "\r\n");
                    UART1_SendData2PC(m_cUartMessage1, cCount);
                }
                //////////////////////////////////////////////////////////////////////////////
                else if (*UartStr == 0xf3)
                {
                    BYTE IP_add[4];
                    if (get_ip_add(UartStr, IP_add))
                    {
                        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                        //ip_set_ok = 1;
                        for (i = 0; i < 4; i++) //i2c_EE_write_one_byte(sys_IP_add + i, IP_add[i]);
                        {
                            psSystemSettingData->acIP_Address[i] = IP_add[i];
                        }
                        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
                        utilDataMgrUpdate(UPDATE_ALL_NODEID);
                    }
                }
//[kyq End: 20151014 15:49:46]
            #ifdef PRD_VHD4_CMD_END
                else if (StrCmp2(UartStr, ".~", (unsigned char *)USART1_rxpoint))
            #else
                else if (StrCmp2(UartStr, ".", (unsigned char *)USART1_rxpoint))
            #endif
                {
                    bExit = TRUE;
                    
                    //APP_Printf("  bExit = TRUE; Wake up by command :%s!\n",UartStr);
                }
                g_bUSART1_DataComplete = FALSE; //uart_data_in_end = 0;
                USART1_rxpoint = USART1_RX_BUFF;
            }//end if (g_bUSART1_DataComplete)
            if (bExit)
            {
            /*                         
                psPowerMgrData = (PPOWER_MANAGER_DATA)utilDataMgrWriteLockData(nnPOWER_MGR_DATA);
                     // if code run here it's power on	
                psPowerMgrData->eLastPwrState =pwrON;
                utilDataMgrUnlock(nnPOWER_MGR_DATA, TRUE);
                utilDataMgrUpdate(nnPOWER_MGR_DATA);
                //NVIC_SystemReset();
            */
                #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                {
                    BYTE CloseLed[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
                    InOutPortLed_Process(CloseLed,1);
                }
                Set_Tm1623ValueChangeFlag(true);
                SetUartDelayReceiveForPoweronFunction(15000);
                #endif
                break;
            }
        }
        break;
        //case pwrSUSPEND:
        //    break;
    case pwrSOFTOFF:
        dvLedOnSet(1 << elnRED);
        dvLedOffSet(1 << elnGREEN);
        dvLCD_On(FALSE);
        BSP_GPIO_FanPower_EN(FALSE);
        BSP_GPIO_AP3012_EN(FALSE);
	    dvCpldClose();
        dvVS010TXPowerOn(OFF);
        //set_clear_standby_flag(1);
        appSystemSwitcherOutportForceCloseAll ();// Video and IR
        // here to send uart message for power off
        SetStartupFlag(TRUE);
        while (TRUE)
        {
            IWDG_ReloadCounter();
            halUICtrlEventHandler();
            
            halIRKeyProcess();
            //AppTaskMsgLoop();
            #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
            {
                BYTE CloseLed[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
                InOutPortLed_Process(CloseLed,0);
            }
            //Set_Tm1623ValueChangeFlag(true);
            #endif
            #if 1
            dvSii9533Process(cSii9533);
            cSii9533++;
            if (cSii9533 == DevNum)
            {
                cSii9533 = 0;
            }
    		if (cSii9679LoopCNT++ > 3)
    		{
    			for (cInstance = 0; cInstance < INSTANCE_9679; cInstance++)
    			{
    				dvSiiDrvAdaptTaskExecute(cInstance);
    			}
    			cSii9679LoopCNT = 0;
    		}
            #endif
    	      if (utilExecMsgPresent(mqUSER_INPUT))			//有消息要处理
                {
                    utilExecMsgRead(mqUSER_INPUT,
                                    (BYTE *)&sKeyEvent);
                    utilExecMsgClear(mqUSER_INPUT);
                    if (sKeyEvent.eKey != keNONE)
                    {
        			  if(sKeyEvent.eKey == keInOut1_LONG)
        			  {
        				bExit = TRUE;
        			  }
                    }
                }
                if (g_bUSART1_DataComplete)
                {
                    memset(m_cUartMessage1, 0, LCD_MAX_CHAR);
                    memset(m_cUartMessage2, 0, LCD_MAX_CHAR);
                    cCount = 0;
                    UartStr = USART1_RX_BUFF;
                    // here some bug
                    while (UartStr < USART1_rxpoint)
                    {
                        if (UartStr == 0)
                            UartStr++;
                        else
                            break;
                    }
                    
                    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE == MHUB4K431))
                    if(*UartStr == 0xa5 && *(UartStr+1)==0x5b)
                    {
                        for(i = 0 ; i< Get_Cmdlist_Len(); i++)
                        {
                            if(*(UartStr+2)==0x08)
                            {
                                if(StrCmp(UartStr, cmdlist[i], (unsigned char *)USART1_rxpoint) != NULL)
                                {
                                    if(*(UartStr+4)==0xf0)
                                    {
                                    }
                                    else if(*(UartStr+4)==0x0f)
                                    {
                                        bExit = TRUE;
                                        // power on
                                    }
                                }
                            }
                        }
                        i = 0;
                    }
                    #endif

                    #ifdef PRD_VHD4_CMD_END
                    if (StrCmp2(UartStr, "PWON.~", (unsigned char *)USART1_rxpoint))
                    #else
                    if (StrCmp2(UartStr, "PWON.", (unsigned char *)USART1_rxpoint))
                    #endif
                    {
                        bExit = TRUE;
    		//     UART1_SendBytes("PWON\r\n", strlen("PWON\r\n"));
                    }
                    //查询
                    #ifdef PRD_VHD4_CMD_END
                    if (StrCmp2(UartStr, "%9962~", (unsigned char *)USART1_rxpoint))
                    #else
                    if (StrCmp2(UartStr, "%9962", (unsigned char *)USART1_rxpoint))
                    #endif
                    {
                        UART1_SendBytes("PWOFF\r\n", strlen("PWOFF\r\n"));
                    }
    //[kyq Begin: 20151014 15:49:44]
                    #ifdef PRD_VHD4_CMD_END
                    else if(StrCmp(UartStr, "%9975.~",  (unsigned char *)USART1_rxpoint))
                    #else
                    else if(StrCmp(UartStr, "%9975.",  (unsigned char *)USART1_rxpoint))
                    #endif
                    {
                        //dvLCDLineDraw(0, 0, "Out  01 02 03 04");
                        //dvLCDLineDraw(1, 0, "In   N  N  N  N");
                        BYTE cInPort = 0;
                        POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
                        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
                        SiiSprintf((char*)m_cUartMessage1, "Out  01 02 03 04");
                        SiiSprintf((char*)m_cUartMessage2, "In   01 01 01 01");
                        cCount = 4;
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
                        
                        // cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
                        UART1_SendData2PC(m_cUartMessage1, cCount);
                        UART1_SendData2PC(m_cUartMessage2, cCount);
                    #if(MAX_OUT>4)  //8 channel
                        #if(MAX_OUT>6)
    			        SiiSprintf((char*)m_cUartMessage1, "Out  05 06 07 08");
    			        SiiSprintf((char*)m_cUartMessage2, "In   01 01 01 01");
                        #else
    			        SiiSprintf((char*)m_cUartMessage1, "Out  05 06");
    			        SiiSprintf((char*)m_cUartMessage2, "In   01 01");
                        #endif

                        cCount = 4;
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
    		            UART1_SendData2PC(m_cUartMessage1, cCount);
                        UART1_SendData2PC(m_cUartMessage2, cCount);
                    #endif      

     			        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, FALSE);
                        
                    }
                    #ifdef PRD_VHD4_CMD_END
                    else if(StrCmp(UartStr, "%9978.~",  (unsigned char *)USART1_rxpoint))
                    #else
                    else if(StrCmp(UartStr, "%9978.",  (unsigned char *)USART1_rxpoint))
                    #endif
                    {
                        //查询输入端的HDCP兼容状态
                        PINPUTPORT_MANAGER_DATA       psInputPortMgrData;
                        psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnINPUTPORT_MAMAGER_DATA);
                        SiiSprintf((char*)m_cUartMessage1, "In   01 02 03 04");
                        SiiSprintf((char*)m_cUartMessage2, "HDCPEN    ");
                        cCount = 8;
                        for (i = 0; i < 4; i++)
                        {
                            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
                        }
                        
                        UART1_SendData2PC(m_cUartMessage1, cCount);
                        UART1_SendData2PC(m_cUartMessage2, cCount);
                    #if(MAX_IN>4)  //8 channel
                        #if(MAX_IN>6)
             			SiiSprintf((char*)m_cUartMessage1, "In   05 06 07 08");
                        SiiSprintf((char*)m_cUartMessage2, "HDCPEN    ");
                        #else
       					SiiSprintf((char*)m_cUartMessage1, "In   05 06");
                        SiiSprintf((char*)m_cUartMessage2, "HDCPEN    ");
                        #endif
                        cCount = 8;
                        for (i = 4; i < MAX_IN; i++)
                        {
                            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
                        }
                        
                        UART1_SendData2PC(m_cUartMessage1, cCount);
                        UART1_SendData2PC(m_cUartMessage2, cCount);

                    #endif
                        utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA, FALSE);
                    }
                    
                #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    #ifdef PRD_VHD4_CMD_END
                    else if(StrCmp(UartStr, "%9979.~",  (unsigned char *)USART1_rxpoint))
                    #else
                    else if(StrCmp(UartStr, "%9979.",  (unsigned char *)USART1_rxpoint))
                    #endif
                    {
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
                        if((OutLinkStatus&0xFF)!=(Get_PreLinkStatus()&0xFF))
                        {
                            //preLinkstatus = OutLinkStatus;
                            Set_PreLinkStatus(OutLinkStatus);
                        }
                        for (i = 0; i < 4; i++)
                        {
                            #if (MACHINETYPE == MHUB4K431)
                            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", (OutLinkStatus&(1<<i)||(i==3)) ? 'N' : 'Y');
                            #else
                            cCount += SiiSprintf((char*)&m_cUartMessage2[cCount], "  %c", OutLinkStatus&(1<<i) ? 'N' : 'Y');
                            #endif
                        }
                        UART1_SendData2PC(m_cUartMessage1, cCount);
                        UART1_SendData2PC(m_cUartMessage2, cCount);
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
                        UART1_SendData2PC(m_cUartMessage1, cCount);
                        UART1_SendData2PC(m_cUartMessage2, cCount);
                    #endif
                        SiiPlatformTimerSet(etLINK_SCAN,mS_TO_TICKS(5000L),0xFF); // use systick: 1ms per tick
                    }
                #endif
                
                    #ifdef PRD_VHD4_CMD_END
                    else if (StrCmp(UartStr, "/^Version;~", (unsigned char *)USART1_rxpoint) != NULL) // Software Version
                    #else
                    else if (StrCmp(UartStr, "/^Version;", (unsigned char *)USART1_rxpoint) != NULL) // Software Version
                    #endif
                    {
                        UART1_SendData2PC(g_FW_Version, strlen(g_FW_Version));
                    }
                    #ifdef PRD_VHD4_CMD_END
                    else if (StrCmp(UartStr, "%9961.~", (unsigned char *)USART1_rxpoint))
                    #else
                    else if (StrCmp(UartStr, "%9961.", (unsigned char *)USART1_rxpoint))
                    #endif
                    {
                        BOOL bLocked;
                        //PSYSTEM_SETTING_DATA    psSystemSettingData;
                        psSystemSettingData  = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
                        bLocked = psSystemSettingData->bSystemLock;
                        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                                          FALSE);
                        cCount += SiiSprintf((char*)m_cUartMessage1, "%s",
                                             bLocked ?	"System Locked!" : "System UnLock!");
                        // cCount += SiiSprintf(&cDrawTXT[cCount], "%s", "\r\n");
                        UART1_SendData2PC(m_cUartMessage1, cCount);
                    }
                    //////////////////////////////////////////////////////////////////////////////
                    else if (*UartStr == 0xf3)
                    {
                        BYTE IP_add[4];
                        if (get_ip_add(UartStr, IP_add))
                        {
                            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                            //ip_set_ok = 1;
                            for (i = 0; i < 4; i++) //i2c_EE_write_one_byte(sys_IP_add + i, IP_add[i]);
                            {
                                psSystemSettingData->acIP_Address[i] = IP_add[i];
                            }
                            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
                            utilDataMgrUpdate(UPDATE_ALL_NODEID);
                        }
                    }
    //[kyq End: 20151014 15:49:46]
                    g_bUSART1_DataComplete = FALSE; //uart_data_in_end = 0;
                    USART1_rxpoint = USART1_RX_BUFF;
                }
                
                if (bExit)
                {
    /*           
                    psPowerMgrData = (PPOWER_MANAGER_DATA)utilDataMgrWriteLockData(nnPOWER_MGR_DATA);
                         // if code run here it's power on	
                    psPowerMgrData->eLastPwrState =pwrON;
                    utilDataMgrUnlock(nnPOWER_MGR_DATA, TRUE);
                    utilDataMgrUpdate(nnPOWER_MGR_DATA);
                    NVIC_SystemReset();
    */
                    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    {
                        BYTE CloseLed[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
                        InOutPortLed_Process(CloseLed,1);
                    }
                    Set_Tm1623ValueChangeFlag(true);
                    SetUartDelayReceiveForPoweronFunction(15000);
                    #endif
                    break;
                }
        }
        break;
    case pwrSOFTRESET:
        ASSERT_ALWAYS();
        NVIC_SystemReset();
        break;
    case pwrOFF:
        ASSERT_ALWAYS();
        dvLedOnSet(1 << elnRED);
        dvLedOffSet(1 << elnGREEN);
        dvLCD_On(FALSE);
        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
        {
            BYTE CloseLed[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
            InOutPortLed_Process(CloseLed,0);
        }
        Set_Tm1623ValueChangeFlag(true);
        #endif
        BSP_GPIO_FanPower_EN(FALSE);
	  	BSP_GPIO_AP3012_EN(FALSE);
        dvVS010TXPowerOn(OFF);
        //set_clear_standby_flag(1);
       	appSystemSwitcherOutportForceCloseAll ();
        // here to send uart message for power off
        // other device power off here
        while (TRUE)
        {
            // lowe power, only uart can wake up
            ;
        }
        break;
    default:
        break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
    }
}
BYTE  get_standby_flag(void)
{
	return standby_flag ;
}
void  set_clear_standby_flag(BYTE EN)
{
	if(EN==1)
	{
		standby_flag=1;
	}
	else if(EN==0)
	{
		standby_flag=0;
	}
}
// ---------------------------------------------------------------------------
// This API is used to get the last power down/power up boot state. In the
// case where user input is detected and processed in the low power monitor
// loop a value will be stored in a CPU register variable that survives the
// boot process. This value is saved in halPowerCtrlInitialize to be used by
// the system power up code to decide weather to display the splash screen and
// finish the boot or to transition into a new power state.
//
// Params: none
//
// Return:
//  The last power state recorded by the low power monitor loop or pwrOFF to
//  signify that the system is coming on from a power fail state.
// ---------------------------------------------------------------------------
//ePOWERSTATE halPowerCtrlGetLastBootPowerState(void)
//{
//    return (m_eLastBootPowerState);
//}
// ---------------------------------------------------------------------------
// This API will return TRUE if the last suspend low power loop exit was
// caused by user input. This is used in case the system is currently in
// suspend mode and the user hits the power button to turn the device off.
//
// Params: none
//
// Return:
//  TRUE if user input was detected as the reason to leave the low power
//  suspend loop.
// ---------------------------------------------------------------------------
//BOOL halPowerCtrlGetLastSuspendExit(void)
//{
// return (m_bUserLastSuspendExit);
//}
