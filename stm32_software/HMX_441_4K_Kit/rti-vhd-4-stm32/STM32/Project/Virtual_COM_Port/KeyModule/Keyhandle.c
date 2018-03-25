
#include "keyhandle.h"
#include "dvTM1623.h"
#include "si_datatypes.h"
#include "KeyTasks.h"
#include "si_timer.h"
#include "appPowerMgrAPI.h"
#include "uart.h"

#if ((MACHINETYPE==MUH4K862)||(MACHINETYPE==MHUB4K431))
#define DEFAULT_UI_ENTER mS_TO_TICKS(3000L)

typedef enum _eTM1623_KEY
{
    K1_KS1 = 0x01,
    K1_KS2 = 0x02,
    K1_KS3 = 0x04,
    K1_KS4 = 0x08,
    K1_KS5 = 0x10,
    K1_KS6 = 0x20,
    K1_KS7 = 0x40,
    K1_KS8 = 0x80,
}eTM1623_KEY;

typedef struct _stTM1623_KeyVlue
{
    BYTE KeyGroupNum;
    BYTE KeyGroupValue;
    eTM1623_KEY msKey;
}stTM1623_KeyVlue;

#if (MACHINETYPE==MHUB4K431)
stTM1623_KeyVlue TmKeyValue[4]={
 /*GroupNum  Value*/
    {1, 0x08,   K1_KS1},
    {1, 0x01,   K1_KS2},
    {0, 0x08,   K1_KS3},
    {0, 0x01,   K1_KS4},
    //{2, 0x01,   K1_KS5},
    //{2, 0x08,   K1_KS6},
    //{3, 0x01,   K1_KS7},
    //{3, 0x08,   K1_KS8},
};
#else
stTM1623_KeyVlue TmKeyValue[8]={
 /*GroupNum  Value*/
    {0, 0x01,   K1_KS1},
    {0, 0x08,   K1_KS2},
    {1, 0x01,   K1_KS3},
    {1, 0x08,   K1_KS4},
    {2, 0x01,   K1_KS5},
    {2, 0x08,   K1_KS6},
    {3, 0x01,   K1_KS7},
    {3, 0x08,   K1_KS8},
};
#endif
#else
enum _TM1623_KEY_t
{
    	K1_KS1=0x01,
	K2_KS1=0x02,
	K3_KS1=0x04,
       K1_KS2=0x08,
	K2_KS2=0x10,
	K3_KS2=0x20,
	
    	K1_KS3=0x01,
	K2_KS3=0x02,
	K3_KS3=0x04,
        K1_KS4=0x08,
	K2_KS4=0x10,
	K3_KS4=0x20,

        K1_KS5=0x01,
	K2_KS5=0x02,
	K3_KS5=0x04,
        K1_KS6=0x08,
	K2_KS6=0x10,
	K3_KS6=0x20,

	K1_KS7=0x01,
	K2_KS7=0x02,
	K3_KS7=0x04,
       K1_KS8=0x08,
	K2_KS8=0x10,
	K3_KS8=0x20,
	
	K1_KS9=0x01,
	K2_KS9=0x02,
	K3_KS9=0x04,
       K1_KS10=0x08,
	K2_KS10=0x10,
	K3_KS10=0x20,
		
} TM1623_KEY_t;

#endif
/************************************

B0   B1   B2      B3   B4   B5   B6   B7
K1   K2   K3  |  K1    K2   K3   X     X
      KS1        |         KS2         0      0   BYTE1
KS3 KS4 0 0 BYTE2
KS5 KS6 0 0 BYTE3
KS7 KS8 0 0 BYTE4
KS9 KS10 0 0 BYTE5

*************************************/
#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
extern BYTE KeyInput[8];
#endif
extern BOOL System_UnLock_flag;
extern BYTE   u8IChannellock[MAX_OUT];
extern int Uart_printf(const char * fmt,...);
extern bool_t GetStartupFlag(void);
unsigned char Key_Scan(void)
{
  	unsigned char  key = 0xff,i ,j ;
   	unsigned char  double_flag=0x00,InOut1_press=0;
   	unsigned char  KeyRegValue[5]={0} ;
    #if ((MACHINETYPE==MUH4K862)||(MACHINETYPE==MHUB4K431))
    unsigned char  ukeyvalue = 0;
    static unsigned char suKeypad = 0xff;
    #endif

	TM1623_Read_Key(0,KeyRegValue);			//读按键值		
	
	//for(i=0;i<5;i++)	
	//   UART1_SendByte(KeyRegValue[i]); 

#if ((MACHINETYPE!=MUH4K862)&&(MACHINETYPE!=MHUB4K431))
   	if((KeyRegValue[0] == 0)&&(KeyRegValue[1] == 0)&&(KeyRegValue[2] == 0)&&(KeyRegValue[3] == 0)&&(KeyRegValue[4] == 0))  return (KEYPAD_NO_KEY);
#endif
#if ((MACHINETYPE==MUH44E_BYE))
   	if(KeyRegValue[0] != 0)
   	{
   	 	for(i=0;i<6;i++)	   
   	 	{
			if(i<2)		//edid,all键编号为1，2
			{
				if(KeyRegValue[0]&BIT(i))
	   	   		{
	   	   			key = i+1 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else if((i>2)&&(i<5))		//输出按键的2,3的编号4，3
			{
				if(KeyRegValue[0]&BIT(i))
	   	   		{
	   	   			key = i+0 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
   	 	} 
   	} 

	if(KeyRegValue[1] != 0)
   	{
   	 	for(i=0;i<6;i++)	   
   	 	{
			if(i==0)		//输出按键4的编号为5
			{
				if(KeyRegValue[1]&BIT(i))
	   	   		{
	   	   			key = i+5 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else	 if(i==3)		//输出按键1的编号为6
			{
				if(KeyRegValue[1]&BIT(i))
	   	   		{
	   	   			key = i+3 ;
					InOut1_press |= 6;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
   	 	} 
   	} 

	if(KeyRegValue[2] != 0)
   	{
   	 	for(i=0;i<6;i++)	   
   	 	{
			if(i==0)		//enter按键的编号为7
			{
				if(KeyRegValue[2]&BIT(i))
	   	   		{
	   	   			key = i+7 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else	 if(i==3)		//clear按键的编号为8
			{
				if(KeyRegValue[2]&BIT(i))
	   	   		{
	   	   			key = i+5 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
   	 	} 
   	} 

	if(KeyRegValue[3] != 0)
   	{
   	 	for(i=0;i<6;i++)	   
   	 	{
			if(i==0)		//输入按键1的编号为9
			{
				if(KeyRegValue[3]&BIT(i))
	   	   		{
	   	   			key = i+9 ;
					InOut1_press |= 9;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else	 if(i==3)		//输入按键2的编号为10
			{
				if(KeyRegValue[3]&BIT(i))
	   	   		{
	   	   			key = i+7 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
   	 	} 
   	} 

	if(KeyRegValue[4] != 0)
   	{
   	 	for(i=0;i<6;i++)	   
   	 	{
			if(i==0)		//输入按键3的编号为11
			{
				if(KeyRegValue[4]&BIT(i))
	   	   		{
	   	   			key = i+11 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else	 if(i==3)		//输入按键4的编号为12
			{
				if(KeyRegValue[4]&BIT(i))
	   	   		{
	   	   			key = i+9 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
   	 	} 
   	}

#elif((MACHINETYPE==MUH88E_BYE))
   for(i=0;i<5;i++)
 	if(KeyRegValue[i] != 0)
   	{
   	 	for(j=0;j<6;j++)	   
   	 	{
/***********************
B0   B1   B2      B3   B4   B5   B6   B7
K1   K2   K3  |  K1   K2    K3 |  X     X
      KS1        |         KS2      | 0      0   BYTE1

******************/
		 if(i==0)
		    switch(KeyRegValue[i]&BIT(j))
		    	{
				 case K1_KS1 :  key =keIN1 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				  case K2_KS1 :  key = keOUT1 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS1:  key = KEYPAD_NO_KEY ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K1_KS2:  key =keIN2 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K2_KS2:  key = keOUT2 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS2:  key = KEYPAD_NO_KEY;
				if(double_flag<0xff)
						double_flag++;  
				break; 		

			}
/***********************
B0   B1   B2      B3   B4   B5   B6   B7
K1   K2   K3  |  K1   K2    K3 |  X     X
      KS3        |         KS4      | 0      0   BYTE2

******************/
		else if(i==1)
			switch(KeyRegValue[i]&BIT(j))
		    	{
				 case K1_KS3 :  key =keIN3;
				if(double_flag<0xff)
						double_flag++;  
				break;
				  case K2_KS3 :  key =keOUT3;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS3:  key = KEYPAD_NO_KEY ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K1_KS4:  key = keIN4 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K2_KS4:  key = keOUT4 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS4:  key = KEYPAD_NO_KEY ;
				if(double_flag<0xff)
						double_flag++;  
				break; 		

			}
/***********************
B0   B1   B2      B3   B4   B5   B6   B7
K1   K2   K3  |  K1   K2    K3 |  X     X
      KS5        |         KS6      | 0      0   BYTE3

******************/
		else if(i==2)
			switch(KeyRegValue[i]&BIT(j))
		    	{
				 case K1_KS5 :  key =keIN5;
				if(double_flag<0xff)
						double_flag++;  
				break;
				  case K2_KS5 :  key = keOUT5 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS5:  key =KEYPAD_NO_KEY ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K1_KS6:  key = keIN6 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K2_KS6:  key =keOUT6 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS6:  key =KEYPAD_NO_KEY ;
				if(double_flag<0xff)
						double_flag++;  
				break; 		

			}
/***********************
B0   B1   B2      B3   B4   B5   B6   B7
K1   K2   K3  |  K1   K2    K3 |  X     X
      KS7        |         KS8      | 0      0   BYTE4

******************/
		else if(i==3)
			switch(KeyRegValue[i]&BIT(j))
		    	{
				 case K1_KS7 :  key = keIN7 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				  case K2_KS7 :  key = keOUT7 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS7:  key = KEYPAD_NO_KEY;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K1_KS8:  key = keIN8 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K2_KS8:  key =keOUT8 ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS8:  key = KEYPAD_NO_KEY ;
				if(double_flag<0xff)
						double_flag++;  
				break; 		

			} 
/***********************
B0   B1   B2      B3   B4   B5   B6   B7
K1   K2   K3  |  K1   K2    K3 |  X     X
      KS9        |         KS10    | 0      0   BYTE5

******************/
		else if(i==4)
			switch(KeyRegValue[i]&BIT(j))
		    	{
				 case K1_KS9 :  key =KEYPAD_NO_KEY ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				  case K2_KS9 :  key =KEYPAD_NO_KEY;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS9:  key =KEYPAD_NO_KEY;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K1_KS10:  key =KEYPAD_NO_KEY;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K2_KS10:  key =KEYPAD_NO_KEY ;
				if(double_flag<0xff)
						double_flag++;  
				break;
				case K3_KS10:  key =KEYPAD_NO_KEY;
				if(double_flag<0xff)
						double_flag++;  
				break; 		

			}
   	 	} 
   	}
#elif ((MACHINETYPE==MUH4K862)||(MACHINETYPE==MHUB4K431))
    if(GetStartupFlag()==FALSE) // 开机初始化未完成，按键不能控制
    {
        //suKeypad = 0xFF;
        return (KEYPAD_NO_KEY);
    }
    for(i=0;i<5;i++)
    {
        if(KeyRegValue[i]!=0)
        {
            //Uart_printf("KeyRegValue[%d]==0x%02x\n",i,KeyRegValue[i]);
            //key = ke1TOALL;
            for(j=0;j<MAX_OUT;j++)
            {
                if((TmKeyValue[j].KeyGroupNum==i)&&(TmKeyValue[j].KeyGroupValue==KeyRegValue[i]))
                {
                    #if 0
                    KeyInput[i]=KeyInput[i]+1;
                    if(KeyInput[i]>=8)
                        KeyInput[i]=0;
	   	   			key = ke1TO1+(j*8)+KeyInput[i];
                    #endif
                    ukeyvalue |= TmKeyValue[j].msKey;
					if(double_flag<0xff)
	   	   				double_flag++;  
                }
            }
        }
    }
   	if((KeyRegValue[0] == 0)&&(KeyRegValue[1] == 0)&&(KeyRegValue[2] == 0)&&(KeyRegValue[3] == 0)&&(KeyRegValue[4] == 0))
    {
        key= KEYPAD_NO_KEY;
        suKeypad = 0xFF;
        return (KEYPAD_NO_KEY);
    }
    if(System_UnLock_flag)
    {
        suKeypad = 0xFF;
        return (KEYPAD_NO_KEY);
    }
    if(appPowerMgrPowerStateGet()==pwrSOFTOFF||appPowerMgrPowerStateGet()==pwrOFF)
    {
        suKeypad = 0xFF;
        return (KEYPAD_NO_KEY);
    }
    
    else if(appPowerMgrPowerStateGet()==pwrSTANDBY)
    {
        suKeypad = 0xFF;
        return (keENTER);
    }

    if(ukeyvalue>0)
    {
        switch(ukeyvalue)
        {
            case K1_KS4:
                {
                    if((suKeypad == K1_KS4)||(suKeypad == KEYPAD_REPEAT_KEY))
                    {
                        suKeypad = KEYPAD_REPEAT_KEY;
                    }
                    else
                    {
                        suKeypad = K1_KS4;
                        if(Get_CloseOutputFlag(0)==true)
                        {
                            KeyInput[0] = GetKeyLedCloseStatus(0);
                            Set_CloseOutputFlag(0,false);
                        }
                        else
                        {
                            KeyInput[0]=KeyInput[0]+1;
                            if(KeyInput[0]>=MAX_IN)
                                KeyInput[0]=0;
                            #if 1
                            for(i=0;i<MAX_IN;i++)
                            {
                                if(u8IChannellock[KeyInput[0]]&BIT(0))
                                {
                                    sprintf(USART1_tx_buff,"This input is locked！\n");
                                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                    KeyInput[0]=KeyInput[0]+1;
                                    if(KeyInput[0]>=MAX_IN)
                                        KeyInput[0]=0;
                                }
                                else
                                    break;
                            }
                            #endif
                        }
                        key = ke1DelayTO1+(KeyInput[0]*8);
                        SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01);    //expect only one
                        Set_Tm1623ValueChangeFlag(true);
                        InOutPortLed_Process(KeyInput,1);
                    }
                }
                break;
            case K1_KS3:
                {
                    if((suKeypad == K1_KS3)||(suKeypad == KEYPAD_REPEAT_KEY))
                    {
                        suKeypad = KEYPAD_REPEAT_KEY;
                    }
                    else
                    {
                        suKeypad = K1_KS3;
                        
                        if(Get_CloseOutputFlag(1)==true)
                        {
                            KeyInput[1] = GetKeyLedCloseStatus(1);
                            Set_CloseOutputFlag(1,false);
                        }
                        else
                        {
                            KeyInput[1]=KeyInput[1]+1;
                            if(KeyInput[1]>=MAX_IN)
                                KeyInput[1]=0;
                            #if 1
                            for(i=0;i<MAX_IN;i++)
                            {
                                if(u8IChannellock[KeyInput[1]]&BIT(1))
                                {
                                    sprintf(USART1_tx_buff,"This input is locked！\n");
                                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                    KeyInput[1]=KeyInput[1]+1;
                                    if(KeyInput[1]>=MAX_IN)
                                        KeyInput[1]=0;
                                }
                                else
                                    break;
                            }
                            #endif
                        }
                        key = ke1DelayTO2+(KeyInput[1]*8);
                        SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01);    //expect only one
                        Set_Tm1623ValueChangeFlag(true);
                        InOutPortLed_Process(KeyInput,1);
                    }
                }
                break;
            case K1_KS2:
                {
                    if((suKeypad == K1_KS2)||(suKeypad == KEYPAD_REPEAT_KEY))
                    {
                        suKeypad = KEYPAD_REPEAT_KEY;
                    }
                    else
                    {
                        suKeypad = K1_KS2;
                        
                        if(Get_CloseOutputFlag(2)==true)
                        {
                            KeyInput[2] = GetKeyLedCloseStatus(2);
                            Set_CloseOutputFlag(2,false);
                        }
                        else
                        {
                            KeyInput[2]=KeyInput[2]+1;
                            if(KeyInput[2]>=MAX_IN)
                                KeyInput[2]=0;
                            #if 1
                            for(i=0;i<MAX_IN;i++)
                            {
                                if(u8IChannellock[KeyInput[2]]&BIT(2))
                                {
                                    sprintf(USART1_tx_buff,"This input is locked！\n");
                                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                    KeyInput[2]=KeyInput[2]+1;
                                    if(KeyInput[2]>=MAX_IN)
                                        KeyInput[2]=0;
                                }
                                else
                                    break;
                            }
                            #endif
                        }
                        key = ke1DelayTO3+(KeyInput[2]*8);
                        SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01);    //expect only one
                        Set_Tm1623ValueChangeFlag(true);
                        InOutPortLed_Process(KeyInput,1);
                    }
                }
                break;
            case K1_KS1:
                {
                    if((suKeypad == K1_KS1)||(suKeypad == KEYPAD_REPEAT_KEY))
                    {
                        suKeypad = KEYPAD_REPEAT_KEY;
                    }
                    else
                    {
                        suKeypad = K1_KS1;
                        if(Get_CloseOutputFlag(3)==true)
                        {
                            KeyInput[3] = GetKeyLedCloseStatus(3);
                            Set_CloseOutputFlag(3,false);
                        }
                        else
                        {
                            KeyInput[3]=KeyInput[3]+1;
                            if(KeyInput[3]>=MAX_IN)
                                KeyInput[3]=0;
                            #if 1
                            for(i=0;i<MAX_IN;i++)
                            {
                                if(u8IChannellock[KeyInput[3]]&BIT(3))
                                {
                                    sprintf(USART1_tx_buff,"This input is locked！\n");
                                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                    KeyInput[3]=KeyInput[3]+1;
                                    if(KeyInput[3]>=MAX_IN)
                                        KeyInput[3]=0;
                                }
                                else
                                    break;
                            }
                            #endif
                        }
                        key = ke1DelayTO4+(KeyInput[3]*8);
                        SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01);    //expect only one
                        Set_Tm1623ValueChangeFlag(true);
                        InOutPortLed_Process(KeyInput,1);
                    }
                }
                break;
            case K1_KS5:
                {
                    if((suKeypad == K1_KS5)||(suKeypad == KEYPAD_REPEAT_KEY))
                    {
                        suKeypad = KEYPAD_REPEAT_KEY;
                    }
                    else
                    {
                        suKeypad = K1_KS5;
                        
                        if(Get_CloseOutputFlag(4)==true)
                        {
                            KeyInput[4] = GetKeyLedCloseStatus(4);
                            Set_CloseOutputFlag(4,false);
                        }
                        else
                        {
                            KeyInput[4]=KeyInput[4]+1;
                            if(KeyInput[4]>=MAX_IN)
                                KeyInput[4]=0;
                            #if 1
                            for(i=0;i<MAX_IN;i++)
                            {
                                if(u8IChannellock[KeyInput[4]]&BIT(4))
                                {
                                    sprintf(USART1_tx_buff,"This input is locked！\n");
                                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                    KeyInput[4]=KeyInput[4]+1;
                                    if(KeyInput[4]>=MAX_IN)
                                        KeyInput[4]=0;
                                }
                                else
                                    break;
                            }
                            #endif
                        }
                        key = ke1DelayTO5+(KeyInput[4]*8);
                        SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01);    //expect only one
                        Set_Tm1623ValueChangeFlag(true);
                        InOutPortLed_Process(KeyInput,1);
                    }
                }
                break;
            case K1_KS6:
                {
                    if((suKeypad == K1_KS6)||(suKeypad == KEYPAD_REPEAT_KEY))
                    {
                        suKeypad = KEYPAD_REPEAT_KEY;
                    }
                    else
                    {
                        suKeypad = K1_KS6;
                        
                        if(Get_CloseOutputFlag(5)==true)
                        {
                            KeyInput[5] = GetKeyLedCloseStatus(5);
                            Set_CloseOutputFlag(5,false);
                        }
                        else
                        {
                            KeyInput[5]=KeyInput[5]+1;
                            if(KeyInput[5]>=MAX_IN)
                                KeyInput[5]=0;
                            #if 1
                            for(i=0;i<MAX_IN;i++)
                            {
                                if(u8IChannellock[KeyInput[5]]&BIT(5))
                                {
                                    sprintf(USART1_tx_buff,"This input is locked！\n");
                                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                    KeyInput[5]=KeyInput[5]+1;
                                    if(KeyInput[5]>=MAX_IN)
                                        KeyInput[5]=0;
                                }
                                else
                                    break;
                            }
                            #endif
                        }
                        key = ke1DelayTO6+(KeyInput[5]*8);
                        SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01);    //expect only one
                        Set_Tm1623ValueChangeFlag(true);
                        InOutPortLed_Process(KeyInput,1);
                    }
                }
                break;
            case K1_KS7:
                {
                    if((suKeypad == K1_KS7)||(suKeypad == KEYPAD_REPEAT_KEY))
                    {
                        suKeypad = KEYPAD_REPEAT_KEY;
                    }
                    else
                    {
                        suKeypad = K1_KS7;
                        
                        if(Get_CloseOutputFlag(6)==true)
                        {
                            KeyInput[6] = GetKeyLedCloseStatus(6);
                            Set_CloseOutputFlag(6,false);
                        }
                        else
                        {
                            KeyInput[6]=KeyInput[6]+1;
                            if(KeyInput[6]>=MAX_IN)
                                KeyInput[6]=0;
                            #if 1
                            for(i=0;i<MAX_IN;i++)
                            {
                                if(u8IChannellock[KeyInput[6]]&BIT(6))
                                {
                                    sprintf(USART1_tx_buff,"This input is locked！\n");
                                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                    KeyInput[6]=KeyInput[6]+1;
                                    if(KeyInput[6]>=MAX_IN)
                                        KeyInput[6]=0;
                                }
                                else
                                    break;
                            }
                            #endif
                        }
                        key = ke1DelayTO7+(KeyInput[6]*8);
                        SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01);    //expect only one
                        Set_Tm1623ValueChangeFlag(true);
                        InOutPortLed_Process(KeyInput,1);
                    }
                }
                break;
            case K1_KS8:
                {
                    if((suKeypad == K1_KS8)||(suKeypad == KEYPAD_REPEAT_KEY))
                    {
                        suKeypad = KEYPAD_REPEAT_KEY;
                    }
                    else
                    {
                        suKeypad = K1_KS8;
                        
                        if(Get_CloseOutputFlag(7)==true)
                        {
                            KeyInput[7] = GetKeyLedCloseStatus(7);
                            Set_CloseOutputFlag(7,false);
                        }
                        else
                        {
                            KeyInput[7]=KeyInput[7]+1;
                            if(KeyInput[7]>=MAX_IN)
                                KeyInput[7]=0;
                            #if 1
                            for(i=0;i<MAX_IN;i++)
                            {
                                if(u8IChannellock[KeyInput[7]]&BIT(7))
                                {
                                    sprintf(USART1_tx_buff,"This input is locked！\n");
                                    UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                                    KeyInput[7]=KeyInput[7]+1;
                                    if(KeyInput[7]>=MAX_IN)
                                        KeyInput[7]=0;
                                }
                                else
                                    break;
                            }
                            #endif
                        }
                        key = ke1DelayTO8+(KeyInput[7]*8);
                        SiiPlatformTimerSet(etUI_ENTER, DEFAULT_UI_ENTER,0x01);    //expect only one
                        Set_Tm1623ValueChangeFlag(true);
                        InOutPortLed_Process(KeyInput,1);
                    }
                }
                break;
            default:
                suKeypad = 0xFF;
                key = KEYPAD_NO_KEY;
                break;
        }
    }
#endif
   		
   	if(double_flag==1)
	{
        //Uart_printf("key==%d\n",key);
		return (key);
   	}
	else if(double_flag==2)		//输入1和输出1组合按下时的键值
	{
		if(InOut1_press==0x0f)
		{
			key = InOut1_press;		
		}
        //Uart_printf("key2==%d\n",key);
		return (key);
   	}
   	else      {return (MULTI_PRESSED);}
}

