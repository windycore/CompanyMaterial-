///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <EDID.h>
//   @author Max.Kao@ite.com.tw
//   @date   2011/12/01
//   @fileversion: IT6623_Sample_1.04
//******************************************/


#ifndef _UART_H_
#define _UART_H_
#include "stm32f10x.h"
#include <string.h>
#include "mcu.h"



#define  USART1_RX_BUFF_MAX     255  //[kyq Begin: 20151021 16:58:04]
#define  USART1_TX_BUFF_MAX     255  //[kyq Begin: 20151021 16:58:06]

typedef enum{
	eSWITCH_OFF,
	eSWITCH_ON,
	eSWITCH_INVALID
}SWITCH_e;

//void init_uart1_part(void);
//初始化变量
void UART1_Init(void);

void UART1_SendByte(u16 Data);
void UART1_SendBytes(unsigned char   *Data,u8 num);
void Uart1_process(void);

void ClearUartInEdidValue(void);
void SetUartInEdidValue(unsigned char port,unsigned char value);
void GetUartInEdidValue(unsigned char *p);

void info(char *fmt, ...);

//void init_uart2_part(void);


unsigned char *StrCmp(unsigned char *str1, unsigned char *str2,unsigned char *USART1_rxprocesspoint);
BOOL  StrCmp2(unsigned char *str1, unsigned char *str2, unsigned char *USART1_rxEnd); //USART1_rxprocesspoint 是尾指令
unsigned char get_ip_add(unsigned char *pp, unsigned char *data);
uint8_t do_with_type(unsigned char RXdata ,unsigned char *USART1_rxtailpoint,unsigned char *USART1_rxprocesspoint);
void SaveToChannelData(uint8_t IN, uint8_t OUT, uint8_t SignalType, uint8_t CMDType);
//void SaveToUSARTLCD(uint8_t in, uint8_t out, uint8_t SignalType, uint8_t CMDType);
void Demo_process(void);
unsigned char EDID_GetDataBlockAddr(unsigned char *pTarget, unsigned char Tag, unsigned char Start);
void SW_Undo(void);
// 更新输出端口的EDID 到输入卡板的 edid
unsigned char EDIDM_process(unsigned char EDIDM_out,unsigned char EDIDM_in) ;
void PWON_process(void);
void PWOFF_process(void);
void standby_wake_up(void);
void standby_process(void);
//gxx 131023
void PWON_senddata(void) ;
void PWOFF_VideoAndAudio(void);
//void close_audio();

//void decode_cmd(unsigned char type,unsigned char num,unsigned char data); //num范围为1到8 
bool_t UserUARTinitialize(void);

//extern vu8   rxdelay_USART1 ;
extern char  USART1_tx_buff[USART1_TX_BUFF_MAX] ;
//extern uint8_t UnDo_vBuff[MAX_OUT];      //used to UnDo function. Save previous available operation
//extern uint8_t CurrOp_vBuff[MAX_OUT];    //used to UnDo function. Save the current operaton
//extern uint8_t UnDo_rBuff[MAX_OUT];      //used to UnDo function. Save previous available operation
//extern uint8_t CurrOp_rBuff[MAX_OUT];    //used to UnDo function. Save the current operaton
//extern uint8_t UnDo_AudioBuff[MAX_OUT];      //used to UnDo function. Save previous available operation
//extern uint8_t CurrOp_AudioBuff[MAX_OUT];    //used to UnDo function. Save the current operaton
//extern uint8_t DEMO_delay;
//extern uint8_t sDEMOFlag ;
//extern uint8_t powerflag ;
//extern uint8_t Re_Message;
//extern uint8_t IR_carrier_mode;

//extern vu8 uart_data_in_start;//有数据在接收
//extern vu8 uart_data_in_end;//数据接收完成

extern vu8  USART_data_flag ;
extern vu16  USART_EDIDdata_count ;
extern BYTE acEDID_data[256];

//extern uint8_t PWON_rBuff[MAX_OUT][48] ;
//extern uint8_t PWOFF_rBuff[MAX_OUT][48] ;
//extern uint8_t PWON_datalen[MAX_OUT] ;
//extern uint8_t PWOFF_datalen[MAX_OUT] ;
//extern uint8_t PWON_baud[MAX_OUT];
//extern uint8_t PWOFF_baud[MAX_OUT];

//extern uint8_t powerflag;//开机关机标志位

//extern uint8_t digit_audio[MAX_OUT] ;
//extern uint8_t IntOrExt_audio[2];

//gxx 131009
//extern uint8_t standby ;
//extern uint8_t standby_old ;


//extern uint8_t IP_add[4];

//extern char LCD1_UPdata[17] ;
//extern char LCD2_UPdata[17] ;

//extern vu8 rxdelay_USART2 ;
//extern char  USART2_tx_buff[USART2_TX_BUFF_MAX] ;
//extern uint8_t USART2_buff[USART2_RX_BUFF_MAX] ;

//extern uint8_t _TX_HDCP_On[MAX_OUT];  //输出通道的HDCP状态变量
//extern uint8_t _TX_HDCP_temp[MAX_OUT];//输出通道的HDCP状态暂存变量
//extern uint8_t HDCPOFF_Flag[MAX_OUT];
//extern uint8_t _TX_HDCP[MAX_OUT];
//extern uint8_t FirstmanegaHDCP ;

//extern uint8_t HPDflag[MAX_OUT];
//extern uint8_t HPDChanflag[MAX_OUT];
//extern uint8_t proc9533_flag;
//extern uint16_t proc9533_delay;
//extern uint16_t HPDflag_delay;

//extern uint8_t OutSwitch_Flag[MAX_OUT];
//extern uint8_t FirstPower_Flag;
//extern uint16_t OutSwitch_TimeDelay;
//extern uint16_t Switch_TimeDelay_3s;
//extern uint8_t HPDLowToHigh_Flag[MAX_OUT];
//extern uint16_t HPDLowToHigh_TimeDelay;

//extern uint16_t SwitchChannle_TimeDelay;
//extern uint8_t LedTimeFlag;

//extern uint8_t time_delay;

//extern vu8 uart_6623_wait ;
//extern vu8 USART2_count ;
//extern const  char cmd_6623[7] ;

//extern vu8 ip_coumt ;
//extern uint8_t ip_set_ok ;

//extern BYTE PWON_UART_DELAY_FLAG ;

//extern uint8_t  power_status_402  ;
//extern vu8      power_status_delay  ;

void UART1_SendData2PC(PBYTE pcBuf, BYTE cLength);
void UART1_SendData2Remote(BYTE cRemote, PBYTE pcBuf, BYTE cLength);

extern int Uart_printf(const char *fmt, ...);

#endif
