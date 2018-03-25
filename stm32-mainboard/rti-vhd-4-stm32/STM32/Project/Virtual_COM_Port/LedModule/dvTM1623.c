
#include "util.h"
#include "si_datatypes.h"
#include "stm32f10x.h"
#include "dvTM1623.h"
#include "LedHandle.h"
#include "si_logger.h"

void TM1623_key_led_Config_moni(void);

static BYTE m_wLedReg[7] ={ 0};

static const BYTE m_acBusID[TM1623_INSTANCES] = {

eTM1623ID_1      

};

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
			#if((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
			TM1623_DIN_HIGH;
			#else
			TM1623_DIO_HIGH;
			#endif
		}
		else
		{ 
			#if((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
			TM1623_DIN_LOW;
			#else
			TM1623_DIO_LOW;
			#endif
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
		TM1623_CLK_HIGH;							//在DOUT的下降沿读取数据
		TM1623_CLK_LOW;
		#if((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
		if(GET_TM1623_DOUT)
			temp|=0x80;
		#else
		if(GET_TM1623_DIO)
			temp|=0x80;
		#endif
		  
	}
   	return temp;  
}  
 
/*********************************************************************************************** 
*Function Name: Write_Cmd        
*Purpose      : Write a conmand to the data port 
*params       : cmd  -------8-bits byte,the conmand,check the data sheet to find the conmand  
*return       : none 
***********************************************************************************************/  
void TM1623_Write_Cmd(uint8_t cmd)
{  
	TM1623_STB_HIGH;  
    	TM1623_STB_LOW;  
    	TM1623_Write_Byte(cmd);  
}  
 
/*********************************************************************************************** 
*Function Name: Read_Key         
*Purpose      : Read the key number which has been pressed 
*params       : none 
*return       : the number of the key. 0-8.  "return 0" represents no key has been pressed. 
***********************************************************************************************/  
void TM1623_Read_Key(uint8_t *key_value)
{  
    uint8_t i=0;  
    TM1623_Write_Cmd(0x42);  
    #if((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    TM1623_DOUT_HIGH;                       //this is obligatory, check the data sheet,GPIO   
    #else
    TM1623_DIO_HIGH;                       //this is obligatory, check the data sheet,GPIO   
    #endif
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
void TM1623_Read_Reg(uint8_t *Regdat)
{  
    uint8_t i=0;  
    TM1623_Write_Cmd(0x42);  
    #if((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    TM1623_DOUT_HIGH;                       //this is obligatory, check the data sheet,GPIO   
    #else
    TM1623_DIO_HIGH;                       //this is obligatory, check the data sheet,GPIO   
    #endif
   *Regdat = TM1623_Read_Byte();

}  

/*********************************************************************************************** 
*Function Name: Write_Dat        
*Purpose      : Write data to the location specified 
*params       : addr  ------the address,0x00 to 0x0f 
        dat   ------the data,segment code 
*return       : none 
***********************************************************************************************/  
void TM1623_Write_Data(uint8_t addr,uint8_t dat)
{  
    TM1623_Write_Cmd(0x44);  
    TM1623_Write_Cmd(0xc0|addr);  
    TM1623_Write_Byte(dat);  
}  

unsigned char dvTM1623HardWarePrintf( uint8_t cInstance)
{    
      bool_t  error=0;
     BYTE acMessage2[USART1_TX_BUFF_MAX] = { 0 };
	  BYTE cCount = 0;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);  
#if INC_UART	 


if(psSystemSettingData->cPOSTEnable)
   switch(cInstance)
   	{
    case eTM1623ID_1:
          	 cCount += SiiSprintf((char*)&acMessage2[cCount],  TM1623ID1_BROAD_INFOR);
		 cCount += SiiSprintf((char*)&acMessage2[cCount],  "  ");
		 cCount += SiiSprintf((char*)&acMessage2[cCount],  TM1623ID1_Uxx_INFOR);
		break;
   case eTM1623ID_2:
          	 cCount += SiiSprintf((char*)&acMessage2[cCount],  TM1623ID2_BROAD_INFOR);
		 cCount += SiiSprintf((char*)&acMessage2[cCount],  "  ");
		 cCount += SiiSprintf((char*)&acMessage2[cCount],  TM1623ID2_Uxx_INFOR);
		break;
   default: break;

   }
   	 UART1_SendData2PC(acMessage2, cCount);	 
 #endif
   utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);   
}

unsigned char dvTM1623PwonSelfTest(void)
{      unsigned char success=0;
       unsigned char i;
   	unsigned char  KeyRegValue[5]={0} ;
	TM1623_Read_Key(KeyRegValue);			//读按键值		
	
	for(i=0;i<5;i++)	
	   UART1_SendByte(KeyRegValue[i]); 

   	if((KeyRegValue[0] == 0)&&(KeyRegValue[1] == 0)&&(KeyRegValue[2] == 0)&&(KeyRegValue[3] == 0)&&(KeyRegValue[4] == 0)) 
	success= 1;  // not error   
	else
	success= 0;	

	return success;	

}
static BYTE  AlreadyInitF[TM1623_INSTANCES]={0};
/*********************************************************************************************** 
*Function Name: TM1638_Init      
*Purpose      : the initialization of tm1638 
*params       : none 
*return       : none 
***********************************************************************************************/  
uint8_t TM1623_Init(void )
{  
    uint8_t i=0;  
    uint8_t cInstance;
    uint8_t error=0;  
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);

	for(cInstance=0;cInstance<TM1623_INSTANCES;cInstance++) 
	{
	  if(!AlreadyInitF[cInstance])  //olny module  initializated  TM1623_Init
	  {    AlreadyInitF[cInstance]=TRUE;//already 
	        dvSelectChilpEnable(m_acBusID[cInstance]);
	        TM1623_key_led_Config_moni();
	      /************************************************************/
		    TM1623_Write_Cmd(0x03); 			//设置为7位11段模式
		    TM1623_Write_Cmd(LEVEL_10);  		//调节亮度
		    for(i=0;i<14;i++)
		    {  
		        TM1623_Write_Data(i,0x00);
		    } 
/*************************Electricity self-inspection on print*******************/
		  if(!dvTM1623PwonSelfTest())
		  {
		  	 error=1;
			 dvTM1623HardWarePrintf( m_acBusID[cInstance]);
		  }
/*******************************************************************/
	  }        
	}
      utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);   
    return error;
}

#if((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
void wLEDRegGPIO( DWORD RegGPIO)
{
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
							m_wLedReg[3] &= ~ BIT(0);	
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
							m_wLedReg[3] |= BIT(0);	
			            			break;
			        case elnGREEN:
							m_wLedReg[2] |= BIT(0);
				      			break;

			        default:	break;
			}
		}
    	}
	
	TM1623_Write_Data(0x00,m_wLedReg[0]);
    	TM1623_Write_Data(0x02,m_wLedReg[1]);
	
    	TM1623_Write_Data(0x04,m_wLedReg[2]);
    	TM1623_Write_Data(0x06,m_wLedReg[3]);
	
    	TM1623_Write_Data(0x08,m_wLedReg[4]);
    	TM1623_Write_Data(0x0A,m_wLedReg[5]);
}

#else
void wLEDRegGPIO( DWORD RegGPIO)
{
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
	
	TM1623_Write_Data(0x00,m_wLedReg[0]);
    	TM1623_Write_Data(0x02,m_wLedReg[1]);
	
    	TM1623_Write_Data(0x04,m_wLedReg[2]);
    	TM1623_Write_Data(0x06,m_wLedReg[3]);
	
    	TM1623_Write_Data(0x08,m_wLedReg[4]);
    	TM1623_Write_Data(0x0A,m_wLedReg[5]);
}

#endif

