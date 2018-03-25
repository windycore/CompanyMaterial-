//***************************************************************************
//!file     si_repeater_sha.c
//!brief    Silicon Image Repeater component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_common.h"
#include "si_repeater_component_internal.h"
#include "si_connection_monitor.h"
#include "si_drv_repeater.h"
#include "si_edid_tx_component.h"
#include "si_drv_repeater_sha.h"



repeaterShaRequest_t 	sha_request_q[DevNum][SII_NUM_REPEATER]; /*buffer to save Rx SHA request*/
uint8_t	req_index[DevNum];
bool_t	first_sha_request[DevNum];		/*indicates first request, if true don't need to check HDCP status*/

//-------------------------------------------------------------------------------------------------
//! @brief      put SHA request to the queue
//-------------------------------------------------------------------------------------------------
static void ShaRequestPutInQue( BYTE cInstance, repeaterShaRequest_t sReq)
{
	uint_t i;
	/*only Rx request to be in the queue. Tx should request again if SHA busy*/
	if (sReq.isTx)
	{
		return;
	}
	/*ignore same request*/
	if (req_index[cInstance] != 0)
	{
		for (i = 0; i < SII_NUM_REPEATER; i++)
		{
			if (sha_request_q[cInstance][i].portNum == sReq.portNum)
				return;
		}
	}
	sha_request_q[cInstance][req_index[cInstance]] = sReq;
	DEBUG_PRINT( RPT_MSG_DBG, "SHA req put in queue %s %d q# %d\n ", sha_request_q[cInstance][0].isTx ? "Tx":"Rx", sha_request_q[cInstance][0].portNum, req_index[cInstance]);
	req_index[cInstance]++;

}

//-------------------------------------------------------------------------------------------------
//! @brief      get the next SHA request from the queue
//-------------------------------------------------------------------------------------------------
static repeaterShaRequest_t ShaRequestGetFromQue( BYTE cInstance)
{
	uint_t i;
	repeaterShaRequest_t 	sha_current_p;
	sha_current_p = sha_request_q[cInstance][0];

	req_index[cInstance]--;

	/*all move one step forward*/
	for (i=0;i<req_index[cInstance];i++)
	{
		sha_request_q[cInstance][i] = sha_request_q[cInstance][i+1];
	}
	DEBUG_PRINT( RPT_MSG_DBG, "SHA req get from queue %s %d q# %d\n ", sha_current_p.isTx ? "Tx":"Rx", sha_current_p.portNum, req_index[cInstance]);

	return sha_current_p;
}

//-------------------------------------------------------------------------------------------------
//! @brief      service the SHA request
//-------------------------------------------------------------------------------------------------
void RepeaterShaHandler(  BYTE cInstance )
{
	if ((req_index[cInstance])&&(!SiiDrvShaInUse( cInstance )))
	{
		DEBUG_PRINT( RPT_MSG_DBG, "queued SHA req\n ");
		SiiDrvShaRequestStart( cInstance, ShaRequestGetFromQue(cInstance));
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief      init data structure
//-------------------------------------------------------------------------------------------------
bool_t RepeaterShaHandlerInitialize (  BYTE cInstance )
{
     memset(&sha_request_q[cInstance][0], 0x00, sizeof(repeaterShaRequest_t) * SII_NUM_REPEATER);
     req_index[cInstance] = 0;
	 SiiDrvShaInit( cInstance );
     return( true );
}

//-------------------------------------------------------------------------------------------------
//! @brief      force SHA pointer to a TX in TX only mode
//! @param[in]  force  - force the SHA request regardless of its availability
//!             srq - the SHA request
//-------------------------------------------------------------------------------------------------
void SiiRepeaterShaRequest ( BYTE cInstance, bool_t force, repeaterShaRequest_t srq )  //add switchConf to 
{
	if (force) //unconditionally, used when application is sure SHA is not used
	{
		SiiDrvShaRequestStart( cInstance, srq);
	}
	else
	{
		if (!SiiDrvShaInUse( cInstance ))
		{
			SiiDrvShaRequestStart( cInstance, srq);
		}
		else
		{
			ShaRequestPutInQue( cInstance, srq);
		}
	}

}

//-------------------------------------------------------------------------------------------------
//! @brief      set the SHA length as a parameter for SHA calculation
//-------------------------------------------------------------------------------------------------
void RepeaterShaSetLength( BYTE cInstance, uint16_t slength)
{
	SiiDrvSetShaLength( cInstance, slength);
}






