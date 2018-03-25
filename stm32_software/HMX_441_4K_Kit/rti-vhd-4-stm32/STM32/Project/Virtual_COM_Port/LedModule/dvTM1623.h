#ifndef _DVTM1623_H_
#define _DVTM1623_H_
#include "si_datatypes.h"
#include "MainApp.h"

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

////////////////////////////////////////////////////
//Register define
#define REG_GRID1_SEG1_8            0xC0    // GRID1  SEG1~SEG8
#define REG_GRID1_SEG9_14           0xC1    // GRID1  SEG9~SEG14
#define REG_GRID2_SEG1_8            0xC2    // GRID2  SEG1~SEG8
#define REG_GRID2_SEG9_14           0xC3    // GRID2  SEG9~SEG14
#define REG_GRID3_SEG1_8            0xC4    // GRID3  SEG1~SEG8
#define REG_GRID3_SEG9_14           0xC5    // GRID3  SEG9~SEG14
#define REG_GRID4_SEG1_8            0xC6    // GRID4  SEG1~SEG8
#define REG_GRID4_SEG9_14           0xC7    // GRID4  SEG9~SEG14
#define REG_GRID5_SEG1_8            0xC8    // GRID5  SEG1~SEG8
#define REG_GRID5_SEG9_14           0xC9    // GRID5  SEG9~SEG14
#define REG_GRID6_SEG1_8            0xCA    // GRID6  SEG1~SEG8
#define REG_GRID6_SEG9_14           0xCB    // GRID6  SEG9~SEG14
#define REG_GRID7_SEG1_8            0xCC    // GRID7  SEG1~SEG8
#define REG_GRID7_SEG9_14           0xCD    // GRID7  SEG9~SEG14

#define REG_WRITE_CMD               0x44    // Set display cmd
#define REG_READ_CMD                0x42    // Set read key value cmd


#if ((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431))
#define TM1623_DIO_HIGH  	   GPIOC->BSRR = GPIO_Pin_7;delay_nus(5)
#define TM1623_DIO_LOW	  	   GPIOC->BRR = GPIO_Pin_7;delay_nus(5)
#define GET_TM1623_DIO		   (GPIOC->IDR  & GPIO_Pin_7)


#define TM1623_CLK_HIGH         GPIOC->BSRR = GPIO_Pin_8;delay_nus(5)
#define TM1623_CLK_LOW          GPIOC->BRR = GPIO_Pin_8;delay_nus(5)

#define TM1623_STB_HIGH         GPIOC->BSRR = GPIO_Pin_9;delay_nus(5)          
#define TM1623_STB_LOW          GPIOC->BRR = GPIO_Pin_9;delay_nus(5)
#elif((MACHINETYPE==MUH88E_BYE))
#define TM1623_DIO_HIGH  	   GPIOB->BSRR = GPIO_Pin_9;delay_nus(5)
#define TM1623_DIO_LOW	  	   GPIOB->BRR = GPIO_Pin_9;delay_nus(5)
#define GET_TM1623_DIO		     (GPIOB->IDR  & GPIO_Pin_9)


#define TM1623_CLK_HIGH         GPIOB->BSRR = GPIO_Pin_12;delay_nus(5)
#define TM1623_CLK_LOW          GPIOB->BRR = GPIO_Pin_12;delay_nus(5)

#define TM1623_STB_HIGH         GPIOB->BSRR = GPIO_Pin_13;delay_nus(5)          
#define TM1623_STB_LOW          GPIOB->BRR = GPIO_Pin_13;delay_nus(5)
#elif (MACHINETYPE==MUH4K862)
#define TM1623_DIO_HIGH  	   GPIOB->BSRR = GPIO_Pin_9;delay_nus(5)
#define TM1623_DIO_LOW	  	   GPIOB->BRR = GPIO_Pin_9;delay_nus(5)
#define GET_TM1623_DIO		     (GPIOB->IDR  & GPIO_Pin_9)


#define TM1623_CLK_HIGH         GPIOB->BSRR = GPIO_Pin_12;delay_nus(5)
#define TM1623_CLK_LOW          GPIOB->BRR = GPIO_Pin_12;delay_nus(5)

#define TM1623_STB_HIGH         GPIOB->BSRR = GPIO_Pin_13;delay_nus(5)          
#define TM1623_STB_LOW          GPIOB->BRR = GPIO_Pin_13;delay_nus(5)

#define TM1623_STB1_HIGH         GPIOC->BSRR = GPIO_Pin_7;delay_nus(5)          
#define TM1623_STB1_LOW          GPIOC->BRR = GPIO_Pin_7;delay_nus(5)

#endif

//#define TM1623_INSTANCES   1



void TM1623_Write_Byte(uint8_t byte);
uint8_t TM1623_Read_Byte(void);
void TM1623_Write_Cmd(BYTE cdevId,uint8_t cmd);
void TM1623_Read_Key(BYTE cdevId,uint8_t *key_value);
void TM1623_Write_Data(BYTE cdevId,uint8_t addr,uint8_t dat);
uint8_t TM1623_Init(uint8_t cInstance);
void wLEDRegGPIO( DWORD RegGPIO);
bool_t Get_Tm1623ValueChangeFlag(void);
void Set_Tm1623ValueChangeFlag(bool_t nflag);

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
void InOutPortLed_Process(PBYTE pOutput,BYTE u8power);
void LED_Display(void);
#endif

#endif

