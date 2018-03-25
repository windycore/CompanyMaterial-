//***************************************************************************
//!file     sk_app_cec.c
//!brief    Wraps board and device functions for the CEC component
//!         and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_app_cec.h"
#if INC_CBUS
#include "sk_app_cbus.h"
#endif
#include "si_edid_tx_component.h"
#include "si_cec_internal.h"
#include "si_cec_switch_api.h"
#include "utilDataMgrAPI.h"
#include "appSystemMgrAPI.h"
static uint8_t  l_sinkCecTypeList[] = { 0, CEC_LOGADDR_UNREGORBC };
static uint8_t  l_allCecTypeList[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, CEC_LOGADDR_UNREGORBC };
uint8_t  appCecSourceLaList[] = { CEC_LOGADDR_PLAYBACK1/*CEC_LOGADDR_AUDSYS*//*, CEC_LOGADDR_PLAYBACK2, CEC_LOGADDR_PLAYBACK3, CEC_LOGADDR_FREEUSE*/, CEC_LOGADDR_UNREGORBC,
		CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC,
		CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC, CEC_LOGADDR_UNREGORBC };

static char equalSymbolLine [] = "**============================**\n";

//follow psSystemSettingData->acProduct_Name   ,  acProduct_Name  can chang.
static char l_cecAvrOsdNameString [17] ;
//static char l_cecAuxOsdNameString [] = "SII AUX";

SiiAppCecInstanceData_t appCec[DevNum][SII_NUM_CPI];
SiiAppCecInstanceData_t *pAppCec[DevNum] = {
  &appCec[0][0],
  	
#if(DevNum>1)
  &appCec[1][0],
#endif
#if(DevNum>2)
  &appCec[2][0],
#endif

#if(DevNum>3)
  &appCec[3][0],
#endif

#if(DevNum>4)
  &appCec[4][0],
#endif

#if(DevNum>5)
  &appCec[5][0],
#endif

#if(DevNum>6)
  &appCec[6][0],
#endif

#if(DevNum>7)
  &appCec[7][0],
#endif


};

//------------------------------------------------------------------------------
//! @brief      Set the correct instance for each of the CEC-related components
//!             and drivers associated with the passed CEC instance.
//! @param[in]  newInstance - new instnce index.
//------------------------------------------------------------------------------
void SkAppCecInstanceSet (BYTE cInstance, int_t newInstance )
{
    // In this case, the instances are one-to-one, but it is
    // possible for a mismatch if, for example, you have one
    // CDC instance that is associated with CPI instance 1
    // instead of instance 0.

    if ( newInstance < SII_NUM_CPI )
    {
        pAppCec[cInstance] = &appCec[cInstance][newInstance];
        (pAppCec[cInstance] )->instanceIndex = newInstance;
        SiiCecInstanceSet( cInstance, newInstance );
#if INC_CDC
        SiiCdcInstanceSet(cInstance, CDC_INSTANCE_RX );
#endif
        SiiDrvCpiInstanceSet( cInstance,newInstance );
    }
}

//------------------------------------------------------------------------------
// Function:    SkAppCecStandby
//! @brief      Places all CEC components/drivers into standby
// Returns:     true if the CEC component entered standby mode successfully,
//              or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SkAppCecStandby (  BYTE cInstance )
{
    // If CEC is not enabled by the user, we're done.

    if ( !app[cInstance].cecEnable )
    {
        return( true );
    }

    // Tell CEC that system power is going off
    SkAppCecInstanceSet( cInstance, CEC_INSTANCE_AVR );
    SiiCecSetPowerState( cInstance, (SiiCecPowerstatus_t)APP_POWERSTATUS_ON_TO_STANDBY );

    // If we're a TV, tell all attached CEC devices to power off
    if ( SiiCecGetDeviceType(cInstance) == CEC_DT_TV )
    {
        SiiCecSendMessage( cInstance, CECOP_STANDBY, CEC_LOGADDR_UNREGORBC );
    }
    SiiCecStandby(cInstance);

    return( true );
}

//------------------------------------------------------------------------------
//! @brief      Restore operation to all enabled CEC components/drivers.
//! @param[in]  powerIsOn   if true, set CEC power state to ON, otherwise the
//!                         component is operational but does not tell any
//!                         other CEC devices that it system power is on.
//! @return     true if the CEC component successfully exited standby mode,
//!             or false if some failure occurred.
//------------------------------------------------------------------------------
bool_t SkAppCecResume ( BYTE cInstance, bool_t powerIsOn )
{
    // If CEC is not enabled by the user, we're done.
    if ( !app[cInstance].cecEnable )
    {
        return( true );
    }
    DEBUG_PRINT(MSG_ALWAYS, "--------- SkAppCecResume() ------------\n");
    SkAppCecInstanceSet(cInstance, CEC_INSTANCE_AVR );
    SiiCecResume(cInstance);
    SiiCecEnumerateDevices(cInstance,   appCecSourceLaList );
    SiiCecEnumerateDeviceLa(cInstance, appCecSourceLaList );
    if ( powerIsOn )
    {
        SiiCecSetPowerState(cInstance, (SiiCecPowerstatus_t)APP_POWERSTATUS_STANDBY_TO_ON );
    }

    // Queue-up the enumerate tasks
    if (SiiCecGetDeviceType(cInstance) == CEC_DT_TV)
    {
        SiiCecEnumerateDevices( cInstance,  l_allCecTypeList );
        SiiCecEnumerateDeviceLa(cInstance, l_sinkCecTypeList );
    }

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SkAppCecConfigure
// Description:
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

void SkAppCecConfigure ( BYTE cInstance,  bool_t enable )
{
    // If CEC enable state has changed, tell the component.

    if ( app[cInstance].oldCecEnable != enable )
    {
        app[cInstance].cecEnable     = enable;
        app[cInstance].oldCecEnable  = enable;
        SkAppCecInstanceSet( cInstance,CEC_INSTANCE_AVR );
        SiiCecConfigure( cInstance,enable );
        PrintStringOnOff( "CEC:", app[cInstance].cecEnable );
    }
}

//------------------------------------------------------------------------------
// Function:    SkAppCecRxMsgHandler
// Description: Parse received messages and execute response as necessary
//              Only handle the messages needed at the top level to interact
//              with the Port Switch hardware.  The SiiAPI message handler
//              handles all messages not handled here.
// Parameters:  none
// Returns:     Returns true if message was processed by this handler
//------------------------------------------------------------------------------

bool_t SkAppCecRxMsgHandler (  BYTE cInstance, SiiCpiData_t *pMsg )
{
    bool_t  processedMsg, isDirectAddressed;

    isDirectAddressed = (bool_t)!((pMsg->srcDestAddr & 0x0F ) == CEC_LOGADDR_UNREGORBC );

    processedMsg = false;
    switch ( pMsg->opcode )
    {
        case CECOP_IMAGE_VIEW_ON:       // In our case, respond the same to both these messages
        case CECOP_TEXT_VIEW_ON:
            if ( !isDirectAddressed )   break;          // Ignore as broadcast message

            // The CEC Component "First Message Handler" has already seen this message
            // and turned on the system power status (as necessary).  Now, the application
            // must do whatever else needs to be done to turn on the display.

            //TODO:OEM - Do whatever else is required to turn on the TV display.

            DEBUG_PRINT( MSG_STAT, equalSymbolLine );
            DEBUG_PRINT( MSG_STAT, "**     IMAGE/TEXT VIEW ON     **\n");
            DEBUG_PRINT( MSG_STAT, equalSymbolLine );
            processedMsg = true;
            break;

        case CECOP_USER_CONTROL_PRESSED:
        case CECOP_USER_CONTROL_RELEASED:
            if ( !isDirectAddressed )   break;          // Ignore as broadcast message

#if INC_CBUS
//            SkAppCbusReceiveCec( pMsg );
#endif
            processedMsg = true;
            break;

        case CECOP_INACTIVE_SOURCE:
            if ( !isDirectAddressed )   break;          // Ignore as broadcast message

            DEBUG_PRINT( MSG_STAT, equalSymbolLine );
            DEBUG_PRINT( MSG_STAT, "** SWITCH TO INTERNAL SOURCE  **\n");
            DEBUG_PRINT( MSG_STAT, equalSymbolLine );
            processedMsg = true;
            break;
        default:
            break;
    }

    return( processedMsg );
}

//------------------------------------------------------------------------------
// Function:    CecTimerSet
// Description: Elapsed timers for CEC use
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

static void CecTimerSet (  BYTE cInstance, clock_time_t msDelay  )
{
    app[cInstance].msCecDelay = msDelay;
    app[cInstance].msCecStart 	= SiiOsTimerTotalElapsed();
}
static bool_t CecTimerElapsed (  BYTE cInstance  )
{
    clock_time_t thisTime = SiiOsTimerTotalElapsed();

    return((bool_t)(( thisTime - app[cInstance].msCecStart) >= app[cInstance].msCecDelay));
}

//------------------------------------------------------------------------------
// Function:    SkAppTaskCec
// Description: CEC control task.  Emulate a task with a blocking semaphore
//              waiting for a device interrupt or a timer-based poll request
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

void SkAppTaskCec (  BYTE cInstance )
{
    bool_t      	cecProcessed = false;
    uint16_t    	cecComponentStatus;

    if ( app[cInstance].cecEnable == false )
    {
        return;
    }

    // If a CEC interrupt occurred, call the CEC handler.

    if ( app[cInstance].cecInterruptRx || app[cInstance].cecInterruptTx ||CecTimerElapsed( cInstance ))
    {
        SkAppCecInstanceSet( cInstance, CEC_INSTANCE_AVR );
        SiiCecHandler(cInstance);
        cecProcessed = true;

        // This instance is the Switch side, so check for a port change.

	 //DEBUG_PRINT( MSG_STAT, "** SiiCecHandler  **\r\n");
        cecComponentStatus = SiiCecStatus(cInstance);    // Only get one chance at these
        if ( cecComponentStatus & SiiCEC_PORT_CHANGE )
        {
            app[cInstance].newSource[app[cInstance].currentZone ] = (SiiSwitchSource_t)SiiCecPortSelectGet(cInstance);
        }

        if ( cecComponentStatus & SiiCEC_POWERSTATE_CHANGE )
        {
            app[cInstance].powerState = (SiiAppPowerStatus_t)SiiCecGetPowerState(cInstance);
        }
        app[cInstance].cecInterruptRx = false;
        app[cInstance].cecInterruptTx = false;
    }

    if ( cecProcessed )
    {
        CecTimerSet(cInstance,mS_TO_TICKS(100));//( 70 );     // 100ms before next poll  //[kyq Begin: 20150717 11:56:42]
    }
}

//------------------------------------------------------------------------------
//! @brief  Initializes CPI and CEC hardware for both Rx and Tx.
//!         The hardware is initialized and message handlers registered,
//!         but the hardware is left in a disabled state.  The hardware (and
//!         thus CEC support) is enabled/disabled via the SkAppCecConfigure
//!         function.
//
//! @note   TX Instance CPI initialization MUST be done after the TPI driver
//!         has been initialized.
//------------------------------------------------------------------------------
bool_t SkAppDeviceInitCec (  BYTE cInstance )
{
    uint16_t    cecPa;
    bool_t      success = false;

    do
    {
        //**************************************************************************************
        // Configure/Initialize the RX CEC/CPI as an AUDIO System with SWITCH
        //**************************************************************************************

        SkAppCecInstanceSet( cInstance,CEC_INSTANCE_AVR );
        if ( !SiiDrvCpiInitialize(cInstance))    break;  // ÉèÖÃCPI µÄÂß¼­µØÖ·
#if INC_CEC_LOGGER  
        if ( !SiiCpiRegisterLogger(cInstance, SkAppCecMessageLogger ))
        {
            DEBUG_PRINT( MSG_ERR, "Unable to register RX CPI logger\n");
            break;
        }
#endif

        SiiEdidTxInstanceSet(cInstance, 0);
        SkAppTxInstanceSet(cInstance, 0);
        // Can't initialize if valid physical address not assigned
        cecPa = SiiEdidTxCecPhysAddrGet(cInstance);
        if (cecPa == 0xFFFF)
			break;
        if ( !SiiCecInitialize( cInstance, cecPa, CEC_DT_PLAYBACK ))
			break;
        
        #if 1
        PSYSTEM_SETTING_DATA  psSystemSettingData;
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        memcpy(l_cecAvrOsdNameString,psSystemSettingData->acProduct_Name, strlen(psSystemSettingData->acProduct_Name));
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
        #endif
        SiiCecSetOsdName(cInstance, l_cecAvrOsdNameString );
        
        DEBUG_PRINT( MSG_DBG, "SiiCecInitialize finish\r\n");
	  
#if INC_CEC_SWITCH
        if(app[cInstance,].isCecAsSwitch)
        {
        	// Initialize CEC Switch component and enable its base service
        	SiiCecSwitchConfig(cInstance);
        	if ( !SiiCecCallbackRegisterParser(cInstance, SiiCecSwitchTaskProcess, false ))            break;
        	if ( SiiCecSwitchSrvStart(cInstance))                                                    break;
        }
#else
        if(!SiiCecCallbackRegisterParser( cInstance,CecRxMsgHandler, false )) break;
#endif

#if INC_CEC_SAC
        // Initialize CEC System Audio Control component and enable its base service
        // Can't initialize SAC if valid physical address not assigned
        SiiCecSacConfig(cInstance,0, NULL, 0); // take default format list
        if ( !SiiCecCallbackRegisterParser( cInstance,SiiCecSacTaskProcess, false ))               break;
        if ( SiiCecSacSrvStart(cInstance))                                                       break;
#endif

#if INC_CBUS
        // Add in a CEC message parser for CBUS to filter User Key Press and Release
        // We add this extra parser AFTER the SAC parser to allow the SAC to use
        // some of the CECOP_USER_CONTROL_PRESSED and CECOP_USER_CONTROL_RELEASED commands
        // if it needs them. It does not have to be called every time by the CEC handler
        // because it runs no tasks.
        if ( !SiiCecCallbackRegisterParser(cInstance, SkAppCbusCecRxMsgFilter, false ))           break;
#endif
   		DEBUG_PRINT( MSG_DBG, "SkAppDeviceInitCec--success\r\n");
        success = true;
    }   while (0);  // Do the above only once

    SkAppCecConfigure ( cInstance, true);
	return( success );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Informs CEC system about the input port change by user action
//!
//!             This function shall be called by the system if user changes
//!             input port selection
//!
//! @param[in]  newInputPortIndex - zero based input port index
//-------------------------------------------------------------------------------------------------

void SiiCecAppInputPortChangeHandler( BYTE cInstance, uint8_t newInputPortIndex)
{
    SiiCecSwitchSources(cInstance, newInputPortIndex );
#if (INC_CEC_SWITCH == 1)
    SiiCecRoutingChangeSend(cInstance, newInputPortIndex);
#endif
}

//-------------------------------------------------------------------------------------------------
//! @brief      CEC Callback reporting ACK/NOACK from SiiCecSendMessageTask() function.
//!
//! @param[in]  opCode          - Opcode of message being reported
//! @param[in]  messageAcked    - true if message was ACKed
//-------------------------------------------------------------------------------------------------
void SiiCecCbSendMessage( uint8_t opCode, bool_t messageAcked )
{

   DEBUG_PRINT2( MSG_DBG, "CEC Message %02X was %s\r\n", opCode, messageAcked ? "ACKed" : "Not ACKed");
}

//-------------------------------------------------------------------------------------------------
//! @brief      CEC Callback requesting to switch input port.
//!
//! @param[in]  inputPortIndex - input port to switch to
//! @return     success flag.
//-------------------------------------------------------------------------------------------------

bool_t SiiCecCbInputPortSet( BYTE cInstance, uint8_t inputPortIndex)
{
    app[cInstance].newSource[app[cInstance].currentZone] = (SiiSwitchSource_t)inputPortIndex;
    SiiCecPortSelectSet(cInstance, inputPortIndex);
    return true;
}

//-------------------------------------------------------------------------------------------------
//! @brief      CEC Callback requesting standby mode of this device.
//!
//! @param[in]  inputPortIndex - input port to switch to
//! @return     sucess flag.
//-------------------------------------------------------------------------------------------------

void SiiCbCecStandby( BYTE cInstance, bool_t standby)
{
    if ( standby && (app[cInstance].powerState == APP_POWERSTATUS_ON ))
    {
        app[cInstance].powerState = APP_POWERSTATUS_ON_TO_STANDBY;
    }
    else if (!standby && (app[cInstance].powerState != APP_POWERSTATUS_ON))
    {
        app[cInstance].powerState = APP_POWERSTATUS_STANDBY_TO_ON;
    }
}

#if (INC_CEC_SWITCH == 1)
//-------------------------------------------------------------------------------------------------
//! @brief      CEC Callback requesting HPD status of selected input port.
//!
//! @return     HPD On status.
//-------------------------------------------------------------------------------------------------

bool_t SiiCbCecSwitchRxHpdGet( BYTE cInstance)
{

    return( (bool_t)SiiDrvSwitchStateGet(cInstance, SiiSwitch_SELECTED_INPUT_CONNECT_STATE));
}

//-------------------------------------------------------------------------------------------------
//! @brief      CEC Callback processing messages from CEC/Switch sub-system.
//!
//! @param[in]  fbMsg - message code
//!
//! @retval     true  - if the message was recognized and processed
//! @retval     false - if the message is irrelevant and wasn't processed
//-------------------------------------------------------------------------------------------------
bool_t SiiCbCecSwitchMessageHandler( BYTE cInstance, uint8_t fbMsg)
{
    bool_t   usedMessage = true;

    /* Process feedback messages.       */
    switch (fbMsg)
    {
        default:
        case CEC_SWITCH_FB_MSG_NONE:
            // nothing to do
            break;

        case CEC_SWITCH_FB_MSG_STATUS_CHANGED:

            // This message occurs when the switch processes a SET_STREAM_PATH
            // or ACTIVE_SOURCE message, which must also be passed to subsequent
            // handlers, so mark message as unused.  They will not feature abort
            // if nothing else uses it, because they are broadcast messages.
            usedMessage = false;
            break;

        case CEC_SWITCH_FB_MSG_SEND_ROUTE_DONE:
           // DEBUG_PRINT(MSG_STAT, "CEC Switch send route task finished.\n");
            break;

        case CEC_SWITCH_FB_MSG_ERR_NONSWITCH_CMD:
             usedMessage = false;
             break;
    }

    return( usedMessage );
}
#endif
#if INC_CEC
//-------------------------------------------------------------------------------------------------
//! @brief      CEC Callback updating enumeration task status
//!
//! @param[in]  isComplete  - true if enumeration task has completed
//-------------------------------------------------------------------------------------------------
void SiiCecCbEnumerateComplete(  BYTE cInstance, bool_t isComplete )
{

    (pAppCec[cInstance] )->isEnumerated = isComplete;

#if INC_CBUS
    // May need to assign an available LA to a CBUS device
    SkAppCbusAssignCecDeviceAddress( cInstance, isComplete );
#endif
}

#endif
