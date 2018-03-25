#include "dvPC9539.h"
#include "BSP_I2C.h"
#include "mcu.h"
#include "bsp_Gpio.h"
#include "Util.h"
#include "dvTM1623.h"

//   0xEC //11101 10 0
//   0xEE //11101 11 0
//   0xE8 //11101 00 0
//   0xEA //11101 01 0
#if((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
#define ADDR_PCA9539_HPDDET_HDPCTRL    	0xE8
#else
#define ADDR_PCA9539_HPDDET_HDPCTRL    	0xEC
#endif

#define ADDR_PCA9539_RESET         			0xE8

static const BYTE m_acInstanceAddress[PCA9539_NUM_INSTANCES] =
{

 ADDR_PCA9539_HPDDET_HDPCTRL, 

#if(MACHINETYPE==MUH88E_BYE)
 ADDR_PCA9539_RESET,  
#endif
};
static const BYTE m_acBusID[PCA9539_NUM_INSTANCES] =
{
    eI2CBus9539,
#if(PCA9539_NUM_INSTANCES>1)	
    eI2CBus9539
#endif		
    
};
// �豸3   9539(������9539:
// 1 key_led   2����9539���� EDID_EN,HPD��⣬UART_EN    3����9539����9187��Ƭѡ��PI3HDMI412Ƭѡ����Դ���أ������豸�����⣬�����������IO��չ)
//uint8_t  write_PC9539(uint8_t type,uint8_t sla_add,uint8_t rigster_add,uint8_t rigster_data1,uint8_t rigster_data2)
// true : OK
// false: NOACK
eRESULT  dvPC9539Write( BYTE cInstance,uint8_t rigster_add,WORD wData)
{
    eRESULT  eResult = rcSUCCESS;
    return dvDeviceBusWrite( m_acBusID[cInstance],m_acInstanceAddress[cInstance], rigster_add,2, (PCBYTE)&wData, 0);    
    
}

eRESULT  dvPC9539Read( BYTE cInstance, uint8_t rigster_add,uint8_t *pdata)
{	
   // eRESULT  eResult = rcSUCCESS;
    return dvDeviceBusRead( m_acBusID[cInstance],m_acInstanceAddress[cInstance], rigster_add,2, pdata,0);       
}

void  dvPC9539Init(void)
{
#if(MACHINETYPE==MUH88E_BYE)
  //BFZ
    BSP_Gpio_Low(egpioPCA9539_Reset);
    delay_nms(20);
    BSP_Gpio_High(egpioPCA9539_Reset);
    delay_nms(20);
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port0,0xFFFF);       // �Ͱ�λΪ��������
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port1,0x0000);       // �߰�λΪ�������
    
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport1, 0x00);// output 5v control  low
	
    dvPC9539Write(epi9539_9679RST,config_port0,0x0000);       // �Ͱ�λΪ�������
    dvPC9539Write(epi9539_9679RST,config_port1,0x0000);       // �߰�λΪ�������
	
    dvPC9539Write(epi9539_9679RST,outport0, 0x0000);
    dvPC9539Write(epi9539_9679RST,outport1, 0x0000);
    delay_nms(10);
    dvPC9539Write(epi9539_9679RST,outport0, 0xffff);
    dvPC9539Write(epi9539_9679RST,outport1, 0xffff);
    //m_wLEDGPIO = 0;         //�ϵ��ȫ��
#elif (MACHINETYPE==MUH44E_BYE)
  //  FDS
    BSP_Gpio_Low(egpioPCA9539_Reset);
    delay_nms(20);
    BSP_Gpio_High(egpioPCA9539_Reset);
    delay_nms(20);
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port0,0x01FF);		// ����λΪ���������λΪ����
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport0, 0x00);
	
    //m_wLEDGPIO = 0;			//�ϵ��ȫ��
   // m_wLEDGPIO_TO_TM1623Port();
#elif ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    BSP_Gpio_Low(egpioPCA9539_Reset);
    delay_nms(20);
    BSP_Gpio_High(egpioPCA9539_Reset);
    delay_nms(20);
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port0,0x0000);		// ȫ������Ϊ���
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport0, 0xff);			// ������˿ڹ�5V��ѹ
	
    //m_wLEDGPIO = 0;			//�ϵ��ȫ��
    // m_wLEDGPIO_TO_TM1623Port();
#endif
}


#if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
//  1~~4�����ͨ��,OutputChannelֵ��0��ʼ
void Output5VConctrol(WORD OutputChannel  ,BYTE OnOff)//  1��ʾ��   0��ʾ��
{
		static WORD Output5VState;
		WORD GPIOVaule;

		dvPC9539Read(epi9539_IN5VDET_OUTHPDDET,outport0,(uint8_t*)&GPIOVaule);	
		GPIOVaule&=~(0x01<<(OutputChannel));
		GPIOVaule|=(OnOff<<(OutputChannel));
		dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport0,GPIOVaule);

}
#else
//  1~~8�����ͨ��
void Output5VConctrol(WORD OutputChannel  ,BYTE OnOff)//  1��ʾ��   0��ʾ��
{
		static WORD Output5VState;
		WORD GPIOVaule;

		dvPC9539Read(epi9539_IN5VDET_OUTHPDDET,outport0,(uint8_t*)&GPIOVaule);	
		GPIOVaule&=~(0x01<<(8+OutputChannel));
		GPIOVaule|=(OnOff<<(8+OutputChannel));
		dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport0,GPIOVaule);

}
#endif

