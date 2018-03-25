/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V3.1.1
* Date               : 04/07/2010
* Description        : Virtual Com Port Demo main file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stm32f10x.h"
#include "hw_config.h"
//#include "usb_pwr.h"
//下面部分跟AVR一样的格式/////////////////
#include "board.h"
//#include "initPeriph.h"
////////////////////////6623////////////////
#include "main.h"
#include "Mcu.h"
//#include "EDID.h"
//#include "key_led_lcd.h"
#include "uart.h"
#include "uart2.h"
//#include "LCD_scan.h"
//#include "IIC_EE_4604_EDID_other.h"
#include "BSP_I2C.h"
//#include "Driver_9187.h"
#include "IR.h"
//#include "switch_driver.h"
#include "dvCPLD.h"
//#include "HDCP_Other_process.h"

#include "sk_application.h"
#if INC_CBUS
#include "sk_app_cbus.h"
#endif
//#include "si_debugger_hdmigear.h"
#include "si_eeprom.h"
#include "si_gpio.h"
#include "si_timer.h"
#include "si_cra.h"

//#define  INC_RTPI  1
#if INC_RTPI
#include "si_rtpi_component.h"
#endif

#include "si_drv_tpi_system.h"
#include "si_tx_component.h"

#include "si_regs_tpi953x.h"
#include "si_drv_audio_mix.h"

#include "si_drv_tpi_system.h"
#include "si_drv_tpi_access.h"
#include "si_tx_info_frm.h"
#include "si_tx_avi.h"
#include "si_tx_hdcp.h"
#include "si_tx_video.h"
#include "si_tx_audio.h"
#include "si_repeater_component.h"
#include "si_edid_tx_component.h"
#include "si_connection_monitor.h"
#ifdef PRD_USB_FUN
#include "usb_lib.h"
#include "usb_desc.h"
#endif
#include "si_tx_component.h"
#include "dvNVRAM.h"
#include "bsp_Gpio.h"
#include "dvADN4604.h"
#include "appPowerMgrAPI.h"
#include "sk_avr_repeater.h"
#include "KeyTasks.h"
#include "appUserInterface.h"
#include "util.h"
#include "dvLcd.h"
#include "si_app_adapter_rx.h"
#include "GitHash.h"
#include "dvPC9539.h"
#include "appSystemMgrAPI.h"

#include "dvTM1623.h"

#include "Si_debugger_hdmigear.h"

#define DEFAULT_5V_ENTER mS_TO_TICKS(10000L)

extern uint16_t SiiDrvDeviceIdGet(BYTE cInstance);
extern uint8_t SiiDrvDeviceRevGet(BYTE cInstance);

extern uint8_t g_Init_EdidFlashEdidTable[256];

extern void SysTick_Configuration(void);
extern void KEY_In_interruptConfig(void);

extern char LCD1_IDLE_TEXT[17];
extern char LCD2_IDLE_TEXT[17];
extern char m_acProduct_Type[17];
extern char m_acProduct_Name[17];

bool_t bStartupFlag = FALSE;

void SetStartupFlag(bool_t tbOn)
{
    bStartupFlag = tbOn;
}
bool_t GetStartupFlag(void)
{
    return bStartupFlag;
}

#if 1
#if 0
bool_t bStartupToSetTmdsFlag[MAX_OUT];
void SetStartupToSetTmdsFlag(BYTE cIntence,bool_t tbOn)
{
    bStartupToSetTmdsFlag[cIntence] = tbOn;
}
bool_t GetStartupToSetTmdsFlag(BYTE cIntence)
{
    return bStartupToSetTmdsFlag[cIntence];
}
#endif
void InitOutHDCP_Flag(void)
{
    BYTE i=0;
    for (i = 0; i < MAX_OUT; i++)	// 修改上电HDCP验证不过的问题
    {
        OutHDCP_Flag[i]=1;
    }
}
#endif

void Init_All_Periph(void);

void Init_StructInstance(BYTE cInstance)
{
    SiiTxInstanceSet(cInstance, 0);
    SiiDrvTpiStructInstanceSet(cInstance, 0);
    SiiDrvTpiAccessStructInstanceSet(cInstance, 0);
    TxInfoStructInstanceSet(cInstance, 0);
    TxAviStructInstanceSet(cInstance, 0);
    TxHdcpStructInstanceSet(cInstance, 0);
#if (SI_TX_VIDEO_CONVERSION == 1)
    TxVideoConversionStructInstanceSet(cInstance, 0);
#endif
    TxAudioStructInstanceSet(cInstance, 0);
    SiiRepeaterStructInstanceSet(cInstance, 0);
    SiiEdidTxStructInstanceSet(cInstance, 0);
    SiiRepeaterConStructInstanceSet(cInstance, 0);
}
/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
    BYTE cInstance = 0;
   BYTE i;	
    unsigned int cLoopCNT = 0;
    BYTE cSii9679LoopCNT = 0;
    BYTE cSii9679 = 0;
    SetStartupFlag(FALSE);

    Set_System(); // 系统时钟配置
    /************USB初始化*********************/
#ifdef PRD_USB_FUN
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
#endif
    ///////////////////////////////////////////
    UART2_Init();
    SiiPlatformTimerInit();
   
    Init_All_Periph(); // GPIO, TIMER,I2C, UART,IR ISR init,
 //    dvSii9533HardWareReset(); // earaly reset
    dvCpldClose();
//#ifdef VHD4_CPLD    // kyq 20151126 ask by dajie
    dvCpldOpenMCUToDB9(1);
//#endif
   
    SysTick_Configuration();
    BSP_Gpio_Low(egpioANDReset);  //复位4604
    SiiPlatformTimerInit();
    dvPC9539Init(); //IO 扩展
    dvSii9533HardWareReset(); // earaly reset
    AppModuleRegisterInit();  //各个模块的初始化
    #if 0//def ENABLE_DB9_BAUDRATE_SWITCH
    InitDB9Baudrate();
    #endif
    PrintAlways("Build: " __DATE__"-" __TIME__ "\r\n");
    //需要把hook 拷贝到.git 的hook 中提交生成m_cGitHash
    PrintAlways(m_acGitHash);
    appUIInitialize();
    dvSii9533Init(); 		//初始化9533
	
    UART1_SendData2PC("Initialization Finished!",strlen("Initialization Finished!"));
    UART1_SendData2PC(m_acProduct_Type,strlen(m_acProduct_Type));
    UART1_SendData2PC(m_acProduct_Name, strlen(m_acProduct_Name));
    UART1_SendData2PC(g_FW_Version, strlen(g_FW_Version));
    #if 0
	InitOutHDCP_Flag();
    #endif
   appPowerMgrInitialize();
#ifdef PRD_IR_OUT
    appSystemIRSwitcherSetStart();
#endif

#if 1
    InitRemoteCtrlMcu();
    InitRemoteBaudrate();
#endif

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    KeyLedStatuesInitialize();
    #if 1//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    InitLinkStatue();
    #endif
    SetUartDelayReceiveForPoweronFunction(0);
#endif
     SetStartupFlag(TRUE);
     SiiPlatformTimerSet(etPWON_5V_DELAY, DEFAULT_5V_ENTER,0x01);    //expect only one
    /***********************************************高级管理系统***************************************************************/
    while (1)
    {
        /*********************系统指示灯*********************************/
        {
            if (cLoopCNT++ == 20)
            {
                BSP_GPIO_LED_Toggle();
                cLoopCNT = 0;
            }
            IWDG_ReloadCounter(); //喂狗
        }
        /************************************************************/
        Uart1_process(); 

        /*****************************************************************/
// let small time for one loop process
  //    for(i=0;i<5;i++)
        dvSii9533Process(cInstance);
        cInstance++;
        if (cInstance == INSTANCE_9533)
        {
            cInstance = 0;
        }

        if (cSii9679LoopCNT++ > 3)
        {
            for (cSii9679 = 0; cSii9679 < INSTANCE_9679; cSii9679++)
            {
                dvSiiDrvAdaptTaskExecute(cSii9679);
            }
            cSii9679LoopCNT = 0;
        }
      
        /**************************************************************************************************************/
        appOutputHDCPProcess();
        appPowerMgrProcess();
        appUIProcess();
        SkRemoteRequestHandler();
        AppTaskMsgLoop();
        
    }
}
#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    //APP_TRACE("###ERROR %s %d###\n",file,line);
    //printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    //APP_Printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    DEBUG_PRINT2(MSG_ALWAYS, "###Wrong parameters value: file %s on line %d\r\n", file, line);
    while (1)
    {}
}
#endif

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
