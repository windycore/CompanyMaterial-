//***************************************************************************
//!file     si_drv_cpi.c
//!brief    Silicon Image CPI Driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "string.h"
#include "si_drv_cpi_internal.h"
#include "si_regs_tpi953x.h"
#include "si_regs_cpi.h"
#include "si_cec_enums.h"
#include "si_osal_timer.h"

//------------------------------------------------------------------------------
//  Driver Data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  CPI Driver Instance Data
//------------------------------------------------------------------------------
/*
    int_t           inIndex;
    int_t           outIndex;
    clock_time_t    msTimeout;
    clock_time_t    msStart;
    SiiCpiData_t    queue[SII_CPI_OUTPUT_QUEUE_LEN];

                          uint8_t         srcDestAddr;            // Source in upper nibble, dest in lower nibble
                          SiiCecOpcodes_t opcode;
                          uint8_t         args[ SII_MAX_CMD_SIZE ];
                          uint8_t         argCount;
                          SiiCpiTxState_t txState;
                          uint16_t        msgId;
    */
CpiInstanceData_t cpiInstance[DevNum][SII_NUM_CPI] =
{
{
    0,                          // structVersion
    0,                          // instanceIndex 1: TX, 0 : RX
    RESULT_CPI_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    {0,SiiTX_IDLE,0,0},                  // cecStatus
    CEC_LOGADDR_UNREGORBC,      // logicalAddr
    //inIndex ,outIndex,msTimeout,msStart,
    //             queue.srcDestAddr
    //                      opcode
    //                                                         args
    //                                                             argCount
    //                                                                   txState
    //                                                                                 msgId
    {0, 0, 0, 0, {{0,CECOP_FEATURE_ABORT,{0},0,SiiTX_IDLE,0}}},    // msgQueueOut
    0,                          // *pLogger
},
#if(DevNum>1)
{
    0,                          // structVersion
    0, // 1,                          // instanceIndex;  // kyq   1: TX, 0 : RX
    RESULT_CPI_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    {0,SiiTX_IDLE,0,0},                  // cecStatus
     CEC_LOGADDR_UNREGORBC,      // logicalAddr  // will not work
    {0, 0, 0, 0, {{0,CECOP_FEATURE_ABORT,{0},0,SiiTX_IDLE,0}}},    // msgQueueOut
    0,                          // *pLogger
},
#endif
#if(DevNum>2)
{
    0,                          // structVersion
    0, // 1,                          // instanceIndex;  // kyq   1: TX, 0 : RX
    RESULT_CPI_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    {0,SiiTX_IDLE,0,0},                  // cecStatus
     CEC_LOGADDR_UNREGORBC,      // logicalAddr  // will not work
    {0, 0, 0, 0, {{0,CECOP_FEATURE_ABORT,{0},0,SiiTX_IDLE,0}}},    // msgQueueOut
    0,                          // *pLogger
},
#endif
#if(DevNum>3)
{
    0,                          // structVersion
    0, // 1,                          // instanceIndex;  // kyq   1: TX, 0 : RX
    RESULT_CPI_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    {0,SiiTX_IDLE,0,0},                  // cecStatus
     CEC_LOGADDR_UNREGORBC,      // logicalAddr  // will not work
    {0, 0, 0, 0, {{0,CECOP_FEATURE_ABORT,{0},0,SiiTX_IDLE,0}}},    // msgQueueOut
    0,                          // *pLogger
},
#endif
#if(DevNum>4)
{
    0,                          // structVersion
    0, // 1,                          // instanceIndex;  // kyq   1: TX, 0 : RX
    RESULT_CPI_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    {0,SiiTX_IDLE,0,0},                  // cecStatus
     CEC_LOGADDR_UNREGORBC,      // logicalAddr  // will not work
    {0, 0, 0, 0, {{0,CECOP_FEATURE_ABORT,{0},0,SiiTX_IDLE,0}}},    // msgQueueOut
    0,                          // *pLogger
},
#endif
#if(DevNum>5)
{
    0,                          // structVersion
    0, // 1,                          // instanceIndex;  // kyq   1: TX, 0 : RX
    RESULT_CPI_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    {0,SiiTX_IDLE,0,0},                  // cecStatus
     CEC_LOGADDR_UNREGORBC,      // logicalAddr  // will not work
    {0, 0, 0, 0, {{0,CECOP_FEATURE_ABORT,{0},0,SiiTX_IDLE,0}}},    // msgQueueOut
    0,                          // *pLogger
},
#endif
#if(DevNum>6)
{
    0,                          // structVersion
    0, // 1,                          // instanceIndex;  // kyq   1: TX, 0 : RX
    RESULT_CPI_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    {0,SiiTX_IDLE,0,0},                  // cecStatus
     CEC_LOGADDR_UNREGORBC,      // logicalAddr  // will not work
    {0, 0, 0, 0, {{0,CECOP_FEATURE_ABORT,{0},0,SiiTX_IDLE,0}}},    // msgQueueOut
    0,                          // *pLogger
},
#endif
#if(DevNum>7)
{
    0,                          // structVersion
    0, // 1,                          // instanceIndex;  // kyq   1: TX, 0 : RX
    RESULT_CPI_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    {0,SiiTX_IDLE,0,0},                  // cecStatus
     CEC_LOGADDR_UNREGORBC,      // logicalAddr  // will not work
    {0, 0, 0, 0, {{0,CECOP_FEATURE_ABORT,{0},0,SiiTX_IDLE,0}}},    // msgQueueOut
    0,                          // *pLogger
},
#endif
#if(DevNum>8)
{
    0,                          // structVersion
    0, // 1,                          // instanceIndex;  // kyq   1: TX, 0 : RX
    RESULT_CPI_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    {0,SiiTX_IDLE,0,0},                  // cecStatus
     CEC_LOGADDR_UNREGORBC,      // logicalAddr  // will not work
    {0, 0, 0, 0, {{0,CECOP_FEATURE_ABORT,{0},0,SiiTX_IDLE,0}}},    // msgQueueOut
    0,                          // *pLogger
},
#endif

};
CpiInstanceData_t *pCpi[DevNum] =
{ 
  &cpiInstance[0][0],
#if(DevNum>1)
  &cpiInstance[1][0],
#endif
#if(DevNum>2)
  &cpiInstance[2][0],
#endif
#if(DevNum>3)
  &cpiInstance[3][0],
#endif
#if(DevNum>4)
  &cpiInstance[4][0],
#endif
#if(DevNum>5)
  &cpiInstance[5][0],
#endif
#if(DevNum>6)
  &cpiInstance[6][0],
#endif
#if(DevNum>7)
  &cpiInstance[7][0],
#endif
#if(DevNum>8)
  &cpiInstance[8][0],
#endif
};

static uint8_t l_devTypes [16] =
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
// Function:    SiiDrvCpiInitialize
//! @brief      Initialize the CPI hardware
//              Note that the CPI hardware is left disabled.  It must be enabled
//              by calling SiiDrvCpiResume()
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//              if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvCpiInitialize (  BYTE cInstance )
{
    bool_t success = true;

    // Initialize current CEC instance.

    if ( !SiiDrvCpiSetLogicalAddr(  cInstance, (pCpi[cInstance])->logicalAddr ))
    {
        success = false;
        DEBUG_PRINT( MSG_ERR, "\n Cannot init CPI/CEC");
    }

    (pCpi[cInstance])->statusFlags   = 0;

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiEnable
//! @brief      Enable the CPI hardware and interrupts for the current instance.
// Parameters:  none
// Returns:     none
//
// API NOTE:    The SiI9535 Tx1 module must be initialized prior to calling this
//              function.  It does not have to be enabled, just initialized.
//------------------------------------------------------------------------------

void SiiDrvCpiEnable (  BYTE cInstance )
{
    // Two types of CEC in Sii9535, with slightly different enable controls
    if ( (pCpi[cInstance])->instanceIndex == 0 )
    {
        SiiRegBitsSet( cInstance,  REG_SYS_RESET_2, BIT_CEC_M_SRST, false );       // Enable Main CEC
    }
    SiiRegWrite(cInstance,  REG_CEC_CONFIG_CPI, CLEAR_BITS );

    // Clear any pre-existing junk from the RX FIFO

    SiiRegBitsSet(  cInstance, REG_CEC_RX_CONTROL, BIT_CLR_RX_FIFO_ALL, true );

    // Clear any existing interrupts

    SiiRegBitsSet(  cInstance, REG_CEC_INT_STATUS_0, BIT_RX_MSG_RECEIVED | BIT_TX_FIFO_EMPTY | BIT_TX_MESSAGE_SENT, true );
    SiiRegBitsSet(  cInstance, REG_CEC_INT_STATUS_1, BIT_FRAME_RETRANSM_OV | BIT_SHORT_PULSE_DET | BIT_START_IRREGULAR | BIT_RX_FIFO_OVERRUN, true );

    // Enable the interrupts within the CEC hardware block

    SiiRegBitsSet(  cInstance, REG_CEC_INT_ENABLE_0, BIT_RX_MSG_RECEIVED | BIT_TX_FIFO_EMPTY | BIT_TX_MESSAGE_SENT, true );
    SiiRegBitsSet(  cInstance, REG_CEC_INT_ENABLE_1, BIT_FRAME_RETRANSM_OV | BIT_SHORT_PULSE_DET | BIT_START_IRREGULAR | BIT_RX_FIFO_OVERRUN, true );

    (pCpi[cInstance])->statusFlags   &= ~SiiCPI_CEC_DISABLED;
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiDisable
//! @brief      Disable the CPI hardware and interrupts for the current instance.
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

void SiiDrvCpiDisable (  BYTE cInstance )
{
    // Two types of CEC in Sii9535, with slightly different enable controls

    if ( (pCpi[cInstance])->instanceIndex == 0 )
    {
        SiiRegBitsSet( cInstance,REG_SYS_RESET_2, BIT_CEC_M_SRST, true );     // Disable RX CEC
    }

    (pCpi[cInstance])->statusFlags   |= SiiCPI_CEC_DISABLED;
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiStatus
// Description: Returns a status flag word containing CPI-specific
//				information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit status flags word for the CPI Component
//------------------------------------------------------------------------------

uint16_t SiiDrvCpiStatus (  BYTE cInstance )
{
    uint16_t statusFlags = (pCpi[cInstance])->statusFlags;

    (pCpi[cInstance])->statusFlags &= ~SiiCPI_CEC_INT;   // INT flag only valid first time it is read.
	return( statusFlags );
}

//------------------------------------------------------------------------------
//! @brief  Places the CPI component into standby mode if available
//! @return true if the CPI component entered standby mode successfully,
//!         or false if some failure occurred.
//------------------------------------------------------------------------------
bool_t SiiDrvCpiStandby (  BYTE cInstance )
{
    int i;

    // Setup the auto feature abort logic for this device by
    // starting out with all opcodes set to feature abort mode.
    for ( i = 0; i < 32; i++ )
    {
        SiiRegWrite(cInstance,  CEC_OP_ABORT_0 + i, 0xFF );
    }

    // Now enable those opcodes we want for the wakeup feature.
    switch ( l_devTypes[(pCpi[cInstance])->logicalAddr])
    {
        case CEC_LOGADDR_TV:
            SiiRegWrite( cInstance,  CEC_OP_ABORT_0,    ~0x10 );    // CECOP_IMAGE_VIEW_ON
            SiiRegWrite( cInstance,  CEC_OP_ABORT_1,    ~0x20 );    // CECOP_TEXT_VIEW_ON
            break;
        case CEC_LOGADDR_AUDSYS:
            SiiRegWrite( cInstance,  CEC_OP_ABORT_14,   ~0x01 );    // CECOP_SYSTEM_AUDIO_MODE_REQUEST
            break;
        case CEC_LOGADDR_PLAYBACK1:
        case CEC_LOGADDR_PLAYBACK2:
            SiiRegWrite( cInstance,  CEC_OP_ABORT_8,    ~0x06 );    // CECOP_PLAY && CECOP_DECK_CONTROL
            break;
    }

    // Everybody must respond to GIVE_DEVICE_POWER_STATUS
    SiiRegWrite( cInstance,  CEC_OP_ABORT_17, (uint8_t)~0x80 );              // CECOP_GIVE_DEVICE_POWER_STATUS

#if INC_CEC_SWITCH
    SiiRegWrite( cInstance,  CEC_OP_ABORT_16,   ~0x41 );            // CECOP_ACTIVE_SOURCE && CECOP_SET_STREAM_PATH
#endif

	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiResume
// Description: Causes operation of the CPI component to return to the state
//				it was prior to the previous call to SiiCpiStandby.
// Parameters:  none
// Returns:     true if the CPI component successfully exited standby mode,
//				or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvCpiResume (  BYTE cInstance )
{
    int i;

    // Setup the auto feature abort logic for this device by
    // starting out with all opcodes set to NOT feature abort mode.
    for ( i = 0; i < 32; i++ )
    {
        SiiRegWrite( cInstance,  CEC_OP_ABORT_0 + i, 0x00 );
    }

    // Now set to feature abort the CECOP_ABORT opcode
    SiiRegWrite( cInstance,  CEC_OP_ABORT_31,   0x80 );    // CECOP_ABORT

	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiInstanceGet
// Description: returns the index of the current CPI instance
// Parameters:  none
// Returns:     Index of the current CPI instance
//------------------------------------------------------------------------------

uint_t SiiDrvCpiInstanceGet (  BYTE cInstance )
{

    return( (pCpi[cInstance])->instanceIndex );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiInstanceSet
// Description: Set the component global data pointer to the requested instance.
// Parameters:  instanceIndex
// Returns:     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------

bool_t SiiDrvCpiInstanceSet (  BYTE cInstance, uint_t instanceIndex )
{
    if ( instanceIndex < SII_NUM_CPI )
    {
        (pCpi[cInstance]) = &cpiInstance[cInstance][ instanceIndex];

        // Set instance for CRA pages used.
        SiiRegInstanceSet(  cInstance, PP_PAGE_B,   instanceIndex );
        SiiRegInstanceSet(  cInstance, CPI_PAGE,    instanceIndex );
        SiiRegInstanceSet(  cInstance, TX_PAGE_TPI, instanceIndex );
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiGetLastResult
// Description: Returns the result of the last SiiCpixxx function called
// Parameters:  none
// Returns:     Result of last CPI component function.
//------------------------------------------------------------------------------

uint_t SiiDrvCpiGetLastResult (  BYTE cInstance )
{

	return( (pCpi[cInstance])->lastResultCode );
}

//------------------------------------------------------------------------------
// Function:    SiiCpiRegisterLogger
// Description: Register a logger callback with the current CPI instance
// Parameters:  Callback function pointer
// Returns:     true if successful
//------------------------------------------------------------------------------

bool_t SiiCpiRegisterLogger ( BYTE cInstance,  void (*pLogger)(SiiCpiData_t *, int, bool_t ) )
{

    (pCpi[cInstance])->pLogger = pLogger;
    return( true );
}

//------------------------------------------------------------------------------
//! @brief  Add or remove the passed CEC logical address to the addresses
//!         responded to by the CPI subsystem.
//! @param[in]  addLa           - true - add, false - remove
//! @param[in]  logicalAddress  - CEC logical address (0-15)
//! @return     true if valid logical address parameter value
//------------------------------------------------------------------------------
bool_t SiiDrvCpiAddLogicalAddr (  BYTE cInstance, bool_t addLa, uint8_t logicalAddress )
{
    int     laOffset = 0;
    bool_t  success = false;
    uint8_t capture_address[2];

    if ( logicalAddress < 0x10 )
    {
        SiiRegReadBlock( cInstance,  REG_CEC_CAPTURE_ID0, capture_address, 2 );

        // Adjust for second byte of capture map
        if ( logicalAddress > 7 )
        {
            laOffset = 1;
            logicalAddress -= 8;
        }

        if ( addLa )
        {
            capture_address[ laOffset] |= (1 << logicalAddress);
        }
        else
        {
            capture_address[ laOffset] &= ~(1 << logicalAddress);
        }

        // Set new capture address map
        SiiRegWriteBlock(  cInstance, REG_CEC_CAPTURE_ID0, capture_address, 2 );
        success = true;
    }

    return( success );
}

//------------------------------------------------------------------------------
//! @brief  Configure the CPI subsystem to respond to and transmit as a
//!         specific CEC logical address.
//! @param[in]  logicalAddress - CEC logical address or 0xFF if unregistered
//! @return     always true
//------------------------------------------------------------------------------
bool_t SiiDrvCpiSetLogicalAddr ( BYTE cInstance,  uint8_t logicalAddress )
{
    uint8_t capture_address[2];
    uint8_t capture_addr_sel = 0x01;

    capture_address[0] = 0;
    capture_address[1] = 0;
    if ( logicalAddress == 0xFF )
    {
        logicalAddress = 0x0F;  // unregistered LA
    }
    else if ( logicalAddress < 8 )
    {
        capture_addr_sel = capture_addr_sel << logicalAddress;
        capture_address[0] = capture_addr_sel;
    }
    else
    {
        capture_addr_sel   = capture_addr_sel << ( logicalAddress - 8 );
        capture_address[1] = capture_addr_sel;
    }

    // Set Capture Address
    SiiRegWriteBlock(cInstance, REG_CEC_CAPTURE_ID0, capture_address, 2 );

    // Set device logical address for transmit
    SiiRegWrite( cInstance,  REG_CEC_TX_INIT, logicalAddress );

    (pCpi[cInstance])->logicalAddr = logicalAddress;
    DEBUG_PRINT(CPI_MSG_DBG, "SiiDrvCpiSetLogicalAddr:  %04X \r\n", logicalAddress);
    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiServiceWriteQueue
// Description: If there is a CEC message waiting in the write queue and the
//              hardware TX buffer is empty, send the message.
//------------------------------------------------------------------------------
//
//  如果有等待发送队列,发送数据。
//  如果有超时则设置超时
//
void SiiDrvCpiServiceWriteQueue (  BYTE cInstance )
{
    SiiCpiData_t *pOutMsg;
    uint8_t cecStatus[2];
    //
    //  SiiDrvCpiHwStatusGet( ) 函数
    //   会循环出队列outIndex
    do
    {
        // No message in the queue?  Get out.
        if ( (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.outIndex ].txState == SiiTX_IDLE )
        {
            break;
        }

        // If last TX command is still being sent (waiting for ACK/NACK), check timeout
        if ( (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.outIndex ].txState == SiiTX_SENDING)
        {
            // If a timeout has occurred, mark the current message as failed.
            // This will be picked up by the SiiDrvCpiHwStatusGet function and passed to the CEC controller
            if (( SiiOsTimerTotalElapsed() - (pCpi[cInstance])->msgQueueOut.msStart ) >= (pCpi[cInstance])->msgQueueOut.msTimeout )
            {
                (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.outIndex ].txState = SiiTX_TIMEOUT;

                if ( (pCpi[cInstance])->msgQueueOut.queue[(pCpi[cInstance])->msgQueueOut.outIndex].opcode != CECOP_SII_SENDPING )
                {
                    DEBUG_PRINT( CPI_MSG_DBG, "%s:CEC Message [W%02X][%02X] send timeout!\n",
                        (pCpi[cInstance])->instanceIndex ? "TX" : "RX",
                        (pCpi[cInstance])->msgQueueOut.queue[(pCpi[cInstance])->msgQueueOut.outIndex].srcDestAddr,
                        (pCpi[cInstance])->msgQueueOut.queue[(pCpi[cInstance])->msgQueueOut.outIndex].opcode
                        );
                }
                (pCpi[cInstance])->statusFlags |= SiiCPI_CEC_STATUS_VALID;
                break;
            }
        }

        // If NOT waiting to be sent, get out.
        if ( (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.outIndex ].txState != SiiTX_WAITCMD)
        {
            break;
        }

        // Current queue entry is waiting to be sent, so send it
        // 等待发送
        // SiiDrvCpiWrite 函数会设立等待
        (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.outIndex ].txState = SiiTX_SENDING;
        pOutMsg = &(pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.outIndex ];

        // Clear Tx-related interrupts; write 1 to bits to be cleared.
        cecStatus[0] = BIT_TX_BUFFER_FULL | BIT_TX_MESSAGE_SENT | BIT_TX_FIFO_EMPTY;
        cecStatus[1] = BIT_FRAME_RETRANSM_OV;
        SiiRegWriteBlock( cInstance,  REG_CEC_INT_STATUS_0, cecStatus, 2 );

        // Special handling for a special opcode.

        if ( pOutMsg->opcode == CECOP_SII_SENDPING )
        {
            SiiRegWrite( cInstance,  REG_CEC_TX_DEST, BIT_SEND_POLL | pOutMsg->srcDestAddr);
        }
        else
        {
            // Set the initiator to the LA specified by the srcDestAddr
            SiiRegWrite( cInstance,  REG_CEC_TX_INIT, (( pOutMsg->srcDestAddr >> 4) & 0x0F));

            // Send the command
            SiiRegWrite( cInstance,  REG_CEC_TX_DEST, pOutMsg->srcDestAddr & 0x0F );
            SiiRegWrite( cInstance,  REG_CEC_TX_COMMAND, pOutMsg->opcode );
	    // if(pOutMsg->argCount >=0 )  //[kyq add: 20150820 19:53:20] // cancel it by kyq, 20151219
	     	{
                  SiiRegWriteBlock( cInstance,  REG_CEC_TX_OPERAND_0, pOutMsg->args, pOutMsg->argCount );
                  SiiRegWrite( cInstance,  REG_CEC_TRANSMIT_DATA, BIT_TRANSMIT_CMD | pOutMsg->argCount );
	     	}
        }

        (pCpi[cInstance])->msgQueueOut.msStart = SiiOsTimerTotalElapsed();

        // If there is a logging callback, do it now.
        if ( (pCpi[cInstance])->pLogger != 0)
        {
            (*(pCpi[cInstance])->pLogger)( pOutMsg, (pCpi[cInstance])->instanceIndex, true );  // SkAppCecMessageLogger
        }

    } while (0);

}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiWrite
// Description: Send CEC command via CPI register set
//
// API NOTE:    The SiiDrvCpiServiceWriteQueue() function must be called
//              periodically to send any messages queued up by this function.
//              The SiiDrvCpiHwStatusGet() function must be called to release
//              the queue from waiting for the previously sent message to complete
//------------------------------------------------------------------------------

uint16_t SiiDrvCpiWrite(  BYTE cInstance, SiiCpiData_t *pMsg )
{
    uint16_t    msgId = 0;
    bool_t      success = true;
    int         i;

    // Store the message in the output queue
    if ( (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.inIndex ].txState == SiiTX_IDLE )
    {
        memcpy( &(pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.inIndex ], pMsg, sizeof( SiiCpiData_t ) );
        (pCpi[cInstance])->msgQueueOut.msTimeout = 2000;     // timeout after 2 seconds
        (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.inIndex ].txState    = SiiTX_WAITCMD;
        msgId = ((pCpi[cInstance])->msgQueueOut.inIndex << 8) | (pMsg->opcode - 1);
        (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.inIndex ].msgId      = msgId;

        (pCpi[cInstance])->msgQueueOut.inIndex = ((pCpi[cInstance])->msgQueueOut.inIndex + 1) % SII_CPI_OUTPUT_QUEUE_LEN;
    }
    else
    {
        DEBUG_PRINT( MSG_DBG, "\nSiiCpiWrite:: CEC Write Queue full!\n");
        for ( i = 0; i < SII_CPI_OUTPUT_QUEUE_LEN; i++ )
        {
            DEBUG_PRINT( MSG_DBG, "SRCDST: %02X - Opcode: %02X\n", (pCpi[cInstance])->msgQueueOut.queue[i].srcDestAddr, (pCpi[cInstance])->msgQueueOut.queue[i].opcode );
        }
        success = false;
    }

    SiiDrvCpiServiceWriteQueue( cInstance);  // Send the message if possible

    (pCpi[cInstance])->lastResultCode = (success) ? RESULT_CPI_SUCCESS : RESULT_CPI_WRITE_QUEUE_FULL;
    return( msgId );
}


//------------------------------------------------------------------------------
// Function:    SiiDrvCpiSendPing
// Description: Initiate sending a ping, and used for checking available
//              CEC devices
//------------------------------------------------------------------------------

uint16_t SiiDrvCpiSendPing (BYTE cInstance,  uint8_t destLA )
{
    SiiCpiData_t cecFrame;

    // Send the ping via the normal CPI message queue.
    DEBUG_PRINT( CPI_MSG_DBG, "---SiiDrvCpiSendPing[%02X]\n",destLA);
    cecFrame.opcode         = CECOP_SII_SENDPING;
    cecFrame.srcDestAddr    = destLA;
    cecFrame.argCount       = 0;
    return( SiiDrvCpiWrite(  cInstance,   &cecFrame ));
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiRead
// Description: Reads a CEC message from the CPI read FIFO, if present.
// Returns:     true if valid message, false if an error occurred
//------------------------------------------------------------------------------

bool_t SiiDrvCpiRead( BYTE cInstance,  SiiCpiData_t *pMsg )
{
    bool_t    success = true;
    uint8_t argCount;

    argCount = SiiRegRead(  cInstance, REG_CEC_RX_COUNT );

    if ( argCount & BIT_MSG_ERROR )
    {
        success = false;
    }
    else
    {
        pMsg->argCount = argCount & 0x0F;
        pMsg->srcDestAddr = SiiRegRead(  cInstance, REG_CEC_RX_CMD_HEADER );
        pMsg->opcode = (SiiCecOpcodes_t)SiiRegRead( cInstance,  REG_CEC_RX_OPCODE );
        if ( pMsg->argCount )
        {
            SiiRegReadBlock( cInstance,  REG_CEC_RX_OPERAND_0, pMsg->args, pMsg->argCount );
        }
    }

    // Clear CLR_RX_FIFO_CUR;
    // Clear current frame from Rx FIFO

    SiiRegModify(  cInstance, REG_CEC_RX_CONTROL, BIT_CLR_RX_FIFO_CUR, BIT_CLR_RX_FIFO_CUR );

    if ( success )
    {
        // If there is a logging callback, do it now.

        if ( (pCpi[cInstance])->pLogger != 0)
        {
            (*(pCpi[cInstance])->pLogger)( pMsg, (pCpi[cInstance])->instanceIndex, false );// SkAppCecMessageLogger
        }
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SiiCpiFrameCount
// Description: Return the number of CEC frames currently available
//------------------------------------------------------------------------------

uint_t SiiDrvCpiFrameCount(  BYTE cInstance )
{

    return(( SiiRegRead(  cInstance, REG_CEC_RX_COUNT) & 0xF0) >> 4);
}

//------------------------------------------------------------------------------
// Function:    SiiCpiGetLogicalAddr
// Description: Get Logical Address
//------------------------------------------------------------------------------

uint8_t SiiDrvCpiGetLogicalAddr(  BYTE cInstance )
{
    return( SiiRegRead(  cInstance,   REG_CEC_TX_INIT));
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCpiHwStatusGet
// Description: returns CPI CEC status register info if it has been updated
//              since the last call to this function.
// Parameters:  pCpiStat - pointer to CPI hardware status for return value.
// Returns:     true if a new status was available, false if not.
//------------------------------------------------------------------------------
// (pCpi[cInstance])->statusFlags & SiiCPI_CEC_STATUS_VALID 才会处理
bool_t  SiiDrvCpiHwStatusGet( BYTE cInstance, SiiCpiStatus_t *pCpiStat )
{
    memset( pCpiStat, 0, sizeof( SiiCpiStatus_t ));     // Always clear status for return
    if ( (pCpi[cInstance])->statusFlags & SiiCPI_CEC_STATUS_VALID )
    {
        memcpy( pCpiStat, &(pCpi[cInstance])->cecStatus, sizeof( SiiCpiStatus_t ));
        if ( (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.outIndex ].txState == SiiTX_TIMEOUT)
        {
            // A CPI write timed out; mark it as a NACK.  Since the CPI writes
            // are serialized, there is no other TX status currently valid.
            pCpiStat->txState = SiiTX_SENDFAILED;
        }

        // Add the message ID to the returned status

        pCpiStat->msgId = (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.outIndex ].msgId;

        // If the status indicates an ACKed or NACKed transmission,
        // update the sent message queue so that any pending message
        // may be sent.  Note that this is the only way that a message
        // waiting in the CPI output message queue can be sent.
        // 在DrvCpiProcessInterrupts 处理中会标志
        if (( pCpiStat->txState == SiiTX_SENDACKED ) || ( pCpiStat->txState == SiiTX_SENDFAILED))
        {
            // Mark this queue entry as available and bump to next entry in queue

            (pCpi[cInstance])->msgQueueOut.queue[ (pCpi[cInstance])->msgQueueOut.outIndex ].txState = SiiTX_IDLE;
            (pCpi[cInstance])->msgQueueOut.outIndex = ((pCpi[cInstance])->msgQueueOut.outIndex + 1) % SII_CPI_OUTPUT_QUEUE_LEN;
        }

        (pCpi[cInstance])->statusFlags &= ~SiiCPI_CEC_STATUS_VALID;
        return( true );
    }
    return( false );
}

//------------------------------------------------------------------------------
// Function:    DrvCpiProcessInterrupts
// Description: Check CPI registers for a CEC event
//------------------------------------------------------------------------------

void DrvCpiProcessInterrupts(  BYTE cInstance )
{
    uint8_t cecStatus[2];

    SiiRegReadBlock(  cInstance, REG_CEC_INT_STATUS_0, cecStatus, 2);

    if ( (cecStatus[0] & 0x7F) || cecStatus[1] )
    {
        (pCpi[cInstance])->cecStatus.cecError    = 0;
        (pCpi[cInstance])->cecStatus.rxState     = 0;
        (pCpi[cInstance])->cecStatus.txState     = (SiiCpiTxState_t)0;

        // Clear interrupts

        if ( cecStatus[1] & BIT_FRAME_RETRANSM_OV )
        {
            /* Flush TX, otherwise after clearing the BIT_FRAME_RETRANSM_OV */
            /* interrupt, the TX command will be re-sent.                   */

            SiiRegModify(  cInstance, REG_CEC_DEBUG_3,BIT_FLUSH_TX_FIFO, BIT_FLUSH_TX_FIFO );
        }

        // Clear interrupt bits that are set
        SiiRegWriteBlock( cInstance,  REG_CEC_INT_STATUS_0, cecStatus, 2 );

        // RX Processing
        if ( cecStatus[0] & BIT_RX_MSG_RECEIVED )
        {
            (pCpi[cInstance])->cecStatus.rxState = 1;    // Flag caller that CEC frames are present in RX FIFO
        }

        // RX Errors processing
        if ( cecStatus[1] & BIT_SHORT_PULSE_DET )
        {
            (pCpi[cInstance])->cecStatus.cecError |= SiiCEC_SHORTPULSE;
        }

        if ( cecStatus[1] & BIT_START_IRREGULAR )
        {
            (pCpi[cInstance])->cecStatus.cecError |= SiiCEC_BADSTART;
        }

        if ( cecStatus[1] & BIT_RX_FIFO_OVERRUN )
        {
            (pCpi[cInstance])->cecStatus.cecError |= SiiCEC_RXOVERFLOW;
        }

        // TX Processing
        if ( cecStatus[0] & BIT_TX_MESSAGE_SENT )
        {
            (pCpi[cInstance])->cecStatus.txState = SiiTX_SENDACKED;
        }
        if ( cecStatus[1] & BIT_FRAME_RETRANSM_OV )
        {
            (pCpi[cInstance])->cecStatus.txState = SiiTX_SENDFAILED;
        }

        // Indicate that an interrupt occurred and status is valid.

        (pCpi[cInstance])->statusFlags |= (SiiCPI_CEC_INT | SiiCPI_CEC_STATUS_VALID);
    }
}
