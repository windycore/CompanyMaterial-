//***************************************************************************
//!file     si_drv_cpi.h
//!brief    Silicon Image CPI driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_CPI_DRV_H__
#define __SI_CPI_DRV_H__
#include "si_common.h"
#include "si_drv_cec_config.h"
#include "si_cec_enums.h"


#define SII_NUM_CPI                 1

//-------------------------------------------------------------------------------
// CPI Enums and manifest constants
//-------------------------------------------------------------------------------

#define CPI_MSG_DBG             (MSG_DBG | DBGF_TS | DBGF_CN), DBG_CPI
#define CPI_MSG_STAT            (MSG_STAT | DBGF_CN), DBG_CPI
#define CPI_MSG_ALWAYS          (MSG_ALWAYS | DBGF_CN), DBG_CPI

#define SII_MAX_CMD_SIZE 16

typedef enum _SiiDrvCpiError_t
{
    RESULT_CPI_SUCCESS,             // Success result code
    RESULT_CPI_FAIL,                // General Failure result code
    RESULT_CPI_WRITE_QUEUE_FULL,    // CPI Write Queue was full
} SiiDrvCpiError_t;


typedef enum _SiiCpiTxState_t
{
    SiiTX_IDLE          = 0,
    SiiTX_WAITCMD,
    SiiTX_SENDING,
    SiiTX_SENDACKED,
    SiiTX_SENDFAILED,
    SiiTX_TIMEOUT
} SiiCpiTxState_t;

typedef enum _SiiDrvCpiStatus_t
{
    SiiCPI_CEC_INT          = 0x0001,
    SiiCPI_CEC_STATUS_VALID = 0x0002,  // 有CPI 中断接收发送完毕或者超时
    SiiCPI_CEC_DISABLED     = 0x0004,
} SiiDrvCpiStatus_t;

//-------------------------------------------------------------------------------
// CPI data structures
//-------------------------------------------------------------------------------

typedef struct _SiiCpiData_t
{
    uint8_t         srcDestAddr;            // Source in upper nibble, dest in lower nibble
    SiiCecOpcodes_t opcode;
    uint8_t         args[ SII_MAX_CMD_SIZE ];
    uint8_t         argCount;
    SiiCpiTxState_t txState;
    uint16_t        msgId;
} SiiCpiData_t;

//-------------------------------------------------------------------------------
// CPI data structures
//-------------------------------------------------------------------------------

typedef struct _SiiCpiStatus_t
{
    uint8_t         rxState;   // 1: 有消息收到 DrvCpiProcessInterrupts
    SiiCpiTxState_t txState; // DrvCpiProcessInterrupts() 会设置
    uint8_t         cecError;
    uint16_t        msgId;

} SiiCpiStatus_t;

typedef bool_t (*MsgHandler)( BYTE cInstance,  SiiCpiData_t* pMsg);
//------------------------------------------------------------------------------
//  Standard driver functions
//------------------------------------------------------------------------------

bool_t      SiiDrvCpiInitialize(  BYTE cInstance );
uint16_t    SiiDrvCpiStatus(  BYTE cInstance );
bool_t      SiiDrvCpiStandby(  BYTE cInstance );
bool_t      SiiDrvCpiResume(  BYTE cInstance );
uint_t      SiiDrvCpiInstanceGet(  BYTE cInstance );
bool_t      SiiDrvCpiInstanceSet(  BYTE cInstance, uint_t instanceIndex );
uint_t      SiiDrvCpiGetLastResult(  BYTE cInstance );

bool_t      SiiCpiRegisterLogger ( BYTE cInstance,  void (*pLogger)(SiiCpiData_t *, int, bool_t ) );

void        SiiDrvCpiEnable(  BYTE cInstance );
void        SiiDrvCpiDisable(  BYTE cInstance );

//------------------------------------------------------------------------------
//  Driver Specific functions
//------------------------------------------------------------------------------

bool_t      SiiDrvCpiRead(  BYTE cInstance, SiiCpiData_t *pMsg );
uint16_t    SiiDrvCpiWrite(  BYTE cInstance, SiiCpiData_t *pMsg );
void        SiiDrvCpiServiceWriteQueue(  BYTE cInstance );
bool_t      SiiDrvCpiHwStatusGet( BYTE cInstance,  SiiCpiStatus_t *pCpiStat );
uint_t      SiiDrvCpiFrameCount(  BYTE cInstance );

bool_t      SiiDrvCpiSetLogicalAddr( BYTE cInstance,  uint8_t logicalAddress );
bool_t      SiiDrvCpiAddLogicalAddr( BYTE cInstance,  bool_t addLa, uint8_t logicalAddress );
uint8_t	    SiiDrvCpiGetLogicalAddr(  BYTE cInstance );
uint16_t    SiiDrvCpiSendPing(  BYTE cInstance, uint8_t destLa );

#endif // __SI_CPI_DRV_H__
