//***************************************************************************
//!file     si_connection_monitor.c
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
#include "si_repeater_component_internal.h"
#include "si_connection_monitor.h"
#include "si_drv_repeater.h"
#include "si_tx_component.h"
#include "si_edid_tx_component.h"
#include "si_drv_tpi_system.h"

typedef struct
{
	bit_fld_t output_tmdson_new 			:1;
	bit_fld_t output_tmdson_old 			:1;
	bit_fld_t output_is_avmute				:1; //previous request status

	uint8_t	  overiden_hold_time;
	uint8_t	  avmute_mask;
	uint16_t  unplugTimeout;

} RepeaterConnectionState_t;

RepeaterConnectionState_t ConState[INSTANCE_9533][SII_NUM_TX];
RepeaterConnectionState_t *pCon[INSTANCE_9533] = 
{

    &ConState[0][0],
#if(INSTANCE_9533>1)
    &ConState[1][0],
#endif
#if(INSTANCE_9533>2)
    &ConState[2][0],
#endif
#if(INSTANCE_9533>3)
    &ConState[3][0],
#endif
#if(INSTANCE_9533>4)
    &ConState[4][0],
#endif
#if(INSTANCE_9533>5)
    &ConState[5][0],
#endif
#if(INSTANCE_9533>6)
    &ConState[6][0],
#endif
#if(INSTANCE_9533>7)
    &ConState[7][0],
#endif
};

#if 0
uint8_t SiiRepeaterConInstanceGet ()
{
#if (SII_NUM_TX > 1)
	uint8_t t = (pCon[cInstance] - &ConState[cInstance][0]);
	return t;
#endif //(SII_NUM_TX > 1)
}
#endif
//-------------------------------------------------------------------------------------------------
//! @brief      Enable the Rx input.
//-------------------------------------------------------------------------------------------------

static void EnableRxInput ( BYTE cInstance )
{
    DEBUG_PRINT( RPT_MSG_DBG, "enable input\n" );
    SiiDrvRepeaterSwitchRxHotPlug( cInstance, ON);
}

//-------------------------------------------------------------------------------------------------
//! @brief      request to turn the TMDS on or off.
//!				The actual TMDS turn on or off is done in a handler
//! @param[in]  qOn
//				- true:  TMDS on
//				- false: TMDS off
//-------------------------------------------------------------------------------------------------

static void ConnectionMonitorTmdsRequestSet( BYTE cInstance, bool_t qOn)
{
	pCon[cInstance]->output_tmdson_new = qOn;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Enables or disables Tx TMDS as appropriate
//!             Watches output disable requests and SCDT state to decide when to enable or disable
//!             TMDS output.  Also restores FakeVideo if it was present before turning TMDS off.
//!             state changes.
//-------------------------------------------------------------------------------------------------

static void MonitorTmds(  BYTE cInstance )
{
    if( pCon[cInstance]->output_tmdson_new != pCon[cInstance]->output_tmdson_old )
    {
        if (pCon[cInstance]->output_tmdson_new == ON)
        {
            if ( SiiDrvRepeaterRxSyncIsOn( cInstance) )      //scdt on
            {
                SiiRepeaterCbTxResume(cInstance);   //Tx TMDS on
            	pCon[cInstance]->output_tmdson_old = ON;
            }
        }
        else
        {
        	pCon[cInstance]->output_tmdson_old = OFF;
        	SiiRepeaterCbTxStandby(cInstance);     //to turn off Tx here.
        	SiiTxHdcpDisable( cInstance ); //if auto termination en, RX TMDS no interruption to turn off the Tx
            DEBUG_PRINT( RPT_MSG_DBG, "Tx TMDS turn OFF \n");
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Monitors the toggle of Rx Hot plug line is complete or not.
//!				After Rx is enabled again, request to turn of Tx TMDS
//! @param[in]  msDiff: time in ms from last time this function is called
//-------------------------------------------------------------------------------------------------

static void DsConnectionMonitor (  BYTE cInstance,clock_time_t   msDiff )
{
   	MonitorTmds( cInstance );

    if ( pCon[cInstance]->unplugTimeout == 0 )
    {
    	return;
    }

    if ( pCon[cInstance]->unplugTimeout > msDiff )
    {
    	pCon[cInstance]->unplugTimeout -= msDiff;
        /*DEBUG_PRINT( RPT_MSG_DBG, "Rx HPD time left %d\n",pCon[cInstance]->unplugTimeout);*/

    }
    else if ( pCon[cInstance]->unplugTimeout <= msDiff )
    {
    	pCon[cInstance]->unplugTimeout = 0;
        EnableRxInput(cInstance);	//time out over, toggle the Rx HPD high
    }
}

void SiiRepeaterConStructInstanceSet ( BYTE cInstance, uint8_t instance )
{
	pCon[cInstance] = &ConState[cInstance][instance];
}

//-------------------------------------------------------------------------------------------------
//! @brief      Brings an instance referred by given index into current context.
//!
//! @param[in]  instance - index of an instance record
//-------------------------------------------------------------------------------------------------
void SiiRepeaterConInstanceSet (  BYTE cInstance,uint8_t instance )
{
#if (SII_NUM_TX > 1)
	pCon[cInstance] = &ConState[cInstance][instance];
	SiiRepeaterCbTxSelect(instance);
	SiiDrvRxAudioMixInstanceSet(instance);
#endif //(SII_NUM_TX > 1)
}


//-------------------------------------------------------------------------------------------------
//! @brief     init data stucture
//!
//-------------------------------------------------------------------------------------------------
void SiiRepeaterConnectionMonitorInit ( BYTE cInstance )
{
	uint8_t i;
	for (i=0; i<SII_NUM_TX; i++)
	{
		ConState[cInstance][i].output_tmdson_new 	= OFF;
		ConState[cInstance][i].output_tmdson_old 	= OFF;
		ConState[cInstance][i].overiden_hold_time  = 0;
		ConState[cInstance][i].unplugTimeout 		= 0;
		ConState[cInstance][i].output_is_avmute    = 0;
		ConState[cInstance][i].avmute_mask         = 0;
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief      disable the Rx input
//!
//! @param[in]  disable_hot_plug_also
//				- true: toggles the HPD too.
//				- false: not toggle HPD
//-------------------------------------------------------------------------------------------------
void RepeaterConDisableInput ( BYTE cInstance, bool_t disable_hot_plug_also )
{
    DEBUG_PRINT( RPT_MSG_DBG, "disable input\n");

    if ( disable_hot_plug_also )
    {   //  emulates disconnection on RX side, disable the HPD, termination, HDCP DDC
    	SiiDrvRepeaterSwitchRxHotPlug(  cInstance, OFF );
    }
    //RDY bit will be cleared when new authentication starts
    //HW SHA is disabled in SDCT event
    SiiDrvRepeaterRxHdcpReset( cInstance );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Rx side handling when downstream HPD status change
//!
//! @param[in]  qOn
//				- true: enable the Rx side when there is active downstream.
//				- false: do nothing since we also have Tx0
//
//-------------------------------------------------------------------------------------------------
void RepeaterConOnTxHpdTurnOn(BYTE cInstance, bool_t qOn , bool_t sameTx)
{
    if ( !qOn )
    {
    	ConnectionMonitorTmdsRequestSet(  cInstance, OFF );
    }
    else
    {
        DEBUG_PRINT( RPT_MSG_DBG, "DS new connection\n");
		ConnectionMonitorTmdsRequestSet( cInstance, ON );
    }

    if (!sameTx) ////disable input both when On and Off
    {
    	RepeaterConDisableInput( cInstance, true );
        pCon[cInstance]->unplugTimeout = UNPLUG_TIMEOUT_MS;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Monitor the state of Rx and Tx connections.
//!
//! @param[in]  msThisDiff: time_period from last call
//-------------------------------------------------------------------------------------------------
clock_time_t RepeaterConnectionMonitor(BYTE cInstance, clock_time_t  msThisDiff )
{
	DsConnectionMonitor( cInstance, msThisDiff );
    return  msThisDiff;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Perform the Tx AVMUTE SET or Clear based on the mute sources status.
//!				To CLEAR: if all the source status clear; To SET: if any one status set
//-------------------------------------------------------------------------------------------------
static void UpdateTxAvMute( BYTE cInstance)
{
	if ( pCon[cInstance]->avmute_mask == 0 ) //clear mute
	{
		if ( pCon[cInstance]->output_is_avmute ) //if current status is different
		{
			SiiDrvTpiAvMuteSourceSet( cInstance, true );
			SiiDrvTpiAvMuteSendingEnable(cInstance, OFF);
			pCon[cInstance]->output_is_avmute = false;

		}
	}
	else //set mute
	{
		if ( !pCon[cInstance]->output_is_avmute ) //if current status is different
		{
			SiiDrvTpiAvMuteSourceSet( cInstance, true );
			SiiDrvTpiAvMuteSendingEnable(cInstance, ON);
			pCon[cInstance]->output_is_avmute = true;
		}
	}
}
//-------------------------------------------------------------------------------------------------
//! @brief      set the AVMUTE state with cause
//!
//! @param[in]  qOn
//				- true:  AVMUTE set
//				- false: AVMUTE clear
//				cause_mask
//				the initiator of the AVMUTE
//-------------------------------------------------------------------------------------------------
void RepeaterConAvMuteSet( BYTE cInstance, uint8_t cause_mask, bool_t qOn )
{
	if ( cause_mask )
	{
		if ( qOn )
		{
			pCon[cInstance]->avmute_mask |= cause_mask;
		}
		else
		{
			pCon[cInstance]->avmute_mask &= ~cause_mask;
		}
	}
	else
	{
		pCon[cInstance]->avmute_mask = 0;
	}

	UpdateTxAvMute( cInstance );
}




