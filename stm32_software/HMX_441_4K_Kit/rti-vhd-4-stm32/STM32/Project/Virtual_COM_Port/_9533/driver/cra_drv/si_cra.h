//***************************************************************************
//!file     si_cra.h
//!brief    Silicon Image Device register I/O support.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_CRA_H__
#define __SI_CRA_H__
#include "si_common.h"
#include "si_cra_internal.h"

typedef uint16_t    SiiReg_t;


bool_t              SiiCraInitialize(  BYTE cInstance );
SiiDrvCraError_t/*SiiResultCodes_t*/    SiiCraGetLastResult(  BYTE cInstance );
bool_t              SiiRegInstanceSet(  BYTE cInstance, SiiReg_t virtualAddress, prefuint_t newInstance );

void    SiiRegReadBlock ( BYTE cInstance, SiiReg_t virtualAddr, uint8_t *pBuffer, uint16_t count );
uint8_t SiiRegRead ( BYTE cInstance, SiiReg_t virtualAddr );
void    SiiRegWriteBlock (BYTE cInstance,  SiiReg_t virtualAddr, uint8_t *pBuffer, uint16_t count );
void    SiiRegWrite( BYTE cInstance, SiiReg_t virtualAddr, uint8_t value );
void    SiiRegModify (BYTE cInstance,  SiiReg_t virtualAddr, uint8_t mask, uint8_t value);
void    SiiRegBitsSet (BYTE cInstance,  SiiReg_t virtualAddr, uint8_t bitMask, bool_t setBits );
void    SiiRegBitsSetNew ( BYTE cInstance, SiiReg_t virtualAddr, uint8_t bitMask, bool_t setBits );

uint8_t SiIRegioRead ( BYTE cInstance, uint16_t regAddr );

// Special purpose
void    SiiRegEdidReadBlock (BYTE cInstance,  SiiReg_t segmentAddr, SiiReg_t virtualAddr, uint8_t *pBuffer, uint16_t count );

#endif  // __SI_CRA_H__
