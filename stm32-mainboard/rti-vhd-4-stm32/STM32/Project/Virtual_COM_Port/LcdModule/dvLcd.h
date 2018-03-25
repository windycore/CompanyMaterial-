//
// FILE: dvLcd.h
//
//
//
//
//
#ifndef _DVLCD_H_
#define _DVLCD_H_

#include "dv74ls138.h"

#if((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
//液晶定义
#define BL_ON	  	 	            GPIOA->BSRR = GPIO_Pin_4;delay_nus(10)     // Back Light on
#define BL_OFF			            GPIOA->BRR  = GPIO_Pin_4;delay_nus(10)

//LCD Enable:PC10一个高脉冲作为LCD使能，将并行数据口上的数据在这个高脉冲的到来  再真正的写进LCD里
#define LCD_Enable		          GPIOA->BSRR = GPIO_Pin_5 ;delay_nus(80);GPIOA->BRR = GPIO_Pin_5

// LCD RS Register: PD7
#define LCD_RS_DAT	   	        GPIOA->BSRR = GPIO_Pin_6;delay_nus(10)    // PA6 set 1: LCD数据寄存器
#define LCD_RS_CMD		          GPIOA->BRR  = GPIO_Pin_6;delay_nus(10)    // PA6 set 0: LCD指令寄存器

// FOR    lCD
#define  LCDHC595_DATA_HIGH  	  GPIOC->BSRR = GPIO_Pin_1;delay_nus(10)
#define  LCDHC595_DATA_LOW		  GPIOC->BRR  = GPIO_Pin_1;delay_nus(10)

// 74HC595 STORAGE CLOCK PULSE : PC5
#define  LCDSTCP_HIGH           GPIOC->BSRR = GPIO_Pin_3;delay_nus(10)
#define  LCDSTCP_LOW            GPIOC->BRR  = GPIO_Pin_3;delay_nus(10)

// 74HC595 SHIFT CLOCK PULSE: PC4
#define  LCDSHCP_HIGH           GPIOC->BSRR = GPIO_Pin_2;delay_nus(10)
#define  LCDSHCP_LOW            GPIOC->BRR  = GPIO_Pin_2;delay_nus(10)

#define  LCDHC595_CS_HIGH           dvSelectChilpEnable(eSil9396IN_EN);delay_nus(10)
#define  LCDHC595_CS_LOW            dvSelectChilpEnable(eLCD1602ID_1);delay_nus(10)

#else
//液晶定义
#define BL_ON	  	 	            GPIOA->BSRR = GPIO_Pin_4;delay_nus(10)     // Back Light on
#define BL_OFF			            GPIOA->BRR  = GPIO_Pin_4;delay_nus(10)

//LCD Enable:PC10一个高脉冲作为LCD使能，将并行数据口上的数据在这个高脉冲的到来  再真正的写进LCD里
#define LCD_Enable		          GPIOA->BSRR = GPIO_Pin_5 ;delay_nus(80);GPIOA->BRR = GPIO_Pin_5

// LCD RS Register: PD7
#define LCD_RS_DAT	   	        GPIOA->BSRR = GPIO_Pin_6;delay_nus(10)    // PA6 set 1: LCD数据寄存器
#define LCD_RS_CMD		          GPIOA->BRR  = GPIO_Pin_6;delay_nus(10)    // PA6 set 0: LCD指令寄存器


// FOR    lCD
#define  LCDHC595_DATA_HIGH  	  GPIOA->BSRR = GPIO_Pin_7;delay_nus(10)
#define  LCDHC595_DATA_LOW		  GPIOA->BRR  = GPIO_Pin_7;delay_nus(10)

// 74HC595 STORAGE CLOCK PULSE : PC5
#define  LCDSTCP_HIGH           GPIOC->BSRR = GPIO_Pin_4;delay_nus(10)
#define  LCDSTCP_LOW            GPIOC->BRR  = GPIO_Pin_4;delay_nus(10)

// 74HC595 SHIFT CLOCK PULSE: PC4
#define  LCDSHCP_HIGH           GPIOC->BSRR = GPIO_Pin_5;delay_nus(10)
#define  LCDSHCP_LOW            GPIOC->BRR  = GPIO_Pin_5;delay_nus(10)

#endif

void dvLCDInit(void);
void dvLCDLineDraw(BYTE  cLine,BYTE cPostion, PBYTE string);
//void dvLCDLineTXTDraw(BYTE  cLine,BYTE cPostion,...);
void dvLCD_On(BOOL bEn);



#endif


