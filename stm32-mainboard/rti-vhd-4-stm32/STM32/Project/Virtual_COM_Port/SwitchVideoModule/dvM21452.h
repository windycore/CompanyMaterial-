
//file: dvM21452.h
#ifndef _DVM21452_H_
#define _DVM21452_H_


#include "stm32f10x.h"
#include "stdarg.h"

BYTE dvM21452Init(void);

eRESULT  dvM21452Write( BYTE  cInstance, uint8_t addr, uint8_t value );

uint8_t dvM21452Read(BYTE  cInstance,  uint8_t addr );

//cInput: 输入端口号,Begin from 0
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void dvM21452SwitcherClosePort( BYTE cPortNumber,PBYTE pcOutPortList);
void dvM21452SwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList);
void dvM21452PowerSet(BOOL bPowerOn);


#endif

