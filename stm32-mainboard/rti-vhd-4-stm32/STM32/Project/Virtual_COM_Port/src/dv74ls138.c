#include "stm32f10x.h"
#include "main.h"
#include "dv74ls138.h"
#include "appSystemMgrAPI.h"
#include "Util.h"


eChipID CurChipIdNum;

void  dvSelectChilpEnable(eChipID ChipIdNum )
{
	switch(ChipIdNum)
   	{
	    case eTM1623ID_1:
			_74HC138_A2_LOW;
			_74HC138_A1_LOW;
			_74HC138_A0_LOW;
			break;
	   case eLCD1602ID_1:
			_74HC138_A2_LOW;
			_74HC138_A1_LOW;
			_74HC138_A0_HIGH;
			break;
	   case eSil9396IN_EN:
			_74HC138_A2_LOW;
			_74HC138_A1_HIGH;
			_74HC138_A0_LOW;
			break;
	   case eSil9396OUT_EN:
			_74HC138_A2_HIGH;
			_74HC138_A1_LOW;
			_74HC138_A0_LOW;
			break;
	   #if(MACHINETYPE!=MUH44A_H2)
	   case eSil9396IN_EN2:
			_74HC138_A2_LOW;
			_74HC138_A1_HIGH;
			_74HC138_A0_HIGH;
			break;
	   case eSil9396OUT_EN2:
			_74HC138_A2_HIGH;
			_74HC138_A1_LOW;
			_74HC138_A0_HIGH;
			break;
	   #endif
	   default: break;

   	}
	CurChipIdNum = ChipIdNum;

}


eChipID  dvGetChilpEnableID(void)
{
	return CurChipIdNum;
}

