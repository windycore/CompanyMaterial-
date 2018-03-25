#ifndef _DV74LS138H_
#define _DV74LS138H_

#include "stdarg.h"	
#include "mcu.h"



typedef enum
{
  eTM1623ID_1,
  eTM1623ID_2,	
  eCD4604ID_1,	
  eCD4604ID_2,	
  eCPLDID,
  eChipInvalid,
}eChipID; //×î´óµÄÐ¾Æ¬

void  dvSelectChilpEnable(eChipID ChipIdNum );

#endif
