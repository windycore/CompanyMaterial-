//***************************************************************************
//!file     si_repeater_component.c
//!brief    Silicon Image Repeater component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <string.h>
#include <stdio.h>
#include "si_repeater_component_internal.h"
#include "si_tx_component.h"
#include "si_connection_monitor.h"
#include "si_drv_repeater.h"
#include "si_hdcp_ddc.h"
#include "si_drv_repeater_sha.h"
#include "si_repeater_sha.h"


#define SII_MAX_KSV_NMB	64

//------------------------------------------------------------------------------
//  Repeater Component Instance Data
//------------------------------------------------------------------------------

RepeaterInstanceData_t repeaterIns[INSTANCE_9533][SII_NUM_REPEATER];
RepeaterInstanceData_t *pRpt[INSTANCE_9533] = 
{
    &repeaterIns[0][0],
#if(INSTANCE_9533>1)
    &repeaterIns[1][0],
#endif
#if(INSTANCE_9533>2)
    &repeaterIns[2][0],
#endif
#if(INSTANCE_9533>3)
    &repeaterIns[3][0],
#endif
#if(INSTANCE_9533>4)
    &repeaterIns[4][0],
#endif
#if(INSTANCE_9533>5)
    &repeaterIns[5][0],
#endif
#if(INSTANCE_9533>6)
    &repeaterIns[6][0],
#endif
#if(INSTANCE_9533>7)
    &repeaterIns[7][0],
#endif
};
uint8_t InputNoHdcp[INSTANCE_9533];



//-------------------------------------------------------------------------------------------------
//! @brief      Update last result structure and print an error message.
//!
//!             This function shall be used to report any error or warning in the component.
//!
//! @param[in]  code  - error or warning message code
//-------------------------------------------------------------------------------------------------
static void ReportIssue( BYTE cInstance,rptErrCode_t code)
{
	switch(code)
	{
		case SI_RPT_ERROR_CODE_INSTANCE_NOT_EXIST:
			DEBUG_PRINT( RPT_MSG_DBG, "repeater instance more than 2!!\n ");
			break;
		default:
			break;

	}
}

//-------------------------------------------------------------------------------------------------
//! @brief	   clear the HDCP status releated to the Tx
//!
//! @param[in]  txN   - the index of the Tx
//-------------------------------------------------------------------------------------------------
void RepeaterResetTxHdcpStatusData( BYTE cInstance,uint8_t txN)
{
	uint8_t i;
	uint8_t numberOfTx = txN + 1;
	uint8_t startingTx = txN;

	if(txN == RPT_ALL_TX)
	{
		numberOfTx = SII_NUM_TX;
		startingTx = 0;
	}

	for(i = startingTx; i<numberOfTx; i++)
	{
		pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxHdcpDone = false;
		pRpt[cInstance]->txOnThePipe[i].txStat.isHdcpRequested = false;
		pRpt[cInstance]->txOnThePipe[i].txStat.isBstatRdy = false;
		pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxKsvRdy = false;
		pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxShaDone = false;
		pRpt[cInstance]->txOnThePipe[i].txStat.isRptDsNoHdcp = false;
	}

}

void SiiRepeaterStructInstanceSet( BYTE cInstance, uint8_t instance)
{
	if (instance < SII_NUM_REPEATER)
    {
        pRpt[cInstance] = &repeaterIns[cInstance][instance];
    }	
}

//-------------------------------------------------------------------------------------------------
//! @brief	  selects the active repeater instance
//!
//! @param[in]  instance   - the index of the instance
//!             this is a cross instance function
//-------------------------------------------------------------------------------------------------
void SiiRepeaterInstanceSet(BYTE cInstance, uint8_t instance)
{
#if (SII_NUM_REPEATER > 1)
    if (instance < SII_NUM_REPEATER)
    {
        pRpt[cInstance] = &repeaterIns[cInstance][instance];
    	SiiDrvRepeaterInstanceSet(cInstance, instance);
    }
    else
    {
        ReportIssue(cInstance, SI_RPT_ERROR_CODE_INSTANCE_NOT_EXIST);
    }
#endif //(SII_NUM_REPEATER > 1)
}



//-------------------------------------------------------------------------------------------------
//! @brief	   Called to set up the SHA for tx from application, when in TPG mode without repeater
//!			
//! @param[in]  txNum   - enable SHA for which tx output DS
//! @param[in]  isStart - start or stop the SHA resource
//-------------------------------------------------------------------------------------------------
void SiiRepeaterForceShaSet (  BYTE cInstance,uint8_t txNum, bool_t isStart  )  //add switchConf to
{
	repeaterShaRequest_t srq;
	srq.isTx = true;
	srq.portNum = txNum;
	srq.manualTxStart = false;

	if (isStart)
	{
	    SiiDrvShaRequestStart( cInstance, srq);
	}
	else
	{
	    SiiDrvShaRequestStop( cInstance, srq.isTx, srq.portNum);
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief	   set up current sytem switch mode
//!
//! @param[in]  repeaterMode   - current switch mode
//-------------------------------------------------------------------------------------------------
void SiiRepeaterSwitchModeSet( BYTE cInstance, RepeaterTopoMode_t repeaterMode )
{
	SiiDrvRepeaterSetMode(cInstance, repeaterMode);
	RepeaterShaHandlerInitialize(cInstance);
	SiiDrvShaSetSwitchMode(cInstance,repeaterMode);
}
//-------------------------------------------------------------------------------------------------
//! @brief      Initialize repeater for operation in the requested mode.
//!             If Repeater option is enabled, init 2 repeater instances
//!
//! @param[in]  enable  - true enables repeater
//! @return     true: success
//!             this is a cross instance function
//-------------------------------------------------------------------------------------------------
bool_t SiiRepeaterInitialize ( BYTE cInstance, bool_t enable, bool_t isPowerUp)
{
    uint8_t i;
    if (enable)
    {
        for (i = 0; i < SII_NUM_REPEATER; i++)
        {
            memset(&repeaterIns[cInstance][i], 0x0, sizeof(RepeaterInstanceData_t));
        }
        SiiRepeaterConnectionMonitorInit( cInstance );
        SiiDrvRepeaterInit( cInstance );
        SiiRepeaterSwitchModeSet( cInstance, SI_RPT_MODE_INITIAL);
        InputNoHdcp[cInstance] = 0; //clear the input port non-HDCP status
        //no Rx HPD here. Will do when Tx connection is active
        //re-init every time when mode change.
        RepeaterShaHandlerInitialize( cInstance );
    }
    else
    {
        //when first power up as init, don't need to clear all status
        if(!isPowerUp)
        {
            SiiRepeaterSwitchModeSet ( cInstance, SI_RPT_MODE_SINGLE_REPEATER);//to clear all the inputs
            SiiDrvRepeaterRxHdcpReset( cInstance ); //reset Rx
            SiiDrvRepeaterEnable( cInstance, false);
            for (i = 0; i < SII_NUM_TX; i++)
            {
                SiiRepeaterConInstanceSet( cInstance, i); //use any tx.
                RepeaterConOnTxHpdTurnOn( cInstance, OFF, false);
            }
            for (i = 0; i < SII_NUM_REPEATER; i++)
            {
                repeaterIns[cInstance][i].repeaterActive = false;
            }
            //re-init every time when mode change.
            RepeaterShaHandlerInitialize( cInstance );
        }

    }


    return true;
}

//-------------------------------------------------------------------------------------------------
//! @brief      configure the repeater instance
//! @param[in]  txId  - the tx index
//!             qOn   - true: the tx is add to this repeater pipe
//!                     false: the tx is removed from the repeater pipe
//! @return     
//-------------------------------------------------------------------------------------------------
void SiiRepeaterPipeTxConnectionSet( BYTE cInstance,uint8_t txId, bool_t qOn)
{
	pRpt[cInstance]->txOnThePipe[txId].txStat.isRptTxOn = qOn;
	if( pRpt[cInstance]->txOnThePipe[txId].txStat.isRptTxHpdOn )//it was an active Tx
	{
		pRpt[cInstance]->txOnThePipe[txId].txStat.isRptTxHpdOn = false;
		pRpt[cInstance]->numberOfTx --;
        //if numberOfTx becomes 0, will change repeater mode later
	}

}

//-------------------------------------------------------------------------------------------------
//! @brief      configure the repeater instance
//! @param[in]  RepeaterInstanceData_t  - detailed instance configuration data
//! @return
//-------------------------------------------------------------------------------------------------
void SiiRepeaterPipeConfigureSet( BYTE cInstance,RepeaterInstanceData_t  *reptr)
{
    uint8_t i;
    bool_t rptActive = false;
	*pRpt[cInstance] = *reptr;

	RepeaterResetTxHdcpStatusData( cInstance, RPT_ALL_TX); //reset all the Tx HDCP status of the pipe

	pRpt[cInstance]->repeaterActive = false;
	pRpt[cInstance]->numberOfTx = 0;
	pRpt[cInstance]->numberOfTxAuthed = 0;
	pRpt[cInstance]->numberOfTxHasBstat = 0;

	for(i = 0; i<SII_NUM_TX; i++)
	{
		if(pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxOn)//the Tx on the pipe
		{
			pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxHpdOn = false;
			rptActive = true;
		}
	}
	if( !rptActive )
	{
		SiiDrvHwAutoShaDisable( cInstance );
		SiiDrvRepeaterEnable( cInstance, OFF); //in case the Tx is connected but changed to TPG/Parl
	}

}

//-------------------------------------------------------------------------------------------------
//! @brief      Manages repeater instance operations
//!
//! @param[in]  msDiff -  the time between last call and this
//-------------------------------------------------------------------------------------------------
void SiiRepeaterHandler (  BYTE cInstance,clock_time_t msDiff )
{
	uint8_t i;

	if(pRpt[cInstance]->repeaterActive)
	{
	    RepeaterIsrHandler( cInstance, msDiff, pRpt[cInstance]);
	    RepeaterShaHandler(cInstance);
	}
	/*for each of the TX on the repeater instance*/
	/*need to full fill the Rx HPLUG pulse even when after repeater mode disabled*/
	for (i = 0; i<SII_NUM_TX; i++)
	{
		if (pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxOn)
		{
			SiiRepeaterConInstanceSet(cInstance,i);
			RepeaterConnectionMonitor(cInstance,msDiff );
		}
	}

}



//-------------------------------------------------------------------------------------------------
//! @brief      Upstream authentication only, not change the downstream HDCP
//!
//! @param[in]  tx_number -  the tx that reported the message
//-------------------------------------------------------------------------------------------------
void SiiRepeaterHdcpUsOnlyAuthentication(  BYTE cInstance, uint8_t tx_number )
{
	uint8_t i;
	bool_t needReFreshHdcpData = false;
	repeaterShaRequest_t srq;
	srq.isTx = false;
	srq.portNum = pRpt[cInstance]->inputPort;
	srq.manualTxStart = false;
	

	DEBUG_PRINT( RPT_MSG_DBG, "US only auth\n ");
	SiiRepeaterConInstanceSet(cInstance,tx_number);
	RepeaterConAvMuteSet(cInstance,AV_MUTE_TX_IN_HDCP, OFF);

	if((pRpt[cInstance]->rxBstatus[1]& MSK_HDCP_DDC__BSTATUS2__DEV_DEPTH) != 0)
	{
		RepeaterShaSetLength(cInstance,pRpt[cInstance]->total_ksv_length); /*restore the SHA length*/
		SiiDrvRepeaterSetUsBstatus(cInstance,pRpt[cInstance]->rxBstatus);
		SiiRepeaterShaRequest(cInstance,false, srq);
		SiiDrvRepeaterSetBcapsFifoReady(cInstance,ON); //DDC still shows not ready, if the HW is not done yet.
	}
	else
	{
		for (i = 0; i<SII_NUM_TX; i++)
		{
			if (pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxOn
				&& pRpt[cInstance]->txOnThePipe[i].txStat.isHdcpRequested
				&& pRpt[cInstance]->txOnThePipe[i].txStat.isRptDsNoHdcp)
			{
				needReFreshHdcpData = true;
			}
		}
		if (needReFreshHdcpData)//DsNoHdcp status ready by now
		{
			for (i = 0; i<SII_NUM_TX; i++)
			{
				if (pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxOn
					&& pRpt[cInstance]->txOnThePipe[i].txStat.isHdcpRequested
					&& !pRpt[cInstance]->txOnThePipe[i].txStat.isRptDsNoHdcp)
				{
					//do not change the instance
					SiiRepeaterHdcpProcessDsBstatus(cInstance,pRpt[cInstance]->txOnThePipe[i].txBstatus, i);
					SiiRepeaterHdcpFinishUsPart2(cInstance,pRpt[cInstance]->txOnThePipe[i].txsBksv, i);
				}
			}
		}
	}
}




//-------------------------------------------------------------------------------------------------
//! @brief      process the downstream Bstatus and update the upstream Bstatus accordingly. 
//!			if KSV number exceeded the limit, return false
//!
//! @param[in]  dsBstatus - downstream Bstatus
//! @param[in]  tx_number -  the tx that reported the message
//! @return     -true: KSV is over limit
//!			    -false: KSV within limit
//-------------------------------------------------------------------------------------------------
bool_t SiiRepeaterHdcpProcessDsBstatus( BYTE cInstance, uint8_t dsBstatus[2],uint8_t tx_number )
{
	uint8_t ds_count = dsBstatus[0] & MSK_HDCP_DDC__BSTATUS1__DEV_COUNT;
	uint8_t ds_depth = dsBstatus[1] & MSK_HDCP_DDC__BSTATUS2__DEV_DEPTH;
	uint8_t our_bstatus[]={0,0}; /*compose repeater own BSTATUS and save it*/

	uint8_t numberOfTxHasBstat =0;
	uint8_t numberOfHdcpTx=0;
	bool_t overLimit = false;
	uint8_t i;

	DEBUG_PRINT( RPT_MSG_DBG, "$$$Tx bstatus =  %x  %x \n ",(int)dsBstatus[1], dsBstatus[0]);
	pRpt[cInstance]->txOnThePipe[tx_number].txStat.isBstatRdy= true;
 
	/* check for HW limitation*/
	if ( ds_count >= SII_MAX_KSV_NMB )
	{
		ds_count = SII_MAX_KSV_NMB;
		overLimit = true;
		DEBUG_PRINT( RPT_MSG_DBG, "max KSV number exceeded\n ");
		/* indicate that the limit is exceeded include only as much devices as could be placed into the FIFO*/
		pRpt[cInstance]->txOnThePipe[tx_number].txBstatus[0] = BIT_HDCP_DDC__BSTATUS1__DEV_EXC;

	}
	else
	{
		pRpt[cInstance]->txOnThePipe[tx_number].txBstatus[0] = ds_count;
	}


	if ( dsBstatus[0] & BIT_HDCP_DDC__BSTATUS1__DEV_EXC )
	{
		pRpt[cInstance]->txOnThePipe[tx_number].txBstatus[0] |= BIT_HDCP_DDC__BSTATUS1__DEV_EXC;
	}


	if (ds_depth < 7 )
	{
		/*adding self level is done at last Tx*/
		pRpt[cInstance]->txOnThePipe[tx_number].txBstatus[1] = ds_depth;
	}
	else/* no more room to indicate bigger depth 0x07 is the maximum depth*/
	{
		pRpt[cInstance]->txOnThePipe[tx_number].txBstatus[1] = BIT_HDCP_DDC__BSTATUS2__CAS_EXC | 0x07;
	}

	if ( dsBstatus[1] & BIT_HDCP_DDC__BSTATUS2__CAS_EXC )
	{
		pRpt[cInstance]->txOnThePipe[tx_number].txBstatus[1] |= BIT_HDCP_DDC__BSTATUS2__CAS_EXC;
	}

	pRpt[cInstance]->txOnThePipe[tx_number].ksv_length = ds_count * LEN_HDCP_DDC__BKSV;

	for ( i = 0; i < SII_NUM_TX; i++)/*start over from Tx HPD beginning*/
	{
		if (pRpt[cInstance]->txOnThePipe[i].txStat.isBstatRdy)
		{
			numberOfTxHasBstat++;
		}
		if ((pRpt[cInstance]->txOnThePipe[i].txStat.isHdcpRequested)
			&&(!pRpt[cInstance]->txOnThePipe[i].txStat.isRptDsNoHdcp))
		{
			numberOfHdcpTx++;
		}
	}

	if (numberOfTxHasBstat == numberOfHdcpTx)//last tx in the pipe		lastTx = true;
	{
		for (i=0; i<SII_NUM_TX; i++)
		{
			if (pRpt[cInstance]->txOnThePipe[i].txStat.isBstatRdy)
			{
				our_bstatus[0] += pRpt[cInstance]->txOnThePipe[i].txBstatus[0];
				if (pRpt[cInstance]->txOnThePipe[i].txBstatus[1] > our_bstatus[1])

					our_bstatus[1] = pRpt[cInstance]->txOnThePipe[i].txBstatus[1];
			}
		}

		/*BIT_HDCP_DDC__BSTATUS2__CAS_EXC should have been carried over*/
		if ( our_bstatus[1] < 7 )
		{
			our_bstatus[1]++;		/*add our own*/
		}
		else
		{
			our_bstatus[1] = BIT_HDCP_DDC__BSTATUS2__CAS_EXC | 0x07;
		}

		if ( our_bstatus[0]<=( SII_MAX_KSV_NMB - numberOfHdcpTx))
		{
			our_bstatus[0] += numberOfHdcpTx;  /* add our own device into the list*/
		}
		else
		{
			overLimit = true;
		}

		if (overLimit)
		{
			/* indicate that the limit is exceeded*/
			/* include only as much devices as could be placed into the FIFO*/
			our_bstatus[0] = BIT_HDCP_DDC__BSTATUS1__DEV_EXC;
		}
	}
	DEBUG_PRINT( RPT_MSG_DBG, "$$$ our bstatus =  %x  %x \n ",(int)our_bstatus[1],
			our_bstatus[0]);

	SiiDrvRepeaterSetUsBstatus( cInstance,our_bstatus );
	pRpt[cInstance]->rxBstatus[0] = our_bstatus[0]; /*save for future use*/
	pRpt[cInstance]->rxBstatus[1] = our_bstatus[1];
	return overLimit;

}

//-------------------------------------------------------------------------------------------------
//! @brief      finish the upstream part 2 of HDCP authentication
//!
//! @param[in]  aDsBksv - downstream BKSV
//! @param[in]  tx_number -  the tx that reported the message
//-------------------------------------------------------------------------------------------------
void SiiRepeaterHdcpFinishUsPart2(  BYTE cInstance,uint8_t aDsBksv[LEN_HDCP_DDC__BKSV], uint8_t tx_number )
{
	uint8_t i,j,n;
	repeaterShaRequest_t srq;
	uint8_t numberOfTxReq;

#if (CHECK_SAME_TX == 1)
	//if the DS is different, have to restart the HDCP from beginning
	if((pRpt[cInstance]->txOnThePipe[tx_number].wasAuthened == true)&& //skipped the HPD relay
	   (!SiiTxStatusGet().isSameDsConnected)) //status ready by now
	{
		pRpt[cInstance]->txOnThePipe[tx_number].wasAuthened = false;
		RepeaterResetTxHdcpStatusData( cInstance, tx_number);
		RepeaterConOnTxHpdTurnOn(  cInstance, true, false);  //turn on, not the same Tx
		DEBUG_PRINT( RPT_MSG_DBG, "$$$Tx %d is not the same as before, re-authentication starts\n ",(int)tx_number);
	}
	else
#endif //#if (CHECK_SAME_TX == 1)
	{
		pRpt[cInstance]->numberOfTxAuthed = 0; //calculate authened number of tx
		pRpt[cInstance]->total_ksv_length = 0;
		numberOfTxReq = 0;

		pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxShaDone = true;
		/*save the BKSV from the tx*/
		for (i=0; i<LEN_HDCP_DDC__BKSV; i++)
		{
			   pRpt[cInstance]->txOnThePipe[tx_number].txsBksv[i] = aDsBksv[i];
		}
		/*add the tx BKSV to the repeater BKSV*/
		n = 0;
		for (i=0;i<SII_NUM_TX;i++)
		{
			if (pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxShaDone)
			{
				pRpt[cInstance]->numberOfTxAuthed++;
				pRpt[cInstance]->total_ksv_length += pRpt[cInstance]->txOnThePipe[i].ksv_length;
				for (j=0; j<LEN_HDCP_DDC__BKSV; j++)
				{
					pRpt[cInstance]->txsBksv[n++]=pRpt[cInstance]->txOnThePipe[i].txsBksv[j];
				}
			}
			if ((pRpt[cInstance]->txOnThePipe[i].txStat.isHdcpRequested)
				&&(!pRpt[cInstance]->txOnThePipe[i].txStat.isRptDsNoHdcp))
				numberOfTxReq++;
		}
		/*for the last tx
		1. adjust pointer and add BKSV for the txs
		2. calculate the sha length
		3. sha point to the US*/
		if (pRpt[cInstance]->numberOfTxAuthed == numberOfTxReq) /*last one*/
		{
			SiiDrvRepeaterFillUsKsvInfo(cInstance,
					pRpt[cInstance]->total_ksv_length,
					pRpt[cInstance]->txsBksv,
					pRpt[cInstance]->numberOfTxAuthed,
					tx_number);

			pRpt[cInstance]->total_ksv_length += LEN_HDCP_DDC__BKSV * pRpt[cInstance]->numberOfTxAuthed;  //add last one
			RepeaterShaSetLength(cInstance, pRpt[cInstance]->total_ksv_length);
			srq.isTx = false;
			srq.portNum = pRpt[cInstance]->inputPort;
			srq.manualTxStart = false;

			/*clear the rdy bit of the input*/
			SiiDrvRepeaterSetBcapsFifoReady(cInstance, OFF);
			SiiRepeaterShaRequest(cInstance, false, srq);
			SiiDrvRepeaterSetBcapsFifoReady(cInstance, ON);
			DEBUG_PRINT( RPT_MSG_DBG, "$$$finish part2\n ");
		}
	}
}


//-------------------------------------------------------------------------------------------------
//! @brief      trun on / off the rx side Fifo ready bits
//-------------------------------------------------------------------------------------------------
void SiiRepeaterHdcpSetBcapsFifoRdyBits( BYTE cInstance, bool_t switchOn )
{
	SiiDrvRepeaterSetBcapsFifoReady(  cInstance, switchOn );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Called when the Tx DDC availability is set to true or false
//! @param[in]  isAcknowledged - DDC available or not
//!             tx_number -  the tx that reported the message

//-------------------------------------------------------------------------------------------------
void SiiRepeaterHdcpDsDdcAck( BYTE cInstance, bool_t isAcknowledged, uint8_t tx_number)
{
	bool_t wasAcknowledged;
	//not update if the tx is not in any repeater instance. if added later, the Rx will be notified
	//and re-read EDID.
	//and this Tx will need to re-authenticated any way.

	wasAcknowledged = !pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptDsNoHdcp;
	if(pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxOn && pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxHpdOn)
	{
		//update now for SiiRepeaterHdcpInformUsDeviceAboutFailure use
		pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptDsNoHdcp = !isAcknowledged;
		//if becomes available in the middle
		if(isAcknowledged && (!wasAcknowledged))
		{
			if (pRpt[cInstance]->txOnThePipe[tx_number].txStat.isHdcpRequested)
			{
			      //DEBUG_PRINT( "-----SiiTxHdcpDisable---222---\n");
				SiiTxHdcpDisable(cInstance );
				pRpt[cInstance]->txOnThePipe[tx_number].txStat.isHdcpRequested = false;
			}
			SiiRepeaterHdcpInformUsDeviceAboutFailure(cInstance, true, tx_number);

		}
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief     called from TPI component when DS is a repeater, but KSV number is 0.
//!			   SHA is still needed in this case, compared to the DS is a none repeater case
//! @param[in]  tx_number -  the tx that reported the message
//! @return     -true: SHA is available for KSV processing
//!			    -false: SHA is NOT available for KSV processing
//-------------------------------------------------------------------------------------------------
bool_t SiiRepeaterHdcpPrepareFor0KsvListProcessing(  BYTE cInstance, uint8_t tx_number )
{
	repeaterShaRequest_t srq;
	pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxKsvRdy = false;/*clear first*/
	if (SiiDrvShaInUse( cInstance ))
	{
		srq = SiiDrvGetCurrentShaReq(cInstance);
		if ( srq.isTx && srq.portNum != tx_number) //not this request
		{
			return false;
		}
	}

	srq.isTx = true;
	srq.portNum = tx_number;
	srq.manualTxStart = true; /*manual is a must for KSV0 case*/
	pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxKsvRdy = true;
	/*SHA is check already. Force request, since this is manual SHA, additional bit enable is needed every time*/
	SiiRepeaterShaRequest(cInstance, true, srq); /*force request, since this is manual SHA, additional enable is needed every time*/

	DEBUG_PRINT( RPT_MSG_DBG, "$$$tx %d had 0 KSV \n ",(int)tx_number);

	return true;

}
//-------------------------------------------------------------------------------------------------
//! @brief     called from TPI component when KSV is ready.
//! @param[in]  tx_number -  the tx that reported the message
//@ Return
//				- true:  KSV list processing is ready, SHA is allocated
//				- false: KSV list processing is not ready, SHA is used at present
//-------------------------------------------------------------------------------------------------
bool_t SiiRepeaterHdcpPrepareForKsvListProcessing( BYTE cInstance, uint8_t tx_number )
{
	uint8_t i;
	repeaterShaRequest_t srq;
	uint16_t	preKsvLength = 0;

	/*clear the status first, to avoid add the Tx itself*/
	pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxKsvRdy = false;

	/*check if Tx needs SHA or not*/
	if (!(pRpt[cInstance]->txOnThePipe[tx_number].txBstatus[0]& MSK_HDCP_DDC__BSTATUS1__DEV_COUNT))
	{
		return true;
	}

	/*Tx needs SHA, prepare to request*/
	if (SiiDrvShaInUse( cInstance ))
	{
		srq = SiiDrvGetCurrentShaReq( cInstance );
		if ( srq.isTx && srq.portNum == tx_number)
		{
			return true;
		}
		else/*not this request*/
		{
			return false;
		}
	}
	else/*SHA is not busy at present*/
	{

		for (i=0;i<SII_NUM_TX;i++)
		{
			if (pRpt[cInstance]->txOnThePipe[i].txStat.isRptTxKsvRdy)
			{
				preKsvLength += pRpt[cInstance]->txOnThePipe[i].ksv_length;
			}
		}
		/*preKsvLength is 0 if it is the first Tx has KSV ready for process*/
		SiiDrvShaAdjustKsvStartAddr( cInstance, preKsvLength );
		pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxKsvRdy = true;

		srq.isTx = true;
		srq.portNum = tx_number;
		srq.manualTxStart = false;

		SiiRepeaterShaRequest(cInstance,false, srq);
		DEBUG_PRINT( RPT_MSG_DBG, "$$$set SHA to Tx %d, SHA start %04X\n ",(int)tx_number,preKsvLength);

		return true;
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief       called from TPI component when downstream Hdcp failed.
//!			 Rx side will reset the ri to inform upstream about the failure
//! @param[in]  tx_number -  the tx that reported the message
//-------------------------------------------------------------------------------------------------
void SiiRepeaterHdcpInformUsDeviceAboutFailure( BYTE cInstance, bool_t isFatal,uint8_t tx_number)
{
	uint8_t clearStatus[] = {0,0};
    SiiRepeaterConInstanceSet(cInstance,tx_number);
    /* Mute DS until HDCP is succeeded*/
	RepeaterConAvMuteSet(cInstance,AV_MUTE_TX_IN_HDCP, ON);

	//do not reset the Rx side if it is DS HDCP not available.
    if ((isFatal)&&(!pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptDsNoHdcp))
    {
    	//do not reset Tx HDCP status here, will do after SCDT interrupt
        /* inform US side about the failure*/
    	SiiDrvRepeaterSetBcapsFifoReady( cInstance,OFF );
       	SiiDrvRepeaterSetUsBstatus( cInstance,clearStatus );
		DEBUG_PRINT( RPT_MSG_DBG,"$$$tx %d  inform us fatal failure\n ",(int)tx_number);
		/*disable HW auto SHA, will re-enable when 2 part of US auth*/
		SiiDrvHwAutoShaDisable(cInstance);
        /* toggle HPD since the number of the downstream may changed*/
		RepeaterConOnTxHpdTurnOn(  cInstance, ON, false);
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief       Called when application has detected valid Hpd change from downstream. repeater module then starts tmds/Hdcp process accordingly:
//!              if it is the first Tx has HPD active, repeater enables the repeater mode
//!              if it is the last active Tx has HPD inactive, repeater enables the receiver mode
//!              if it is a HPD toggle event, repeater relays the HPD toggle to the upstream

//! @param[in]  tx_number -  the tx that reported the message
//!             newHpd - DS HPD event type
//-------------------------------------------------------------------------------------------------
void SiiRepeaterOnTxHpdChange(  BYTE cInstance, rptHpdTxEvent_t newHpd, uint8_t tx_number)
{
	bool_t sameDs = false;

	SiiRepeaterConInstanceSet( cInstance, tx_number);
	RepeaterResetTxHdcpStatusData( cInstance, tx_number);

	if (pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxOn)
	{
		if ((newHpd == SI_RPT_TX_HPD_ON) &&
			(pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxHpdOn == false))
		{
			pRpt[cInstance]->numberOfTx ++;
			pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxHpdOn = true;
			if (pRpt[cInstance]->numberOfTx == 1)/*first downstream*/
			{
				/* Since we have a downstream connection, we must operate in Repeater mode.*/
				SiiDrvRepeaterEnable(cInstance,ON);
				pRpt[cInstance]->repeaterActive = true;
			}
		}
		 /*SI_RPT_TX_HPD_OFF */
		else if ((newHpd == SI_RPT_TX_HPD_OFF) &&
			(pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxHpdOn == true))
		{
			pRpt[cInstance]->numberOfTx--;
			pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxHpdOn = false;
			if(pRpt[cInstance]->numberOfTx == 0)
			{
				SiiDrvRepeaterEnable(cInstance,OFF);
				pRpt[cInstance]->repeaterActive = false;
			}
		}

#if (CHECK_SAME_TX == 1)
		if(pRpt[cInstance]->txOnThePipe[tx_number].wasAuthened == true)
		{
			//assume it is the same at this time. Check again after KSV
			sameDs =  true; //SiiTxStatusGet().isSameDsConnected;
		}
#endif  //#if (CHECK_SAME_TX == 1)

		if (newHpd == SI_RPT_TX_HPD_ON)
		{
			RepeaterConOnTxHpdTurnOn(  cInstance, ON, sameDs);
			if ( sameDs && SiiDrvRepeaterMainPortAuthed(cInstance))
			{
				RepeaterConAvMuteSet(cInstance,AV_MUTE_TX_IN_HDCP, ON);
				//DEBUG_PRINT(  "SiiTxHdcpEnable 11111\n ");
				SiiTxHdcpEnable(cInstance);
				pRpt[cInstance]->txOnThePipe[tx_number].txStat.isHdcpRequested = true;
			}
		}
		else
		{   /*either TOGGLE or OFF, turn off the RX*/
			RepeaterConOnTxHpdTurnOn(  cInstance, OFF, false);
			SiiDrvShaRequestStop(cInstance,true, tx_number); //stop the current Tx SHA request
		}
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief   called when Tpi has Hdcp authentication successfully done.
//!			 repeater module then unmutes tx
//! @param[in]  tx_number -  the tx that reported the message
//-------------------------------------------------------------------------------------------------
void SiiRepeaterOnTxHdcpDone( BYTE cInstance, uint8_t tx_number )
{
	DEBUG_PRINT( RPT_MSG_DBG, "Tx %d auth done\n ",(int)tx_number);

	pRpt[cInstance]->txOnThePipe[tx_number].txStat.isRptTxHdcpDone = true;
	pRpt[cInstance]->txOnThePipe[tx_number].wasAuthened = true;

	SiiRepeaterConInstanceSet(cInstance,tx_number);
	RepeaterConAvMuteSet(cInstance,AV_MUTE_TX_IN_HDCP, OFF);

}

//-------------------------------------------------------------------------------------------------
//! @brief      change the active input source
//! @param[in]  portIndex - active input index
//-------------------------------------------------------------------------------------------------
void SiiRepeaterSetActiveSource( BYTE cInstance, uint8_t portIndex)
{
	pRpt[cInstance]->inputPort = portIndex;
	SiiDrvRepeaterActiveInputSet( cInstance,portIndex );
}


//-------------------------------------------------------------------------------------------------
//! @brief      retrieve the repeater interrupt status
//! @param[in]  none
//-------------------------------------------------------------------------------------------------
bool_t SiiRepeaterInterruptPending(  BYTE cInstance )
{
	return (SiiDrvRepeaterGetInterruptsOn( cInstance )
	        ||SiiDrvRepeaterResChngStatusGet( cInstance ));
}

//-------------------------------------------------------------------------------------------------
//! @brief      Mark the input as Non-HDCP input, and expect the repeater to start the HDCP
//!             at the Downstream when this input is active
//! @param[in]  portIndex- the input port index
//!             qOn-       true:  set to non-HDCP input
//!                        false: clear the non-HDCP input status
//-------------------------------------------------------------------------------------------------
void SiiRepeaterHdcpDsOnlyAuthentication( BYTE cInstance, uint8_t portIndex, bool_t qOn )
{
	if( qOn )
	{
		InputNoHdcp[cInstance] |=  0x01 << portIndex;
	}
	else
	{
		InputNoHdcp[cInstance] &=~(0x01 << portIndex);
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief      return the query that the input port is set up as non-HDCP port or not
//! @param[in]  none
//@ Return
//				- true:  input port is set up as non-HDCP port
//				- false: input port is not set up as non-HDCP port
//-------------------------------------------------------------------------------------------------

bool_t RepeaterCurrentInputPortIsSetToNoHdcp(  BYTE cInstance )
{
	//return (( InputNoHdcp[cInstance] & (0x01 << pRpt[cInstance]->inputPort)) == (0x01 << pRpt[cInstance]->inputPort));
	return (( (0x01 << pRpt[cInstance]->inputPort)) == (0x01 << pRpt[cInstance]->inputPort));//quickfix
}
