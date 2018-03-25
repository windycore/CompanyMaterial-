 #include "USB_process.h"
 #include "usb_lib.h"
 #include "IAP.h"
 #include "Mcu.h"
unsigned char USART_USBreceive_overflag  ;
unsigned char  USART_USB_RX_BUFF[USART_USB_RX_BUFF_SIZE]; 
unsigned char  USBUSARTFalg;
unsigned char USART_USBreceive_count;
unsigned char FeedBackCmd[USBLENGTH];
unsigned char usb_mode;
extern unsigned char frame_ok;
static void  dealwithdata(uint8_t *USBdata);
unsigned int CurrentNum;

extern uint8_t While9396;
extern uint8_t TxOrRx;

extern bool ResetEn ;
 unsigned int crc_check(unsigned char *data , unsigned char lenl)
{
    unsigned int crc=0,i;
   for(i=0;i<4+lenl;i++)
   {
      crc += data[i];
   }
  return crc;
}
  /*******************************************************************************
* Function Name  : dealwithCMD
* Description    : 处理对应的指令内容
* Input          : 指针（这里防止出错，最好是帧的首地址） 
* Output         : None.
* Return         : None.
*******************************************************************************/
void  dealwithCMD(uint8_t *USBdata)
{

	switch(USBdata[3]) //命令
	{

		case 0x01:frame_ok =USBCONNECT ;break;
		case 0x11:frame_ok =SHAKEHANDS ;break;
		case 0x22:frame_ok =USBMODE_FPGA ;break;
		case 0x24:
			frame_ok =USBMODE_9396 ;
				if(USBdata[5] == 'I')
				{
					TxOrRx = 'I';
					/*Num9396 = USBdata[6];
					if(0<Num9396<=MAX_IN)
						frame_ok =USBMODE_9396 ;
					else
						frame_ok =USBERROR ;*/
				}
				else
					if(USBdata[5] == 'O')
					{
						TxOrRx = 'O';
						/*Num9396 = USBdata[6] + MAX_IN;
						if(MAX_IN<Num9396<=MAX_IN+MAX_OUT)
							frame_ok =USBMODE_9396 ;
						else
							frame_ok =USBERROR ;*/
					}
				else
					frame_ok =USBERROR ;
				break;
		case 0x21:frame_ok =USBMODE_STM32 ;break;
		case 0xA2:frame_ok =USBEXITBOOTLOARDER ; break;
		default:  break;	  		
	}

}

  /*******************************************************************************
* Function Name  : SendAcktoPC
* Description    : 相应指令，反馈正确或错误代号
* Input          : 指令类型
* Output         : None.
* Return         : None.
*******************************************************************************/

void SendAcktoPC(unsigned char USB_Back_Cmd)
{	unsigned int CRC16;
	FeedBackCmd[0] = 0x55 ;
	FeedBackCmd[1] = 0xAA ;
	FeedBackCmd[2] = 0x52 ;
   switch(USB_Back_Cmd)
   {
   	   case USBEXITBOOTLOARDER:
		  	FeedBackCmd[3] = 0xA2 ;
	        break;
	   case USBCONNECT:
   			FeedBackCmd[3] = 0x03 ;
			break;
	   case	USBERROR :
			FeedBackCmd[3] = 0x3F ;
			break;
	   case SHAKEHANDS:
   			FeedBackCmd[3] = 0x11 ;
		    break;
	   case USBMODE_FPGA:
			FeedBackCmd[3] = 0x22 ;
			break;
	   case USBMODE_STM32:
	      	FeedBackCmd[3] = 0x21 ;
			break;
	   case USBMODE_9396:
			FeedBackCmd[3] = 0x24 ;
			break;
	   case USBENDOFSEGMENT:
	      	FeedBackCmd[3] = 0x40 ;
			break;
	   case USBERASE_STM32:
	        FeedBackCmd[3] = 0x50 ;
			break;
	   case USBERASE_FPGA:
	        FeedBackCmd[3] = 0x5A ;
			break;
	   case USBNOCONNECT:
	        FeedBackCmd[3] = 0x3A ;
			break;
	   case USBNOSELECTMODE:
	        FeedBackCmd[3] = 0x3B ;
			break;
	   case	USBNORMAL:
	      	FeedBackCmd[3] = 0x30 ;
			break;  
	  default: 	break;
   }
    if(USB_Back_Cmd==USBERROR||USB_Back_Cmd==USBNORMAL||USB_Back_Cmd==USBENDOFSEGMENT)
	  FeedBackCmd[4] = CurrentNum;
	else
	  FeedBackCmd[4] = 0x00 ;
if(!ResetEn )//ResetEn为0,表示更新9396
{
	if(While9396>MAX_IN)
	{
		FeedBackCmd[5] = 'O' ;
		FeedBackCmd[6] = While9396 - MAX_IN ;
	}
	else
	{
		FeedBackCmd[5] = 'I' ;
		FeedBackCmd[6] = While9396  ;
	}
}
else
{
	FeedBackCmd[5] = 0x00 ;
	FeedBackCmd[6] = 0x00 ;
}
	CRC16=crc_check(&FeedBackCmd[3],0);
	FeedBackCmd[7]=(CRC16>>8)&0x00ff;
	FeedBackCmd[8]=CRC16 &0x00ff;
	FeedBackCmd[9]  = 'E' ;
	FeedBackCmd[10] = 'N' ;
	FeedBackCmd[11] = 'D' ;



   /*
	//选择串口或者USB发送给PC
	if(USBUSARTFalg == 0) //串口
	{
	   USART_send(feedback_erro,13);
	}
 */
//	else   //USB转串口
	{
	   USB_SIL_Write(EP1_IN,FeedBackCmd ,USBLENGTH);	  
	   #ifndef STM32F10X_CL
	   SetEPTxValid(ENDP1);
	   #endif /* STM32F10X_CL */
//	   delay_nms(10);
	}

}


void USART_USB_Process(void)//处理串口或者USB收到的按键属性数据包或者级联包
{

	unsigned char *temppoint;
	temppoint = USART_USB_RX_BUFF;
	if(*temppoint == 0x55&&*(temppoint+1)==0xAA&&*(temppoint+2)==0x57)  
		dealwithdata(temppoint);
	IWDG_ReloadCounter();//喂狗

	USART_USBreceive_count = 0;
	USART_USBreceive_overflag = 0; 
}

/*******************************************************************************
* Function Name  : dealwithdata
* Description    : 
* Input          : 指针（这里防止出错，最好是帧的首地址） 
* Output         : None.
* Return         : None.
*******************************************************************************/
void  dealwithdata(uint8_t *USBdata)
{

//	uint8_t cmd_MCMD = 0;
	unsigned int  CRC16_A,CRC16_B;

   	unsigned char Length=0;

	CRC16_A=crc_check(&USBdata[3],Length);
	CRC16_B=(USBdata[Length+7]*256+USBdata[Length+8]);
 	//if(CRC16_A!=CRC16_B )  return ;	  //校验不正确
	IWDG_ReloadCounter();//喂狗
	dealwithCMD(USBdata);  //处理对应的指令内容
	IWDG_ReloadCounter();//喂狗		 
}

