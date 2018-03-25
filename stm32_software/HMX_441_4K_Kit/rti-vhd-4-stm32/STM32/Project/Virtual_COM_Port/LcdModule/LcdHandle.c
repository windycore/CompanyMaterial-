#include "LcdHandle.h"
#include <stdio.h>
#include "si_datatypes.h"
#include <stdarg.h>
#include <string.h>

static char m_acDrawString[LINE_TXT_MAX];     // SkPrintf internal buffer
void dvLCDLineTXTDraw(BYTE  cLine,BYTE cPostion,...)
{
    unsigned long   ulCount = 0;
    const char      *pcString;
    va_list         args;
    char           i;
    // Start the varargs processing.

    va_start(args, cPostion);

    // Write message prefix as necessary...
    ulCount = 0;
    pcString = va_arg( args, const char *);
    ulCount += sprintf( &m_acDrawString[ulCount], pcString,  args ); // kyq  (void *)
    //SiiPlatformUartWrite( outString, ulCount );
     dvLCDLineDraw(cLine,cPostion,m_acDrawString);

    //va_end(vaArgP);
    va_end(args);
}



// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************
// ---------------------------------------------------------------------------
// This API sets up the internal state of component.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
 bool_t LcdInit(void)
{
	dvLCDInit();		//≥ı ºªØLCD
    return true;
}