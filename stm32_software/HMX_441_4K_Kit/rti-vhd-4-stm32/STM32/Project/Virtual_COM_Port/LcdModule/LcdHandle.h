#ifndef _LEDHANDLE_H_
#define _LEDHANDLE_H_
#include "si_datatypes.h"
#include "dvLcd.h"

#define LINE_TXT_MAX   16

void dvLCDLineTXTDraw(BYTE  cLine,BYTE cPostion,...);
bool_t LcdInit(void);
#endif