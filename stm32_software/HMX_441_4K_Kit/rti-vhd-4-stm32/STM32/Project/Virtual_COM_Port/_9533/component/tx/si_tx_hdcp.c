//************************************************************************************************
//! @file   si_tx_hdcp.h
//! @brief  HDCP related methods
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//************************************************************************************************/

#include <string.h>
#include "si_tx_component_internal.h"
#include "si_tx_hdcp.h"
#include "si_tx_info_frm.h"
#include "si_drv_tpi_system.h"
#include "si_drv_tpi_access.h"
#include "si_drv_tpi_ddc_access.h"
#include "si_hdcp_ddc.h"
#include "si_edid_tx_internal.h"
#include "si_edid_black_list.h"
#include "si_osal_timer.h"

#include "main.h"		//zzl
#include "Mcu.h"
#include "uart.h"
#include "Util.h"

#include "si_drv_repeater.h"
txHdcpInstanceRecord_t HdcpInstRec[INSTANCE_9533][SII_NUM_TX];
txHdcpInstanceRecord_t *pHdcp[INSTANCE_9533] = 
{
     &HdcpInstRec[0][0],		
#if (INSTANCE_9533>1)
      &HdcpInstRec[1][0],
#endif

#if (INSTANCE_9533>2)
      &HdcpInstRec[2][0],
#endif

#if (INSTANCE_9533>3)
      &HdcpInstRec[3][0],
#endif

#if (INSTANCE_9533>4)
      &HdcpInstRec[4][0],
#endif

#if (INSTANCE_9533>5)
      &HdcpInstRec[5][0],
#endif
#if (INSTANCE_9533>6)
      &HdcpInstRec[6][0],
#endif
#if (INSTANCE_9533>7)
      &HdcpInstRec[7][0],
#endif

};
//[kyq Begin: 20151016 17:50:10]
static BYTE m_acHDCPTry[INSTANCE_9533] =
{
     0,		
#if (INSTANCE_9533>1)
      0,
#endif

#if (INSTANCE_9533>2)
      0,
#endif

#if (INSTANCE_9533>3)
     0,
#endif

#if (INSTANCE_9533>4)
     0,
#endif

#if (INSTANCE_9533>5)
      0,
#endif
#if (INSTANCE_9533>6)
      0,
#endif
#if (INSTANCE_9533>7)
      0,
#endif

};
//[kyq End: 20151016 17:50:12]
extern txInstanceData_t *pTx[]; // pointer to TX component record


//-------------------------------------------------------------------------------------------------
//  Local service functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Show HDCP authentication phase and error messages, if any.
//-------------------------------------------------------------------------------------------------

static void PrintHdcpStatus( BYTE cInstance)
{
    if (pHdcp[cInstance]->prevAuthState != pHdcp[cInstance]->authState)
    {
#if (API_DEBUG_CODE==1)
        if(DEBUG_PRINT(MSG_STAT /*TX_MSG_DBG*/, "DS HDCP: "))
        {
            switch (pHdcp[cInstance]->authState)
            {
            case SI_TX_HDCP__OFF:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "OFF\n");
                break;
            case SI_TX_HDCP__WAIT_FOR_START:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "WAIT FOR START\n");
                break;
            case SI_TX_HDCP__WAIT_FOR_RETRY:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "WAIT FOR RETRY\n");
                break;
            case SI_TX_HDCP__WAIT_FOR_DONE:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "WAIT FOR DONE\n");
                break;
            case SI_TX_HDCP__WAIT_FIFO_READY:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "WAIT FOR FIFO READY\n");
                break;
            case SI_TX_HDCP__KSV_FIFO_READ:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "KSV FIFO READ\n");
                break;
            case SI_TX_HDCP__V_CHECK:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "V=V' CHECK\n");
                break;
            case SI_TX_HDCP__AUTHENTICATED:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "AUTHENTICATED\n");
		  m_acHDCPTry[cInstance] = 0;
                break;
            case SI_TX_HDCP__SECURE:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "SECURE\n");
                break;
            case SI_TX_HDCP__FAILED:
                DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "FAILED\n");
/*
		  ////[kyq Begin: 20151016 16:32:07]
		  m_acHDCPTry[cInstance]++; 
		  if(m_acHDCPTry[cInstance]> 5) 
          {
              SiiDrvTpiTmdsOutputEnable(cInstance, FALSE);
              SiiDrvTpiTmdsOutputEnable(cInstance, TRUE);
          }
		 //[kyq End: 20151016 17:49:48]		
  */              
                break;
            case SI_TX_HDCP__INITIAL:
                break;

            }
        }
#endif
        pHdcp[cInstance]->prevAuthState = pHdcp[cInstance]->authState;
    }

    if (pHdcp[cInstance]->prevError != pHdcp[cInstance]->error)
    {
#if (API_DEBUG_CODE==1)
        if (pHdcp[cInstance]->error)
        {
            if(DEBUG_PRINT(TX_MSG_ERR, "HDCP Errors:\n"))
            {

                if (pHdcp[cInstance]->error & ERR_HDCP__NACK)
                    DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tNo ACK at downstream DDC\n");
                if (pHdcp[cInstance]->error & ERR_HDCP__INTERNAL)
                    DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tInternal\n");
                if (pHdcp[cInstance]->error & ERR_HDCP__DEV_LIMIT)
                    DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tDevice limit exceeded\n");
                if (pHdcp[cInstance]->error & ERR_HDCP__V)
                    DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tDownstream V' != V\n");
                if (pHdcp[cInstance]->error & ERR_HDCP__P2_TO)
                    DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tFIFO READY was not set within 5 s\n");
                if (pHdcp[cInstance]->error & ERR_HDCP__DS_EXCEED)
                    DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tDownstream device indicated DEV_EXC or CAS_EXC\n");
            }
        }
        else
        {
            DEBUG_PRINT(TX_MSG_DBG, "No HDCP Errors\n");
        }
#endif
        pHdcp[cInstance]->prevError = pHdcp[cInstance]->error;
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Show HDCP query status
//-------------------------------------------------------------------------------------------------

static void PrintHdcpQueryStatus(BYTE cInstance,uint8_t query)
{
#if (API_DEBUG_CODE==1)
    if(DEBUG_PRINT(TX_MSG_DBG, "HDCP Query Status:\n"))
    {

        DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tLink Encryption: \t");

        switch (query &  MSK_TPI__HDCP_QUERY__STATUS)
        {
        case VAL_TPI__HDCP_QUERY__STATUS_NORMAL:
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "Normal\n");
            break;
        case VAL_TPI__HDCP_QUERY__STATUS_LOST:
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "Lost\n");
            break;
        case VAL_TPI__HDCP_QUERY__STATUS_FAILED:
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "Failed\n");
            break;
        case VAL_TPI__HDCP_QUERY__STATUS_SUSPENDED:
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "Suspended\n");
            break;
        }

        DEBUG_PRINT(TX_MSG_DBG_PLAIN, "\tConnector Type: \t");
        switch (query & MSK_TPI__HDCP_QUERY__CONNECTR)
        {
        case VAL_TPI__HDCP_QUERY__CONNECTR_DVI:
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "DVI\n");
            break;
        case VAL_TPI__HDCP_QUERY__CONNECTR_HDMI:
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "HDMI\n");
            break;
        }

        DEBUG_PRINT(TX_MSG_DBG_PLAIN, "\tProtection Type: \t");
        if (query & BIT_TPI__HDCP_QUERY__SINK_CAPABLE)
        {
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "HDCP\n");
        }
        else
        {
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "None\n");
        }

        DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tRepeater: \t\t");
        if (query & BIT_TPI__HDCP_QUERY__DS_REPEATER)
        {
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "Yes\n");
        }
        else
        {
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "No\n");
        }

        DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tLocal Protection: \t");
        if (query & BIT_TPI__HDCP_QUERY__LOCAL_PROTECT)
        {
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "Yes\n");
        }
        else
        {
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "No\n");
        }

        DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "\tExtended Protection: \t");
        if (query & BIT_TPI__HDCP_QUERY__EXTENDED_PROTECT)
        {
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "Yes\n");
        }
        else
        {
            DEBUG_PRINT2(TX_MSG_DBG_PLAIN, "No\n");
        }
    }
#endif
}

//-------------------------------------------------------------------------------------------------
//! @brief      Begin calculation of a new KSV hash value.
//-------------------------------------------------------------------------------------------------

static void NewKsvHash( BYTE cInstance)
{
    // Keep previous hash value
    pHdcp[cInstance]->dsKsvHashPrev = pHdcp[cInstance]->dsKsvHash;
    // Reset new hash value
    pHdcp[cInstance]->dsKsvHash = 0;
    // Reset number of bytes in tail storage
    pHdcp[cInstance]->hashWordLen = 0;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Mixing hash function.
//!
//-------------------------------------------------------------------------------------------------

static uint32_t Mixer(uint32_t word)
{
    uint8_t i;

    // Mixing hash function. Apply many times
    for (i = 0; i < 32; i++)
    {
        word = 0x9069 * (word & 0xFFFF) + (word >> 16);
    }

    return word;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Add new input to KSV hash algorithm.
//!
//! @param[in]  pData   - pointer to KSV data
//! @param[in]  len     - number of bytes in pData[] buffer
//! @param[in]  isLast  - flag of the last portion of KSV list
//-------------------------------------------------------------------------------------------------

static void AddBytesToKsvHash( BYTE cInstance, uint8_t *pData, uint8_t len, bool_t isLast)
{
    uint8_t i;
    uint32_t word;
    uint8_t bytesLeft = len +  pHdcp[cInstance]->hashWordLen;

    while (bytesLeft >= 4)
    {
        // consume 32 bits
        word = 0;
        for (i = 0; i < 4; i++)
        {
            if (pHdcp[cInstance]->hashWordLen != 0)
            {
                word |= (pHdcp[cInstance]->hashWord & 0xFF) << (i*8);
                pHdcp[cInstance]->hashWord >>= 8;
                pHdcp[cInstance]->hashWordLen--;
            }
            else
            {
                word |= (*pData++) << (i*8);
            }
            bytesLeft--;
        }

        word = Mixer(word);

        if (pHdcp[cInstance]->dsKsvHash == 0)
        { // very first word goes to hash stack
            pHdcp[cInstance]->dsKsvHash = word;
            continue;
        }
        else
        { // secondary words LSB will combine with LSB of the hash stack
            pHdcp[cInstance]->dsKsvHash = Mixer(((pHdcp[cInstance]->dsKsvHash & 0xFFFF) << 16) | (word & 0xFFFF));
        }
    }


    if (bytesLeft > 0)
    {
        // Put unprocessed tail to hashWord storage
        pHdcp[cInstance]->hashWord = 0;
        pHdcp[cInstance]->hashWordLen = bytesLeft;
        for (i = 0; i < bytesLeft; i++)
        {
            pHdcp[cInstance]->hashWord |= (*pData++) << (i*8);
        }

        // In case of last transaction, use zero padded hashWord immediately
        if (isLast)
        {
            pHdcp[cInstance]->dsKsvHash = Mixer(((pHdcp[cInstance]->dsKsvHash & 0xFFFF) << 16) | (pHdcp[cInstance]->hashWord & 0xFFFF));
        }
    }

    // When KSV list Hash calculation is complete,
    // set "Same Downstream Device Connected" status flag
    if (isLast)
    {
        pTx[cInstance]->status.isSameDsConnected = (pHdcp[cInstance]->dsKsvHash == pHdcp[cInstance]->dsKsvHashPrev);
        DEBUG_PRINT(TX_MSG_ALWAYS, "KSV HASH: %s downstream detected.\n", pTx[cInstance]->status.isSameDsConnected ? "Same" : "New");
    }

}


//-------------------------------------------------------------------------------------------------
//! @brief      Unit Test for KsvHash.
//-------------------------------------------------------------------------------------------------

#if (SI_TX_COMP_UNIT_TESTS == SI_ENABLE)
static void TestKsvHash( BYTE cInstance)
{
    uint8_t i,j;
    uint8_t data[45] =
        {
                0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x0a,
                0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x1a,
                0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x2a,
                0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x3a,
                0x40,0x41,0x42,0x43,0x44
        };

    for (j = 0; j < 10; j++)
    {
        // little change in one byte should change the hash dramatically
        data[0] = j;

        NewKsvHash( cInstance );

        for (i = 0; i < 4; i++)
        {
            AddBytesToKsvHash( cInstance ,&data[i * 10], 10, false);
        }

        AddBytesToKsvHash( cInstance ,&data[40], 5, true);

        DEBUG_PRINT(TX_MSG_ALWAYS,"KSV HASH TEST:  val = 0x%04x%04x\n", (int) ((pHdcp[cInstance]->dsKsvHash & 0xffff0000) >> 16),
                (int) (pHdcp[cInstance]->dsKsvHash & 0x0000ffff));
    }
}
#endif


//-------------------------------------------------------------------------------------------------
//! @brief      Mute AV during HDCP transient states.
//!
//!             Muting is achieved either by enabling AV Mute bit in GCP, or by CTRL3 gating
//!             (HDCP encryption disabling).
//!
//! @param[in]  isMuted - mute request flag.
//-------------------------------------------------------------------------------------------------

static void AvMuteEnable( BYTE cInstance, bool_t isMuted)
{
    // Never unmute internally, if in repeater mode
    if (pHdcp[cInstance]->isRepeaterMode)
    {
        SiiDrvTpiHdcpEncriptionEnable( cInstance , !isMuted);
    }
    else
    {
        TxInfoAvMute( cInstance,isMuted);
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Clear any pending events or activity in HDCP hardware (Reset HDCP FSM).
//-------------------------------------------------------------------------------------------------

static void CancelHdcpAuthentication( BYTE cInstance)
{
    switch (pHdcp[cInstance]->authState)
    {
        case SI_TX_HDCP__WAIT_FOR_DONE:
        case SI_TX_HDCP__WAIT_FIFO_READY:
        case SI_TX_HDCP__KSV_FIFO_READ:
	 case SI_TX_HDCP__V_CHECK:						
            AvMuteEnable(cInstance,true);
            SiiDrvTpiHdcpProtectionEnable(cInstance,false);
			//UART1_SendBytes("CancelHdcpAuthentication\r\n",strlen("CancelHdcpAuthentication\r\n"));

            // Reset HDCP engine by temporal switching output TMDS OFF.
#if (SI_TX_TMDS_TOGGLE_FOR_HDCP_RESET == SI_ENABLE)
            // SWWA:18705
            TxTmdsBlankPause(cInstance);
#endif
            break;
        default:
            break;
    }
}


static void ProcessBstatus( BYTE cInstance,uint8_t dsBstatus[2])
{
    uint8_t dsCount = dsBstatus[0] & MSK_HDCP_DDC__BSTATUS1__DEV_COUNT;
    pHdcp[cInstance]->fifoByteCounter = dsCount * LEN_HDCP_DDC__BKSV; // DS Bksv list length

    if (pHdcp[cInstance]->isRepeaterMode)
    {
        if(SiiTxCbHdcpProcessDsBstatus( cInstance, dsBstatus))
        {
            pHdcp[cInstance]->error |= ERR_HDCP__DEV_LIMIT;
        }
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Send BKSV to US device FIFO.
//-------------------------------------------------------------------------------------------------

static void SendBksvToUsFifo(BYTE cInstance)
{
    if(pHdcp[cInstance]->isRepeaterMode)
    {
        // Read BKSV only for repeater mode
        uint8_t aDsBksv[LEN_HDCP_DDC__BKSV];

        // Copy Bksv to the FIFO
        SiiDrvTpiBksvGet(cInstance, aDsBksv);

        SiiTxCbHdcpFinishUsPart2(cInstance,aDsBksv);
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Finalize HDCP Authentication Part 2 by sending BKSV to US.
//!
//!             Note: in authentication failure case,
//!             transition to SI_TX_HDCP__FAILED state is done in TxHdcpTimerHandler()
//-------------------------------------------------------------------------------------------------

static void FinishAuthPart2( BYTE cInstance)
{
    if(SiiDrvTpiHdcpIsAuthenticationGood( cInstance ))
    {
        SendBksvToUsFifo( cInstance );
        pHdcp[cInstance]->authState = SI_TX_HDCP__AUTHENTICATED;
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Copy a portion of KSV list from DS to an upper layer.
//!
//! @return     Flag of the KSV list last portion.
//! @retval     true - on the last portion of KSVs.
//-------------------------------------------------------------------------------------------------

static bool_t CopyKsvListPortion( BYTE cInstance)
{
    uint8_t aKsvBuffer[VAL_TPI__HDCP_FIFO_BUFFER_LENGTH];
    uint8_t bytesToRead;
    bool_t  isDone;

    isDone = SiiDrvTpiKsvListPortionSizeGet( cInstance, &bytesToRead);

    if((pHdcp[cInstance]->fifoByteCounter != 0) && (bytesToRead != 0))
    {
        if(bytesToRead > pHdcp[cInstance]->fifoByteCounter) // extra check
        {
            DEBUG_PRINT(TX_MSG_ERR, "HDCP Error: Incorrect number of KSVs!\n");
            pHdcp[cInstance]->error |= ERR_HDCP__INTERNAL;
            bytesToRead = pHdcp[cInstance]->fifoByteCounter;
            isDone = true;
        }

        // get DS BKSV list
        SiiDrvTpiKsvListGet(cInstance,aKsvBuffer, bytesToRead);

        if (pHdcp[cInstance]->isRepeaterMode)
        {
            // Forward portion of KSV list to upper layer for processing
            SiiTxCbHdcpProcessKsvListPortion(cInstance,aKsvBuffer, bytesToRead);

            // Add new portion to the hash
            AddBytesToKsvHash(cInstance,aKsvBuffer, bytesToRead, isDone);
        }

        pHdcp[cInstance]->fifoByteCounter -= bytesToRead;
    }

    return isDone;
}


//-------------------------------------------------------------------------------------------------
//! @brief      HDCP Timer event handler.
//!
//!             This function is to be called periodically. The time past from the last call
//!             should be indicated as a parameter.
//!
//! @param[in]  timeDelta - time in ms elapsed since the last call.
//-------------------------------------------------------------------------------------------------

static void OnTimer( BYTE cInstance,time_ms_t timeDelta)
{
    uint8_t ksvListSize;
    bool_t  isKsvListReadingDone;
    uint8_t txPlugStatus = SiiDrvTpiPlugStatusGet( cInstance );
#if (FPGA_BUILD == SI_ENABLE)
    // FPGA model doesn't support RSEN
    bool_t rsen = true;
#else
    bool_t rsen = ((txPlugStatus & SI_TX_STATUS_MASK_RX_SENSE) != 0);
#endif


    bool_t hp = ((txPlugStatus & SI_TX_STATUS_MASK_HPD) != 0);

    if (pHdcp[cInstance]->tmdsOnCountDownStart)  // TxHdcpOnOutputEnable() 设立起来
    {
        pHdcp[cInstance]->tmdsOnCountDownStart = false;
        if ((SiiEdidTxDsDeviceRemedyMaskGet(cInstance) & SI_TX_EDID_REMEDY_SP_TEST) == 0)
        {
            if (SiiEdidTxDsDeviceRemedyMaskGet(cInstance) & SI_TX_EDID_REMEDY_DELAY_DS_AUTH)
            {
                pHdcp[cInstance]->tmdsCountdown = TIM_MS_HDCP__TMDS_ON_DELAY_MAX;
            }
            else
            {
                pHdcp[cInstance]->tmdsCountdown = TIM_MS_HDCP__TMDS_ON_DELAY_MIN;
            }
        }
    }

    switch (pHdcp[cInstance]->authState)
    {

        case SI_TX_HDCP__KSV_FIFO_READ:
            if(!SiiTxCbHdcpPrepareForKsvListProcessing( cInstance ))
            {
               // Keep waiting if repeater isn't ready to continue
               break;
            }
            isKsvListReadingDone = SiiDrvTpiKsvListPortionSizeGet( cInstance, &ksvListSize);

//           NOTE: the following debug message is commented to avoid adverse delays caused by printing
           DEBUG_PRINT(TX_MSG_DBG, "fifo level before = 0x%02X, done: %s\n", (int) ksvListSize,
                                    isKsvListReadingDone ? "Y" : "N");
               //DEBUG_PRINT( "fifo level before = 0x%02X, done: %s\n", (int) ksvListSize,  isKsvListReadingDone ? "Y" : "N");
		if (CopyKsvListPortion( cInstance ))
		{
//          NOTE: the following read and debug message is commented to avoid adverse delays caused by printing
//          isKsvListReadingDone = SiiDrvTpiKsvListPortionSizeGet(&ksvListSize);
			DEBUG_PRINT(TX_MSG_DBG, "fifo level after = 0x%02X, done: %s\n", (int) ksvListSize,
					                 isKsvListReadingDone ? "Y" : "N");
     // DEBUG_PRINT( "fifo level after = 0x%02X, done: %s\n", (int) ksvListSize,  isKsvListReadingDone ? "Y" : "N");
			pHdcp[cInstance]->authState = SI_TX_HDCP__V_CHECK;
		}
		// don't break here to expedite switching to the "V check" phase
			
        case SI_TX_HDCP__V_CHECK:
			// Downstream HDCP Part 2 is successfully finished,
			// after downstream V' matches own V.
			if (SiiDrvTpiHdcpIsPart2Done( cInstance ))
			{
				FinishAuthPart2( cInstance );
			}
            break;

        case SI_TX_HDCP__OFF:
            if (pHdcp[cInstance]->isAuthRequested)
            {
                pHdcp[cInstance]->authState = SI_TX_HDCP__WAIT_FOR_START;

                AvMuteEnable( cInstance, true);

                SiiDrvTpiHdcpProtectionEnable(cInstance,false);

                pHdcp[cInstance]->timeoutForPart1AndPart2 = TMO_MS_HDCP__MAX_AUTH_ATTEMPT;
                pHdcp[cInstance]->timeoutForDdcNack = TMO_MS_HDCP__MAX_DDC_NACK;

                if (pHdcp[cInstance]->isRepeaterMode)
                {
                    SiiTxCbHdcpSetUsRepeaterBcaps( cInstance, false);
                }
            }
            else if (pHdcp[cInstance]->dsDdcAckCheck)
            {
                if (SiiDrvTpiIsDownstreamHdcpAvailable( cInstance ))
                {
                    if (pHdcp[cInstance]->isRepeaterMode)
                    {
                        SiiTxCbHdcpDsDdcAck( cInstance, true);
                    }
                    pHdcp[cInstance]->dsDdcAckCheck = false;
                }
            }
            break;

        case SI_TX_HDCP__WAIT_FOR_START:
            // control comes here when HDCP authentication was requested from US device

            if (TxIsRunning( cInstance ) && hp && rsen && TxIsTmdsEnabled( cInstance ) && (pHdcp[cInstance]->tmdsCountdown == 0))
            {
                // TX chip input is provided and stable
                // (If scaler or/and OSD are installed in the schematic,
                // their output signals are provided and stable as well as their input ones)
                if (SiiDrvTpiIsDownstreamHdcpAvailable( cInstance )) // DDC ACK check
                {
                    // DS device must be ready to start HDCP authentication

                    SiiDrvTpiHdcpProtectionEnable( cInstance, false); // just in case
                    SiiDrvTpiHdcpProtectionEnable( cInstance, true); // start authentication
                    pHdcp[cInstance]->authState = SI_TX_HDCP__WAIT_FOR_DONE;

                    // DS DDC access was with ACK
                    pHdcp[cInstance]->dsDdcAckCheck = false;
                    pHdcp[cInstance]->error = 0;
                }
                else
                {
                    // DS DDC access was with NACK (i.e. NO ACK)
                    if (pHdcp[cInstance]->timeoutForDdcNack == 0)
                    {
                        // If the time is out and there is still no ACK,
                        // consider DS device as not having HDCP function.
                        pHdcp[cInstance]->error |= ERR_HDCP__NACK;
                        pHdcp[cInstance]->authState = SI_TX_HDCP__FAILED;
                        pHdcp[cInstance]->fbMsg = SI_TX_FB_MSG_ERR_HDCP_AUTH_FAILED;

                        SiiDrvTpiHdcpProtectionEnable(cInstance, false);

                        if (pHdcp[cInstance]->isRepeaterMode)
                        {
                        	//not fatal here to avoid repeater toggle HPD on Rx
                        	//repeater will use the DdcAck below to decide HPD toggle
                            SiiTxCbHdcpInformUsDeviceAboutFailure( cInstance, false);
                        }
                        //report the DDC NACK after Inform the fatal failure
                        if (pHdcp[cInstance]->upstreamDdcCtrl)
                        {
                            // No ACK on DS DDC bus
                            // Probably DS device has No HDCP function.
                            SiiTxCbHdcpDsDdcAck( cInstance, false);
                        }


                    }
                }

            }
            break;

        case SI_TX_HDCP__WAIT_FOR_RETRY:
            if (IS_UPDATED_TIMER_EXPIRED(pHdcp[cInstance]->timeToRetry, timeDelta))
            {
                pHdcp[cInstance]->authState = SI_TX_HDCP__OFF; //SI_TX_HDCP__WAIT_FOR_START;
                //AvMuteEnable(true); // 100218 for SI_TxHdcpForceDsAuthentication()
                //SiiDrvTpiHdcpProtectionEnable(false);
            }
            break;

        case SI_TX_HDCP__WAIT_FOR_DONE:
            break;

        case SI_TX_HDCP__WAIT_FIFO_READY:
            if (SiiDrvTpiIsZeroKsvListReady( cInstance ))
            {
                uint8_t timeOutMs = TMO_MS_HDCP__ZERO_KSV_READY_MAX;

                if(!SiiTxCbHdcpPrepareFor0KsvListProcessing( cInstance ))
                {
                    // Keep waiting if repeater isn't ready to continue
                	break;
                }
                // Wait until Part 2 is done after the above preparations stabilize
                while (!SiiDrvTpiHdcpIsPart2Done( cInstance ) && timeOutMs)
                {
                    delay_nms(2);//zzl SiiOsTimerWait(1);
                    timeOutMs--;
                }

                if (timeOutMs != 0) // Part 2 is done before the timeout
                {
                    uint8_t dsBstatus[2] = { 0, 0 };
                    ProcessBstatus( cInstance, dsBstatus);
                    FinishAuthPart2( cInstance );
                    NewKsvHash( cInstance ); // reset DS KSV list hash because it doesn't exist
                    pHdcp[cInstance]->authState = SI_TX_HDCP__AUTHENTICATED;
                }
                else
                {
                    // Try to re-authenticate if failed to receive KSV Ready (part 2 done)
                    TxHdcpRequestReauthentication( cInstance );
                }
            }
            else if(SiiDrvTpiHdcpIsPart2Done( cInstance ))
            {
                uint8_t dsBstatus[2] = { 0, 0 };
                ProcessBstatus( cInstance, dsBstatus);
                FinishAuthPart2( cInstance );
                NewKsvHash( cInstance ); // reset DS KSV list hash because it doesn't exist
                pHdcp[cInstance]->authState = SI_TX_HDCP__AUTHENTICATED;
            }
            break;

        case SI_TX_HDCP__AUTHENTICATED:
            if (pHdcp[cInstance]->minimumUnmuteCountdown == 0)
            {
                AvMuteEnable( cInstance, false);
                pHdcp[cInstance]->authState = SI_TX_HDCP__SECURE;
                pHdcp[cInstance]->fbMsg = SI_TX_FB_MSG_HDCP_AUTH_DONE;
            }
            break;

        case SI_TX_HDCP__SECURE:
            break;

        case SI_TX_HDCP__FAILED:
            if (pHdcp[cInstance]->isAuthRequested)
            {
             //hanjoy 20161116
		m_acHDCPTry[cInstance]++; 
		//  Output5VConctrol(WORD OutputChannel  ,BYTE OnOff)//  1表示开   0表示低
		if(m_acHDCPTry[cInstance]> 5) 
		{
	        //Output5VConctrol(cInstance   ,FALSE);//[hyj End: 20160517 14:25:15]
		SiiDrvTpiTmdsOutputEnable(cInstance, FALSE);
	//	delay_nms(1000);//zzl SiiOsTimerWait(1);
		SiiDrvTpiTmdsOutputEnable(cInstance, TRUE);
		//Output5VConctrol(cInstance   ,TRUE);//[hyj End: 20160517 14:25:15]
		}
                if (SiiDrvTpiIsDownstreamHdcpAvailable( cInstance))
                {
                    // check for DDC ACK;
                    pHdcp[cInstance]->dsDdcAckCheck = false;

                    SiiTxCbHdcpDsDdcAck( cInstance, true);

                    if (!SiiEdidTxDoesDsSupport( cInstance, SI_EDID_TX_SUPPORT_HDMI))
                    {
                        // shadow BCAPS is only available when DDC ACKs
                        if ((SiiDrvTpiBcapsGet( cInstance ) & BIT_HDCP_DDC__BCAPS__FAST_RE_AUTH) == 0)
                        {
                            // For devices not capable of fast re-authentication.
                            // (see page 19 (2.4) of HDCP 1.2)
                            // Temporary turn TMDS output OFF for minimum 100ms.
                            TxTmdsBlankPause( cInstance );
                        }
                    }

                    if (0 != pHdcp[cInstance]->timeoutForPart1AndPart2)
                    {
                        // If time for part one is not out,
                        // try another attempt of DS authentication.
                        pHdcp[cInstance]->authState = SI_TX_HDCP__WAIT_FOR_RETRY;
                        pHdcp[cInstance]->timeToRetry = TIM_MS_HDCP__RP_PART1_RETRY;
                    }
                }
                else
                {
                    pHdcp[cInstance]->dsDdcAckCheck = true;
                }

                if (pHdcp[cInstance]->timeoutForPart1AndPart2 == 0)
                {
                    if (pHdcp[cInstance]->isRepeaterMode)
                    {
                        // Repeater mode
                        SiiTxCbHdcpInformUsDeviceAboutFailure(cInstance,true);
                        pHdcp[cInstance]->isReauthRequested = false;
                        pHdcp[cInstance]->isAuthRequested = false;
                    }
                    else
                    {
                        // TX Only mode
                        pHdcp[cInstance]->isReauthRequested = true;
                    }
                    pHdcp[cInstance]->authState = SI_TX_HDCP__OFF;
                }
            }
            else
            {
                // Authentication is not required.
                // Do nothing.
            }
            break;

        default:
            break;
    }

    // Update HDCP timers
    UPDATE_TIMER(pHdcp[cInstance]->timeoutForPart1AndPart2, timeDelta);
    UPDATE_TIMER(pHdcp[cInstance]->timeoutForDdcNack, timeDelta);
    UPDATE_TIMER(pHdcp[cInstance]->minimumUnmuteCountdown, timeDelta);
    UPDATE_TIMER(pHdcp[cInstance]->tmdsCountdown, timeDelta);

    PrintHdcpStatus(cInstance);
}



//-------------------------------------------------------------------------------------------------
//  TX HDCP API functions
//-------------------------------------------------------------------------------------------------
void TxHdcpStructInstanceSet( BYTE cInstance, uint8_t deviceIndex)
{
    if(deviceIndex < SII_NUM_TX)
    {
        pHdcp[cInstance] = &HdcpInstRec[cInstance][deviceIndex];
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Select Tx device instance in multi-transmitter system.
//!
//! @param[in]  device_idx - zero based index of an instance.
//-------------------------------------------------------------------------------------------------

#if SII_NUM_TX > 1
void TxHdcpInstanceSet( BYTE cInstance, uint8_t deviceIndex)
{
    if(deviceIndex < SII_NUM_TX)
    {
        pHdcp[cInstance] = &HdcpInstRec[cInstance][deviceIndex];
    }
}
#endif // SII_NUM_TX > 1


//-------------------------------------------------------------------------------------------------
//! @brief      Initialization of TX HDCP module.
//-------------------------------------------------------------------------------------------------

void TxHdcpInit( BYTE cInstance)
{
    memset(pHdcp[cInstance], 0, sizeof(txHdcpInstanceRecord_t));

    pHdcp[cInstance]->prevAuthState = SI_TX_HDCP__INITIAL; // will be changed in print_status()
    TxHdcpReset( cInstance );

    // option for unconditional re-authentication
    SiiDrvTpiHdcpDynamicAuthenticationEnable( cInstance ,true);

#if (SI_TX_COMP_UNIT_TESTS == SI_ENABLE)
	//test
    TestKsvHash( cInstance );
#endif
}


//-------------------------------------------------------------------------------------------------
//! @brief      TX HDCP module initialization
//!
//!             The function has to be called once during system initialization.
//!             The function provides TX HDCP module with configuration settings.
//!
//! @param[in]  isRepeaterMode     - true for repeater, false for pure source,
//! @param[in]  tmdsOnToUnmuteTime - Minimum time from turning TMDS ON to AV unmute (in 100ms units),
//! @param[in]  usDdcCtrl
//!             - true: upstream HDCP DDC enabling/disabling depends on downstream DDC
//!             - false: upstream HDCP DDC does not depend on downstream
//-------------------------------------------------------------------------------------------------

void TxHdcpConfig(BYTE cInstance, bool_t isRepeaterMode, time_ms_t tmdsOnToUnmuteTime, bool_t usDdcCtrl)
{
    pHdcp[cInstance]->isRepeaterMode = isRepeaterMode;
    pHdcp[cInstance]->tmdsOnToUnmuteTime = tmdsOnToUnmuteTime;
    pHdcp[cInstance]->upstreamDdcCtrl = usDdcCtrl;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Reset TX HDCP module without scheduling new authentication.
//!
//!             Resets HDCP function to initial state. It is needed on input change,
//!             going into of out of standby, etc.
//-------------------------------------------------------------------------------------------------

void TxHdcpStop( BYTE cInstance)
{
	TxHdcpReset( cInstance );
	pHdcp[cInstance]->isAuthRequested = false;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Request to start downstream authentication.
//!
//!             The function starts downstream authentication if the device was not
//!             authenticated before. If it is already authenticated,
//!             the function does nothing.
//-------------------------------------------------------------------------------------------------

void TxHdcpStart( BYTE cInstance)
{
    pHdcp[cInstance]->isAuthRequested = true;

	switch (pHdcp[cInstance]->authState)
	{
		case SI_TX_HDCP__AUTHENTICATED:
		case SI_TX_HDCP__SECURE:
			// The upstream KSV FIFO has already been preloaded,
			// no need doing downstream authentication.
			if (pHdcp[cInstance]->isRepeaterMode)
			{
				SiiTxCbHdcpUsOnlyAuthentication( cInstance );
			}
			break;
		case SI_TX_HDCP__FAILED:
			pHdcp[cInstance]->authState = SI_TX_HDCP__OFF;
			break;
		case SI_TX_HDCP__WAIT_FIFO_READY:
		case SI_TX_HDCP__WAIT_FOR_RETRY:
			// Terminate previous HDCP work, if in progress
			TxHdcpReset( cInstance );
			break;
		default:
			break;
	}

    PrintHdcpStatus( cInstance );
}


//-------------------------------------------------------------------------------------------------
//! @brief      TMDS Output Enable event handler.
//!
//!             This function schedules start of HDCP authentication after a short safety delay.
//!             Should be called whenever TMDS output has been enabled after disable state.
//!             Example: resuming from Standby state.
//-------------------------------------------------------------------------------------------------
//     --->RequestTmdsOutputEnable() 立即打开TMDS后调用
//
void TxHdcpOnOutputEnable( BYTE cInstance)
{
    pHdcp[cInstance]->tmdsOnCountDownStart = true;
    if(SiiEdidTxDsDeviceRemedyMaskGet( cInstance) & SI_TX_EDID_REMEDY_SP_TEST)
    {
        pHdcp[cInstance]->minimumUnmuteCountdown = 0;
    }
    else
    {
        // Wait after turning on before starting authentication
        pHdcp[cInstance]->minimumUnmuteCountdown = pHdcp[cInstance]->tmdsOnToUnmuteTime;

        // Increase the delay for noncompliant devices
        if(SiiEdidTxDsDeviceRemedyMaskGet( cInstance ) & SI_TX_EDID_REMEDY_DELAY_DS_AUTH)
        {
            if(pHdcp[cInstance]->minimumUnmuteCountdown < TIM_MS_HDCP__MIN_MUTE_TIME)
                pHdcp[cInstance]->minimumUnmuteCountdown = TIM_MS_HDCP__MIN_MUTE_TIME;
        }
    }
    DEBUG_PRINT( TX_MSG_DBG,"TX HDCP min mute time %d ms\n", (int) pHdcp[cInstance]->minimumUnmuteCountdown);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Reset the HDCP TX engine.
//!
//-------------------------------------------------------------------------------------------------

void TxHdcpReset( BYTE cInstance)
{
	CancelHdcpAuthentication( cInstance );

	pHdcp[cInstance]->authState = SI_TX_HDCP__OFF;
	pHdcp[cInstance]->error = 0;
	pHdcp[cInstance]->timeoutForPart1AndPart2 = 0;
	pHdcp[cInstance]->timeoutForDdcNack = 0;
	pHdcp[cInstance]->timeToRetry = 0;
	pHdcp[cInstance]->isReauthRequested = false;

	// Disable HDCP on TX as well, otherwise under some circumstances
	// the DS device could get encrypted data before authentication
	// (and the data is broken).
	SiiDrvTpiHdcpProtectionEnable( cInstance, false);

	pHdcp[cInstance]->dsDdcAckCheck = false;

	if (SiiDrvTpiIsDownstreamHdcpAvailable( cInstance ))
	{
		if (pHdcp[cInstance]->isRepeaterMode)
		{
			SiiTxCbHdcpDsDdcAck( cInstance, true);
		}
	}
	PrintHdcpStatus( cInstance );
}



//-------------------------------------------------------------------------------------------------
//! @brief      HDCP Interrupt handler routine.
//!
//!             More detailed description.
//!
//! @param[in]  interrupts - a 16-bit word; every bit represents an interrupt flag.
//-------------------------------------------------------------------------------------------------

void TxHdcpInterruptHandler( BYTE cInstance,uint16_t interrupts)
{

    if (interrupts & SI_TX_IRQ__HPD)
    {
        AvMuteEnable( cInstance, true);
        SiiDrvTpiHdcpProtectionEnable( cInstance, false);

        pHdcp[cInstance]->isAuthRequested = false;
        pHdcp[cInstance]->authState = SI_TX_HDCP__OFF;
    }

    if (interrupts & SI_TX_IRQ__RX_SENSE)
    {
        AvMuteEnable( cInstance, true);
        SiiDrvTpiHdcpProtectionEnable( cInstance, false);

        pHdcp[cInstance]->authState = SI_TX_HDCP__OFF;
    }

    if (interrupts & (SI_TX_IRQ__HDCP_CHNG))
    {
        uint8_t hdcpStatus = SiiDrvTpiHdcpStatusGet( cInstance );

        PrintHdcpQueryStatus(cInstance,hdcpStatus);
        m_acHDCPTry[cInstance] = 0; //[kyq Begin: 20151016 16:32:07]
        switch (hdcpStatus & MSK_TPI__HDCP_QUERY__STATUS)
        {
            case VAL_TPI__HDCP_QUERY__STATUS_NORMAL:
            case VAL_TPI__HDCP_QUERY__STATUS_SUSPENDED:
                if (pHdcp[cInstance]->authState == SI_TX_HDCP__WAIT_FOR_DONE)
                {
                    // Part 1 is done
                    if (hdcpStatus & BIT_TPI__HDCP_QUERY__DS_REPEATER)
                    {
                        pHdcp[cInstance]->authState = SI_TX_HDCP__WAIT_FIFO_READY;
                    }
                    else
                    {
                        // Part 1 is done and no downstream Part 2 needs to be done
                        uint8_t aDsBStatus[2] = { 0, 0 };
                        ProcessBstatus( cInstance ,aDsBStatus);
                        if (pHdcp[cInstance]->isRepeaterMode)
                        {
                            SiiTxCbHdcpPrepareForKsvListProcessing( cInstance );
                        }
                        SendBksvToUsFifo( cInstance );

                        pHdcp[cInstance]->authState = SI_TX_HDCP__AUTHENTICATED;
                    }
                }
                break;

            case VAL_TPI__HDCP_QUERY__STATUS_FAILED:
                // failure during authentication case
                AvMuteEnable( cInstance, true);
                SiiTxCbHdcpInformUsDeviceAboutFailure( cInstance, false);
                if (pHdcp[cInstance]->isRepeaterMode && ((hdcpStatus & BIT_TPI__HDCP_QUERY__DS_REPEATER)))
                {
                    uint8_t aDsBStatus[2] = { 0, 0 };
                    SiiDrvTpiBStatusGet( cInstance, aDsBStatus);
                    // Note, in case of R0 failure (for example) the BSTATUS shadow registers
                    // are not read, but since the registers value is automatically cleared
                    // on clearing 0x2A.0, the EXCEED bits are not set. By checking the EXCEED bits,
                    // it is possible to distinguish the DEV_EXCCED and CASCADE_EXCEED
                    // failures from any other failure.
                    if ((aDsBStatus[0] & BIT_HDCP_DDC__BSTATUS1__DEV_EXC) || (aDsBStatus[1] & BIT_HDCP_DDC__BSTATUS2__CAS_EXC))
                    {
                        ProcessBstatus( cInstance, aDsBStatus);
                        if (pHdcp[cInstance]->isRepeaterMode)
                        {
                            SiiTxCbHdcpSetUsRepeaterBcaps( cInstance, true);
                        }
                        pHdcp[cInstance]->error |= ERR_HDCP__DS_EXCEED;
                    }
                }
                SiiDrvTpiHdcpProtectionEnable( cInstance, false);
                if( (pHdcp[cInstance]->authState == SI_TX_HDCP__KSV_FIFO_READ)
					||(pHdcp[cInstance]->authState == SI_TX_HDCP__V_CHECK))					
                {
                    // error happened during the KSV list reading,
                    // need to restart the DDC access state machine.
                    SiiDrvTpiDdcMasterReset( cInstance );
                    // Most probably the error is caused by V mismatch.
                    pHdcp[cInstance]->error |= ERR_HDCP__V;
                }
                else if (pHdcp[cInstance]->authState == SI_TX_HDCP__WAIT_FIFO_READY)
                {
                    // error happened while waiting for KSV FIFO READY bit
                    pHdcp[cInstance]->error |= ERR_HDCP__P2_TO;
                }
                pHdcp[cInstance]->authState = SI_TX_HDCP__FAILED;
                break;

            case VAL_TPI__HDCP_QUERY__STATUS_LOST:
                // failure on Part 3 case
                pHdcp[cInstance]->authState = SI_TX_HDCP__FAILED;
                AvMuteEnable( cInstance, true);
                SiiTxCbHdcpInformUsDeviceAboutFailure( cInstance, true);
                SiiDrvTpiHdcpProtectionEnable( cInstance, false);
                break;
        }

    }

    if (interrupts & SI_TX_IRQ__KSV_READY)
    {
        if (pHdcp[cInstance]->authState == SI_TX_HDCP__WAIT_FIFO_READY)
        {
            uint8_t aDsBStatus[2];
            SiiDrvTpiBStatusGet( cInstance, aDsBStatus);
            ProcessBstatus( cInstance, aDsBStatus);

            // Start new KSV hash calculation
            NewKsvHash( cInstance );

            pHdcp[cInstance]->authState = SI_TX_HDCP__KSV_FIFO_READ;

        }
    }

    PrintHdcpStatus( cInstance );
}


//-------------------------------------------------------------------------------------------------
//! @brief      HDCP Timer event handler.
//!
//!             This function is to be called periodically.
//!
//! @param[in]  timeDelta - time in ms elapsed since the last call.
//!
//! @return     Maximum time interval (in ms) to wait before the next call.
//-------------------------------------------------------------------------------------------------
//   ----> SiiTxTaskProcess()  loop call
time_ms_t TxHdcpTimerHandler( BYTE cInstance,time_ms_t timeDelta)
{
    time_ms_t maxTimeBeforeNextCallMs = TIM_MS_HDCP__HANDLER_DEFAULT_INVOCATION;

    if (pHdcp[cInstance]->isReauthRequested)
    {
        // The reason to use dedicated flag (pHdcp[cInstance]->isReauthRequested)
        // is to align pHdcp[cInstance]->timeToRetry counter with HDCP task time.
        pHdcp[cInstance]->isReauthRequested = false;

        CancelHdcpAuthentication( cInstance );

        pHdcp[cInstance]->authState = SI_TX_HDCP__WAIT_FOR_RETRY;
        pHdcp[cInstance]->timeToRetry = TIM_MS_HDCP__DS_PART1_RETRY;
    }
    else
    {
        OnTimer(cInstance,timeDelta);

        switch (pHdcp[cInstance]->authState)
        {
            case SI_TX_HDCP__WAIT_FOR_START:
                // Time critical case for repeaters to pass 100ms AKSV propagation test.
            case SI_TX_HDCP__KSV_FIFO_READ:
	     case SI_TX_HDCP__V_CHECK:				
                // Time critical case for repeaters to process 127KSVs in 500ms.
                maxTimeBeforeNextCallMs = TIM_MS_HDCP__HANDLER_URGENT_INVOCATION;
                break;

            default:
                if (pHdcp[cInstance]->tmdsCountdown)
                {
                    maxTimeBeforeNextCallMs = pHdcp[cInstance]->tmdsCountdown;
                }
        }
    }

    return maxTimeBeforeNextCallMs;
}
 

//-------------------------------------------------------------------------------------------------
//! @brief      Schedules HDCP re-authentication if HDCP is in operational state.
//-------------------------------------------------------------------------------------------------
//  需要重新认证
void TxHdcpRequestReauthentication( BYTE cInstance)
{
    switch (pHdcp[cInstance]->authState)
    {
        case SI_TX_HDCP__WAIT_FOR_DONE:
        case SI_TX_HDCP__WAIT_FIFO_READY:
        case SI_TX_HDCP__KSV_FIFO_READ:
        case SI_TX_HDCP__V_CHECK:
        case SI_TX_HDCP__AUTHENTICATED:
        case SI_TX_HDCP__SECURE:
            pHdcp[cInstance]->isReauthRequested = true;
            AvMuteEnable( cInstance ,true);
            SiiDrvTpiHdcpProtectionEnable( cInstance ,false);
            DEBUG_PRINT(TX_MSG_ALWAYS, "HDCP: Re-authentication request\n");
            break;
        default:
            break;
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return HDCP error code. @see HDCP_ERRORS
//-------------------------------------------------------------------------------------------------

uint16_t TxHdcpLastErrorGet( BYTE cInstance)
{
    return pHdcp[cInstance]->error;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return HDCP message reflecting HDCP engine current state.
//-------------------------------------------------------------------------------------------------

txFeedbackMsg_t TxHdcpLastMessageGet( BYTE cInstance)
{
    txFeedbackMsg_t fbMessage = pHdcp[cInstance]->fbMsg;
    pHdcp[cInstance]->fbMsg = SI_TX_FB_MSG_NONE; // clear after reporting

    return fbMessage;
}


