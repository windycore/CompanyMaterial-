#ifndef _DV74LS138H_
#define _DV74LS138H_

#include "stdarg.h"	
#include "mcu.h"


#define _74HC138_A0_HIGH        GPIOC->BSRR = GPIO_Pin_13;delay_nus(5)          
#define _74HC138_A0_LOW          GPIOC->BRR = GPIO_Pin_13;delay_nus(5)

#define _74HC138_A1_HIGH        GPIOC->BSRR = GPIO_Pin_14;delay_nus(5)          
#define _74HC138_A1_LOW          GPIOC->BRR = GPIO_Pin_14;delay_nus(5)

#define _74HC138_A2_HIGH        GPIOC->BSRR = GPIO_Pin_15;delay_nus(5)          
#define _74HC138_A2_LOW          GPIOC->BRR = GPIO_Pin_15;delay_nus(5)


typedef enum
{
  eTM1623ID_1,  
  eTM1623ID_2,	  
  eCD4604ID_1,	  
  eCD4604ID_2,	  
  eCPLDID,
  eLCD1602ID_1,	
  eSil9396IN_EN,
  eSil9396OUT_EN,
  #if(MACHINETYPE!=MUH44A_H2)
  eSil9396IN_EN2,
  eSil9396OUT_EN2,
  #endif
  eChipInvalid,
}eChipID; //×î´óµÄÐ¾Æ¬

void  dvSelectChilpEnable(eChipID ChipIdNum );
eChipID  dvGetChilpEnableID(void);

#endif
