#include "IAP.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "USB_process.h"
#include "ST25P16.h"
#include "Flash.h"
#include "Sii_api.h"

uint32_t FlashDestination = ApplicationAddress; /* Flash user program offset */
uint16_t PageSize = PAGE_SIZE;
uint32_t EraseCounter = 0x0;
uint32_t NbrOfPage = 0;
FLASH_Status FLASHStatus = FLASH_COMPLETE;

uint32_t RamSource;
uint8_t  connect_ok = 0;
uint8_t  frame_ok = 0;
uint16_t frame_num = 0;
uint8_t  temp[5] = {0xef,0x0f,0,0,0};

extern uint8_t While9396;
extern uint8_t TxOrRx;

uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
  uint32_t pagenumber = 0x0;
  uint32_t size = Size;

  if ((size % PAGE_SIZE) != 0)
  {
    pagenumber = (size / PAGE_SIZE) + 1;
  }
  else
  {
    pagenumber = size / PAGE_SIZE;
  }
  return pagenumber;

}

//擦除应用区的flash，参数就是需要擦除的区域大小
void ERASE_APP(uint32_t size)
{
	/* Initialize FlashDestination variable */
	FlashDestination = ApplicationAddress;
	/* Erase the needed pages where the user application will be loaded */
	/* Define the number of page to be erased */
	//NbrOfPage = FLASH_PagesMask(0x10000);//擦除64K
	NbrOfPage = FLASH_PagesMask(size);
	/* Erase the FLASH pages */
	for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
	 	FLASHStatus = FLASH_ErasePage(FlashDestination + (PageSize * EraseCounter));
	}
}

void  write_APP(uint32_t buff_add,uint8_t buff_lenth)
{
	uint8_t  n = 0 ;
	if(connect_ok ==USBCLEAR)  return ;
	RamSource = buff_add; // 数据地址
	for (n = 0;n < buff_lenth;n += 4)
	{
		/* Program the data received into STM32F10x Flash */
		FLASH_ProgramWord(FlashDestination, *(uint32_t*)RamSource);
		
		if (*(uint32_t*)FlashDestination != *(uint32_t*)RamSource)  //表示写地址出错
		{
			 //重新发送整个APP数据
			 frame_ok = USBERROR;
		}
		FlashDestination += 4;
		RamSource += 4;
	}
}

uint8_t  IAP_flag_read(void)
{
	uint8_t   IAP_APP = 0;
	uint32_t  Ramflag = IAP_FLAG_Adderss;
	if(*(uint32_t*)Ramflag != 0x55551111)    IAP_APP = 1; //要进行IAP
	else  IAP_APP = 0; //直接跳到APP区
	
	return   IAP_APP ;
}

void  IAP_flag_clear(void)
{
	FLASH_Status  flash_ok = FLASH_COMPLETE;
	while(flash_ok == FLASH_COMPLETE)
	{
	  flash_ok = FLASH_ErasePage(IAP_FLAG_Adderss);	
	  if(flash_ok == FLASH_COMPLETE)
	  {
			FLASH_ProgramWord(IAP_FLAG_Adderss, 0x55551111); //写 0x55551111表示不升级
			return ;	
	  }
	  else  flash_ok = FLASH_COMPLETE;
	}
}

bool ResetEn = 1;
void  process(void)
{	
char UartMessage[50] = {0};
	if(frame_ok != USBCLEAR)
	  {
		  switch(frame_ok)
		  {	 case USBERROR:
				 if(usb_mode==USBMODE_FPGA)
				 	         ; 
		
				 else if(usb_mode==USBMODE_STM32)
				 {
				 	  ERASE_APP(APP_FLASH_SIZE);
		              FlashDestination = ApplicationAddress; /* Flash user program offset */
				  }
				     
				 break;
		  	 case USBERASE_STM32:
			      usb_mode=USBCLEAR;
				  ERASE_APP(APP_FLASH_SIZE);
		          FlashDestination = ApplicationAddress; /* Flash user program offset */
		  		  break;
		  	 case USBERASE_FPGA:
			      usb_mode=USBCLEAR;
			      Flash_Erase()	;
				  FlashDestination = FPGAAddress; /* Flash user program offset */
		  	      break;
			 case USBMODE_9396:
			if(TxOrRx == 'I')
			{
				for(While9396 = 1;While9396 <= MAX_IN;While9396++)
				{
			 	ResetEn = 0;
			      usb_mode=USBMODE_9396;
				  SiiFlashInit(While9396-1);
			        sii_device_isr_enable(While9396-1, false);
					if (flash_get_status(While9396-1) != FLASH_READY) {
                                		dbg("flash status err!\n");
						UART1_SendData2PC("FLASH status error.......", strlen("FLASH status error......."));
 					}
					flash_set_status(While9396-1, FLASH_IN_PROGRESS);
				    flash_erase(While9396-1);
				    SiiSprintf(UartMessage, "9396 Erase:%d",While9396);
                                UART1_SendData2PC(UartMessage, strlen(UartMessage));
				  FlashDestination = 0; //* Flash user program offset *
				}
			//While9396 = 1;
			}
			else if(TxOrRx == 'O')
			{
				for(While9396 = MAX_IN + 1;While9396 <= MAX_OUT+ MAX_IN;While9396++)
				{
			 	   ResetEn = 0;
			         usb_mode=USBMODE_9396;
				  SiiFlashInit(While9396-1);
			        sii_device_isr_enable(While9396-1, false);
					if (flash_get_status(While9396-1) != FLASH_READY) {
                                		dbg("flash status err!\n");
						UART1_SendData2PC("FLASH status error.......", strlen("FLASH status error......."));
 					}
					flash_set_status(While9396-1, FLASH_IN_PROGRESS);
				    flash_erase(While9396-1);
                                 SiiSprintf(UartMessage, "9396 Erase:%d",While9396);
                                UART1_SendData2PC(UartMessage, strlen(UartMessage));
				  FlashDestination = 0; //* Flash user program offset *
				}
			//While9396 = MAX_IN + 1;
			}
		  	      break;
			 case USBMODE_FPGA:
		          usb_mode=USBMODE_FPGA;
				  Flash_Erase();//整片擦除
				  FlashDestination = FPGAAddress; /* Flash user program offset */
			       break;
		  	 case USBMODE_STM32:
		          usb_mode=USBMODE_STM32;
		          ERASE_APP(APP_FLASH_SIZE);
			      FlashDestination = ApplicationAddress; /* Flash user program offset */
			      break;
			 case USBEXITBOOTLOARDER:
			 	SendAcktoPC(frame_ok);
			if(!ResetEn)
			 {
				SiiSprintf(UartMessage, "Write 9396 Finish!");
				UART1_SendData2PC(UartMessage, strlen(UartMessage));
			}
			else
			{
		           USB_Cable_Config(DISABLE);
		      	    IAP_flag_clear();
			}
                       frame_ok =USBCLEAR;
		 	  usb_mode=USBCLEAR;
			  connect_ok = 0;   //断开USB连接
			     if(ResetEn)
			     	{
			      		NVIC_SystemReset();//系统复位
			     	}
				 ResetEn = 1;
			   	  break;
		  	case USBENDOFSEGMENT:
			      usb_mode=USBCLEAR;
				  break;
			case SHAKEHANDS:
				 connect_ok =1;   //USB连接正常
				 usb_mode=USBCLEAR;
			     break;
			default: break;
		  }
	
		SendAcktoPC(frame_ok);//处理完后，回应PC
		frame_ok =USBCLEAR;
	}
}

uint8_t  reself_change(uint8_t data)
{
	uint8_t  i = 0,j = 0;
	j = data<<4;
	i = data>>4;
	return  (j|i) ;
}


void  deciphering(uint16_t fram_num ,uint8_t *buff_in,uint8_t *buff_out,uint8_t buff_num)
{
	uint8_t  i;
	for(i=0;i<buff_num;i++)  buff_out[i] = buff_in[i] ;		 //内存拷贝
	if(buff_num != 32)  return ;
	if(fram_num<=200)
	{
		if(fram_num != 21)
		{
			buff_out[5] = buff_in[0];
			buff_out[0] = buff_in[5];
			buff_out[15] = buff_in[10];
			buff_out[10] = buff_in[15];
			buff_out[25] = buff_in[20];
			buff_out[20] = buff_in[25];
		//	buff_out[35] = buff_in[30];
	//		buff_out[30] = buff_in[35];
	  }
	  else
	  {
	  	    buff_out[4] = buff_in[1];
			buff_out[1] = buff_in[4];
			buff_out[9] = buff_in[6];
			buff_out[6] = buff_in[9];
			buff_out[2] = reself_change(buff_in[2]);
			buff_out[8] = reself_change(buff_in[8]); 
	  }
	}
	else if(fram_num<=500)
	{
		if(fram_num != 451)
		{
			buff_out[2] = reself_change(buff_in[2]);
			buff_out[4] = reself_change(buff_in[4]); 
			buff_out[8] = reself_change(buff_in[8]); 
		  buff_out[10] = reself_change(buff_in[10]);
	  }
	  else
	  {
			buff_out[3] = reself_change(buff_in[3]);
			buff_out[9] = reself_change(buff_in[9]); 
			buff_out[13] = ~buff_in[13];
			buff_out[19] = ~buff_in[19];
	  }
	}
	else if(fram_num<=800)
	{
		if(fram_num != 581)
		{
			buff_out[1] = ~buff_in[1];
			buff_out[8] = ~buff_in[8];
			buff_out[18] = ~buff_in[18];
			buff_out[28] = ~buff_in[28];
	  }
	  else
	  { 
			buff_out[1] = ~buff_in[1];
			buff_out[4] = ~buff_in[4];
		//	buff_out[40] = CROLBYTE(buff_in[40],2);
		//	buff_out[51] = CROLBYTE(buff_in[51],2);
	  }
	}
	else if(fram_num<=1000)
	{
		if(fram_num != 821)
		{
			buff_out[10] = CROLBYTE(buff_in[10],3);
			buff_out[20] = CROLBYTE(buff_in[20],3);
			buff_out[30] = CROLBYTE(buff_in[30],3);
		//	buff_out[40] = CROLBYTE(buff_in[40],3);
	  }
	  else
	  { 
			buff_out[5] = CROLBYTE(buff_in[5],1);
			buff_out[8] = CROLBYTE(buff_in[8],1);
			buff_out[30] = CRORBYTE(buff_in[30],2);
		//	buff_out[56] = CRORBYTE(buff_in[56],2);
	  }
	}
	
}
