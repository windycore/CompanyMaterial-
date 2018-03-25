
#ifndef _LEDHANDLE_H_
#define _LEDHANDLE_H_
#include "si_datatypes.h"
#include "dvTM1623.h"
typedef enum
{
  elnIN1,
  elnIN2,
  elnIN3,
  elnIN4,
    elnIN5,
  elnIN6,
  elnIN7,
  elnIN8,

  elnCLEAR,
  elnENTER,
  elnEDID, 
   
  elnOUT1,
  elnOUT2,
  elnOUT3,
  elnOUT4,  
    elnOUT5,
  elnOUT6,
  elnOUT7,
  elnOUT8,
  
  elnALL,


  elnRED,
  elnGREEN,
  
  eLedInvalid,
}eLED_NAME;  //最大容纳32 盏灯

void dvLedOnSet(DWORD wLedGPIO);
void dvLedOffSet(DWORD wLedGPIO);
// 得到哪些灯亮,除了电源指示灯
DWORD dvLedOnGet(void);
void dvLedToggleSet(DWORD wLedGPIO);
void dvLedAllOnSet(DWORD wLedGPIO);
void dvLedAllOffSet(DWORD wLedGPIO);
bool_t LedInitialize(void);

#endif