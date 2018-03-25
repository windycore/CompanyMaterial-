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

//��ʾ�ѵ�y·Զ��402�ĺ����ź��е���x·����IR OUT
void dvCPLDIRSwitchSet(BYTE cRemoteIRIn, BYTE cLocalIROut);

//   bEnable - TRUE
//                   ���ز���ֱͨ��û�ز� CPLD �����ز�
//               -FASLE
//                  ֱͨ
void dvCPLDIRCarrierSet(BOOL bEnable);

// Զ�˹���������
// ����232�ź�(MCU)�е���y·Զ�ˡ�
// cRemoteUart - 0: ��ʾ�Ͽ�
//             - f: ��ʾ��������Զ�˹����Ĵ�������
void dvCpldUartRemote2McuSet(BYTE cRemoteUart);

//
//ee    : ���ݰ���ͷ
//cRemote: ͨ����, ��0 ��ʼ
//bps_rate: �Ժ��ֲ����ʷ��ʹ������ݸ�Զ��, ��1��ʼ
//data_0
// ef:���ݰ�����
void dvCpldSendDataToRemote(BYTE cRemote, BYTE cbps_rate, PBYTE pcData, BYTE cDataLength);
void dvCpldRemoteBraundSet(BYTE cRemote);
#ifdef PRD_IR_OUT
//�������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void dvCPLDIRSwitcherSet(BYTE cInput, BYTE cPortNumber, va_list pArgs);

//�������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0, MAX_IN will be mute the output
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void dvCPLDIRSwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList);
#endif
// MCU �� DB9( PC ͨѶʱ�� �����������? ���ݲ��ᷢ��402?
//        ���ڷ��������ݺ�, �ָ�
//bConnect - 0: �Ͽ�Զ��
void dvCpldMcuUartConnectToRemote(BOOL bConnect);


BYTE dvCpldInit(void);

void dvCpldOpen(void);
void dvCpldClose(void);
//#ifdef VHD4_CPLD
void dvCpldOpenMCUToDB9(BOOL bEnable);
//#endif

#endif
