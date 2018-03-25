//file dvPC9539.h
#ifndef _DVPC9539_H_
#define _DVPC9539_H_
#include "stm32f10x.h"
#include "LedHandle.h"
//command type 
#define inport0       0x00
#define inport1       0x01
#define outport0     0x02
#define outport1     0x03
#define config_inverseport0   0x04
#define config_inverseport1   0x05
//If a bit in these registers is set to 1, the corresponding port pin is enabled as a high-impedance input.
//If a bit in these registers is cleared to 0, the corresponding port pin is enabled as an output.
#define config_port0   0x06
#define config_port1   0x07
#define  IIC_9539_key_led        3  //IIC����   ����9539����key��led
#define  IIC_9539_1              4  //IIC����   ����9539���� EDID_EN,HPD��⣬UART_EN
#define  IIC_9539_2              5  //IIC����   ����9539����9187��Ƭѡ��PI3HDMI412Ƭѡ����Դ���أ������豸�����⣬�����������IO��չ
typedef enum
{
    epi9539_IN5VDET_OUTHPDDET,
#if(PCA9539_NUM_INSTANCES>1)
    epi9539_9679RST,
#endif
#if(PCA9539_NUM_INSTANCES>2)
    epi9539_LINK_POECTRL,
#endif
    epi9539_INVALID,
} ePCA9539_INSTANCE;

typedef enum
{
    IO0_0=0,
    IO0_1,
    IO0_2,
    IO0_3,
    IO0_4,
    IO0_5,
    IO0_6,
    IO0_7,
    
    IO1_0,
    IO1_1,
    IO1_2,
    IO1_3,
    IO1_4,
    IO1_5,
    IO1_6,
    IO1_7,
    
    IO_All,
    
    IO_max,
}enPCA9539Ioport;

eRESULT  dvPC9539Write( BYTE cInstance, uint8_t rigster_add,WORD wData);
eRESULT  dvPC9539Read(BYTE cInstance,uint8_t rigster_add,uint8_t *pdata);
//void power_LED(uint8_t type);
//uint8_t check_HPD(uint8_t port);
void  dvPC9539Init(void) ;
void appPCA9539Write(BYTE cInstance, uint8_t u8IO, WORD u8Data);
void Output5VConctrol(WORD OutputChannel  ,BYTE OnOff);//  1��ʾ��   0��ʾ��
#endif
