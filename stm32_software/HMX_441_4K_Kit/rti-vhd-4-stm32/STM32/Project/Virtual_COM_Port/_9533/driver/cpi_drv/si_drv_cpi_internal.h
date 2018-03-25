//***************************************************************************
//!file     si_drv_cpi_internal.h
//!brief    Silicon Image CPI internal driver functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_CPI_DRV_INTERNAL_H__
#define __SI_CPI_DRV_INTERNAL_H__
#include "si_common.h"
#include "si_drv_cpi.h"
#include "si_drv_internal.h"
#include "si_drv_device.h"

//-------------------------------------------------------------------------------
// CPI Enums and manifest constants
//-------------------------------------------------------------------------------

typedef enum _SiiCecError_t
{
    SiiCEC_SHORTPULSE       = 0x80,
    SiiCEC_BADSTART         = 0x40,
    SiiCEC_RXOVERFLOW       = 0x20,
    SiiCEC_ERRORS           = (SiiCEC_SHORTPULSE | SiiCEC_BADSTART | SiiCEC_RXOVERFLOW)
} SiiCecError_t;

#define SII_CPI_OUTPUT_QUEUE_LEN    4

//------------------------------------------------------------------------------
//  CPI Driver Instance Data
//------------------------------------------------------------------------------

typedef struct _CpiMsgQueue_t
{
    int_t           inIndex;
    int_t           outIndex;
    clock_time_t    msTimeout;
    clock_time_t    msStart;
    SiiCpiData_t    queue[SII_CPI_OUTPUT_QUEUE_LEN];
} CpiMsgQueue_t;

typedef struct _CpiInstanceData_t
{
    int                 structVersion;
    int                 instanceIndex; // 1: tx  0: rx  _kyq
    SiiDrvCpiError_t    lastResultCode;     // Contains the result of the last API function called
    uint16_t            statusFlags;

    SiiCpiStatus_t      cecStatus;          // Valid only after interrupt

    uint8_t             logicalAddr;        // CEC logical address for this instance

    CpiMsgQueue_t       msgQueueOut;        // CEC output message queue

    void        (*pLogger)( SiiCpiData_t *, int, bool_t );// SkAppCecMessageLogger
}	CpiInstanceData_t;

extern CpiInstanceData_t cpiInstance[DevNum][SII_NUM_CPI];

//------------------------------------------------------------------------------
//  Driver Specific functions
//------------------------------------------------------------------------------

void    DrvCpiProcessInterrupts(  BYTE cInstance );

//------------------------------------------------------------------------------
//  CPI register access - supports multiple CPI units
//------------------------------------------------------------------------------

uint8_t SiiDrvCpiRegisterRead( BYTE cInstance,  uint8_t devicePage, uint16_t regAddr );
void SiiDrvCpiRegisterWrite(  BYTE cInstance, uint8_t devicePage, uint16_t regAddr, uint8_t value );
void SiiDrvCpiRegisterReadBlock( BYTE cInstance,  uint8_t devicePage, uint16_t regAddr, uint8_t* buffer, uint_t count );
void SiiDrvCpiRegisterWriteBlock( BYTE cInstance,  uint8_t devicePage, uint16_t regAddr, uint8_t *pBuffer, uint_t count );
void SiiDrvCpiRegisterModify( BYTE cInstance,  uint8_t devicePage, uint16_t regAddr, uint8_t mask, uint8_t value);
void SiiDrvCpiRegisterBitsSet(  BYTE cInstance, uint8_t devicePage, uint16_t regAddr, uint8_t bits );
void SiiDrvCpiRegisterBitsClear(  BYTE cInstance, uint8_t devicePage, uint16_t regAddr, uint8_t bits );


#endif // __SI_CPI_DRV_INTERNAL_H__
