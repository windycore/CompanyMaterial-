
#ifndef _UTILCRC_H_
#define _UTILCRC_H_

#include "si_datatypes.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// This file does not contain any non-configurable definitions.

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************
// This file does not contain any public configuration options.

// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

// ---------------------------------------------------------------------------
// This function calculates the 16-bit CRC of the nCount bytes pointed to by
// pData.
//
// Params:
//  pData  : Pointer to the data
//  nCount : Number of bytes
//
// Return:
//  CRC value
// ---------------------------------------------------------------------------
WORD utilCRC16Calc(PBYTE pData,
                   INT   nCount);

#endif // _UTILCRC_H_
