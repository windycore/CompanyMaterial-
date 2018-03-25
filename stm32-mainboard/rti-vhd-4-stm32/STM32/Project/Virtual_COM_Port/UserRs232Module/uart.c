///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Utility.c>
//   @author wuxiaoyong
//   @date   2012/11/09
//   @fileversion: IT6623_Sample_1.04
//******************************************/
#include <string.h>
#include "main.h"
#include "stm32f10x.h"
#include "uart.h"
#include "dvCPLD.h"
#include "dvPC9539.h"
#include <stdarg.h>
#include "dvNVRAM.h"
#include "dvM21452.h"
#include "BSP_I2C.h"
#include "dvNVRAM.h"
#include "util.h"
#include "appSystemMgrAPI.h"
#include "appUserInterface.h"
#include "si_logger.h"
#include "GitHash.h"
#include "Si_timer.h"
#include "appPowerMgrAPI.h"
#include "LedReceiveEvent.h"
#include "SwitchVideoReceiveEvent.h"
#include "KeyTasks.h"
#include "InputHandle.h"
#include "CPLDHandle.h"
#include "si_osal_timer.h"
#include "dvLcd.h"
#include "sii_api.h"

//===================================


vu8      USART_data_flag = 0; //0  ����ָ��     1 EDID���ڸ���ָ��    2����EDID���ڸ���ָ��
vu16     USART_EDIDdata_count = 0;   // ���ڽ���EDID ��������
uint8_t  EDID_updata_port = 0; // ����EDIDD�Ķ˿�
uint8_t  USART_EDIDdata[EDID_TABLE_LEN];
/////////////////////////////////////////////////////////////////////////
uint8_t  USART1_RX_BUFF[USART1_RX_BUFF_MAX]; //���ڽ���������

// ָ��ǰҪ�������ݵ�buffer ( βָ��)
vu8      *USART1_rxpoint;  //���ж����бȽϵĶ���������������volatile�ͱ���
//vu8      rxdelay_USART1 = 100; // ���ڽ������ݺ��ʱ��
WORD     g_wUSART1_TIMER;
// TRUE: �������յ�
BOOL     g_bUSART1_DataReceived; // vu8 uart_data_in_start = 0; //�������ڽ���
BOOL     g_bUSART1_DataComplete; // TRUE: the RX message is complete vu8 uart_data_in_end = 0; //���ݽ������
//uint8_t IR_carrier_mode = 0;
char     USART1_tx_buff[USART1_TX_BUFF_MAX];

clock_time_t EdidUpataStart = 0;
static BOOL m_bCommdDemo = FALSE; // kyq 20150929
static BOOL bDemoEn = FALSE;

static uint8_t UartInEdidValue[MAX_IN]={0};
extern unsigned char KeyValue;
extern BYTE OutputEdidData[MAX_OUT][256];

extern char LCD1_IDLE_TEXT[17];
extern char LCD2_IDLE_TEXT[17];
extern BYTE GetGuiIP;

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
extern BYTE KeyInput[8];
#endif
BOOL  System_UnLock_flag;
BYTE  u8IChannellock[MAX_OUT];

struct Demo_struct
{
    uint8_t InChanel;
    uint8_t OutChanel;
}Demo_buff;

BYTE acEDID_data[256]={0};

uint8_t g_Init_EdidFlashEdidTable [ 256 ] =
{
	0x00 ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0x00 ,0x7F ,0xFF ,0x00 ,0x90 ,0x0F ,0x00 ,0x00 ,0x00 ,
	0x01 ,0x12 ,0x01 ,0x03 ,0x80 ,0xA0 ,0x5A ,0x78 ,0x0A ,0x01 ,0xC1 ,0xA0 ,0x57 ,0x47 ,0x98 ,0x27 ,
	0x12 ,0x4C ,0x4C ,0x21 ,0x08 ,0x00 ,0x31 ,0x40 ,0x45 ,0x40 ,0x61 ,0x40 ,0x81 ,0x80 ,0x01 ,0x01 ,
	0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x02 ,0x3A ,0x80 ,0x18 ,0x71 ,0x38 ,0x2D ,0x40 ,0x58 ,0x2C ,
	0x45 ,0x00 ,0x40 ,0x84 ,0x63 ,0x00 ,0x00 ,0x1E ,0x28 ,0x3C ,0x80 ,0xA0 ,0x70 ,0xB0 ,0x23 ,0x40 ,
	0x30 ,0x20 ,0x36 ,0x00 ,0x40 ,0x84 ,0x63 ,0x00 ,0x00 ,0x1A ,0x00 ,0x00 ,0x00 ,0xFC ,0x00 ,0x0A ,
	0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x00 ,0x00 ,0x00 ,0xFD ,
	0x00 ,0x30 ,0x3E ,0x0E ,0x44 ,0x0F ,0x00 ,0x0A ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x01 ,0xC1 ,

	0x02 ,0x03 ,0x23 ,0x70 ,0x4F ,0x01 ,0x02 ,0x03 ,0x04 ,0x05 ,0x06 ,0x07 ,0x10 ,0x11 ,0x12 ,0x13 ,
	0x14 ,0x15 ,0x16 ,0x1F ,0x23 ,0x09 ,0x07 ,0x07 ,0x83 ,0x01 ,0x00 ,0x00 ,0x66 ,0x03 ,0x0C ,0x00 ,
	0x20 ,0x00 ,0x80 ,0x01 ,0x1D ,0x80 ,0xD0 ,0x72 ,0x1C ,0x16 ,0x20 ,0x10 ,0x2C ,0x25 ,0x80 ,0x40 ,
	0x84 ,0x63 ,0x00 ,0x00 ,0x9E ,0x01 ,0x1D ,0x80 ,0x18 ,0x71 ,0x1C ,0x16 ,0x20 ,0x58 ,0x2C ,0x25 ,
	0x00 ,0x40 ,0x84 ,0x63 ,0x00 ,0x00 ,0x98 ,0x01 ,0x1D ,0x00 ,0xBC ,0x52 ,0xD0 ,0x1E ,0x20 ,0xB8 ,
	0x28 ,0x55 ,0x40 ,0x40 ,0x84 ,0x63 ,0x00 ,0x00 ,0x1E ,0x02 ,0x3A ,0x80 ,0xD0 ,0x72 ,0x38 ,0x2D ,
	0x40 ,0x10 ,0x2C ,0x45 ,0x80 ,0x40 ,0x84 ,0x63 ,0x00 ,0x00 ,0x1E ,0x02 ,0x3A ,0x80 ,0x18 ,0x71 ,
	0x38 ,0x2D ,0x40 ,0x58 ,0x2C ,0x45 ,0x00 ,0x40 ,0x84 ,0x63 ,0x00 ,0x00 ,0x1E ,0x00 ,0x00 ,0xF8
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void UART1_SendByte(u16 Data)
{
    while (!(USART1->SR & USART_FLAG_TXE));
    USART1->DR = (Data & (uint16_t)0x01FF);
    while (!(USART1->SR & USART_FLAG_TC));
}

void UART1_SendBytes(unsigned char  *Data, u8 num)
{
    u8 i = 0;
    for (i = 0; i < num; i++)	  UART1_SendByte(*(Data + i));
}

extern void USART1_Configuration(void);

bool_t UserUARTinitialize(void)
{
     UART1_Init();
     USART1_Configuration();
    return 1;
}

void USART1_IRQHandler(void)
{
    BYTE cData;
    //if(uart_data_in_end == 0)
    //{
    //rxdelay_USART1 = 40;

    //g_wUSART1_TIMER = mS_TO_TICKS(100);  // 100ms
    g_wUSART1_TIMER = mS_TO_TICKS(50);  // 100ms
    g_bUSART1_DataReceived = TRUE;  //uart_data_in_start = 1;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        cData = USART1->DR;

        if (USART_data_flag == 0)
        {
            if (USART1_rxpoint < (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                *USART1_rxpoint++ = cData;

            if (USART1_rxpoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            {
                USART1_rxpoint = USART1_RX_BUFF;
                IWDG_ReloadCounter(); //ι��
            }
        }
        else
        {
            //  1 EDID���ڸ���ָ��
            if (USART_EDIDdata_count < EDID_TABLE_LEN) ////[kyq add: 20150901 20:12:34]
            {
                USART_EDIDdata[USART_EDIDdata_count] = cData;
                USART_EDIDdata_count++;
            }
        }
    }
    //else  UART_temp = USART1->DR;  //�������ж��ǵ���������ݺ�Ӳ���Զ�������ж�

    if (USART_GetITStatus(USART1, USART_IT_ORE) != RESET)
    {
        USART_ClearFlag(USART1, USART_IT_ORE);
        cData = USART1->DR;  //�������ж��ǵ���������ݺ�Ӳ���Զ�������ж�
    }

}

void ClearUartInEdidValue(void)
{
	unsigned char i = 0;
	for(i=0;i<MAX_IN;i++)
	{
		UartInEdidValue[i]=0xff;
	}
}

/*
port  ��0��ʼ
value Ϊ����edid�ı�Ŵ�0--11
*/
void SetUartInEdidValue(unsigned char port,unsigned char value)
{
	UartInEdidValue[port]=value;
        KeyValue = eEDID_RS232_GUI_CONTROL;
}

void GetUartInEdidValue(unsigned char *p)
{
	unsigned char i = 0;
	for(i=0;i<MAX_IN;i++)
	{
		*p = UartInEdidValue[i];
		p++;
	}
}



/**********************************************************************************************************
* �������ܣ� ��str1�в���str2������ҵ����򷵻�str1��str2���һ����ĸ����һ����ַ�����򷵻�NULL
* ���������
* ���������
* ����ֵ  ��
**********************************************************************************************************/
// �������ֻ�ܺʹ��ڽ��յ����ݱȽ�
//
#if(MACHINETYPE==PRD_VHD4)
// ֻ�ȽϿ�ͷ����
unsigned char* StrCmp(unsigned char *str1, unsigned char *str2, unsigned char *USART1_rxprocesspoint) //USART1_rxprocesspoint ��βָ��
{
    WORD strlength1,strlength2, i;
    strlength1 = strlen((char *)str1);
    strlength2 = strlen((char *)str2);

    for (i = 0 ; (i< strlength2)&&(str1 < USART1_rxprocesspoint) ; i++)
    {
        if (*str1 == *str2)
        {
              str1++;
              str2++;
        }
        else
            break;
    }
    if (i==strlength2)
    {
      return str1;
    }
    return NULL;
}
#else
unsigned char* StrCmp(unsigned char *str1, unsigned char *str2, unsigned char *USART1_rxprocesspoint) //USART1_rxprocesspoint ��βָ��
{
    WORD strlength, i;

    strlength = strlen((char *)str2);
    while (str1 != USART1_rxprocesspoint)
    {
        if (*str1 == *str2)
        {
            for (i = 0; i < strlength; i++)
            {
                if (*str1 == *str2)
                {
                    str1++;
                    if (str1 >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        str1 = USART1_RX_BUFF;
                    str2++;
                }
                else
                    return NULL;
            }
            return str1;
        }
        else
        {
            str1++;
            if (str1 >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            {
                str1 = USART1_RX_BUFF;
                break; // Error
            }
        }
    }
    return NULL;
}
#endif

//         begin
// stR1: P  P  P   W   O   N  .
// str2:      P  W  O     N.
BOOL  StrCmp2(unsigned char *str1, unsigned char *str2, unsigned char *USART1_rxEnd) //USART1_rxprocesspoint ��βָ��
{
    unsigned char *str1_begin = str1;
    //unsigned char *str2_begin  = str2;
    while (str1_begin < USART1_rxEnd)
    {
        if (StrCmp(str1, str2, USART1_rxEnd) == NULL)
        {
            str1_begin++;
            str1 = str1_begin;
            //str2 = str2_begin;
        }
        else
        {
            return TRUE;
        }

    }
    return FALSE;
}


/*
** HEX��ASCII֮���ת�� wf8421 20160815
*/
unsigned char HexAndAsciiChange(unsigned char cvalue,unsigned char cmode)  // cmode : 1 Hex to Ascii    0: Ascii to Hex
{
    unsigned char reValue=0;

    if(cmode == 1)
    {
        if((cvalue&0x0F)>=0&&(cvalue&0x0F)<=9)
        {
            reValue=cvalue+'0';
        }
        else if((cvalue&0x0F)>=0x0A&&(cvalue&0x0F)<=0x0F)
        {
            reValue=cvalue-0x0A+'A';
        }
        else
        {
            reValue=cvalue;
            //printf("Change error: %x\n",cvalue);
        }
    }
    else
    {
        if(((cvalue)>='0'&&(cvalue)<='9'))
        {
            reValue=cvalue-'0';
        }
        else if((cvalue)>='A'&&(cvalue)<='F')
        {
            reValue=cvalue+0x0A-'A';
        }
        else if((cvalue)>='a'&&(cvalue)<='f')
        {
            reValue=cvalue+0x0A-'a';
        }
        else
        {
            reValue=cvalue;
            //printf("Change error: %x\n",cvalue);
        }
    }

    return reValue;
}

//�ӵ�128�ֽڿ�ʼ�����Ƶ�л���Ϣ   F0 ~ F9  ÿ�� 16�ֽ� ��160�ֽ�
//�ӵ�288�ֽڿ�ʼ�����Ƶ�л���Ϣ   F0 ~ F9  ÿ�� 116�ֽ� ��160�ֽ�



unsigned char EDID_GetDataBlockAddr(unsigned char *pTarget, unsigned char Tag, unsigned char Start)
{
    unsigned int i = 0;
    if (pTarget[126] >= 0x01)
    {
        for (i = Start; i < (pTarget[130] + 128); ++i)
        {
            if ((pTarget[i] & 0xE0) == Tag)  // find tag code
            {
                return i;
            }
            else
            {
                i += (pTarget[i] & 0x1F);
            }
        }
        if (i >= (pTarget[130] + 128))  // Error, can not find
        {
            return 0;
        }
    }
    return 0;
}

// ��������ڵ�EDID
void do_with_EDID_UP(void)
{
    unsigned char j = 0, crc = 0/*, ediden = 0*/;
    unsigned int i = 0;

    if ((USART_EDIDdata_count == 128) || (USART_EDIDdata_count == 256))
    {
        for (i = 0; i < USART_EDIDdata_count; i++) crc += USART_EDIDdata[i];
        if (crc == 0)
        {

            if ((EDID_updata_port > 0) && (EDID_updata_port < edsEDID_INTERN1))
            {

                if (TRUE)
                {
                    appSystemInputPortEdidDataSet(EDID_updata_port - 1, (eEDIDSELECT)(edsEDID_IN1 + EDID_updata_port - 1), USART_EDIDdata);
                    IWDG_ReloadCounter(); //ι��
                }
                UART1_SendBytes("EDID Upgrade OK!\r\n", strlen("EDID Upgrade OK!\r\n"));
            }
            else if (EDID_updata_port == edsEDID_INTERN1)
            {
		  appSystemInputPortEdidSet(0, edsEDID_EXTERN1, USART_EDIDdata,ALL_INPORT);
                UART1_SendBytes("EDID Upgrade OK!\r\n", strlen("EDID Upgrade OK!\r\n"));
            }
            else
                UART1_SendBytes("EDID Upgrade Fail!\r\n", strlen("EDID Upgrade Fail!\r\n"));
        }
        else
            UART1_SendBytes("EDID Upgrade Fail!\r\n", strlen("EDID Upgrade Fail!\r\n"));
    }
    else
        UART1_SendBytes("EDID Upgrade Fail!\r\n", strlen("EDID Upgrade Fail!\r\n"));
}

//������EDID ����
void do_with_int_EDID_UP(void)
{
    unsigned char crc = 0;
    unsigned int i = 0;
    unsigned char g_tempData[EDID_TABLE_LEN]={0};
    if ((USART_EDIDdata_count == 128) || (USART_EDIDdata_count == 256))
    {
        for (i = 0; i < USART_EDIDdata_count; i++) crc += USART_EDIDdata[i];
        if (crc == 0)
        {
            if (EDID_updata_port <= (edsEDID_EXTERN1-edsEDID_INTERN1))
            {
                //write_int_EDID(EDID_updata_port, USART_EDIDdata);
				dvNVRAMUserEdidWrite(edsEDID_INTERN1 + EDID_updata_port - 1, USART_EDIDdata);
				UART1_SendBytes("IntEDID Upgrade OK!\r\n", strlen("IntEDID Upgrade OK!\r\n"));
			    //��ȡĿǰ��Ӧ��Ĭ��EDID����
				dvNVRAMUserEdidRead(edsEDID_INTERN1 + EDID_updata_port - 1,g_tempData);
			    //�˺�������Щ������෢��128λ��Ϊ�˼���256bytes ��2�η���
				UART1_SendBytes(g_tempData,EDID_TABLE_LEN>>1);
				UART1_SendBytes(g_tempData+(EDID_TABLE_LEN>>1),EDID_TABLE_LEN>>1);
            }
            else
                UART1_SendBytes("IntEDID Upgrade Fail!\r\n", strlen("IntEDID Upgrade Fail!\r\n"));
        }
        else
            UART1_SendBytes("IntEDID Upgrade Fail!\r\n", strlen("IntEDID Upgrade Fail!\r\n"));
    }
    else
        UART1_SendBytes("IntEDID Upgrade Fail!\r\n", strlen("IntEDID Upgrade Fail!\r\n"));
}


unsigned char get_ip_add(unsigned char *pp, unsigned char *data)
{
    unsigned char i = 0;
    unsigned char IP_add_temp[4] = { 0, 0, 0, 0 };
    pp += 1;
    while ((*pp != '.') && (*pp != 0xfe))
    {
        IP_add_temp[0] = IP_add_temp[0] * 10 + (*pp - 0x30);
        pp++;
    }
    //if (IP_add_temp[0] > 255)
    //    return 0;
    pp++;
    while ((*pp != '.') && (*pp != 0xfe))
    {
        IP_add_temp[1] = IP_add_temp[1] * 10 + (*pp - 0x30);
        pp++;
    }
    //if (IP_add_temp[1] > 255)
    //    return 0;
    pp++;
    while ((*pp != '.') && (*pp != 0xfe))
    {
        IP_add_temp[2] = IP_add_temp[2] * 10 + (*pp - 0x30);
        pp++;
    }
    //if (IP_add_temp[2] > 255)
    //    return 0;
    pp++;
    while ((*pp != '.') && (*pp != 0xfe))
    {
        IP_add_temp[3] = IP_add_temp[3] * 10 + (*pp - 0x30);
        pp++;
    }
    //if (IP_add_temp[3] > 255)
    //    return 0;

    for (i = 0; i < 4; i++) data[i] = IP_add_temp[i];
    return 1;
}



//------------------Begain Process the uart struction([X1]V[X2])-------------------------
#ifndef SWITCH_CMD_COTINUE
void ProessVedioSwitch(BYTE cInPort, BYTE cOutPort, SWITCH_e eFlag)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);
	if(eSWITCH_OFF == eFlag)
	{
	        cOutPort = cOutPort - '0' - 1;
	        cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d Closed.", cOutPort + 1);
	        appUIUartMessageSet(NULL, acMessage2, TRUE);
	        appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulClose);
	        appUIStateChange(uiUART_SWITCH_VIDEO);
	        appSystemSwitcherSet(cInPort, 1, cOutPort);
	}
	else if(eSWITCH_ON == eFlag)
	{
	        cOutPort = cOutPort - '0' - 1;
            //cCount += SiiSprintf(&acMessage2[cCount], "AV: 01->%2d", cOutPort + 1);
            cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: %02d->%2d", cInPort + 1, cOutPort + 1);
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
            appUIStateChange(uiUART_SWITCH_VIDEO);
            appSystemSwitcherSet(cInPort, 1, cOutPort);
	}
}
#else
void ProessVedioSwitch(BYTE cInPort, unsigned int i, PBYTE acOutPortList)
{        Event_t   bEvent;
	   BYTE cCount = 0,cCount1 = 0,OutPut=0;
	   BYTE acMessage1[LCD_MAX_CHAR] = { 0 };
	   BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	   memset(acMessage1, 0, LCD_MAX_CHAR);
	   memset(acMessage2, 0, LCD_MAX_CHAR);

	   if(i<MAX_OUT)				// LCD����ʾ��Ϣ
	   {
	          if(cInPort+1>MAX_IN)
	          {
	            		cCount += SiiSprintf((char*)&acMessage1[cCount], "AV:OFF->");
	          }
	          else
	          {
	            		cCount += SiiSprintf((char*)&acMessage1[cCount], "AV: %2d->", cInPort+1);
	          }
	          for(OutPut=0; OutPut<i; OutPut++)
	          {
				if(OutPut<=2)
				{
					cCount += SiiSprintf((char*)&acMessage1[cCount], "%2d,",acOutPortList[OutPut]+1);
				}
				else
				{
					cCount1 += SiiSprintf((char*)&acMessage2[cCount1], ",%2d",acOutPortList[OutPut]+1);
				}
	          }
		   if(cCount1==0)
		   {
				acMessage1[cCount-1] = ' ';		//ȥ����һ�е����һ������
				appUIUartMessageSet(NULL, acMessage1, FALSE);
		   }
		   else
		   {
				acMessage1[cCount-1] = ' ';		//ȥ����һ�е����һ������
				appUIUartMessageSet(acMessage1, acMessage2, FALSE);
		   }
          	   appUartSwitchLedStateChange(cInPort, cInPort, ulAllThrough);
          	   appUIStateChange(uiUART_SWITCH_VIDEO);
	   }
	   else
	   {
		   cCount=0;
            	   if(cInPort+1>MAX_IN)
                 {
                		cCount += SiiSprintf((char*)&acMessage2[cCount], "OFF To All.");
                 }
            	   else
            	   {
                		cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d To All.", cInPort+1);
            	   }
            	   appUIUartMessageSet(NULL, acMessage2, FALSE);
          	   appUartSwitchLedStateChange(cInPort, cInPort, ulAllThrough);
          	   appUIStateChange(uiUART_SWITCH_VIDEO);
	   }

      	   //Event_t   bEvent;
	   bEvent.Head.opcode=eLEDChannelSwitch;
	   bEvent.Head.DestAddr=mqLED;
	   bEvent.args[0]=cInPort;
	   bEvent.args[1]=i;
	   memcpy(&bEvent.args[2],acOutPortList,i);
	   bEvent.Head.argCount=i+2;
	   utilExecOlnyMsgSend(mqLED,bEvent);

          //***********************************************
          bEvent.Head.opcode=eSwitchxTox;
	   bEvent.Head.DestAddr=mqSWITCH;
	   bEvent.args[0]=cInPort;
	   bEvent.args[1]=i;
	   memcpy(&bEvent.args[2],acOutPortList,i);
	   bEvent.Head.argCount=i+2;
	   utilExecOlnyMsgSend(mqSWITCH, bEvent);
          //***************************************************
}
#endif
//-----------------End Process the uart struction([X1]V[X2])---------------------------


//-----------------Begain Process the uart struction([X1]B[X2])------------------------
#ifndef SWITCH_CMD_COTINUE
void ProessVedioIRSwitch(BYTE cInPort, BYTE cOutPort, SWITCH_e eFlag)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);
	if(eSWITCH_OFF == eFlag)
	{
	        cOutPort = cOutPort - '0' - 1;
	        cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d Closed.", cOutPort + 1);
	        appUIUartMessageSet(NULL, acMessage2, TRUE);
	        appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulClose);
	        appUIStateChange(uiUART_SWITCH_VIDEO);
	        appSystemSwitcherSet(cInPort, 1, cOutPort);
		 #ifdef PRD_IR_OUT
	        appSystemIRSwitcherSet(cInPort, 1, cOutPort);
		 #endif
	}
	else if(eSWITCH_ON == eFlag)
	{
	        cOutPort = cOutPort - '0' - 1;
            //cCount += SiiSprintf(&acMessage2[cCount], "AV: 01->%2d", cOutPort + 1);
            cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: %02d->%2d", cInPort + 1, cOutPort + 1);
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
            appUIStateChange(uiUART_SWITCH_VIDEO);
            appSystemSwitcherSet(cInPort, 1, cOutPort);
	     #ifdef PRD_IR_OUT
            appSystemIRSwitcherSet(cInPort, 1, cOutPort);
	     #endif
	}
}
#else
void ProessVedioIRSwitch(BYTE cInPort, unsigned int i, PBYTE acOutPortList)
{
            Event_t   bEvent;

                //  appUartSwitchInfoSet(cInPort-1, i, acOutPortList,TRUE);
             //   appSystemSwitcherPortSet(cInPort-1, i, acOutPortList); // 3B
                            /****************/
		//Event_t   bEvent;
		bEvent.Head.opcode=eLEDChannelSwitch;
		bEvent.Head.DestAddr=mqLED;
		bEvent.args[0]=cInPort-1;
	       bEvent.args[1]=i;
		memcpy(&bEvent.args[2],acOutPortList,i);
		bEvent.Head.argCount=i+2;
	       utilExecOlnyMsgSend(mqLED,
                            bEvent);
               /****************/
            //***********************************************
              bEvent.Head.opcode=eSwitchxTox;
		bEvent.Head.DestAddr=mqSWITCH;
		bEvent.args[0]=cInPort-1;
	       bEvent.args[1]=i;
		memcpy(&bEvent.args[2],acOutPortList,i);
		bEvent.Head.argCount=i+2;
	       utilExecOlnyMsgSend(mqSWITCH,
                            bEvent);
          //***************************************************
          #ifdef PRD_IR_OUT
               appSystemIRSwitcherPortSet(cInPort-1, i, acOutPortList);
	   #endif
}
#endif
//----------------End Process the uart struction([X1]B[X2])----------------------------


//----------------Begain Process the uart struction([X1]R[X2])-------------------------
#ifndef SWITCH_CMD_COTINUE
void ProessIRSwitch(BYTE cInPort, BYTE cOutPort, SWITCH_e eFlag)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);
	if(eSWITCH_OFF == eFlag)
	{
	        cOutPort = cOutPort - '0' - 1;
            cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d Closed.", cOutPort + 1);
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulClose);
            appUIStateChange(uiUART_SWITCH_VIDEO);
	     #ifdef PRD_IR_OUT
            appSystemIRSwitcherSet(cInPort, 1, cOutPort);
	     #endif
	}
	else if(eSWITCH_ON == eFlag)
	{
	        cOutPort = cOutPort - '0' - 1;
            //cCount += SiiSprintf(&acMessage2[cCount], "IR: 01->%2d", cOutPort + 1);
            cCount += SiiSprintf((char*)&acMessage2[cCount], "IR: %02d->%2d", cInPort + 1, cOutPort + 1);
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
            appUIStateChange(uiUART_SWITCH_VIDEO);
	     #ifdef PRD_IR_OUT
            appSystemIRSwitcherSet(cInPort, 1, cOutPort);
	     #endif
	}
}
#else
void ProessIRSwitch(BYTE cInPort, unsigned int i, PBYTE pOutPortList)
{
        appUartSwitchInfoSet(cInPort, i, pOutPortList,FALSE);
	 #ifdef PRD_IR_OUT
        appSystemIRSwitcherPortSet(cInPort, i, pOutPortList);
	 #endif
}
#endif
//---------------End Process the uart struction([X1]R[X2])-----------------------------


//---------------Begain Process the uart struction(/+[Y]/[X]:******.)-----------------------------------------
//---------------This API only send data to remote when the state of the product is POWON or POWOFF-----------
#ifdef PRD_POWER_ONOFF_MESSAGE
void ProcessSendData2Remote(BYTE cRemote, BYTE cBraud, unsigned char* puart_data_temp, unsigned int count, BYTE cbPowrON)
{
	unsigned int i = 0;
	PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
    if (cbPowrON)
    {

        psSystemSettingData->acPowerOnMsgBaudRate[cRemote] = cBraud;
        psSystemSettingData->acPowerOnMsgLength[cRemote] = count;
        for (i = 0; i < count; i++)
        {
            psSystemSettingData->acPowerOnMsg[cRemote][i] = puart_data_temp[i];
        }
        psSystemSettingData->acPowerOnMsg[cRemote][i] = '\0';

    }
    else
    {
        psSystemSettingData->acPowerOffMsgBaudRate[cRemote] = cBraud;
        psSystemSettingData->acPowerOffMsgLength[cRemote] = count;
        for (i = 0; i < count; i++)
        {
            psSystemSettingData->acPowerOffMsg[cRemote][i] = puart_data_temp[i];
        }
        psSystemSettingData->acPowerOffMsg[cRemote][i] = '\0';
    }
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);

    UART1_SendData2PC(puart_data_temp, count);
}

#endif
//---------------End Process the uart struction(/+[Y]/[X]:******.)----------------------------------------------


//---------------End Process the uart struction(/+[Y]/[X]:******.)----------------------------------------------

#if 1
//---------------Begain Process the uart struction(All#.)---------------------------
void ProcessAllThrough(void)
{
   BYTE cCount = 0;
   BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
   //memset(acMessage2, 0, LCD_MAX_CHAR);
   Event_t   bEvent;

   bEvent.Head.opcode=eSwitchAllThrough;
   bEvent.Head.DestAddr=mqSWITCH;
   bEvent.Head.argCount=0;
   utilExecOlnyMsgSend(mqSWITCH,bEvent);

   bEvent.Head.opcode=eLEDAllThrough;
   bEvent.Head.DestAddr=mqLED;
   bEvent.Head.argCount=0;
   utilExecOlnyMsgSend(mqLED,bEvent);

   cCount += SiiSprintf(&acMessage2[cCount],"%s", "All Through."); 		//��LCD��ʾ����ʾ
   appUIUartMessageSet(NULL, acMessage2, FALSE);
   appUartSwitchLedStateChange(1, 1, ulAllThrough);
   appUIStateChange(uiUART_SWITCH_VIDEO);

    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    {
        BYTE tempLed[8]={0,1,2,3,4,5,6,7};
        KeyLedSetAll(tempLed);
    }
    #endif
}

#else
//---------------Begain Process the uart struction(All#.)---------------------------
void ProcessAllThrough(unsigned char InPut, unsigned char OutPut)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);
	Event_t   bEvent;

	bEvent.Head.opcode=eSwitchAllThrough;
	bEvent.Head.DestAddr=mqSWITCH;
	bEvent.Head.argCount=0;
	utilExecOlnyMsgSend(mqSWITCH,bEvent);

	bEvent.Head.opcode=eLEDAllThrough;
	bEvent.Head.DestAddr=mqLED;
	bEvent.Head.argCount=0;
	utilExecOlnyMsgSend(mqLED,bEvent);

    cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "All Through.");
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUartSwitchLedStateChange(InPut, OutPut, ulAllThrough);
    appUIStateChange(uiUART_SWITCH_VIDEO);
    #if (MACHINETYPE == MUH4K862)
    {
        BYTE tempLed[8]={0,1,2,3,4,5,6,7};
        KeyLedSetAll(tempLed);
    }
	#endif
}
#endif
//---------------End Process the uart struction(All#.)------------------------------


//---------------Begain Process the uart struction(All$.)---------------------------
void ProcessAllClose(void)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);

    appSystemSwitcherOutportCloseAll(); // video and IR
    cCount += SiiSprintf((char*)&acMessage2[cCount], "%s","All Closed.");
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUartSwitchLedStateChange(1, 1, ulAllClose);
    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    KeyLedSetAllClose();
    #endif
    appUIStateChange(uiUART_SWITCH_VIDEO);
}
//---------------End Process the uart struction(All$.)------------------------------


//---------------Begain Process the uart struction(All@.)---------------------------
void ProcessAllOpen(BOOL Re_Message)
{
	appSystemSwitcherOutportRecoverAll();// Video and IR
    if (Re_Message == ON)
    {
        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
        KeyLedStatuesInitialize();
        #endif
        appUIUartMessageSet(NULL, "All Open.", TRUE);
        appUartSwitchLedStateChange(1, 1, ulAllOpen);
        appUIStateChange(uiUART_SWITCH_VIDEO);
    }
    else
    {
        appUIUartMessageSet(NULL, "Switch Ok!", TRUE);
        appUartSwitchLedStateChange(1, 1, ulAllOpen);
        appUIStateChange(uiUART_SWITCH_VIDEO);
    }
    //delay_nms(10);
}
//---------------End Process the uart struction(All@.)------------------------------


//---------------Begain Process the uart struction([X]All.)-------------------------
void ProcessOne2All(unsigned char InPut)
{         Event_t   bEvent;
	    BYTE cCount = 0;
	    BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
          /*********************************/
		bEvent.Head.opcode=eSwitchxAll;
		bEvent.Head.DestAddr=mqSWITCH;
		bEvent.args[0]=InPut-1;
		bEvent.Head.argCount=1;
	       utilExecOlnyMsgSend(mqSWITCH,
                            bEvent);
               /****************/
	            /*********************************/
		bEvent.Head.opcode=eLEDSwitchxAll;
		bEvent.Head.DestAddr=mqLED;
		bEvent.args[0]=InPut-1;
		bEvent.Head.argCount=1;
	       utilExecOlnyMsgSend(mqLED,
                            bEvent);
               /****************/
		//appSystemSwitcherSet(InPut - 1, MAX_OUT, 0, 1, 2, 3,4,5,6,7); // xAll.
#ifdef PRD_IR_OUT
		appSystemIRSwitcherSet(InPut - 1, MAX_OUT, 0, 1, 2, 3,4,5,6,7);
#endif

		cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d To All.", InPut);
		appUIUartMessageSet(NULL, acMessage2, FALSE);
		appUartSwitchLedStateChange(InPut, InPut, ulToAll);
		appUIStateChange(uiUART_SWITCH_VIDEO);
}
//---------------End Process the uart struction([X]All.)----------------------------

//---------------End Process the uart struction([X]All.)----------------------------

#if 1
//---------------Begain Process the uart struction([X]#.)---------------------------
void ProcessOne2One(unsigned char InPut, unsigned char OutPut)
{
    Event_t   bEvent;
    BYTE cCount = 0;
    BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
    //memset(acMessage2, 0, LCD_MAX_CHAR);

#ifdef PRD_IR_OUT
    appSystemIRSwitcherSet(InPut - 1, 1, OutPut - 1);
#endif
         //Event_t   bEvent;
    bEvent.Head.opcode=eLEDChannelSwitch;
    bEvent.Head.DestAddr=mqLED;
    bEvent.args[0]=InPut-1;
    bEvent.args[1]=1;
    memset(&bEvent.args[2],OutPut - 1,1);
    bEvent.Head.argCount=2+1;
    utilExecOlnyMsgSend(mqLED, bEvent);
    /****************/
    //***********************************************
    bEvent.Head.opcode=eSwitchxThrough;
    bEvent.Head.DestAddr=mqSWITCH;
    bEvent.args[0]=InPut-1;
    bEvent.Head.argCount=1;
    utilExecOlnyMsgSend(mqSWITCH, bEvent);
    //***************************************************
     //appSystemSwitcherSet(InPut - 1, 1, OutPut - 1);
     cCount += SiiSprintf(&acMessage2[cCount], "%2d Through.", InPut);
     appUIUartMessageSet(NULL, acMessage2, FALSE);
     appUartSwitchLedStateChange(InPut, InPut, ulThrough);
     appUIStateChange(uiUART_SWITCH_VIDEO);
#if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    KeyLedSetOne(OutPut-1,InPut-1);
#endif
}

#else
//---------------Begain Process the uart struction([X]#.)---------------------------
void ProcessOne2One(unsigned char InPut, unsigned char OutPut)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);

    appSystemSwitcherSet(InPut - 1, 1, OutPut - 1);
    cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d Through.", InPut);
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUartSwitchLedStateChange(InPut, InPut, ulThrough);
    appUIStateChange(uiUART_SWITCH_VIDEO);
    #if (MACHINETYPE == MUH4K862)
    KeyLedSetOne(OutPut-1,InPut-1);
    #endif
}
#endif
//---------------End Process the uart struction([X]#.)------------------------------


void SwitchHDMIVersionOneClose(unsigned char OutPut)
{
	POUTPUTPORT_MANAGER_DATA	psOutputPortMgrData;

    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    if ((psOutputPortMgrData->acInPortSelect[OutPut - 1]) < MAX_IN)
        psOutputPortMgrData->acCloseInPortSelect[OutPut - 1] = psOutputPortMgrData->acInPortSelect[OutPut - 1];

    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    appSystemSwitcherSet(MAX_IN, 1, OutPut - 1);
}

void SwitchHDMIVersionOneOpen(unsigned char OutPut)
{
	POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;

	psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);

	if ((psOutputPortMgrData->acInPortSelect[OutPut - 1]) >= MAX_IN)
	{
	    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,TRUE);
	    appSystemSwitcherSet(psOutputPortMgrData->acCloseInPortSelect[OutPut - 1], 1, OutPut - 1);
	}
	else
	    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,TRUE);
}


//---------------Begain Process the uart struction([X]$.)---------------------------
void ProcessOneClose(unsigned char OutPut)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);
	POUTPUTPORT_MANAGER_DATA	psOutputPortMgrData;

    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    if ((psOutputPortMgrData->acInPortSelect[OutPut - 1]) < MAX_IN)
        psOutputPortMgrData->acCloseInPortSelect[OutPut - 1] = psOutputPortMgrData->acInPortSelect[OutPut - 1];
#ifdef PRD_IR_OUT
    if ((psOutputPortMgrData->acIRInPortSelect[OutPut - 1]) < MAX_IN)
        psOutputPortMgrData->acIRCloseInPortSelect[OutPut - 1] = psOutputPortMgrData->acIRInPortSelect[OutPut - 1];
#endif

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    KeyLedSetOne(OutPut-1,0xFF);
    KeyLedCloseSetOne(OutPut-1,psOutputPortMgrData->acCloseInPortSelect[OutPut - 1]);
    Set_CloseOutputFlag(OutPut-1,true);
#endif
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    appSystemSwitcherSet(MAX_IN, 1, OutPut - 1);
#ifdef PRD_IR_OUT
    appSystemIRSwitcherSet(MAX_IN, 1, OutPut - 1);
#endif

    cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d Closed.", OutPut);
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUartSwitchLedStateChange(OutPut, OutPut, ulClose);
    appUIStateChange(uiUART_SWITCH_VIDEO);
}
//---------------End Process the uart struction([X]$.)------------------------------


//---------------Begain Process the uart struction([X]@.)---------------------------
void ProcessOneOpen(unsigned char OutPut, BOOL Re_Message)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);
	POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;

	psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);


	if ((psOutputPortMgrData->acInPortSelect[OutPut - 1]) >= MAX_IN)
	{
	    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,TRUE);
	    appSystemSwitcherSet(psOutputPortMgrData->acCloseInPortSelect[OutPut - 1], 1, OutPut - 1);
	#ifdef PRD_IR_OUT
	appSystemIRSwitcherSet(psOutputPortMgrData->acIRCloseInPortSelect[OutPut - 1], 1, OutPut - 1);
	#endif

        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
        KeyLedSetOne(OutPut-1,psOutputPortMgrData->acInPortSelect[OutPut - 1]);
        Set_CloseOutputFlag(OutPut-1,false);
        #endif
	}
	else
	    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,TRUE);

	if (Re_Message == ON)
	{
	    cCount += SiiSprintf((char*)&acMessage2[cCount], "%02d Open.", OutPut);
	    appUIUartMessageSet(NULL, acMessage2, TRUE);
	    appUartSwitchLedStateChange(OutPut, OutPut, ulOpen);
	    appUIStateChange(uiUART_SWITCH_VIDEO);
	}
	else
	{
	    cCount += SiiSprintf((char*)&acMessage2[cCount], "Switch Ok!");
	    appUIUartMessageSet(NULL, acMessage2, TRUE);
	    appUartSwitchLedStateChange(OutPut, OutPut, ulOpen);
	    appUIStateChange(uiUART_SWITCH_VIDEO);
	}
    //delay_nms(10);
}
//---------------End Process the uart struction([X]@.)------------------------------


//---------------Begain Process the uart struction(Save[Y].)------------------------
void ProcessSaveCurrentStatus(unsigned char InPut)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);

    cCount += SiiSprintf((char*)&acMessage2[cCount], "Save To F%d", InPut);
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUIStateChange(uiUART_MESSAGE);
    appSystemSwitcherScenceSave(InPut-1);
}
//---------------End Process the uart struction(Save[Y].)---------------------------


//---------------Begain Process the uart struction(Recall[Y].)----------------------
void ProcessRecallStatus(unsigned char InPut)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);

    cCount += SiiSprintf((char*)&acMessage2[cCount], "Recall From F%d", InPut);
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUIStateChange(uiUART_MESSAGE);

    appSystemSwitcherScenceCall(InPut-1);
}
//---------------End Process the uart struction(Recall[Y].)-------------------------


//--------------Begain Process the uart struction(Clear[Y].)------------------------
void ProcessClearStatus(unsigned char InPut)
{
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);

    cCount += SiiSprintf((char*)&acMessage2[cCount], "Clear F%d", InPut);
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUIStateChange(uiUART_MESSAGE);

    appSystemSwitcherScenceDel(InPut-1);
}
//-------------End Process the uart struction(Clear[Y].)----------------------------


//-------------Begain Process the uart struction(I-Lock[x].)-------------------------
uint8_t ProcessLockOneChannel(unsigned char InPut)
{
	BYTE cInPort = 0;
	unsigned int i;
	PSYSTEM_SETTING_DATA   psSystemSettingData;
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;

	psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
	psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
	if ((InPut >0) && (InPut <= MAX_IN ))             // �鿴һ��ͨ��״̬
    {
		psSystemSettingData->acIChannellock[InPut - 1]=0x00;		//ˢ�¸�������ͨ��
		for(i=0;i<MAX_OUT;i++)
    		{
    			cInPort = psOutputPortMgrData->acInPortSelect[i];				//�ж�ÿ������ڵ�ǰ������źţ�����Ǳ����������¼
			if(cInPort!=InPut-1)
			{
		 		psSystemSettingData->acIChannellock[InPut - 1] = psSystemSettingData->acIChannellock[InPut - 1]|BIT(i);	//0x0 xxxx
                		u8IChannellock[InPut - 1]=psSystemSettingData->acIChannellock[InPut - 1];
			}
    		}

		utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,  TRUE);
		utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,  FALSE);
		sprintf(USART1_tx_buff,"Channel %d Lock!\r\n",InPut);
		UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
    		return (CH_CORRECT);
    }
    else
    {   utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,FALSE);
		return (CH_ERROR);
    }
}
//-------------End Process the uart struction(I-Lock[x].)----------------------------


//-------------Begain Process the uart struction(I-Unlock[x].)--------------------------
uint8_t ProcessUnlockOneChannel(unsigned char InPut)
{
	PSYSTEM_SETTING_DATA   psSystemSettingData;
	psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
    if ((InPut > 0) && (InPut <=MAX_IN ))
    {
		psSystemSettingData->acIChannellock[InPut - 1] =0x00; //0x0 xxxx
        u8IChannellock[InPut - 1]=psSystemSettingData->acIChannellock[InPut - 1];
    	utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,TRUE);
	 	sprintf(USART1_tx_buff,"Channel %d Unlock!\r\n",InPut);
        UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
        return (CH_CORRECT);
    }
    else
    {
		utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,FALSE);
		return (CH_ERROR);
    }
}
//-------------End Process the uart struction(I-Unlock[x]..)----------------------------


//-------------Begain Process the uart struction(A-Lock.)--------------------------
void ProcessLockAllChannel(void)
{
	BYTE cInPort = 0;
	unsigned int i = 0, j = 0;
	PSYSTEM_SETTING_DATA   psSystemSettingData;
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;

	psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
	psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
	for(i=0;i<MAX_IN;i++)
    {
	 	psSystemSettingData->acIChannellock[i]=0x00;
		cInPort=i;
		for(j=0;j<MAX_OUT;j++)
	    {
	    	if(cInPort != psOutputPortMgrData->acInPortSelect[j])//�ж�ÿ������ڵ�ǰ������źţ�����Ǳ����������¼
			{
                psSystemSettingData->acIChannellock[i] = psSystemSettingData->acIChannellock[i]|(BIT(j));
                u8IChannellock[i]=psSystemSettingData->acIChannellock[i];
	    	}
	    }

    }
    sprintf(USART1_tx_buff,"All Channel Lock!\r\n");
    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,  false);
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,TRUE);
}
//-------------End Process the uart struction(A-Lock.)----------------------------


//-------------Begain Process the uart struction(A-UnLock.)--------------------------
void ProcessUnlockAllChannel(void)
{
	 unsigned int i = 0;
	 PSYSTEM_SETTING_DATA   psSystemSettingData;

	 psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
	 for(i=0;i<MAX_IN;i++)
	 {
		psSystemSettingData->acIChannellock[i]=0x00;
        u8IChannellock[i]=psSystemSettingData->acIChannellock[i];
	 }
	 sprintf(USART1_tx_buff,"All Channel Unlock!\r\n");
	 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
	 utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,TRUE);
}
//-------------End Process the uart struction(A-UnLock.)----------------------------


//-------------Begain Process the uart struction(Lock-Sta.)--------------------------
void ProCheckAllLockStatus(void)
{
	 unsigned int i = 0, j = 0;
	 PSYSTEM_SETTING_DATA   psSystemSettingData;

     psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
	 for(j=0;j<MAX_OUT;j++)
	 {
		for(i=0;i<MAX_IN;i++)
		{
			if( (psSystemSettingData->acIChannellock[i]&BIT(j)))
			{
				sprintf(USART1_tx_buff,"Channel %d->%d Lock!\r\n",i+1,j+1);
        		UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
			}
			else
			{
				sprintf(USART1_tx_buff,"Channel %d->%d Unlock!\r\n",i+1,j+1);
        		UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
			}
		}

	}
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
}
//-------------End Process the uart struction(Lock-Sta.)----------------------------


//-------------Begain Process the uart struction(Demo.)--------------------------
void ProSettingDemoStatus(void)
{
	 Event_t   bEvent;
	 BYTE cCount = 0;
	 BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	 memset(acMessage2, 0, LCD_MAX_CHAR);

     if (bDemoEn)
         cCount += SiiSprintf((char*)&acMessage2[cCount], "Normal Mode");
     else
         cCount += SiiSprintf((char*)&acMessage2[cCount], "Demo Mode");

     UART1_SendData2PC(acMessage2, cCount);
     appUIUartMessageSet(NULL, acMessage2, FALSE);//[kyq add: 20150924 15:49:38]
     appUIStateChange(uiUART_MESSAGE);

	 bEvent.Head.opcode=eSwitchDemoMode;
	 bEvent.Head.DestAddr=mqSWITCH;
	 bEvent.args[0]=!bDemoEn;
	 bEvent.Head.argCount=1;
     utilExecOlnyMsgSend(mqSWITCH, bEvent);

     bDemoEn=!bDemoEn;
     m_bCommdDemo = TRUE; // kyq 20150929
}
//-------------End Process the uart struction(Demo.)----------------------------


//-------------Begain Process the uart struction(/%[Y]/[X]:[Z].)--------------------------
void ProChannelHdcp(BOOL bIsInport, BOOL bAllPort, BYTE cPort, BYTE cEnalbe)
{
	unsigned int i = 0;
	POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    PINPUTPORT_MANAGER_DATA       psInputPortMgrData;

    if (bIsInport == 1) //��������HDCP����
    {
        psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnINPUTPORT_MAMAGER_DATA);
        //k = 0;
        if (bAllPort == 0)
        {
            //  ���������ʾ������HDCP�豸��
            //  ���벻֧�ִ�HDCP
            psInputPortMgrData->abInputHDCP_EN[cPort - 1] = cEnalbe;
        }
        else // �������ͨ��
        {
            for (i = 0; i < MAX_IN; i++)
            {
                psInputPortMgrData->abInputHDCP_EN[i] = cEnalbe;
            }
        }
        utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA, TRUE);
    }
    else if (bIsInport == 0)  //  �����HDCP ����
    {
        psOutputPortMgrData = utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
        if (bAllPort == 0)
        {
            psOutputPortMgrData->abHDCP_EN[cPort - 1] = cEnalbe;
            psOutputPortMgrData->abHDCP_AUTO[cPort - 1] = FALSE;

        }
        else
        {
            for (i = 0; i < MAX_OUT; i++)
            {
                psOutputPortMgrData->abHDCP_EN[i] = cEnalbe;
                psOutputPortMgrData->abHDCP_AUTO[i] = FALSE;
            }
        }
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, TRUE);
    }
}
//-------------End Process the uart struction(/%[Y]/[X]:[Z].)----------------------------


//-------------Begain Process the uart struction(EDIDMInit.)--------------------------
void ProFactoryDefaultEDID(void)
{
       BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	BYTE   acEdid=0;
	memset(acMessage2, 0, LCD_MAX_CHAR);

	if(eEDID_RS232_GUI_CONTROL !=appGetDialSwitchEdidStatus())
	{
		sprintf(USART1_tx_buff, "Set RS232 or GUI control first.\r\n");
        	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
		return;
	}

       cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "EDIDMInit.");
       appUIUartMessageSet(NULL, acMessage2, TRUE);
       appUIStateChange(uiUART_MESSAGE);
       appSystemInputPortEdidSet(0, edsEDID_Default, &acEdid,ALL_INPORT);			//Intialize 9396 EDID
}
//-------------End Process the uart struction(EDIDMInit.)-----------------------------


//-------------Begain Process the uart struction(EDIDG[x].)--------------------------
uint8_t ProGetOutportEDIDData(unsigned char HPnum1)
{
	unsigned int j = 0;
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);
	BYTE cSii9396Instance;
	//BYTE acEDID_data[256];
	memset(acEDID_data, 0, 256);

    if ((HPnum1 > 0) && (HPnum1 <= MAX_OUT))
    {

        cSii9396Instance = utilOutputPortToInstance(HPnum1 - 1);
        if (TRUE==SiiTxHpdGet(cSii9396Instance))
        {
            SiiTxEdidGetEdidData(cSii9396Instance, acEDID_data);
            memcpy(OutputEdidData[HPnum1 - 1],acEDID_data,256);
            for (j = 0; j < EDID_TABLE_LEN; j++) UART1_SendByte(acEDID_data[j]);
            UART1_SendByte(0x0D);
            UART1_SendByte(0x0A);
        }
        else
        {
            cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "NO device !");
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUIStateChange(uiUART_MESSAGE);
        }
        return (CH_CORRECT);
    }
    else
    {
        return (CH_ERROR);
    }
}
//-------------End Process the uart struction(EDIDG[x].)-----------------------------


//-------------Begain Process the uart struction(GetIntEDID[x].)--------------------------
void ProGetEmbeddingEDIDData(unsigned char HPnum1)
{
	unsigned int j = 0;
	//BYTE acEDID_data[256];
	memset(acEDID_data, 0, 256);

    for (j = 0; j < EDID_TABLE_LEN; j++)
    {
        acEDID_data[j] = 0;
    }
    dvNVRAMUserEdidRead(HPnum1, acEDID_data);
    for (j = 0; j < EDID_TABLE_LEN; j++) UART1_SendByte(acEDID_data[j]);

    UART1_SendByte(0x0D);
    UART1_SendByte(0x0A);
}
//-------------End Process the uart struction(GetIntEDID[x].)-----------------------------


//-------------Begain Process the uart struction(GetInPortEDID[x].)--------------------------
void ProGetInportEDIDData(unsigned char HPnum1)
{
	unsigned int j = 0;
	//BYTE acEDID_data[256];
	memset(acEDID_data, 0, 256);

    for (j = 0; j < EDID_TABLE_LEN; j++)
    {
        acEDID_data[j] = 0;
    }

    dvNVRAMUserEdidRead(edsEDID_IN1+HPnum1 - 1, acEDID_data);
    for (j = 0; j < EDID_TABLE_LEN; j++) UART1_SendByte(acEDID_data[j]);
}
//-------------End Process the uart struction(GetInPortEDID[x].)-----------------------------


//-------------Begain Process the uart struction(EDIDM[X]B[Y].)--------------------------
void ProCopyEDIDDataOutport2Inport(unsigned char HPnum1, unsigned char HPnum2)
{
	BYTE cCount = 0,crc=0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	BYTE   acEdid[256]={0};
	WORD conut=0;
	memset(acMessage2, 0, LCD_MAX_CHAR);

	if(eEDID_RS232_GUI_CONTROL !=appGetDialSwitchEdidStatus())
	{
		sprintf(USART1_tx_buff, "Set RS232 or GUI control first.\r\n");
        	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
		return;
	}

	SiiTxEdidGetEdidData(utilOutputPortToInstance(HPnum1-1), acEdid);

	if ((acEdid[0] == 0) && (acEdid[1] == 0xff) && (acEdid[3] == 0xff)&& (acEdid[7] == 0))
       {
	     if(acEdid[126] != 1)			// DVI
	     {
			for (conut = 0; conut < 128; conut++)  crc += acEdid[conut] ;
			for (conut = 128; conut < 256; conut++)    acEdid[conut] = 0xff;
	     }
	     else						//HDMI
	     {
			for (conut = 0; conut < 256; conut++)  crc += acEdid[conut] ;
	     }
	     if(crc !=0)
		 	memcpy(acEdid,g_Init_EdidFlashEdidTable,  EDID_TABLE_LEN); //�������Ĭ�ϵ�EDID����
       }
	else
	{
		memcpy(acEdid,g_Init_EdidFlashEdidTable,  EDID_TABLE_LEN); //�������Ĭ�ϵ�EDID����
	}

    if (HPnum2 == MAX_IN+1)
    {
         appSystemInputPortEdidSet(0, (eEDIDSELECT)(edsEDID_EXTERN1 + HPnum1 - 1), acEdid,ALL_INPORT);
    }
    else
    {
        appSystemInputPortEdidSet(HPnum2 - 1, (eEDIDSELECT)(edsEDID_EXTERN1 + HPnum1 - 1), acEdid,SINGLE_INPORT);
    }

    //��������Ϣ
    if (HPnum2 < (MAX_IN + 1))
    {
        cCount += SiiSprintf((char*)&acMessage2[cCount], "EDIDM%dB%d", HPnum1, HPnum2);
        appUIUartMessageSet(NULL, acMessage2, TRUE);
        appUIStateChange(uiUART_MESSAGE);
    }
    else
    {
        cCount += SiiSprintf((char*)&acMessage2[cCount], "EDIDM%dBALL", HPnum1);
        appUIUartMessageSet(NULL, acMessage2, TRUE);
        appUIStateChange(uiUART_MESSAGE);
    }
}
//-------------End Process the uart struction(EDIDM[X]B[Y].)-----------------------------


//-------------Begain Process the uart struction(EDIDH[x]B[y].)--------------------------
uint8_t ProCopyEDIDDataOutport2Inport_AudioPCM(unsigned char HPnum1, unsigned char HPnum2)
{
	unsigned char crc = 0;
	BYTE cInstance = 0;
	unsigned int j = 0, IndexC = 0, EndT = 0, conut = 0, m = 0, n = 0, x = 0, k = 0;
	//BYTE acEDID_data[256]={0};
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);
	memset(acEDID_data, 0, 256);

	if(eEDID_RS232_GUI_CONTROL !=appGetDialSwitchEdidStatus())
	{
		sprintf(USART1_tx_buff, "Set RS232 or GUI control first.\r\n");
        	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
		return (CH_CORRECT);
	}

	cInstance = utilOutputPortToInstance(HPnum1 - 1);
	if (TRUE==SiiTxHpdGet(cInstance))
       {
	    //����Ӧ�����ͨ����EDID���ݶ�ȡ����

	    SiiTxEdidGetEdidData(cInstance, acEDID_data);

	    if (acEDID_data[126] == 1) //HDMI
	    {
	        for (j = 0; j < EDID_TABLE_LEN; j++)  crc += acEDID_data[j];
	    }
	    else
	    {
	        for (j = 0; j < 128; j++)  crc += acEDID_data[j];
	        for (j = 128; j < EDID_TABLE_LEN; j++)  acEDID_data[j] = 0xff;
	    }
	    if (crc == 0)
	    {
	        if ((acEDID_data[0] == 0) && (acEDID_data[1] == 0xff) && (acEDID_data[7] == 0));
	        else
	        {
	            return (CM_ERROR);
	        }
	    }
	    else
	    {
	        return (CM_ERROR);
	    }
	}
	else
	    return (CM_ERROR);
	//����ǿ��PCM����
	if ((acEDID_data[0] == 0x00) && (acEDID_data[1] == 0xff) && (acEDID_data[7] == 0x00))
    {
        if (acEDID_data[126] == 0x01) //HDMI��EDID����
        {
            IndexC = EDID_GetDataBlockAddr(acEDID_data, 0x40, 132);
            EndT = (acEDID_data[IndexC] & 0x1F);
            IndexC = EDID_GetDataBlockAddr(acEDID_data, 0x20, IndexC + EndT + 1);
            if (IndexC > 132) //�ҵ���Ƶ����
            {
                EndT = (acEDID_data[IndexC] & 0x1F);
                if ((acEDID_data[IndexC] == 0x23) && ((acEDID_data[IndexC + 1] & 0x78) == 0x08)); //ֻ֧��PCM��ʱ��
                else
                {
                    if (EndT > 3)
                        acEDID_data[130] -= (EndT - 3); //��Ƶ������ƫ�Ƶ�ַ��Ҫ�䶯
                    acEDID_data[IndexC] =  0x23;
                    acEDID_data[IndexC + 1] =  0x09;
                    acEDID_data[IndexC + 2] =  0x07;
                    acEDID_data[IndexC + 3] =  0x07;

                    m = IndexC + EndT + 1; //��ʼ�±�
                    x = 0;
                    for (n = m; n < 255; n++)
                    {
                        k = acEDID_data[n];
                        acEDID_data[IndexC + 4 + x] = k;
                        x++;
                    }

                    for (n = (IndexC + 4 + x); n < 255; n++)
                    {
                        acEDID_data[n] = 0;
                    }

                    for (conut = 128; conut < 255; conut++)  crc += acEDID_data[conut];
                    acEDID_data[255] = ~(crc - 1);
                }
            }
            else //û���ҵ���Ƶ��������
            {
                for (conut = 128; conut < 256; conut++)  acEDID_data[conut] = g_Init_EdidFlashEdidTable[conut];  //��Ĭ�ϵ�EDID���ݵĺ�128�ֽڿ�����ȥ
            }
        }
        else //DVI��EDIDƵ����
        {
            for (conut = 128; conut < 256; conut++)    acEDID_data[conut] = 0xff;
        }

        // ��ԭ���Ѿ����õ�EDID ���ݱȽ�
        /*
        Read_EDID(INPUT_EDID_add + 256 * (HPnum2 - 1), g_edidFlashEdidTable);  //��ȡĿǰ��Ӧ������ͨ����EDID����
        for (j = 0; j < EDID_TABLE_LEN - 1; j++) //�Ƚ�Ŀǰ��EDID��ָ����ʾ����EDID����
        {
            if (g_tempData[j] == g_edidFlashEdidTable[j])
            {ediden = 0;}
            else
            {
                ediden = 1;
                break;
            }
        }
        */

        if (TRUE) // if (ediden)
        {

            //dvSiiDrvAdaptRxHpdSet(HPnum2 - 1, SII_DRV_ADAPT_INP_CTRL__DISABLE);
            //dvSiiDrvAdaptRxEdidSramWrite(HPnum2 - 1, 0, g_edidFlashEdidTable, EDID_TABLE_LEN);
            //IWDG_ReloadCounter(); //ι��
            //dvSiiDrvAdaptRxHpdSet(HPnum2 - 1, SII_DRV_ADAPT_INP_CTRL__ENABLE);
            appSystemInputPortEdidDataSet(HPnum2 - 1, (eEDIDSELECT)(edsEDID_IN1 + HPnum1 - 1), acEDID_data);
            //write_EDID(HPnum2, g_tempData);

        }
    }
    //��������Ϣ
    //sprintf(USART1_tx_buff, "EDIDH%02dB%02d      ", HPnum1, HPnum2);
    //strcpy(key_D2, USART1_tx_buff);
    //LCD_LineRefresh(key_D2, LINE2);
    //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
    //UART1_SendBytes("\r\n", 2);
    cCount += SiiSprintf((char*)&acMessage2[cCount], "EDIDH%dB%d", HPnum1, HPnum2);
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUIStateChange(uiUART_MESSAGE);
    return (CH_CORRECT);
}
//-------------End Process the uart struction(EDIDH[x]B[y].)-----------------------------


//-------------Begain Process the uart struction(EDIDPCM[x].)----------------------------
void ProModifyEDIDDataInport_AudioPCM(unsigned char HPnum2)
{
	unsigned int IndexC = 0, EndT = 0, conut = 0, m = 0, n = 0, x = 0, k = 0;
    unsigned char crc = 0, ediden = 0;
	BYTE acEdid[256];

	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);

	if(eEDID_RS232_GUI_CONTROL !=appGetDialSwitchEdidStatus())
	{
		sprintf(USART1_tx_buff, "Set RS232 or GUI control first.\r\n");
        	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
		return;
	}

    //������ǰ��EDID��Ϣ
    dvNVRAMUserEdidRead(edsEDID_IN1 + HPnum2 - 1, acEdid); // Read_EDID(INPUT_EDID_add + 256 * (HPnum2 - 1), g_tempData);
                                                           //����PCM����
    if ((acEdid[0] == 0x00) && (acEdid[1] == 0xff) && (acEdid[7] == 0x00))
    {
            if (acEdid[126] == 0x01) //HDMI��EDID����
            {
                IndexC = EDID_GetDataBlockAddr(acEdid, 0x40, 132);
                EndT = (acEdid[IndexC] & 0x1F);
                IndexC = EDID_GetDataBlockAddr(acEdid, 0x20, IndexC + EndT + 1);
                if (IndexC > 132) //�ҵ���Ƶ����
                {
                    EndT = (acEdid[IndexC] & 0x1F); //ԴEDID����Ƶ�������ֽ���
                    if ((acEdid[IndexC] == 0x23) && ((acEdid[IndexC + 1] & 0x78) == 0x08))
                    {
                        ediden = 0;
                    }
                    else
                    {
                        if (EndT > 3)
                            acEdid[130] -= (EndT - 3); //��Ƶ������ƫ�Ƶ�ַ��Ҫ�䶯
                        acEdid[IndexC] =  0x23;
                        acEdid[IndexC + 1] =  0x09;
                        acEdid[IndexC + 2] =  0x07;
                        acEdid[IndexC + 3] =  0x07;

                        m = IndexC + EndT + 1; //��ʼ�±�
                        x = 0;
                        for (n = m; n < 255; n++)
                        {
                            k = acEdid[n];
                            acEdid[IndexC + 4 + x] = k;
                            x++;
                        }

                        for (n = (IndexC + 4 + x); n < 255; n++)
                        {
                            acEdid[n] = 0;
                        }

                        for (conut = 128; conut < 255; conut++)  crc += acEdid[conut];
                        acEdid[255] = ~(crc - 1);

                        ediden = 1;
                    }
                }
                else //û���ҵ���Ƶ��������
                {
                    ediden = 0;
                }
            }
            else //DVI��EDIDƵ����
            {
                for (conut = 128; conut < 256; conut++)    acEdid[conut] = 0xff;
            }

            if (ediden)
            {

                appSystemInputPortEdidDataSet(HPnum2 - 1, (eEDIDSELECT)(edsEDID_IN1 + HPnum2 - 1), acEdid);
                //dvSiiDrvAdaptRxHpdSet(HPnum2 - 1, SII_DRV_ADAPT_INP_CTRL__DISABLE);
                //dvSiiDrvAdaptRxEdidSramWrite(HPnum2 - 1, 0, g_edidFlashEdidTable, EDID_TABLE_LEN);
                //IWDG_ReloadCounter(); //ι��
                //dvSiiDrvAdaptRxHpdSet(HPnum2 - 1, SII_DRV_ADAPT_INP_CTRL__ENABLE);
#if 0
                //device_slect_9187(HPnum2);
                SiiRegWrite(Build_cInstance, 0x0010, 0x00); //���Ȳ������
                delay_nms(10);
                SiiDrvNvramSramWrite(Build_cInstance, SRAM_P2, g_tempData, 0, EDID_TABLE_LEN); //����EDID����
                IWDG_ReloadCounter(); //ι��
                SiiRegWrite(Build_cInstance, 0x0010, 0x55); //���Ȳ�����ߣ������Ļ��ٴ�������ô�ź�Դ�ͻ����¶�ȡEDID����
                                                            //disconnect_9187()
#endif
                //write_EDID(HPnum2, g_tempData);
            }
     }
    //��������Ϣ

    //sprintf(USART1_tx_buff, "EDIDPCM%02d     ", HPnum2);
    //strcpy(key_D2, USART1_tx_buff);
    // LCD_LineRefresh(key_D2, LINE2);
    //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
    //UART1_SendBytes("\r\n", 2);
    cCount += SiiSprintf((char*)&acMessage2[cCount], "EDIDPCM%d", HPnum2);
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUIStateChange(uiUART_MESSAGE);
}
//-------------End Process the uart struction(EDIDPCM[x].)-----------------------------


//-------------Begain Process the uart struction(%080[x].)----------------------------
void ProConfigHDCPMode(unsigned char HPnum1)
{
	unsigned int i = 0;
	POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);

    //psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
    psOutputPortMgrData = utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    if (HPnum1 == '0') //�ֶ�
    {
        //HDCP_AUTO_FLAG = 0;
        //AUTO_change_FLAG = 0;
        for (i = 0; i < MAX_OUT; i++)
        {
            psOutputPortMgrData->abHDCP_AUTO[i] = false;
        }

        //UART1_SendBytes("%0800\r\n", 7);
        cCount += SiiSprintf((char*)&acMessage2[cCount], "%s","%0800");
        appUIUartMessageSet(NULL, acMessage2, TRUE);
        appUIStateChange(uiUART_MESSAGE);
        //while (!(USART1->SR & USART_FLAG_TC));
        //strcpy(D2, "%0800");
    }
    else  if (HPnum1 == '1') //�Զ�
    {
        //if (psOutputPortMgrData->bHDCP_AUTO == false)
        {
            //HDCP_AUTO_FLAG = 1;
            //NewCommandInFlag = NewCommandInFlag | 0x01;
            //AUTO_change_FLAG = 1;
            for (i = 0; i < MAX_OUT; i++)
            {
                psOutputPortMgrData->abHDCP_AUTO[i] = true;
            }

            //UART1_SendBytes("%0801\r\n", 7);

            cCount += SiiSprintf((char*)&acMessage2[cCount], "%s","%0801");
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUIStateChange(uiUART_MESSAGE);
            //for (i = 0; i < MAX_OUT; i++) OutSwitch_Flag[i] = 1;
            //OutSwitch_TimeDelay = 200;
        }
        //while (!(USART1->SR & USART_FLAG_TC));
        //strcpy(D2, "%0801");
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
}
//-------------End Process the uart struction(%080[x].)-----------------------------


//-------------Begain Process the uart struction(%090[x].)----------------------------
void ProConfigIRCarrierMode(unsigned char HPnum1)
{
	PSYSTEM_SETTING_DATA   psSystemSettingData;

	BYTE cCount = 0;
	BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
	memset(acMessage2, 0, LCD_MAX_CHAR);

	psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

    if (HPnum1 == '0')      //�����ز�����
    {
        // IR_carrier_mode = 0;
        psSystemSettingData->cIR_Carrier_Mode = 0;
        //send_data_to_FPGA(0xDE);
        //if (check_ack())
        //    send_data_to_FPGA(0xDE);
        dvCPLDIRCarrierSet(FALSE);
        //UART1_SendBytes("Carrier native\r\n", 16);
        //sprintf(USART1_tx_buff, "Carrier native\r\n");
        // strcpy(D2, "Carrier native");
        cCount += SiiSprintf((char*)&acMessage2[cCount], "%s","Carrier native");
        appUIUartMessageSet(NULL, acMessage2, TRUE);
        appUIStateChange(uiUART_MESSAGE);

    }
    else  if (HPnum1 == '1')        //�����ز�ǿ��
    {
        //IR_carrier_mode = 1;
        psSystemSettingData->cIR_Carrier_Mode = 1;
        dvCPLDIRCarrierSet(TRUE);
        //send_data_to_FPGA(0xDF);
        //if (check_ack())
        //    send_data_to_FPGA(0xDF);
        //sprintf(USART1_tx_buff, "Force carrier\r\n");
        //UART1_SendBytes("Force carrier\r\n", 15);
        //  strcpy(D2, "Force carrier");
        cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "Force carrier");
        appUIUartMessageSet(NULL, acMessage2, TRUE);
        appUIStateChange(uiUART_MESSAGE);

    }
    //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                      TRUE);
}
//-------------End Process the uart struction(%090[x].)-----------------------------


//-------------Begain Process the uart struction(DigitAudioON[x].)----------------------------
uint8_t ProDigitAudioON(unsigned char HPnum1)
{
	BYTE cSii9396Instance;
	unsigned int j = 0;
    if ((HPnum1 < (MAX_OUT + 1)) && (HPnum1 > 0))        //��������
    {
        cSii9396Instance = utilOutputPortToInstance(HPnum1 - 1);
	 Sii9396TxSpdifAudioOnOff(cSii9396Instance,SII_AUDIO__SPDIF);

        sprintf(USART1_tx_buff, "DigitAudio ON with Output %d\r\n", HPnum1);
        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

        return (CH_CORRECT);
    }
    else if (HPnum1 == (MAX_OUT + 1))
    {
        for (j = 0; j < MAX_OUT; j++)
        {
            cSii9396Instance = utilOutputPortToInstance(j);
	     Sii9396TxSpdifAudioOnOff(cSii9396Instance,SII_AUDIO__SPDIF);
        }
        sprintf(USART1_tx_buff, "DigitAudio ON with ALL Outputs\r\n");
        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
        return (CH_CORRECT);
    }
    else
        return (CM_ERROR);
}
//-------------End Process the uart struction(DigitAudioON[x].)-----------------------------


//-------------Begain Process the uart struction(DigitAudioOFF[x].)----------------------------
uint8_t ProDigitAudioOFF(unsigned char HPnum1)
{
	BYTE cSii9396Instance;
	unsigned int j = 0;
	if ((HPnum1 < (MAX_OUT + 1)) && (HPnum1 > 0))	   //�ر�����
	{
		cSii9396Instance = utilOutputPortToInstance(HPnum1 - 1);
	 	Sii9396TxSpdifAudioOnOff(cSii9396Instance,SII_AUDIO__I2S);

		sprintf(USART1_tx_buff, "DigitAudio OFF with Output %d\r\n", HPnum1);
		UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
		return (CH_CORRECT);
	}
	else if (HPnum1 == (MAX_OUT + 1))
	{
		for (j = 0; j < MAX_OUT; j++)
		{
			cSii9396Instance = utilOutputPortToInstance(j);
	     		Sii9396TxSpdifAudioOnOff(cSii9396Instance,SII_AUDIO__I2S);
		}
		sprintf(USART1_tx_buff, "DigitAudio OFF with ALL Outputs\r\n");
		UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
		return (CH_CORRECT);
	}
	else
		return (CM_ERROR);
}
//-------------End Process the uart struction(DigitAudioOFF[x].)-----------------------------


//-------------Begain Process the uart struction(%997[x].)----------------------------
void  ProQueryAudioVedioStatus(unsigned char cNum)
{
    switch (cNum)
    {
        case '1' :          // %9971
            appUIStateChange(uiUART_INCONNECT_INFO);
            break;
        case '2' :
            appUIStateChange(uiUART_OUTCONNECT_INFO); // uiSYSTEM_OUTCONNECT_INFO
            break;
        case '3' :
            appUIStateChange(uiUART_IN_HDCP_INFO);
            break;
        case '4' :
            appUIStateChange(uiUART_OUT_HDCP_INFO);
            break;
        case '5' :
            appUIStateChange(uiUART_SWITCH_INFO);
            break;
        case '6' :
            appUIStateChange(uiUART_OUT_RESOLUTION_INFO);
            break;
        case '7' : //��ѯ���ͨ������������״̬
            appUIStateChange(uiUART_OUT_HDMIAUDIO_INFO);
            break;
        case '8' : //��ѯ����˵�HDCP����״̬
            appUIStateChange(uiUART_IN_HDCPENABLE_INFO);
            break;
        case '9' : //��ѯedid����ѡ��״̬
            appUIStateChange(uiUART_EDID_STATUS);
            break;
        default :
            break;
    }
}
//-------------End Process the uart struction(%997[x].)-----------------------------


//-------------Begain Process the uart struction(%996[x].)----------------------------
void ProQuerySystermStatus(unsigned char HPnum1)
{
	PSYSTEM_SETTING_DATA   psSystemSettingData;

	switch(HPnum1)
	{
		case '1' :
			appUIStateChange(uiSYSTEM_LOCK_INFO);
			break;
		case '2' :
			sprintf(USART1_tx_buff, "PWON \r\n");
			UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

			break;
		case '3' :
			// NO LCD Disaplay
			psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
			if (psSystemSettingData->cIR_Carrier_Mode == 0)
			{
				sprintf(USART1_tx_buff, "Carrier native\r\n");
				UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
			}
			else if (psSystemSettingData->cIR_Carrier_Mode == 1)
			{
				sprintf(USART1_tx_buff, "Force carrier\r\n");
				UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
			}
			utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
							  FALSE);
			break;

		case '4' :
			appUIStateChange(uiUART_IP_INFO);
			break;
		default :
			break;

		}

}
//-------------End Process the uart struction(%996[x].)-----------------------------


#ifdef PRD_POWER_ONOFF_MESSAGE
//-------------Begain Process the uart struction(%995[x].)----------------------------
uint8_t ProQueryPortPWONSendData(unsigned char HPnum1)
{
	PSYSTEM_SETTING_DATA   psSystemSettingData;

	psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);

	if (psSystemSettingData->acPowerOnMsgLength[HPnum1 - 1] == 0)
	    sprintf(USART1_tx_buff, "Port %d: NO Data\r\n", HPnum1);
	else
	    sprintf(USART1_tx_buff, "Port %d: %s when PWON\r\n", HPnum1, psSystemSettingData->acPowerOnMsg[HPnum1 - 1]);

	utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
	              FALSE);
	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
	return (CH_CORRECT);

}
//-------------End Process the uart struction(%995[x].)-----------------------------

uint8_t ProQueryPortPWOFFSendData(unsigned char HPnum1)
{
	PSYSTEM_SETTING_DATA   psSystemSettingData;

                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);

                    if (psSystemSettingData->acPowerOffMsgLength[HPnum1 -1] == 0)
                        sprintf(USART1_tx_buff, "Port %d: NO Data\r\n", HPnum1);
                    else
                        sprintf(USART1_tx_buff, "Port %d: %s when PWOFF\r\n", HPnum1 , psSystemSettingData->acPowerOffMsg[HPnum1 - 1]);

                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                                  FALSE);
                UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                return (CH_CORRECT);

}
#endif


//-------------Begain Process the uart struction(%0911.)----------------------------
void ProRecallFactoryDefault(void)
{
	unsigned int j = 0;
	BYTE   acEdid[256]={0};
	for (j = 0; j < MAX_OUT; j++)
       {
            //dvCpldRemoteBraundSet(j + 1);
	     Sii9396TxSpdifAudioOnOff(utilOutputPortToInstance(j),SII_AUDIO__SPDIF);
    	}
    utilDataMgrReset(nveRESETALL);
    appUIIRKeyEnableSet(TRUE);
    dvCPLDIRCarrierSet(FALSE); //[kyq ADD: 20151010 11:42:15]
    appSystemSwitcherSetStart();
    delay_nms(100);
    appSystemInputPortEdidSet(0, (eEDIDSELECT) edsEDID_Default, acEdid,ALL_INPORT);
#ifdef PRD_IR_OUT
    appSystemIRSwitcherSetStart();
#endif
	#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    KeyLedStatuesInitialize();
	//Set_Tm1623ValueChangeFlag(true);
	#endif

#ifdef ENABLE_DB9_BAUDRATE_SWITCH
    {
        PSYSTEM_SETTING_DATA   psSystemSettingData;
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
        //psSystemSettingData->PcUartBaudrateSelect = 3;
        SetDB9Baudrate(psSystemSettingData->PcUartBaudrateSelect);
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
    }
#endif
}
//-------------End Process the uart struction(%0911.)-----------------------------


uint8_t ProCECCommand(uint8_t cPort, uint8_t opCode, uint8_t i, uint8_t ArgCnt, uint8_t* pArg)
{
	BYTE   cInstance;
	BYTE   cLocal;

    cLocal = cPort & BIT4;
    cPort = cPort & 0x0f;
    if (i < 2 || cPort > MAX_OUT || cPort == 0)
        return CH_ERROR;

    cInstance = utilOutputPortToInstance(cPort - 1);
    if ((cPort == 1 | cPort == 2) && cLocal)
    {
        cInstance++;
    }

    //Init_StructInstance(cInstance);
    //SiiCecSendUserControlCmd(cInstance, opCode, (SiiCecLogicalAddresses_t)0, pArg, ArgCnt);
    //UART1_SendBytes("CEC OK\r\n",sizeof("CEC OK\r\n"));
    return CH_CORRECT;
}

uint8_t ProCECCommand2(uint8_t cPort, uint8_t cecaddr,uint8_t opCode, uint8_t i, uint8_t ArgCnt, uint8_t* pArg)
{
	BYTE   cInstance;
	//BYTE   cLocal;

    //cLocal = cPort & BIT4;
    cPort = cPort & 0x0f;
    if (i < 2 || cPort > MAX_OUT || cPort == 0)
        return CH_ERROR;

    cInstance = utilOutputPortToInstance(cPort - 1);
    #if 0
    if ((cPort == 1 | cPort == 2) && cLocal)
    {
        cInstance++;
    }
    #endif

    //Init_StructInstance(cInstance);
    //SiiCecSendUserControlCmd2(cInstance, opCode,cecaddr, pArg, ArgCnt);
    //UART1_SendBytes("CEC OK\r\n",sizeof("CEC OK\r\n"));
    return CH_CORRECT;
}


//-------------Begain Process the uart struction(0xfa********0xfe)----------------------------
//----------------------Modify the product name-----------------------------------------------
void ProModifyProductName(unsigned char *pp)
{
	unsigned int i = 0;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);


    pp++;
    for (i = 0; i < 16; i++)
    {
        if (*(pp + i) != 0xfe)
            psSystemSettingData->acProduct_Name[i] = *(pp + i);
        else
            break;
    }
	psSystemSettingData->acProduct_Name[16] = 0;// kyq 20151201
    if (i == 16)
    {
        psSystemSettingData->acProduct_Name[i] = 0;

        UART1_SendBytes("sys_name is ", strlen("sys_name is "));
        sprintf(USART1_tx_buff, "%s \r\n", psSystemSettingData->acProduct_Name);
        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
    }
    else
        UART1_SendBytes("sys_name setting fail!", strlen("sys_name setting fail!"));
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);

}
//-------------End Process the uart struction(0xfa********0xfe)-----------------------------


//-------------Begain Process the uart struction(0xfb********0xfe)----------------------------
//----------------------Modify the product type-----------------------------------------------
void ProModifyProductType(unsigned char *pp)
{
	unsigned int i = 0;
	PSYSTEM_SETTING_DATA   psSystemSettingData;
	psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

	pp++;
	for (i = 0; i < 16; i++)
	{
		if (*(pp + i) != 0xfe)
			psSystemSettingData->acProduct_Type[i] = *(pp + i);
		else
			break;
	}
	psSystemSettingData->acProduct_Type[16] = 0; // kyq 20151201
	if (i == 16)
	{
		psSystemSettingData->acProduct_Type[i] = 0;
		// for(j=0;j<i;j++)  i2c_EE_write_one_byte(sys_type_add+j,sys_type[j]);

		UART1_SendBytes("sys_type is ", strlen("sys_type is "));
		sprintf(USART1_tx_buff, "%s \r\n", psSystemSettingData->acProduct_Type);
		UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
	}
	else
		UART1_SendBytes("sys_type setting fail!", strlen("sys_type setting fail!"));

	utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
}
//-------------End Process the uart struction(0xfb********0xfe)-----------------------------


//-------------Begain Process the uart struction(0xfc********0xfe)----------------------------
//----------------------Modify the default EDID name-----------------------------------------------
void ProModifyDefaultEDIDName(unsigned char *pp)
{
	unsigned int i = 0;
	unsigned char crc = 0;
	uint16_t sys_ID = 0x07ff;

    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

    pp++;
    for (i = 0; i < 3; i++)
    {
        if (*(pp + i) != 0xfe)
            psSystemSettingData->acDefaultEdidProductName[i] = *(pp + i); //   [i] = *(pp+i) ;
        else
            break;
    }
    if (i == 3)
    {
        psSystemSettingData->acDefaultEdidProductName[i] = 0; // EDID_name[i] = 0;

        if (psSystemSettingData->acDefaultEdidProductName[2] >= 'A')
        {
            sys_ID &= 0xffe0; //�����5λ
            sys_ID |= psSystemSettingData->acDefaultEdidProductName[2] - 0x40;
        }
        if (psSystemSettingData->acDefaultEdidProductName[1] >= 'A')
        {
            sys_ID &= 0xfc1f; //�����5λ
            sys_ID |= ((psSystemSettingData->acDefaultEdidProductName[1] - 0x40) << 5);
        }
        if (psSystemSettingData->acDefaultEdidProductName[0] >= 'A')
        {
            sys_ID &= 0x03ff; //�����5λ
            sys_ID |= ((psSystemSettingData->acDefaultEdidProductName[0] - 0x40) << 10);
        }

        g_Init_EdidFlashEdidTable[8] = sys_ID >> 8;
        g_Init_EdidFlashEdidTable[9] = sys_ID;
        for (i = 0; i < 127; i++)  crc += g_Init_EdidFlashEdidTable[i];
        g_Init_EdidFlashEdidTable[127] = ~(crc - 1);

        UART1_SendBytes("EDID_name is ", strlen("EDID_name is "));
        sprintf(USART1_tx_buff, "%s \r\n", psSystemSettingData->acDefaultEdidProductName);
        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
        //[kyq Begin: 20150820 20:03:17]

        dvNVRAMUserEdidWrite(edsEDID_Default, g_Init_EdidFlashEdidTable);
        //[kyq End: 20150820 20:03:20]
    }
    else
        UART1_SendBytes("EDID_name setting fail!", strlen("EDID_name setting fail!"));
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
}
//-------------End Process the uart struction(0xfc********0xfe)-----------------------------


//-------------Begain Process the uart struction(0xf1********0xfe)----------------------------
//----------------------Modify the LCD1 display-----------------------------------------------
void ProModifyLCD1Display(unsigned char *pp)
{
	unsigned int i = 0, j = 0;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

    pp++;
    for (i = 0; i < 16; i++)
    {
        if (*(pp + i) != 0xfe)
            LCD1_IDLE_TEXT[i] = *(pp + i);
        else
            break;
    }
	LCD1_IDLE_TEXT[16] = 0;
    if (i == 16)
    {

        memcpy(psSystemSettingData->acLCDLine1DefaultTXT, LCD1_IDLE_TEXT, 16);
        appUIStateChange(uiSPLASH);

        {
            UART1_SendBytes("LCD 1: ", strlen("LCD 1: "));
            for (j = 0; j < 16; j++)
            {
                UART1_SendByte(LCD1_IDLE_TEXT[j]);
            }
            UART1_SendBytes("\r\n", 2);
        }
    }
    else
    {
        UART1_SendBytes("Error!\r\n", strlen("Error!\r\n"));
    }
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
}
//-------------End Process the uart struction(0xf1********0xfe)-----------------------------


//-------------Begain Process the uart struction(0xf2********0xfe)----------------------------
//----------------------Modify the LCD2 display-----------------------------------------------
void ProModifyLCD2Display(unsigned char *pp)
{
	unsigned int i = 0, j = 0;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

    pp++;
    for (i = 0; i < 16; i++)
    {
        if (*(pp + i) != 0xfe)
            LCD2_IDLE_TEXT[i] = *(pp + i);
        else
            break;
    }
    LCD2_IDLE_TEXT[16] = 0;
    if (i == 16)
    {

        memcpy(psSystemSettingData->acLCDLine2DefaultTXT, LCD2_IDLE_TEXT, 16);
        appUIStateChange(uiSPLASH);

        {

            UART1_SendBytes("LCD 2: ", strlen("LCD 2: "));
            for (j = 0; j < 16; j++)
            {
                UART1_SendByte(LCD2_IDLE_TEXT[j]);
            }
            UART1_SendBytes("\r\n", 2);
        }
    }
    else
    {
        UART1_SendBytes("Error!\r\n", strlen("Error!\r\n"));
    }
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
}
//-------------End Process the uart struction(0xf2********0xfe)-----------------------------


//-------------Begain Process the uart struction(0xf3********0xfe)----------------------------
//----------------------Modify the IP address-----------------------------------------------
void ProModifyIPAddress(unsigned char *pp)
{
	unsigned int i = 0;
    BYTE IP_add[4];
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

    if (get_ip_add(pp, IP_add))
    {
        //ip_set_ok = 1;
        for (i = 0; i < 4; i++) //i2c_EE_write_one_byte(sys_IP_add + i, IP_add[i]);
        {
            psSystemSettingData->acIP_Address[i] = IP_add[i];
        }
	GetGuiIP=1;
        appUIStateChange(uiUART_IP_INFO);

    }
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
}

#ifdef ENABLE_DB9_BAUDRATE_SWITCH
void Function_SetPcUartBaudrate(unsigned char tbaudrate)
{
    PSYSTEM_SETTING_DATA   psSystemSettingData;

    switch(tbaudrate)
    {
        case 1:
            UART1_SendBytes("Set Local RS232 bardrate is 2400!\r\n", strlen("Set Local RS232 bardrate is 2400!\r\n"));
            break;
        case 2:
            UART1_SendBytes("Set Local RS232 bardrate is 4800!\r\n", strlen("Set Local RS232 bardrate is 4800!\r\n"));
            break;
        case 3:
            UART1_SendBytes("Set Local RS232 bardrate is 9600!\r\n", strlen("Set Local RS232 bardrate is 9600!\r\n"));
            break;
        case 4:
            UART1_SendBytes("Set Local RS232 bardrate is 19200!\r\n", strlen("Set Local RS232 bardrate is 19200!\r\n"));
            break;
        case 5:
            UART1_SendBytes("Set Local RS232 bardrate is 38400!\r\n", strlen("Set Local RS232 bardrate is 38400!\r\n"));
            break;
        case 6:
            UART1_SendBytes("Set Local RS232 bardrate is 57600!\r\n", strlen("Set Local RS232 bardrate is 57600!\r\n"));
            break;
        case 7:
            UART1_SendBytes("Set Local RS232 bardrate is 115200!\r\n", strlen("Set Local RS232 bardrate is 115200!\r\n"));
            break;
        default:
            UART1_SendBytes("Baudrate set error!\r\n", strlen("Baudrate set error!\r\n"));
            break;
    }
    delay_nms(100);
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
    psSystemSettingData->PcUartBaudrateSelect = tbaudrate;
    SetDB9Baudrate(tbaudrate);
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
}

void Function_QueryBaudrate(void)
{
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

    switch(psSystemSettingData->PcUartBaudrateSelect)
    {
        case 1:
            UART1_SendBytes("Set Local RS232 bardrate is 2400!\r\n", strlen("Set Local RS232 bardrate is 2400!\r\n"));
            break;
        case 2:
            UART1_SendBytes("Set Local RS232 bardrate is 4800!\r\n", strlen("Set Local RS232 bardrate is 4800!\r\n"));
            break;
        case 3:
            UART1_SendBytes("Set Local RS232 bardrate is 9600!\r\n", strlen("Set Local RS232 bardrate is 9600!\r\n"));
            break;
        case 4:
            UART1_SendBytes("Set Local RS232 bardrate is 19200!\r\n", strlen("Set Local RS232 bardrate is 19200!\r\n"));
            break;
        case 5:
            UART1_SendBytes("Set Local RS232 bardrate is 38400!\r\n", strlen("Set Local RS232 bardrate is 38400!\r\n"));
            break;
        case 6:
            UART1_SendBytes("Set Local RS232 bardrate is 57600!\r\n", strlen("Set Local RS232 bardrate is 57600!\r\n"));
            break;
        case 7:
            UART1_SendBytes("Set Local RS232 bardrate is 115200!\r\n", strlen("Set Local RS232 bardrate is 115200!\r\n"));
            break;
        default:
            UART1_SendBytes("Baudrate set error!\r\n", strlen("Baudrate set error!\r\n"));
            break;
    }

    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
}

#endif
//-------------End Process the uart struction(0xf3********0xfe)-----------------------------
#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE == MHUB4K431))
BYTE cmdlist[][13]=
{
    {0xa5,0x5b,0x02,0x03,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xf9},// Input 1,to,Output,1
    {0xa5,0x5b,0x02,0x03,0x01,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xf8},// Input,1,to,Output,2
    {0xa5,0x5b,0x02,0x03,0x01,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0xf7},// Input,1,to,Output,3
    {0xa5,0x5b,0x02,0x03,0x01,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0xf6},// Input,1,to,Output,4
    #if (MAX_IN>4)
    {0xa5,0x5b,0x02,0x03,0x01,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0xf5},// Input,1,to,Output,5
    {0xa5,0x5b,0x02,0x03,0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0xf4},// Input,1,to,Output,6
    {0xa5,0x5b,0x02,0x03,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xf3},// Input,1,to,Output,7
    {0xa5,0x5b,0x02,0x03,0x01,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0xf2},// Input,1,to,Output,8
    #endif

    {0xa5,0x5b,0x02,0x03,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xf8},// Input,2,to,Output,1
    {0xa5,0x5b,0x02,0x03,0x02,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xf7},// Input,2,to,Output,2
    {0xa5,0x5b,0x02,0x03,0x02,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0xf6},// Input,2,to,Output,3
    {0xa5,0x5b,0x02,0x03,0x02,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0xf5},// Input,2,to,Output,4
    #if (MAX_IN>4)
    {0xa5,0x5b,0x02,0x03,0x02,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0xf4},// Input,2,to,Output,5
    {0xa5,0x5b,0x02,0x03,0x02,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0xf3},// Input,2,to,Output,6
    {0xa5,0x5b,0x02,0x03,0x02,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xf2},// Input,2,to,Output,7
    {0xa5,0x5b,0x02,0x03,0x02,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0xf1},// Input,2,to,Output,8
    #endif

    {0xa5,0x5b,0x02,0x03,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xf7},// Input,3,to,Output,1
    {0xa5,0x5b,0x02,0x03,0x03,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xf6},// Input,3,to,Output,2
    {0xa5,0x5b,0x02,0x03,0x03,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0xf5},// Input,3,to,Output,3
    {0xa5,0x5b,0x02,0x03,0x03,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0xf4},// Input,3,to,Output,4
    #if (MAX_IN>4)
    {0xa5,0x5b,0x02,0x03,0x03,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0xf3},// Input,3,to,Output,5
    {0xa5,0x5b,0x02,0x03,0x03,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0xf2},// Input,3,to,Output,6
    {0xa5,0x5b,0x02,0x03,0x03,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xf1},// Input,3,to,Output,7
    {0xa5,0x5b,0x02,0x03,0x03,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0xf0},// Input,3,to,Output,8
    #endif

    {0xa5,0x5b,0x02,0x03,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xf6},// Input,4,to,Output,1
    {0xa5,0x5b,0x02,0x03,0x04,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xf5},// Input,4,to,Output,2
    {0xa5,0x5b,0x02,0x03,0x04,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0xf4},// Input,4,to,Output,3
    {0xa5,0x5b,0x02,0x03,0x04,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0xf3},// Input,4,to,Output,4
    #if (MAX_IN>4)
    {0xa5,0x5b,0x02,0x03,0x04,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0xf2},// Input,4,to,Output,5
    {0xa5,0x5b,0x02,0x03,0x04,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0xf1},// Input,4,to,Output,6
    {0xa5,0x5b,0x02,0x03,0x04,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xf0},// Input,4,to,Output,7
    {0xa5,0x5b,0x02,0x03,0x04,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0xef},// Input,4,to,Output,8
    #endif

    #if (MAX_IN>4)
    {0xa5,0x5b,0x02,0x03,0x05,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xf5},// Input,5,to,Output,1
    {0xa5,0x5b,0x02,0x03,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xf4},// Input,5,to,Output,2
    {0xa5,0x5b,0x02,0x03,0x05,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0xf3},// Input,5,to,Output,3
    {0xa5,0x5b,0x02,0x03,0x05,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0xf2},// Input,5,to,Output,4
    {0xa5,0x5b,0x02,0x03,0x05,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0xf1},// Input,5,to,Output,5
    {0xa5,0x5b,0x02,0x03,0x05,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0xf0},// Input,5,to,Output,6
    {0xa5,0x5b,0x02,0x03,0x05,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xef},// Input,5,to,Output,7
    {0xa5,0x5b,0x02,0x03,0x05,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0xee},// Input,5,to,Output,8

    {0xa5,0x5b,0x02,0x03,0x06,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xf4},// Input,6,to,Output,1
    {0xa5,0x5b,0x02,0x03,0x06,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xf3},// Input,6,to,Output,2
    {0xa5,0x5b,0x02,0x03,0x06,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0xf2},// Input,6,to,Output,3
    {0xa5,0x5b,0x02,0x03,0x06,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0xf1},// Input,6,to,Output,4
    {0xa5,0x5b,0x02,0x03,0x06,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0xf0},// Input,6,to,Output,5
    {0xa5,0x5b,0x02,0x03,0x06,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0xef},// Input,6,to,Output,6
    {0xa5,0x5b,0x02,0x03,0x06,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xee},// Input,6,to,Output,7
    {0xa5,0x5b,0x02,0x03,0x06,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0xed},// Input,6,to,Output,8

    {0xa5,0x5b,0x02,0x03,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xf3},// Input,7,to,Output,1
    {0xa5,0x5b,0x02,0x03,0x07,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xf2},// Input,7,to,Output,2
    {0xa5,0x5b,0x02,0x03,0x07,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0xf1},// Input,7,to,Output,3
    {0xa5,0x5b,0x02,0x03,0x07,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0xf0},// Input,7,to,Output,4
    {0xa5,0x5b,0x02,0x03,0x07,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0xef},// Input,7,to,Output,5
    {0xa5,0x5b,0x02,0x03,0x07,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0xee},// Input,7,to,Output,6
    {0xa5,0x5b,0x02,0x03,0x07,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xed},// Input,7,to,Output,7
    {0xa5,0x5b,0x02,0x03,0x07,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0xec},// Input,7,to,Output,8

    {0xa5,0x5b,0x02,0x03,0x08,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xf2},// Input,8,to,Output,1
    {0xa5,0x5b,0x02,0x03,0x08,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xf1},// Input,8,to,Output,2
    {0xa5,0x5b,0x02,0x03,0x08,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0xf0},// Input,8,to,Output,3
    {0xa5,0x5b,0x02,0x03,0x08,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0xef},// Input,8,to,Output,4
    {0xa5,0x5b,0x02,0x03,0x08,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0xee},// Input,8,to,Output,5
    {0xa5,0x5b,0x02,0x03,0x08,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0xed},// Input,8,to,Output,6
    {0xa5,0x5b,0x02,0x03,0x08,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xec},// Input,8,to,Output,7
    {0xa5,0x5b,0x02,0x03,0x08,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0xeb},// Input,8,to,Output,8
    #endif

    //{0xa5,0x5b,0x06,0x01,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09},// Beep,,Off
    //{0xa5,0x5b,0x06,0x01,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xea},// Beep,,On
    {0xa5,0x5b,0x08,0x0b,0xf0,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x0d},// Power,Off
    {0xa5,0x5b,0x08,0x0b,0x0f,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0xcf},// Power,On
};

int Get_Cmdlist_Len(void)
{
    return (sizeof(cmdlist)/sizeof(cmdlist[0]));
}
#endif

#if 0

//����Ϊͷָ�룬βָ��
// RXdata: ��ǰָ��� ����
// USART1_rxprocesspoint: ��ǰָ��
// USART1_rxtailpoint: ��ͷָ��
//USART1_rxtailpoint
//   |       rxdata
//   |         |USART1_rxprocesspoint
//   |         |              |
//  []   []   []             []          []     [][][][][][][][][][][][][][][][]
//
uint8_t do_with_type(unsigned char RXdata,
                     unsigned char *USART1_rxtailpoint,
                     unsigned char *USART1_rxprocesspoint)
{
    unsigned char *temppoint, *tempptr, *pp;
    unsigned char cmddata = 0, HPnum1 = 0, HPnum2 = 0; //in_flag = 0
	unsigned char InPut = 0, OutPut = 0;
	//unsigned char crc = 0, InOutflag = 0,Signaltype = 0;
	//unsigned int x = 0,flag = 0, m = 0, k = 0, EndT = 0;
    unsigned char returnval = 0;
    unsigned int  i = 0,  count = 0, jiucuo = 0;
    unsigned char user_temp[16] = { 0 };
    unsigned char uart_data_temp[USART1_RX_BUFF_MAX] = { 0 };
    //unsigned char a = 0, ediden = 0;
    //unsigned int  j = 0, conut = 0, n = 0, IndexC = 0;
  //  Event_t   bEvent;
    uint8_t cReturnValue = 0;
    //BYTE cSii9533Instance;
    BYTE cInPort = 0, cOutPort = 0;
    //BYTE acMessage1[LCD_MAX_CHAR] = { 0 };
    BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
    BYTE cCount = 0;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
   // POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
   // PINPUTPORT_MANAGER_DATA       psInputPortMgrData;
    BOOL Re_Message;
    //BYTE acEDID_data[256];
    Re_Message = 1; //psSystemSettingData->bUartReturnMessage_EN;
    //memset(acMessage1, 0, LCD_MAX_CHAR);
    memset(acMessage2, 0, LCD_MAX_CHAR);
    m_bCommdDemo = FALSE; // kyq 20150929

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE == MHUB4K431))
    if(*USART1_rxtailpoint == 0xa5 && *(USART1_rxtailpoint+1)==0x5b)
    {
        //sprintf(USART1_tx_buff,"strlen(cmdlist)==%d strlen(cmdlist[0])==%d!\r\n",sizeof(cmdlist),sizeof(cmdlist[0]));
        //UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
        for(i = 0 ; i< (sizeof(cmdlist)/sizeof(cmdlist[0])); i++)
        {
            if(*(USART1_rxtailpoint+2)==0x02)
            {
                if(StrCmp(USART1_rxtailpoint, cmdlist[i], USART1_rxprocesspoint) != NULL)
                {
                    BYTE acOutPortList[MAX_OUT];
                    cInPort = *(USART1_rxtailpoint+4);
                    acOutPortList[0] = *(USART1_rxtailpoint+6)-1;

                    #if (MACHINETYPE == MHUB4K431)
                    if((cInPort>MAX_IN)||(*(USART1_rxtailpoint+6)>MAX_OUT))
                        return (CM_ERROR);
                    #endif
                    ProessVedioSwitch(cInPort, 1, acOutPortList);

                    return (CH_CORRECT);
                }
            }
            #if 0
            else if(*(USART1_rxtailpoint+2)==0x08)
            {
                if(StrCmp(USART1_rxtailpoint, cmdlist[i], USART1_rxprocesspoint) != NULL)
                {
                    if(*(USART1_rxtailpoint+4)==0xf0)
                    {
                        // beep off
                    }
                    else if(*(USART1_rxtailpoint+4)==0x0f)
                    {
                        // beep on
                    }
                    return (CH_CORRECT);
                }
            }
            #endif
            else if(*(USART1_rxtailpoint+2)==0x08)
            {
                if(StrCmp(USART1_rxtailpoint, cmdlist[i], USART1_rxprocesspoint) != NULL)
                {
                    if(*(USART1_rxtailpoint+4)==0xf0)
                    {
                        cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "PWOFF");
                        appUIUartMessageSet(NULL, acMessage2, FALSE);
                        appUIStateChange(uiUART_MESSAGE);
                        appPowerMgrPowerStateSet(pwrSOFTOFF);
                        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                        KeyLedSetAllClose();
                        #endif
                        return (CH_CORRECT);
                    }
                    else if(*(USART1_rxtailpoint+4)==0x0f)
                    {
                        // power on
                    }
                }
            }
        }
        i = 0;
    }
#endif

    if (RXdata == '.')
    {
//=========================================================================
// [x1]V[x2].  ��[x1]·���뵽[x2]·���������Ƶ��
// [x1]B[x2].  ��[x1]·���뵽[x2]·���������Ƶ������ͬ����
// [x1]R[x2]. ��[x1]·���뵽[x2]·��������⡣
//  �����ж������ڵ������?
        pp = StrCmp(USART1_rxtailpoint, "0V", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = MAX_IN+1 ;
        #ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
                #if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d Closed.", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulClose);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                #endif
                #if 0//(MACHINETYPE == MUH4K862)
                KeyInput[cOutPort]=cInPort;
                #endif
				ProessVedioSwitch(cInPort, cOutPort, eSWITCH_OFF);
                return (CH_CORRECT);
            }
        #else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    //KeyInput[acOutPortList[i]]=cInPort+1;
                    KeyLedSetOne(acOutPortList[i],cInPort+1);
                    #endif
                    i++;
                }
            }
            if (i > 0)
            {
		        ProessVedioSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
        #endif
        }

        pp = StrCmp(USART1_rxtailpoint, "1V", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 1;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: 01->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;

                }
            }
            if (i > 0)
            {
                // 1V1,2,3,4.   AV:1->1 2 3 4
//              cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:1->");
//              for (j = 0; j < i; j++)
//              {
//                  cCount += SiiSprintf((char*)&acMessage2[cCount], "%d ", acOutPortList[i] + 1);
//
//              }

                //appUIUartMessageSet(NULL, acMessage2, TRUE);
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                //appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); //1V
#endif
				ProessVedioSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);

            }
#endif

        }

        pp = StrCmp(USART1_rxtailpoint, "2V", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 2;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: 02->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }

#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;

                }
            }
            if (i > 0)
            {
                // 1V1,2,3,4.   AV:1->1 2 3 4
//              cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:1->");
//              for (j = 0; j < i; j++)
//              {
//                  cCount += SiiSprintf((char*)&acMessage2[cCount], "%d ", acOutPortList[i] + 1);
//
//              }

                //appUIUartMessageSet(NULL, acMessage2, TRUE);
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                //appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList);//2V
#endif
				ProessVedioSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);

            }
#endif


        }

        pp = StrCmp(USART1_rxtailpoint, "3V", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 3;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: 03->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;

                }
            }
            if (i > 0)
            {
                // 1V1,2,3,4.   AV:1->1 2 3 4
//              cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:1->");
//              for (j = 0; j < i; j++)
//              {
//                  cCount += SiiSprintf((char*)&acMessage2[cCount], "%d ", acOutPortList[i] + 1);
//
//              }

                //appUIUartMessageSet(NULL, acMessage2, TRUE);
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                //appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList);//3V
#endif
				ProessVedioSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);

            }
#endif
        }

        pp = StrCmp(USART1_rxtailpoint, "4V", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 4;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: 04->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioSwitch(cInPort, cOutPort, eSWITCH_ON);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;

                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); //4V
#endif
				ProessVedioSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);

            }
#endif

        }


        pp = StrCmp(USART1_rxtailpoint, "5V", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 5;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: 05->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioSwitch(cInPort, cOutPort, eSWITCH_ON);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;

                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); //4V
#endif
				ProessVedioSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);

            }
#endif

        }



        pp = StrCmp(USART1_rxtailpoint, "6V", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 6;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: 06->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioSwitch(cInPort, cOutPort, eSWITCH_ON);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;

                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); //4
#endif
				ProessVedioSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);

            }
#endif

        }


        pp = StrCmp(USART1_rxtailpoint, "7V", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 7;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: 07->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioSwitch(cInPort, cOutPort, eSWITCH_ON);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;

                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); //4V
#endif
				ProessVedioSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);

            }
#endif

        }


        pp = StrCmp(USART1_rxtailpoint, "8V", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 8;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: 08->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioSwitch(cInPort, cOutPort, eSWITCH_ON);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;

                }
            }
            if (i > 0)
            {
                // 1V1,2,3,4.   AV:1->1 2 3 4
//              cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:1->");
//              for (j = 0; j < i; j++)
//              {
//                  cCount += SiiSprintf((char*)&acMessage2[cCount], "%d ", acOutPortList[i] + 1);
//
//              }

                //appUIUartMessageSet(NULL, acMessage2, TRUE);
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                //appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); //4V
#endif
				ProessVedioSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);

            }
#endif

        }
//----------------------------------------------------------------------------------------
#ifdef PRD_IR_OUT
// [x1]B[x2].  ��[x1]·���뵽[x2]·���������Ƶ������ͬ����
        pp = StrCmp(USART1_rxtailpoint, "0B", USART1_rxprocesspoint);
        if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
        {
            cInPort = MAX_IN+1;
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d Closed.", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulClose);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort;
                #endif
				ProessVedioIRSwitch(cInPort, cOutPort, eSWITCH_OFF);
                return (CH_CORRECT);
            }
        }

        pp = StrCmp(USART1_rxtailpoint, "1B", USART1_rxprocesspoint);
        if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
        {
            cInPort = 1;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV: 01->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
 				ProessVedioIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); // 1B
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessVedioIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }

        pp = StrCmp(USART1_rxtailpoint, "2B", USART1_rxprocesspoint);
        if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
        {
            cInPort = 2;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "AV: 02->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); // 2B
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessVedioIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }

        pp = StrCmp(USART1_rxtailpoint, "3B", USART1_rxprocesspoint);
        if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
        {
            cInPort = 3;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "AV: 03->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); // 3B
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessVedioIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }

        pp = StrCmp(USART1_rxtailpoint, "4B", USART1_rxprocesspoint);
        if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
        {
            cInPort = 4;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "AV: 04->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); // 4B
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessVedioIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }
        pp = StrCmp(USART1_rxtailpoint, "5B", USART1_rxprocesspoint);
        if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
        {
            cInPort = 5;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "AV: 05->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); // 4B
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessVedioIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }
		        pp = StrCmp(USART1_rxtailpoint, "6B", USART1_rxprocesspoint);
        if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
        {
            cInPort = 6;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "AV: 06->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); // 4B
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessVedioIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }
	pp = StrCmp(USART1_rxtailpoint, "7B", USART1_rxprocesspoint);
        if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
        {
            cInPort = 7;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "AV: 07->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); // 4B
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessVedioIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }
		        pp = StrCmp(USART1_rxtailpoint, "8B", USART1_rxprocesspoint);
        if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
        {
            cInPort = 8;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "AV: 08->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                KeyInput[cOutPort]=cInPort-1;
                #endif
				ProessVedioIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyInput[acOutPortList[i]]=cInPort-1;
                    #endif
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,TRUE);
                appSystemSwitcherPortSet(cInPort, i, acOutPortList); // 4B
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessVedioIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }

//----------------------------------------------------------------------------------------
        pp = StrCmp(USART1_rxtailpoint, "0R", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = MAX_IN;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "%2d Closed.", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulClose);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                //appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
				ProessIRSwitch(cInPort, cOutPort, eSWITCH_OFF);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,FALSE);
                //appSystemSwitcherPortSet(cInPort, i, acOutPortList);
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }

        pp = StrCmp(USART1_rxtailpoint, "1R", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 0;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "IR: 01->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                //appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
				ProessIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,FALSE);
                //appSystemSwitcherPortSet(cInPort, i, acOutPortList);
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }

        pp = StrCmp(USART1_rxtailpoint, "2R", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 1;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "IR: 02->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                //appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
				ProessIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,FALSE);
                //appSystemSwitcherPortSet(cInPort, i, acOutPortList);
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }

        pp = StrCmp(USART1_rxtailpoint, "3R", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 2;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "IR: 03->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                //appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
				ProessIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,FALSE);
                //appSystemSwitcherPortSet(cInPort, i, acOutPortList);
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }

        pp = StrCmp(USART1_rxtailpoint, "4R", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 3;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "IR: 04->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                //appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
				ProessIRSwitch(cInPort, cOutPort, eSWITCH_ON);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,FALSE);
                //appSystemSwitcherPortSet(cInPort, i, acOutPortList);
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }
///////////////////////
         pp = StrCmp(USART1_rxtailpoint, "5R", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 4;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "IR: 05->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                //appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
				ProessIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,FALSE);
                //appSystemSwitcherPortSet(cInPort, i, acOutPortList);
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }
        ///////////////////////
         pp = StrCmp(USART1_rxtailpoint, "6R", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 5;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "IR: 06->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                //appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
				ProessIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,FALSE);
                //appSystemSwitcherPortSet(cInPort, i, acOutPortList);
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }
        ///////////////////////
         pp = StrCmp(USART1_rxtailpoint, "7R", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 6;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "IR: 07->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                //appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
				ProessIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,FALSE);
                //appSystemSwitcherPortSet(cInPort, i, acOutPortList);
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }
        ///////////////////////
         pp = StrCmp(USART1_rxtailpoint, "8R", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            cInPort = 7;
#ifndef SWITCH_CMD_COTINUE
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT &&
                *pp == '.')
            {
#if 0
                cOutPort = cOutPort - '0' - 1;
                cCount += SiiSprintf(&acMessage2[cCount], "IR: 08->%2d", cOutPort + 1);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(cInPort + 1, cOutPort + 1, ulChannelSwitch);
                appUIStateChange(uiUART_SWITCH_VIDEO);
                //appSystemSwitcherSet(cInPort, 1, cOutPort);
                appSystemIRSwitcherSet(cInPort, 1, cOutPort);
#endif
				ProessIRSwitch(cInPort, cOutPort, eSWITCH_ON);
                return (CH_CORRECT);
            }
#else
            BYTE acOutPortList[MAX_OUT];
            i = 0;
            while (*pp != '.' && i < MAX_OUT)
            {
                cOutPort = *pp++;
                if ((cOutPort - '0') > 0 &&
                    (cOutPort - '0') <= MAX_OUT)
                {
                    acOutPortList[i] = cOutPort - '0' - 1;
                    i++;
                }
            }
            if (i > 0)
            {
#if 0
                appUartSwitchInfoSet(cInPort, i, acOutPortList,FALSE);
                //appSystemSwitcherPortSet(cInPort, i, acOutPortList);
                appSystemIRSwitcherPortSet(cInPort, i, acOutPortList);
#endif
				ProessIRSwitch(cInPort, i, acOutPortList);
                return (CH_CORRECT);
            }
#endif
        }

#endif

    #if INC_CEC
        #if 1
    //[kyq Begin: 20150729 16:38:03]
    //dddCEC123.
    //CEC12.
    //Command: CEC[PORT][OPCODE][ARGS].
    //CEC: this is CEC command
    //PORT:BIT4: 0 REMOTE, 1: LOCAL
    //         BIT3~0: outputPort number
    //OPCODE: CEC opcode, CEC ������
    // ARGS:  CEC ���� ���16���ֽ�
    // . : end char ������
        if (StrCmp(USART1_rxtailpoint, "CEC", USART1_rxprocesspoint) != NULL)
        {
            uint8_t cPort = 0;
            uint8_t opCode = 0;
            uint8_t i = 0;
            uint8_t Arg[16]={0};
            uint8_t ArgCnt = 0;
            //BYTE   cInstance;
            //BYTE   cLocal;
            //BYTE cAdress = 0;       // wf8421 20160111
            temppoint = USART1_rxtailpoint + 3; //     SiiCecSendUserControlCmd();

            while (*(temppoint + i) != '.')
            {

                cmddata = *(temppoint + i);
                if (i == 0)
                {
                    cPort = *(temppoint + i);
                }
            #if 0
                else if(i == 1)
                    cAdress= *(temppoint + i);
                else if(i == 2)
                    opCode = *(temppoint + i);
                else
                {
                    Arg[i - 3] = *(temppoint + i);
                    ArgCnt++;
                }
            #else
                else if (i == 1)
                    opCode = *(temppoint + i);
                else
                {
                    Arg[i - 2] = *(temppoint + i);
                    ArgCnt++;
                }
            #endif
                i++;
                if (temppoint + i >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    break;
            } // end while

            return ProCECCommand(cPort, opCode, i, ArgCnt, Arg);
            //return ProCECCommand2(cPort,cAdress, opCode, i, ArgCnt, Arg);
        }
        #endif

        #if 0
        //  CEC X:<AA:XX......>.
        if((StrCmp(USART1_rxtailpoint, "CEC ", USART1_rxprocesspoint) != NULL))
        {
            uint8_t cecop=0,Args[16]={0},Addr=0,ArgCnt=0;
            uint8_t temp[4]={0};
            uint8_t cPort = 0;
            #define UserDataOffset(x)   (x)
            temppoint = USART1_rxtailpoint + 4;
            cPort = *(temppoint + UserDataOffset(0))-0x30;
            // port
            #if (MACHINETYPE == MUH4K862)
            if(cPort<=0||(cPort>MAX_OUT-2)||(*(temppoint + UserDataOffset(1))!=':')||(*(temppoint + UserDataOffset(2))!='<'))
            #else
            if(cPort<=0||cPort>MAX_OUT||(*(temppoint + UserDataOffset(1))!=':')||(*(temppoint + UserDataOffset(2))!='<'))
            #endif
            {
                return (CM_ERROR);
            }
            // addr and opcode
            temp[0]=*(temppoint + UserDataOffset(3));temp[1]=*(temppoint + UserDataOffset(4));temp[2]=*(temppoint + UserDataOffset(6));temp[3]=*(temppoint + UserDataOffset(7));
            if((HexAndAsciiChange(*(temppoint + UserDataOffset(3)),0)==temp[0])
                ||(HexAndAsciiChange(*(temppoint + UserDataOffset(4)),0)==temp[1])
                ||(HexAndAsciiChange(*(temppoint + UserDataOffset(6)),0)==temp[2])
                ||(HexAndAsciiChange(*(temppoint + UserDataOffset(7)),0)==temp[3]))
            {
                return (CM_ERROR);
            }

            Addr=(HexAndAsciiChange(*(temppoint + UserDataOffset(3)),0)<<4)|(HexAndAsciiChange(*(temppoint + UserDataOffset(4)),0));
            cecop=(HexAndAsciiChange(*(temppoint + UserDataOffset(6)),0)<<4)|(HexAndAsciiChange(*(temppoint + UserDataOffset(7)),0));
            i=0;
            while((*(temppoint + UserDataOffset(8)+i)!='>')&&ArgCnt<16)
            {
                if(*(temppoint + UserDataOffset(8)+i)==':')
                {
                    temp[0]=*(temppoint + UserDataOffset(9)+i);temp[1]=*(temppoint + UserDataOffset(10)+i);
                    if((HexAndAsciiChange(*(temppoint + UserDataOffset(9)+i),0)==temp[0])
                        ||(HexAndAsciiChange(*(temppoint + UserDataOffset(10)+i),0)==temp[1]))
                    {
                        return (CM_ERROR);
                    }
                    Args[ArgCnt++]=(HexAndAsciiChange(*(temppoint + UserDataOffset(9)+i),0)<<4)|(HexAndAsciiChange(*(temppoint + UserDataOffset(10)+i),0));
                }
                i++;
            }

            //sprintf(USART1_tx_buff, "CEC %d:<%02d:%02d",cPort);
            //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

            #if 1
            cCount += SiiSprintf((char*)&USART1_tx_buff[cCount], "%s%d:<%02x:%02x","CEC ",cPort,Addr,cecop);
            if(ArgCnt>0)
            {
                cCount +=SiiSprintf((char*)&USART1_tx_buff[cCount], ":");
                for(i=0;i<ArgCnt;i++)
                {
                    cCount +=SiiSprintf((char*)&USART1_tx_buff[cCount], "%02x",Args[i]);
                    if((i+1)<ArgCnt)
                        cCount +=SiiSprintf((char*)&USART1_tx_buff[cCount], ":");
                }
            }
            cCount +=SiiSprintf((char*)&USART1_tx_buff[cCount], ">");

            UART1_SendData2PC((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
            #endif

            ProCECCommand2(cPort, Addr, cecop,3, ArgCnt,Args);

            #undef UserDataOffset(x)
            return CH_CORRECT;
        }
        #endif
    #endif

        /////////////////////////////////////////////////////////
//=========================================================================

        //PC���ڵ�������Զ��ĳ��402
        /* :   /+[Y]/[X]:******.
           PC��Զ��402����Э�飬

           [Y]Ϊ1��2��3��4,5,6,7,8��ʾ�Զ�ӦԶ�˷���ָ�������ʵ����ݣ�
                                                     Ϊ9��ʱ��
                                                      ��ʾ��8·Զ��ͬʱ�������ݣ�
                    A,B,C,Dʱ��ʾ�趨��PWONʱ����Ӧ��1��2��3��4,·Զ�˷��͵�����
                    I,J,K,L  ʱ��ʾ�趨��PWOFFʱ����Ӧ��1��2��3��4·Զ�˷��͵�����?
          [X]Ϊ1--2400��2--4800��3--9600��4--19200��5--38400��6--57600��7--115200��
          [X]��ʾ������
          ****��ʾ���ݣ����֧��48�ֽڣ���
        */
        pp = StrCmp(USART1_rxtailpoint, "/+", USART1_rxprocesspoint);
       if (pp != NULL)
        {
            BYTE cBraud, cRemote;
            cRemote = *pp; // Y
            if ((cRemote > '0') && (cRemote <=( '0'+MAX_OUT+1))) //if  MAX_OUT+1 send All port
            {
                cRemote -= 0x30;
                pp++;
                if (*pp != '/')
                    return (CM_ERROR);
                pp++;
                cBraud = *pp - 0x30;  // X
                if ((cBraud > 0) && (cBraud <= 7))  // 1--2400��2--4800��3--9600��4--19200��5--38400��6--57600��7--115200
                {
                    pp++;
                    if (*pp != ':')
                        return (CM_ERROR);
                    pp++;
                    count = 0;
                    //βָ���1
                    if (USART1_rxprocesspoint > USART1_RX_BUFF)
                        USART1_rxprocesspoint--;
                    else
                        USART1_rxprocesspoint = (USART1_RX_BUFF + USART1_RX_BUFF_MAX - 1);
                    while (pp != USART1_rxprocesspoint)
                    {
                        uart_data_temp[count] = *pp;
                        count++;
                        pp++;
                        if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                            pp = USART1_RX_BUFF;
                    }

#if(MACHINETYPE!=PRD_VHD4)
                    UART1_SendData2PC(uart_data_temp, count);
#endif

                    dvCpldSendDataToRemote(cRemote,
                                           cBraud,
                                           uart_data_temp,
                                           count);

                    return (CH_CORRECT);
                }
                else
                    return (CM_ERROR);
            }
#ifdef PRD_POWER_ONOFF_MESSAGE
            else if ((cRemote >= 'a') && (cRemote <= 'h') || // A,B,C,D,ʱ��ʾ�趨��PWONʱ����Ӧ��Զ�˷��͵����ݣ�
                     (cRemote >= 'A') && (cRemote <= 'H'))  //  E,F,G,H  ʱ��ʾ�趨��PWOFFʱ����Ӧ��1��2��3��4·Զ�˷��͵�����?
            {
                BYTE cbPowrON;
                if((cRemote >= 'a') && (cRemote <= 'h'))
                {
                    cbPowrON = FALSE;
                    cRemote -= 'a';
                }
                else if((cRemote >= 'A') && (cRemote <= 'H'))
                {
                    cbPowrON = TRUE;
                    cRemote -= 'A';
                }
                pp++;
                if (*pp != '/')
                    return (CM_ERROR);
                pp++;
                cBraud = *pp - 0x30;
                if ((cBraud > 0) && (cBraud <= 7))// 1--2400��2--4800��3--9600��4--19200��5--38400��6--57600��7--115200
                {
                    pp++;
                    if (*pp != ':')
                        return (CM_ERROR);
                    pp++;
                    count = 0;
                    //βָ���1
                    if (USART1_rxprocesspoint > USART1_RX_BUFF)
                        USART1_rxprocesspoint--;
                    else
                        USART1_rxprocesspoint = (USART1_RX_BUFF + USART1_RX_BUFF_MAX - 1);
                    while (pp != USART1_rxprocesspoint)
                    {
                        uart_data_temp[count] = *pp;
                        count++;
                        pp++;
                        if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                            pp = USART1_RX_BUFF;
                    }
#if 0
                    psSystemSettingData = utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                    if (cbPowrON)
                    {

                        psSystemSettingData->acPowerOnMsgBaudRate[cRemote] = cBraud;
                        psSystemSettingData->acPowerOnMsgLength[cRemote] = count;
                        for (i = 0; i < count; i++)
                        {
                            psSystemSettingData->acPowerOnMsg[cRemote][i] = uart_data_temp[i];
                        }
                        psSystemSettingData->acPowerOnMsg[cRemote][i] = '\0';

                    }
                    else
                    {
                        psSystemSettingData->acPowerOffMsgBaudRate[cRemote] = cBraud;
                        psSystemSettingData->acPowerOffMsgLength[cRemote] = count;
                        for (i = 0; i < count; i++)
                        {
                            psSystemSettingData->acPowerOffMsg[cRemote][i] = uart_data_temp[i];
                        }
                        psSystemSettingData->acPowerOffMsg[cRemote][i] = '\0';
                    }
                    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);

                    UART1_SendData2PC(uart_data_temp, count);
#endif
					ProcessSendData2Remote(cRemote, cBraud, uart_data_temp, count, cbPowrON);
                    return (CH_CORRECT);
                }
                else
                    return (CM_ERROR);
            }
#endif
            else
                return (CM_ERROR);
        }
//================================================================================================
//[x]All.	[x]·���뵽����·�����
//All#.	����Ϊ����ͨ��һһ��Ӧ���磺1->1, 2->2, 3->3��
//All$.	�ر�����ͨ����

        temppoint = USART1_rxtailpoint;
        tempptr = StrCmp(temppoint, "All", USART1_rxprocesspoint);
        if (tempptr != NULL)
        {
            while (*tempptr != '.')
            {
                tempptr++;
                if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    tempptr = USART1_RX_BUFF;
                jiucuo++;
                if (jiucuo > 1)
                    return (CM_ERROR);
            }
            tempptr = StrCmp(temppoint, "All", USART1_rxprocesspoint);
            if (*tempptr == '#') //if (*tempptr == 0x23)
            { //   '#' ȫ��ֱͨ// ����Ϊ����ͨ��һһ��Ӧ���磺1->1, 2->2, 3->3��
                /****************/

#if 0
		bEvent.Head.opcode=eSwitchAllThrough;
		bEvent.Head.DestAddr=mqSWITCH;
		bEvent.Head.argCount=0;
	       utilExecOlnyMsgSend(mqSWITCH,
                            bEvent);
               /****************/
/*
		   for (OutPut = 0; OutPut < MAX_OUT; OutPut++)
                {
                    InPut = OutPut;
                    appSystemSwitcherSet(InPut, 1, OutPut); //ALL#.
#ifdef PRD_IR_OUT
                    appSystemIRSwitcherSet(InPut, 1, OutPut); //ALL#.
#endif
                }
 */  //MSG send
               /****************/
		//Event_t   bEvent;
		bEvent.Head.opcode=eLEDAllThrough;
		bEvent.Head.DestAddr=mqLED;
		bEvent.Head.argCount=0;
	       utilExecOlnyMsgSend(mqLED,
                            bEvent);
               /****************/
                cCount += SiiSprintf(&acMessage2[cCount],"%s", "All Through.");
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(InPut, OutPut, ulAllThrough);
                appUIStateChange(uiUART_SWITCH_VIDEO);
#endif
				ProcessAllThrough();
                return (CH_CORRECT);
            }
            else if (*tempptr == '$')   // else if (*tempptr == 0x24)                          // '$' ȫ���ر�
            {

#if 0
                appSystemSwitcherOutportCloseAll(); // video and IR
                cCount += SiiSprintf(&acMessage2[cCount], "%s","All Closed.");
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(1, 1, ulAllClose);
                appUIStateChange(uiUART_SWITCH_VIDEO);
#endif
				ProcessAllClose();
                return (CH_CORRECT);
            }
            else if (*tempptr == '@')  // '@' ȫ������ //All@.
            {

#if 0
                appSystemSwitcherOutportRecoverAll();// Video and IR

                if (Re_Message == ON)
                {
                    appUIUartMessageSet(NULL, "All Open.", TRUE);
                    appUartSwitchLedStateChange(1, 1, ulAllOpen);
                    appUIStateChange(uiUART_SWITCH_VIDEO);
                }
                else
                {
                    appUIUartMessageSet(NULL, "Switch Ok!", TRUE);
                    appUartSwitchLedStateChange(1, 1, ulAllOpen);
                    appUIStateChange(uiUART_SWITCH_VIDEO);
                }
#endif
				ProcessAllOpen(Re_Message);
                return (CH_CORRECT);
            }
            InPut = 0;
            while (*tempptr != '.')
            {
                tempptr++;
                if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    tempptr = USART1_RX_BUFF;
                jiucuo++;
                if (jiucuo >= 1)
                    return (CM_ERROR);
            }
            temppoint = USART1_rxtailpoint;
            while (*temppoint != 'A')                          // xAll.  [x]·���뵽����·�����
            {
                if ((*temppoint >= '0') && (*temppoint <= '9'))  // 0 - 9
                {
                    InPut = InPut * 10 + *temppoint - 0x30;
                    returnval = 1;
                }
                else
                {
                    returnval = 0;
                    InPut = 0;
                }
                temppoint++;
                if (temppoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    temppoint = USART1_RX_BUFF;
            } // end of while(*temppoint != 'A')
            if (returnval == 0)
                return (CH_ERROR);
            if ((InPut > 0) && (InPut <= MAX_IN))  // һ·���뵽����·���
            {

#if 0
                appSystemSwitcherSet(InPut - 1, MAX_OUT, 0, 1, 2, 3,4,5,6,7); // xAll.
#ifdef PRD_IR_OUT
                appSystemIRSwitcherSet(InPut - 1, MAX_OUT, 0, 1, 2, 3,4,5,6,7);
#endif

                cCount += SiiSprintf(&acMessage2[cCount], "%2d To All.", InPut);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUartSwitchLedStateChange(InPut, InPut, ulToAll);
                appUIStateChange(uiUART_SWITCH_VIDEO);
#endif
                #if 0//((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                for(i=0;i<MAX_OUT;i++)
                {
                    KeyInput[i]=InPut-1;
                }
                #endif
				ProcessOne2All(InPut);
                return (CH_CORRECT);
            }
            else
                return (CH_ERROR);
        }
        ////////////////////////////////////////////////////////////////////
//================================================================================
////////////////////////////////////////////////////////////////////
//[x1]V[x2].
// [x]·�����[x]·�����
        temppoint = USART1_rxtailpoint;
        if (StrCmp(temppoint, "#", USART1_rxprocesspoint) != NULL)      // '#': Through
        {
            OutPut = 0;
            temppoint = USART1_rxtailpoint;
            while (*temppoint != '.')
            {
                cmddata = *temppoint;
                if ((cmddata > '/') && (cmddata < ':'))       // number 0 - 9
                {
                    OutPut = OutPut * 10 + cmddata - 0x30;
                }
                if ((cmddata == ',') || (cmddata == '#'))
                {
                    if ((OutPut > 0) && (OutPut <= MAX_OUT))
                    {
                        InPut = OutPut;
#if 0
                        appSystemSwitcherSet(InPut - 1, 1, OutPut - 1);
                        cCount += SiiSprintf(&acMessage2[cCount], "%2d Through.", InPut);
                        appUIUartMessageSet(NULL, acMessage2, TRUE);
                        appUartSwitchLedStateChange(InPut, InPut, ulThrough);
                        appUIStateChange(uiUART_SWITCH_VIDEO);
#endif
						ProcessOne2One(InPut, OutPut);
                    }
                    else
                        return (CH_ERROR);
                    OutPut = 0;
                }
                temppoint++;
                if (temppoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    temppoint = USART1_RX_BUFF;
            } // end of while
            return (CH_CORRECT);
        }
        ///////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////
//================================================================================
// [x]$.	    �ر�[x]·�����
        temppoint = USART1_rxtailpoint;
        if (StrCmp(temppoint, "$", USART1_rxprocesspoint) != NULL)      // '$': close
        {
            OutPut = 0;
            temppoint = USART1_rxtailpoint;
            while (*temppoint != '.')
            {
                cmddata = *temppoint;
                if ((cmddata > '/') && (cmddata < ':'))       // number 0 - 9
                {
                    OutPut = OutPut * 10 + cmddata - 0x30;
                }
                if ((cmddata == ',') || (cmddata == '$'))
                {
                    if ((OutPut > 0) && (OutPut <= MAX_OUT))
                    {
#if 0
                        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
                        if ((psOutputPortMgrData->acInPortSelect[OutPut - 1]) < MAX_IN)
                            psOutputPortMgrData->acCloseInPortSelect[OutPut - 1] = psOutputPortMgrData->acInPortSelect[OutPut - 1];
#ifdef PRD_IR_OUT
                        if ((psOutputPortMgrData->acIRInPortSelect[OutPut - 1]) < MAX_IN)
                            psOutputPortMgrData->acIRCloseInPortSelect[OutPut - 1] = psOutputPortMgrData->acIRInPortSelect[OutPut - 1];

#endif
                        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                                          TRUE);
                        appSystemSwitcherSet(MAX_IN, 1, OutPut - 1);
#ifdef PRD_IR_OUT
			            appSystemIRSwitcherSet(MAX_IN, 1, OutPut - 1);
#endif

                        cCount += SiiSprintf(&acMessage2[cCount], "%2d Closed.", OutPut);
                        appUIUartMessageSet(NULL, acMessage2, TRUE);
                        appUartSwitchLedStateChange(OutPut, OutPut, ulClose);
                        appUIStateChange(uiUART_SWITCH_VIDEO);
#endif
						ProcessOneClose(OutPut);
                    }
                    else
                        return (CH_ERROR);
                    OutPut = 0;
                }
                temppoint++;
                if (temppoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    temppoint = USART1_RX_BUFF;
            } // end of while
            return (CH_CORRECT);
        }
//===================================================================================
//[x]@.	����[x]·�����
        temppoint = USART1_rxtailpoint;
        pp = StrCmp(temppoint, "@", USART1_rxprocesspoint);
        if (pp != NULL)      // '@': open
        {
            while (*pp != '.')
            {
                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;
                jiucuo++;
                if (jiucuo >= 1)
                    return (CM_ERROR);
            }
            OutPut = 0;
            temppoint = USART1_rxtailpoint;
            while (*temppoint != '.')
            {
                cmddata = *temppoint;
                if ((cmddata >= '0') && (cmddata <= '9'))       // number 0 - 9
                {
                    OutPut = OutPut * 10 + cmddata - 0x30;
                }
                if ((cmddata == ',') || (cmddata == '@'))
                {
                    if ((OutPut > 0) && (OutPut <= MAX_OUT))
                    {
#if 0
                        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);

                        if ((psOutputPortMgrData->acInPortSelect[OutPut - 1]) >= MAX_IN)
                        {
                            psOutputPortMgrData->acInPortSelect[OutPut - 1] = psOutputPortMgrData->acCloseInPortSelect[OutPut - 1];
#ifdef PRD_IR_OUT
                            psOutputPortMgrData->acIRInPortSelect[OutPut - 1] = psOutputPortMgrData->acIRCloseInPortSelect[OutPut - 1];
#endif

                            utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                                              TRUE);
                            appSystemSwitcherSet(psOutputPortMgrData->acInPortSelect[OutPut - 1], 1, OutPut - 1);
                        }
                        else
                            utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                                              TRUE);
                        if (Re_Message == ON)
                        {
                            cCount += SiiSprintf(&acMessage2[cCount], "%02d Open.", OutPut);
                            appUIUartMessageSet(NULL, acMessage2, TRUE);
                            appUartSwitchLedStateChange(OutPut, OutPut, ulOpen);
                            appUIStateChange(uiUART_SWITCH_VIDEO);

                        }
                        else
                        {

                            cCount += SiiSprintf(&acMessage2[cCount], "Switch Ok!");
                            appUIUartMessageSet(NULL, acMessage2, TRUE);
                            appUartSwitchLedStateChange(OutPut, OutPut, ulOpen);
                            appUIStateChange(uiUART_SWITCH_VIDEO);

                        }
#endif
						ProcessOneOpen(OutPut, Re_Message);
                    }
                    else
                        return (CH_ERROR);
                    OutPut = 0;
                }
                temppoint++;
                if (temppoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    temppoint = USART1_RX_BUFF;
            } // end of while
            return (CH_CORRECT);
        }
        temppoint = USART1_rxtailpoint;
//=========================================================================================
//Save[Y].	���浱ǰ״̬����[Y]�飬[Y]Ϊ0 - 9 ���ּ���

        tempptr = StrCmp(temppoint, "Save", USART1_rxprocesspoint);    // Save to 24C32
        if (tempptr != NULL)
        {
            while (*tempptr != '.')
            {
                if ((*tempptr > '/') && (*tempptr < ':')) // number 0 - 9
                {
                    InPut = InPut * 10 + *tempptr - 0x30;
                    i = 0x55;
                }

                tempptr++;
                if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    tempptr = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 4)
                    return (CM_ERROR); //���֧��3λ����λ
            }

            if ((InPut < 10) && (i == 0x55))
            {
#if 0
                cCount += SiiSprintf(&acMessage2[cCount], "Save To F%d", InPut);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUIStateChange(uiUART_MESSAGE);


                appSystemSwitcherScenceSave(InPut);
#endif
 				ProcessSaveCurrentStatus(InPut);
            }
            else
                return (CM_ERROR); //���֧��3λ����λ
            return (CH_CORRECT);
        }
//==================================================================================
//Recall[Y].	�ӵ�[Y]��ָ���[Y]Ϊ0�C9���ּ���

        temppoint = USART1_rxtailpoint;
        tempptr = StrCmp(temppoint, "Recall", USART1_rxprocesspoint);  // Recall from 24C32
        if (tempptr != NULL)
        {
            while (*tempptr != '.')
            {
                if ((*tempptr > '/') && (*tempptr < ':')) // number 0 - 9
                {
                    InPut = InPut * 10 + *tempptr - 0x30;
                    i = 0x55;
                }

                tempptr++;
                if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    tempptr = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 4)
                    return (CM_ERROR); //���֧��3λ����λ
            }

            if ((InPut < 10) && (i == 0x55))
            {

#if 0
                cCount += SiiSprintf(&acMessage2[cCount], "Recall From F%d", InPut);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUIStateChange(uiUART_MESSAGE);

                appSystemSwitcherScenceCall(InPut);
#endif
				ProcessRecallStatus(InPut);
            }
            else
                return (CM_ERROR); //���֧��3λ����λ
            return (CH_CORRECT);
        }
//====================================================================
        temppoint = USART1_rxtailpoint;
        tempptr = StrCmp(temppoint, "Clear", USART1_rxprocesspoint);   // Clear 24C32
        if (tempptr != NULL)
        {
            while (*tempptr != '.')
            {
                if ((*tempptr > '/') && (*tempptr < ':')) // number 0 - 9
                {
                    InPut = InPut * 10 + *tempptr - 0x30;
                    i = 0x55;
                }

                tempptr++;
                if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    tempptr = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 4)
                    return (CM_ERROR); //���֧��3λ����λ
            }

            if ((InPut < 10) && (i == 0x55))
            {

#if 0
                cCount += SiiSprintf(&acMessage2[cCount], "Clear F%d", InPut);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUIStateChange(uiUART_MESSAGE);

                appSystemSwitcherScenceDel(InPut);
#endif
				ProcessClearStatus(InPut);
            }
            else
                return (CM_ERROR); //���֧��3λ����λ
            return (CH_CORRECT);
        }
//================================================================
//Status.	��ѯȫ�����ͨ���ĵ�ǰ״̬��
        temppoint = USART1_rxtailpoint;
        tempptr = StrCmp(temppoint, "Status", USART1_rxprocesspoint);   // Status
        if (tempptr != NULL)
        {
            if (*temppoint != 'S')
            {
                return (CH_ERROR);
            }
            OutPut = *(tempptr);
            if ((OutPut > 0x30) && (OutPut < (MAX_OUT + 0x31)))             // �鿴һ��ͨ��״̬
            {
                OutPut = OutPut - 0x30; // ASCII to Digital //[kyq add: 20150825 13:41:52]
                appUIUartMessageSwitcherPortSet(OutPut - 1);
                return (CH_CORRECT);
            }
            else if (*tempptr == '.')                                 // �鿴ȫ��ͨ��״̬
            {
                appUIUartMessageSwitcherPortSet(MAX_OUT);
                return (CH_CORRECT);
            }
            else
                return (CH_ERROR);
        }
/*************************************************************************************************************************/
	//I-Lock[x].	ָ������ͨ����������
		temppoint = USART1_rxtailpoint;
        tempptr = StrCmp(temppoint, "I-Lock", USART1_rxprocesspoint);   // Status
        if (tempptr != NULL)
        {
#if 0
        		POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
            	psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
        		psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
#endif
             	while (*tempptr != '.')
            	{
	                if ((*tempptr > '0') && (*tempptr <= ('0'+MAX_IN))) // number 0 - 9
	                {
	                    InPut =  *tempptr - 0x30;
	                }

                	tempptr++;
                	if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    	tempptr = USART1_RX_BUFF;

                	jiucuo++;
                	if (jiucuo > 1)
                    	return (CM_ERROR); //���֧��1λ����λ
                }

#if 0
                //POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
            	psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
        		psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
            	if ((InPut >0) && (InPut <= MAX_IN ))             // �鿴һ��ͨ��״̬
	            {
					psSystemSettingData->acIChannellock[InPut - 1]=0x00;//ˢ�¸�������ͨ��
					for(i=0;i<MAX_OUT;i++)
	            	{
	            		cInPort = psOutputPortMgrData->acInPortSelect[i];//�ж�ÿ������ڵ�ǰ������źţ�����Ǳ����������¼
						if(cInPort!=InPut-1)
						{
					 		psSystemSettingData->acIChannellock[InPut - 1] = psSystemSettingData->acIChannellock[InPut - 1]|BIT(i);	//0x0 xxxx
						}
	            	}

					utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,  TRUE);
					utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,  TRUE);
					sprintf(USART1_tx_buff,"Channel %d Lock!\r\n",InPut);
					UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
	            	return (CH_CORRECT);
	            }
            	else
                	return (CH_ERROR);
#endif
				return (cReturnValue = ProcessLockOneChannel(InPut));
        }
		  //================================================================
//I-Lock[x].	ָ������ͨ������
		temppoint = USART1_rxtailpoint;
        tempptr = StrCmp(temppoint, "I-UnLock", USART1_rxprocesspoint);   //
        if (tempptr != NULL)
        {
#if 0
        	  POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
		  psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
#endif
           while (*tempptr != '.')
            	{
                 if ((*tempptr > '0') && (*tempptr <= ('0'+MAX_IN))) // number 0 - 9
                {
                    InPut =  *tempptr - 0x30;

                }

                tempptr++;
                if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    tempptr = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 4)
                    return (CM_ERROR); //���֧��3λ����λ
            }
#if 0
            if ((InPut > 0) && (InPut <=MAX_IN ))
            {
			psSystemSettingData->acIChannellock[InPut - 1] =0x00; //0x0 xxxx
					 //OutPut = OutPut - 0x30; // ASCII to Digital //[kyq add: 20150825 13:41:52]
            	utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,TRUE);
			 sprintf(USART1_tx_buff,"Channel %d Unlock!\r\n",InPut);
                       UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                return (CH_CORRECT);
            }
            else
            {
				utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,TRUE);
				 return (CH_ERROR);
            }
#endif
			return (cReturnValue = ProcessUnlockOneChannel(InPut));
        }
//=============================================================================
//I-Lock All.��������ͨ��

        if (StrCmp(USART1_rxtailpoint, "A-Lock.", USART1_rxprocesspoint) != NULL)
        {
            if (*USART1_rxtailpoint != 'A')
            {
          //      return (CH_ERROR);
            }
#if 0
		 POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
		  psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
		   psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
		 for(i=0;i<MAX_IN;i++)
		 {
		 	psSystemSettingData->acIChannellock[i]=0x00;
			cInPort=i;
			for(j=0;j<MAX_OUT;j++)
            	{
            		if(cInPort != psOutputPortMgrData->acInPortSelect[j])//�ж�ÿ������ڵ�ǰ������źţ�����Ǳ����������¼
					psSystemSettingData->acIChannellock[i] = psSystemSettingData->acIChannellock[i]|(BIT(j));
			}

		 }
		  sprintf(USART1_tx_buff,"All Channel Lock!\r\n");
            UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
		  utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,  false);
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,TRUE);
#endif
			ProcessLockAllChannel();
            return (CH_CORRECT);
        }
//=============================================================================
//I-UnLock All.��������ͨ��
//
        if (StrCmp(USART1_rxtailpoint, "A-UnLock.", USART1_rxprocesspoint) != NULL)
        {
            if (*USART1_rxtailpoint != 'A')
            {
     //           return (CH_ERROR);
            }
#if 0
		 POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
		  psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
		 for(i=0;i<MAX_IN;i++)
		 {
			psSystemSettingData->acIChannellock[i]=0x00;
		 }
		  sprintf(USART1_tx_buff,"All Channel Unlock!\r\n");
            UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,TRUE);
#endif
			ProcessUnlockAllChannel();
            return (CH_CORRECT);
        }
//=============================================================================
//lock.	�鿴��ǰͨ������״̬
//
        if (StrCmp(USART1_rxtailpoint, "Lock-Sta.", USART1_rxprocesspoint) != NULL)
        {
            if (*USART1_rxtailpoint != 'S')
            {
           //     return (CH_ERROR);
            }
#if 0
		 POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
		  psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
		 for(j=0;j<MAX_OUT;j++)
		 {
			for(i=0;i<MAX_IN;i++)
			{
				if( (psSystemSettingData->acIChannellock[i]&BIT(j)))
				{
					sprintf(USART1_tx_buff,"Channel %d->%d Lock!\r\n",i+1,j+1);
            			UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
				}
				else
				{
					sprintf(USART1_tx_buff,"Channel %d->%d Unlock!\r\n",i+1,j+1);
            			UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
				}
			}

		 }
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,false);
#endif
			ProCheckAllLockStatus();
            return (CH_CORRECT);
        }
//=======================================================================
//=======================================================================
//Demo.D
        if (StrCmp(USART1_rxtailpoint, "Demo.", USART1_rxprocesspoint) != NULL) // DEMO
        {
         //   BOOL bDemoEn = appSystemSwitchDemoModeEnableGet();//alone in the module

#if 0
            if (bDemoEn)
                cCount += SiiSprintf(&acMessage2[cCount], "Normal Mode");

            else
                cCount += SiiSprintf(&acMessage2[cCount], "Demo Mode");

            UART1_SendData2PC(acMessage2, cCount);
	     appUIUartMessageSet(NULL, acMessage2, FALSE);//[kyq add: 20150924 15:49:38]
	      appUIStateChange(uiUART_MESSAGE);
/****************/
		//Event_t   bEvent;
		bEvent.Head.opcode=eSwitchDemoMode;
		bEvent.Head.DestAddr=mqSWITCH;
		bEvent.args[0]=!bDemoEn;
		bEvent.Head.argCount=1;
	       utilExecOlnyMsgSend(mqSWITCH,
                            bEvent);
/****************/
          bDemoEn=!bDemoEn;
        //    appSystemSwitchDemoModeEnableSet(!bDemoEn);  //msg send
          m_bCommdDemo = TRUE; // kyq 20150929
#endif
			ProSettingDemoStatus();
            return (CH_CORRECT);
        }
//=============================================================================
//Undo.	ȡ����ǰ������������һ���л���״̬��
// UUUUUUndo.
        if (StrCmp(USART1_rxtailpoint, "Undo.", USART1_rxprocesspoint) != NULL)             // Undo
        {
            if (*USART1_rxtailpoint != 'U')
            {
                return (CH_ERROR);
            }
            //SW_Undo();
            cCount += SiiSprintf((char*)&acMessage2[cCount], "%s","Undo Ok!");
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUIStateChange(uiUART_MESSAGE);

            appSystemSwitcherUndo();
            return (CH_CORRECT);
        }
//===============================================================================
#if 0
        if (StrCmp(USART1_rxtailpoint, "PWON.", USART1_rxprocesspoint) != NULL)
        {
            if (*USART1_rxtailpoint != 'P')
            {
                return (CH_ERROR);
            }
            PWON_process();


            return (CH_CORRECT);
        }
#endif


//===============================================================================
        if (StrCmp(USART1_rxtailpoint, "STANDBY.", USART1_rxprocesspoint) != NULL)
        {

            cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "STANDBY...");
            appUIUartMessageSet(NULL, acMessage2, FALSE);
            appUIStateChange(uiUART_MESSAGE);
            appPowerMgrPowerStateSet(pwrSTANDBY);
            #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
            KeyLedSetAllClose();
            #endif
            return (CH_CORRECT);
        }

//===============================================================================

        if (StrCmp(USART1_rxtailpoint, "PWOFF.", USART1_rxprocesspoint) != NULL)
        {
            cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "PWOFF");
            appUIUartMessageSet(NULL, acMessage2, FALSE);
            appUIStateChange(uiUART_MESSAGE);
            appPowerMgrPowerStateSet(pwrSOFTOFF);
            #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
            KeyLedSetAllClose();
            #endif
            return (CH_CORRECT);
        }
//===============================================================================
//          /%I/1:1.
// /%[Y]/[X]:[Z].
// HDCP����ָ�
// [Y]��ʾI/O��
// [x]��ʾ�˿ںţ�ALL��ʾ����ͨ������
// [Z]��ʾ״̬
//                          1���������ʾ��HDCP�����������ʾ����HDCP�豸��
//                          0���������ʾ�������HDCP  ���������ʾ������HDCP�豸��
        /****************************�������й�HDCP����ָ��**************************************/
        pp = StrCmp(USART1_rxtailpoint, "/%", USART1_rxprocesspoint);
        if (pp != NULL)   // ����HDCP
        {
            BYTE  cPort = 0;
            BOOL bIsInport = 0;
            BYTE cEnalbe = 0;
            BOOL bAllPort = 0;

            //���￪ʼ��������
            pp = StrCmp(USART1_rxtailpoint, "/%", USART1_rxprocesspoint);
            if (*pp == 'I')
                bIsInport = 1;
            else if (*pp == 'O')
                bIsInport = 0;
            else
                return (CH_CORRECT);

            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
            if (StrCmp(USART1_rxtailpoint, "ALL", USART1_rxprocesspoint) == NULL)
            {
                bAllPort = 0; //��ʾ�Ե���ͨ�����в���
                              //�ó�ͨ����
                while (*pp != ':')
                {
                    // �õ��˿ں�
                    if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                    {
                        cPort = cPort * 10 + *pp - 0x30;
                    }

                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;

                    jiucuo++;
                    if (jiucuo > 4)
                        return (CM_ERROR); //���֧��3λ����λ
                }

                if ((cPort == 0) || (cPort > MAX_IN))
                    return (CM_ERROR);
            }
            else
            {
                // �������ͨ��
                pp = StrCmp(USART1_rxtailpoint, "ALL", USART1_rxprocesspoint);
                bAllPort = 1;
            }
            //�ó�HDCP״̬
            pp++;
            jiucuo = 0;
            while (*pp != '.')
            {
                if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                {
                    cEnalbe = cEnalbe * 10 + *pp - 0x30;
                }
                else
                    return (CM_ERROR);

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 4)
                    return (CM_ERROR);

                if (cEnalbe > 1)
                    return (CM_ERROR);
            }
#if 0
            ////////////////////////////////////////////////////////
            if (bIsInport == 1) //��������HDCP����
            {
                psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnINPUTPORT_MAMAGER_DATA);
                //k = 0;
                if (bAllPort == 0)
                {
                    //  ���������ʾ������HDCP�豸��
                    //  ���벻֧�ִ�HDCP
                    psInputPortMgrData->abInputHDCP_EN[cPort - 1] = cEnalbe;
                }
                else // �������ͨ��
                {
                    for (i = 0; i < MAX_IN; i++)
                    {
                        psInputPortMgrData->abInputHDCP_EN[i] = cEnalbe;
                    }
                }
                utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA, TRUE);
            }
            else if (bIsInport == 0)  //  �����HDCP ����
            {
                psOutputPortMgrData = utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
                if (bAllPort == 0)
                {
                    psOutputPortMgrData->abHDCP_EN[cPort - 1] = cEnalbe;
                    psOutputPortMgrData->abHDCP_AUTO[cPort - 1] = FALSE;

                }
                else
                {
                    for (i = 0; i < MAX_OUT; i++)
                    {
                        psOutputPortMgrData->abHDCP_EN[i] = cEnalbe;
                        psOutputPortMgrData->abHDCP_AUTO[i] = FALSE;
                    }
                }
                utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, TRUE);

            }
#endif
			ProChannelHdcp(bIsInport, bAllPort, cPort, cEnalbe);

            pp++;
            memcpy(acMessage2, USART1_rxtailpoint, (pp - USART1_rxtailpoint));
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUIStateChange(uiUART_MESSAGE);

            return (CH_CORRECT);
        }
//=====================================================================================
//////////////////////////////////////////////////////////////////////////////
// pp = StrCmp(USART1_rxtailpoint, "Audio/", USART1_rxprocesspoint);

        //////////////////////////////////////////////////////////////////////////////////////////////
        ///EDIDMInit.
        ///�ָ���������EDID���ݵ�ÿ������˿�
        if (StrCmp(USART1_rxtailpoint, "EDIDMInit", USART1_rxprocesspoint) != NULL)
        {
#if 0
            cCount += SiiSprintf(&acMessage2[cCount],"%s", "EDIDMInit.");
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUIStateChange(uiUART_MESSAGE);


            //memcpy(g_edidFlashEdidTable, init_edidFlashEdidTable, EDID_TABLE_LEN);
            for (i = 0; i < MAX_IN; i++)      //Intialize 9679 EDID
                appSystemInputPortEdidSet(i, edsEDID_Default);
#endif
 			ProFactoryDefaultEDID();
            return (CH_CORRECT);
        }
//========================================================================================
// EDIDG[x].
//��ȡ����˿ڵ�EDID���ݲ�ͨ��������ʾ����[x]����˿ں�
        pp = StrCmp(USART1_rxtailpoint, "EDIDG", USART1_rxprocesspoint);
        if (pp != NULL)
        {

            while (*pp != '.')
            {
                if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                {
                    HPnum1 = HPnum1 * 10 + *pp - 0x30;
                }

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 3)
                    return (CM_ERROR); //���֧��3λ����λ
            }

#if 0
            if ((HPnum1 > 0) && (HPnum1 <= MAX_OUT))
            {

                cSii9533Instance = utilOutputPortToInstance(HPnum1 - 1);
                if (SiiTxStatusGet(cSii9533Instance).isHotPlugDetected) //[kyq add: 20150728 15:38:23]
                {
                    SiiTxEdidGetEdidData(cSii9533Instance, acEDID_data); //[kyq ADD: 20150728 15:30:37]

                    for (j = 0; j < EDID_TABLE_LEN; j++) UART1_SendByte(acEDID_data[j]);
                    UART1_SendByte(0x0D);
                    UART1_SendByte(0x0A);
                }
                else
                {

                    cCount += SiiSprintf(&acMessage2[cCount],"%s", "NO device !");
                    appUIUartMessageSet(NULL, acMessage2, TRUE);
                    appUIStateChange(uiUART_MESSAGE);

                }
            }
            else
            {
                return (CH_ERROR);
            }
            return (CH_CORRECT);
#endif
			return (cReturnValue = ProGetOutportEDIDData(HPnum1));
        }
//========================================================================================

        ////////////////////////////////////////////////////////////////////////////
        //��ȡ���ö�Ӧ��ŵ�EDID���� ��X��1?2?3?4,5,��ʾ�洢5������EDID,��ͨ��232��ʾ����
        pp = StrCmp(USART1_rxtailpoint, "GetIntEDID", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            HPnum1 = *pp - 0x30;
            if ((HPnum1 > 0) && (HPnum1 < 6))
            {

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;
                if (*pp == '.')
                {
#if 0
                    for (j = 0; j < EDID_TABLE_LEN; j++)
                    {
                        acEDID_data[j] = 0;
                    }
                    dvNVRAMUserEdidRead(HPnum1, acEDID_data);
                    for (j = 0; j < EDID_TABLE_LEN; j++) UART1_SendByte(acEDID_data[j]);

                    UART1_SendByte(0x0D);
                    UART1_SendByte(0x0A);
#endif
					ProGetEmbeddingEDIDData(HPnum1);
                    return (CH_CORRECT);
                }
                else
                    return (CM_ERROR);
            }
            else
                return (CM_ERROR);
        }

//========================================================================================

        ////////////////////////////////////////////////////////////////////////////
        ///��ȡ��ǰ��Ӧ������ͨ����EDID���� ��X��1?2?3?4��ʾ��ǰ��Ӧ������˿�?��ͨ��232��ʾ����
        pp = StrCmp(USART1_rxtailpoint, "GetInPortEDID", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            //BYTE cEdidSelect;
            HPnum1 = *pp - 0x30;
            if ((HPnum1 > 0) && (HPnum1 < (MAX_IN + 1)))
            {
                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                if (*pp == '.')
                {
                #if 0
                    for (j = 0; j < EDID_TABLE_LEN; j++)
                    {
                        acEDID_data[j] = 0;
                    }

                    dvNVRAMUserEdidRead(edsEDID_IN1+HPnum1 - 1, acEDID_data);
                    for (j = 0; j < EDID_TABLE_LEN; j++) UART1_SendByte(acEDID_data[j]);
                #endif
					ProGetInportEDIDData(HPnum1);
                    return (CH_CORRECT);
                }
                else
                    return (CM_ERROR);
            }
            else
                return (CM_ERROR);
        }
//==============================================================================
// EDIDM[X]B[Y].
// ����[X]·����ڽӵ���ʾ����EDID��������[Y]·�����?�����ȡ����?������ʼ����EDID��������
        pp = StrCmp(USART1_rxtailpoint, "EDIDM", USART1_rxprocesspoint);
        if (pp != NULL)             // EdidM 1B1
        {
            while (*pp != '.')
            {
                user_temp[count++] = *pp;

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;
            }
            pp = StrCmp(USART1_rxtailpoint, "EDIDM", USART1_rxprocesspoint);
            if ((*pp > '/') && (*pp < ':'))
            {
                while (*pp != 'B')
                {
                    if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                    {
                        HPnum1 = HPnum1 * 10 + *pp - 0x30; // ����˿�
                    }

                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;

                    jiucuo++;
                    if (jiucuo > 3)
                        return (CM_ERROR);

                }
                if ((HPnum1 > MAX_OUT) || (HPnum1 == 0))
                    return (CM_ERROR);
                jiucuo = 0;
                pp++;
                if (StrCmp(pp, "ALL", USART1_rxprocesspoint) == NULL)
                {
                    while (*pp != '.')
                    {
                        if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                        {
                            HPnum2 = HPnum2 * 10 + *pp - 0x30;
                        }

                        pp++;
                        if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                            pp = USART1_RX_BUFF;

                        jiucuo++;
                        if (jiucuo > 3)
                            return (CM_ERROR);

                    }

                }
                else
                {
                    HPnum2 = MAX_IN+1;  //over MAX_IN is All channel
                }

                if ((HPnum2 > (MAX_IN + 1)) || (HPnum2 == 0))
                    return (CM_ERROR);
                //    HPnum1:  �����
                //	 HPnum2: �����
#if 0
                if (HPnum2 == MAX_IN+1)
                {
                    for (i = 0; i < MAX_IN; i++)
                    {
                        appSystemInputPortEdidSet(i, edsEDID_EXTERN1 + HPnum1 - 1);

                    }

                }
                else
                {
                    appSystemInputPortEdidSet(HPnum2 - 1, edsEDID_EXTERN1 + HPnum1 - 1);
                }


                //��������Ϣ

                if (HPnum2 < (MAX_IN + 1))
                {
                    cCount += SiiSprintf(&acMessage2[cCount], "EDIDM%dB%d", HPnum1, HPnum2);
                    appUIUartMessageSet(NULL, acMessage2, TRUE);
                    appUIStateChange(uiUART_MESSAGE);

                }
                else
                {
                    cCount += SiiSprintf(&acMessage2[cCount], "EDIDM%dBALL", HPnum1);
                    appUIUartMessageSet(NULL, acMessage2, TRUE);
                    appUIStateChange(uiUART_MESSAGE);

                }
#endif
				ProCopyEDIDDataOutport2Inport(HPnum1, HPnum2);
                return (CH_CORRECT);
            }
            else
                return (CM_ERROR);
        }
//==================================================================================
//EDIDH[x]B[y].
//����[X]·����ڽӵ���ʾ����EDID��������[Y]·�����,�����ȡ��
//EDID������ȷ����������ֻ��֧��PCM��ʱ�򣬾�ǿ���޸�Ϊֻ֧��PCM��
//�����ȡ���󣬽�����ʼ����EDID��������;
//
        pp = StrCmp(USART1_rxtailpoint, "EDIDH", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            //BYTE cInstance = 0;
            //BYTE acEdid[256];
            while (*pp != '.')
            {
                user_temp[count++] = *pp;

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;
            }
            pp = StrCmp(USART1_rxtailpoint, "EDIDH", USART1_rxprocesspoint);
            if ((*pp > '/') && (*pp < ':'))
            {
                while (*pp != 'B')
                {
                    if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                    {
                        HPnum1 = HPnum1 * 10 + *pp - 0x30;
                    }

                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;

                    jiucuo++;
                    if (jiucuo > 3)
                        return (CM_ERROR);
                }
                if ((HPnum1 > MAX_OUT) || (HPnum1 == 0))
                    return (CM_ERROR);
                jiucuo = 0;
                pp++;
                while (*pp != '.')
                {
                    if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                    {
                        HPnum2 = HPnum2 * 10 + *pp - 0x30;
                    }

                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;

                    jiucuo++;
                    if (jiucuo > 3)
                        return (CM_ERROR);
                }
                if ((HPnum2 > MAX_IN) || (HPnum2 == 0))
                    return (CM_ERROR);
#if 0
                crc = 0;
                // if (check_HPD(HPnum1)) //�����˽����豸
                cInstance = utilOutputPortToInstance(HPnum1 - 1);
                if (SiiTxStatusGet(cInstance).isHotPlugDetected) //[kyq add: 20150728 15:38:23]
                {
                    //����Ӧ�����ͨ����EDID���ݶ�ȡ����
                    //EDID_get_slect_en(HPnum1, 0);
                    //get_EDID(HPnum1, g_tempData);
                    SiiTxEdidGetEdidData(cInstance, acEDID_data); //[kyq ADD: 20150728 15:31:00]
                                                                  //EDID_get_slect_en(HPnum1, 1);

                    if (acEDID_data[126] == 1) //HDMI
                    {
                        for (j = 0; j < EDID_TABLE_LEN; j++)  crc += acEDID_data[j];
                    }
                    else
                    {
                        for (j = 0; j < 128; j++)  crc += acEDID_data[j];
                        for (j = 128; j < EDID_TABLE_LEN; j++)  acEDID_data[j] = 0xff;
                    }
                    if (crc == 0)
                    {
                        if ((acEDID_data[0] == 0) && (acEDID_data[1] == 0xff) && (acEDID_data[7] == 0));
                        else
                        {
                            return (CM_ERROR);
                        }
                    }
                    else
                    {
                        return (CM_ERROR);
                    }
                }
                else
                    return (CM_ERROR);
                //����ǿ��PCM����
                if ((acEDID_data[0] == 0x00) && (acEDID_data[1] == 0xff) && (acEDID_data[7] == 0x00))
                {
                    if (acEDID_data[126] == 0x01) //HDMI��EDID����
                    {
                        IndexC = EDID_GetDataBlockAddr(acEDID_data, 0x40, 132);
                        EndT = (acEDID_data[IndexC] & 0x1F);
                        IndexC = EDID_GetDataBlockAddr(acEDID_data, 0x20, IndexC + EndT + 1);
                        if (IndexC > 132) //�ҵ���Ƶ����
                        {
                            EndT = (acEDID_data[IndexC] & 0x1F);
                            if ((acEDID_data[IndexC] == 0x23) && ((acEDID_data[IndexC + 1] & 0x78) == 0x08)); //ֻ֧��PCM��ʱ��
                            else
                            {
                                if (EndT > 3)
                                    acEDID_data[130] -= (EndT - 3); //��Ƶ������ƫ�Ƶ�ַ��Ҫ�䶯
                                acEDID_data[IndexC] =  0x23;
                                acEDID_data[IndexC + 1] =  0x09;
                                acEDID_data[IndexC + 2] =  0x07;
                                acEDID_data[IndexC + 3] =  0x07;

                                m = IndexC + EndT + 1; //��ʼ�±�
                                x = 0;
                                for (n = m; n < 255; n++)
                                {
                                    k = acEDID_data[n];
                                    acEDID_data[IndexC + 4 + x] = k;
                                    x++;
                                }

                                for (n = (IndexC + 4 + x); n < 255; n++)
                                {
                                    acEDID_data[n] = 0;
                                }

                                for (conut = 128; conut < 255; conut++)  crc += acEDID_data[conut];
                                acEDID_data[255] = ~(crc - 1);
                            }
                        }
                        else //û���ҵ���Ƶ��������
                        {
                            for (conut = 128; conut < 256; conut++)  acEDID_data[conut] = g_Init_EdidFlashEdidTable[conut];  //��Ĭ�ϵ�EDID���ݵĺ�128�ֽڿ�����ȥ
                        }
                    }
                    else //DVI��EDIDƵ����
                    {
                        for (conut = 128; conut < 256; conut++)    acEDID_data[conut] = 0xff;
                    }

                    // ��ԭ���Ѿ����õ�EDID ���ݱȽ�
                    /*
                    Read_EDID(INPUT_EDID_add + 256 * (HPnum2 - 1), g_edidFlashEdidTable);  //��ȡĿǰ��Ӧ������ͨ����EDID����
                    for (j = 0; j < EDID_TABLE_LEN - 1; j++) //�Ƚ�Ŀǰ��EDID��ָ����ʾ����EDID����
                    {
                        if (g_tempData[j] == g_edidFlashEdidTable[j])
                        {ediden = 0;}
                        else
                        {
                            ediden = 1;
                            break;
                        }
                    }
                    */

                    if (TRUE) // if (ediden)
                    {

                        //dvSiiDrvAdaptRxHpdSet(HPnum2 - 1, SII_DRV_ADAPT_INP_CTRL__DISABLE);
                        //dvSiiDrvAdaptRxEdidSramWrite(HPnum2 - 1, 0, g_edidFlashEdidTable, EDID_TABLE_LEN);
                        //IWDG_ReloadCounter(); //ι��
                        //dvSiiDrvAdaptRxHpdSet(HPnum2 - 1, SII_DRV_ADAPT_INP_CTRL__ENABLE);
                        appSystemInputPortEdidDataSet(HPnum2 - 1, edsEDID_IN1 + HPnum1 - 1, acEDID_data);
                        //write_EDID(HPnum2, g_tempData);

                    }
                }
                //��������Ϣ
                //sprintf(USART1_tx_buff, "EDIDH%02dB%02d      ", HPnum1, HPnum2);
                //strcpy(key_D2, USART1_tx_buff);
                //LCD_LineRefresh(key_D2, LINE2);
                //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                //UART1_SendBytes("\r\n", 2);
                cCount += SiiSprintf(&acMessage2[cCount], "EDIDH%dB%d", HPnum1, HPnum2);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUIStateChange(uiUART_MESSAGE);


                return (CH_CORRECT);
#endif
				return (cReturnValue = ProCopyEDIDDataOutport2Inport_AudioPCM(HPnum1, HPnum2));
            }
            else
                return (CM_ERROR);
        }
//=====================================================================================
//gxx131023
//EDIDPCM[x].
//������˿�[x]��EDID��Ϣ��Ƶ���ָ�ΪPCM��ʽ
        pp = StrCmp(USART1_rxtailpoint, "EDIDPCM", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            //BYTE acEdid[256];
            while (*pp != '.')
            {
                user_temp[count++] = *pp;

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;
            }
            pp = StrCmp(USART1_rxtailpoint, "EDIDPCM", USART1_rxprocesspoint);
            if ((*pp > '/') && (*pp < ':'))
            {
                jiucuo = 0;
                while (*pp != '.')
                {
                    if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                    {
                        HPnum2 = HPnum2 * 10 + *pp - 0x30;
                    }

                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;

                    jiucuo++;
                    if (jiucuo > 3)
                        return (CM_ERROR);
                }
                if ((HPnum2 > MAX_IN) || (HPnum2 == 0))
                    return (CM_ERROR);
#if 0
                //������ǰ��EDID��Ϣ
                dvNVRAMUserEdidRead(edsEDID_IN1 + HPnum2 - 1, acEdid); // Read_EDID(INPUT_EDID_add + 256 * (HPnum2 - 1), g_tempData);
                                                                       //����PCM����
                if ((acEdid[0] == 0x00) && (acEdid[1] == 0xff) && (acEdid[7] == 0x00))
                {
                    if (acEdid[126] == 0x01) //HDMI��EDID����
                    {
                        IndexC = EDID_GetDataBlockAddr(acEdid, 0x40, 132);
                        EndT = (acEdid[IndexC] & 0x1F);
                        IndexC = EDID_GetDataBlockAddr(acEdid, 0x20, IndexC + EndT + 1);
                        if (IndexC > 132) //�ҵ���Ƶ����
                        {
                            EndT = (acEdid[IndexC] & 0x1F); //ԴEDID����Ƶ�������ֽ���
                            if ((acEdid[IndexC] == 0x23) && ((acEdid[IndexC + 1] & 0x78) == 0x08))
                            {
                                ediden = 0;
                            }
                            else
                            {
                                if (EndT > 3)
                                    acEdid[130] -= (EndT - 3); //��Ƶ������ƫ�Ƶ�ַ��Ҫ�䶯
                                acEdid[IndexC] =  0x23;
                                acEdid[IndexC + 1] =  0x09;
                                acEdid[IndexC + 2] =  0x07;
                                acEdid[IndexC + 3] =  0x07;

                                m = IndexC + EndT + 1; //��ʼ�±�
                                x = 0;
                                for (n = m; n < 255; n++)
                                {
                                    k = acEdid[n];
                                    acEdid[IndexC + 4 + x] = k;
                                    x++;
                                }

                                for (n = (IndexC + 4 + x); n < 255; n++)
                                {
                                    acEdid[n] = 0;
                                }

                                for (conut = 128; conut < 255; conut++)  crc += acEdid[conut];
                                acEdid[255] = ~(crc - 1);

                                ediden = 1;
                            }
                        }
                        else //û���ҵ���Ƶ��������
                        {
                            ediden = 0;
                        }
                    }
                    else //DVI��EDIDƵ����
                    {
                        for (conut = 128; conut < 256; conut++)    acEdid[conut] = 0xff;
                    }

                    if (ediden)
                    {

                        appSystemInputPortEdidDataSet(HPnum2 - 1, edsEDID_IN1 + HPnum2 - 1, acEdid);
                        //dvSiiDrvAdaptRxHpdSet(HPnum2 - 1, SII_DRV_ADAPT_INP_CTRL__DISABLE);
                        //dvSiiDrvAdaptRxEdidSramWrite(HPnum2 - 1, 0, g_edidFlashEdidTable, EDID_TABLE_LEN);
                        //IWDG_ReloadCounter(); //ι��
                        //dvSiiDrvAdaptRxHpdSet(HPnum2 - 1, SII_DRV_ADAPT_INP_CTRL__ENABLE);
#if 0
                        //device_slect_9187(HPnum2);
                        SiiRegWrite(Build_cInstance, 0x0010, 0x00); //���Ȳ������
                        delay_nms(10);
                        SiiDrvNvramSramWrite(Build_cInstance, SRAM_P2, g_tempData, 0, EDID_TABLE_LEN); //����EDID����
                        IWDG_ReloadCounter(); //ι��
                        SiiRegWrite(Build_cInstance, 0x0010, 0x55); //���Ȳ�����ߣ������Ļ��ٴ�������ô�ź�Դ�ͻ����¶�ȡEDID����
                                                                    //disconnect_9187()
#endif
                        //write_EDID(HPnum2, g_tempData);
                    }
                }
                //��������Ϣ

                //sprintf(USART1_tx_buff, "EDIDPCM%02d     ", HPnum2);
                //strcpy(key_D2, USART1_tx_buff);
                // LCD_LineRefresh(key_D2, LINE2);
                //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                //UART1_SendBytes("\r\n", 2);
                cCount += SiiSprintf(&acMessage2[cCount], "EDIDPCM%d", HPnum2);
                appUIUartMessageSet(NULL, acMessage2, TRUE);
                appUIStateChange(uiUART_MESSAGE);
#endif
				ProModifyEDIDDataInport_AudioPCM(HPnum2);
                return (CH_CORRECT);
            }
            else
                return (CM_ERROR);
        }
        ///////////////////////////////////////////////////////////////////////////
//======================================================================================
/*
EDIDUpgrade[x].	"��������EDID���ݣ�[x]��ʾ����˿�,Ϊ5��ʾ����������˿ڲ�����
�յ�ָ��󣬻�������ʾ�뷢��EDID�ļ�10S�ӳ�ʱʱ�䣬�ļ���ʽΪ.bin
��Ϊ�˱�֤�����������գ��ڷ���ָ��ǰҪ�Ͽ�����HDBaseT��"

*/
        pp = StrCmp(USART1_rxtailpoint, "EDIDUpgrade", USART1_rxprocesspoint);
        if (pp != NULL)             // EdidM 1B1
        {

            EDID_updata_port = *pp - 0x30;
            if ((EDID_updata_port > 0) && (EDID_updata_port <=(edsEDID_INTERN1-edsEDID_IN1)))
            {
                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;
                if (*pp == '.')
                {
                    USART_data_flag = 1; //��־����Ҫ����EDID
                    USART_EDIDdata_count = 0;
                    //sys_delay = 550;
                    EdidUpataStart = SiiPlatformTimerSysTicksGet();
                    UART1_SendBytes("Please send the EDID file\r\n", strlen("Please send the EDID file\r\n"));
                    return (CH_CORRECT);
                }
                else
                    return (CM_ERROR); //gxx 140310
            }
            else
                return (CM_ERROR); //gxx 140310
        }
        ///////////////////////////////////////////////////////////////////////////
//======================================================================================
//UpgradeIntEDID[x].	"��������EDID���ݣ�[x]��ʾ����˿�,�յ�ָ���
//��������ʾ�뷢��EDID�ļ�10S�ӳ�ʱʱ�䣬�ļ���ʽΪ.bin"
        pp = StrCmp(USART1_rxtailpoint, "UpgradeIntEDID", USART1_rxprocesspoint);
        if (pp != NULL)             // EdidM 1B1
        {
            EDID_updata_port = *pp - 0x30;
            if ((EDID_updata_port > 0) && (EDID_updata_port <= (edsEDID_EXTERN1-edsEDID_INTERN1)))
            {
                USART_data_flag = 2; //��־����Ҫ����EDID
                USART_EDIDdata_count = 0;
                //sys_delay = 550;
                EdidUpataStart = SiiPlatformTimerSysTicksGet();
                UART1_SendBytes("Please send the EDID file\r\n", strlen("Please send the EDID file\r\n"));
                return (CH_CORRECT);
            }
            else
                return (CM_ERROR); //gxx 140310
        }
        /////////////////////////////////////////////////////////////////////////////
//============================================================================
// EDID/[x]/[y].	����˿�xʹ�����õı��Ϊy��EDID
//
//
        pp = StrCmp(USART1_rxtailpoint, "EDID/", USART1_rxprocesspoint);
        if (pp != NULL)
        {
            while (*pp != '.')
            {
                user_temp[count++] = *pp;

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;
            }
            pp = StrCmp(USART1_rxtailpoint, "EDID/", USART1_rxprocesspoint);
            while (*pp != '/')
            {
                if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                {
                    HPnum1 = HPnum1 * 10 + *pp - 0x30;
                }

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 3)
                    return (CM_ERROR);
            }
            if ((HPnum1 > MAX_IN) || (HPnum1 == 0))
                return (CM_ERROR);
            jiucuo = 0;
            pp++;
            while (*pp != '.')
            {
                if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                {
                    HPnum2 = HPnum2 * 10 + *pp - 0x30;
                }

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 3)
                    return (CM_ERROR);
            }
            if ((HPnum2 > (edsEDID_EXTERN1-edsEDID_INTERN1)) || (HPnum2 == 0))
                return (CM_ERROR);

            if (TRUE)
            {

                appSystemInputPortEdidSet(HPnum1 - 1, (eEDIDSELECT)(edsEDID_INTERN1 + HPnum2 - 1));

            }

            cCount += SiiSprintf((char*)&acMessage2[cCount], "EDID/%d/%d", HPnum1, HPnum2);
            appUIUartMessageSet(NULL, acMessage2, TRUE);
            appUIStateChange(uiUART_MESSAGE);


            return (CH_CORRECT);
        }
        /////////////////////////////////////////////////////////////////////////////
//===================================================================================================
//%080
// HDCP�Զ�����(����ģʽ)
        pp = StrCmp(USART1_rxtailpoint, "%080", USART1_rxprocesspoint);
        {
            if (pp != NULL)
            {
                if ((*pp > '0') && (*pp < '2'))
                    HPnum1 = *pp;
                else
                    return (CM_ERROR);
                pp++;
                if (*pp != '.')
                    return (CM_ERROR);
#if 0
                //psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
                psOutputPortMgrData = utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
                if (HPnum1 == '0') //�ֶ�
                {
                    //HDCP_AUTO_FLAG = 0;
                    //AUTO_change_FLAG = 0;
                    for (i = 0; i < MAX_OUT; i++)
                    {
                        psOutputPortMgrData->abHDCP_AUTO[i] = false;
                    }

                    //UART1_SendBytes("%0800\r\n", 7);
                    cCount += SiiSprintf((char*)&acMessage2[cCount], "%s","%0800");
                    appUIUartMessageSet(NULL, acMessage2, TRUE);
                    appUIStateChange(uiUART_MESSAGE);
                    //while (!(USART1->SR & USART_FLAG_TC));
                    //strcpy(D2, "%0800");
                }
                else  if (HPnum1 == '1') //�Զ�
                {
                    //if (psOutputPortMgrData->bHDCP_AUTO == false)
                    {
                        //HDCP_AUTO_FLAG = 1;
                        //NewCommandInFlag = NewCommandInFlag | 0x01;
                        //AUTO_change_FLAG = 1;
                        for (i = 0; i < MAX_OUT; i++)
                        {
                            psOutputPortMgrData->abHDCP_AUTO[i] = true;
                        }

                        //UART1_SendBytes("%0801\r\n", 7);

                        cCount += SiiSprintf(&acMessage2[cCount], "%s","%0801");
                        appUIUartMessageSet(NULL, acMessage2, TRUE);
                        appUIStateChange(uiUART_MESSAGE);
                        //for (i = 0; i < MAX_OUT; i++) OutSwitch_Flag[i] = 1;
                        //OutSwitch_TimeDelay = 200;
                    }
                    //while (!(USART1->SR & USART_FLAG_TC));
                    //strcpy(D2, "%0801");
                }
                utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                                  TRUE);
#endif
				ProConfigHDCPMode(HPnum1);
                //LCD_LineRefresh(D2, LINE2);
                //i2c_EE_write_one_byte(auto_hdcp_manage_add, HDCP_AUTO_FLAG);
                return (CH_CORRECT);
            }
        }
        // IR_IN 1~4 ֻ��������ز�, û�м��ز����ܣ�ֻ�ܷ���Զ��
        // IR_ALL_IN ֻ�ܿ���Զ��,���Լ��ز������ܿ��Ʊ���
        // IR_CTRL  ֻ�ܿ��Ʊ������ز��Ͳ��ز����붼���Կ��Ʊ�����MCU ����Ҫ���ơ�
        // %0900.     �����ز�����ģʽ
        // %0901.     �����ز�ǿ��ģʽ
        pp = StrCmp(USART1_rxtailpoint, "%090", USART1_rxprocesspoint);
        {
            if (pp != NULL)
            {
                if ((*pp >= '0') && (*pp < '2'))
                    HPnum1 = *pp;
                else
                    return (CM_ERROR);
                pp++;
                if (*pp != '.')
                    return (CM_ERROR);
#if 0
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

                if (HPnum1 == '0')      //�����ز�����
                {
                    // IR_carrier_mode = 0;
                    psSystemSettingData->cIR_Carrier_Mode = 0;
                    //send_data_to_FPGA(0xDE);
                    //if (check_ack())
                    //    send_data_to_FPGA(0xDE);
                    dvCPLDIRCarrierSet(FALSE);
                    //UART1_SendBytes("Carrier native\r\n", 16);
                    //sprintf(USART1_tx_buff, "Carrier native\r\n");
                    // strcpy(D2, "Carrier native");
                    cCount += SiiSprintf(&acMessage2[cCount], "%s","Carrier native");
                    appUIUartMessageSet(NULL, acMessage2, TRUE);
                    appUIStateChange(uiUART_MESSAGE);

                }
                else  if (HPnum1 == '1')        //�����ز�ǿ��
                {
                    //IR_carrier_mode = 1;
                    psSystemSettingData->cIR_Carrier_Mode = 1;
                    dvCPLDIRCarrierSet(TRUE);
                    //send_data_to_FPGA(0xDF);
                    //if (check_ack())
                    //    send_data_to_FPGA(0xDF);
                    //sprintf(USART1_tx_buff, "Force carrier\r\n");
                    //UART1_SendBytes("Force carrier\r\n", 15);
                    //  strcpy(D2, "Force carrier");
                    cCount += SiiSprintf(&acMessage2[cCount],"%s", "Force carrier");
                    appUIUartMessageSet(NULL, acMessage2, TRUE);
                    appUIStateChange(uiUART_MESSAGE);

                }
                //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                                  TRUE);
#endif
				ProConfigIRCarrierMode(HPnum1);
                // LCD_LineRefresh(D2, LINE2);
                // i2c_EE_write_one_byte(IR_carrier_mode_add, IR_carrier_mode);
                return (CH_CORRECT);
            }
        }
//====================================================================================
//����HDMI����������;[x]1,2,3,4��ʾ������һ�������������
//Ϊ5��ʾ���������������������
        pp = StrCmp(USART1_rxtailpoint, "DigitAudioON", USART1_rxprocesspoint);
        {
            //BYTE cInstance = 0;
            if (pp != NULL)
            {
                HPnum1 = *pp - 0x30;
                pp++;
                if (*pp != '.')
                    return (CM_ERROR);
#if 0
                if ((HPnum1 < (MAX_OUT + 1)) && (HPnum1 > 0))        //��������
                {
                    cSii9533Instance = utilOutputPortToInstance(HPnum1 - 1);
                    if ((HPnum1 == 1) || (HPnum1 == 2))
                    {
                        //device_slect_9187(HPnum1+5);

                        //SiiDrvRxAudioMixPassThroughConfig(cSii9533Instance, true, true);
                        dvSii9533TXAudioMixPassThroughConfig(cSii9533Instance, true, true);
                        //device_slect_9187(HPnum1+6);
                        dvSii9533TXAudioMixPassThroughConfig(cSii9533Instance + 1, true, true);
                    }
                    else
                    {
                        //device_slect_9187(HPnum1+6);
                        dvSii9533TXAudioMixPassThroughConfig(cSii9533Instance, true, true);
                    }
                    //digit_audio[HPnum1 - 1] = 1;
                    sprintf(USART1_tx_buff, "DigitAudio ON with Output %d\r\n", HPnum1);
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                    //i2c_EE_write_one_byte(Digit_audio_addr + (HPnum1 - 1), digit_audio[HPnum1 - 1]);
                }
                else if (HPnum1 == (MAX_OUT + 1))
                {
                    for (j = 0; j < MAX_OUT; j++)
                    {
                        //digit_audio[j] = 1;
                        HPnum1 = j + 1;
                        if ((HPnum1 == 1) || (HPnum1 == 2))
                        {
                            //device_slect_9187(HPnum1+5);
                            dvSii9533TXAudioMixPassThroughConfig(utilOutputPortToInstance(HPnum1 - 1), true, true);
                            //device_slect_9187(HPnum1+6);
                            dvSii9533TXAudioMixPassThroughConfig(utilOutputPortToInstance(HPnum1 - 1) + 1, true, true);
                        }
                        else
                        {
                            //device_slect_9187(HPnum1+6);
                            dvSii9533TXAudioMixPassThroughConfig(utilOutputPortToInstance(HPnum1 - 1), true, true);
                        }
                        //i2c_EE_write_one_byte(Digit_audio_addr + j, digit_audio[j]);
                    }
                    sprintf(USART1_tx_buff, "DigitAudio ON with ALL Outputs\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                }
                else
                    return (CM_ERROR);
                return (CH_CORRECT);
#endif
				return (cReturnValue = ProDigitAudioON(HPnum1));
            }
        }
        pp = StrCmp(USART1_rxtailpoint, "DigitAudioOFF", USART1_rxprocesspoint);
        {
            if (pp != NULL)
            {
                HPnum1 = *pp - 0x30;
                pp++;
                if (*pp != '.')
                    return (CM_ERROR);
#if 0
                if ((HPnum1 < (MAX_OUT + 1)) && (HPnum1 > 0))      //�ر�����
                {
                    if ((HPnum1 == 1) || (HPnum1 == 2))
                    {
                        //device_slect_9187(HPnum1+5);
                        dvSii9533TXAudioMixPassThroughConfig(utilOutputPortToInstance(HPnum1 - 1), false, true);
                        //device_slect_9187(HPnum1+6);
                        dvSii9533TXAudioMixPassThroughConfig(utilOutputPortToInstance(HPnum1 - 1) + 1, false, true);
                    }
                    /*
                    else if (HPnum1 == 2)
                    {
                        //device_slect_9187(HPnum1+3);
                        SiiDrvRxAudioMixPassThroughConfig(Build_cInstance, false, true);
                        //device_slect_9187(HPnum1+6);
                        SiiDrvRxAudioMixPassThroughConfig(Build_cInstance, false, true);
                    }
                    else
                    */
                    {
                        //device_slect_9187(HPnum1+6);
                        dvSii9533TXAudioMixPassThroughConfig(utilOutputPortToInstance(HPnum1 - 1), false, true);
                    }
                    //digit_audio[HPnum1 - 1] = 0;

                    sprintf(USART1_tx_buff, "DigitAudio OFF with Output %d\r\n", HPnum1);
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                    //i2c_EE_write_one_byte(Digit_audio_addr + (HPnum1 - 1), digit_audio[HPnum1 - 1]);
                }
                else if (HPnum1 == (MAX_OUT + 1))
                {
                    for (j = 0; j < MAX_OUT; j++)
                    {
                        //digit_audio[j] = 0;
                        HPnum1 = 1 + j;
                        if ((HPnum1 == 1) || (HPnum1 == 2))
                        {
                            //device_slect_9187(HPnum1+5);
                            dvSii9533TXAudioMixPassThroughConfig(utilOutputPortToInstance(HPnum1 - 1), false, true);
                            //device_slect_9187(HPnum1+6);
                            dvSii9533TXAudioMixPassThroughConfig(utilOutputPortToInstance(HPnum1 - 1) + 1, false, true);
                        }
                        /*
                        else if (HPnum1 == 2)
                        {
                            //device_slect_9187(HPnum1+3);
                            SiiDrvRxAudioMixPassThroughConfig(Build_cInstance, false, true);
                            //device_slect_9187(HPnum1+6);
                            SiiDrvRxAudioMixPassThroughConfig(Build_cInstance, false, true);
                        }
                        */
                        else
                        {
                            //device_slect_9187(HPnum1+6);
                            dvSii9533TXAudioMixPassThroughConfig(utilOutputPortToInstance(HPnum1 - 1), false, true);
                        }
                        // i2c_EE_write_one_byte(Digit_audio_addr + j, digit_audio[j]);
                    }
                    sprintf(USART1_tx_buff, "DigitAudio OFF with ALL Outputs\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                }
                else
                    return (CM_ERROR);
                return (CH_CORRECT);
#endif
				return (cReturnValue = ProDigitAudioOFF(HPnum1));
            }
        }
///////////////////////��ѯָ��///////////////////////////////////////////////
//============================================================================================
        pp = StrCmp(USART1_rxtailpoint, "%997", USART1_rxprocesspoint);
        {
            if (pp != NULL)
            {
                while (*pp != '.')
                {
                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;
                    jiucuo++;
                    if (jiucuo > 1)
                        return (CM_ERROR);
                }
                pp = StrCmp(USART1_rxtailpoint, "%997", USART1_rxprocesspoint);
                #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                if ((*pp > '0') && (*pp <= '9'))
                #else
                if ((*pp > '0') && (*pp < '9'))
                #endif
                {
#if 0
                    switch (*pp)
                    {
                    case '1' :          // %9971
                        appUIStateChange(uiUART_INCONNECT_INFO);
                        break;
                    case '2' :
                        appUIStateChange(uiUART_OUTCONNECT_INFO); // uiSYSTEM_OUTCONNECT_INFO
                        break;
                    case '3' :
                        appUIStateChange(uiUART_IN_HDCP_INFO);
                        break;
                    case '4' :
                        appUIStateChange(uiUART_OUT_HDCP_INFO);
                        break;
                    case '5' :
                        appUIStateChange(uiUART_SWITCH_INFO);
                        break;
                    case '6' :
                        appUIStateChange(uiUART_OUT_RESOLUTION_INFO);
                        break;
                    case '7' : //��ѯ���ͨ������������״̬
                        appUIStateChange(uiUART_OUT_HDMIAUDIO_INFO);
                        break;
                    case '8' : //��ѯ����˵�HDCP����״̬
                        appUIStateChange(uiUART_IN_HDCPENABLE_INFO);
                        break;
                    default :
                        break;
                    }
#endif
					ProQueryAudioVedioStatus(*pp);
                }
                else
                    return (CM_ERROR);
                return (CH_CORRECT);
            }
        }
        //////////////////////////////////////////////////////////////////////////////
//==========================================================================
        pp = StrCmp(USART1_rxtailpoint, "%996", USART1_rxprocesspoint);
        {
            if (pp != NULL)
            {
                if ((*pp > '0') && (*pp < '5'))
                    HPnum1 = *pp;
                else
                    return (CM_ERROR);
                pp++;
                if (*pp != '.')
                    return (CM_ERROR);
/*               while (*pp != '.')
                {
                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;
                    jiucuo++;
                    if (jiucuo > 1)
                        return (CM_ERROR);
                }*/
#if 0
                switch (HPnum1)
                {
                case '1' :
                    appUIStateChange(uiSYSTEM_LOCK_INFO);
                    break;
                case '2' :
                    sprintf(USART1_tx_buff, "PWON \r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

                    break;
                case '3' :
                    // NO LCD Disaplay
                    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
                    if (psSystemSettingData->cIR_Carrier_Mode == 0)
                    {
                        sprintf(USART1_tx_buff, "Carrier native\r\n");
                        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                    }
                    else if (psSystemSettingData->cIR_Carrier_Mode == 1)
                    {
                        sprintf(USART1_tx_buff, "Force carrier\r\n");
                        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                    }
                    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                                      FALSE);
                    break;

                case '4' :
                    appUIStateChange(uiUART_IP_INFO);
                    break;
                default :
                    break;

                }
#endif
				ProQuerySystermStatus(HPnum1);
                return (CH_CORRECT);
            }
        }
        //////////////////////////////////////////////////////////////////////////////
#ifdef PRD_POWER_ONOFF_MESSAGE
        pp = StrCmp(USART1_rxtailpoint, "%994", USART1_rxprocesspoint);
        {
            if (pp != NULL)
            {
                if ((*pp > '0') && (*pp <=( '0'+MAX_OUT)))
                    HPnum1 = *pp - 0x30;
		  else
		  	  return (CM_ERROR);

                pp++;
                if (*pp != '.')
                    return (CM_ERROR);
#if 0
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);

                    if (psSystemSettingData->acPowerOffMsgLength[HPnum1 -1] == 0)
                        sprintf(USART1_tx_buff, "Port %d: NO Data\r\n", HPnum1);
                    else
                        sprintf(USART1_tx_buff, "Port %d: %s when PWOFF\r\n", HPnum1 , psSystemSettingData->acPowerOffMsg[HPnum1 - 1]);

                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                                  FALSE);
                UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                return (CH_CORRECT);
#endif
               return (cReturnValue = ProQueryPortPWOFFSendData(HPnum1));
            }
        }

        pp = StrCmp(USART1_rxtailpoint, "%995", USART1_rxprocesspoint);
        {
            if (pp != NULL)
            {
                if((*pp > '0') && (*pp <=( '0'+MAX_OUT)))
                    HPnum1 = *pp - 0x30;
                else
                    return (CM_ERROR);

                pp++;
                if (*pp != '.')
                    return (CM_ERROR);
                /*              while (*pp != '.')
                               {
                                   pp++;
                                   if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                                       pp = USART1_RX_BUFF;
                                   jiucuo++;
                                   if (jiucuo > 1)
                                       return (CM_ERROR);
                               }*/
#if 0
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
                if ((HPnum1 > 0) && (HPnum1 < 5))
                {
                    // if (PWON_datalen[HPnum1-1] == 0)
                    if (psSystemSettingData->acPowerOnMsgLength[HPnum1 - 1] == 0)
                        sprintf(USART1_tx_buff, "Port %d: NO Data\r\n", HPnum1);
                    else
                        sprintf(USART1_tx_buff, "Port %d: %s when PWON\r\n", HPnum1, psSystemSettingData->acPowerOnMsg[HPnum1 - 1]);
                }
                else if ((HPnum1 > 4) && (HPnum1 < 9))
                {
                    if (psSystemSettingData->acPowerOffMsgLength[HPnum1 - 5] == 0)
                        sprintf(USART1_tx_buff, "Port %d: NO Data\r\n", HPnum1);
                    else
                        sprintf(USART1_tx_buff, "Port %d: %s when PWOFF\r\n", HPnum1 - 4, psSystemSettingData->acPowerOffMsg[HPnum1 - 5]);
                }
                else
                {
                    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                                      FALSE);
                    return (CM_ERROR);

                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                                  FALSE);
                UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                return (CH_CORRECT);
#endif
				return (cReturnValue = ProQueryPortPWONSendData(HPnum1));
            }
        }
#endif
        //////////////////////////////////////////////////////////////////////////////
        pp = StrCmp(USART1_rxtailpoint, "%0911", USART1_rxprocesspoint);
        {
            if (pp != NULL)
            {
                while (*pp != '.')
                {
                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;
                    jiucuo++;
                    if (jiucuo >= 1)
                        return (CM_ERROR);
                }
                //_ Factory_initialization();
                sprintf(USART1_tx_buff, "Factory Default\r\n");
                UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                //sprintf(USART1_tx_buff, "Plesse Reboot System\r\n");
                //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
#if 0
                utilDataMgrReset(nveRESETALL);
		   		appUIIRKeyEnableSet(TRUE);
                for (i = 0; i < MAX_IN; i++)
                {
                    appSystemInputPortEdidSet(i, edsEDID_IN1 + i);
                }
                for (j = 0; j < MAX_OUT; j++)
                {
                    //digit_audio[j] = 1;
                    dvCpldRemoteBraundSet(j + 1);

                    if ((j == 0) || (j == 1))
                    {
                        //device_slect_9187(HPnum1+5);
                        dvSii9533TXAudioMixPassThroughConfigStart(utilOutputPortToInstance(j));
                        //device_slect_9187(HPnum1+6);
                        dvSii9533TXAudioMixPassThroughConfigStart(utilOutputPortToInstance(j) + 1);
                    }
                    else
                    {
                        //device_slect_9187(HPnum1+6);
                        dvSii9533TXAudioMixPassThroughConfigStart(utilOutputPortToInstance(j));
                    }
                    //i2c_EE_write_one_byte(Digit_audio_addr + j, digit_audio[j]);
                }
                dvCPLDIRCarrierSet(FALSE); //[kyq ADD: 20151010 11:42:15]
                appSystemSwitcherSetStart();
#ifdef PRD_IR_OUT
                appSystemIRSwitcherSetStart();
#endif
#endif
				ProRecallFactoryDefault();
                // any other need to process?
                return (CH_CORRECT);
            }
        }
//===========================================================
        /////////////////////DEBUG///////////////////////////////

        if (StrCmp(USART1_rxtailpoint, "SIIMON", USART1_rxprocesspoint) != NULL)
        {
            BYTE cNumber = 0;
            temppoint = USART1_rxtailpoint + 6; //

            while (*(temppoint + i) != '.')
            {

                cNumber = *(temppoint + i) - '0';
                i++;
                if (temppoint + i >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    break;

            } // end while

            SkSiMonUartNumberSet(cNumber);
            return CH_CORRECT;
        }

        if (StrCmp(USART1_rxtailpoint, "DEBUGEN.", USART1_rxprocesspoint) != NULL)
        {
            SiiPlatformLoggerPrintEnable(TRUE);
            return CH_CORRECT;
        }
        if (StrCmp(USART1_rxtailpoint, "DEBUGCOLORON.", USART1_rxprocesspoint) != NULL)
        {
            SiiPlatformLoggerColorEnable(TRUE);
            return CH_CORRECT;
        }
        if (StrCmp(USART1_rxtailpoint, "DEBUGCOLOROFF.", USART1_rxprocesspoint) != NULL)
        {
            SiiPlatformLoggerColorEnable(FALSE);
            return CH_CORRECT;
        }
        if (StrCmp(USART1_rxtailpoint, "GITVERSION.", USART1_rxprocesspoint) != NULL)
        {

            UART1_SendData2PC(m_acGitHash,strlen(m_acGitHash));
            return CH_CORRECT;
        }

        //DEBUGLEVEL255.
        if (StrCmp(USART1_rxtailpoint, "DEBUGLEVEL", USART1_rxprocesspoint) != NULL)
        {
            BYTE cNumber = 0;
            temppoint = USART1_rxtailpoint + 10; //strlen("DEBUGLEVEL"); //     SiiCecSendUserControlCmd();
            i = 0;
            while (*(temppoint + i) != '.')
            {

                cNumber = cNumber * 10 + (*(temppoint + i) - '0');
                i++;
                if (temppoint + i >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    break;

            } // end while
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
            psSystemSettingData->cDebugLevel = 	cNumber;
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
            SiiPlatformLoggerMessageLevelSet(cNumber);
            return CH_CORRECT;
        }
    	        //DEBUGLEVEL255.
        if (StrCmp(USART1_rxtailpoint, "DEBUGPOST", USART1_rxprocesspoint) != NULL)
        {
            BYTE cNumber = 0;
            temppoint = USART1_rxtailpoint + 10; //strlen("DEBUGLEVEL"); //     SiiCecSendUserControlCmd();
            i = 0;
            while (*(temppoint + i) != '.')
            {

                cNumber = cNumber * 10 + (*(temppoint + i) - '0');
                i++;
                if (temppoint + i >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    break;

            } // end while
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
            psSystemSettingData->cPOSTEnable = 	cNumber;
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
            return CH_CORRECT;
        }
        /////////////////////��������ָ��///////////////////////////////
        //////////////////////////////////////////////////////////////////////////////
#if 0
        pp = StrCmp(USART1_rxtailpoint, "%9980", USART1_rxprocesspoint);
        {
            if (pp != NULL)
            {
                while (*pp != '.')
                {
                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;
                    jiucuo++;
                    if (jiucuo >= 1)
                    return (CM_ERROR);
                }
                sys_check();
                return (CH_CORRECT);
            }
        }
        pp = StrCmp(USART1_rxtailpoint, "%999", USART1_rxprocesspoint);
        {
            if ((*pp > '0') && (*pp < '5'))
            {
                UART1_SendByte(dvADN4604Read(0, ch_out[*pp - 0x31]));
                UART1_SendByte(dvADN4604Read(0, ch_out[*pp - 0x31] + 1));

                UART1_SendByte(dvADN4604Read(0, ch_out_en[*pp - 0x31]));
                UART1_SendByte(dvADN4604Read(0, ch_out_en[*pp - 0x31] + 1));
                UART1_SendByte(dvADN4604Read(0, ch_out_en[*pp - 0x31] + 2));
                UART1_SendByte(dvADN4604Read(0, ch_out_en[*pp - 0x31] + 3));
                return (CH_CORRECT);
            }
            else if (*pp == '9')
            {
                i2c_EE_write_one_byte(first_use_add, 0x51);
                UART1_SendBytes("EEPROM_init\r\n", strlen("EEPROM_init\r\n"));
                return (CH_CORRECT);
            }
        }
#endif
        /*
        pp = StrCmp(USART1_rxtailpoint, "%988",USART1_rxprocesspoint) ;
        {
            if((*pp > '0')&&(*pp < '7'))
            {
                 UART1_SendByte(bUpHDMIMode[*pp-0x31]);

                 UART1_SendByte(VState[*pp-0x31]);
                 UART1_SendByte(TxDev[*pp-0x31].TxVState);

                 UART1_SendByte(TxDev[*pp-0x31].bTxInputVideoMode);
                 UART1_SendByte(TxDev[*pp-0x31].bTxOutputVideoMode);

               UART1_SendByte(MuteByPKG[*pp-0x31]);

               UART1_SendByte(TxDev[*pp-0x31].AudSrcHBR);
               UART1_SendByte(TxDev[*pp-0x31].bTxAudSel);
               UART1_SendByte(TxDev[*pp-0x31].AucSrcLPCM);
                 return(CH_CORRECT);
            }
        }
        */
        //gxx 140310

        // wf8421 20160823
        #ifdef ENABLE_DB9_BAUDRATE_SWITCH
        if(StrCmp(USART1_rxtailpoint, "Baudrate 2400.",USART1_rxprocesspoint) != NULL)
        {
            unsigned char tbaudrate;

            Function_SetPcUartBaudrate(1); // set baudrate 2400
            return(CH_CORRECT);
        }
        if(StrCmp(USART1_rxtailpoint, "Baudrate 4800.",USART1_rxprocesspoint) != NULL)
        {
            unsigned char tbaudrate;

            Function_SetPcUartBaudrate(2); // set baudrate 4800
            return(CH_CORRECT);
        }
        if(StrCmp(USART1_rxtailpoint, "Baudrate 9600.",USART1_rxprocesspoint) != NULL)
        {
            unsigned char tbaudrate;

            Function_SetPcUartBaudrate(3); // set baudrate 9600
            return(CH_CORRECT);
        }
        if(StrCmp(USART1_rxtailpoint, "Baudrate 19200.",USART1_rxprocesspoint) != NULL)
        {
            unsigned char tbaudrate;

            Function_SetPcUartBaudrate(4); // set baudrate 19200
            return(CH_CORRECT);
        }
        if(StrCmp(USART1_rxtailpoint, "Baudrate 38400.",USART1_rxprocesspoint) != NULL)
        {
            unsigned char tbaudrate;

            Function_SetPcUartBaudrate(5); // set baudrate 38400
            return(CH_CORRECT);
        }
        if(StrCmp(USART1_rxtailpoint, "Baudrate 57600.",USART1_rxprocesspoint) != NULL)
        {
            unsigned char tbaudrate;

            Function_SetPcUartBaudrate(6); // set baudrate 57600
            return(CH_CORRECT);
        }
        if(StrCmp(USART1_rxtailpoint, "Baudrate 115200.",USART1_rxprocesspoint) != NULL)
        {
            unsigned char tbaudrate;

            Function_SetPcUartBaudrate(7); // set baudrate 115200
            return(CH_CORRECT);
        }

        if(StrCmp(USART1_rxtailpoint, "Get Baudrate.",USART1_rxprocesspoint) != NULL)
        {
            unsigned char tbaudrate;

            Function_QueryBaudrate(); // set baudrate 115200
            return(CH_CORRECT);
        }
        #endif

        return (CM_ERROR);
    }
    if (RXdata == ';')
    {
        /*
            if (powerflag == 1)
                return (OTHERS);
            if (sDEMOFlag == 1)
                sDEMOFlag = 0;
            if (standby == 1)
            {
                standby = 0; //ȡ������ģʽ
                i2c_EE_write_one_byte(Standby_addr, standby);
            }
        */
//=========================================================================
///     %Lock;	���̵�����

        if (StrCmp(USART1_rxtailpoint, "/%Lock;", USART1_rxprocesspoint) != NULL) // System Lock
        {
#if 0
            System = OFF;
            sprintf(USART1_tx_buff, "System Locked!\r\n");
            UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
            strcpy(D2, "System Locked!");
            LCD_LineRefresh(D2, LINE2);
            i2c_EE_write_one_byte(sys_lock_flag_add, System);
            //gxx 131217
            EXTI->IMR &= ~EXTI_Line11;
#endif
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
            psSystemSettingData->bSystemLock = true;
            System_UnLock_flag = psSystemSettingData->bSystemLock;
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                              TRUE);
            appUIIRKeyEnableSet(FALSE);
            appUIStateChange(uiSYSTEM_LOCK_INFO);
            //sprintf(USART1_tx_buff, "System Locked!\r\n");
            //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
            //m_eNextState = uiSYSTEM_LOCK;
            //SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(3000L));
            //EXTI->IMR &= ~EXTI_Line11;
            return (OTHERS);
        }
        if (StrCmp(USART1_rxtailpoint, "/%Unlock;", USART1_rxprocesspoint) != NULL) // System Unlock
        {
#if 0
            System = ON;
            sprintf(USART1_tx_buff, "System Unlock!\r\n");
            UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
            strcpy(D2, "System Unlock!");
            LCD_LineRefresh(D2, LINE2);
            i2c_EE_write_one_byte(sys_lock_flag_add, System);
            //gxx 131217
            EXTI->IMR |= EXTI_Line11;
#endif
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
            psSystemSettingData->bSystemLock = FALSE;
            System_UnLock_flag = psSystemSettingData->bSystemLock;
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                              TRUE);
            appUIIRKeyEnableSet(TRUE);
            appUIStateChange(uiSYSTEM_LOCK_INFO);

            //sprintf(USART1_tx_buff, "System Unlock!\r\n");
            //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
            //m_eNextState = uiSYSTEM_Unlock;
            //SiiPlatformTimerSet(etUI_CLEAR, mS_TO_TICKS(3000L));
            return (OTHERS);
        }
//===========================================================================
// ��ѯ
        if (StrCmp(USART1_rxtailpoint, "/*Type;", USART1_rxprocesspoint) != NULL) // System Type
        {
            //psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);

            appUIStateChange(uiUART_SYSTEM_TYPE);
            return (OTHERS);
        }
        if (StrCmp(USART1_rxtailpoint, "/^Version;", USART1_rxprocesspoint) != NULL) // Software Version
        {
            appUIStateChange(uiSYSTEM_VERSION);
            return (OTHERS);
        }
	if (StrCmp(USART1_rxtailpoint, "/^TestVersion;", USART1_rxprocesspoint) != NULL) // Software Version
        {
            appUIStateChange(uiSYSTEM_VERSION);
            return (OTHERS);
        }
#if 0
        if (StrCmp(USART1_rxtailpoint, "/:MessageOff;", USART1_rxprocesspoint) != NULL)              // "/:MessageOff;"
        {
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
            psSystemSettingData->bUartReturnMessage_EN = OFF;
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                              TRUE);
            appUIStateChange(uiSYSTEM_MESSAGE_ONOFF_INFO);
            return (OTHERS);
        }
        if (StrCmp(USART1_rxtailpoint, "/:MessageOn;", USART1_rxprocesspoint) != NULL)              // "/:MessageOn;"
        {
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
            psSystemSettingData->bUartReturnMessage_EN = ON;
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                              TRUE);
            appUIStateChange(uiSYSTEM_MESSAGE_ONOFF_INFO);
            return (OTHERS);
        }
#endif
    }
    //gxx 140310
    return (CM_ERROR);
    //return(CH_CORRECT);
}

#else

#include "CmdFunction.h"
uint8_t do_with_type(unsigned char RXdata,
                     unsigned char *USART1_rxtailpoint,
                     unsigned char *USART1_rxprocesspoint)
{
    unsigned int  i = 0/*,  count = 0, jiucuo = 0*/;
    BYTE cInPort = 0/*, cOutPort = 0*/;
    BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
    BYTE cCount = 0;
    memset(acMessage2, 0, LCD_MAX_CHAR);
    m_bCommdDemo = FALSE; 		// kyq 20150929

    BYTE FuncFlag = OTHERS;

    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE == MHUB4K431))
    if(*USART1_rxtailpoint == 0xa5 && *(USART1_rxtailpoint+1)==0x5b)
    {
        //sprintf(USART1_tx_buff,"strlen(cmdlist)==%d strlen(cmdlist[0])==%d!\r\n",sizeof(cmdlist),sizeof(cmdlist[0]));
        //UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
        for(i = 0 ; i< (sizeof(cmdlist)/sizeof(cmdlist[0])); i++)
        {
            if(*(USART1_rxtailpoint+2)==0x02)
            {
                if(StrCmp(USART1_rxtailpoint, cmdlist[i], USART1_rxprocesspoint) != NULL)
                {
                    BYTE acOutPortList[MAX_OUT];
                    cInPort = *(USART1_rxtailpoint+4);
                    acOutPortList[0] = *(USART1_rxtailpoint+6)-1;

                #if (MACHINETYPE == MHUB4K431)
                    if((cInPort>MAX_IN)||(*(USART1_rxtailpoint+6)>MAX_OUT))
                        return (CM_ERROR);
                #endif
                    ProessVedioSwitch(cInPort, 1, acOutPortList);

                    return (CH_CORRECT);
                }
            }
        #if 0
            else if(*(USART1_rxtailpoint+2)==0x08)
            {
                if(StrCmp(USART1_rxtailpoint, cmdlist[i], USART1_rxprocesspoint) != NULL)
                {
                    if(*(USART1_rxtailpoint+4)==0xf0)
                    {
                        // beep off
                    }
                    else if(*(USART1_rxtailpoint+4)==0x0f)
                    {
                        // beep on
                    }
                    return (CH_CORRECT);
                }
            }
        #endif
            else if(*(USART1_rxtailpoint+2)==0x08)
            {
                if(StrCmp(USART1_rxtailpoint, cmdlist[i], USART1_rxprocesspoint) != NULL)
                {
                    if(*(USART1_rxtailpoint+4)==0xf0)
                    {
                        cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "PWOFF");
                        appUIUartMessageSet(NULL, acMessage2, FALSE);
                        appUIStateChange(uiUART_MESSAGE);
                        appPowerMgrPowerStateSet(pwrSOFTOFF);
                    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                        KeyLedSetAllClose();
                    #endif
                        return (CH_CORRECT);
                    }
                    else if(*(USART1_rxtailpoint+4)==0x0f)
                    {
                        // power on
                    }
                }
            }
        }
        i = 0;
    }
    #endif

    if (RXdata == '.')
    {
        for(i=0;i<(GetCmdListToMode1_Len());i++)
        {
            if(CmdListToMode1[i].Flags & Flag_HeadMatch)
            {
                if(CmdListToMode1[i].cmdlen==UseStrlen)
                {
                    if(strncmp(USART1_rxtailpoint,CmdListToMode1[i].ucmd,strlen(CmdListToMode1[i].ucmd))==NULL)
                    {
                        FuncFlag=CmdListToMode1[i].CmdFun(USART1_rxtailpoint, USART1_rxprocesspoint);
                    }
                }
                else// if(CmdListToMode1[i].cmdlen==UseStrlen)
                {
                    if(StrCmp(USART1_rxtailpoint,CmdListToMode1[i].ucmd,USART1_rxprocesspoint)!=NULL)
                    {
                        FuncFlag=CmdListToMode1[i].CmdFun(USART1_rxtailpoint, USART1_rxprocesspoint);
                    }
                }
            }
            else if(CmdListToMode1[i].Flags & Flag_WholeWord)
            {
                //if(strcmp(USART1_rxtailpoint,CmdListToMode1[i].ucmd)==NULL)
                if(StrCmp(USART1_rxtailpoint,CmdListToMode1[i].ucmd,USART1_rxprocesspoint)!=NULL)
                {
                    FuncFlag=CmdListToMode1[i].CmdFun(USART1_rxtailpoint, USART1_rxprocesspoint);
                }
            }
            else
            {
                if(StrCmp(USART1_rxtailpoint,CmdListToMode1[i].ucmd,USART1_rxprocesspoint)!=NULL)
                {
                    FuncFlag=CmdListToMode1[i].CmdFun(USART1_rxtailpoint, USART1_rxprocesspoint);
                }
            }
            if(FuncFlag != OTHERS)
                return FuncFlag;
        }
    }
    else if(RXdata == ';')
    {

        for(i=0;i<(GetCmdListToMode2_Len());i++)
        {
	     if(CmdListToMode2[i].Flags & Flag_HeadMatch)
            {
                if(CmdListToMode2[i].cmdlen==UseStrlen)
                {
                    if(strncmp(USART1_rxtailpoint,CmdListToMode2[i].ucmd,strlen(CmdListToMode2[i].ucmd))==NULL)
                    {
                        FuncFlag=CmdListToMode2[i].CmdFun(USART1_rxtailpoint, USART1_rxprocesspoint);
                    }
                }
                else// if(CmdListToMode1[i].cmdlen==UseStrlen)
                {
                    if(StrCmp(USART1_rxtailpoint,CmdListToMode2[i].ucmd,USART1_rxprocesspoint)!=NULL)
                    {
                        FuncFlag=CmdListToMode2[i].CmdFun(USART1_rxtailpoint, USART1_rxprocesspoint);
                    }
                }
            }
            else if(CmdListToMode2[i].Flags & Flag_WholeWord)
            {
		  if(StrCmp(USART1_rxtailpoint,CmdListToMode2[i].ucmd,USART1_rxprocesspoint)!=NULL)
                {
                    FuncFlag=CmdListToMode2[i].CmdFun(USART1_rxtailpoint, USART1_rxprocesspoint);
                }
            }
            else
            {
		  if(StrCmp(USART1_rxtailpoint,CmdListToMode2[i].ucmd,USART1_rxprocesspoint)!=NULL)
                {
                    FuncFlag=CmdListToMode2[i].CmdFun(USART1_rxtailpoint, USART1_rxprocesspoint);
                }
            }
            if(FuncFlag != OTHERS)
                return FuncFlag;
        }
    }
    return CM_ERROR;
}

#endif

//����ָ���ͷָ�� USART1_rxtailpoint
//����ָ���βָ�� USART1_rxprocesspoint
//�������ݵ�βָ�� USART1_rxpoint
void Uart1_process(void)
{
    unsigned char rxdata = 0, temp = 0/*,  crc = 0*/;

    unsigned char *USART1_rxtailpoint;         // buffer ͷָ��
    unsigned char *USART1_rxprocesspoint; // ��ǰָ��
    unsigned char *pp = NULL;
    //uint16_t sys_ID = 0x07ff;
    uint16_t	i = 0/*, j = 0*/;

    USART1_rxtailpoint = USART1_RX_BUFF;
    USART1_rxprocesspoint = USART1_RX_BUFF;
    if (SkTimeDiffMs(EdidUpataStart, SiiPlatformTimerSysTicksGet()) > mS_TO_TICKS(10000))
    {
	 if((USART_data_flag==1)||(USART_data_flag==2))
	 {
		UART1_SendBytes("EDID Upgrade time out!\r\n", strlen("EDID Upgrade time out!\r\n"));
	 }
	 USART_data_flag = 0;
    }

    if (g_bUSART1_DataComplete == 1)
    {
        //���ڳ���100ms û���յ����ݣ���Ϊ���������
        if (USART_data_flag == 0)
        {
            ////0  ����ָ��
            IWDG_ReloadCounter(); //ι��
                                  // ��ͷ��ʼһ��һ�����ݱȽ�
           // while (USART1_rxprocesspoint != USART1_rxpoint) //USART1_rxpoint��βָ��
            while (USART1_rxprocesspoint < USART1_rxpoint) 		//USART1_rxpoint��βָ��
            {
                //USART1_rxtailpoint
                //   |       rxdata
                //   |         |USART1_rxprocesspoint
                //   |         |              |
                //  []   []   []             []          []     [][][][][][][][][][][][][][][][]
                //
                //
                //
                //


                rxdata = *USART1_rxprocesspoint;
                USART1_rxprocesspoint++;

		 //[kyq Begin: 20151118 17:33:18]
#if 0//def  PRD_VHD4
                 if(USART1_rxprocesspoint>=USART1_rxpoint)
			 break;
#endif
	       //[kyq End: 20151118 17:33:24]

		   //UART1_SendByte(rxdata);
                i++; //ͳ���⴮ָ��ĳ���
                if (i > USART1_RX_BUFF_MAX)
                {
                    g_bUSART1_DataComplete = FALSE; //uart_data_in_end = 0;
                    USART1_rxpoint = USART1_RX_BUFF;
                    memset(USART1_RX_BUFF, 0, USART1_RX_BUFF_MAX); //�������buff
                    return;
                }

		  //UART1_SendByte(rxdata);

//Huang 2016-Jun-28, 17:35:10
                if (((rxdata == '.') || (rxdata == ';')) // ָ���߼����⻹�ǱȽ϶࣬����������
                #ifdef  PRD_VHD4_CMD_END
    			&&(*USART1_rxprocesspoint=='~')
                #endif
                #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE == MHUB4K431))
                ||((*USART1_rxtailpoint == 0xa5) && (*(USART1_rxtailpoint + 1) == 0x5b))
                #endif
			    )
                {
                  if((USART1_rxprocesspoint < USART1_rxpoint))
                  	{
                        if (rxdata == ';')
                        {
                            //�����/+ָ�˵�����������û����ݣ�������Ϊ������
                            if ((*USART1_rxtailpoint == '/') && (*(USART1_rxtailpoint + 1) == '+')) // /+ ��ͷ
                                continue;

                            /********************************************gxx 131023  *************************************/
                            // ������趨ϵͳָ�˵��;���ǽ�����
                            if ((*USART1_rxtailpoint == 0xfa) || (*USART1_rxtailpoint == 0xfb) || (*USART1_rxtailpoint == 0xfc) || (*USART1_rxtailpoint == 0xf1) || (*USART1_rxtailpoint == 0xf2) || (*USART1_rxtailpoint == 0xf3))
                                continue;
                            #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE == MHUB4K431))
                            if((*USART1_rxtailpoint == 0xa5) && (*(USART1_rxtailpoint + 1) == 0x5b))
                                continue;
                            #endif
                        }
                        else
                        {
                            //���.���滹��һ��.˵�����.�����û����ݣ�������Ϊ������
                            if ((*USART1_rxprocesspoint == '.')&&(USART1_rxprocesspoint < USART1_rxpoint))
                                continue;
                            /********************************************gxx 131023  *************************************/
                            // �����IPָ�˵��.���ǽ�����
                            if ((*USART1_rxtailpoint == 'I') && (*(USART1_rxtailpoint + 1) == 'P'))
                                continue;
                            // ������趨ϵͳָ�˵��.���ǽ�����
                            if ((*USART1_rxtailpoint == 0xfa) || (*USART1_rxtailpoint == 0xfb) || (*USART1_rxtailpoint == 0xfc) || (*USART1_rxtailpoint == 0xf1) || (*USART1_rxtailpoint == 0xf2) || (*USART1_rxtailpoint == 0xf3))
                                continue;
                            #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE == MHUB4K431))
                            if((*USART1_rxtailpoint == 0xa5) && (*(USART1_rxtailpoint + 1) == 0x5b))
                                continue;
                            #endif
                        }
                  	}
                    //����.  ; ������
                    temp = do_with_type(rxdata, USART1_rxtailpoint, USART1_rxprocesspoint); //����ͷָ��,βָ��
                    // add by kyq 20150929
                    if ((CH_CORRECT == temp)&& (!m_bCommdDemo)
                        &&appSystemSwitchDemoModeEnableGet())
                    {
                        appSystemSwitchDemoModeEnableSet(FALSE);
                    }
                    // kyq 20151012
                     if((temp==CH_ERROR)||(temp==CM_ERROR))//
                    {
                        UART1_SendBytes("CMD Error!\r\n", strlen("CMD Error!\r\n"));
			   appUIStateChange(uiUART_CMD_ERROR);
                    }
                     // end
                    USART1_rxtailpoint = USART1_rxprocesspoint; //��βָ���ͷָ��

                }
//==============================================================
                else if (rxdata == 0xfe)
                {
                    pp = USART1_rxtailpoint;
                    if ((*pp != 0xfa) && (*pp != 0xfb) && (*pp != 0xfc) && (*pp != 0xf1) && (*pp != 0xf2) && (*pp != 0xf3))
                    {
                        continue; // kyq 20151126
                    }
                        if (*pp == 0xfa)
                        {
#if 0
                            PSYSTEM_SETTING_DATA   psSystemSettingData;
                            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);


                            pp++;
                            for (i = 0; i < 16; i++)
                            {
                                if (*(pp + i) != 0xfe)
                                    psSystemSettingData->acProduct_Name[i] = *(pp + i);
                                else
                                    break;
                            }
							psSystemSettingData->acProduct_Name[16] = 0;// kyq 20151201
                            if (i == 16)
                            {
                                psSystemSettingData->acProduct_Name[i] = 0;

                                UART1_SendBytes("sys_name is ", strlen("sys_name is "));
                                sprintf(USART1_tx_buff, "%s \r\n", psSystemSettingData->acProduct_Name);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                            }
                            else
                                UART1_SendBytes("sys_name setting fail!", strlen("sys_name setting fail!"));
                            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
#endif
							ProModifyProductName(pp);
                        }
//=============================================================================
                        else if (*pp == 0xfb)
                        {
#if 0
                            PSYSTEM_SETTING_DATA   psSystemSettingData;
                            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

                            pp++;
                            for (i = 0; i < 16; i++)
                            {
                                if (*(pp + i) != 0xfe)
                                    psSystemSettingData->acProduct_Type[i] = *(pp + i);
                                else
                                    break;
                            }
							psSystemSettingData->acProduct_Type[16] = 0; // kyq 20151201
                            if (i == 16)
                            {
                                psSystemSettingData->acProduct_Type[i] = 0;
                                // for(j=0;j<i;j++)  i2c_EE_write_one_byte(sys_type_add+j,sys_type[j]);

                                UART1_SendBytes("sys_type is ", strlen("sys_type is "));
                                sprintf(USART1_tx_buff, "%s \r\n", psSystemSettingData->acProduct_Type);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                            }
                            else
                                UART1_SendBytes("sys_type setting fail!", strlen("sys_type setting fail!"));

                            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
#endif
							ProModifyProductType(pp);
                        }
//=========================================================================
                        else if (*pp == 0xfc)   // ��Ĭ��EDID �ĳ�������
                        {
#if 0
                            PSYSTEM_SETTING_DATA   psSystemSettingData;
                            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

                            pp++;
                            for (i = 0; i < 3; i++)
                            {
                                if (*(pp + i) != 0xfe)
                                    psSystemSettingData->acDefaultEdidProductName[i] = *(pp + i); //   [i] = *(pp+i) ;
                                else
                                    break;
                            }
                            if (i == 3)
                            {
                                psSystemSettingData->acDefaultEdidProductName[i] = 0; // EDID_name[i] = 0;

                                if (psSystemSettingData->acDefaultEdidProductName[2] >= 'A')
                                {
                                    sys_ID &= 0xffe0; //�����5λ
                                    sys_ID |= psSystemSettingData->acDefaultEdidProductName[2] - 0x40;
                                }
                                if (psSystemSettingData->acDefaultEdidProductName[1] >= 'A')
                                {
                                    sys_ID &= 0xfc1f; //�����5λ
                                    sys_ID |= ((psSystemSettingData->acDefaultEdidProductName[1] - 0x40) << 5);
                                }
                                if (psSystemSettingData->acDefaultEdidProductName[0] >= 'A')
                                {
                                    sys_ID &= 0x03ff; //�����5λ
                                    sys_ID |= ((psSystemSettingData->acDefaultEdidProductName[0] - 0x40) << 10);
                                }

                                g_Init_EdidFlashEdidTable[8] = sys_ID >> 8;
                                g_Init_EdidFlashEdidTable[9] = sys_ID;
                                for (i = 0; i < 127; i++)  crc += g_Init_EdidFlashEdidTable[i];
                                g_Init_EdidFlashEdidTable[127] = ~(crc - 1);

                                UART1_SendBytes("EDID_name is ", strlen("EDID_name is "));
                                sprintf(USART1_tx_buff, "%s \r\n", psSystemSettingData->acDefaultEdidProductName);
                                UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
                                //[kyq Begin: 20150820 20:03:17]

                                dvNVRAMUserEdidWrite(edsEDID_Default, g_Init_EdidFlashEdidTable);
                                //[kyq End: 20150820 20:03:20]




                            }
                            else
                                UART1_SendBytes("EDID_name setting fail!", strlen("EDID_name setting fail!"));


                            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
#endif
							ProModifyDefaultEDIDName(pp);
                        }
                        else if (*pp == 0xf1)
                        {
#if 0
                            PSYSTEM_SETTING_DATA   psSystemSettingData;
                            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

                            pp++;
                            for (i = 0; i < 16; i++)
                            {
                                if (*(pp + i) != 0xfe)
                                    LCD1_IDLE_TEXT[i] = *(pp + i);
                                else
                                    break;
                            }
							LCD1_IDLE_TEXT[16] = 0;
                            if (i == 16)
                            {

                                memcpy(psSystemSettingData->acLCDLine1DefaultTXT, LCD1_IDLE_TEXT, 16);
                                appUIStateChange(uiSPLASH);

                                {
                                    UART1_SendBytes("LCD 1: ", strlen("LCD 1: "));
                                    for (j = 0; j < 16; j++)
                                    {
                                        UART1_SendByte(LCD1_IDLE_TEXT[j]);
                                    }
                                    UART1_SendBytes("\r\n", 2);
                                }
                            }
                            else
                            {
                                UART1_SendBytes("Error!\r\n", strlen("Error!\r\n"));
                            }
                            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
#endif
							ProModifyLCD1Display(pp);
                        }
                        else if (*pp == 0xf2)
                        {
                        #if 0
                            PSYSTEM_SETTING_DATA   psSystemSettingData;
                            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

                            pp++;
                            for (i = 0; i < 16; i++)
                            {
                                if (*(pp + i) != 0xfe)
                                    LCD2_IDLE_TEXT[i] = *(pp + i);
                                else
                                    break;
                            }
                            LCD2_IDLE_TEXT[16] = 0;
                            if (i == 16)
                            {

                                memcpy(psSystemSettingData->acLCDLine2DefaultTXT, LCD2_IDLE_TEXT, 16);
                                appUIStateChange(uiSPLASH);

                                {

                                    UART1_SendBytes("LCD 2: ", strlen("LCD 2: "));
                                    for (j = 0; j < 16; j++)
                                    {
                                        UART1_SendByte(LCD2_IDLE_TEXT[j]);
                                    }
                                    UART1_SendBytes("\r\n", 2);
                                }
                            }
                            else
                            {
                                UART1_SendBytes("Error!\r\n", strlen("Error!\r\n"));
                            }
                            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
                        #endif
							ProModifyLCD2Display(pp);
                        }
//===========================================================================
                        else if (*pp == 0xf3)
                        {
                        #if 0
                            BYTE IP_add[4];

                            PSYSTEM_SETTING_DATA   psSystemSettingData;
                            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

                            if (get_ip_add(pp, IP_add))
                            {
                                //ip_set_ok = 1;
                                for (i = 0; i < 4; i++) //i2c_EE_write_one_byte(sys_IP_add + i, IP_add[i]);
                                {
                                    psSystemSettingData->acIP_Address[i] = IP_add[i];
                                }
                                appUIStateChange(uiUART_IP_INFO);

                            }
                            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
                        #endif
							ProModifyIPAddress(pp);
                        }
//==============================================================================

                    USART1_rxtailpoint = USART1_rxprocesspoint; //��βָ���ͷָ��

                }
                IWDG_ReloadCounter(); //ι��
            }
            USART1_rxpoint = USART1_RX_BUFF;
            memset(USART1_RX_BUFF, 0, i); //����⴮ָ��
        }
        else if (USART_data_flag == 1)
        {
            IWDG_ReloadCounter(); //ι��
            EdidUpataStart = SiiPlatformTimerSysTicksGet();
            do_with_EDID_UP();
            USART_data_flag = 0;
            USART_EDIDdata_count = 0;

        }
        else if (USART_data_flag == 2)
        {
            IWDG_ReloadCounter(); //ι��
            EdidUpataStart = SiiPlatformTimerSysTicksGet();
            do_with_int_EDID_UP();
            USART_data_flag = 0;
            USART_EDIDdata_count = 0;
        }

        g_bUSART1_DataComplete = 0;
    }
}

void UART1_Init(void)
{
    USART1_rxpoint = USART1_RX_BUFF;
}

void APP_Printf(char *fmt, ...) // (char* fmt, ...)
{
}

//�������ݷ���DB9(PC)
void UART1_SendData2PC(PBYTE pcBuf, BYTE cLength)
{
    dvCpldMcuUartConnectToRemote(FALSE); // ����������ݣ�����Զ�˶����յ�����
    UART1_SendBytes(pcBuf, cLength);
    UART1_SendBytes("\r\n",2);
    dvCpldMcuUartConnectToRemote(TRUE);
}
//�������ݷ���Remote(402)
// ���������������
void UART1_SendData2Remote(BYTE cRemote, PBYTE pcBuf, BYTE cLength)
{
    dvCpldUartRemote2McuSet(cRemote);
    UART1_SendBytes(pcBuf, cLength);
    dvCpldUartRemote2McuSet(0xf);
}


#if 0

void dbg(char *fmt, ...)
{
	// the function does not check buffer length,
	// i.e. RS232_printf() may cause buffer overflow.
	va_list arg_ptr;
	char buf[200] = {0};

	va_start (arg_ptr, fmt);
	vsprintf (buf, fmt, arg_ptr);			 // Use the Sys_Ram in Flash.c to save the usage of ram
//	vsprintf (Sys_Ram, fmt, arg_ptr);
	va_end (arg_ptr);
	UART1_SendBytes(buf,strlen(buf));
	UART1_SendBytes("\r\n",2);
}

void err(char *fmt, ...)
{
	// the function does not check buffer length,
	// i.e. RS232_printf() may cause buffer overflow.
	va_list arg_ptr;
	char buf[200] = {0};

	va_start (arg_ptr, fmt);
	vsprintf (buf, fmt, arg_ptr);			 // Use the Sys_Ram in Flash.c to save the usage of ram
//	vsprintf (Sys_Ram, fmt, arg_ptr);
	va_end (arg_ptr);
	UART1_SendBytes(buf,strlen(buf));
	UART1_SendBytes("\r\n",2);
}

void warn(char *fmt, ...)
{
	// the function does not check buffer length,
	// i.e. RS232_printf() may cause buffer overflow.
	va_list arg_ptr;
	char buf[200] = {0};

	va_start (arg_ptr, fmt);
	vsprintf (buf, fmt, arg_ptr);			 // Use the Sys_Ram in Flash.c to save the usage of ram
//	vsprintf (Sys_Ram, fmt, arg_ptr);
	va_end (arg_ptr);
	UART1_SendBytes(buf,strlen(buf));
	UART1_SendBytes("\r\n",2);
}

void info(char *fmt, ...)
{
	// the function does not check buffer length,
	// i.e. RS232_printf() may cause buffer overflow.
	va_list arg_ptr;
	char buf[200] = {0};

	va_start (arg_ptr, fmt);
	vsprintf (buf, fmt, arg_ptr);			 // Use the Sys_Ram in Flash.c to save the usage of ram
//	vsprintf (Sys_Ram, fmt, arg_ptr);
	va_end (arg_ptr);
	UART1_SendBytes(buf,strlen(buf));
	UART1_SendBytes("\r\n",2);
}

void edid_info(char *fmt, ...)
{
	// the function does not check buffer length,
	// i.e. RS232_printf() may cause buffer overflow.
	va_list arg_ptr;
	char buf[200] = {0};

	va_start (arg_ptr, fmt);
	vsprintf (buf, fmt, arg_ptr);			 // Use the Sys_Ram in Flash.c to save the usage of ram
//	vsprintf (Sys_Ram, fmt, arg_ptr);
	va_end (arg_ptr);
	UART1_SendBytes(buf,strlen(buf));
}


#else
int DEBUG_PRINT(unsigned int x,char *fmt, ...)
{
}

void dbg(char *fmt, ...)
{
}

void err(char *fmt, ...)
{
}

void warn(char *fmt, ...)
{
}

void info(char *fmt, ...)
{
}

void edid_info(char *fmt, ...)
{
	#if 0

	// the function does not check buffer length,
	// i.e. RS232_printf() may cause buffer overflow.
	va_list arg_ptr;
	char buf[200] = {0};

	va_start (arg_ptr, fmt);
	vsprintf (buf, fmt, arg_ptr);			 // Use the Sys_Ram in Flash.c to save the usage of ram
//	vsprintf (Sys_Ram, fmt, arg_ptr);
	va_end (arg_ptr);
	UART1_SendBytes(buf,strlen(buf));

	#endif
}

#endif


#if 0
#define _PRINTF_C_

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
//#include "USART.h"
//#include "Printf.h"

#ifndef ASSERT
#define ASSERT(expr)    do {} while (0)
#endif

#define COMPATIBILITY_8051      1

#define to_digit(c)     ((c) - '0')
#define is_digit(c)     ((unsigned) to_digit(c) <= 9)

static const char hex16[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                              '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

/*
 * %[-^][digits][.digits][scd]
 */

#define ZEROPAD     (1 << 0)
#define LADJUST     (1 << 1)
#define CADJUST     (1 << 2)

/*
 * Length
 */
#define LEN_DEFAULT     0
#define LEN_CHAR        1
#define LEN_SHORT       2
#define LEN_LONG        3
#define LEN_LONGLONG    4

#define PRINTF_CHAR_MAX     256

#define NUM(str, n, base)                   \
    do {                                    \
        if (n < base)                       \
            *--str = hex16[n];              \
        else                                \
            do {                            \
                *--s = hex16[n % base];     \
            } while ( (n /= base) );        \
    } while (0)

int Uart_vsnprintf(char *str, size_t size, const char *fmt, va_list ap)
{
    signed int          sInt;
    unsigned int        uInt;
    signed long long    sLongLong;
    unsigned long long  uLongLong;

    int flags, width, prec, length;
    char *s, *p, *dst, *strMax;
    char digit[24];

    ASSERT(str != NULL && fmt != NULL);

    dst = str;
    strMax = str + size;

    while ( (uInt = *fmt++) )
    {

        if (uInt != '%')
        {
            if (dst < strMax)
                *dst++ = uInt;
            continue;
        }

        /* % */

        width = 0;
        length = LEN_DEFAULT;
        flags = 0;
        prec = -1;

    rflag:                /* read flag */
        uInt = *fmt++;

    reswitch:

        switch (uInt)
        {

        case 's':
            s = va_arg(ap, char *);
            if (width != 0 || prec >= 0)
            {
                p = s;
                if (prec < 0)
                    while (*p)
                        p++;
                else
                    while (*p && prec-- > 0)
                        p++;
                prec = p - s;
                break;
            }

            while ( (dst < strMax) && (*dst = *s) )
            {
                s++;
                dst++;
            }
            continue;

        case 'd':
        case 'u':
            p = s = digit + sizeof(digit);
            if (length == LEN_LONGLONG)
            {
                uLongLong = va_arg(ap, unsigned long long);
                if (uInt == 'd')
                {
                    sLongLong = uLongLong;
                    if (sLongLong < 0)
                        uLongLong = -sLongLong;
                    NUM(s, uLongLong, 10);
                    if (sLongLong < 0)
                        *--s = '-';
                }
                else
                {
                    NUM(s, uLongLong, 10);
                }
            }
            else
            {
                if (uInt == 'd')
                {
                    sInt = uInt = va_arg(ap, int);
                    if (sInt < 0)
                        uInt = -sInt;
                    NUM(s, uInt, 10);
                    if (sInt < 0)
                        *--s = '-';
                }
                else
                {
                    uInt = va_arg(ap, int);
                    NUM(s, uInt, 10);
                }
            }

            ASSERT( (p - s) <= (int) sizeof(digit));

            uInt = p - s;
            if (prec < 0 || prec > (int) uInt)
                prec = uInt;
            break;

        case 'p':
        case 'x': case 'X':
            p = s = digit + sizeof(digit);
            if (length == LEN_LONGLONG)
            {
                uLongLong = va_arg(ap, unsigned long long);
                NUM(s, uLongLong, 16);
            }
            else
            {
                uInt = va_arg(ap, int);
                NUM(s, uInt, 16);
            }

            ASSERT( (p - s) <= (int) sizeof(digit));

            uInt = p - s;
            if (prec < 0 || prec > (int) uInt)
                prec = uInt;
            break;

        case 'c':
            *(s = digit) = va_arg(ap, int);
            prec = 1;
            break;

        case '*':  /* must be postive value */
            width = va_arg(ap, int);
            goto rflag;

        case '0':
            flags |= ZEROPAD;
            goto rflag;

        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9':
            width = to_digit(uInt);

            while (is_digit( (uInt = *fmt++) ) )
            {
                /* width = 10 * width + to_digit(uInt); */
                width <<= 1;
                width += (width << 2) + to_digit(uInt);
            }
            goto reswitch;

        case '.':
            uInt = *fmt++;
            if (uInt == '*')
            {
                prec = va_arg(ap, int);
                goto rflag;
            }

            ASSERT(is_digit(uInt));

            if (is_digit(uInt))
            {
                prec = to_digit(uInt);

                while ( is_digit( (uInt = *fmt++) ) )
                    prec = 10 * prec + to_digit(uInt);
            }
            goto reswitch;

        case '-':
            flags |= LADJUST;
            goto rflag;

        case '^':           /* center */
            flags |= CADJUST;
            goto rflag;

    #if COMPATIBILITY_8051
        /* 8051 compatibility */
        case 'b':
        case 'B':
            length = LEN_CHAR;
            goto rflag;

        case 'h':
            length = (length == LEN_SHORT) ? LEN_CHAR : LEN_SHORT;
            goto rflag;

        case 'l':
        case 'L':
            length = (length == LEN_LONG) ? LEN_LONGLONG : LEN_LONG;
            goto rflag;
    #endif
        default:
            ASSERT(uInt == '%');
            if ((dst < strMax) && !(*dst++ = uInt))
                return (dst - str);
            continue;
        } /* switch */


        /* format */
        /* prec now is length of visual part of string s */
        if (width > prec)
        {
            uInt = width - prec;

            if ( !(flags & (LADJUST | CADJUST)) )
            {
                if (uInt > 0)
                {
                    flags = (flags & ZEROPAD) ?  '0' : ' ';
                    do {
                        if (dst < strMax)
                            *dst++ = flags;
                    } while (--uInt > 0);
                }
                while (prec-- > 0)
                {
                    if (dst < strMax)
                        *dst++ = *s++;
                }

            }
            else if (flags & LADJUST)
            {
                while (prec-- > 0 && dst < strMax)
                    *dst++ = *s++;
                while (uInt-- > 0 && dst < strMax)
                    *dst++ = ' ';

            }
            else    /* CADJUST */
            {
                width = uInt / 2;
                uInt -= width;

                while (width-- > 0 && dst < strMax)
                    *dst++ = ' ';
                while (prec-- > 0 && dst < strMax)
                    *dst++ = *s++;
                while (uInt-- > 0 && dst < strMax)
                    *dst++ = ' ';
            }
        }
        else
        {
            while (prec-- > 0 && dst < strMax)
                *dst++ = *s++;
        }
    } /* while */

    *dst = '\0';

    return (dst - str);
}


int Uart_snprintf(char *str, size_t size, const char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = Uart_vsnprintf(str, size, fmt, ap);
    va_end(ap);

    return ret;
}

extern void UART2_SendByte(u16 Data);

int Uart_printf(const char *fmt, ...)
{
    int i, iret;
    char szPrint[PRINTF_CHAR_MAX];
    va_list ap;

    va_start(ap, fmt);
    iret = Uart_vsnprintf(szPrint, PRINTF_CHAR_MAX, fmt, ap);
    va_end(ap);

    for (i = 0; i < iret; i++)
    {
        if(szPrint[i]=='\n')
            UART2_SendByte('\r');
        UART2_SendByte(szPrint[i]);
    }

    return iret;
}

#endif

