#include "dvVS010TX.h"
#include "BSP_I2C.h"
#include "bsp_Gpio.h"
#include "Util.h"
#define VS010_INSTANCE  1
void dvVS010TXInit(BYTE cInstance)
{
}
void dvVS010TXPowerSet(BYTE cInstance, BOOL bPowerON)
{
#if (MACHINETYPE==MUH44E_BYE)
if(!bPowerON)
BSP_Gpio_High(egpioPOE_CTRL1+cInstance);		//加了三极管，高电平断电
else
BSP_Gpio_Low(egpioPOE_CTRL1+cInstance);
#elif (MACHINETYPE==MUH88E_BYE)
if(!bPowerON)
   BSP_Gpio_Low(egpioPOE_CTRL1+cInstance);
else
  BSP_Gpio_High(egpioPOE_CTRL1+cInstance);	

#endif

	
}
void dvVS010TXPowerOn(uint8_t en)
{
   BYTE i;
   for(i = 0; i< VS010_INSTANCE; i++)
   	{
        dvVS010TXPowerSet(i,en);
	 delay_nms(200); // 500
   	}
}
