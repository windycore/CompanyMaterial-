#include "dvPC9539.h"
#include "BSP_I2C.h"
//#include "key_led_lcd.h"
#include "mcu.h"
#include "bsp_Gpio.h"
#include "Util.h"
#include "dvTM1623.h"

/*Device address : |1|1|1|0| |1|A1|A0|0| */
#define ADDR_PCA9539_HPDDET_HDPCTRL     0xEC    //                          cInstance = 2 
#if (MACHINETYPE==MHUB4K431)
#define ADDR_PCA9539_RESET              0xEE
#else
#define ADDR_PCA9539_RESET              0xE8   //                            cInstance = 3
#endif
#define ADDR_PCA9539_POECTRL            0xEA

static const BYTE m_acInstanceAddress[PCA9539_NUM_INSTANCES] =
{

 ADDR_PCA9539_HPDDET_HDPCTRL, 

#if (PCA9539_NUM_INSTANCES>1)
 ADDR_PCA9539_RESET,  
#endif
#if (PCA9539_NUM_INSTANCES>2)
 ADDR_PCA9539_POECTRL,  
#endif
};
static const BYTE m_acBusID[PCA9539_NUM_INSTANCES] =
{
    eI2CBus9539,
#if(PCA9539_NUM_INSTANCES>1)	
    eI2CBus9539,
#endif		
#if(PCA9539_NUM_INSTANCES>2)	
    eI2CBus9539,
#endif		
};
// 设备3   9539(有三种9539:
// 1 key_led   2控制9539控制 EDID_EN,HPD检测，UART_EN    3控制9539控制9187的片选，PI3HDMI412片选，电源开关，输入设备接入检测，输入输出卡板IO扩展)
//uint8_t  write_PC9539(uint8_t type,uint8_t sla_add,uint8_t rigster_add,uint8_t rigster_data1,uint8_t rigster_data2)
// true : OK
// false: NOACK
eRESULT  dvPC9539Write( BYTE cInstance,uint8_t rigster_add,WORD wData)
{
    //eRESULT  eResult = rcSUCCESS;
    return dvDeviceBusWrite( m_acBusID[cInstance],m_acInstanceAddress[cInstance], rigster_add,2, (PCBYTE)&wData, 0);    
    
 
}
eRESULT  dvPC9539Read( BYTE cInstance, uint8_t rigster_add,uint8_t *pdata)
{	
   // eRESULT  eResult = rcSUCCESS;
    return dvDeviceBusRead( m_acBusID[cInstance],m_acInstanceAddress[cInstance], rigster_add,2, pdata,0);       
}
void  dvPC9539Init(void)
{
#if((MACHINETYPE==MUH88E_BYE))
  //BFZ
    BSP_Gpio_Low(egpioPCA9539_Reset);
    delay_nms(20);
    BSP_Gpio_High(egpioPCA9539_Reset);
    delay_nms(20);
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port0,0xFFFF);       // 低八位为设置输入
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port1,0x0000);       // 高八位为设置输出
    
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport1, 0x00);// output 5v control  low
	
    dvPC9539Write(epi9539_9679RST,config_port0,0x0000);       // 低八位为设置输出
    dvPC9539Write(epi9539_9679RST,config_port1,0x0000);       // 高八位为设置输出
	
    dvPC9539Write(epi9539_9679RST,outport0, 0x0000);
    dvPC9539Write(epi9539_9679RST,outport1, 0x0000);
    delay_nms(10);
    dvPC9539Write(epi9539_9679RST,outport0, 0xffff);
    dvPC9539Write(epi9539_9679RST,outport1, 0xffff);
    //m_wLEDGPIO = 0;         //上电灯全灭
#elif (MACHINETYPE==MUH4K862)
    BSP_Gpio_Low(egpioPCA9539_Reset);
    delay_nms(20);
    BSP_Gpio_High(egpioPCA9539_Reset);
    delay_nms(20);
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port0,0xFFFF);       // 低八位为设置输入
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port1,0x0000);       // 高八位为设置输出

    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport1, 0xFF);// output 5v control  low

    dvPC9539Write(epi9539_9679RST,config_port0,0x0000);       // 低八位为设置输出
    dvPC9539Write(epi9539_9679RST,config_port1,0x0000);       // 高八位为设置输出

    dvPC9539Write(epi9539_9679RST,outport0, 0x0000);
    dvPC9539Write(epi9539_9679RST,outport1, 0x0000);
    delay_nms(10);
    dvPC9539Write(epi9539_9679RST,outport0, 0xffff);
    dvPC9539Write(epi9539_9679RST,outport1, 0xffff);
    
    dvPC9539Write(epi9539_LINK_POECTRL,config_port0, 0x3f00);    // 00111111 00000000   0 输出；1 输入
    //dvPC9539Write(epi9539_LINK_POECTRL,config_port1, 0xffff);   
#elif ((MACHINETYPE==MUH44E_BYE))
  //  FDS
    BSP_Gpio_Low(egpioPCA9539_Reset);
    delay_nms(20);
    BSP_Gpio_High(egpioPCA9539_Reset);
    delay_nms(20);
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port0,0x01FF);		// 高七位为输出，其他位为输入
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport0, 0x00);
	
    //m_wLEDGPIO = 0;			//上电灯全灭
   // m_wLEDGPIO_TO_TM1623Port();
#elif (MACHINETYPE==MUHBT44)
    BSP_Gpio_Low(egpioPCA9539_Reset);
    delay_nms(20);
    BSP_Gpio_High(egpioPCA9539_Reset);
    delay_nms(20);
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port0,0xFFFF);       // 高七位为输出，其他位为输入
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport0, 0xFFFF);
    
    dvPC9539Write(epi9539_9679RST,config_port0,0x0000);       // 00001111 00000000    0 输出；1 输入
    dvPC9539Write(epi9539_9679RST,outport0, 0xFFFF);

#elif (MACHINETYPE==MHUB4K431)
    BSP_Gpio_Low(egpioPCA9539_Reset);
    delay_nms(20);
    BSP_Gpio_High(egpioPCA9539_Reset);
    delay_nms(20);
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,config_port0,0xFFFF);       // 高七位为输出，其他位为输入
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport0, 0xFFFF);
    
    dvPC9539Write(epi9539_9679RST,config_port0,0x0000);       // 00001111 00000000    0 输出；1 输入
    dvPC9539Write(epi9539_9679RST,outport0, 0xFFFF);
#endif
}


void appPCA9539Write(BYTE cInstance, uint8_t u8IO, WORD u8Data)
{
    WORD  outport0_value;
    //WORD  outport1_value;
    dvPC9539Read(cInstance,outport0,(uint8_t*)&outport0_value);
    //dvPC9539Read(cInstance,outport1,&outport1_value);

    switch(u8IO)
    {
        // IO0
        case IO0_0:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT0):(outport0_value&(~BIT0)))|(outport0_value&0xFF00));
            break;
        case IO0_1:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT1):(outport0_value&(~BIT1)))|(outport0_value&0xFF00));
            break;
        case IO0_2:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT2):(outport0_value&(~BIT2))|(outport0_value&0xFF00)));
            break;
        case IO0_3:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT3):(outport0_value&(~BIT3)))|(outport0_value&0xFF00));
            break;
        case IO0_4:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT4):(outport0_value&(~BIT4)))|(outport0_value&0xFF00));
            break;
        case IO0_5:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT5):(outport0_value&(~BIT5)))|(outport0_value&0xFF00));
            break;
        case IO0_6:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT6):(outport0_value&(~BIT6)))|(outport0_value&0xFF00));
            break;
        case IO0_7:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT7):(outport0_value&(~BIT7)))|(outport0_value&0xFF00));
            break;
        // I01
        case IO1_0:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT8):(outport0_value&(~BIT8)))|(outport0_value&0x00FF));
            break;
        case IO1_1:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT9):(outport0_value&(~BIT9)))|(outport0_value&0x00FF));
            break;
        case IO1_2:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT10):(outport0_value&(~BIT10)))|(outport0_value&0x00FF));
            break;
        case IO1_3:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT11):(outport0_value&(~BIT11)))|(outport0_value&0x00FF));
            break;
        case IO1_4:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT12):(outport0_value&(~BIT12)))|(outport0_value&0x00FF));
            break;
        case IO1_5:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT13):(outport0_value&(~BIT13)))|(outport0_value&0x00FF));
            break;
        case IO1_6:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT14):(outport0_value&(~BIT14)))|(outport0_value&0x00FF));
            break;
        case IO1_7:
            dvPC9539Write(cInstance,outport0,((u8Data==1)?(outport0_value|BIT15):(outport0_value&(~BIT15)))|(outport0_value&0x00FF));
            break;
        case IO_All:
            dvPC9539Write(cInstance,outport0,u8Data);
            break;
            
        default:
            break;
    }
}

//  1~~8个输出通道
void Output5VConctrol(WORD OutputChannel  ,BYTE OnOff)//  1表示开   0表示低
{
	//static WORD Output5VState;
    #if (MACHINETYPE == MHUB4K431)
    appPCA9539Write(epi9539_IN5VDET_OUTHPDDET,(uint8_t)(IO1_1+OutputChannel),(WORD)OnOff);
    #elif (MACHINETYPE == MUH4K862)
    appPCA9539Write(epi9539_IN5VDET_OUTHPDDET,(uint8_t)(IO1_0+OutputChannel),(WORD)OnOff);
    delay_nms(10);
    #else
    WORD GPIOVaule;
    dvPC9539Read(epi9539_IN5VDET_OUTHPDDET,outport0,(uint8_t*)&GPIOVaule);	
    GPIOVaule&=~(0x01<<(8+OutputChannel));
    GPIOVaule|=(OnOff<<(8+OutputChannel));
    dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport0,GPIOVaule);
    #endif
      if(OnOff)
		 InitOutHDCP_Flag();
}

