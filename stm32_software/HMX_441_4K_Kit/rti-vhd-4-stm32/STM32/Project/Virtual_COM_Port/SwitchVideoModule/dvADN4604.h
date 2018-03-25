
//file: dvADN4604.h
//[kyq Begin: 20150727 18:45:57]
#ifndef _DVADN4604_H_
#define _DVADN4604_H_
#include "stm32f10x.h"
#include "stdarg.h"




void dvADN4604Init(void);



eRESULT  dvADN4604Write( BYTE  cInstance, uint8_t addr, uint8_t value );

uint8_t dvADN4604Read(BYTE  cInstance,  uint8_t addr );

//cInput: 输入端口号,Begin from 0
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void dvADN4604SwitcherClosePort( BYTE cPortNumber,PBYTE pcOutPortList);
void dvADN4604SwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList);
void dvADN4604PowerSet(BOOL bPowerOn);


#endif

