//****************************************************************************
//! @file   si_cec_switch_api.c
//! @brief  Top & low level CEC / Switch API
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_common.h"
#include "si_cec_internal.h"
#include "si_cec_switch_api.h"


#ifndef CEC_PC_SIMULATION
#include <string.h>
#else
#include "stdio.h"
#include "memory.h"
#include "debug_print.h"
#endif

//-------------------------------------------------------------------------------------------------
// Global Data
//-------------------------------------------------------------------------------------------------
CecSwitchInstanceRecord_t gSwitchInstRec[INSTANCE_9533][SII_NUM_CEC_SWITCH];       //!< global CEC/Switch instance record
CecSwitchInstanceRecord_t *pCecSwitch[INSTANCE_9533] =
{
    &gSwitchInstRec[0][0],
#if(INSTANCE_9533>1)
    &gSwitchInstRec[1][0],
#endif
#if(INSTANCE_9533>2)
    &gSwitchInstRec[2][0],
#endif
#if(INSTANCE_9533>3)
    &gSwitchInstRec[3][0],
#endif
#if(INSTANCE_9533>4)
    &gSwitchInstRec[4][0],
#endif
#if(INSTANCE_9533>5)
    &gSwitchInstRec[5][0],
#endif
#if(INSTANCE_9533>6)
    &gSwitchInstRec[6][0],
#endif
#if(INSTANCE_9533>7)
    &gSwitchInstRec[7][0],
#endif


};   //!< global pointer to CEC/Switch Instance Record


//-------------------------------------------------------------------------------------------------
// Local Functions
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//! @brief      Sends "CEC Report Physical Address" broadcast message to
//!             announce own physical address
//-------------------------------------------------------------------------------------------------

static void CecPhysicalAddressReportSend( BYTE cInstance)
{
    SiiCpiData_t cecFrame;
    /* Respond by broadcasting Physical Address */
    cecFrame.args[0]  = ((pCec[cInstance])->physicalAddr >> 8) & 0xFF; // Physical Address, MSB
    cecFrame.args[1]  = (pCec[cInstance])->physicalAddr & 0xFF;        // Physical Address, LSB
    cecFrame.args[2]  = (pCec[cInstance])->deviceType;                 // Device Type
    SiiCecCpiWrite( cInstance, CECOP_REPORT_PHYSICAL_ADDRESS, 3, CEC_LOGADDR_UNREGORBC, &cecFrame);
}


//-------------------------------------------------------------------------------------------------
// Top Level API Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Brings device instance record into current context. This function
//!             is useful when multiple HDMI outputs are present
//! @param[in]  deviceIndex - index of the device in case of multiple instantiation
//-------------------------------------------------------------------------------------------------*/

void SiiCecSwitchInstanceSet( BYTE cInstance, uint16_t deviceIndex)
{
    if( deviceIndex < SII_NUM_CEC_SWITCH)
    {
        (pCecSwitch[cInstance]) = &gSwitchInstRec[cInstance][deviceIndex];
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Returns pointer to current device CEC/Switch instance record
//-------------------------------------------------------------------------------------------------

CecSwitchInstanceRecord_t *SiiCecSwitchDeviceInstanceGet( BYTE cInstance)
{
    return (pCecSwitch[cInstance]);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Initializes current CEC/Switch Instance Record
//-------------------------------------------------------------------------------------------------

static void CecSwitchInstanceRecordReset( BYTE cInstance)
{
    memset((pCecSwitch[cInstance]), 0, sizeof(CecSwitchInstanceRecord_t)); // erase all data
}

//-------------------------------------------------------------------------------------------------
//! @brief      Initializes the CEC/Switch Instance Record and fills its fields related to
//!             device capabilities and non-volatile configuration.
//-------------------------------------------------------------------------------------------------

void SiiCecSwitchConfig( BYTE cInstance)
{
    CecSwitchInstanceRecordReset();    // Initialize all Instance Record fields
    // Initialize current input port index
    (pCecSwitch[cInstance])->activeInputPortIndex = SiiCecPortSelectGet();
    SiiCecSwitchTaskInit();
}


//-------------------------------------------------------------------------------------------------
//! @brief      Resets all tasks
//-------------------------------------------------------------------------------------------------

void SiiCecSwitchTaskInit( BYTE cInstance)
{
    (pCecSwitch[cInstance])->taskFlags = 0;
    (pCecSwitch[cInstance])->taskMask  = CEC_SWITCH_TASK_MASK_ALL_UNMASKED; // all tasks are unmasked by default
    // Reset time counters
    CecTimeCounterReset(&(pCecSwitch[cInstance])->taskTimeCount);
    // Reset task states
    (pCecSwitch[cInstance])->taskState = CEC_SWITCH_TASK_EMPTY_STATE_NONE;
    (pCecSwitch[cInstance])->sysTimerCountCurMs = 0;
    (pCecSwitch[cInstance])->sysTimerCountPrvMs = 0;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Stops given task
//!             For internal use in the last stage of a task
//-------------------------------------------------------------------------------------------------

static void StopTask( BYTE cInstance, uint16_t task)
{
    (pCecSwitch[cInstance])->taskFlags &= ~task; // clear the task activity flag
    (pCecSwitch[cInstance])->taskState = CEC_SWITCH_TASK_EMPTY_STATE_NONE;  // reset stage
    (pCecSwitch[cInstance])->taskMask  = CEC_SWITCH_TASK_MASK_ALL_UNMASKED;  // reset task mask as this task is done
}


//-------------------------------------------------------------------------------------------------
//! @brief      Standard check for the Task Timer event
//!             Including the Recall Timer update
//-------------------------------------------------------------------------------------------------

static bool_t IsUpdatedTaskTimerExpired( BYTE cInstance)
{
    bool_t isExpired;
    // Set recall timer to maximum in order to allow the task counters to set it to a new value
    CecTimeCounterSet(CEC_TIME_MS2TCK(CEC_MAX_RECALL_TIME_MS), &(pCecSwitch[cInstance])->recallTimeCount);
    isExpired = CecTimeCounterUpdate(&(pCecSwitch[cInstance])->taskTimeCount, &(pCecSwitch[cInstance])->recallTimeCount);
    return isExpired;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Sends Routing information (own physical address) after HPD stabilization
//!             on upstream side
//!
//!             The task starts with ensuring that upstream HPD is high (input connection is active).
//!             Then it will wait for CEC_SWITCH_ROUTE_INFO_DELAY_MS as many times as it takes for
//!             upstream HPD to become high again. Finally, the switch will broadcast it's
//!             physical address.
//!
//! @return     Feedback message to the caller (Task status or error reports)
//-------------------------------------------------------------------------------------------------

static uint8_t CecSwitchTaskRouteInfo( BYTE cInstance)
{
    uint8_t switchFeedbackMsg = CEC_SWITCH_FB_MSG_NONE;
    switch((pCecSwitch[cInstance])->taskState)
    {
    case CEC_SWITCH_TASK_EMPTY_STATE_NONE: // all tasks shall start from empty state
        (pCecSwitch[cInstance])->taskMask = CEC_SWITCH_TASK_SEND_ROUTE_INFO; // mask out other tasks until this one is done
        (pCecSwitch[cInstance])->taskState = CEC_SWITCH_TASK_SEND_ROUTE_INFO_STATE_PAUSE;
        // don't break here to save time and go ahead to the fist step
    case CEC_SWITCH_TASK_SEND_ROUTE_INFO_STATE_CHECK_HPD: // Stay here until upstream HPD is high
        // Check upstream HPD
        if (SiiCbCecSwitchRxHpdGet( cInstance ) == true) // HPD high
        {
            // Set the pause timer
            CecTimeCounterSet(CEC_TIME_MS2TCK(CEC_SWITCH_ROUTE_INFO_DELAY_MS), &(pCecSwitch[cInstance])->taskTimeCount);
            // Jump to the next state
            (pCecSwitch[cInstance])->taskState = CEC_SWITCH_TASK_SEND_ROUTE_INFO_STATE_PAUSE;
        }
        // If HPD is low, will come back to this state again
        break;
    case CEC_SWITCH_TASK_SEND_ROUTE_INFO_STATE_PAUSE: // Take a pause before sending new route
        if (IsUpdatedTaskTimerExpired())
        {
            // if timer expired
            if (SiiCbCecSwitchRxHpdGet( cInstance )) // HPD high
            {
                // Report own physical address
                CecPhysicalAddressReportSend( cInstance );
                DEBUG_PRINT(CEC_MSG_DBG, "[CEC_SWITCH_TASK_SEND_ROUTE_INFO]: Broadcasted physical address after delay\n");
                // and finalize the task
                (pCecSwitch[cInstance])->taskState = CEC_SWITCH_TASK_SEND_ROUTE_INFO_STATE_REPORT_RESULTS;
            }
            else // HPD still low
            {
                // Set the timer again and repeat the pause
                CecTimeCounterSet(CEC_TIME_MS2TCK(CEC_SWITCH_ROUTE_INFO_DELAY_MS), &(pCecSwitch[cInstance])->taskTimeCount);
            }
        }
        break;
    case CEC_SWITCH_TASK_SEND_ROUTE_INFO_STATE_REPORT_RESULTS: // Last stage
        // event message to the caller
        switchFeedbackMsg = CEC_SWITCH_FB_MSG_SEND_ROUTE_DONE;
        StopTask(CEC_SWITCH_TASK_SEND_ROUTE_INFO);
        break;
    }
    return  switchFeedbackMsg;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Internal error reporting service for CecSwitchTaskBase()
//-------------------------------------------------------------------------------------------------

static void CecSwitchBadCmdReport(char *str)
{
    DEBUG_PRINT(CEC_MSG_DBG, "[CEC_SW]: ERROR in <%s> message: wrong length!\n", str);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Calculates input port index by given physical address
//!
//! @return     zero based input port index
//! @retval     0xFF - if physical address doesn't correspond to any of the switch inputs
//-------------------------------------------------------------------------------------------------
static uint8_t CecSwitchInputPortGet( uint16_t physAddr )
{
    uint8_t     nmb = 0; // input number (0 means "not applicable")
    uint16_t    pa = SiiCecGetDevicePA();
    /*    if ( pa == 0x0000 )
        {
            nmb = (uint8_t)(physAddr >> 15);
        }
        else if ( pa & 0xF000 )
        {
            nmb = (uint8_t)(physAddr >> 12);
        }
        else */
    if ( pa & 0xF000 )
    {
        nmb = (uint8_t)((physAddr & 0x0F00) >> 8) + 1;
    }
    /*else if ( pa & 0x00F0 )
    {
        nmb = (uint8_t)(physAddr >> 4);
    }
    else if ( pa & 0x000F )
    {
        nmb = (uint8_t)physAddr ;
    }*/
    if (nmb > 0)
    {
        return((nmb & 0x0F) - 1);   // return zero-based port index
    }
    else
    {
        return( 0xFF );             // invalid PA
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Performs CEC/Switch message parsing and support basic Switch functionality
//!             This task can't be masked and will run in background of the
//!             other active tasks, if an incoming CEC message is pending.
//
//! @param[in]  pCecMessage - pointer to a pending CEC message or 0, if there is no message awaiting
//
//! @return     Feedback message to the caller (Task status or error reports)
//-------------------------------------------------------------------------------------------------

static uint8_t CecSwitchTaskBase( BYTE cInstance, SiiCpiData_t *pCecMessage)
{
    uint8_t     feedbackMsg = CEC_SWITCH_FB_MSG_NONE;
    uint8_t     senderLogAddr;
    uint8_t     destLogAddr;
    uint16_t    newPhysAddr;
    uint8_t     inputPortIndex;
    SiiCpiData_t cecFrame;
    bool_t  isDirectAddressed;
    isDirectAddressed   = !((pCecMessage->srcDestAddr & 0x0F ) == CEC_LOGADDR_UNREGORBC );
    if (pCecMessage != 0) //decode a message, update status
    {
        senderLogAddr = (pCecMessage->srcDestAddr & 0xF0) >> 4; // sender's logical address
        destLogAddr   =  pCecMessage->srcDestAddr & 0x0F; // destination logical address
        if (destLogAddr == 0xF) // ignore non-broadcast messages
        {
            switch (pCecMessage->opcode) // CEC operation code
            {
            case CECOP_STANDBY:
                if (pCecMessage->argCount == 0)
                {
                    DEBUG_PRINT(MSG_STAT, "[CEC_SW]: Standby mode On Request\n");
                    SiiCbCecStandby( cInstance, true);
                }
                else
                {
                    CecSwitchBadCmdReport("Standby");
                }
                break;
            case CECOP_ROUTING_CHANGE:
                if (pCecMessage->argCount == 4)
                {
                    newPhysAddr = (pCecMessage->args[2] << 8) + pCecMessage->args[3];
                    if (SiiCecGetDevicePA() == newPhysAddr)
                    {
                        DEBUG_PRINT(MSG_STAT, "[CEC_SW]: Broadcast Route Info in response to Routing Change\n");
                        SiiCecRoutingInformationSend();
                    }
                }
                else
                {
                    CecSwitchBadCmdReport("Routing Change");
                }
                break;
            case CECOP_ROUTING_INFORMATION:
                if (pCecMessage->argCount == 2)
                {
                    newPhysAddr = (pCecMessage->args[0] << 8) + pCecMessage->args[1];
                    if (SiiCecGetDevicePA() == newPhysAddr)
                    {
                        DEBUG_PRINT(MSG_STAT, "[CEC_SW]: Broadcast Route Info in response to Routing Information\n");
                        SiiCecRoutingInformationSend();
                    }
                }
                else
                {
                    CecSwitchBadCmdReport("Routing Information");
                }
                break;
            case CECOP_ACTIVE_SOURCE:
                if (pCecMessage->argCount != 2)
                {
                    CecSwitchBadCmdReport("Active Source");
                }
                if ( !isDirectAddressed && (pCecMessage->srcDestAddr & 0xF0))                   // Ignore as direct message
                {
                    /*                      DEBUG_PRINT(MSG_ALWAYS,("ACTIVE SOURCE BEING ADDRESSED\n"));
                    (pCec[cInstance])->activeSrcLogical  = (pCecMessage->srcDestAddr >> 4) & 0x0F;
                    (pCec[cInstance])->activeSrcPhysical = ((uint16_t)pCecMessage->args[0] << 8 ) | pCecMessage->args[1];
                    SiiCecUpdateLogicalDeviceInfo( (pCec[cInstance])->activeSrcLogical, (pCec[cInstance])->activeSrcPhysical, true );

                    // Determine the index of the HDMI port that is handling this physical address.

                    (pCec[cInstance])->portSelect = (((pCec[cInstance])->activeSrcPhysical >> 8) & 0x000F);
                    (pCec[cInstance])->statusFlags |= SiiCEC_PORT_CHANGE;
                    }
                    break;*/
                    // Update CEC records (LA & PA) about the active source
                    SiiCecSetActiveSource( cInstance, (pCecMessage->srcDestAddr >> 4) & 0x0F,
                                           ((uint16_t) pCecMessage->args[0] << 8) | pCecMessage->args[1]);
                    (pCec[cInstance])->portSelect = (((((uint16_t) pCecMessage->args[0] << 8) | pCecMessage->args[1]) >> 8) & 0x000F);
                    (pCec[cInstance])->statusFlags |= SiiCEC_PORT_CHANGE;
                }
                break;
                // Process this message further the same way as the next one, so no break here
            case CECOP_SET_STREAM_PATH:
                if(pCecMessage->argCount == 2)
                {
                    newPhysAddr = (pCecMessage->args[0] << 8) | pCecMessage->args[1];
                    inputPortIndex = CecSwitchInputPortGet(newPhysAddr);
                    if(inputPortIndex != 0xFF)
                    {
                        DEBUG_PRINT(MSG_STAT, "[CEC_SW]: received Active Source or Set Stream Path msg.\n");
                        DEBUG_PRINT(MSG_STAT, "[CEC_SW]: Standby mode Off Request\n");
                        SiiCbCecStandby( cInstance, false);
                        if (SiiCecCbInputPortSet( cInstance, inputPortIndex))
                        {
                            (pCec[cInstance])->statusFlags |= SiiCEC_PORT_CHANGE;
                            /*(pCec[cInstance])->activeSrcLogical  = (pCecMessage->srcDestAddr >> 4) & 0x0F;
                            (pCec[cInstance])->activeSrcPhysical = newPhysAddr;

                            SiiCecUpdateLogicalDeviceInfo( (pCec[cInstance])->activeSrcLogical, (pCec[cInstance])->activeSrcPhysical, true );*/
                            DEBUG_PRINT(MSG_STAT, "[CEC_SW]: Switched to input port %d\n", (int) inputPortIndex);
                            cecFrame.opcode         = CECOP_ACTIVE_SOURCE;
                            cecFrame.srcDestAddr    = MAKE_SRCDEST( (pCec[cInstance])->logicalAddr, CEC_LOGADDR_UNREGORBC );
                            cecFrame.args[0]        = (newPhysAddr >> 8);        // [Physical Address] High
                            cecFrame.args[1]        = newPhysAddr & 0xFF;      // [Physical Address] Low
                            cecFrame.argCount       = 2;
                            SiiDrvCpiWrite( cInstance,  &cecFrame );
                            (pCec[cInstance])->statusFlags |= SiiCEC_PORT_CHANGE;
                        }
                        else
                        {
                            DEBUG_PRINT(MSG_STAT, "[CEC_SW]: Don't need to switch to port %d\n", (int) inputPortIndex);
                        }
                    }
                    feedbackMsg = CEC_SWITCH_FB_MSG_STATUS_CHANGED;
                }
                else
                {
                    CecSwitchBadCmdReport("Set Stream Path");
                }
                break;
            default:  // the message doesn't belong to CEC/Switch group
                feedbackMsg = CEC_SWITCH_FB_MSG_ERR_NONSWITCH_CMD;
            }
        }
        else
        {
            feedbackMsg = CEC_SWITCH_FB_MSG_ERR_NONSWITCH_CMD;
        }
        /*
                if (feedbackMsg == CEC_SWITCH_FB_MSG_ERR_NONSWITCH_CMD)
                {
                    DEBUG_PRINT(CEC_MSG_DBG, "[CEC_SW]: Non CEC Switch message\n");
                }
        */
    }
    return feedbackMsg;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Task dispatching function.
//
//!             Shall be called periodically.
//!             Using of SiiCecSwitchNextInvocationTimeGet() helps to figure out
//!             exact recall time that can vary depending on current task status.
//!             Alternatively, simple periodic polling would work as well;
//!             in the case of polling, CEC_MIN_RECALL_TIME_MS is a suggested
//!             recall time.
//
//! @param[in]  pCecMessage   - pointer to a pending CEC message or 0, if there is no message awaiting
//
//! @return     Feedback message to the caller (Task status or error reports)
//-------------------------------------------------------------------------------------------------

bool_t SiiCecSwitchTaskProcess( BYTE cInstance, SiiCpiData_t *pCecMessage)
{
    uint8_t  feedbackMsg = CEC_SWITCH_FB_MSG_NONE;
    uint16_t taskFlagsMasked;
    uint16_t sysTimerCountMs = SiiOsTimerTotalElapsed();
    // Keep all tasks frozen if the Base task is off
    if ((pCecSwitch[cInstance])->taskFlags & CEC_SWITCH_TASK_BASE_SERVICE)
    {
        // Update time reading variables
        CecSysTimeMsSet(sysTimerCountMs, &(pCecSwitch[cInstance])->sysTimerCountCurMs, &(pCecSwitch[cInstance])->sysTimerCountPrvMs);
        if ((((pCecSwitch[cInstance])->taskFlags & CEC_SWITCH_TASK_MASK_ALL_BUT_BASE) == 0) && (pCecMessage == 0))
        {
            // Initialize the recall time counter by maximum possible value
            // in order to ensure rare recalls of the task handler when tasks are inactive
            CecTimeCounterSet(CEC_TIME_MS2TCK(CEC_MAX_RECALL_TIME_MS), &(pCecSwitch[cInstance])->recallTimeCount);
        }
        else
        {
            // Ensure fast recall if there are active tasks or pending messages as they could
            // require immediate subsequent action (e.g., launching of active tasks)
            // Actual recall time will be corrected by task internal timer updates, if any
            CecTimeCounterSet(CEC_TIME_MS2TCK(CEC_MIN_RECALL_TIME_MS), &(pCecSwitch[cInstance])->recallTimeCount);
        }
        // Do one task at a time. Tasks shall never be running concurrently except the Base Service that
        // can run in background of another task, not corrupting its context.
        // Base Service has highest priority if a CEC message is pending and lowest priority otherwise
        taskFlagsMasked = ((pCecSwitch[cInstance])->taskFlags & (pCecSwitch[cInstance])->taskMask);
        if (pCecMessage != 0) // CEC message is pending
        {
            feedbackMsg = CecSwitchTaskBase( cInstance, pCecMessage);
        }
        else
        {
            if (taskFlagsMasked & CEC_SWITCH_TASK_SEND_ROUTE_INFO)
            {
                feedbackMsg = CecSwitchTaskRouteInfo( cInstance );
            }
            // Base Task can't be masked and can run in background of the other active task if incoming CEC message is pending
            else
            {
                feedbackMsg = CecSwitchTaskBase( cInstance ,0);
            }
        }
    }
    return SiiCbCecSwitchMessageHandler( cInstance, feedbackMsg);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Returns longest possible time delay before the next invocation
//!             of the SiiCecSwitchTaskProcess() function.
//
//!             This function can be used to set a countdown timer and call
//!             the SiiCecSwitchTaskProcess() on the timer's expiration event unless
//!             new CEC message is pending
//
//! @param[in]  sysTimerCountMs - current value of the system time counter (in milliseconds)
//
//! @return     time in milliseconds
//-------------------------------------------------------------------------------------------------

uint16_t SiiCecSwitchNextInvocationTimeGet( BYTE cInstance, uint16_t sysTimerCountMs)
{
    return SiiCecNextInvocationTimeGet(sysTimerCountMs, (pCecSwitch[cInstance])->recallTimeCount);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Launches CEC/Switch Active and Base Services.
//
//! @return     true, if the task can't be scheduled, false - otherwise
//-------------------------------------------------------------------------------------------------

bool_t SiiCecSwitchSrvStart( BYTE cInstance)
{
    bool_t errStatus = false;
    (pCecSwitch[cInstance])->taskFlags |= CEC_SWITCH_TASK_BASE_SERVICE;
    return errStatus;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Stops Active and Base Services
//-------------------------------------------------------------------------------------------------

void SiiCecSwitchSrvStop( BYTE cInstance)
{
    (pCecSwitch[cInstance])->taskFlags &= ~CEC_SWITCH_TASK_BASE_SERVICE;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Initiate task sending routing information after HPD event
//
//! @retval     true     - if the task is scheduled successfully,
//! @retval     false    - error
//-------------------------------------------------------------------------------------------------

bool_t SiiCecSwitchSendRouteInfo( BYTE cInstance)
{
    // schedule immediate start of the task
    (pCecSwitch[cInstance])->taskFlags |= CEC_SWITCH_TASK_SEND_ROUTE_INFO;
    return true;
}


//-------------------------------------------------------------------------------------------------
// Low level API functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Sends "CEC Routing Information" broadcast message to
//!             announce current active path
//-------------------------------------------------------------------------------------------------

void SiiCecRoutingInformationSend( BYTE cInstance)
{
    SiiCpiData_t cecFrame;
    uint16_t upstreamPhysAddr;
    uint8_t inputPortIndex;
    inputPortIndex = SiiCecPortSelectGet( cInstance ); // Get currently selected input port index
    // Calculate physical address of the device at the active input port
    upstreamPhysAddr = SiiCecAdjacentPhysAddrGet( cInstance, SiiCecGetDevicePA( cInstance ), false, inputPortIndex);
    cecFrame.args[0]  = (upstreamPhysAddr >> 8) & 0xFF; // Upstream Physical Address, MSB
    cecFrame.args[1]  = upstreamPhysAddr & 0xFF;        // Upstream Physical Address, LSB
    SiiCecCpiWrite( cInstance, CECOP_ROUTING_INFORMATION, 2, CEC_LOGADDR_UNREGORBC, &cecFrame);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Sends "CEC Routing Change" broadcast message to
//!             announce that active route below the CEC switch has changed
//-------------------------------------------------------------------------------------------------

void SiiCecRoutingChangeSend( BYTE cInstance, uint8_t newInputPortIndex)
{
    SiiCpiData_t cecFrame;
    uint16_t upstreamPhysAddr;
    uint8_t oldInputPortIndex;
    // Will only send routing change message, if new input port selection
    // is different as compared to the previous setting
    if ((pCecSwitch[cInstance])->activeInputPortIndex != newInputPortIndex)
    {
        oldInputPortIndex = (pCecSwitch[cInstance])->activeInputPortIndex;   // get previously selected port index
        (pCecSwitch[cInstance])->activeInputPortIndex = newInputPortIndex;   // update current input port index
        // Calculate physical address of the device at the old input port
        upstreamPhysAddr = SiiCecAdjacentPhysAddrGet(SiiCecGetDevicePA(), false, oldInputPortIndex);
        cecFrame.args[0]  = (upstreamPhysAddr >> 8) & 0xFF; // Old Upstream Physical Address, MSB
        cecFrame.args[1]  = upstreamPhysAddr & 0xFF;        // Old Upstream Physical Address, LSB
        // Calculate physical address of the device at the new input port
        upstreamPhysAddr = SiiCecAdjacentPhysAddrGet( cInstance, SiiCecGetDevicePA( cInstance ), false, newInputPortIndex);
        cecFrame.args[2]  = (upstreamPhysAddr >> 8) & 0xFF; // New Upstream Physical Address, MSB
        cecFrame.args[3]  = upstreamPhysAddr & 0xFF;        // New Upstream Physical Address, LSB
        SiiCecCpiWrite( cInstance, CECOP_ROUTING_CHANGE, 4, CEC_LOGADDR_UNREGORBC, &cecFrame);
    }
}


