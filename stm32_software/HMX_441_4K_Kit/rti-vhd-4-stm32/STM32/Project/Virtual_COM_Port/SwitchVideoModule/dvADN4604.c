#include "dvADN4604.h"
#include "BSP_I2C.h"
#include "mcu.h"
#include "bsp_Gpio.h"
#include "Util.h"
static const BYTE m_acInstanceAddress[ADN4604_NUM_INSTANCES] =
{
    0x92,   //
#if(ADN4604_NUM_INSTANCES>1)
    0x96,   //
#endif
};
static const BYTE m_acBusID[ADN4604_NUM_INSTANCES] =
{
    eI2CBusAD4604_EEPROM,
#if(ADN4604_NUM_INSTANCES>1)
    eI2CBusAD4604_EEPROM,
#endif
};

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
extern BYTE KeyInput[8];
extern void Set_Tm1623ValueChangeFlag(bool_t nflag);
#endif

//�豸 6  ADV4604
eRESULT dvADN4604Write(BYTE  cInstance, uint8_t addr, uint8_t value)
{
    eRESULT eResult = rcSUCCESS;
    eResult = dvDeviceBusWrite(m_acBusID[cInstance],
                               m_acInstanceAddress[cInstance],
                               addr,
                               1,
                               &value,
                               0);
    return eResult;
}
uint8_t dvADN4604Read(BYTE  cInstance,  uint8_t addr)
{
    BYTE cValue = 0;
    eRESULT eResult = rcSUCCESS;
    eResult = dvDeviceBusRead(m_acBusID[cInstance],
                              m_acInstanceAddress[cInstance],
                              addr,
                              1,
                              &cValue,
                              0);
    ASSERT(eResult == rcSUCCESS);
    return cValue;
}
//void InitAdn4604(void)
void dvADN4604Init(void)
{
  unsigned char i;
 
    BSP_Gpio_Low(egpioANDReset);
    delay_nms(2);
    BSP_Gpio_High(egpioANDReset);
    delay_nms(2);
 
   for (i = 0; i < ADN4604_NUM_INSTANCES; i++)    
   {
    dvADN4604Write(i , 0x00, 0x01); //��ADN4604�Ĵ�����λ��Ĭ��ֵ
    delay_nms(2);
    dvADN4604Write(i , 0x81, 0x00);        //ѡ���һ���Ĵ�����ӳ��0
    delay_nms(2);
    //dvADN4604Write(i , 0xF0, 0x00);        //ʹ������˺�����˵Ķ˽ӵ���
    dvADN4604Write(i , 0xF0, 0xFC); 	   //ʹ������˽ӵ��������˲��ӵ���
    delay_nms(2);
    dvADN4604Write(i , 0x12, 0x00);        //�źŲ���ת
    delay_nms(2);
    dvADN4604Write(i , 0x13, 0x00);
    delay_nms(2);
   }
}



//�������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0, MAX_IN will be mute the output
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
#if (ADN4604_NUM_INSTANCES==1)
void dvADN4604SwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList)
{
    BYTE         i;
    BYTE         cOutput;
    // CH1|CH0            CH3|CH2
    // INPUTCH2|INPUTCH3, INPUTCH0|INPUTCH1
    BYTE         cMapCh[2]; // = {INPUTCH2|INPUTCH3,INPUTCH0|INPUTCH1}
                            // Start the varargs processing.
   
    for (i = 0; i < cPortNumber; i++)
    {
        cOutput  = pcOutPortList[i];
        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
        KeyInput[cOutput]=cInput;
        Set_Tm1623ValueChangeFlag(true);
        #endif
        cMapCh[0] = (cInput*4 + 2) << 4 | (cInput*4 + 3);
        cMapCh[1] = (cInput*4) << 4 | (cInput*4 + 1);
        // this will be disable output
        // �Ƿ��������?
        dvADN4604Write(0,  0x20 + 4 * cOutput, 0x04);
        delay_nus(10);
        dvADN4604Write(0,  0x20 + 4 * cOutput + 1, 0x04);
        delay_nus(10);
        dvADN4604Write(0,  0x20 + 4 * cOutput + 2, 0x04);
        delay_nus(10);
        dvADN4604Write(0,  0x20 + 4 * cOutput + 3, 0x04);
        delay_nus(10);
        dvADN4604Write(0, 0x80, 0x01); //updata
       if(cInput < MAX_IN)
        {
            // change the map
            dvADN4604Write(0, 0x90 + 2 * cOutput, cMapCh[0]);
            dvADN4604Write(0, 0x90 + 2 * cOutput + 1, cMapCh[1]);
            dvADN4604Write(0,  0x20 + 4 * cOutput, 0x34);
            delay_nus(10);
            dvADN4604Write(0,  0x20 + 4 * cOutput + 1, 0x34);
            delay_nus(10);
            dvADN4604Write(0,  0x20 + 4 * cOutput + 2, 0x34);
            delay_nus(10);
            dvADN4604Write(0,  0x20 + 4 * cOutput + 3, 0x34);
            delay_nus(10);
        }
    }
    dvADN4604Write(0, 0x80, 0x01); //updata
    
}
#elif(ADN4604_NUM_INSTANCES==2)
void dvADN4604SwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList)
{
    BYTE         i;
    BYTE         cOutput;
    // CH1|CH0            CH3|CH2
    // INPUTCH2|INPUTCH3, INPUTCH0|INPUTCH1
    BYTE         cMapCh[2]; // = {INPUTCH2|INPUTCH3,INPUTCH0|INPUTCH1}
                            // Start the varargs processing.
   
    for (i = 0; i < cPortNumber; i++)
    {
        cOutput  = pcOutPortList[i];
        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
        KeyInput[cOutput]=cInput;
        Set_Tm1623ValueChangeFlag(true);
        #endif
        cMapCh[0]=dvADN4604Read(0, 0x90+cOutput/2);  //��һ��TMDS//�ó��뵱ǰ���ͨ�����üĴ���������һ�����ͨ����Ӧ������
        cMapCh[1]=dvADN4604Read(0, 0x94+cOutput/2);  //��һ��TMDS//�ó��뵱ǰ���ͨ�����üĴ���������һ�����ͨ����Ӧ������
        if(cOutput%2)
        {
            cMapCh[0] =(cMapCh[0]&0x0f)| (cInput&0x0f)<<4;  	
            cMapCh[1] =(cMapCh[1]&0x0f)|( (cInput+8)&0x0f)<<4; 
        }
        else
        {
            cMapCh[0] =(cMapCh[0]&0xf0)| (cInput&0x0f);
            cMapCh[1] =(cMapCh[1]&0xf0)|( (cInput+8)&0x0f);
        }

        // this will be disable output
        // �Ƿ��������?
        dvADN4604Write(0,  0x20+cOutput, 0x04);  //��һ��TMDS
        dvADN4604Write(0,  0x28+cOutput, 0x04);  //�ڶ���TMDS
        dvADN4604Write(1,  0x20+cOutput, 0x04);  //������TMDS
        dvADN4604Write(1,  0x28+cOutput, 0x04);  //������TMDS	
        dvADN4604Write(0, 0x80, 0x01); //updata
        dvADN4604Write(1, 0x80, 0x01); //updata
        if (cInput < MAX_IN)
        {
            // change the map
            dvADN4604Write(0,0x90+cOutput/2, cMapCh[0]);
            dvADN4604Write(0,0x94+cOutput/2 , cMapCh[1]);

            dvADN4604Write(1,0x90+cOutput/2, cMapCh[0]);
            dvADN4604Write(1,0x94+cOutput/2 , cMapCh[1]);
            dvADN4604Write(0,  0x20+cOutput, 0x34);  //��һ��TMDS
            dvADN4604Write(0,  0x28+cOutput, 0x34);  //�ڶ���TMDS
            dvADN4604Write(1,  0x20+cOutput, 0x34);  //������TMDS
            dvADN4604Write(1,  0x28+cOutput, 0x34);  //������TMDS
        }
    }
    dvADN4604Write(0, 0x80, 0x01); //updata
    dvADN4604Write(1, 0x80, 0x01); //updata
    
}
#endif

//�������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0, MAX_IN will be mute the output
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
#if (ADN4604_NUM_INSTANCES==1)
void dvADN4604SwitcherClosePort( BYTE cPortNumber,PBYTE pcOutPortList)
{
    BYTE         i;
    BYTE         cOutput;

  
    for (i = 0; i < cPortNumber; i++)
    {
        cOutput  = pcOutPortList[i];
        // this will be disable output
        // �Ƿ��������?
        dvADN4604Write(0,  0x20 + 4 * cOutput, 0x04);
        dvADN4604Write(0,  0x20 + 4 * cOutput + 1, 0x04);
        dvADN4604Write(0,  0x20 + 4 * cOutput + 2, 0x04);
        dvADN4604Write(0,  0x20 + 4 * cOutput + 3, 0x04);
        dvADN4604Write(0, 0x80, 0x01); //updata

    }

    
}
#elif(ADN4604_NUM_INSTANCES==2)
void dvADN4604SwitcherClosePort(BYTE cPortNumber,PBYTE pcOutPortList)
{
    BYTE         i;
    BYTE         cOutput;

    for (i = 0; i < cPortNumber; i++)
    {
          cOutput  = pcOutPortList[i];
          // this will be disable output
        // �Ƿ��������?
        dvADN4604Write(0,  0x20+cOutput, 0x04);  //��һ��TMDS
        dvADN4604Write(0,  0x28+cOutput, 0x04);  //�ڶ���TMDS
	  dvADN4604Write(1,  0x20+cOutput, 0x04);  //������TMDS
        dvADN4604Write(1,  0x28+cOutput, 0x04);  //������TMDS	
        dvADN4604Write(0, 0x80, 0x01); //updata
        dvADN4604Write(1, 0x80, 0x01); //updata

    }
 
    
}
#endif
void dvADN4604PowerSet(BOOL bPowerOn)
{
}
