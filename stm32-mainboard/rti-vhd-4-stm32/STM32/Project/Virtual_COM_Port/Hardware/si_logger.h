//***************************************************************************
//!file     si_logger.h
//!brief    Silicon Image Starter Kit debug print message interface
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#if !defined __SI_LOGGER_H__
#define __SI_LOGGER_H__
#include <stdarg.h>
//------------------------------------------------------------------------------
//  Logger UI API functions
//------------------------------------------------------------------------------

unsigned long     SiiPlatformLoggerPrintf( BYTE cInstance,uint_t printFlags, ... );
void    SiiPlatformLoggerMessageLevelSet( uint_t msgLevel );
void    SiiPlatformLoggerColorEnable( bool_t enable );
void    SiiPlatformLoggerPrintEnable ( bool_t enable );
uint8_t SiiPlatformLoggerCharGet(void);
uint8_t SiiPlatformLoggerDigitGet(uint8_t rangeEnd);

//------------------------------------------------------------------------------
//  API for formatted printing to a string
//------------------------------------------------------------------------------

unsigned long SiiSprintf ( char *pOutString, const char *pcString, ... );
unsigned long ___sprintf ( char *pOutString, const char *pcString, va_list pArgs );// kyq void*


#endif
