/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V3.1.1
* Date               : 04/07/2010
* Description        : Endpoint routines
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
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define  USB_RX_BUFF_MAX      20
#define IAP_FLAG_Adderss      0x8003C00

uint8_t buffer_out[VIRTUAL_COM_PORT_DATA_SIZE];
__IO uint32_t count_out = 0;
uint32_t count_in = 0;
uint8_t  USB_RX_BUFF[USB_RX_BUFF_MAX];	//用来存储USB收到的属性数据，一次性的存储完，等完全接收完毕后才挨个处理

uint8_t  updata_temp[13]      = {0x7b,0x3a,0x00,0x05,0xef,0x00,0x00,0x00,0x00,0xef,0x45,0x4e,0x44};
uint8_t  usb_connect_temp[13] = {0x7b,0x3a,0x00,0x05,0xf1,0x00,0x00,0x00,0x00,0xf1,0x45,0x4e,0x44};

void USB_do_with_type(void);

void delay_nms(uint16_t count);
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
  count_in = 0;
}

/*******************************************************************************
* Function Name  : EP3_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
  uint8_t i;
  /* Get the received data buffer and update the counter */
  count_out = USB_SIL_Read(EP3_OUT, buffer_out);
  if ((count_out != 0) && (bDeviceState == CONFIGURED))
  {
 	   for (i = 0; i < count_out; i++)
     {
     	   USB_RX_BUFF[i] = buffer_out[i];  //串口清中断是当你读走数据后硬件自动完成清中断
     	   if(((USB_RX_BUFF[i] == 'D')&&(USB_RX_BUFF[i-1] == 'N')&&(i>1)))  USB_do_with_type();
     }
     count_out = 0;
  }
#ifndef STM32F10X_CL
  /* Enable the receive of data on EP3 */
  SetEPRxValid(ENDP3);
#endif /* STM32F10X_CL */
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

void updata_source(void)
{
	FLASH_Status  flash_ok = FLASH_COMPLETE;
	/* Flash unlock */
  FLASH_Unlock();
	while(flash_ok == FLASH_COMPLETE)
	{
	  flash_ok = FLASH_ErasePage(IAP_FLAG_Adderss);	
	  if(flash_ok == FLASH_COMPLETE)
	  {
			FLASH_ProgramWord(IAP_FLAG_Adderss, 0x12344321);   
			
			NVIC_SystemReset();
			return ;	
	  }
	  else  flash_ok = FLASH_COMPLETE;
	}			
}

//专门处理IAP
void USB_do_with_type(void)
{
	unsigned char  *tempptr,i;

	tempptr = USB_RX_BUFF;	
	if(*tempptr == 0x7D)
	{
		for(i=0;i<4;i++)
		{
			tempptr ++ ;
	    if(tempptr >= (USB_RX_BUFF + USB_RX_BUFF_MAX))
	       tempptr = USB_RX_BUFF; 
		}
		if(*tempptr == 0xef)
		{
			//IAP    		     
      USB_SIL_Write(EP1_IN,updata_temp ,13);	  
		  #ifndef STM32F10X_CL
		  SetEPTxValid(ENDP1);
		  #endif /* STM32F10X_CL */
		  delay_nms(10);
		  count_out = 0;
      updata_source();
		}
		else if(*tempptr == 0xf1)
		{   		     
      USB_SIL_Write(EP1_IN,usb_connect_temp ,13);	  
		  #ifndef STM32F10X_CL
		  SetEPTxValid(ENDP1);
		  #endif /* STM32F10X_CL */
		  delay_nms(10);
		}	 
	}
}

