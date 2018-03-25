//
// $RCSfile: halPowerCtrl.c,v $
// $Revision: 1.0 $
// $Author:  $
// $Date:  20150805 9:57:45]
//
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
#include "IRTasks.h"
#include "SwitchVideoHandle.h"
#include "si_logger.h"
#include "sii_api.h"
#include "hal.h"
#include "InputHandle.h"
// The last boot power state.
//static ePOWERSTATE      m_eLastBootPowerState;
// BOOL TRUE if the last suspend exit was due to user input.
//static BOOL             m_bUserLastSuspendExit;
extern uint8_t  USART1_RX_BUFF[];
extern vu8      *USART1_rxpoint;  
extern BOOL   g_bUSART1_DataComplete;
static BYTE standby_flag =0;
extern char LCD1_IDLE_TEXT[17];
BYTE GetGuiIP=0;
// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
//#pragma warn -use
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
    PPOWER_MANAGER_DATA psPowerMgrData;
    PSYSTEM_SETTING_DATA         psSystemSettingData;
    unsigned char *pp;
    BYTE IP_add[4];
    BYTE acBraudRate[MAX_OUT];
    //unsigned int jiucuo; 
    BYTE m_cUartMessage1[LCD_MAX_CHAR];
    BYTE m_cUartMessage2[LCD_MAX_CHAR];
    BYTE cInstance = 0;
    BYTE cSii9396 = 0;
    
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
        dvLedOnSet(1 << elnGREEN);
        dvLedOffSet(1 << elnRED);
        BSP_GPIO_FanPower_EN(TRUE);
	 appSystemSwitcherOutportForceRecoverAll();//// Video and IR  huang
        // here to send uart message for power on
        //BL_ON;
        // Before we let the system wake up see how we got here.
        //psPowerMgrData->eLastPwrState;
        delay_nms(200); // 500
        dvCpldOpen();
        for (i = 0; i < MAX_OUT; i++)
        {
           /*dvCpldSendDataToRemote(i+1,
                                     acBraudRate[i],
                                     NULL,
                                     0);
                */
           dvCpldRemoteBraundSet(i + 1);
        }
         
        break;
    case pwrSTANDBY:
        dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
	 dvLCDLineDraw(1, 0, "STANDBY...");
        dvLedOnSet(1 << elnRED);
        dvLedOffSet(1 << elnGREEN);
        dvLCD_On(FALSE);
        BSP_GPIO_FanPower_EN(FALSE);
        dvVS010TXPowerOn(ON);
        //appSystemSwitcherOutportForceCloseAll ();
		// here to send uart message for power standby
        while (TRUE)
        {
            // IR, KEY, UART can week up
            IWDG_ReloadCounter();
            halUICtrlEventHandler();
            halIRKeyProcess();
            //AppTaskMsgLoop();
#if 0
		process_9396(cSii9396);
	   	cSii9396++;
	   	if (cSii9396 == DevNum)
	   	{
		   	cSii9396 = 0;
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
				if(GetGuiIP==1)			//上电时等GUI发送完IP才能进行机器standby或pwoff的唤醒
					bExit = TRUE;
			  }
			  else
			  {
				if(GetGuiIP==1)
				{
					bExit = TRUE;
					sKeyEvent.eKey = keNONE;
					set_clear_standby_flag(1);
				}
			  }
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
                    SiiSprintf(m_cUartMessage1, "Out  01 02 03 04");
                    SiiSprintf(m_cUartMessage2, "In   01 01 01 01");
                    cCount = 4;
                    for (i = 0; i < 4; i++)
                    {
                        cInPort = psOutputPortMgrData->acInPortSelect[i];
                        if (cInPort >= MAX_IN)
                        {
                            cCount += SiiSprintf(&m_cUartMessage2[cCount], " %s", "00");
                        }
                        else
                        {
                            cCount += SiiSprintf(&m_cUartMessage2[cCount], " %2d", cInPort + 1);
                        }
                    }
		      UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
 #if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
        SiiSprintf(m_cUartMessage1, "Out  05 06 07 08");
        SiiSprintf(m_cUartMessage2, "In   01 01 01 01");
#else
        SiiSprintf(m_cUartMessage1, "Out  05 06");
        SiiSprintf(m_cUartMessage2, "In   01 01");
#endif
                    cCount = 4;
                    for (i = 4; i < MAX_OUT; i++)
                    {
                        cInPort = psOutputPortMgrData->acInPortSelect[i];
                        if (cInPort >= MAX_IN)
                        {
                            cCount += SiiSprintf(&m_cUartMessage2[cCount], " %s", "00");
                        }
                        else
                        {
                            cCount += SiiSprintf(&m_cUartMessage2[cCount], " %2d", cInPort + 1);
                        }
                    }
		      UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
#endif
                    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, FALSE);
        
                    
                }
		  else if(StrCmp(UartStr, "%9977.",  (unsigned char *)USART1_rxpoint))
		  {
			 POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
			 SiiSprintf(m_cUartMessage1, "Out  01 02 03 04");
		        SiiSprintf(m_cUartMessage2, "Audio    Y Y Y Y");
		        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
		        cCount = 8;

			for(i=0;i<4;i++)
		        cCount += SiiSprintf(&m_cUartMessage2[cCount], " %c",
		                             psOutputPortMgrData->abHDMIAudio_EN[i]==SII_AUDIO__SPDIF ? 'Y' : 'N'
		                        
		                            );
		        UART1_SendData2PC(m_cUartMessage1, cCount);
		        UART1_SendData2PC(m_cUartMessage2, cCount);

			#if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
		        SiiSprintf(m_cUartMessage1, "Out  05 06 07 08");
		        SiiSprintf(m_cUartMessage2, "Audio    Y Y Y Y");
#else
		        SiiSprintf(m_cUartMessage1, "Out  05 06");
		        SiiSprintf(m_cUartMessage2, "Audio  Y  Y");
#endif
		        cCount = 8;
		  	 for(i=4;i<MAX_OUT;i++)
		        cCount += SiiSprintf(&m_cUartMessage2[cCount], " %c",
		                             psOutputPortMgrData->abHDMIAudio_EN[i]==SII_AUDIO__SPDIF ? 'Y' : 'N'
		                        
		                            );
		        UART1_SendData2PC(m_cUartMessage1, cCount);
		        UART1_SendData2PC(m_cUartMessage2, cCount);
#endif
		        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, FALSE);
		  }
		  else if(StrCmp(UartStr, "%9979.",  (unsigned char *)USART1_rxpoint))
		  {
			 switch(appGetDialSwitchEdidStatus())
			 {
				case eEDID_BYPASS:			
						sprintf(USART1_tx_buff, "EDID BYPASS 0000\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;         
				case eEDID_720P_2D_BYPASS:
						sprintf(USART1_tx_buff, "EDID 720P 2D BYPASS 0001\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_720P_3D_BYPASS:
						sprintf(USART1_tx_buff, "EDID 720P 3D BYPASS 0010\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_1080P_2D_BYPASS:
						sprintf(USART1_tx_buff, "EDID 1080P 2D BYPASS 0011\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_1080P_3D_BYPASS:
						sprintf(USART1_tx_buff, "EDID 1080P 3D BYPASS 0100\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_30HZ_BYPASS:
						sprintf(USART1_tx_buff, "EDID 4K 30HZ BYPASS 0101\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_30HZ_2CH:
						sprintf(USART1_tx_buff, "EDID 4K 30HZ 2.0CH 0110\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_30HZ_8CH:
						sprintf(USART1_tx_buff, "EDID 4K 30HZ 7.1CH 0111\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_60HZ_420_BYPASS:
						sprintf(USART1_tx_buff, "EDID 4K 60HZ 420 BYPASS 1000\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_60HZ_420_8CH:
						sprintf(USART1_tx_buff, "EDID 4K 60HZ 420 7.1CH 1001\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_60HZ_444_BYPASS:
						sprintf(USART1_tx_buff, "EDID 4K 60HZ 444 BYPASS 1010\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_60HZ_444_8CH:
						sprintf(USART1_tx_buff, "EDID 4K 60HZ 444 7.1CH 1011\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_RS232_GUI_CONTROL:
						sprintf(USART1_tx_buff, "EDID RS232 GUI CONTROL 1111\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				default:break;
			 }	
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
                    SiiSprintf(m_cUartMessage1, "In   01 02 03 04");
        		SiiSprintf(m_cUartMessage2, "HDCPEN  Y Y Y Y ");
                    cCount = 8;
                    for (i = 0; i < 4; i++)
                    {
                        cCount += SiiSprintf(&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
                    }
		      UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
#if(MAX_IN>4)  //8 channel
#if(MAX_IN>6)
        SiiSprintf(m_cUartMessage1, "In   05 06 07 08");
        SiiSprintf(m_cUartMessage2, "HDCPEN  Y Y Y Y ");
#else
        SiiSprintf(m_cUartMessage1, "In   05 06");
        SiiSprintf(m_cUartMessage2, "HDCPEN  Y Y");
#endif

                    cCount = 8;
                    for (i =4; i < MAX_IN; i++)
                    {
                        cCount += SiiSprintf(&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
                    }
		      UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
#endif
                    utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA, FALSE);
             
                }
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
                    cCount += SiiSprintf(m_cUartMessage1, "%s",
                                         bLocked ?	"System Locked!" : "System UnLock!");
                    // cCount += SiiSprintf(&cDrawTXT[cCount], "%s", "\r\n");
                    UART1_SendData2PC(m_cUartMessage1, cCount);
                }
                //////////////////////////////////////////////////////////////////////////////
                else if (*UartStr == 0xf3)
                {
                    BYTE IP_add[4]={0},cCount2=0;
		      BYTE acUartMsg[40] = { 0 };
                    if (get_ip_add(UartStr, IP_add))
                    {
                        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                        //ip_set_ok = 1;
                        for (i = 0; i < 4; i++) //i2c_EE_write_one_byte(sys_IP_add + i, IP_add[i]);
                        {
                            psSystemSettingData->acIP_Address[i] = IP_add[i];
                        }
			   GetGuiIP=1;
        		   cCount2 += SiiSprintf(acUartMsg, "IP:%d.%d.%d.%d",
                              psSystemSettingData->acIP_Address[0],
                              psSystemSettingData->acIP_Address[1],
                              psSystemSettingData->acIP_Address[2],
                              psSystemSettingData->acIP_Address[3]
                             );
        		   UART1_SendData2PC(acUartMsg, cCount2);	
                        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
                        utilDataMgrUpdate(UPDATE_ALL_NODEID);
                    }
                }
		  else if (StrCmp2(UartStr, "PWOFF.", (unsigned char *)USART1_rxpoint))
                {
		     	 UART1_SendBytes("Wake up first!\r\n", strlen("Wake up first!\r\n"));
                }
//[kyq End: 20151014 15:49:46]
#ifdef PRD_VHD4_CMD_END
                else if (StrCmp2(UartStr, ".~", (unsigned char *)USART1_rxpoint))
#else
                else if (StrCmp2(UartStr, ".", (unsigned char *)USART1_rxpoint))
#endif
                {
			 if(GetGuiIP==1)
			 	bExit = TRUE;
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
		  dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
		  dvLCDLineDraw(1, 0, "PWON");
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
	 dvCpldClose();
        dvVS010TXPowerOn(OFF);
	 appSystemSwitcherOutportForceCloseAll ();// Video and IR
        // here to send uart message for power off
        while (TRUE)
        {
            IWDG_ReloadCounter();
            halUICtrlEventHandler();
#if 0
	     process_9396(cSii9396);
	     cSii9396++;
	     if (cSii9396 == DevNum)
	     {
		   	cSii9396 = 0;
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
				if(GetGuiIP==1)
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
#ifdef PRD_VHD4_CMD_END
                if (StrCmp2(UartStr, "PWON.~", (unsigned char *)USART1_rxpoint))
#else
                if (StrCmp2(UartStr, "PWON.", (unsigned char *)USART1_rxpoint))
#endif
                {
			if(GetGuiIP==1)
				bExit = TRUE;
			//UART1_SendBytes("PWON\r\n", strlen("PWON\r\n"));
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
                    SiiSprintf(m_cUartMessage1, "Out  01 02 03 04");
                    SiiSprintf(m_cUartMessage2, "In   01 01 01 01");
                    cCount = 4;
                    for (i = 0; i < 4; i++)
                    {
                        cInPort = psOutputPortMgrData->acInPortSelect[i];
                        if (cInPort >= MAX_IN)
                        {
                            cCount += SiiSprintf(&m_cUartMessage2[cCount], " %s", "00");
                        }
                        else
                        {
                            cCount += SiiSprintf(&m_cUartMessage2[cCount], " %2d", cInPort + 1);
                        }
                    }
                    
                    // cCount += SiiSprintf(&m_cUartMessage2[cCount], "%s", "\r\n");
                    UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
#if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
        SiiSprintf(m_cUartMessage1, "Out  05 06 07 08");
        SiiSprintf(m_cUartMessage2, "In   01 01 01 01");
#else
        SiiSprintf(m_cUartMessage1, "Out  05 06");
        SiiSprintf(m_cUartMessage2, "In   01 01");
#endif

                    cCount = 4;
                   for (i = 4; i < MAX_OUT; i++)
                    {
                        cInPort = psOutputPortMgrData->acInPortSelect[i];
                        if (cInPort >= MAX_IN)
                        {
                            cCount += SiiSprintf(&m_cUartMessage2[cCount], " %s", "00");
                        }
                        else
                        {
                            cCount += SiiSprintf(&m_cUartMessage2[cCount], " %2d", cInPort + 1);
                        }
                    }
		      UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
#endif      

 			utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, FALSE);
                    
                }
		  else if(StrCmp(UartStr, "%9977.",  (unsigned char *)USART1_rxpoint))
		  {
			 POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
			 SiiSprintf(m_cUartMessage1, "Out  01 02 03 04");
		        SiiSprintf(m_cUartMessage2, "Audio    Y Y Y Y");
		        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
		        cCount = 8;

			for(i=0;i<4;i++)
		        cCount += SiiSprintf(&m_cUartMessage2[cCount], " %c",
		                             psOutputPortMgrData->abHDMIAudio_EN[i]==SII_AUDIO__SPDIF ? 'Y' : 'N'
		                        
		                            );
		        UART1_SendData2PC(m_cUartMessage1, cCount);
		        UART1_SendData2PC(m_cUartMessage2, cCount);

			#if(MAX_OUT>4)  //8 channel
#if(MAX_OUT>6)
		        SiiSprintf(m_cUartMessage1, "Out  05 06 07 08");
		        SiiSprintf(m_cUartMessage2, "Audio    Y Y Y Y");
#else
		        SiiSprintf(m_cUartMessage1, "Out  05 06");
		        SiiSprintf(m_cUartMessage2, "Audio  Y  Y");
#endif
		        cCount = 8;
		  	 for(i=4;i<MAX_OUT;i++)
		        cCount += SiiSprintf(&m_cUartMessage2[cCount], " %c",
		                             psOutputPortMgrData->abHDMIAudio_EN[i]==SII_AUDIO__SPDIF ? 'Y' : 'N'
		                        
		                            );
		        UART1_SendData2PC(m_cUartMessage1, cCount);
		        UART1_SendData2PC(m_cUartMessage2, cCount);
#endif
		        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, FALSE);
		  }
		  else if(StrCmp(UartStr, "%9979.",  (unsigned char *)USART1_rxpoint))
		  {
			 switch(appGetDialSwitchEdidStatus())
			 {
				case eEDID_BYPASS:			
						sprintf(USART1_tx_buff, "EDID BYPASS 0000\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;         
				case eEDID_720P_2D_BYPASS:
						sprintf(USART1_tx_buff, "EDID 720P 2D BYPASS 0001\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_720P_3D_BYPASS:
						sprintf(USART1_tx_buff, "EDID 720P 3D BYPASS 0010\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_1080P_2D_BYPASS:
						sprintf(USART1_tx_buff, "EDID 1080P 2D BYPASS 0011\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_1080P_3D_BYPASS:
						sprintf(USART1_tx_buff, "EDID 1080P 3D BYPASS 0100\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_30HZ_BYPASS:
						sprintf(USART1_tx_buff, "EDID 4K 30HZ BYPASS 0101\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_30HZ_2CH:
						sprintf(USART1_tx_buff, "EDID 4K 30HZ 2.0CH 0110\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_30HZ_8CH:
						sprintf(USART1_tx_buff, "EDID 4K 30HZ 7.1CH 0111\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_60HZ_420_BYPASS:
						sprintf(USART1_tx_buff, "EDID 4K 60HZ 420 BYPASS 1000\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_60HZ_420_8CH:
						sprintf(USART1_tx_buff, "EDID 4K 60HZ 420 7.1CH 1001\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_60HZ_444_BYPASS:
						sprintf(USART1_tx_buff, "EDID 4K 60HZ 444 BYPASS 1010\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_4K_60HZ_444_8CH:
						sprintf(USART1_tx_buff, "EDID 4K 60HZ 444 7.1CH 1011\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				case eEDID_RS232_GUI_CONTROL:
						sprintf(USART1_tx_buff, "EDID RS232 GUI CONTROL 1111\r\n");
						UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
						break;
				default:break;
			 }	
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
                    SiiSprintf(m_cUartMessage1, "In   01 02 03 04");
                    SiiSprintf(m_cUartMessage2, "HDCPEN    ");
                    cCount = 8;
                    for (i = 0; i < 4; i++)
                    {
                        cCount += SiiSprintf(&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
                    }
                    
                    UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);
#if(MAX_IN>4)  //8 channel
#if(MAX_IN>6)
         SiiSprintf(m_cUartMessage1, "In   05 06 07 08");
                    SiiSprintf(m_cUartMessage2, "HDCPEN    ");
#else
    SiiSprintf(m_cUartMessage1, "In   05 06");
                    SiiSprintf(m_cUartMessage2, "HDCPEN    ");
#endif
                    cCount = 8;
                    for (i = 4; i < MAX_IN; i++)
                    {
                        cCount += SiiSprintf(&m_cUartMessage2[cCount], "%c ", psInputPortMgrData->abInputHDCP_EN[i] ? 'Y' : 'N');
                    }
                    
                    UART1_SendData2PC(m_cUartMessage1, cCount);
                    UART1_SendData2PC(m_cUartMessage2, cCount);

#endif
     utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA, FALSE);
                }
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
                    cCount += SiiSprintf(m_cUartMessage1, "%s",
                                         bLocked ?	"System Locked!" : "System UnLock!");
                    // cCount += SiiSprintf(&cDrawTXT[cCount], "%s", "\r\n");
                    UART1_SendData2PC(m_cUartMessage1, cCount);
                }
                //////////////////////////////////////////////////////////////////////////////
                else if (*UartStr == 0xf3)
                {
                    BYTE IP_add[4]={0},cCount2=0;
		      BYTE acUartMsg[40] = { 0 };
                    if (get_ip_add(UartStr, IP_add))
                    {
                        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                        //ip_set_ok = 1;
                        for (i = 0; i < 4; i++) //i2c_EE_write_one_byte(sys_IP_add + i, IP_add[i]);
                        {
                            psSystemSettingData->acIP_Address[i] = IP_add[i];
                        }
			   GetGuiIP=1;
			   cCount2 += SiiSprintf(acUartMsg, "IP:%d.%d.%d.%d",
                              psSystemSettingData->acIP_Address[0],
                              psSystemSettingData->acIP_Address[1],
                              psSystemSettingData->acIP_Address[2],
                              psSystemSettingData->acIP_Address[3]
                             );
        		   UART1_SendData2PC(acUartMsg, cCount2);	
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
		  dvLCDLineDraw(0, 0, LCD1_IDLE_TEXT);
		  dvLCDLineDraw(1, 0, "PWON");
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
        BSP_GPIO_FanPower_EN(FALSE);
        dvVS010TXPowerOn(OFF);
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
