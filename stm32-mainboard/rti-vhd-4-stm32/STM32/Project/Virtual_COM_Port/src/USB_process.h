
#ifndef __USB_PROCESS_h
#define __USB_PROCESS_h
#include "stm32f10x.h"
#include "sys.h" 

 enum USBBACKCMD{  USBCLEAR=0,
                   USBNORMAL,
				   USBERROR,
				   USBEXITBOOTLOARDER,
                   USBCONNECT,
				   USBNOCONNECT,
				   USBBREAK,
			       SHAKEHANDS,
				   USBMODE_FPGA,
                                   USBMODE_9396,
				   USBMODE_STM32,
				   USBENDOFSEGMENT,
				   USBERASE_STM32,
				   USBERASE_FPGA,
				   USBNOSELECTMODE,
                } ;

#define USART_USB_RX_BUFF_SIZE   45
#define USBLENGTH  12
extern unsigned char USART_USBreceive_overflag  ;
extern unsigned char  USART_USB_RX_BUFF[USART_USB_RX_BUFF_SIZE];
extern unsigned char  USBUSARTFalg;
extern unsigned char USART_USBreceive_count;
extern unsigned char usb_mode;
extern unsigned int CurrentNum;
void USART_USB_Process(void);//处理串口或者USB收到的按键属性数据包或者级联包
unsigned int crc_check(unsigned char *data , unsigned char lenl);
void SendAcktoPC(unsigned char USB_Back_Cmd);
#endif

