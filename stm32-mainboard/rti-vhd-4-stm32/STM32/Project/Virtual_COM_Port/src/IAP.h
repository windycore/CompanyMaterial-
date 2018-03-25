#ifndef __IAP_h
#define __IAP_h

#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
#include "stm32f10x.h"

#define  STM32F103RC   1
//#define	 STM32F103RB   2

#ifdef STM32F103RC
//IAP代码占用不到12K的空间
#define ApplicationAddress    0x8004000     //APP程序在FLASH的16K的位置开始存放
#define IAP_FLAG_Adderss      0x8003C00     //升级标志位可以放在15K的起始位置
#define FPGAAddress           0x0000000
#define PAGE_SIZE             (0x800)    /* 1 Kbyte */
#define STM32_FLASH_SIZE      (0x40000)  /* 256 KBytes */
#define APP_FLASH_SIZE        (0x3C000)  /* 240 KBytes */

#elif  STM32F103RB
//IAP代码占用不到12K的空间
#define ApplicationAddress    0x8004000     //APP程序在FLASH的16K的位置开始存放
#define IAP_FLAG_Adderss      0x8003C00     //升级标志位可以放在15K的起始位置
#define FPGAAddress           0x0000000
#define PAGE_SIZE             (0x800)    /* 1 Kbyte */
#define STM32_FLASH_SIZE      (0x40000)  /* 256 KBytes */
#define APP_FLASH_SIZE        (0x3C000)  /* 240 KBytes */

#endif
typedef  void (*pFunction)(void);

#define CROLBYTE(value, bits) ((value << bits) | (value >> (8 - bits)))  //循环左移
#define CRORBYTE(value, bits) ((value >> bits) | (value << (8 - bits)))  //循环右移

void delay_nus(uint16_t time);
void delay_nms(uint16_t count);

void process(void);
uint8_t  IAP_flag_read(void);

extern uint32_t FlashDestination;

#endif

