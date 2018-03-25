//FILE: bsp_Gpio.h

#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__
#include "stm32f10x.h"

typedef struct
{
	GPIO_TypeDef	*GPIO_PORT;
	uint32_t		GPIO_CLK;
	uint32_t		GPIO_PIN;
	GPIOMode_TypeDef GPIO_MODE;  // KYQ
	BOOL              InitValue;
}BSP_GPIO_ConfigTypeDef;




typedef enum
{
/* 
  * CONFIGURATION: Add user input events here */
 
  egpioSii9533Reset,         // C13
  egpioSii9679Reset,            //C6
  egpioPCA9539_Reset,             // B10
  egpioFAN_PWRCTRL,               //C6
  egpioAP3012_PWRCTRL,		  //C6
  egpioPOE_CTRL1,                    //C0  // MUST keep order
  egpio4604_PWRCTRL,               //C14
  egpio5V_PWRCTRL,               //C15
  //egpioPOE_CTRL2,                    //C7
  //egpioPOE_CTRL3,                    //C8
  //egpioPOE_CTRL4,                    //C9
  egpioLED,                               //A15

  egpioANDReset,                    // PD2


   
   sgpINVALID,						           // used for enumerated type range
										   // checking (DO NOT REMOVE)
} eSTM32_GPIO;	


void BSP_Gpio_Init(void);
void BSP_Gpio_Low(WORD id);
void BSP_Gpio_High(WORD id);
void BSP_Gpio_Toggle(WORD id);

BOOL BSP_Gpio_GetBitValue(WORD id);
void BSP_GPIO_FanPower_EN(uint8_t en);
void BSP_GPIO_AP3012_EN(uint8_t en);
void BSP_GPIO_LED_Toggle(void);
void BSP_GPIO_4604_EN(uint8_t en);
void BSP_GPIO_5V_EN(uint8_t en);


#endif


