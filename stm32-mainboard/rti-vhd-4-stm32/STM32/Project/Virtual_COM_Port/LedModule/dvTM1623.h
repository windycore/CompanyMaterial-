#ifndef _DVTM1623_H_
#define _DVTM1623_H_
#include "si_datatypes.h"
#include "MainApp.h"
#include "dv74ls138.h"

//显示亮度级别
#define LEVEL_OFF   0x80
#define LEVEL_1     0x88
#define LEVEL_2     0x89
#define LEVEL_4     0x8a
#define LEVEL_10    0x8b
#define LEVEL_11    0x8c
#define LEVEL_12    0x8d
#define LEVEL_13    0x8e
#define LEVEL_14    0x8f

#if(MACHINETYPE==MUH44E_BYE)
#define TM1623_DIO_HIGH  	   GPIOC->BSRR = GPIO_Pin_7;delay_nus(5)
#define TM1623_DIO_LOW	  	   GPIOC->BRR = GPIO_Pin_7;delay_nus(5)
#define GET_TM1623_DIO		     (GPIOC->IDR  & GPIO_Pin_7)


#define TM1623_CLK_HIGH         GPIOC->BSRR = GPIO_Pin_8;delay_nus(5)
#define TM1623_CLK_LOW          GPIOC->BRR = GPIO_Pin_8;delay_nus(5)

#define TM1623_STB_HIGH         GPIOC->BSRR = GPIO_Pin_9;delay_nus(5)          
#define TM1623_STB_LOW          GPIOC->BRR = GPIO_Pin_9;delay_nus(5)

#define TM1623ID1_BROAD_INFOR   "AUH88EACx"
#define TM1623ID1_Uxx_INFOR   "U1"

#define TM1623ID2_BROAD_INFOR   "AUH88EACx"
#define TM1623ID2_Uxx_INFOR   "U1"
#elif(MACHINETYPE==MUH88E_BYE)
#define TM1623_DIO_HIGH  	   GPIOB->BSRR = GPIO_Pin_9;delay_nus(5)
#define TM1623_DIO_LOW	  	   GPIOB->BRR = GPIO_Pin_9;delay_nus(5)
#define GET_TM1623_DIO		     (GPIOB->IDR  & GPIO_Pin_9)


#define TM1623_CLK_HIGH         GPIOB->BSRR = GPIO_Pin_12;delay_nus(5)
#define TM1623_CLK_LOW          GPIOB->BRR = GPIO_Pin_12;delay_nus(5)

#define TM1623_STB_HIGH         GPIOB->BSRR = GPIO_Pin_13;delay_nus(5)          
#define TM1623_STB_LOW          GPIOB->BRR = GPIO_Pin_13;delay_nus(5)


#define TM1623ID1_BROAD_INFOR   "AUH88EACx"
#define TM1623ID1_Uxx_INFOR   "U1"

#define TM1623ID2_BROAD_INFOR   "AUH88EACx"
#define TM1623ID2_Uxx_INFOR   "U1"
#elif((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))

#define TM1623_DIN_HIGH  	   GPIOC->BSRR = GPIO_Pin_6;delay_nus(5)
#define TM1623_DIN_LOW	  	   GPIOC->BRR = GPIO_Pin_6;delay_nus(5)

#define TM1623_DOUT_HIGH  	   GPIOC->BSRR = GPIO_Pin_8;delay_nus(5)
#define TM1623_DOUT_LOW	  	   GPIOC->BRR = GPIO_Pin_8;delay_nus(5)
#define GET_TM1623_DOUT		     (GPIOC->IDR  & GPIO_Pin_8)

#define TM1623_CLK_HIGH         GPIOC->BSRR = GPIO_Pin_7;delay_nus(5)
#define TM1623_CLK_LOW          GPIOC->BRR = GPIO_Pin_7;delay_nus(5)

#define TM1623_STB_HIGH         dvSelectChilpEnable(eSil9396IN_EN);delay_nus(5)          
#define TM1623_STB_LOW          dvSelectChilpEnable(eTM1623ID_1);delay_nus(5)


#define TM1623ID1_BROAD_INFOR   "AUH88EACx"
#define TM1623ID1_Uxx_INFOR   "U1"

#define TM1623ID2_BROAD_INFOR   "AUH88EACx"
#define TM1623ID2_Uxx_INFOR   "U1"
#endif

#define TM1623_INSTANCES   1


unsigned char dvTM1623PwonSelfTest(void);
void TM1623_Write_Byte(uint8_t byte);
uint8_t TM1623_Read_Byte(void);
void TM1623_Write_Cmd(uint8_t cmd);
void TM1623_Read_Key(uint8_t *key_value);
void TM1623_Write_Data(uint8_t addr,uint8_t dat);
uint8_t TM1623_Init(void);
void wLEDRegGPIO( DWORD RegGPIO);

#endif

