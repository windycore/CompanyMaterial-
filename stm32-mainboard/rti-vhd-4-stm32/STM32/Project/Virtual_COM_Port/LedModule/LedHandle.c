#include "LedHandle.h"

extern void TIM4_Configuration(void); //180us--->200us

static DWORD m_wLEDGPIO = 0;
// ����ָ���ĵ���,����δָ���Ĳ���
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


// �õ���Щ����,���˵�Դָʾ��
// return - Ϊ1 ��
DWORD dvLedOnGet(void)
{
   // // 0: ��,1 : ����
     DWORD  LedOnState;
   LedOnState=(~m_wLEDGPIO)&(~(1<<elnRED|1<<elnGREEN));
    return LedOnState;

}

// ��תָ���ĵ�,�����Ȳ���
void dvLedToggleSet(DWORD wLedGPIO)
{
	DWORD wPowerLED;
	m_wLEDGPIO ^= wLedGPIO;
	wPowerLED = (m_wLEDGPIO&(1<<elnRED|1<<elnGREEN)); 		// power led���䣬������0
	m_wLEDGPIO =	~m_wLEDGPIO;			//ȡ��
	m_wLEDGPIO =	(m_wLEDGPIO&(~(1<<elnRED|1<<elnGREEN)));	//power led���㣬��������
	m_wLEDGPIO |= wPowerLED;				//�ָ�power led�ĵ�			
    	wLEDRegGPIO(m_wLEDGPIO);
}

// ����ָ���ĵ���,δָ���Ķ����� // not change the power led
void dvLedAllOnSet(DWORD wLedGPIO)
{
      DWORD wPowerLED;
	wPowerLED = (m_wLEDGPIO&(1<<elnRED|1<<elnGREEN))&((1<<elnRED|1<<elnGREEN)); // not change the power led
	m_wLEDGPIO &= wLedGPIO;		//�Ȱ�����λ�����㣬Ҫָ���ĵƲ���
    	m_wLEDGPIO |=  wLedGPIO;		//��Ҫָ���ĵ���1����Ϊ��1��ʾ����
   	m_wLEDGPIO |= wPowerLED;		//��ԭ��λ����ĵ�Դ��
       wLEDRegGPIO(m_wLEDGPIO);
}

// ����ָ���ĵ���,δָ���Ķ����� // not change the power led
void dvLedAllOffSet(DWORD wLedGPIO)
{
      DWORD wPowerLED;
	wPowerLED = (m_wLEDGPIO&(1<<elnRED|1<<elnGREEN))&((1<<elnRED|1<<elnGREEN)); // not change the power led
	m_wLEDGPIO &=(~ wLedGPIO);		//�Ȱ�����λ�����㣬Ҫָ���ĵƲ���
    //	m_wLEDGPIO |=  wLedGPIO;		//��Ҫָ���ĵ���1����Ϊ��1��ʾ����
   	m_wLEDGPIO |= wPowerLED;		//��ԭ��λ����ĵ�Դ��
       wLEDRegGPIO(m_wLEDGPIO);
}

bool_t LedInitialize(void)
{
    bool_t  error=0;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
     if(!TM1623_Init())
     	{ 	error=1;
#if INC_UART	 
      if(psSystemSettingData->cPOSTEnable)
	 UART1_SendData2PC("TM1623 fail!", strlen("TM1623 fail!"));
#endif
     	}
     utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);   
     dvLedOnSet(1 << elnGREEN);
     dvLedOffSet(1 << elnRED);
     TIM4_Configuration();
    return error;
}







