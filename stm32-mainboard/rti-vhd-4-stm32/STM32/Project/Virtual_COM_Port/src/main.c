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
#include "uart.h"
#include "uart2.h"
#include "BSP_I2C.h"
#include "IR.h"
#include "dvCPLD.h"
#include "si_timer.h"

#ifdef PRD_USB_FUN
#include "usb_lib.h"
#include "usb_desc.h"
#endif
#include "dvNVRAM.h"
#include "bsp_Gpio.h"
#include "dvM21452.h"
#include "appPowerMgrAPI.h"
#include "KeyTasks.h"
#include "appUserInterface.h"
#include "util.h"
#include "dvLcd.h"
#include "GitHash.h"
#include "dvPC9539.h"
#include "appSystemMgrAPI.h"
#include "dvTM1623.h"
#include "InputHandle.h"
#include "OutputHandle.h"

#include "sii_common.h"
#include "sii_api.h"
#include "hal.h"

extern BOOL   g_bUSART1_DataComplete;
extern uint8_t g_Init_EdidFlashEdidTable[256];

extern void SysTick_Configuration(void);
extern void KEY_In_interruptConfig(void);

extern char LCD1_IDLE_TEXT[17];
extern char LCD2_IDLE_TEXT[17];
extern char m_acProduct_Type[17];
extern char m_acProduct_Name[17];

extern uint16_t run9396_count;
extern BYTE FirstPwonFlag;

void Init_All_Periph(void);
extern enum sii_scramble_state cOutHDMIVersion[MAX_OUT];
extern enum sii_scramble_state cOutHaveDetectHDMI20[MAX_OUT];
extern unsigned char usb_mode;
 
/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{

    BYTE cInstance = 0,i=0;
    unsigned int cLoopCNT = 0;
    BYTE cSii9396LoopCNT = 0;
    BYTE RXHpd_flag=1;

    Set_System(); // 系统时钟配置
    /************USB初始化*********************/
#ifdef PRD_USB_FUN
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
#endif
    ////////////////////////////////////////////
    for(i=0;i<MAX_OUT;i++)
    {
		cOutHDMIVersion[i]=SII_SCRAMBLE_OFF;
		cOutHaveDetectHDMI20[i]=SII_SCRAMBLE_OFF;
    }
    UART2_Init();
    ClearUartInEdidValue();
    Init_All_Periph(); 				// GPIO, TIMER,I2C, UART,IR ISR init,
    SysTick_Configuration();
    BSP_GPIO_POWER_SW_EN(TRUE);
    delay_nms(200);
    //dvInSii9396HardWareReset(); 	// earaly reset
    //dvOutSii9396HardWareReset();
    dvPC9539Init(); 				//IO 扩展
    SiiPlatformTimerInit();
    AppModuleRegisterInit();  		//各个模块的初始化

    appUIInitialize();

    UART1_SendData2PC("Initialization Finished!",strlen("Initialization Finished!"));
    UART1_SendData2PC(m_acProduct_Type,strlen(m_acProduct_Type));
    UART1_SendData2PC(m_acProduct_Name, strlen(m_acProduct_Name));
    UART1_SendData2PC(g_FW_Version, strlen(g_FW_Version));
    UART1_SendBytes("\r\n",2);

   appPowerMgrInitialize();
#ifdef PRD_IR_OUT
    appSystemIRSwitcherSetStart();
#endif

    #if INC_OUTPUT_VIDEO
    Init_OutPutVideo_module(mqOUTPUT);
    #endif
    //for(cInstance=0;cInstance<MAX_OUT;cInstance++)
        //appOutputHDCPProcess();
    run9396_count=0;
    cInstance=0;
    //cSii9396LoopCNT=20;
    /***********************************************高级管理系统***************************************************************/
    while (1)
    {
        /*********************系统指示灯*********************************/
        {
            if (cLoopCNT++ == 50)
            {
                BSP_GPIO_LED_Toggle();
                cLoopCNT = 0;
            }
            IWDG_ReloadCounter(); //喂狗
        }
 /************************************************************/
       Uart1_process();
 	//halUICtrlEventHandler();

/*****************************************************************/
// let small time for one loop process
	 if(run9396_count==0)
	 {
		if (cSii9396LoopCNT <3)
			run9396_count=80;
		else
			run9396_count=300;
		if(g_bUSART1_DataComplete==FALSE)		//如果串口没数据才处理9396
		{
			process_9396(cInstance);
			cInstance++;
		}
		info("sil9396= %d ..............\n",cInstance);
		info("");
		if(cSii9396LoopCNT==5)
		{
		   cSii9396LoopCNT++;
		   FirstPwonFlag=1;
		   //SiiPlatformTimerSet(etINPORTMAG_EDID,mS_TO_TICKS(500) ,1);
		   //TIM2_Configuration();
		}
	 }
     	 if (cInstance ==SII9396_DevNum)
     	 {
            cInstance = 0;
	     if (cSii9396LoopCNT <20)
	     		cSii9396LoopCNT++;
     	 }
 /**************************************************************************************************************/
        appPowerMgrProcess();
        AppTaskMsgLoop();
	while(1)
        {
          if((usb_mode!=8)&&(usb_mode!=9)&&(usb_mode!=10))  //升级9396，不做任何处理
            break;
        }
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
