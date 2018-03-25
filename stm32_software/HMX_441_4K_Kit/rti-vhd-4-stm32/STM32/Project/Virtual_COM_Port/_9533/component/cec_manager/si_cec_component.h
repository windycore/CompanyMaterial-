//***************************************************************************
//!file     si_cec_component.h
//!brief    Silicon Image CEC Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_CEC_COMPONENT_H__
#define __SI_CEC_COMPONENT_H__
#include "si_common.h"
#include "si_cec_config.h"
#include "si_cec_timer.h"
#include "si_cec_enums.h"
#include "si_drv_cpi.h"
#include "si_cec_internal.h"
//-------------------------------------------------------------------------------
// CPI Enums, typedefs, and manifest constants
//-------------------------------------------------------------------------------

typedef enum _SiiDrvCecError_t
{
    RESULT_CEC_SUCCESS,             // Success result code
    RESULT_CEC_FAIL,                // General Failure result code

    RESULT_CEC_INVALID_PARAMETER,

    RESULT_CEC_INVALID_LOGICAL_ADDRESS,
    RESULT_CEC_INVALID_PHYSICAL_ADDRESS,
    RESULT_CEC_INVALID_PORT_INDEX,
    RESULT_CEC_NOT_ADJACENT,
    RESULT_CEC_NO_PA_FOUND,
    RESULT_CEC_TASK_QUEUE_FULL,
    RESULT_CEC_NO_LA_FOUND,

} SiiDrvCecError_t;


typedef enum
{
    SiiCEC_PORT_CHANGE          = 0x0001,
    SiiCEC_POWERSTATE_CHANGE    = 0x0002,
    SiiCEC_SOURCE_LOST          = 0x0004,
} SiiCecStatus_t;

#define MAKE_SRCDEST( src, dest )   ((( (src) << 4) & 0xF0) | ((dest) & 0x0F))
#define GET_CEC_SRC( srcDest )      (( srcDest >> 4) & 0x0F)
#define GET_CEC_DEST( srcDest )     (( srcDest >> 0) & 0x0F)
/*
#define MAX_CEC_PARSERS             8   // Number of CEC message handlers that can
                                        // be registered with this instance.
#define MAX_CEC_TASKS               3   // Number of CEC Task handlers that can
                                        // be registered with this instance.
*/
//------------------------------------------------------------------------------
//  Component functions
//------------------------------------------------------------------------------

int         SiiCecInstanceGet(  BYTE cInstance );
bool_t      SiiCecInstanceSet( BYTE cInstance,  int instanceIndex );

bool_t      SiiCecInitialize(  BYTE cInstance, uint16_t physicalAddress, SiiCecDeviceTypes_t deviceType );
bool_t      SiiCecConfigure(  BYTE cInstance, bool_t enable );

uint16_t    SiiCecStatus(  BYTE cInstance );
bool_t      SiiCecStandby(  BYTE cInstance );
bool_t      SiiCecResume(  BYTE cInstance );
bool_t      SiiCecGetLastResult(  BYTE cInstance );

int_t       SiiCecPortSelectGet(  BYTE cInstance );
void        SiiCecPortSelectSet(  BYTE cInstance, uint8_t inputPort);

uint16_t    SiiCecSendMessage(  BYTE cInstance, uint8_t opCode, uint8_t dest );                      // Deprecated
uint16_t    SiiCecSendMessageEx( BYTE cInstance,  uint8_t opCode, uint8_t src, uint8_t dest );
bool_t      SiiCecSendMessageTask( BYTE cInstance,  uint8_t opCode, uint8_t dest );                  // Deprecated
bool_t      SiiCecSendMessageTaskEx(  BYTE cInstance, uint8_t opCode, uint8_t src, uint8_t dest );
void        SiiCecSendUserControlPressed(  BYTE cInstance, SiiCecUiCommand_t keyCode, SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa );
void        SiiCecSendUserControlReleased(  BYTE cInstance );
uint16_t    SiiCecSendReportPhysicalAddress( BYTE cInstance,  SiiCecLogicalAddresses_t srcLa, uint16_t srcPa );
uint16_t    SiiCecSendReportPowerStatus(  BYTE cInstance, SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa, uint8_t powerState );
bool_t      SiiCecValidateKeyCode( uint8_t keyData );

uint8_t     SiiCecGetAvailableLa(  BYTE cInstance, uint8_t *pDeviceLaList );
bool_t      SiiCecSetOsdName(  BYTE cInstance, char *pOsdName );

void        SiiCecHandler(  BYTE cInstance );
uint8_t     SiiCecGetPowerState(  BYTE cInstance );
void        SiiCecSetPowerState(  BYTE cInstance, SiiCecPowerstatus_t newPowerState );

uint8_t     SiiCecGetDeviceLA(  BYTE cInstance );
void        SiiCecSetDeviceLA (  BYTE cInstance, SiiCecLogicalAddresses_t logicalAddr );
uint16_t    SiiCecGetDevicePA(  BYTE cInstance );
void        SiiCecSetDevicePA( BYTE cInstance,  uint16_t devPa );
SiiCecDeviceTypes_t SiiCecGetDeviceType (  BYTE cInstance );
void        SiiCecSetActiveSource(  BYTE cInstance, SiiCecLogicalAddresses_t activeSrcLA, uint16_t activeSrcPA );
void        SiiCecSetActiveSourceLA( BYTE cInstance,  SiiCecLogicalAddresses_t activeSrcLA);
void        SiiCecSetActiveSourcePA(  BYTE cInstance, uint16_t activeSrcPa );
SiiCecLogicalAddresses_t SiiCecGetActiveSourceLA(  BYTE cInstance );
uint16_t    SiiCecGetActiveSourcePA(  BYTE cInstance );

//bool_t      SiiCecCallbackRegisterParser(  BYTE cInstance, bool_t (*pCallBack)(), bool_t callAlways );
bool_t      SiiCecCallbackRegisterParser(  BYTE cInstance, MsgHandler pCallBack /*bool_t (*pCallBack)()*/, bool_t callAlways );

int_t       SiiCecCallbackRegisterTask(  BYTE cInstance, bool_t (*pCallBack)() );

void        SiiCecFeatureAbortSend( BYTE cInstance, uint8_t opCode, uint8_t reason, uint8_t destLogAddr);
void        SiiCecCpiWrite( BYTE cInstance, uint8_t opCode, uint8_t argCount, uint8_t destLogAddr, SiiCpiData_t *pCecMsg);
void        SiiCecFeatureAbortSendEx( BYTE cInstance, 
                uint8_t opCode, uint8_t reason,
                SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa );
void        SiiCecCpiWriteEx( BYTE cInstance, 
                uint8_t opCode, uint8_t argCount,
                SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa,
                SiiCpiData_t *pCecMsg );

//------------------------------------------------------------------------------
// Enumeration Functions
//------------------------------------------------------------------------------

bool_t      SiiCecEnumerateDevices(  BYTE cInstance, uint8_t *pDeviceList );
bool_t      SiiCecEnumerateDeviceLa(  BYTE cInstance, uint8_t *pDeviceList );
void        SiiCecEnumerateCancel(  BYTE cInstance );
bool_t      SiiCecEnumerateIsComplete(  BYTE cInstance );
void        SiiCecUpdateLogicalDeviceInfo(  BYTE cInstance, uint8_t newLA, uint16_t newPA, bool_t isActive );

//------------------------------------------------------------------------------
// HDMI RX-specific API Function Templates
//------------------------------------------------------------------------------

bool_t      SiiCecSwitchSources(  BYTE cInstance, uint8_t portIndex );
uint16_t    SiiCecPortToPA(  BYTE cInstance, uint8_t portIndex );
uint8_t     SiiCecPortToLA(  BYTE cInstance, uint8_t portIndex );
int_t       SiiCecLaToPort(  BYTE cInstance, uint8_t logicalAddr );
bool_t      SiiCecIsAdjacentLA( BYTE cInstance,  uint8_t logicalAddr );
uint8_t     SiiCecPortToAdjacentLA(  BYTE cInstance, int_t portIndex );
bool_t      SiiCecPortToActSrcSelect( BYTE cInstance);
bool_t      SiiCecDeviceLaIsAvailable(  BYTE cInstance, uint8_t deviceLa );

//------------------------------------------------------------------------------
// HDMI TX-specific API Function Templates
//------------------------------------------------------------------------------

bool_t      SiiCecOneTouchPlay(  BYTE cInstance );
void        SiiCecSendActiveSource( BYTE cInstance,  uint8_t logicalAddr, uint16_t physicalAddr );
void        SiiCecSendInactiveSource( BYTE cInstance,  uint8_t logicalAddr, uint16_t physicalAddr );

//------------------------------------------------------------------------------
// HDMI Repeater-specific CEC API Function Templates
//------------------------------------------------------------------------------

uint16_t    SiiCecNextInvocationTimeGet( BYTE cInstance, uint16_t sysTimerCountMs, CecTimeCounter_t recallTimerCount);
uint16_t    SiiCecAdjacentPhysAddrGet( BYTE cInstance, const uint16_t basePhysAddr, const bool_t isAtOutput, const uint8_t inputPortIndex);

//------------------------------------------------------------------------------
// HDMI CEC Callback API Functions
//------------------------------------------------------------------------------

bool_t      SiiCecCbInputPortSet(  BYTE cInstance, uint8_t inputPortIndex);
void        SiiCecCbSendMessage( uint8_t opCode, bool_t messageAcked );
void        SiiCecCbEnumerateComplete( BYTE cInstance,  bool_t isComplete );
void        SiiCecSendUserControlCmd(  BYTE cInstance, uint8_t opCode, SiiCecLogicalAddresses_t destLa, uint8_t *cArgs,uint8_t cArgCnt  );
void        SiiCecSendUserControlCmd2(  BYTE cInstance, uint8_t opCode, BYTE cAddress, uint8_t *cArgs,uint8_t cArgCnt  );

#endif // __SI_CEC_COMPONENT_H__
