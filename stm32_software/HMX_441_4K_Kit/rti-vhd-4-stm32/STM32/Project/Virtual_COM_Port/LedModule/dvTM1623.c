
#include "util.h"
#include "si_datatypes.h"
#include "stm32f10x.h"
#include "dvTM1623.h"
#include "LedHandle.h"
#include "Mcu.h"
#include "appSystemMgrAPI.h"
#include "appPowerMgrAPI.h"


static BYTE m_wLedReg[7] ={ 0};
static const BYTE m_acBusID[TM1623_INSTANCES] = {

eTM1623ID_1      

};

/************************************************************/
static bool_t Tm1623_ValueChangeFlag = false;

/*********************************************************************************************** 
*Function Name: Write_Byte       
*Purpose      : Write one byte to the data port 
*params       : byte  -------8-bits byte   
*return       : none 
***********************************************************************************************/  
void TM1623_Write_Byte(uint8_t byte)
{  
    	uint8_t i=0;  
	for(i=0;i<8;i++)
	{
		if(byte&0x01)
		{
			TM1623_DIO_HIGH;
		}
		else
		{ 
			TM1623_DIO_LOW;
		}
		TM1623_CLK_LOW;
		TM1623_CLK_HIGH;
		byte>>=1; 
	}
}

/*********************************************************************************************** 
*Function Name: Read_Byte        
*Purpose      : Read one byte from data port 
*params       : none 
*return       : the 8-bits byte which is read from data port 
***********************************************************************************************/  
uint8_t TM1623_Read_Byte(void)
{  
	uint8_t i=0;  
	uint8_t temp=0x00;  
	for(i=0;i<8;i++)
	{
		temp>>=1; 
		TM1623_CLK_LOW;
		TM1623_CLK_HIGH;
		if(GET_TM1623_DIO)
			temp|=0x80;  
	}
   	return temp;  
}  
 
/*********************************************************************************************** 
*Function Name: Write_Cmd        
*Purpose      : Write a conmand to the data port 
*params       : cmd  -------8-bits byte,the conmand,check the data sheet to find the conmand  
*return       : none 
***********************************************************************************************/  
void TM1623_Write_Cmd(BYTE cdevId,uint8_t cmd)
{  
    #if (MACHINETYPE == MUH4K862)
    if(cdevId == 0)
    {
        TM1623_STB1_HIGH;  
        TM1623_STB_HIGH;  
        TM1623_STB_LOW;  
    }
    else if(cdevId == 1)
    {
        TM1623_STB_HIGH;  
        TM1623_STB1_HIGH;  
        TM1623_STB1_LOW;  
    }
    #else
    TM1623_STB_HIGH;  
    TM1623_STB_LOW;  
    #endif
    
    TM1623_Write_Byte(cmd);  
}  
 
/*********************************************************************************************** 
*Function Name: Read_Key         
*Purpose      : Read the key number which has been pressed 
*params       : none 
*return       : the number of the key. 0-8.  "return 0" represents no key has been pressed. 
***********************************************************************************************/  
void TM1623_Read_Key(BYTE cdevId,uint8_t *key_value)
{  
    uint8_t i=0;  
    TM1623_Write_Cmd(cdevId,0x42);  
    TM1623_DIO_HIGH;                       //this is obligatory, check the data sheet,GPIO   
    for(i=0;i<5;i++)					  //连续读取5个字节
    {
		*key_value = TM1623_Read_Byte();
		key_value++;
    }
}  
/*********************************************************************************************** 
*Function Name: Read_Key         
*Purpose      : Read the key number which has been pressed 
*params       : none 
*return       : the number of the key. 0-8.  "return 0" represents no key has been pressed. 
***********************************************************************************************/  
void TM1623_Read_Reg(BYTE cdevId,uint8_t *Regdat)
{  
    //uint8_t i=0;  
    TM1623_Write_Cmd(cdevId,0x42);  
    TM1623_DIO_HIGH;                       //this is obligatory, check the data sheet,GPIO   
   *Regdat = TM1623_Read_Byte();

}  

/*********************************************************************************************** 
*Function Name: Write_Dat        
*Purpose      : Write data to the location specified 
*params       : addr  ------the address,0x00 to 0x0f 
        dat   ------the data,segment code 
*return       : none 
***********************************************************************************************/  
void TM1623_Write_Data(BYTE cdevId,uint8_t addr,uint8_t dat)
{  
    TM1623_Write_Cmd(cdevId,0x44);  
    TM1623_Write_Cmd(cdevId,0xc0|addr);  
    TM1623_Write_Byte(dat);  
}  

static BYTE AlreadyInitF[TM1623_INSTANCES]={
    0,
    #if (TM1623_INSTANCES>1)
    0,
    #endif
};
extern void TM1623_key_led_Config_moni(void);
/*********************************************************************************************** 
*Function Name: TM1638_Init      
*Purpose      : the initialization of tm1638 
*params       : none 
*return       : none 
***********************************************************************************************/  
uint8_t TM1623_Init(uint8_t cInstance)
{  
    uint8_t i=0;  
    uint8_t error=0;  
    //PSYSTEM_SETTING_DATA   psSystemSettingData;
    //psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
 
    if(!AlreadyInitF[cInstance])  //olny module  initializated  TM1623_Init
    {
        AlreadyInitF[cInstance]=TRUE;//already 
        //dvSelectChilpEnable(m_acBusID[cInstance]);
        TM1623_key_led_Config_moni();
      /************************************************************/
        TM1623_Write_Cmd(cInstance,0x03); 			//设置为7位11段模式
        TM1623_Write_Cmd(cInstance,LEVEL_2);  		//调节亮度
        for(i=0;i<14;i++)
        {  
            TM1623_Write_Data(cInstance,i,0x00);
        } 
    }
    //utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);   
    return error;
}

void wLEDRegGPIO( DWORD RegGPIO)
{
#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    RegGPIO=RegGPIO;
    return;
#else
	uint8_t i = 0;
    
	for (i = 0; i < eLedInvalid ;i++)
    {
		if((RegGPIO&BIT(i)) == 0)				//根据m_wLEDGPIO这个变量的每位对应的是0来给T1623原理图中实际控制灯的引脚也写0
		{
			switch (i)
		      {
			        case elnIN1:
							m_wLedReg[0] &= ~ BIT(0);
			            			break;
			        case elnIN2:
							m_wLedReg[0] &= ~ BIT(1);
			            			break;
			        case elnIN3:
							m_wLedReg[0] &= ~ BIT(2);		
				      			break;
			        case elnIN4:
							m_wLedReg[0] &= ~ BIT(3);
			            			break;
				 case elnIN5:
							m_wLedReg [0]&= ~ BIT(4);
			            			break;
			        case elnIN6:
							m_wLedReg[0] &= ~ BIT(5);
			            			break;
			        case elnIN7:
							m_wLedReg[0] &= ~ BIT(6);		
				      			break;
			        case elnIN8:
							m_wLedReg[0] &= ~ BIT(7);
			            			break;
			        case elnCLEAR:
							m_wLedReg [4]&= ~ BIT(5);
			            			break;
				  case elnENTER:
							m_wLedReg [4]&= ~ BIT(4);
			            			break;
			        case elnEDID:
							m_wLedReg [4] &= ~ BIT(0);
			            			break;
			        case elnOUT1:
							m_wLedReg[1] &= ~ BIT(0);
				      			break;
			        case elnOUT2:
							m_wLedReg[1] &= ~ BIT(1);
			            			break;
			        case elnOUT3:
							m_wLedReg[1] &= ~ BIT(2);
			            			break;
				  case elnOUT4:
							m_wLedReg [1]&= ~ BIT(3);
			            			break;
				  case elnOUT5:
							m_wLedReg[1]&= ~ BIT(4);
				      			break;
			        case elnOUT6:
							m_wLedReg[1] &= ~ BIT(5);
			            			break;
			        case elnOUT7:
							m_wLedReg[1] &= ~ BIT(6);
			            			break;
				  case elnOUT8:
							m_wLedReg [1]&= ~ BIT(7);
			            			break;
				  case elnALL:
							m_wLedReg[4] &= ~ BIT(0);
			            			break;
			        case elnRED:
							m_wLedReg[4] &= ~ BIT(0);	
			            			break;
			        case elnGREEN:
							m_wLedReg[2] &= ~ BIT(0);
				      			break;

			        default:	break;
			}
		}
		else			//根据m_wLEDGPIO这个变量的每位对应的是1来给T1623原理图中实际控制灯的引脚也写1
		{
			switch (i)
		      {
			        case elnIN1:
							m_wLedReg[0] |= BIT(0);
			            			break;
			        case elnIN2:
							m_wLedReg[0] |= BIT(1);
			            			break;
			        case elnIN3:
							m_wLedReg[0] |= BIT(2);		
				      			break;
			        case elnIN4:
							m_wLedReg[0] |= BIT(3);
				  case elnIN5:
							m_wLedReg[0] |= BIT(4);
			            			break;
			        case elnIN6:
							m_wLedReg[0] |= BIT(5);
			            			break;
			        case elnIN7:
							m_wLedReg[0] |= BIT(6);		
				      			break;
			        case elnIN8:
							m_wLedReg[0] |= BIT(7);
			            			break;
			        case elnCLEAR:
							m_wLedReg[4] |= BIT(5);
			            			break;
				  case elnENTER:
							m_wLedReg[4] |= BIT(4);
			            			break;
			        case elnEDID:
							m_wLedReg[4] |= BIT(0);
			            			break;
			        case elnOUT1:
							m_wLedReg[1] |= BIT(0);
				      			break;
			        case elnOUT2:
							m_wLedReg[1] |= BIT(1);
			            			break;
			        case elnOUT3:
							m_wLedReg[1] |= BIT(2);
			            			break;
				  case elnOUT4:
							m_wLedReg[1] |= BIT(3);
			            			break;
				 case elnOUT5:
							m_wLedReg[1] |= BIT(4);
				      			break;
			        case elnOUT6:
							m_wLedReg[1] |= BIT(5);
			            			break;
			        case elnOUT7:
							m_wLedReg[1] |= BIT(6);
			            			break;
				  case elnOUT8:
							m_wLedReg[1] |= BIT(7);
			            			break;
				  case elnALL:
							m_wLedReg[4] |= BIT(0);
			            			break;
			        case elnRED:
							m_wLedReg[4] |= BIT(0);	
			            			break;
			        case elnGREEN:
							m_wLedReg[2] |= BIT(0);
				      			break;

			        default:	break;
			}
		}
    }
	
    TM1623_Write_Data(0,0x00,m_wLedReg[0]);
    TM1623_Write_Data(0,0x02,m_wLedReg[1]);

    TM1623_Write_Data(0,0x04,m_wLedReg[2]);
    TM1623_Write_Data(0,0x06,m_wLedReg[3]);

    TM1623_Write_Data(0,0x08,m_wLedReg[4]);
    TM1623_Write_Data(0,0x0A,m_wLedReg[5]);
#endif
}

bool_t Get_Tm1623ValueChangeFlag(void)
{
    return Tm1623_ValueChangeFlag;
}

void Set_Tm1623ValueChangeFlag(bool_t nflag)
{
    Tm1623_ValueChangeFlag = nflag;
}

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
/***************************************************************************
**函数名称: InOutPortLed_Process
**输入参数: pOutput 输出端口; u8power 电源指示灯
**输出参数: 
**功能说明: 改变LED的状态
***************************************************************************/
#ifdef MODIFY_IR_SWITCH_2017_2_21
static BYTE u8dat1_record=0,u8dat2_record=0,u8dat3_record=0,u8dat4_record=0;
void LightColumnOutPortLed_Process(BYTE Output, bool bLight)
{
#if (MACHINETYPE==MHUB4K431)
	//BYTE u8dat1=0,u8dat2=0,u8dat3=0,u8dat4=0;
	
	u8dat1_record |= BIT4;
	switch(Output)
	{
		case 0:
			if(bLight)
			{
				u8dat1_record |= BIT3;
				u8dat2_record |= BIT3;
				u8dat3_record |= BIT3;
				u8dat4_record |= BIT3;
			}
			else
			{
				u8dat1_record &= ~BIT3;
				u8dat2_record &= ~BIT3;
				u8dat3_record &= ~BIT3;
				u8dat4_record &= ~BIT3;
			}
			break;
		case 1:
			if(bLight)
			{
				u8dat1_record |= BIT2;
				u8dat2_record |= BIT2;
				u8dat3_record |= BIT2;
				u8dat4_record |= BIT2;
			}
			else
			{
				u8dat1_record &= ~BIT2;
				u8dat2_record &= ~BIT2;
				u8dat3_record &= ~BIT2;
				u8dat4_record &= ~BIT2;
			}
			break;
		case 2:
			if(bLight)
			{
				u8dat1_record |= BIT1;
				u8dat2_record |= BIT1;
				u8dat3_record |= BIT1;
				u8dat4_record |= BIT1;
			}
			else
			{
				u8dat1_record &= ~BIT1;
				u8dat2_record &= ~BIT1;
				u8dat3_record &= ~BIT1;
				u8dat4_record &= ~BIT1;
			}
			break;
		case 3:
			if(bLight)
			{
				u8dat1_record |= BIT0;
				u8dat2_record |= BIT0;
				u8dat3_record |= BIT0;
				u8dat4_record |= BIT0;
			}
			else
			{
				u8dat1_record &= ~BIT0;
				u8dat2_record &= ~BIT0;
				u8dat3_record &= ~BIT0;
				u8dat4_record &= ~BIT0;
			}
			break;
		default:
			break;
	}

    TM1623_Write_Data(0,REG_GRID1_SEG1_8,u8dat1_record);
    TM1623_Write_Data(0,REG_GRID2_SEG1_8,u8dat2_record);
    TM1623_Write_Data(0,REG_GRID3_SEG1_8,u8dat3_record);
    TM1623_Write_Data(0,REG_GRID4_SEG1_8,u8dat4_record);
#elif(MACHINETYPE==MUH4K862)
	BYTE u8dat1=0,u8dat2=0,u8dat3=0,u8dat4=0,u8dat5=0,u8dat6=0,u8dat7=0,u8dat8=0,u8dat9=0;
    BYTE u8dat10=0,u8dat11=0;

    u8dat9 |= 1;
    switch(Output)
    {
		case 0:
			if(bLight)
				u8dat1 = 0xff;
			else
				u8dat1 = 0x00;
			TM1623_Write_Data(0,REG_GRID1_SEG1_8,u8dat1);
			break;
		case 1:
			if(bLight)
				u8dat2 = 0xff;
			else
				u8dat2 = 0x00;
			TM1623_Write_Data(0,REG_GRID2_SEG1_8,u8dat2);
			break;
		case 2:
			if(bLight)
				u8dat3 = 0xff;
			else
				u8dat3 = 0x00;
			TM1623_Write_Data(0,REG_GRID3_SEG1_8,u8dat3);
			break;
		case 3:
			if(bLight)
				u8dat4 = 0xff;
			else
				u8dat4 = 0x00;
			TM1623_Write_Data(0,REG_GRID4_SEG1_8,u8dat4);
			break;
		case 4:
			if(bLight)
				u8dat5 = 0xff;
			else
				u8dat5 = 0x00;
			TM1623_Write_Data(0,REG_GRID5_SEG1_8,u8dat5);
			break;
		case 5:
			if(bLight)
				u8dat6 = 0xff;
			else
				u8dat6 = 0x00;
			TM1623_Write_Data(0,REG_GRID6_SEG1_8,u8dat6);
			break;
		case 6:
			if(bLight)
				u8dat7 = 0xff;
			else
				u8dat7 = 0x00;
			TM1623_Write_Data(0,REG_GRID7_SEG1_8,u8dat7);
			break;
		case 7:
			if(bLight)
			{
				u8dat8 |= (BIT0|BIT1|BIT2);
				u8dat10 |= (BIT0|BIT1|BIT2);
				u8dat11 |= (BIT0|BIT1);
			}
			else
			{
				u8dat8 &= ~(BIT0|BIT1|BIT2);
				u8dat10 &= ~(BIT0|BIT1|BIT2);
				u8dat11 &= ~(BIT0|BIT1);
			}
			TM1623_Write_Data(0,REG_GRID5_SEG9_14,u8dat8);
    		TM1623_Write_Data(0,REG_GRID6_SEG9_14,u8dat10);
    		TM1623_Write_Data(0,REG_GRID7_SEG9_14,u8dat11);
			break;
		default:
			break;
    }           
    TM1623_Write_Data(0,REG_GRID1_SEG9_14,u8dat9);
#endif
}
#endif


void InOutPortLed_Process(PBYTE pOutput,BYTE u8power)
{
#if (MACHINETYPE==MHUB4K431)
    BYTE u8dat1=0,u8dat2=0,u8dat3=0,u8dat4=0;
    
    if(pOutput[0]<MAX_IN)
    {
        switch(pOutput[0])
        {
            case 0:
                u8dat1 |= BIT0;
                break;
            case 1:
                u8dat2 |= BIT0;
                break;
            case 2:
                u8dat3 |= BIT0;
                break;
            case 3:
                u8dat4 |= BIT0;
                break;
            default:
                break;
        }
    }
    
    if(pOutput[2]<MAX_IN)
    {
        switch(pOutput[2])
        {
            case 0:
                u8dat1 |= BIT1;
                break;
            case 1:
                u8dat2 |= BIT1;
                break;
            case 2:
                u8dat3 |= BIT1;
                break;
            case 3:
                u8dat4 |= BIT1;
                break;
            default:
                break;
        }
    }
    
    if(pOutput[1]<MAX_IN)
    {
        switch(pOutput[1])
        {
            case 0:
                u8dat1 |= BIT2;
                break;
            case 1:
                u8dat2 |= BIT2;
                break;
            case 2:
                u8dat3 |= BIT2;
                break;
            case 3:
                u8dat4 |= BIT2;
                break;
            default:
                break;
        }
    }
    
    if(pOutput[3]<MAX_IN)
    {
        switch(pOutput[3])
        {
            case 0:
                u8dat1 |= BIT3;
                break;
            case 1:
                u8dat2 |= BIT3;
                break;
            case 2:
                u8dat3 |= BIT3;
                break;
            case 3:
                u8dat4 |= BIT3;
                break;
            default:
                break;
        }
    }
    
    if(u8power)
    {
        u8dat1 |= BIT4;
    }
	#ifdef MODIFY_IR_SWITCH_2017_2_21
    u8dat1_record = u8dat1;
	u8dat2_record = u8dat2;
	u8dat3_record = u8dat3;
	u8dat4_record = u8dat4;
	#endif
    TM1623_Write_Data(0,REG_GRID1_SEG1_8,u8dat1);
    TM1623_Write_Data(0,REG_GRID2_SEG1_8,u8dat2);
    TM1623_Write_Data(0,REG_GRID3_SEG1_8,u8dat3);
    TM1623_Write_Data(0,REG_GRID4_SEG1_8,u8dat4);
#else
    //BYTE i,j;
    BYTE u8dat1=0,u8dat2=0,u8dat3=0,u8dat4=0,u8dat5=0,u8dat6=0,u8dat7=0,u8dat8=0,u8dat9=0;
    BYTE u8dat10=0,u8dat11=0;
    
    if(pOutput[0]<MAX_IN)
    {
        u8dat1 |= 1<<(pOutput[0]);
    }
    if(pOutput[1]<MAX_IN)
    {
        u8dat2 |= 1<<(pOutput[1]);
    }
    if(pOutput[2]<MAX_IN)
    {
        u8dat3 |= 1<<(pOutput[2]);
    }
    if(pOutput[3]<MAX_IN)
    {
        u8dat4 |= 1<<(pOutput[3]);
    }
    if(pOutput[4]<MAX_IN)
    {
        u8dat5 |= 1<<(pOutput[4]);
    }
    if(pOutput[5]<MAX_IN)
    {
        u8dat6 |= 1<<(pOutput[5]);
    }
    if(pOutput[6]<MAX_IN)
    {
        u8dat7 |= 1<<(pOutput[6]);
    }
    #if 0
    if(pOutput[7]<MAX_IN)
    {
        u8dat8 |= 1<<(pOutput[7]);
    }
    #else
    if(pOutput[7]<MAX_IN)
    {
        if(pOutput[7]<3)
            u8dat8 |= 1<<(pOutput[7]);
        else if(pOutput[7]<6)
            u8dat10 |= 1<<(pOutput[7]-3);
        else
            u8dat11 |= 1<<(pOutput[7]-6);
    }
    #endif
    
    if(u8power)
    {
        u8dat9 |= 1;
    }
    
    TM1623_Write_Data(0,REG_GRID1_SEG1_8,u8dat1);
    TM1623_Write_Data(0,REG_GRID2_SEG1_8,u8dat2);
    TM1623_Write_Data(0,REG_GRID3_SEG1_8,u8dat3);
    TM1623_Write_Data(0,REG_GRID4_SEG1_8,u8dat4);
    TM1623_Write_Data(0,REG_GRID5_SEG1_8,u8dat5);
    TM1623_Write_Data(0,REG_GRID6_SEG1_8,u8dat6);
    TM1623_Write_Data(0,REG_GRID7_SEG1_8,u8dat7);
    //delay_nms(1);
    #if 0//(TM1623_INSTANCES>1)
    TM1623_Write_Data(1,REG_GRID1_SEG1_8,u8dat8);
    TM1623_Write_Data(1,REG_GRID2_SEG1_8,u8dat9);
    #else
    TM1623_Write_Data(0,REG_GRID1_SEG9_14,u8dat9);
    TM1623_Write_Data(0,REG_GRID5_SEG9_14,u8dat8);
    TM1623_Write_Data(0,REG_GRID6_SEG9_14,u8dat10);
    TM1623_Write_Data(0,REG_GRID7_SEG9_14,u8dat11);
    #endif
    
#endif
}

extern BYTE KeyInput[8];

extern BYTE get_standby_flag(void);
#define LED_UPDATE_PERIOD    (1000L)
void LED_Display(void) 
{
    if(!SiiPlatformTimerExpired(etLED_UPDATE))
    {
        return; // time not enougth
    }
    
    //if(Get_Tm1623ValueChangeFlag())
    {
        //if(get_standby_flag()==1)
        //if(appPowerMgrPowerStateGet()!=pwrON)
        if(appPowerMgrPowerStateGet()==pwrSTANDBY)
        {
            BYTE CloseLed[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
            InOutPortLed_Process(CloseLed,0);
        }
        else
        {
            //if(Get_Tm1623ValueChangeFlag())
            {
                //Get_LastInPortSelect(KeyInput);
                InOutPortLed_Process(KeyInput,1);
            }
            Set_Tm1623ValueChangeFlag(false);
        }
        //Set_Tm1623ValueChangeFlag(false);
    }
    SiiPlatformTimerSet(etLED_UPDATE,mS_TO_TICKS(LED_UPDATE_PERIOD),0xFF); // use systick: 1ms per tick
}
#endif

