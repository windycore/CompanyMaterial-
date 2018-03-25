
#include "keyhandle.h"
#include "dvTM1623.h"
#include "si_datatypes.h"
#include "KeyTasks.h"
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
/************************************

B0   B1   B2      B3   B4   B5   B6   B7
K1   K2   K3  |  K1    K2   K3   X     X
      KS1        |         KS2         0      0   BYTE1
KS3 KS4 0 0 BYTE2
KS5 KS6 0 0 BYTE3
KS7 KS8 0 0 BYTE4
KS9 KS10 0 0 BYTE5



*************************************/

unsigned char Key_Scan(void)
{
  	unsigned char  key = 0xff,i ,j ;
   	unsigned char  double_flag=0x00,InOut1_press=0;
   	unsigned char  KeyRegValue[5]={0} ;

	TM1623_Read_Key(KeyRegValue);			//������ֵ		
	
	//for(i=0;i<5;i++)	
	//   UART1_SendByte(KeyRegValue[i]); 

   	if((KeyRegValue[0] == 0)&&(KeyRegValue[1] == 0)&&(KeyRegValue[2] == 0)&&(KeyRegValue[3] == 0)&&(KeyRegValue[4] == 0))  return (KEYPAD_NO_KEY);
#if(MACHINETYPE==MUH44E_BYE)
   	if(KeyRegValue[0] != 0)
   	{
   	 	for(i=0;i<6;i++)	   
   	 	{
			if(i<2)		//edid,all�����Ϊ1��2
			{
				if(KeyRegValue[0]&BIT(i))
	   	   		{
	   	   			key = i+1 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else if((i>2)&&(i<5))		//���������2,3�ı��4��3
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
			if(i==0)		//�������4�ı��Ϊ5
			{
				if(KeyRegValue[1]&BIT(i))
	   	   		{
	   	   			key = i+5 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else	 if(i==3)		//�������1�ı��Ϊ6
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
			if(i==0)		//enter�����ı��Ϊ7
			{
				if(KeyRegValue[2]&BIT(i))
	   	   		{
	   	   			key = i+7 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else	 if(i==3)		//clear�����ı��Ϊ8
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
			if(i==0)		//���밴��1�ı��Ϊ9
			{
				if(KeyRegValue[3]&BIT(i))
	   	   		{
	   	   			key = i+9 ;
					InOut1_press |= 9;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else	 if(i==3)		//���밴��2�ı��Ϊ10
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
			if(i==0)		//���밴��3�ı��Ϊ11
			{
				if(KeyRegValue[4]&BIT(i))
	   	   		{
	   	   			key = i+11 ;
					if(double_flag<0xff)
	   	   				double_flag++;  
	   	   		}
			}
			else	 if(i==3)		//���밴��4�ı��Ϊ12
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

#elif(MACHINETYPE==MUH88E_BYE)
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
#elif((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
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
#endif
   		
   	if(double_flag==1)
	{
		return (key);
   	}
	else if(double_flag==2)		//����1�����1��ϰ���ʱ�ļ�ֵ
	{
		if(InOut1_press==0x0f)
		{
			key = InOut1_press;		
		}
		return (key);
   	}
   	else      {return (MULTI_PRESSED);}
}


