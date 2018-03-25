//***************************************************************************
//!file     si_api_cec_tasks.c
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
#include "si_common.h"
#include "si_drv_switch_config.h"
#include "si_cec_component.h"
#include "si_cec_internal.h"

//------------------------------------------------------------------------------
//! @brief      Store the LA, PA, and type of the specified device.
//! @param[in]  newLA       - CEC logical address of device to add
//!                         - If bit 7 is set, this is a virtual device
//! @param[in]  newPA       - CEC physical address of device to add
//! @param[in]  isActive    - true if this device is the active source
//------------------------------------------------------------------------------
void SiiCecUpdateLogicalDeviceInfo ( BYTE cInstance, uint8_t newLA, uint16_t newPA, bool_t isActive )
{
    int_t   i, portIndex;
    bool_t  isVirtual;

    DEBUG_PRINT( MSG_STAT, "UpdateLogicalDeviceInfo: newla %x portIndex %x %x %x\n",newLA,
    		((newPA & (pCec[cInstance])->paChildMask ) >> (pCec[cInstance])->paShift) - 1,
    		(pCec[cInstance])->paChildMask, (pCec[cInstance])->paShift );

    isVirtual = ((newLA & BIT7) != 0);
    newLA &= ~BIT7;
    if ( newLA > CEC_LOGADDR_UNREGORBC )
    {
        (pCec[cInstance])->lastResultCode = RESULT_CEC_INVALID_LOGICAL_ADDRESS;
        return;
    }
    //CEC网络中的其余子设备在连接时读取其父设备的EDID
    //，并将其父设备的物理地址中的第一个数字0的位置替换成一个非0数字
    //，便为该子设备的物理地址。
    // 应该是父节点的端口号
    portIndex = ((newPA & (pCec[cInstance])->paChildMask ) >> (pCec[cInstance])->paShift) - 1;  // Determine actual child port.
    if (portIndex < 0)
    {
    	portIndex = 0;
    }

    if ( portIndex < SII_INPUT_PORT_COUNT )
    {
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].cecPA      = newPA;
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].port       = portIndex;
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].selected   = false;
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].isVirtual  = isVirtual;
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].deviceType = l_devTypes[ newLA ];
        (pCec[cInstance])->logicalAddr = (SiiCecLogicalAddresses_t)newLA; // ???????????????????????????

        // If called from an ACTIVE SOURCE handler, mark this device as the
        // selected device for this port.

        if ( isActive )
        {
            for ( i = 0; i <= CEC_LOGADDR_UNREGORBC; i++ )
            {
                // Update all logical addresses on this port.

                if ( (pCec[cInstance])->logicalDeviceInfo[ i ].port == portIndex )
                {
                    if ( i == newLA )
                    {
                        // The requested port is selected
                        (pCec[cInstance])->logicalDeviceInfo[ newLA ].selected = true;
                    }
                    else
                    {
                        (pCec[cInstance])->logicalDeviceInfo[ newLA ].selected = false;
                    }
                }
            }
        }
        DEBUG_PRINT( CEC_MSG_DBG, "UpdateLogicalDeviceInfo- LA:%02X PA:%04X on port %d\n",
                (int)newLA, newPA, (int)portIndex );
        (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    }
    else
    {
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].cecPA      = 0xFFFF;
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].port       = SII_INPUT_PORT_COUNT;
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].selected   = false;
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].isVirtual  = false;
        (pCec[cInstance])->logicalDeviceInfo[ newLA ].deviceType = CEC_DT_COUNT;

        (pCec[cInstance])->lastResultCode = RESULT_CEC_INVALID_PHYSICAL_ADDRESS;
        DEBUG_PRINT( CEC_MSG_DBG, "UpdateLogicalDeviceInfo: Bad Physical Address: %04X\n", newPA );
    }
}

//------------------------------------------------------------------------------
// Function:    CecHandleActiveSource
// Description: Process the CEC Active Source command by switching to the
//              broadcast port.
//------------------------------------------------------------------------------

static void CecHandleActiveSource ( BYTE cInstance,  SiiCpiData_t *pMsg )
{
    // Extract the logical and physical addresses of the new active source.

    (pCec[cInstance])->activeSrcLogical  = (SiiCecLogicalAddresses_t)((pMsg->srcDestAddr >> 4) & 0x0F);
    (pCec[cInstance])->activeSrcPhysical = ((uint16_t)pMsg->args[0] << 8 ) | pMsg->args[1];

   // SiiCecUpdateLogicalDeviceInfo( (pCec[cInstance])->activeSrcLogical, (pCec[cInstance])->activeSrcPhysical, true );

    // Determine the index of the HDMI port that is handling this physical address.

   // (pCec[cInstance])->portSelect = (((pCec[cInstance])->activeSrcPhysical >> (pCec[cInstance])->paShift) & 0x000F) - 1;
  DEBUG_PRINT( CEC_MSG_DBG, "\nACTIVE_SOURCE: %02X (%04X) (port %02X)\n",
            (int)(pCec[cInstance])->activeSrcLogical, (pCec[cInstance])->activeSrcPhysical, (int)(pCec[cInstance])->portSelect );
    // Signal App layer that a port select change has occurred.
    // The App layer will perform the physical port switch.

    (pCec[cInstance])->statusFlags |= SiiCEC_PORT_CHANGE;
}

//------------------------------------------------------------------------------
// Function:    CecHandleInactiveSource
// Description: Process the CEC Inactive Source command
//------------------------------------------------------------------------------

static void CecHandleInactiveSource (  BYTE cInstance, SiiCpiData_t *pMsg )
{
    uint8_t la;

    la = (pMsg->srcDestAddr >> 4) & 0x0F;
    if ( la == (pCec[cInstance])->activeSrcLogical )    // The active source has deserted us!
    {
        (pCec[cInstance])->activeSrcLogical  = CEC_LOGADDR_TV;
        (pCec[cInstance])->activeSrcPhysical = 0x0000;
    }
    // Signal upper layer that the active source has been lost

    (pCec[cInstance])->statusFlags |= SiiCEC_SOURCE_LOST;
}

//------------------------------------------------------------------------------
// Function:    CecHandleReportPhysicalAddress
// Description: Store the PA and LA of the subsystem.
//              This routine is called when a physical address was broadcast.
//              usually this routine is used for a system which configure as TV or Repeater.
//------------------------------------------------------------------------------

static void CecHandleReportPhysicalAddress (  BYTE cInstance, SiiCpiData_t *pMsg )
{
	if(pMsg->args[2] != 0)
		pMsg->srcDestAddr = (pMsg->srcDestAddr & 0x0F) | (pMsg->args[2] << 4) ;
    SiiCecUpdateLogicalDeviceInfo( cInstance, (pMsg->srcDestAddr >> 4) & 0x0F,         // broadcast logical address
        (((uint16_t)pMsg->args[0]) << 8) | pMsg->args[1],   // broadcast physical address
        false
        );
}

//------------------------------------------------------------------------------
// Function:    CecRxMsgHandler
// Description: This is the first RX message handler called in the chain. It parses
//              messages that it needs to keep the internal component status
//              up to date.
//------------------------------------------------------------------------------
// (pCec[cInstance])->deviceType = CEC_DT_TV 
//                                 CEC_DT_PLAYBACK 
//      will regist
bool_t CecRxMsgHandler ( BYTE cInstance,  SiiCpiData_t *pMsg )
{
    bool_t  usedMessage         = true;
    bool_t  isDirectAddressed   = !((pMsg->srcDestAddr & 0x0F ) == CEC_LOGADDR_UNREGORBC );

  DEBUG_PRINT( CEC_MSG_DBG,"CecRxMsgHandler pMsg->srcDestAddr [0x%04X] \r\n", pMsg->srcDestAddr );


    switch ( pMsg->opcode )
    {
        case CECOP_IMAGE_VIEW_ON:       // In our case, respond the same to both these messages
        case CECOP_TEXT_VIEW_ON:
            if ( !isDirectAddressed )   break;          // Ignore as broadcast message

            // Make sure the system is powered on.  The application layer
            // must handle any other procedures needed to turn on the display
            if ( (pCec[cInstance])->powerState != CEC_POWERSTATUS_ON )
            {
                // Next time through the main loop, power will be cycled on
                (pCec[cInstance])->powerState    = CEC_POWERSTATUS_STANDBY_TO_ON;
                (pCec[cInstance])->statusFlags   |= SiiCEC_POWERSTATE_CHANGE;            // Signal upper layer
            }
            // don't tell anyone we looked at this message.  The app layer
            // likely needs to see it also.
            usedMessage = false;
            break;

        case CECOP_ACTIVE_SOURCE:
            if ( !isDirectAddressed )                   // Ignore as direct message
            {
               // CecHandleActiveSource( pMsg );
            }
            break;

        case CECOP_REPORT_PHYSICAL_ADDRESS:
            if ( !isDirectAddressed )                   // Ignore as direct message
            {
     //           CecHandleReportPhysicalAddress( pMsg );

                // Let Enumerate task know about it.
                if ( ACTIVE_TASK.task == SiiCECTASK_ENUMERATE )
                {
                    ACTIVE_TASK.cpiState = CPI_RESPONSE;
                }
            }
            break;

        case CECOP_INACTIVE_SOURCE:
            if ( isDirectAddressed )                    // Ignore as broadcast message
            {
                CecHandleInactiveSource(  cInstance, pMsg );
            }
            break;

        default:
            usedMessage = false;                        // Didn't use the message
            break;
    }

    return( usedMessage );
}

//------------------------------------------------------------------------------
//! @brief  Send the appropriate CEC commands for switching from the current
//!         active source to the requested source.
//!         This is called after the main process has ALREADY performed the
//!         actual port switch on the RX, so the physical address is changed
//!         even if the input is NOT in the CEC list, either because it has
//!         not reported itself as an active source, or it is not CEC
//!         capable.
//! @param[in]  portIndex - new HDMI port index.  May be CEC or not.
//! @return     bool_t    - true if successful, false if could not queue CEC task.
//------------------------------------------------------------------------------
bool_t SiiCecSwitchSources ( BYTE cInstance,  uint8_t portIndex )
{
    uint16_t    portPA;

    (pCec[cInstance])->lastResultCode = RESULT_CEC_SUCCESS;
    if ( (pCec[cInstance])->enable )
    {
        // If bad port index or not yet enumerated, get out.
        if (( portIndex >= SII_INPUT_PORT_COUNT ) || ( SiiCecGetDeviceLA( cInstance ) == CEC_LOGADDR_UNREGORBC ))
        {
            (pCec[cInstance])->lastResultCode = RESULT_CEC_INVALID_PORT_INDEX;
            return( false );
        }

        // If the CEC portSelect value is NOT already set to the requested port,
        // chances are the new port was requested via the Sink UI as opposed to
        // receiving an Active Source message from a source device.  If the
        // request came from the Sink UI, send a SET_STREAM_PATH to point
        // everybody to our new source (and wake it up, if necessary).
        if ( portIndex != (pCec[cInstance])->portSelect )
        {
            (pCec[cInstance])->portSelect = portIndex;

            // Only send this message if we are a SINK device
            if ( (pCec[cInstance])->deviceType == CEC_DT_TV )
            {
                // If port has an associated CEC physical address, send the message
                portPA = SiiCecPortToPA( cInstance,  portIndex );
                if ( portPA != 0xFFFF )
                {
                    CecSendSetStreamPath(  cInstance, portPA );
                }
            }
        }
    }

    return( true );
}
