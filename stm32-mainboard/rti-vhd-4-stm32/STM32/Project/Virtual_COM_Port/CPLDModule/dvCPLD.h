#ifndef _CPLD_H_
#define _CPLD_H_

#include "stdarg.h"	
#include "mcu.h"

#define HC595_DATA_HIGH  	GPIOC->BSRR = GPIO_Pin_1;delay_nus(1)
#define HC595_DATA_LOW	  GPIOC->BRR = GPIO_Pin_1;delay_nus(1)

#define STCP_HIGH         GPIOC->BSRR = GPIO_Pin_3;delay_nus(1)
#define STCP_LOW          GPIOC->BRR = GPIO_Pin_3;delay_nus(1)

#define SHCP_HIGH         GPIOC->BSRR = GPIO_Pin_2;delay_nus(1)          
#define SHCP_LOW          GPIOC->BRR = GPIO_Pin_2;delay_nus(1)

#define set_ack_high      GPIOC->BSRR = GPIO_Pin_0;delay_nus(1)
#define CPLD_ACK		     (GPIOC->IDR  & GPIO_Pin_0)

void send_data_to_FPGA(unsigned char data);
unsigned char check_ack(void);

//表示把第y路远端402的红外信号切到第x路本地IR OUT
void dvCPLDIRSwitchSet(BYTE cRemoteIRIn, BYTE cLocalIROut);

//   bEnable - TRUE
//                   有载波就直通，没载波 CPLD 加上载波
//               -FASLE
//                  直通
void dvCPLDIRCarrierSet(BOOL bEnable);

// 远端过来的数据
// 本机232信号(MCU)切到第y路远端。
// cRemoteUart - 0: 表示断开
//             - f: 表示开启所有远端过来的串口数据
void dvCpldUartRemote2McuSet(BYTE cRemoteUart);

//
//ee    : 数据包包头
//cRemote: 通道号, 从0 开始
//bps_rate: 以何种波特率发送串口数据给远端, 从1开始
//data_0
// ef:数据包结束
void dvCpldSendDataToRemote(BYTE cRemote, BYTE cbps_rate, PBYTE pcData, BYTE cDataLength);
void dvCpldRemoteBraundSet(BYTE cRemote);
#ifdef PRD_IR_OUT
//把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void dvCPLDIRSwitcherSet(BYTE cInput, BYTE cPortNumber, va_list pArgs);

//把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0, MAX_IN will be mute the output
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void dvCPLDIRSwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList);
#endif
// MCU 与 DB9( PC 通讯时候 调用这个函数? 数据不会发给402?
//        串口放松完数据后, 恢复
//bConnect - 0: 断开远端
void dvCpldMcuUartConnectToRemote(BOOL bConnect);


BYTE dvCpldInit(void);

void dvCpldOpen(void);
void dvCpldClose(void);
//#ifdef VHD4_CPLD
void dvCpldOpenMCUToDB9(BOOL bEnable);
//#endif

#endif
