//***************************************************************************
//!file     si_api cec_controller.c
//!brief    Silicon Image mid-level CEC handler
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <string.h>
#include "si_cec_internal.h"

//------------------------------------------------------------------------------
// Component data
//------------------------------------------------------------------------------

uint8_t l_devTypes [16] =
{
    CEC_LOGADDR_TV,
    CEC_LOGADDR_RECDEV1,
    CEC_LOGADDR_RECDEV1,
    CEC_LOGADDR_TUNER1,
    CEC_LOGADDR_PLAYBACK1,
    CEC_LOGADDR_AUDSYS,
    CEC_LOGADDR_PURE_SWITCH,
    CEC_LOGADDR_VIDEO_PROCESSOR,
    CEC_LOGADDR_PLAYBACK2,
    CEC_LOGADDR_RECDEV1,
    CEC_LOGADDR_TUNER1,
    CEC_LOGADDR_PLAYBACK3,
    CEC_LOGADDR_RECDEV2,
    CEC_LOGADDR_RECDEV2,
    CEC_LOGADDR_TV,
    CEC_LOGADDR_TV
};

//------------------------------------------------------------------------------
// Module data
//------------------------------------------------------------------------------

static uint8_t cecValidate [128] =
{
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1,

    //0x0E - 0x0F Reserved
    0, 0,

    1, 1,

    //0x12 - 0x1C Reserved
    0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0,

    1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1,

    //0x2D - 0x2E Reserved
    0, 0,

    1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1,

    //0x39 - 0x3F Reserved
    0, 0, 0,
    0, 0, 0, 0,

    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1,

    //0x4D - 0x4F Reserved
    0, 0, 0,

    1, 1, 1, 1,
    1, 1, 1, 1,

    //0x58 - 0x5F Reserved
    0, 0, 0, 0,
    0, 0, 0, 0,

    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1,

    //0x6E - 0x70 Reserved
    0, 0,
    0,

    1, 1, 1,
    1, 1, 1,

    // 0x77 - 0x7F Reserved
    0,
    0, 0, 0, 0,
    0, 0, 0, 0
};


//------------------------------------------------------------------------------
//! @brief  Check for valid CEC key code.
//! @param[in]  CEC RC key
//------------------------------------------------------------------------------
// not cInstance
bool_t SiiCecValidateKeyCode ( uint8_t keyData )
 {
    bool_t  validKey = false;

    // All keys 0x80 - 0xFF are invalid, use the table for the rest
    if (( cecValidate[ keyData & ~BIT7]) != 0 )
    {
        validKey = true;
    }

    return( validKey );
 }

//------------------------------------------------------------------------------
// Function:
// Description:
//------------------------------------------------------------------------------

void CecPrintLogAddr ( uint8_t bLogAddr )
{

    //    DEBUG_PRINT( MSG_DBG, " [%X] %s\n", (int)bLogAddr, CpCecTranslateLA( bLogAddr ) );
    DEBUG_PRINT2( MSG_DBG, " [%X]\n", (int)bLogAddr );
}

//------------------------------------------------------------------------------
//! @brief      Broadcast a REPORT PHYSICAL ADDRESS message.
//!             Does not wait for a reply.
//! @param[in]  srcLa     - source CEC logical address
//! @param[in]  srcPa     - source CEC physical address
//! @return     uint16_t    16 bit message ID or 0 if CEC not enabled
//------------------------------------------------------------------------------
//给别人报告自己的地址
uint16_t SiiCecSendReportPhysicalAddress (  BYTE cInstance, SiiCecLogicalAddresses_t srcLa, uint16_t srcPa )
{
    SiiCpiData_t cecFrame;

    cecFrame.opcode         = CECOP_REPORT_PHYSICAL_ADDRESS;
    cecFrame.srcDestAddr    = MAKE_SRCDEST( srcLa, CEC_LOGADDR_UNREGORBC );
    cecFrame.args[0]        = srcPa >> 8;           // [Physical Address] High
    cecFrame.args[1]        = srcPa & 0xFF;         // [Physical Address] Low
    cecFrame.args[2]        = l_devTypes[srcLa];    // [Device Type]
    cecFrame.argCount       = 3;

    return( SiiDrvCpiWrite(  cInstance, &cecFrame ));
}

//------------------------------------------------------------------------------
//! @brief      Broadcast a REPORT PHYSICAL ADDRESS message.
//!             Does not wait for a reply.
//! @param[in]  srcLa     - source CEC logical address
//! @param[in]  srcPa     - source CEC physical address
//! @return     uint16_t    16 bit message ID or 0 if CEC not enabled
//------------------------------------------------------------------------------
uint16_t SiiCecSendReportPowerStatus ( BYTE cInstance,  SiiCecLogicalAddresses_t srcLa, SiiCecLogicalAddresses_t destLa, uint8_t powerState )
{
    SiiCpiData_t cecFrame;

    cecFrame.opcode        = CECOP_REPORT_POWER_STATUS;
    cecFrame.srcDestAddr   = MAKE_SRCDEST( srcLa, destLa );
    cecFrame.args[0]       = powerState;
    cecFrame.argCount      = 1;

    return( SiiDrvCpiWrite(  cInstance, &cecFrame ));
}

//------------------------------------------------------------------------------
// Function:    CecSendSetStreamPath
// Description: Broadcast a SET STREAM PATH message.  Does not wait for a reply.
//------------------------------------------------------------------------------

void CecSendSetStreamPath (  BYTE cInstance, uint16_t destPhysAddr )
{
    SiiCpiData_t cecFrame;

    cecFrame.opcode         = CECOP_SET_STREAM_PATH;
    cecFrame.srcDestAddr    = MAKE_SRCDEST( (pCec[cInstance])->logicalAddr, CEC_LOGADDR_UNREGORBC );
    cecFrame.args[0]        = (uint8_t)(destPhysAddr >> 8);
    cecFrame.args[1]        = (uint8_t)destPhysAddr;
    cecFrame.argCount       = 2;
	DEBUG_PRINT( MSG_DBG, "CecSendSetStreamPath\r\n");
    SiiDrvCpiWrite(  cInstance, &cecFrame );
}

//------------------------------------------------------------------------------
//! @brief  Send the device name string as the OSD name
//------------------------------------------------------------------------------
static void CecSendSetOsdName (  BYTE cInstance, uint8_t destLogicalAddr )
{
    SiiCpiData_t cecFrame;

    cecFrame.opcode         = CECOP_SET_OSD_NAME;
    cecFrame.srcDestAddr    = MAKE_SRCDEST( (pCec[cInstance])->logicalAddr, destLogicalAddr );
    memcpy( &cecFrame.args, (pCec[cInstance])->osdName, (pCec[cInstance])->osdNameLen );
    cecFrame.argCount       = (pCec[cInstance])->osdNameLen;

    SiiDrvCpiWrite(  cInstance, &cecFrame );
}

//------------------------------------------------------------------------------
// Function:    CecHandleFeatureAbort
// Description: Received a failure response to a previous message.  Print a
//              message and notify the rest of the system
//TODO: Enhancement - This function should also keep track of the opcode and the
//              follower that does not recognize the opcode so that the opcode
//              is not sent to that follower again.
//------------------------------------------------------------------------------

static void CecHandleFeatureAbort ( SiiCpiData_t *pMsg )
{
    DEBUG_PRINT2(
         CEC_MSG_STAT,
        "\nCEC Message %02X was Feature Aborted (%02X) by logical address %d\n",
        (int)pMsg->args[0],
        pMsg->args[1],
        (int)(pMsg->srcDestAddr >> 4));
}

//------------------------------------------------------------------------------
// Function:    CecValidateMessage
// Description: Validate parameter count of passed CEC message
//              Returns FALSE if not enough operands for the message
//              Returns TRUE if the correct amount or more of operands (if more
//              the message processor will just ignore them).
//------------------------------------------------------------------------------
// return : false   Invalid
//             true     valid
static bool_t CecValidateMessage ( SiiCpiData_t *pMsg )
{
    uint8_t parameterCount = 0;
    bool_t  countOK = true;
    bool_t  isFromUnregistered = false;

    // If message is from Broadcast address, we ignore it except for
    // some specific cases.

    if (( pMsg->srcDestAddr & 0xF0 ) == 0xF0 )
    {
        switch ( pMsg->opcode )
        {
            case CECOP_STANDBY:
            case CECOP_SYSTEM_AUDIO_MODE_REQUEST:
            case CECOP_ROUTING_CHANGE:
            case CECOP_ROUTING_INFORMATION:
            case CECOP_ACTIVE_SOURCE:
            case CECOP_GIVE_PHYSICAL_ADDRESS:
            case CECOP_REPORT_PHYSICAL_ADDRESS:
            case CECOP_REQUEST_ACTIVE_SOURCE:
            case CECOP_GET_MENU_LANGUAGE:
            case CECOP_SET_STREAM_PATH:
            case CDCOP_HEADER:
                break;
            default:
                isFromUnregistered = true;          // All others should be ignored
                break;
        }
    }

    /* Determine required parameter count   */

    switch ( pMsg->opcode )
    {
        case CECOP_IMAGE_VIEW_ON:
        case CECOP_TEXT_VIEW_ON:
        case CECOP_STANDBY:
        case CECOP_GIVE_PHYSICAL_ADDRESS:
        case CECOP_GIVE_DEVICE_VENDOR_ID:
        case CECOP_GIVE_DEVICE_POWER_STATUS:
        case CECOP_GET_MENU_LANGUAGE:
        case CECOP_GET_CEC_VERSION:
        case CECOP_INITIATE_ARC:
        case CECOP_REPORT_ARC_INITIATED:
        case CECOP_REPORT_ARC_TERMINATED:
        case CECOP_REQUEST_ARC_INITIATION:
        case CECOP_REQUEST_ARC_TERMINATION:
        case CECOP_TERMINATE_ARC:
        case CECOP_ABORT:
            parameterCount = 0;
            break;
        case CECOP_REPORT_POWER_STATUS:         // power status
        case CECOP_CEC_VERSION:                 // cec version
            parameterCount = 1;
            break;
        case CECOP_INACTIVE_SOURCE:             // physical address
        case CECOP_FEATURE_ABORT:               // feature opcode / abort reason
        case CECOP_ACTIVE_SOURCE:               // physical address
            parameterCount = 2;
            break;
        case CECOP_REPORT_PHYSICAL_ADDRESS:     // physical address / device type
        case CECOP_DEVICE_VENDOR_ID:            // vendor id
            parameterCount = 3;
            break;
        case CECOP_USER_CONTROL_PRESSED:        // UI command
        case CECOP_SET_OSD_NAME:                // osd name (1-14 bytes)
        case CECOP_SET_OSD_STRING:              // 1 + x   display control / osd string (1-13 bytes)
            parameterCount = 1;                 // must have a minimum of 1 operands
            break;

        default:
            break;
    }

    /* Test for correct parameter count.    */

    if (( pMsg->argCount < parameterCount ) || isFromUnregistered )
    {
        countOK = false;
    }

    return( countOK );
}

//------------------------------------------------------------------------------
// Function:    UpdateTaskQueueState
// Description: If the current task is idle, clear the queue state and bump
//              the task out index to the next task.
// NOTE:        This function is to be called ONLY after the task server has
//              run a task function.
//------------------------------------------------------------------------------

static void UpdateTaskQueueState ( BYTE cInstance )
{
    if ( (pCec[cInstance])->currentTask == SiiCECTASK_IDLE )
    {
        ACTIVE_TASK.queueState = SiiCecTaskQueueStateIdle;
        (pCec[cInstance])->taskQueueOut = ((pCec[cInstance])->taskQueueOut + 1) % TASK_QUEUE_LENGTH;
    }
}

//------------------------------------------------------------------------------
//! @brief  Return state of current enumeration task.
//------------------------------------------------------------------------------
bool_t SiiCecEnumerateIsComplete (  BYTE cInstance )
{
    return( (pCec[cInstance])->enumerateComplete );
}

//------------------------------------------------------------------------------
//! @brief  Return availability of passed logical address.
//------------------------------------------------------------------------------
bool_t SiiCecDeviceLaIsAvailable ( BYTE cInstance,  uint8_t deviceLa )
{
    return( (pCec[cInstance])->logicalDeviceInfo[ deviceLa].deviceType == CEC_DT_COUNT );
}

//------------------------------------------------------------------------------
//! @brief      Ping every logical address on the CEC bus and log the attached
//!             devices
// cecTaskState.taskData1 == Not Used.
// cecTaskState.taskData2 == Not used
//------------------------------------------------------------------------------
static uint8_t CecTaskEnumerateDevices ( BYTE cInstance,  SiiCpiStatus_t *pCecStatus )
{
    uint8_t newTask = ACTIVE_TASK.task;

    switch ( ACTIVE_TASK.cpiState )
    {
    case CPI_IDLE:

        // We're done if we've reached the unregistered address
        if ( ACTIVE_TASK.pTaskData3[ ACTIVE_TASK.taskData1] >= CEC_LOGADDR_UNREGORBC )
        {
            (pCec[cInstance])->enumerateComplete = true;
            SiiCecCbEnumerateComplete( cInstance,  true );          // Let the app layer know.
            DEBUG_PRINT( CEC_MSG_DBG, "ENUM DONE\n" );
            ACTIVE_TASK.cpiState = CPI_IDLE;
            newTask = SiiCECTASK_IDLE;
            break;
        }
        ACTIVE_TASK.destLA = ACTIVE_TASK.pTaskData3[ ACTIVE_TASK.taskData1++];
        ACTIVE_TASK.msgId = SiiDrvCpiSendPing(  cInstance, ACTIVE_TASK.destLA );
        ACTIVE_TASK.cpiState = CPI_WAIT_ACK;
        break;

    case CPI_WAIT_ACK:
        // Make sure this message status is associated with the message we sent.
        if ( pCecStatus->msgId != ACTIVE_TASK.msgId )
        {
            break;
        }
        if ( pCecStatus->txState == SiiTX_SENDFAILED )
        {
        // 没有应当,说明没有这个设备
            DEBUG_PRINT2( MSG_DBG, "-------Enum Task NoAck" );
            CecPrintLogAddr( ACTIVE_TASK.destLA );
            DEBUG_PRINT2( MSG_DBG, "\r\n" );

            // Remove LA from active list unless it is a virtual device,
            // which apparently do not respond to our ping even though
            // they are enabled in the CAPTURE_ID register.
            if ( !(pCec[cInstance])->logicalDeviceInfo[ ACTIVE_TASK.destLA].isVirtual )
            {
                DEBUG_PRINT( CEC_MSG_DBG, "Remove LA %X from CEC list((pCec[cInstance])->logicalDeviceInfo[])\n", ACTIVE_TASK.destLA);
                (pCec[cInstance])->logicalDeviceInfo[ ACTIVE_TASK.destLA].cecPA = 0xFFFF;
                (pCec[cInstance])->logicalDeviceInfo[ ACTIVE_TASK.destLA].deviceType = CEC_DT_COUNT;
                (pCec[cInstance])->logicalDeviceInfo[ ACTIVE_TASK.destLA].selected = false;
            }

            // Restore Tx State to IDLE to try next address.
            ACTIVE_TASK.cpiState = CPI_IDLE;
        }
        else if ( pCecStatus->txState == SiiTX_SENDACKED )
        {
        // 查询物理地址? 物理地址返回在 CECOP_REPORT_PHYSICAL_ADDRESS 消息中处理??
            DEBUG_PRINT2( MSG_DBG, "-------Rx Enum Ack" );
            CecPrintLogAddr( ACTIVE_TASK.destLA );
            DEBUG_PRINT2( MSG_DBG, "\r\n" ); //Remove LA

            // Get the physical address from this source and add it to our
            // list if it responds within 2 seconds, otherwise, ignore it.
            DEBUG_PRINT(MSG_ALWAYS,"PHYSICAL ADDRESS IS BEING REPORTED WITH DST LA:%d\n",ACTIVE_TASK.destLA);
            ACTIVE_TASK.msgId = SiiCecSendMessage( cInstance,  CECOP_GIVE_PHYSICAL_ADDRESS, ACTIVE_TASK.destLA );
            SiiOsTimerSet( &ACTIVE_TASK.taskTimer, 2000 );
            ACTIVE_TASK.cpiState = CPI_WAIT_RESPONSE;
        }
        break;

    case CPI_WAIT_RESPONSE:
        if ( SiiOsTimerExpired( ACTIVE_TASK.taskTimer ))
        {
            DEBUG_PRINT( CEC_MSG_DBG,"RX Enumerate: Timed out waiting for response\n" );

            // Ignore this LA and move on to the next.
            ACTIVE_TASK.cpiState = CPI_IDLE;  // 是否一直在等待呢??  好像收到消息后会设置: 查找: 20150718 11:04:19]
        }
        break;

    case CPI_RESPONSE:
        // The CEC Rx Message Handler has updated the child port list,
        // restore Tx State to IDLE to try next address.
        ACTIVE_TASK.cpiState = CPI_IDLE;
        break;
    }

    return( newTask );
}

//------------------------------------------------------------------------------
//! @brief      Send a CEC message and wait for ACK.  Perform a callback
//!             when ACK is received or a timeout occurs.
//! @note       This task STARTS in CPI_WAIT_ACK state.
// cecTaskState.taskData1 == Not Used.
// cecTaskState.taskData2 == Not used
//------------------------------------------------------------------------------
static uint8_t CecTaskSendMsg ( BYTE cInstance, SiiCpiStatus_t *pCecStatus )
{
    uint8_t newTask = ACTIVE_TASK.task;

    switch ( ACTIVE_TASK.cpiState )
    {
        case CPI_IDLE:

            ACTIVE_TASK.cpiState = CPI_IDLE;
            newTask = SiiCECTASK_IDLE;
            break;

        case CPI_WAIT_ACK:
            // Make sure this message status is associated with the message we sent.
            if ( pCecStatus->msgId != ACTIVE_TASK.msgId )
            {
                break;
            }
            if ( pCecStatus->txState == SiiTX_SENDFAILED )
            {
                DEBUG_PRINT2( MSG_DBG, "CEC Message Send NoAck" );
                CecPrintLogAddr( ACTIVE_TASK.destLA );
                DEBUG_PRINT2( MSG_DBG, "\r\n" );

                // Remove LA from active list
                DEBUG_PRINT( CEC_MSG_DBG, "Remove LA %X from CEC list\n", ACTIVE_TASK.destLA );
                (pCec[cInstance])->logicalDeviceInfo[ ACTIVE_TASK.destLA].cecPA      = 0xFFFF;
                (pCec[cInstance])->logicalDeviceInfo[ ACTIVE_TASK.destLA].deviceType = CEC_DT_COUNT;
                (pCec[cInstance])->logicalDeviceInfo[ ACTIVE_TASK.destLA].selected   = false;
                (pCec[cInstance])->logicalDeviceInfo[ ACTIVE_TASK.destLA].isVirtual  = false;
                SiiCecCbSendMessage( ACTIVE_TASK.taskData1, false );
            }
            else if ( pCecStatus->txState == SiiTX_SENDACKED )
            {
                SiiCecCbSendMessage(ACTIVE_TASK.taskData1, true );
            }
            ACTIVE_TASK.cpiState = CPI_IDLE;
            break;
        default:
            ACTIVE_TASK.cpiState = CPI_IDLE;
            break;
    }

    return( newTask );
}

//------------------------------------------------------------------------------
//! @brief      Returns an available logical address from the passed list of
//!             devices.
//! @param[in]  pDeviceList - list of logical addresses to check
//! @return     Available logical address or CEC_LOGADDR_UNREGORBC.
//------------------------------------------------------------------------------
// 得到自己的逻辑地址吗??
// 查找第一个没被占用的逻辑地址
uint8_t SiiCecGetAvailableLa (  BYTE cInstance, uint8_t *pDeviceLaList )
{
    int     i;
    bool_t  laFound = false;

    // We must have already enumerated
    if ( (pCec[cInstance])->enumerateComplete )
    {
        for ( i = 0; pDeviceLaList[i] < CEC_LOGADDR_UNREGORBC; i++ )
        {
	    DEBUG_PRINT2( MSG_DBG, "My LA  is " );
            CecPrintLogAddr( pDeviceLaList[i] );
            if ( SiiCecDeviceLaIsAvailable( cInstance,  pDeviceLaList[i] ))
            {
                laFound = true;
                break;
            }
        }

        if ( !laFound )
        {
            DEBUG_PRINT( MSG_DBG, "No LA available from list\n" );
        }
    }
    else
    {
        DEBUG_PRINT( MSG_DBG, "Must be enumerated to set LA\n");
    }

    return( laFound ? pDeviceLaList[i] : CEC_LOGADDR_UNREGORBC );
}

//------------------------------------------------------------------------------
//! @brief  Send the device name string as the OSD name
//------------------------------------------------------------------------------
bool_t SiiCecSetOsdName (  BYTE cInstance, char *pOsdName )
{
    int nameLen = strlen( pOsdName );

    (pCec[cInstance])->lastResultCode = RESULT_CEC_INVALID_PARAMETER;
    if ( nameLen < 14 )
    {
        (pCec[cInstance])->osdNameLen = nameLen;
        memcpy( (pCec[cInstance])->osdName, pOsdName, (pCec[cInstance])->osdNameLen );
        (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    }

    return( (pCec[cInstance])->lastResultCode == RESULT_CEC_SUCCESS );
}

//------------------------------------------------------------------------------
//! @brief  Set the device logical address based on the first available entry
//!         in the passed list of device addresses.  Expects that an enumerate
//!         function has been run.
//------------------------------------------------------------------------------
bool_t SiiCecEnumerateDeviceLa ( BYTE cInstance,  uint8_t *pDeviceList )
{
    SiiCecTaskState_t  newTask;

    newTask.task        = SiiCECTASK_SETLA;
    newTask.taskState   = 0;
    newTask.destLA      = 0;
    newTask.cpiState    = CPI_IDLE;
    newTask.taskData1   = 0;
    newTask.taskData2   = 0;
    newTask.pTaskData3  = pDeviceList;
    return( CecAddTask( cInstance,  &newTask ));
}

//------------------------------------------------------------------------------
//! @brief  Cancel enumeration task.
//------------------------------------------------------------------------------
void SiiCecEnumerateCancel (  BYTE cInstance )
{
    if ( (pCec[cInstance])->currentTask == SiiCECTASK_ENUMERATE )
    {
        (pCec[cInstance])->currentTask = SiiCECTASK_CANCEL;
        CecTaskServer( cInstance,  NULL );
        (pCec[cInstance])->enumerateComplete = false;
    }
}

//------------------------------------------------------------------------------
//! @brief  Send the appropriate CEC commands to enumerate the passed list of
//!         logical devices on the CEC bus.
//! @return true if added to task queue, false if not
//------------------------------------------------------------------------------
bool_t SiiCecEnumerateDevices (  BYTE cInstance, uint8_t *pDeviceList )
{
    SiiCecTaskState_t  newTask;
    DEBUG_PRINT( MSG_DBG, "---SiiCecEnumerateDevices() ---\n" );
    (pCec[cInstance])->enumerateComplete = false;
    SiiCecCbEnumerateComplete(  cInstance, false );     // Let the app layer know.

    newTask.task        = SiiCECTASK_ENUMERATE;   // CecTaskEnumerateDevices()
    newTask.taskState   = 0;
    newTask.destLA      = 0;
    newTask.cpiState    = CPI_IDLE;
    newTask.taskData1   = 0;
    newTask.taskData2   = 0;
    newTask.pTaskData3  = pDeviceList;
    return( CecAddTask( cInstance,  &newTask ));
}

//------------------------------------------------------------------------------
//! @brief      Send the appropriate CEC commands to enumerate the passed list
//!             of logical devices on the CEC bus.
//! @param[in]  opCode  - CEC opcode to send
//! @param[in]  src     - source CEC logical address
//! @param[in]  dest    - destination CEC logical address
//! @return     bool_t  true if added to task queue, false if not
//------------------------------------------------------------------------------
bool_t SiiCecSendMessageTaskEx (  BYTE cInstance, uint8_t opCode, uint8_t src, uint8_t dest )
{
    SiiCecTaskState_t  newTask;

    if ( !CecIsRoomInTaskQueue( cInstance ))
    {
        return( false );
    }

    // Add the message to the message queue, then the task will
    // wait for an ACK/NACK
    newTask.msgId       = SiiCecSendMessageEx(  cInstance, opCode, src, dest );
    if (( newTask.msgId == 0 ) || (SiiDrvCpiGetLastResult( cInstance ) != 0))
    {
        return( false );
    }

    // Message sent successfully, add it to the task queue
    newTask.task        = SiiCECTASK_SENDMSG;
    newTask.taskState   = 0;
    newTask.destLA      = 0;
    newTask.cpiState    = CPI_WAIT_ACK; // Start already looking for ACK/NOACK
    newTask.taskData1   = opCode;
    newTask.taskData2   = 0;
    newTask.pTaskData3  = 0;
    return( CecAddTask(  cInstance, &newTask ));
}

//------------------------------------------------------------------------------
//! @brief      Send the appropriate CEC commands to enumerate the passed list
//!             of logical devices on the CEC bus.
//! @param[in]  opCode  - CEC opcode to send
//! @param[in]  dest    - destination CEC logical address
//! @return     bool_t  true if added to task queue, false if not
//!
//! @note       This function is deprecated; please use SiiCecSendMessageTaskEx
//------------------------------------------------------------------------------
bool_t SiiCecSendMessageTask ( BYTE cInstance,  uint8_t opCode, uint8_t dest )
{
    return( SiiCecSendMessageTaskEx ( cInstance,  opCode, (pCec[cInstance])->logicalAddr, dest ));
}

//------------------------------------------------------------------------------
// Function:    CecTaskServer
// Description: Calls the current task handler.  A task is used to handle cases
//              where we must send and receive a specific set of CEC commands.
//
// NOTE:        If the contents of the SiiCpiStatus_t structure are not new,
//              they should be set to 0 before calling.
//------------------------------------------------------------------------------

void CecTaskServer (  BYTE cInstance, SiiCpiStatus_t *pCecStatus )
{
    uint8_t logicalAddr;
    int_t   regTaskIndex;

    switch ( (pCec[cInstance])->currentTask )
    {
        case SiiCECTASK_CANCEL:

            // If a task is in progress, cancel it.
            if ( (pCec[cInstance])->taskQueue[ (pCec[cInstance])->taskQueueOut].queueState == SiiCecTaskQueueStateRunning )
            {
                (pCec[cInstance])->taskQueue[ (pCec[cInstance])->taskQueueOut].queueState = SiiCecTaskQueueStateIdle;
                (pCec[cInstance])->currentTask = SiiCECTASK_IDLE;
            }
            break;

        case SiiCECTASK_IDLE:
            if ( (pCec[cInstance])->taskQueue[ (pCec[cInstance])->taskQueueOut].queueState == SiiCecTaskQueueStateQueued )
            {
                (pCec[cInstance])->taskQueue[ (pCec[cInstance])->taskQueueOut].queueState = SiiCecTaskQueueStateRunning;
                (pCec[cInstance])->currentTask = (pCec[cInstance])->taskQueue[ (pCec[cInstance])->taskQueueOut].task;
            }
            break;

        case SiiCECTASK_ENUMERATE:
            (pCec[cInstance])->currentTask = (SiiCecTasks_t)CecTaskEnumerateDevices( cInstance,  pCecStatus );
            UpdateTaskQueueState( cInstance );
            break;

        case SiiCECTASK_SETLA:
            logicalAddr = SiiCecGetAvailableLa(  cInstance, ACTIVE_TASK.pTaskData3 );
            if ( logicalAddr < CEC_LOGADDR_UNREGORBC )
            {
                SiiCecSetDeviceLA( cInstance,  (SiiCecLogicalAddresses_t)logicalAddr );
            }
            (pCec[cInstance])->currentTask = SiiCECTASK_IDLE;
            UpdateTaskQueueState( cInstance );
            break;

        case SiiCECTASK_ONETOUCH:
            (pCec[cInstance])->currentTask = (SiiCecTasks_t)CecTaskOneTouchPlay(  cInstance ,pCecStatus );
            UpdateTaskQueueState( cInstance );
            break;

        case SiiCECTASK_SENDMSG:
            (pCec[cInstance])->currentTask = (SiiCecTasks_t)CecTaskSendMsg(  cInstance, pCecStatus );
            UpdateTaskQueueState( cInstance );
            break;

        default:

            // Check the registered task list.
            regTaskIndex = (pCec[cInstance])->currentTask - SiiCECTASK_COUNT;
            if (( regTaskIndex < MAX_CEC_TASKS) && ( (pCec[cInstance])->pTasks[ regTaskIndex ] != 0))
            {
                (pCec[cInstance])->currentTask =(SiiCecTasks_t)((*(pCec[cInstance])->pTasks[ regTaskIndex])( pCecStatus ));
            }
            break;
    }
}

//------------------------------------------------------------------------------
//! @brief      This is the first message handler called in the chain. It parses
//!             messages that are handled the same way for every system type and
//!             those that it needs to keep the internal component status
//!             up to date.  Some message are left marked as unused so that the
//!             application layer can also see them.
//! @param[in]  pMsg - Pointer to CEC message received
//! @return     true if message consumed, false if not used
//------------------------------------------------------------------------------
static bool_t CecMsgHandlerFirst (  BYTE cInstance, SiiCpiData_t *pMsg )
{
    bool_t  usedMessage         = true;
    bool_t  isDirectAddressed   = !((pMsg->srcDestAddr & 0x0F ) == CEC_LOGADDR_UNREGORBC );// false : 广播

    // Don't process unless it is intended for the local logical address (we must check
    // in case another device has been added for virtual device support).
    if ( GET_CEC_DEST( pMsg->srcDestAddr) != (pCec[cInstance])->logicalAddr )
    {
        return( false ); // Didn't handle the message
    }
    switch ( pMsg->opcode )
    {
        case CECOP_STANDBY:                                             // Direct and Broadcast

  /*
         if ( (pCec[cInstance])->powerState != CEC_POWERSTATUS_STANDBY )
            {
                // Next time through the main loop, power will be cycled off

                (pCec[cInstance])->powerState    = CEC_POWERSTATUS_ON_TO_STANDBY;
                (pCec[cInstance])->statusFlags   |= SiiCEC_POWERSTATE_CHANGE;        // Signal upper layer
                (pCec[cInstance])->isActiveSource = false;                           // Only impacts TX
            }
      */
            break;

        case CECOP_GIVE_PHYSICAL_ADDRESS:  // 对方要求自己发送物理地址
            if ( isDirectAddressed )                    // Ignore as broadcast message
            {
            	DEBUG_PRINT(MSG_ALWAYS, "PHYSICAL ADDRESS BEING GIVE FROM FIRST HANFLER\n");
		DEBUG_PRINT(MSG_ALWAYS, "Report Own PhysicalAddress\n");
               if( SiiCecSendReportPhysicalAddress(  cInstance, (pCec[cInstance])->logicalAddr, (pCec[cInstance])->physicalAddr ))
		      DEBUG_PRINT(MSG_ALWAYS, " 0x%X   0x%X\n", (pCec[cInstance])->logicalAddr, (pCec[cInstance])->physicalAddr);
            }
            break;

        case CECOP_REPORT_PHYSICAL_ADDRESS: // 对方报告对方的物理地址
            if ( !isDirectAddressed )                   // Ignore as direct message
            {
            	if(pMsg->args[2] != 0)
            		pMsg->srcDestAddr = (pMsg->srcDestAddr & 0x0F) | (pMsg->args[2] << 4) ;
                SiiCecUpdateLogicalDeviceInfo( cInstance, (pMsg->srcDestAddr >> 4) & 0x0F,         // broadcast logical address
                    (((uint16_t)pMsg->args[0]) << 8) | pMsg->args[1],   // broadcast physical address
                    false
                    );

                // Let Enumerate task know about it.
                if ( ACTIVE_TASK.task == SiiCECTASK_ENUMERATE )
                {
                    ACTIVE_TASK.cpiState = CPI_RESPONSE;  // //[_kyq : 20150718 11:04:19]
                }
            }
            break;

        case CECOP_GIVE_DEVICE_POWER_STATUS:
            if ( isDirectAddressed )                // Ignore as broadcast message
            {
                SiiCecSendReportPowerStatus(  cInstance, 
					      (SiiCecLogicalAddresses_t)((pCec[cInstance])->logicalAddr),
					      (SiiCecLogicalAddresses_t)(pMsg->srcDestAddr >> 4), 
					      (pCec[cInstance])->powerState );
            }
            break;

        case CECOP_REPORT_POWER_STATUS:                 // Someone sent us their power state.
            if ( isDirectAddressed )                    // Ignore as broadcast message
            {
                (pCec[cInstance])->sourcePowerStatus = (SiiCecPowerstatus_t)(pMsg->args[0]);
            }
            break;

        case CECOP_USER_CONTROL_PRESSED:
            if ( !isDirectAddressed )   break;  // Ignore as broadcast message
            DEBUG_PRINT(MSG_ALWAYS, "SiiCecValidateKeyCode\n");
            // If not a VALID CEC key, feature abort
            if ( !SiiCecValidateKeyCode( pMsg->args[0]))
            {
                SiiCecFeatureAbortSendEx(cInstance, 
                                        (uint8_t)(pMsg->opcode),
                                         (uint8_t)CECAR_INVALID_OPERAND,
                                        (SiiCecLogicalAddresses_t)((pCec[cInstance])->logicalAddr),
                                         (SiiCecLogicalAddresses_t)(GET_CEC_SRC( pMsg->srcDestAddr )));
                break;              // Used the message...
            }
            usedMessage = false;    // Didn't use the message, we just validated the key ID
            break;

        default:
            usedMessage = false;                        // Didn't use the message
            break;
    }

    return( usedMessage );
}

//------------------------------------------------------------------------------
// Function:    CecMsgHandlerLast
// Description: This is the last message handler called in the chain, and
//              parses any messages left untouched by the previous handlers.
//
// NOTE:        Messages handled here should be common to all system types.
//
//------------------------------------------------------------------------------

static void CecMsgHandlerLast (  BYTE cInstance, SiiCpiData_t *pMsg )
{
    uint8_t         srcAddr;
    bool_t          isDirectAddressed;
    SiiCpiData_t    cecFrame;

    isDirectAddressed   = !((pMsg->srcDestAddr & 0x0F ) == CEC_LOGADDR_UNREGORBC );
    srcAddr             = GET_CEC_SRC( pMsg->srcDestAddr );

    DEBUG_PRINT(MSG_ALWAYS, "CecMsgHandlerLast cmd   0x%X \r\n", pMsg->opcode);
//   UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
    switch ( pMsg->opcode )
    {
        case CECOP_FEATURE_ABORT:
            if ( isDirectAddressed )                // Ignore as broadcast message
            {
                CecHandleFeatureAbort( pMsg );
            }
            break;

            // These messages have already been handled for internal purposes
            // by CecRxMsgHandler and passed to the application level
            // and/or were ignored but not consumed by the application level.
            // Ignore them here.

        case CECOP_IMAGE_VIEW_ON:
        case CECOP_TEXT_VIEW_ON:
        case CECOP_GET_MENU_LANGUAGE:
        case CECOP_USER_CONTROL_PRESSED:
        case CECOP_USER_CONTROL_RELEASED:
            break;

            // Handle this here because the app level may have upgraded the version
            // and handled it before it gets here.

        case CECOP_GET_CEC_VERSION:
            if ( isDirectAddressed )                    // Ignore as broadcast message
            {
                // Respond to this request with the CEC version support.
                cecFrame.srcDestAddr   = MAKE_SRCDEST( (pCec[cInstance])->logicalAddr, pMsg->srcDestAddr >> 4 );
                cecFrame.opcode        = CECOP_CEC_VERSION;
                cecFrame.args[0]       = 0x05;       // Report CEC1.4
                cecFrame.argCount      = 1;
                SiiDrvCpiWrite(  cInstance, &cecFrame );
            }
            break;

        case CECOP_GIVE_OSD_NAME:
            if ( isDirectAddressed )
            {
                CecSendSetOsdName(  cInstance, srcAddr );
            }
            break;

            // Ignore these messages if unrecognized AND broadcast
            // but feature abort them if directly addressed
        case CECOP_GIVE_DEVICE_VENDOR_ID:
        case CDCOP_HEADER:
            if ( isDirectAddressed )
            {
                     DEBUG_PRINT(MSG_ALWAYS, " pMsg->opcode CDCOP_HEADER\r\n");
                SiiCecFeatureAbortSendEx( cInstance,  pMsg->opcode, CECAR_UNRECOG_OPCODE, (SiiCecLogicalAddresses_t)(GET_CEC_DEST( pMsg->srcDestAddr)), (SiiCecLogicalAddresses_t)srcAddr );
            }
            break;

            // Any directly addressed message that gets here is not supported by this
            // device, so feature abort it with unrecognized opcode.
            // This means that the app layer must be sure to properly handle any messages
            // that it should be able to handle.

        case CECOP_ABORT:
        default:
            if((pMsg->opcode != CECOP_ABORT) &&
            		((pMsg->opcode == CECOP_ROUTING_CHANGE) || (pMsg->opcode == CECOP_ROUTING_INFORMATION) || (pMsg->opcode == CECOP_SET_STREAM_PATH)))
                break;
            if ( isDirectAddressed )                    // Ignore as broadcast message
            {
                DEBUG_PRINT(MSG_ALWAYS, " pMsg->opcode CECOP_ABORT\r\n");

                SiiCecFeatureAbortSendEx( cInstance,  pMsg->opcode, CECAR_UNRECOG_OPCODE, (SiiCecLogicalAddresses_t)(GET_CEC_DEST( pMsg->srcDestAddr)), (SiiCecLogicalAddresses_t)srcAddr );
            }
            break;
    }
}

//------------------------------------------------------------------------------
// Function:    SiiCecHandler
// Description: Polls the send/receive state of the CPI hardware and runs the
//              current task, if any.
//
//              A task is used to handle cases where we must send and receive
//              a specific set of CEC commands.
//------------------------------------------------------------------------------

void SiiCecHandler (  BYTE cInstance  )
{
    bool_t          processedMsg;
    int_t           callbackIndex, frameCount;
    SiiCpiStatus_t  cecStatus;
    SiiCpiData_t    cecFrame;
    //static unsigned char command=0; 
    do
    {
        if ( !(pCec[cInstance])->enable ) break;

        SiiDrvCpiServiceWriteQueue( cInstance );               // Send any pending messages
        SiiDrvCpiHwStatusGet(  cInstance, &cecStatus );         // Get the CEC transceiver status

        // Now look to see if any CEC commands were received.
        if ( cecStatus.rxState )
        {
            // Get CEC frames until no more are present.
            cecStatus.rxState = 0;      // Clear activity flag
            for ( ;; )
            {
                frameCount = SiiDrvCpiFrameCount( cInstance );
                if ( frameCount == 0 )
                {
                    break;
                }
                if ( !SiiDrvCpiRead( cInstance,  &cecFrame ))
                {
                    DEBUG_PRINT( CEC_MSG_DBG, "Error in Rx Fifo\n");
                    break;
                }
                processedMsg = false;
                if ( !CecValidateMessage(&cecFrame ))   // If invalid message, ignore it, but treat it as handled
                {
                    processedMsg = true;
                }
                if ( CecMsgHandlerFirst(  cInstance, &cecFrame ))   // Handle the common system messages
                {
                    processedMsg = true;
                }

                // Iterate through registered message handlers.
                // SiiCecInitialize 注册回调函数
                // called  CecRxMsgHandler() CecTxMsgHandler()
                for ( callbackIndex = 0; callbackIndex < MAX_CEC_PARSERS; callbackIndex++ )
                {
                    if ( (pCec[cInstance])->messageHandlers[ callbackIndex].pMsgHandler != 0)
                    {
                        // Some handlers must be called every time through the handler.
                        if ( (pCec[cInstance])->messageHandlers[ callbackIndex].callAlways )
                        {
                            if ( (*(pCec[cInstance])->messageHandlers[ callbackIndex].pMsgHandler)( cInstance , &cecFrame ) )
                            {
                                processedMsg = true;
                            }
                        }

                        // Other handlers are called only if the message has not been used.
                        else if ( !processedMsg )
                        {
                            if ( (*(pCec[cInstance])->messageHandlers[ callbackIndex].pMsgHandler)( cInstance,  &cecFrame ) )
                            {
                                processedMsg = true;
                            }
                        }
                    }

                }


                if ( !processedMsg )
                {
                    CecMsgHandlerLast(  cInstance, &cecFrame );   // Let the built-in handler take care of leftovers.
                }
            }
        }
        else
        {
            // No message, but iterate through registered message handlers in case
            // one or more has an 'always call' tag.  In this case, the handler
            // is called with a null pointer instead of a pointer to a CEC message
            for ( callbackIndex = 0; callbackIndex < MAX_CEC_PARSERS; callbackIndex++ )
            {
                if ( (pCec[cInstance])->messageHandlers[ callbackIndex].pMsgHandler != 0)
                {
                    if ( (pCec[cInstance])->messageHandlers[ callbackIndex].callAlways )
                    {
                        (*(pCec[cInstance])->messageHandlers[ callbackIndex].pMsgHandler)(  cInstance, NULL );
                    }
                }
            }
        }

        // After the current message (if any) has been processed, cecStatus.msgId is
        // still valid, so we pass that information to the task server in case the message
        // was one that a task was waiting for.
        CecTaskServer(  cInstance, &cecStatus );                // Process task even if no new status

    } while ( 0 );
}

//------------------------------------------------------------------------------
//! @brief  Determine if there is a slot available in the CEC task queue
//------------------------------------------------------------------------------
bool_t CecIsRoomInTaskQueue (  BYTE cInstance )
{

    if ( (pCec[cInstance])->taskQueue[ (pCec[cInstance])->taskQueueIn ].queueState == SiiCecTaskQueueStateIdle )
    {
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
//! @brief  Add the passed task to the CEC task queue if room.
//------------------------------------------------------------------------------
bool_t CecAddTask ( BYTE cInstance,  SiiCecTaskState_t *pNewTask )
{
    bool_t      success = true;

    // Store the message in the task queue
    if ( (pCec[cInstance])->taskQueue[ (pCec[cInstance])->taskQueueIn ].queueState == SiiCecTaskQueueStateIdle )
    {
        memcpy( &(pCec[cInstance])->taskQueue[ (pCec[cInstance])->taskQueueIn ], pNewTask, sizeof( SiiCecTaskState_t ));
        (pCec[cInstance])->taskQueue[ (pCec[cInstance])->taskQueueIn ].queueState = SiiCecTaskQueueStateQueued;

        (pCec[cInstance])->taskQueueIn = ((pCec[cInstance])->taskQueueIn + 1) % TASK_QUEUE_LENGTH;
    }
    else
    {
        DEBUG_PRINT( MSG_DBG, "CEC Task Queue full\n" );
        success = false;
    }

    (pCec[cInstance])->lastResultCode = (success) ? RESULT_CEC_SUCCESS : RESULT_CEC_TASK_QUEUE_FULL;
    return( success );
}

