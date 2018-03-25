//***************************************************************************
//!file     si_cec_component.c
//!brief    Silicon Image CEC Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "string.h"
#include "si_cec_component.h"
#include "si_cec_internal.h"
#include "si_drv_switch_config.h"
#include "si_cec_enums.h"  //[kyq add: 20150725 16:33:40]


//------------------------------------------------------------------------------
//  CEC Component Instance Data
//------------------------------------------------------------------------------

CecInstanceData_t cecInstance[INSTANCE_9533][SII_NUM_CEC];
CecInstanceData_t *pCec[INSTANCE_9533] = 
{
     &cecInstance[0][0],
#if(INSTANCE_9533>1)
     &cecInstance[1][0],
#endif
#if(INSTANCE_9533>2)
     &cecInstance[2][0],
#endif
#if(INSTANCE_9533>3)
     &cecInstance[3][0],
#endif
#if(INSTANCE_9533>4)
     &cecInstance[4][0],
#endif
#if(INSTANCE_9533>5)
     &cecInstance[5][0],
#endif
#if(INSTANCE_9533>6)
     &cecInstance[6][0],
#endif
#if(INSTANCE_9533>7)
     &cecInstance[7][0],
#endif
};
                                           
//------------------------------------------------------------------------------
// Function:    SiiCecInitialize
// Description: Initialize the current instance of the CEC component.
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//				if some failure occurred.
//------------------------------------------------------------------------------
// 初始化CEC 结构体
//设置本身逻辑地址, 物理地址
// 设置本身设备类型, 注册消息处理函数
bool_t SiiCecInitialize (  BYTE cInstance, uint16_t physicalAddress, SiiCecDeviceTypes_t deviceType )
{
    int_t   i;
    bool_t  error = false;
    DEBUG_PRINT( MSG_STAT, "---SiiCecInitialize() :physicalAddress =[%04X] ,DeviceTypes = [%04X]\n", (int)physicalAddress, (int)deviceType);
    // Initialize the internal data structures used by CEC
    memset( (pCec[cInstance]), 0, sizeof( CecInstanceData_t ));

    (pCec[cInstance])->enable            = false;
    (pCec[cInstance])->logicalAddr       = CEC_LOGADDR_UNREGORBC;
    (pCec[cInstance])->sourcePowerStatus = CEC_POWERSTATUS_STANDBY;
    (pCec[cInstance])->activeSrcLogical  = CEC_LOGADDR_UNREGORBC;
    (pCec[cInstance])->deviceType        = deviceType;

    (pCec[cInstance])->lastUserControlPressedSourceLa = CEC_LOGADDR_UNREGORBC;
    (pCec[cInstance])->lastUserControlPressedTargetLa = CEC_LOGADDR_UNREGORBC;

    SiiCecSetDevicePA(  cInstance, physicalAddress );

    // Mark all devices as not found
    for ( i = 0; i <= CEC_LOGADDR_UNREGORBC; i++ )
    {
        (pCec[cInstance])->logicalDeviceInfo[ i].cecPA = 0xFFFF;
        (pCec[cInstance])->logicalDeviceInfo[ i].deviceType = CEC_DT_COUNT;
    }

   // Register internal message handlers defined for certain device types
    switch ( (pCec[cInstance])->deviceType )
    {
        case CEC_DT_TV:
            error = !SiiCecCallbackRegisterParser( cInstance,CecRxMsgHandler, false );  // SiiCecMsgHandlers_t
            break;
        case CEC_DT_VIDEO_PROCESSOR:
            error = !SiiCecCallbackRegisterParser( cInstance,CecTxMsgHandler, false );
            SiiCecSetDeviceLA(  cInstance, CEC_LOGADDR_VIDEO_PROCESSOR );
            break;
        case CEC_DT_AUDIO_SYSTEM:
//            error = !SiiCecCallbackRegisterParser(  cInstance, CecRxMsgHandler, false );
            error = !SiiCecCallbackRegisterParser( cInstance,  CecTxMsgHandler, false );
//    		(pCec[cInstance])->devLogicalAddr = CEC_LOGADDR_AUDSYS; //store the actaul device logical address

             SiiCecSetDeviceLA(  cInstance, CEC_LOGADDR_AUDSYS ); // kyq: org code have it //[kyq Begin: 20150717 16:47:48]
            break;
        case CEC_DT_PLAYBACK:
            error = !SiiCecCallbackRegisterParser(  cInstance, CecRxMsgHandler, false ); // kyq: org code have it //[kyq Begin: 20150717 16:47:47]
            error = !SiiCecCallbackRegisterParser(  cInstance, CecTxMsgHandler, false );
            SiiCecSetDeviceLA( cInstance,  CEC_LOGADDR_PLAYBACK2 );  //[kyq Begin: 20150717 16:48:07]
            break;
        default:
            break;
    }


	return( !error );
}

//------------------------------------------------------------------------------
// Function:    SiiCecConfigure
// Description:
// Parameters:  none
// Returns:     true if the configuration was successful, or false if some
//              failure occurred
//------------------------------------------------------------------------------

bool_t SiiCecConfigure (  BYTE cInstance, bool_t enable )
{

    (pCec[cInstance])->enable   = enable;
    (pCec[cInstance])->enable ? SiiDrvCpiEnable( cInstance ) : SiiDrvCpiDisable( cInstance );

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiCecStatus
// Description: Returns a status flag word containing CEC-specific
//				information about the state of the device.
//              NOTE: Some flags are cleared as soon as they are read.
// Parameters:  none
// Returns:     Sixteen-bit status flags word for the CEC Component
//------------------------------------------------------------------------------

uint16_t SiiCecStatus (  BYTE cInstance )
{
    uint16_t statusFlags = (pCec[cInstance])->statusFlags;

    (pCec[cInstance])->statusFlags &= ~(SiiCEC_POWERSTATE_CHANGE | SiiCEC_PORT_CHANGE);
    return( statusFlags );
}

//------------------------------------------------------------------------------
// Function:    SiiCecStandby
//! @brief      Prepare for device standby mode.
//              Make any configuration changes required for standby mode and
//              save any settings that must be restored after a power resume.
// Returns:     true if the CEC component entered standby mode successfully,
//				or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiCecStandby (  BYTE cInstance )
{
    return( SiiDrvCpiStandby( cInstance ));
}

//------------------------------------------------------------------------------
// Function:    SiiCecResume
//! @brief      Recover from power down standby mode
//				Restore settings saved by SiiCecStandby and configure the
//              component as required.
// Parameters:  none
// Returns:     true if the CEC component successfully exited standby mode,
//				or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiCecResume (  BYTE cInstance )
{
    return( SiiDrvCpiResume( cInstance ));
}

//------------------------------------------------------------------------------
// Function:    SiiCecInstanceGet
// Description: returns the index of the current CEC instance
// Parameters:  none
// Returns:     Index of the current CEC instance
//------------------------------------------------------------------------------
int SiiCecInstanceGet (  BYTE cInstance )
{

    return( (pCec[cInstance])->instanceIndex );
}

//------------------------------------------------------------------------------
// Function:    SiiCecInstanceSet
// Description: Set the component global data pointer to the requested instance.
// Parameters:  instanceIndex
// Returns:     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------
bool_t SiiCecInstanceSet ( BYTE cInstance, int instanceIndex )
{
    if ( instanceIndex < SII_NUM_CPI )
    {
        (pCec[cInstance]) = &cecInstance[cInstance][ instanceIndex];
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
// Function:    SiiCecGetLastResult
// Description: Returns the result of the last SiiCecxxx function called
// Parameters:  none
// Returns:     Result of last CEC component function.
//------------------------------------------------------------------------------

bool_t SiiCecGetLastResult (  BYTE cInstance )
{

	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiCecCallbackRegisterParser
// Description: Register a message handler with the specified CEC instance
// Parameters:  Callback function pointer
// Returns:     true if successful
//------------------------------------------------------------------------------

bool_t SiiCecCallbackRegisterParser (  BYTE cInstance, MsgHandler pCallBack /*bool_t (*pCallBack)()*/, bool_t callAlways )
{
    bool_t  success = false;
    int_t   callBackIndex;

    for ( callBackIndex = 0; callBackIndex < MAX_CEC_PARSERS; callBackIndex++ )
    {
        if ( (pCec[cInstance])->messageHandlers[ callBackIndex ].pMsgHandler == 0)
        {
            (pCec[cInstance])->messageHandlers[ callBackIndex ].pMsgHandler  = pCallBack;//pCallBack;  // SiiCecMsgHandlers_t
            (pCec[cInstance])->messageHandlers[ callBackIndex ].callAlways   = callAlways;
            success = true;
            break;
        }
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SiiCecPortSelectGet
// Description: Returns the last port selected via an ACTIVE_SOURCE or other
//              CEC command that may change the selected port.  Note that it
//              is only valid immediately after the SiiCEC_PORT_CHANGE flag is
//              set.
// Parameters:  none
// Returns:     Input Processor port selected via CEC command.
//------------------------------------------------------------------------------

int_t SiiCecPortSelectGet (  BYTE cInstance )
{

    return( (pCec[cInstance])->portSelect );
}

//------------------------------------------------------------------------------
// Function:    SiiCecPortSelectSet
// Description: Set internal port selection. Used for communication with
//              other CEC components.
// Parameters:  none
// Returns:
//------------------------------------------------------------------------------

void SiiCecPortSelectSet (  BYTE cInstance, uint8_t inputPort)
{
    (pCec[cInstance])->portSelect = inputPort;
}

//------------------------------------------------------------------------------
// Function:    SiiCecCallbackRegisterTask
// Description: Register a CEC task with the current CEC instance
// Parameters:  Callback function pointer
// Returns:     Index of registered task, or -1 if no room
//------------------------------------------------------------------------------

int_t SiiCecCallbackRegisterTask ( BYTE cInstance,  bool_t (*pCallBack)() )
{
    int_t callBackIndex;

    for ( callBackIndex = 0; callBackIndex < MAX_CEC_TASKS; callBackIndex++ )
    {
        if ( (pCec[cInstance])->pTasks[ callBackIndex ] == 0)
        {
            (pCec[cInstance])->pTasks[ callBackIndex ] = pCallBack;
            break;
        }
    }

    return( ( callBackIndex == MAX_CEC_TASKS) ? -1 : (callBackIndex + SiiCECTASK_COUNT) );
}

//------------------------------------------------------------------------------
//! @brief      Send a single byte (no parameter) message on the CEC bus.  Does
//!             not wait for a reply.
//! @param[in]  opCode  - CEC opcode to send
//! @param[in]  src     - source CEC logical address
//! @param[in]  dest    - destination CEC logical address
//! @return     uint16_t    16 bit message ID or 0 if CEC not enabled
//------------------------------------------------------------------------------
uint16_t SiiCecSendMessageEx (  BYTE cInstance, uint8_t opCode, uint8_t src, uint8_t dest )
{
    SiiCpiData_t cecFrame;

    if ( !(pCec[cInstance])->enable )
    {
        (pCec[cInstance])->lastResultCode = RESULT_CEC_FAIL;
        return( 0 );
    }
    cecFrame.opcode        = (SiiCecOpcodes_t)opCode;
    cecFrame.srcDestAddr   = MAKE_SRCDEST( src, dest );
    cecFrame.argCount      = 0;

    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    return( SiiDrvCpiWrite(  cInstance, &cecFrame ));
}

//------------------------------------------------------------------------------
//! @brief      Send a single byte (no parameter) message on the CEC bus.  Does
//!             not wait for a reply. The local device CEC logical address is
//!             used as the source device in the CEC message.
//! @param[in]  opCode  - CEC opcode to send
//! @param[in]  dest    - destination CEC logical address
//! @return     uint16_t    16 bit message ID
//!
//! @note       This function is deprecated; please use SiiCecSendMessageEx
//------------------------------------------------------------------------------
uint16_t SiiCecSendMessage ( BYTE cInstance,  uint8_t opCode, uint8_t dest )
{

    return( SiiCecSendMessageEx(  cInstance, opCode, (pCec[cInstance])->logicalAddr, dest ));
}

//------------------------------------------------------------------------------
//! @brief  Send a remote control key pressed command to the specified logical
//!         device.
//! @param[in]  keyCode - UI Command from CEC enum lists
//------------------------------------------------------------------------------
void SiiCecSendUserControlPressed (  BYTE cInstance, SiiCecUiCommand_t keyCode, SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa )
{
    SiiCpiData_t cecFrame;

    if ( !(pCec[cInstance])->enable )
    {
        return;
    }

    (pCec[cInstance])->lastUserControlPressedSourceLa = srcLa;
    (pCec[cInstance])->lastUserControlPressedTargetLa = destLa;

    cecFrame.opcode        = CECOP_USER_CONTROL_PRESSED;
    cecFrame.srcDestAddr   = MAKE_SRCDEST( srcLa, destLa );
    cecFrame.args[0]       = keyCode;
    cecFrame.argCount      = 1;
    SiiDrvCpiWrite(  cInstance, &cecFrame );
}

//------------------------------------------------------------------------------
//! @brief  Send a remote control key released command to the logical device
//!         specified in the last SiiCecSendUserControlPressed() call.
//------------------------------------------------------------------------------
void SiiCecSendUserControlReleased (  BYTE cInstance )
{
    if ( !(pCec[cInstance])->enable )
    {
        return;
    }

    if ( (pCec[cInstance])->lastUserControlPressedTargetLa != CEC_LOGADDR_UNREGORBC )
    {
        SiiCecSendMessageEx( cInstance, 
            CECOP_USER_CONTROL_RELEASED,
            (pCec[cInstance])->lastUserControlPressedSourceLa,
            (pCec[cInstance])->lastUserControlPressedTargetLa );
        (pCec[cInstance])->lastUserControlPressedSourceLa = CEC_LOGADDR_UNREGORBC;
        (pCec[cInstance])->lastUserControlPressedTargetLa = CEC_LOGADDR_UNREGORBC;
    }
}

//------------------------------------------------------------------------------
// Function:    SiiCecGetPowerState
// Description: Returns the CEC local power state.
//              Should be called after every call to SiiCecHandler since
//              a CEC device may have issued a standby or view message.
//
// Valid values:    CEC_POWERSTATUS_ON
//                  CEC_POWERSTATUS_STANDBY
//                  CEC_POWERSTATUS_STANDBY_TO_ON
//                  CEC_POWERSTATUS_ON_TO_STANDBY
//------------------------------------------------------------------------------

uint8_t SiiCecGetPowerState (  BYTE cInstance )
{

    return( (pCec[cInstance])->powerState );
}

//------------------------------------------------------------------------------
// Function:    SiiCecSetPowerState
// Description: Set the CEC local power state.
//
// Valid values:    CEC_POWERSTATUS_ON
//                  CEC_POWERSTATUS_STANDBY
//                  CEC_POWERSTATUS_STANDBY_TO_ON
//                  CEC_POWERSTATUS_ON_TO_STANDBY
//------------------------------------------------------------------------------

void SiiCecSetPowerState (  BYTE cInstance, SiiCecPowerstatus_t newPowerState )
{
    if ( !(pCec[cInstance])->enable )
    {
        return;
    }

    if ( (pCec[cInstance])->powerState != newPowerState )
    {
        switch ( newPowerState )
        {
            case CEC_POWERSTATUS_STANDBY_TO_ON:
            case CEC_POWERSTATUS_ON:
                newPowerState = CEC_POWERSTATUS_ON;
                break;

            case CEC_POWERSTATUS_ON_TO_STANDBY:
            case CEC_POWERSTATUS_STANDBY:
                newPowerState = CEC_POWERSTATUS_STANDBY;
                break;

            default:
                break;
        }

    (pCec[cInstance])->powerState = newPowerState;
    }
}

//------------------------------------------------------------------------------
// Function:    SiiCecPortToPA
// Description: Return the CEC physical address of the last selected logical
//              device on the CEC path for this port.  If no logical devices
//              on this path, return 0xFFFF
// Parameters:  portIndex:  HDMI RX, 0-3
//                          HDMI TX, 0
// Returns:     CEC physical address for the port or 0xFFFF if not found
//------------------------------------------------------------------------------

uint16_t SiiCecPortToPA (  BYTE cInstance, uint8_t portIndex )
{
    int_t i;
    uint16_t    physicalAddress = 0xFFFF;   // failure return

    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;

    for ( i = 0; i <= CEC_LOGADDR_UNREGORBC; i++ )
    {
        // Does requested port has the same index as the current logical address?
        if ( (pCec[cInstance])->logicalDeviceInfo[i].port == portIndex )
        {
            // Save the physical address.
            physicalAddress = (pCec[cInstance])->logicalDeviceInfo[ i].cecPA;

            // There may be more than one device in the physical address chain
            // for a particular port, check to see if this is the last device
            // that was selected (ACTIVE SOURCE) on this particular port.
            // If NO logical device was the last active source on this port,
            // we'll take the first logical device on this port.
            if ( (pCec[cInstance])->logicalDeviceInfo[i].selected )
            {
                break;
            }
        }
    }

    if ( physicalAddress == 0xFFFF )
    {
        (pCec[cInstance])->lastResultCode = RESULT_CEC_NO_PA_FOUND;
    }
    return( physicalAddress );
}

//------------------------------------------------------------------------------
//! @brief  Return the CEC logical address of the passed port
//! @param[in]  portIndex
//! @return     CEC logical address for the port or CEC_LOGADDR_UNREGORBC
//!             if port has not been assigned a logical address
//------------------------------------------------------------------------------
uint8_t SiiCecPortToLA (  BYTE cInstance, uint8_t portIndex )
{
    uint8_t logicalAddress;

    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    for ( logicalAddress = 0; logicalAddress <= CEC_LOGADDR_UNREGORBC; logicalAddress++ )
    {
        // Does requested port has the same index as the current logical address?
        if ( (pCec[cInstance])->logicalDeviceInfo[ logicalAddress].port == portIndex )
        {
            break;
        }
    }

    if ( logicalAddress == CEC_LOGADDR_UNREGORBC )
    {
        (pCec[cInstance])->lastResultCode = RESULT_CEC_NO_LA_FOUND;
    }
    return( logicalAddress );
}

//------------------------------------------------------------------------------
// Function:    SiiCecLaToPort
// Description: Translate the passed logical address into a physical HDMI port
// Parameters:  logicalAddress  - The CEC logical address to translate
// Returns:     HDMI physical port index or oxFF if not found.
//------------------------------------------------------------------------------

int_t SiiCecLaToPort (  BYTE cInstance, uint8_t logicalAddr )
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;

    if ( logicalAddr > CEC_LOGADDR_UNREGORBC )
    {
        (pCec[cInstance])->lastResultCode = RESULT_CEC_INVALID_LOGICAL_ADDRESS;
        return( 0xFF );
    }
    return( (pCec[cInstance])->logicalDeviceInfo[ logicalAddr].port );
}

//------------------------------------------------------------------------------
// Function:    SiiCecGetDeviceLA
// Description: Return the CEC logical address for this Host device
//------------------------------------------------------------------------------

uint8_t SiiCecGetDeviceLA (  BYTE cInstance )
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    return( (pCec[cInstance])->logicalAddr );
}

//------------------------------------------------------------------------------
// Function:    SiiCecSetDeviceLA
// Description: Set the CEC logical address for this Host device
//------------------------------------------------------------------------------

void SiiCecSetDeviceLA (  BYTE cInstance, SiiCecLogicalAddresses_t logicalAddr )
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    (pCec[cInstance])->logicalAddr = logicalAddr;
    SiiDrvCpiSetLogicalAddr(  cInstance, logicalAddr );
}

//------------------------------------------------------------------------------
// Function:    SiiCecGetDevicePA
// Description: Return the physical address for this Host device
//------------------------------------------------------------------------------

uint16_t SiiCecGetDevicePA (  BYTE cInstance )
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;

    return( (pCec[cInstance])->physicalAddr );
}


//------------------------------------------------------------------------------
// Function:    SiiCecSetDevicePA
// Description: Set the host device physical address (initiator physical address)
//------------------------------------------------------------------------------

void SiiCecSetDevicePA ( BYTE cInstance,  uint16_t devPa )
{
    uint8_t     index;
    uint16_t    mask;

    (pCec[cInstance])->physicalAddr = devPa;

    /* We were given the Host, or parent, PA, so we determine   */
    /* the direct child Physical Address field (A.B.C.D).       */

    mask = 0x00F0;
    for ( index = 1; index < 4; index++ )
    {
        if (( devPa & mask ) != 0)
        {
            break;
        }
        mask <<= 4;
    }

    (pCec[cInstance])->paShift = (index - 1) * 4;
    (pCec[cInstance])->paChildMask = 0x000F << (pCec[cInstance])->paShift;

    DEBUG_PRINT( MSG_STAT, "---Device PA:[%04X] Mask:[%04X] Shift:[%d]\n", (int)devPa, (int)(pCec[cInstance])->paChildMask, (int)(pCec[cInstance])->paShift );

#if (INC_CEC_SWITCH == 1)
    // PA change makes the switch to respond by sending new routing information after a short
    // pause, which is required to let the other HDMI devices to settle down after
    // HDP and following reconfiguration
    SiiCecSwitchSendRouteInfo( cInstance );
    // TODO: Indicate for CEC dispatcher that new task is pending and has to be processed ASAP
    //SiiCecForceFastHandling();
#endif

}

//------------------------------------------------------------------------------
// Function:    SiiCecGetDeviceType
// Description: Return the CEC device type
//------------------------------------------------------------------------------

SiiCecDeviceTypes_t SiiCecGetDeviceType (  BYTE cInstance )
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    return( (pCec[cInstance])->deviceType );
}
//------------------------------------------------------------------------------
// Function:    SiiCecSetActiveSource
// Description: Set the CEC logical and physical address for active source device
//------------------------------------------------------------------------------

void SiiCecSetActiveSource(  BYTE cInstance, SiiCecLogicalAddresses_t activeSrcLA, uint16_t activeSrcPA )
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;

    (pCec[cInstance])->activeSrcLogical = activeSrcLA;
    (pCec[cInstance])->activeSrcPhysical= activeSrcPA;
    DEBUG_PRINT(CEC_MSG_DBG,"SiiCecSetActiveSource   = 0x%04x\n",  activeSrcPA);
}

//------------------------------------------------------------------------------
// Function:    SiiCecSetActiveSourceLA
// Description: Set the CEC logical address for active source device
//------------------------------------------------------------------------------

void SiiCecSetActiveSourceLA( BYTE cInstance,  SiiCecLogicalAddresses_t activeSrcLA)
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    (pCec[cInstance])->activeSrcLogical = activeSrcLA;
}

//------------------------------------------------------------------------------
//! @brief  Set the CEC physical address for active source device
//------------------------------------------------------------------------------
void SiiCecSetActiveSourcePA (  BYTE cInstance, uint16_t activeSrcPa )
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    (pCec[cInstance])->activeSrcPhysical = activeSrcPa;

    DEBUG_PRINT(CEC_MSG_DBG,"SiiCecSetActiveSourcePA   = 0x%04x\n",  activeSrcPa);
}


//------------------------------------------------------------------------------
// Function:    SiiCecGetActiveSource
// Description: Get the CEC logical address for active source device
//------------------------------------------------------------------------------

SiiCecLogicalAddresses_t SiiCecGetActiveSourceLA(  BYTE cInstance )
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    return ((pCec[cInstance])->activeSrcLogical);
}

//------------------------------------------------------------------------------
//! @brief  Return the CEC physical address for active source device
//------------------------------------------------------------------------------
uint16_t SiiCecGetActiveSourcePA (  BYTE cInstance )
{
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    return( (pCec[cInstance])->activeSrcPhysical );
}

//------------------------------------------------------------------------------
// Function:    SiiCecAdjacentPA
// Description: Returns true if passed physical devices are adjacent
//
//  Examples:
//
//  Upstream                        Downstream
//  --------                        ----------
//  0.0.0.0     is adjacent     to  2.0.0.0
//  0.0.0.0     is NOT adjacent to  2.1.0.0
//  2.1.0.0     is adjacent     to  2.0.0.0
//  2.1.0.0     is NOT adjacent to  2.2.0.0
//  2.1.0.0     is adjacent     to  2.1.1.0
//------------------------------------------------------------------------------

bool_t SiiCecAdjacentPA (  BYTE cInstance, uint16_t paOne, uint16_t paTwo )
{
    uint8_t     i;
    uint16_t    one, two, paMask;
    bool_t      isChild = false;

    /* Compare nodes from A to D.  If they don't match and one of them  */
    /* is 0, they are adjacent.                                         */

    isChild = false;
    paMask = 0xF000;
    for ( i = 0; i < 4; i++ )
    {
        one = paOne & paMask;
        two = paTwo & paMask;

        /* If a previous node mismatched, but this node is not  */
        /* both 0, these two are not adjacent.                  */

        if ( isChild && ( one != two ))
        {
            isChild = false;
            break;
        }

        /* If the two nodes are NOT the same, and one of them is 0, */
        /* they MIGHT be adjacent, but we have to check for the     */
        /* remainder of the nodes to be the same on both.           */

        if ( one != two )
        {
            /* If one node is zero we have adjacent nodes.  */

            if (( one == 0 ) || ( two == 0 ))
            {
                isChild = true;
            }
        }
        paMask >>= 4;
    }
    DEBUG_PRINT( CEC_MSG_DBG, "one: %04X  two: %04X  paMask: %04X\n", one, two, paMask );
    return( isChild );
}

//------------------------------------------------------------------------------
// Function:    SiiCecIsAdjacentLA
// Description: Returns true if passed logical device is in the adjacent rank
//              of devices.
//------------------------------------------------------------------------------

bool_t SiiCecIsAdjacentLA (  BYTE cInstance, uint8_t logicalAddr )
{

    if ( logicalAddr > CEC_LOGADDR_UNREGORBC)
    {
        (pCec[cInstance])->lastResultCode = RESULT_CEC_INVALID_LOGICAL_ADDRESS;
        return( false );
    }

    if ( !SiiCecAdjacentPA( cInstance,  (pCec[cInstance])->physicalAddr, (pCec[cInstance])->logicalDeviceInfo[ logicalAddr].cecPA ))
        {
            DEBUG_PRINT(
                CEC_MSG_DBG,
                "LA=%02X (%04X) is NOT adjacent to %02X (%04X)\n",
                logicalAddr, (pCec[cInstance])->logicalDeviceInfo[ logicalAddr].cecPA, (pCec[cInstance])->logicalAddr, (pCec[cInstance])->physicalAddr);
            (pCec[cInstance])->lastResultCode = RESULT_CEC_NOT_ADJACENT;
            return( false );
        }
    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    return( true );
    }

//------------------------------------------------------------------------------
// Function:    SiiCecPortToAdjacentLA
// Description: Return the CEC logical address for a device that is adjacent
//              to the specified device port
// Parameters:  portIndex:  HDMI RX port number, 0-based
// Returns:     CEC Logical address for the port, or 0xFF if no LA found.
//------------------------------------------------------------------------------

uint8_t SiiCecPortToAdjacentLA ( BYTE cInstance,  int_t portIndex )
{
    int_t       thisLa;

    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;

    for ( thisLa = 0; thisLa <= CEC_LOGADDR_UNREGORBC; thisLa++ )
    {
        // Does requested port has the same index as the current logical address?
        if ( (pCec[cInstance])->logicalDeviceInfo[ thisLa].port == portIndex )
        {
            // Is the physical address adjacent to the requested port on our device?
            if ( SiiCecAdjacentPA(  cInstance, (pCec[cInstance])->logicalDeviceInfo[ thisLa].cecPA, (pCec[cInstance])->physicalAddr ))
            {
                break;
            }
        }
    }

    if ( thisLa >= CEC_LOGADDR_UNREGORBC )
    {
        thisLa = 0xFF;
    }

    return( thisLa );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Figures out whether two devices have relationship
//!             of a Sink and a Source.
//!
//!             If so, returns index of the Sink's
//!             input port that is connected to the Source output port
//!
//! @param[in]  sinkPhysAddr      - PA of the possible Sink device,
//! @param[in]  sourcePhysAddr    - PA of the possible Source device,
//! @param[out] pInputPortIndex   - (return value) pointer to an index of an input  port of the Sink
//!                                 0xFF - in the case when Sink-Source relationship hasn't been verified
//!
//! @retval     true, if devices are linked directly over some of the Sink's inputs
//! @retval     false - otherwise
//-------------------------------------------------------------------------------------------------

static bool_t CecIsLinkedDirectlyInPortGet(const uint16_t sinkPhysAddr, const uint16_t sourcePhysAddr, uint8_t *pInputPortIndex)
{
    // criteria: 1. Sink must be one level above the Source
    //           2. The input port number (= inputPortIdx+1) must be equal to
    //              the Source's physical address last nonzero digit (p. 8.7.2. of HDMI 1.4 spec)
    uint8_t     i;
    uint8_t     n = 0;
    uint16_t    mask = 0x000F;
    uint16_t    sourceMasked;
    uint16_t    sinkMasked;
    bool_t      isLinked = false;

    // crit.1 meets if and only if sink and source have one of the following patterns:
    // {0000, n000}, {x000, xn00}, {xy00, xyn0}, {xyz0, xyzn}
    for (i = 0; i < 4; i++)
    {
        sourceMasked = sourcePhysAddr & mask;
        sinkMasked   = sinkPhysAddr & mask;

        if (n == 0) // n point not found yet
        {
             // skip zero-zero pairs
             if ((sinkMasked == 0) && (sourceMasked !=0)) // first zero-nonzero pair found
             {
                 n = sourceMasked >> (4 * i); // input port number (nonzero)
                 *pInputPortIndex = n - 1;      // input port index to return
                 isLinked = true;             // potential detection
             }
             else if ((sinkMasked != 0) && (sourceMasked !=0))
             {
                break;
             }
             else if ((sinkMasked != 0) && (sourceMasked ==0))
             {
                break;
             }
        }
        else // all other digit pairs must be non-zero & equal
        {
            if ((sinkMasked == 0) || (sinkMasked != sourceMasked))
            {
                isLinked = false; // reset linked flag if wrong MSB pattern found
                *pInputPortIndex = 0xFF;
                break;
            }
        }

        mask <<= 4; // scan all 4 digits
    }


    return isLinked;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Selects an input port or ARC output port adjacent to the active audio source
//!
//! @retval     true  - success,
//! @retval     false - active source device isn't adjacent or can't be selected
//-------------------------------------------------------------------------------------------------

bool_t SiiCecPortToActSrcSelect( BYTE cInstance)
{
    uint8_t inputPortIndex;
    bool_t  isLinkedToSink = false;
    bool_t  isLinkedToSource = false;
    bool_t  isSuccess = true;

    // Check if active source is adjacent to this device

   isLinkedToSource = CecIsLinkedDirectlyInPortGet( (pCec[cInstance])->physicalAddr, (pCec[cInstance])->activeSrcPhysical, &inputPortIndex);    // sony   -----  9533

//    0x0000  ----0x2000       	
//    	sprintf(USART1_tx_buff,"SiiCecPortToActSrcSelect1  isLinkedToSource 0x%04x    inputPortIndex 0x%04x   \n",isLinkedToSource,inputPortIndex );
//	 UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));


// 9533      -----  sony     -------inputport
// 0x0000  ----    0x2000     -------0x0001

	DEBUG_PRINT(CEC_MSG_DBG,"SiiCecPortToActSrcSelect1  physicalAddr 0x%04x    activeSrcPhysical 0x%04x   \n",(pCec[cInstance])->physicalAddr,(pCec[cInstance])->activeSrcPhysical);
	
    if(!isLinkedToSource) //output port
    {
        isLinkedToSink = CecIsLinkedDirectlyInPortGet((pCec[cInstance])->activeSrcPhysical, (pCec[cInstance])->physicalAddr, &inputPortIndex);
		DEBUG_PRINT(CEC_MSG_DBG,"SiiCecPortToActSrcSelect2 isLinkedToSink 0x%04x    inputPortIndex 0x%04x   \n",isLinkedToSink,inputPortIndex);

        if (!isLinkedToSink)
        {
            DEBUG_PRINT( CEC_MSG_DBG, "SiiCecPortToActSrcSelect(): Active source isn't adjacent to this device.\n");
            isSuccess = false;
        }
        else
        {   // Use ARC to receive audio
            DEBUG_PRINT( CEC_MSG_DBG, "Audio comes over Audio Return Channel.\n");
            //isSuccess = false; // If ARC isn't available
            isSuccess = true;
        }
    }
    else
    {   // Switch to Active Source
           DEBUG_PRINT( CEC_MSG_DBG, "SiiCecCbInputPortSet\n");
        SiiCecCbInputPortSet( cInstance, inputPortIndex);
    }

    if (isSuccess)
    {
        DEBUG_PRINT(CEC_MSG_DBG, "\nActive Source: LA[%02X], PA[%04X], Port[%02X]\n", (int)(pCec[cInstance])->activeSrcLogical, (pCec[cInstance])->activeSrcPhysical, (int)(pCec[cInstance])->portSelect);
    }

    return isSuccess;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Calculates physical address of a device, potential or real, that
//!             is adjacent to the reference device.
//!
//! @param[in]  basePhysAddr   - PA of the reference device,
//! @param[in]  isAtOutput     - if true, returns a device's PA adjacent to output port,
//!                              if false, returns a device's PA adjacent PA to
//!                              specified input port
//! @param[in]  inputPortIndex - zero-based index of the input port of the adjacent device
//!                             (ignored, if isAtOutput == true)
//!
//! @return     Physical Address of the Adjacent device,
//! @retval     0xFFFF - if the Adjacent device PA doesn't exist
//!                       (sink of a root or source of 5th level)
//-------------------------------------------------------------------------------------------------

uint16_t SiiCecAdjacentPhysAddrGet( BYTE cInstance, const uint16_t basePhysAddr, const bool_t isAtOutput, const uint8_t inputPortIndex)
{
    uint16_t mask = 0xFFF0;
    uint16_t adjPhysAddr = 0xFFFF;
    uint8_t  n = 0;

    if (inputPortIndex >= (SII_INPUT_PORT_COUNT -1 )) // illegal port index
    {
        return 0xFFFF;
    }

    // find the last non-zero digit in basePhysAddr
    while ((n < 4) && ((basePhysAddr & mask) == basePhysAddr))
    {
        n++;
        mask <<= 4;
    }

    if (isAtOutput)
    {
        if (basePhysAddr != 0x0000)
        {
            adjPhysAddr = basePhysAddr & mask;
        }
    }
    else // adjacent to input
    {
        if (n != 0) // if last digit is 0
        {
           adjPhysAddr = basePhysAddr | ((inputPortIndex/* + 1*/) << (4 * (n - 1)));
        }
    }

    return adjPhysAddr;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Returns longest possible time delay before the next invocation
//!             of the SiiCecXXXTaskProcess() function.
//!
//!             This function can be used to set a countdown timer and call
//!             the SiiCecXXXTaskProcess() on the timer's expiration event unless
//!             new CEC message is pending
//!
//! @param[in]  sysTimerCountMs  - current value of the system time counter (in milliseconds)
//! @param[in]  recallTimerCount - pointer to the recall time counter
//!
//! @return     time in milliseconds
//-------------------------------------------------------------------------------------------------

uint16_t SiiCecNextInvocationTimeGet( BYTE cInstance, uint16_t sysTimerCountMs, CecTimeCounter_t recallTimerCount)
{
    uint32_t totalDelayMs;
    uint16_t invDelayMs;

    if (CecIsTimeCounterExpired(&recallTimerCount))
    {
        totalDelayMs = 0;
    }
    else
    {
        totalDelayMs =  (uint32_t) recallTimerCount.tRov * 0x10000 /* 2^16 */ +
                        (uint32_t) recallTimerCount.tRes * CEC_TIME_TCK_MS - (uint32_t) sysTimerCountMs;
    }

    // Apply limits to recall delay
    if (totalDelayMs < CEC_MIN_RECALL_TIME_MS)
    {
        invDelayMs = CEC_MIN_RECALL_TIME_MS;
    }
    else if (totalDelayMs > CEC_MAX_RECALL_TIME_MS)
    {
        invDelayMs = CEC_MAX_RECALL_TIME_MS;
    }
    else
    {
        invDelayMs = (uint16_t) totalDelayMs;
    }

    //DEBUG_PRINT(MSG_DBG_CEC, "SiiCecNextInvocationTimeGet(): -- recall in -- %d ms\n", invDelayMs);

    return invDelayMs;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Sends a CEC message over the CEC bus using CPI
//!
//! @param[in]  opCode      - CEC operation code
//! @param[in]  argCount    - number of byte-size arguments in the CEC message
//! @param[in]  srcLa       - logical address of source device
//! @param[in]  destLa      - logical address of destination device
//! @param[in]  pCecMsg     - pointer to the CEC message frame (structure)
//-------------------------------------------------------------------------------------------------
void SiiCecCpiWriteEx( BYTE cInstance, 
    uint8_t opCode, uint8_t argCount,
    SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa,
    SiiCpiData_t *pCecMsg)
{
    pCecMsg->opcode          = (SiiCecOpcodes_t)opCode;
    pCecMsg->srcDestAddr     = MAKE_SRCDEST( srcLa, destLa );
    pCecMsg->argCount        = argCount;

    SiiDrvCpiWrite( cInstance,  pCecMsg );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Sends a CEC message over the CEC bus using CPI
//!
//! @param[in]  opCode      - CEC operation code
//! @param[in]  argCount    - number of byte-size arguments in the CEC message
//! @param[in]  destLogAddr - logical address of destination device
//! @param[in]  pCecMsg     - pointer to the CEC message frame (structure)
//-------------------------------------------------------------------------------------------------
void SiiCecCpiWrite( BYTE cInstance, uint8_t opCode, uint8_t argCount, uint8_t destLogAddr, SiiCpiData_t *pCecMsg)
{
    pCecMsg->opcode          = (SiiCecOpcodes_t)opCode;
    pCecMsg->srcDestAddr     = MAKE_SRCDEST(SiiCecGetDeviceLA( cInstance ), destLogAddr);
    pCecMsg->argCount        = argCount;

    SiiDrvCpiWrite( cInstance, pCecMsg);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Send a feature abort as a response to an inappropriate message
//!
//! @param[in]  opCode  - CEC operation code
//! @param[in]  reason  - abort reason code @see CEC_ABORT_REASON_t
//! @param[in]  srcLa   - logical address of the source device
//! @param[in]  destLa  - logical address of the destination device
//-------------------------------------------------------------------------------------------------
void SiiCecFeatureAbortSendEx ( BYTE cInstance, uint8_t opCode, uint8_t reason, SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa )
{
    SiiCpiData_t cecFrame;

    cecFrame.args[0]        = opCode;
    cecFrame.args[1]        = reason;
    cecFrame.opcode         = CECOP_FEATURE_ABORT;
    cecFrame.srcDestAddr    = MAKE_SRCDEST( srcLa, destLa );
    cecFrame.argCount       = 2;
	DEBUG_PRINT( MSG_DBG, "SiiCecFeatureAbortSendEx\r\n");

    SiiDrvCpiWrite(  cInstance, &cecFrame );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Send a feature abort as a response to an inappropriate message
//!
//! @param[in]  opCode      - CEC operation code
//! @param[in]  reason      - abort reason code @see CEC_ABORT_REASON_t
//! @param[in]  destLogAddr - logical address of the destination device
//-------------------------------------------------------------------------------------------------

void SiiCecFeatureAbortSend( BYTE cInstance, uint8_t opCode, uint8_t reason, uint8_t destLogAddr)
{
    SiiCpiData_t cecFrame;

    cecFrame.args[0]       = opCode;
    cecFrame.args[1]       = reason;

    SiiCecCpiWrite( cInstance, CECOP_FEATURE_ABORT, 2, destLogAddr, &cecFrame);
}

//[kyq Begin: 20150721 10:23:30]

//------------------------------------------------------------------------------
//! @brief  Send a  command to the specified logical
//!         device.
//! @param[in]  Command from CEC enum lists
//------------------------------------------------------------------------------
void SiiCecSendUserControlCmd(  BYTE cInstance, uint8_t opCode, /*SiiCecLogicalAddresses_t srcLa, */SiiCecLogicalAddresses_t destLa, uint8_t *cArgs,uint8_t cArgCnt  )
{
    SiiCpiData_t cecFrame;

    if ( !(pCec[cInstance])->enable )
    {
        return;
    }
    if(cArgCnt>=SII_MAX_CMD_SIZE)
	cArgCnt = SII_MAX_CMD_SIZE;
    cecFrame.opcode        = (SiiCecOpcodes_t)opCode;
    cecFrame.srcDestAddr   = MAKE_SRCDEST(  (pCec[cInstance])->logicalAddr, destLa );
    if(cArgCnt > 0)
     memcpy( &cecFrame.args,cArgs , cArgCnt );
    cecFrame.argCount      = cArgCnt;
    SiiDrvCpiWrite(  cInstance, &cecFrame );
}

void SiiCecSendUserControlCmd2(  BYTE cInstance, uint8_t opCode, BYTE cAddress, uint8_t *cArgs,uint8_t cArgCnt  )
{
    SiiCpiData_t cecFrame;

    if ( !(pCec[cInstance])->enable )
    {
        return;
    }
    if(cArgCnt>=SII_MAX_CMD_SIZE)
	cArgCnt = SII_MAX_CMD_SIZE;
    cecFrame.opcode        = (SiiCecOpcodes_t)opCode;
    cecFrame.srcDestAddr   = cAddress;//MAKE_SRCDEST(  srcLa, destLa );
    //Uart_printf("22 srcDestAddr==0x%02x\n",cecFrame.srcDestAddr);
    if(cArgCnt > 0)
     memcpy( &cecFrame.args,cArgs , cArgCnt );
    cecFrame.argCount      = cArgCnt;
    SiiDrvCpiWrite(  cInstance, &cecFrame );
}




//[kyq End: 20150721 10:23:32]
