
//file: dvM21452.h
#ifndef _DVM21452_H_
#define _DVM21452_H_


#include "stm32f10x.h"
#include "stdarg.h"

BYTE dvM21452Init(void);

eRESULT  dvM21452Write( BYTE  cInstance, uint8_t addr, uint8_t value );

uint8_t dvM21452Read(BYTE  cInstance,  uint8_t addr );

//cInput: ����˿ں�,Begin from 0
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void dvM21452SwitcherClosePort( BYTE cPortNumber,PBYTE pcOutPortList);
void dvM21452SwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList);
void dvM21452PowerSet(BOOL bPowerOn);


#endif

