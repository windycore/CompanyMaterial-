//***************************************************************************
//!file     si_drv_repeater_sha.c
//!brief    Silicon Image Repeater component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "si_common.h"
#include "si_drv_repeater_sha.h"
#include "si_regs_repeater953x.h"
#include "si_hdcp_ddc.h"
#include "si_drv_internal.h"
#include "si_tx_component.h"
#include "si_drv_tpi_system.h"

#define KSV_FIFO_SEL_CLEAR	0xFF
#define TX0_KSV_LENGTH(x)	(x)
#define TX1_KSV_LENGTH(x)	(x + 0x140)

RepeaterTopoMode_t currentMode[DevNum];
repeaterShaRequest_t currentSrq[DevNum];
bool_t firstTime;

void SiiDrvShaInit( BYTE cInstance)
{
	currentMode[cInstance] = SI_RPT_MODE_INITIAL;
	currentSrq[cInstance].isTx = false;
	currentSrq[cInstance].manualTxStart = false;
	currentSrq[cInstance].portNum = 0;
	currentSrq[cInstance].isValid = false;
	firstTime = true;
}


repeaterShaRequest_t SiiDrvGetCurrentShaReq( BYTE cInstance)
{
	return currentSrq[cInstance];
}
//-------------------------------------------------------------------------------------------------
//! @brief      Set the current switch mode
//!				single repeater - InstraPort mode
//!				multiple repeater - Matrix mode
//-------------------------------------------------------------------------------------------------

void SiiDrvShaSetSwitchMode( BYTE cInstance, RepeaterTopoMode_t mode)
{
	currentMode[cInstance] = mode;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Monitors if the SHA is busy
//!
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvShaInUse( BYTE cInstance)
{
    bool_t result = true;
    uint8_t regVal = 0;

    if (!currentSrq[cInstance].isValid)
    {
    	result = false; /*if not a valid req, not busy*/
    }
    else if (!(SiiRegRead( cInstance,  REG_HDCP_STAT ) & BIT_SHA_BUSY))
	{
		/*since KSV FIFO cannot be read/write until HDCP done for the previous requester*/
		if (currentSrq[cInstance].isTx)
		{
		    regVal =  SiiTxInstanceGet( cInstance );
			/*wait till HDCP done*/
#if (SII_NUM_TX > 1)
			SiiDrvTpiInstanceSet( cInstance ,currentSrq[cInstance].portNum);
#endif
			if (!SiiDrvTpiHdcpIsPart2Done( cInstance ))
			{
				result = true;
			}
			else
			{
				result = false;
			}
#if (SII_NUM_TX > 1)
			SiiDrvTpiInstanceSet( cInstance, regVal);
#endif
		}
		else
		{
			result = false; /*For rx, SHA busy bit is enough to indicate the SHA occupancy*/
		}
	}
	else
	{
		result = true;
	}
	return result;
}

//-------------------------------------------------------------------------------------------------
//! @brief      service the SHA request, point SHA to the right position
//! @param[in]  srq: the SHA requester 
//-------------------------------------------------------------------------------------------------

void SiiDrvShaRequestStart( BYTE cInstance, repeaterShaRequest_t srq)
{
	srq.isValid = true;
	currentSrq[cInstance] = srq;
	if (srq.manualTxStart)
	{
		if (srq.isTx)
		{
			SiiRegWrite( cInstance, REG_SHA_CTRL, VAL_SHA_DS_SWMODE); //manual start
			if (srq.portNum == 0)
			{
				SiiRegModify( cInstance, REG_SHA_CTRL3, BIT_SHA_START_TX0,SET_BITS); //write enable
			}
#if (SII_NUM_TX > 1)
			else if (srq.portNum == 1)
			{
				SiiRegModify( cInstance, REG_SHA_CTRL3, BIT_SHA_START_TX1,SET_BITS);
			}
#endif
		}
	}
	else /*SHA HW start*/
	{
		if (srq.isTx)
		{
			SiiDrvRepeaterKsvPointTo( cInstance, srq.portNum);
			SiiRegWrite( cInstance, REG_SHA_CTRL, BIT_SHA_EN); //no DS enable, HW does it itself
			DEBUG_PRINT( RPT_MSG_DBG, "Tx %d SHA req\n", srq.portNum);
		}
		else  /*rx ports*/
		{
			SiiDrvRepeaterKsvPointTo( cInstance, KSV_FIFO_SEL_CLEAR);
			SiiDrvShaAdjustKsvStartAddr(  cInstance, 0 );
			DEBUG_PRINT( RPT_MSG_DBG, "Rx %d SHA req\n", srq.portNum);
			SiiRegWrite( cInstance, REG_SHA_CTRL, BIT_SHA_EN|BIT_SHA_CTRL_MANUAL_US);
			if (currentMode[cInstance] == SI_RPT_MODE_SINGLE_REPEATER)
			{
				SiiRegWrite( cInstance, REG_SHA_CTRL, VAL_SHA_US_HWMODE);
			}
			else //matrix mode
			{
				SiiRegWrite( cInstance, REG_SHA_CTRL, VAL_SHA_US_SWMODE);
				//SiiRegWrite(REG_MS_US_PORT_REQ, 0x01<<srq.portNum);
			}
		}/*end of rx ports*/
	}
}

void SiiDrvShaRequestStop( BYTE cInstance, bool_t isTx, uint8_t portNumber)
{
	if (isTx)
	{
		if ((currentSrq[cInstance].isTx) && (currentSrq[cInstance].portNum == portNumber))
		{
			currentSrq[cInstance].isValid = false;
		}
	}
	else
	{
		if ((!currentSrq[cInstance].isTx) && (currentSrq[cInstance].portNum == portNumber))
		{
			currentSrq[cInstance].isValid = false;
		}
	}
}
//-------------------------------------------------------------------------------------------------
//! @brief      set the SHA length register
//! @param[in]  slength - SHA length of this SHA calculation
//-------------------------------------------------------------------------------------------------

void SiiDrvSetShaLength( BYTE cInstance, uint16_t slength)
{
	SiiRegWrite( cInstance, REG_SHA_LEN1, slength & 0xFF);
	SiiRegWrite( cInstance, REG_SHA_LEN2, (slength>>8) & 0xFF);
}

//-------------------------------------------------------------------------------------------------
//! @brief      set the Ksv start address
//! @param[in]  startPtr: Ksv start address
//-------------------------------------------------------------------------------------------------

void SiiDrvShaAdjustKsvStartAddr( BYTE cInstance, uint16_t startPtr )
{
	uint8_t temp;/*save the original FIFO select*/
	temp = SiiRegRead( cInstance, REG_EDID_FIFO_SEL);

	/*needs to clear the VAL_KSV_SELECT_MASK before modify the SHA start address*/
	SiiRegModify( cInstance, REG_EDID_FIFO_SEL, VAL_KSV_SELECT_MASK, CLEAR_BITS);
	SiiRegWrite( cInstance, REG_HDCP_KSV_SHA_START1, startPtr & 0xFF);
	SiiRegWrite( cInstance, REG_HDCP_KSV_SHA_START2, (startPtr>>8) & 0xFF);

	DEBUG_PRINT( RPT_MSG_DBG, "adjust start adr to %d \n ", startPtr);
	SiiRegWrite( cInstance, REG_EDID_FIFO_SEL, temp); /*write back the original setting.*/

}
//-------------------------------------------------------------------------------------------------
//! @brief      fill the ksv fifo with the BKSV(s) from DS(s), and set the ksv length registers
//!
//! @param[in]  selectedInstance - current instance
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterFillUsKsvInfo( BYTE cInstance, uint16_t ksv_length, uint8_t txsBksv[], uint8_t num_of_tx, uint8_t txIndx)
{

	if ( currentMode[cInstance] == SI_RPT_MODE_SINGLE_REPEATER )
	{
		SiiDrvShaAdjustKsvStartAddr( cInstance,  ksv_length );
		SiiDrvRepeaterKsvPointTo( cInstance, KSV_FIFO_SEL_CLEAR);
		SiiRegWriteBlock( cInstance, REG_KSV_FIFO, txsBksv, num_of_tx * LEN_HDCP_DDC__BKSV);

	}
	else if ( currentMode[cInstance] == SI_RPT_MODE_MULTIPLE_REPEATER )
	{
		uint8_t temp;
		temp = SiiRegRead( cInstance, REG_EDID_FIFO_SEL);

		SiiRegModify( cInstance, REG_EDID_FIFO_SEL, VAL_KSV_SELECT_MASK, CLEAR_BITS);
        //SHA is pointing to the current Tx.
		if (txIndx == 1)
		{
			SiiDrvShaAdjustKsvStartAddr(  cInstance, TX1_KSV_LENGTH(ksv_length));
		}
		else
		{
			SiiDrvShaAdjustKsvStartAddr(  cInstance, TX0_KSV_LENGTH(ksv_length));
		}

		SiiRegWriteBlock( cInstance, REG_KSV_FIFO, txsBksv, num_of_tx * LEN_HDCP_DDC__BKSV);
		SiiRegWrite( cInstance, REG_EDID_FIFO_SEL, temp); /*write back the original setting*/
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief      define which tx will forward the KSV first, for 1to2 case
//!
//! @param[in]  tx_number - current tx first
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterKsvPointTo( BYTE cInstance, uint8_t tx_number)
{
	SiiRegModify( cInstance ,REG_EDID_FIFO_SEL, VAL_KSV_SELECT_MASK, CLEAR_BITS);
	if ((tx_number == 0))
	{
		SiiRegModify( cInstance, REG_EDID_FIFO_SEL, VAL_KSV_SELECT_TX0, SET_BITS);
		DEBUG_PRINT( RPT_MSG_DBG, "KSV FIFO to Tx 0\n");

	}
	else if (tx_number == 1)
	{
		SiiRegModify( cInstance, REG_EDID_FIFO_SEL, VAL_KSV_SELECT_TX1, SET_BITS);
		DEBUG_PRINT( RPT_MSG_DBG, "KSV FIFO to Tx 1\n");

	}


}

//-------------------------------------------------------------------------------------------------
//! @brief      disable the auto SHA calculation started by HW
//!
//! @param[in]  none
//-------------------------------------------------------------------------------------------------

void SiiDrvHwAutoShaDisable( BYTE cInstance)
{
	SiiRegModify( cInstance, REG_SHA_CTRL, BIT_SHA_CTRL_MANUAL_US, CLEAR_BITS);
}




