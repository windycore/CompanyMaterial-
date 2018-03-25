#ifndef _LCDHANDLE_H_
#define _LCDHANDLE_H_
#include "si_datatypes.h"
#include "dvLcd.h"

#define LINE_TXT_MAX   16

void dvLCDLineTXTDraw(BYTE  cLine,BYTE cPostion,...);
 bool_t LcdInit(void);
#endif