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
#include "flash.h"
#include "Sii_api.h"
#include "USB_process.h"
#include "ST25P16.h"
#include "IAP.h"

#define REVDataMaxLength  32
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define  USB_RX_BUFF_MAX      20
#define IAP_FLAG_Adderss      0x8003C00
#define sii_inst_t	uint8_t

uint8_t  USB_RX_BUFF[USB_RX_BUFF_MAX];	//用来存储USB收到的属性数据，一次性的存储完，等完全接收完毕后才挨个处理

uint8_t  updata_temp[13]      = {0x7b,0x3a,0x00,0x05,0xef,0x00,0x00,0x00,0x00,0xef,0x45,0x4e,0x44};
uint8_t  usb_connect_temp[13] = {0x7b,0x3a,0x00,0x05,0xf1,0x00,0x00,0x00,0x00,0xf1,0x45,0x4e,0x44};

void USB_do_with_type(void);
void USART_USB_Process(void);//处理串口或者USB收到的按键属性数据包或者级联包
unsigned int crc_check(unsigned char *data , unsigned char lenl);
void SendAcktoPC(unsigned char USB_Back_Cmd);
void delay_nms(uint16_t count);

uint8_t While9396=0;
uint8_t TxOrRx=0;

uint8_t buffer_out[VIRTUAL_COM_PORT_DATA_SIZE];
uint8_t buffer_temp[VIRTUAL_COM_PORT_DATA_SIZE];
__IO uint32_t count_out = 0;
uint32_t count_in = 0;
uint32_t data_point = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern uint8_t  connect_ok;
extern uint8_t  frame_ok;
extern uint16_t frame_num;
extern void  write_APP(uint32_t buff_add,uint8_t buff_lenth);
extern void  deciphering(uint16_t fram_num ,uint8_t *buff_in,uint8_t *buff_out,uint8_t buff_num);





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
     	   if(((USB_RX_BUFF[i] == 'D')&&(USB_RX_BUFF[i-1] == 'N')&&(i>1)))
	   {
		USB_do_with_type();
     	   }
     }
     count_out = 0;
  }
#ifndef STM32F10X_CL
  /* Enable the receive of data on EP3 */
  SetEPRxValid(ENDP3);
#endif /* STM32F10X_CL */
}






unsigned char FPGA_write_buf[256]; 
unsigned int FPGALength=0;
unsigned int Length9396=0;

void write_9396(sii_inst_t inst,uint32_t offset, uint8_t* bpData, uint32_t lenth)
{
	unsigned char Write_count = 0;
	uint8_t read_buffer[SII_ISP_PAGE_SIZE]; 
	char UartMessage[50];
        //写Flash
	flash_write_block(inst, offset, lenth, bpData);
		
        //检查写Flash是否正确
retry:	flash_read_block(inst, offset, lenth, read_buffer);
			if (memcmp(bpData, read_buffer, lenth)) {
				if(Write_count < 5) {
						flash_write_block(inst, offset, lenth, bpData);
						SiiSprintf(UartMessage,  "Write Flash Error:%d",Write_count + 1 );
					UART1_SendData2PC(UartMessage , strlen(UartMessage));
                                        Write_count++;
						goto retry;
					}
                              else
                                {
                                 SiiSprintf(UartMessage,  "Write Flash Error");
				UART1_SendData2PC(UartMessage , strlen(UartMessage));;
                                }
                           }
}




void USB_do_with_type1(void)
{
uint8_t i=0;
  unsigned char Length;
  unsigned int 	CRC16_A,CRC16_B;
  char UartMessage[10] = {0};
  /* Get the received data buffer and update the counter */
  count_out = USB_SIL_Read(EP3_OUT, buffer_out);
  if ((count_out != 0) && (bDeviceState == CONFIGURED))
  {	  
        if(count_out > 12)	// 数据接收32字节
		  {	
		    if(connect_ok)
		       {
			    if((buffer_out[0]==0x55)&&(buffer_out[1]==0XAA)&&(buffer_out[2]==0X57)&&(buffer_out[3]==0X30))
			      {
				     CurrentNum=buffer_out[4]<<8|buffer_out[5];		 
					 Length=buffer_out[6];
	            	 CRC16_B=crc_check(&buffer_out[3],Length);
					 CRC16_A=buffer_out[Length+7]*256+buffer_out[Length+8];
	             	if(CRC16_A != CRC16_B) 
					 {
					 	frame_ok =USBERROR ;
						UART1_SendData2PC("CRC error......", strlen("CRC error......"));
					       return ;	  //校验不正确
					 }
					else		 
				 	   frame_ok = USBNORMAL ;
				  }
				 if(Length<REVDataMaxLength)
				        frame_ok = USBENDOFSEGMENT ;

			  	 if(usb_mode == USBMODE_STM32) //升级STM32
				 {
	   		        deciphering(CurrentNum,&buffer_out[7],buffer_temp,Length);//解密算法
                    
				    data_point = (uint32_t)&buffer_temp[0];
					write_APP(data_point,Length);
				 }                 
				
			  	 else 
			  	  if(usb_mode == USBMODE_FPGA)	//升级FPGA 
				   {   

				      deciphering(CurrentNum,&buffer_out[7],buffer_temp,Length);//解密算法
					  while(i<Length)
					  {
					     FPGA_write_buf[((CurrentNum-1)%8)*32+i]=buffer_temp[i];
						 i++;
					  }

					   FPGALength+=Length;	//每帧32字节

				//	  if(CurrentNum==200||CurrentNum==201)
					//  {
				//	  				   i=0;
				//	  }
				      if((CurrentNum%8==0)||(frame_ok==USBENDOFSEGMENT))	 //满256字节或最后一帧
					  {
					    FlashPageProgram(FlashDestination, &FPGA_write_buf[0], FPGALength);
						FlashDestination+=FPGALength;	 //falsh地址偏移
						FPGALength=0;
					  }
				}
				                    					
				 else
				 if(usb_mode == USBMODE_9396)	//升级FPGA 
				   {   

				      //deciphering(CurrentNum,&buffer_out[7],buffer_temp,Length);//解密算法

					  while(i<Length)
					  {
					     FPGA_write_buf[((CurrentNum-1)%8)*32+i]=buffer_out[7+i];//buffer_temp[i];
						 i++;
					  }

					   Length9396+=Length;	//每帧32字节

				
				      if((CurrentNum%8==0)||(frame_ok==USBENDOFSEGMENT))	 //满256字节或最后一帧
					  {
					   // FlashPageProgram(FlashDestination, &FPGA_write_buf[0], FPGALength);
						if(Length9396 <= 256)
						{
							IWDG_ReloadCounter();//喂狗
							if(TxOrRx == 'I')
							{
								for(While9396= 1;While9396<= MAX_IN;While9396++)
									write_9396(While9396-1,FlashDestination,&FPGA_write_buf[0], Length9396);
							}
							else if(TxOrRx == 'O')
							{
								for(While9396= MAX_IN+1;While9396<= MAX_IN + MAX_OUT;While9396++)
									write_9396(While9396-1,FlashDestination,&FPGA_write_buf[0], Length9396);
							}
						
                                                FlashDestination+=Length9396;	 //flash地址偏移
						IWDG_ReloadCounter();//喂狗
						Length9396=0;
						SiiSprintf(UartMessage, "%6d",FlashDestination);
						UART1_SendData2PC(UartMessage, strlen(UartMessage));
						}
						else
						{
							SiiSprintf(UartMessage, "Data Length %6d   %d",Length9396,Length);
							UART1_SendData2PC(UartMessage, strlen(UartMessage));
							Length9396 = 0;
						}
					  }
					Length = 0;

				 }
					else
					  frame_ok = USBNOSELECTMODE ;   
				}
			else  //未连接数据无效
			  frame_ok=USBNOCONNECT ;
		  }
		else //命令接收
 	      for (i = 0; i < count_out; i++)
         {
              USART_USB_RX_BUFF[USART_USBreceive_count++] = buffer_out[i];
		     if((USART_USBreceive_count>3)&&(USART_USB_RX_BUFF[USART_USBreceive_count-1]=='D')&&(USART_USB_RX_BUFF[USART_USBreceive_count-2]=='N')&&(USART_USB_RX_BUFF[USART_USBreceive_count-3]=='E'))
				 {
					  //USART_USBreceive_overflag = 1;
					  //USBUSARTFalg = 1;
					  USART_USB_Process();
				 } 
				 if(USART_USBreceive_count >= 45)	 USART_USBreceive_count = 0;
         }
     count_out = 0;
  }
  process();
#ifndef STM32F10X_CL
  /* Enable the receive of data on EP3 */
  SetEPRxValid(ENDP3);
#endif /* STM32F10X_CL */
}







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
	else
		USB_do_with_type1();
}


