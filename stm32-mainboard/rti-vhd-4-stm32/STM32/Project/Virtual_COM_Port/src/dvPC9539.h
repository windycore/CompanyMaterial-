//file dvPC9539.h
#ifndef _DVPC9539_H_
#define _DVPC9539_H_
#include "stm32f10x.h"
#include "LedHandle.h"
//command type 
#define inport0       0x00
#define inport1       0x01
#define outport0     0x02
#define outport1     0x03
#define config_inverseport0   0x04
#define config_inverseport1   0x05
//If a bit in these registers is set to 1, the corresponding port pin is enabled as a high-impedance input.
//If a bit in these registers is cleared to 0, the corresponding port pin is enabled as an output.
#define config_port0   0x06
#define config_port1   0x07

typedef enum
{
    epi9539_IN5VDET_OUTHPDDET,
#if(PCA9539_NUM_INSTANCES>1)
    epi9539_9679RST,
#endif

    epi9539_INVALID,
    
} ePCA9539_INSTANCE;

eRESULT  dvPC9539Write( BYTE cInstance, uint8_t rigster_add,WORD wData);
eRESULT  dvPC9539Read(BYTE cInstance,uint8_t rigster_add,uint8_t *pdata);

//uint8_t check_HPD(uint8_t port);
void  dvPC9539Init(void) ;
void Output5VConctrol(WORD OutputChannel  ,BYTE OnOff);//  1表示开   0表示低

#endif
