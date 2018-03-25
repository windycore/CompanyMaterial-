#include "LedHandle.h"
#include "appSystemMgrAPI.h"
#include "Uart.h"
#include "dvTM1623.h"

static DWORD m_wLEDGPIO = 0;
// 设置指定的灯亮,其他未指定的不变
void dvLedOnSet(DWORD wLedGPIO)
{
    m_wLEDGPIO |= wLedGPIO;

    wLEDRegGPIO(m_wLEDGPIO);
}

void dvLedOffSet(DWORD wLedGPIO)
{
    m_wLEDGPIO &= (~wLedGPIO);  // not change the power led

    wLEDRegGPIO(m_wLEDGPIO);
}


// 得到哪些灯亮,除了电源指示灯
// return - 为1 亮
DWORD dvLedOnGet(void)
{
   // // 0: 亮,1 : 不亮
     DWORD  LedOnState;
   LedOnState=(~m_wLEDGPIO)&(~(1<<elnRED|1<<elnGREEN));
    return LedOnState;

}

// 翻转指定的灯,其他等不变
void dvLedToggleSet(DWORD wLedGPIO)
{
	DWORD wPowerLED;
	m_wLEDGPIO ^= wLedGPIO;
	wPowerLED = (m_wLEDGPIO&(1<<elnRED|1<<elnGREEN)); 		// power led不变，其他清0
	m_wLEDGPIO =	~m_wLEDGPIO;			//取反
	m_wLEDGPIO =	(m_wLEDGPIO&(~(1<<elnRED|1<<elnGREEN)));	//power led清零，其他不变
	m_wLEDGPIO |= wPowerLED;				//恢复power led的灯			
    	wLEDRegGPIO(m_wLEDGPIO);
}

// 设置指定的灯亮,未指定的都不亮 // not change the power led
void dvLedAllOnSet(DWORD wLedGPIO)
{
      DWORD wPowerLED;
	wPowerLED = (m_wLEDGPIO&(1<<elnRED|1<<elnGREEN))&((1<<elnRED|1<<elnGREEN)); // not change the power led
	m_wLEDGPIO &= wLedGPIO;		//先把其他位都清零，要指定的灯不变
    	m_wLEDGPIO |=  wLedGPIO;		//把要指定的灯置1，因为置1表示灯亮
   	m_wLEDGPIO |= wPowerLED;		//还原两位不变的电源灯
       wLEDRegGPIO(m_wLEDGPIO);
}

// 设置指定的灯亮,未指定的都不亮 // not change the power led
void dvLedAllOffSet(DWORD wLedGPIO)
{
      DWORD wPowerLED;
	wPowerLED = (m_wLEDGPIO&(1<<elnRED|1<<elnGREEN))&((1<<elnRED|1<<elnGREEN)); // not change the power led
	m_wLEDGPIO &=(~ wLedGPIO);		//先把其他位都清零，要指定的灯不变
    //	m_wLEDGPIO |=  wLedGPIO;		//把要指定的灯置1，因为置1表示灯亮
   	m_wLEDGPIO |= wPowerLED;		//还原两位不变的电源灯
       wLEDRegGPIO(m_wLEDGPIO);
}

extern void TIM4_Configuration(void); //180us--->200us

bool_t LedInitialize(void)
{
    bool_t  error=0;
    BYTE i;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    for(i=0;i<TM1623_INSTANCES;i++)
    {
        if(TM1623_Init(i))
        { 	error=1;
            #if INC_UART	 
            if(psSystemSettingData->cPOSTEnable)
                UART1_SendData2PC("TM1623_Init fail!", strlen("TM1623_Init fail!"));
            #endif
        }
    }
    
    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    {
        BYTE CloseLed[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
        InOutPortLed_Process(CloseLed,1);
        SiiPlatformTimerSet(etLED_UPDATE,mS_TO_TICKS(1000L),0xFF); // use systick: 1ms per tick
    }
    Set_Tm1623ValueChangeFlag(true);
    #endif
    
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);   
    dvLedOnSet(1 << elnGREEN);
    dvLedOffSet(1 << elnRED);
    TIM4_Configuration();
    return error;
}







