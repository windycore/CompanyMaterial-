//***************************************************************************
//!file     si_audio_component.c
//!brief    Silicon Image AUDIO Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "string.h"
#include "si_audio_component.h"
#include "si_debug.h"
#include "si_drv_audio_internal.h"
#include "si_timer.h"
#include "Si_common.h"
#include "Mcu.h"
#include "uart.h"
#include "Si_osal_timer.h"
SiiRxAudioInstanceData_t RxAudInstance[INSTANCE_9533][SII_NUM_RX_AUDIO];
SiiRxAudioInstanceData_t *pAudExtraction[INSTANCE_9533] = 
{
      &RxAudInstance[0][0],
#if (INSTANCE_9533>1)
      &RxAudInstance[1][0],
#endif
#if (INSTANCE_9533>2)
      &RxAudInstance[2][0],
#endif
#if (INSTANCE_9533>3)
      &RxAudInstance[3][0],
#endif
#if (INSTANCE_9533>4)
      &RxAudInstance[4][0],
#endif
#if (INSTANCE_9533>5)
      &RxAudInstance[5][0],
#endif
#if (INSTANCE_9533>6)
      &RxAudInstance[6][0],
#endif
#if (INSTANCE_9533>7)
      &RxAudInstance[7][0],
#endif


};

//-------------------------------------------------------------------------------------------------
//! @brief      Set the component global data pointer to the requested instance.
//! @param[in]  instanceIndex   Instance of RX Audio component.
//! @returns    true if legal index, false if index value is illegal.
//-------------------------------------------------------------------------------------------------
bool_t SiiRxAudioInstanceSet( BYTE cInstance, int instanceIndex )
{
    if ( instanceIndex < SII_NUM_RX_AUDIO )
    {
        SiiDrvRxAudioInstanceSet(  cInstance, instanceIndex );
        (pAudExtraction[cInstance]) = &RxAudInstance[cInstance][ instanceIndex ];
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
// Function:    SiiRxAudioInit
// Description:  Initialize the audio state machine and audio hardware start up
//              for both main and sub channel
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------
void SiiRxAudioInit(  BYTE cInstance )
{
    (pAudExtraction[cInstance])->audState    = SII_AUD_STATE_OFF;
    (pAudExtraction[cInstance])->audMode     = SII_AUD_PCM_MODE;
    (pAudExtraction[cInstance])->audLayout   = SII_AUD_TWO_CHANNEL;

    SiiDrvRxAudioInit( cInstance );
    SiiDrvRxSpdifOutEnable( cInstance ,false); // SPDIF digital outputs are disabled by default
}

//------------------------------------------------------------------------------
//! @brief  Place audio component in standby
//------------------------------------------------------------------------------
bool_t SiiRxAudioStandby(  BYTE cInstance )
{
    int i;

    for ( i = 0; i < SII_NUM_RX_AUDIO; i++ )
    {
        SiiRxAudioInstanceSet( cInstance , i );
        SiiDrvRxSpdifOutEnable(  cInstance, false );
    }

    return( SiiDrvRxAudioStandby( cInstance ) );
}

//-------------------------------------------------------------------------------------------------
//! @brief
//! @return
//-------------------------------------------------------------------------------------------------
uint8_t SiiRxAudioHandler( BYTE cInstance, SiiRxAudioInstanceData_t *pAudRx)
{
    uint8_t IntrStatus[3];
	uint8_t intTrigger=false;

	if (SiiDrvRxAudioIsInterruptAssert( cInstance ))
	{
	    //read all interrupt registers
	    SiiDrvRxAudioGetIntStatus( cInstance, IntrStatus);
	    //SiiDrvRxAudioUnmuteReady(false);
	    if (IntrStatus[1] & SII_AUD_RX_AACDONE)    //if soft mute done
	    {
			//DEBUG_PRINT( MSG_ALWAYS, "=== AAC_DONE ===\n");
	        SiiDrvRxAudioInternalMute( cInstance, true);
	        if( SiiDrvRxAudioModeGet( cInstance )== SII_AUD_DSD_MODE)
	        {
	        	DEBUG_PRINT( MSG_ALWAYS, "AAC_DONE: DSD mode, enable AAC_ALL\n");
	        	SiiDrvRxAudioEnableAAC( cInstance );
	        }
            if( (pAudExtraction[cInstance])->audState != SII_AUD_STATE_OFF ){ // from Audio ON condition.
		        (pAudExtraction[cInstance])->audState = SII_AUD_STATE_OFF;
                //SiiDrvRxAudioEnableCHSTINT();
				SiiDrvRxAudioInitMclk( cInstance );

			}
	    }
	    else 
	    {
			//if( IntrStatus[2] & SII_AUD_RX_FSCHG){
	    	if(((pAudExtraction[cInstance])->audState == SII_AUD_STATE_OFF)&&
	    			(IntrStatus[1] & SII_AUD_RX_FNCHG))//&&(IntrStatus[2] & BIT_CHST_RDY))
	    	{
				//DEBUG_PRINT( MSG_ALWAYS, "=== FNCHG ===\n");
	    		//if audFs is unknown, don't clear FNCHG, then check again
				(pAudExtraction[cInstance])->audMode = (SiiRxAudioModes_t)SiiDrvRxAudioModeGet( cInstance );
				(pAudExtraction[cInstance])->audFs = (SiiRxAudioFsValues_t)SiiDrvRxAudioCheckFsAndFix( cInstance );
				if ( ((pAudExtraction[cInstance])->audMode == SII_AUD_DSD_MODE) ||
					( (((pAudExtraction[cInstance])->audMode == SII_AUD_HBRA_MODE) || ((pAudExtraction[cInstance])->audMode == SII_AUD_PCM_MODE) ) &&
					    ((pAudExtraction[cInstance])->audFs != (SiiRxAudioFsValues_t)SII_AUD_CHST4_FS_UNKNOWN)	) )

				{
					SiiDrvRxAudioSetMclk( cInstance,  (pAudExtraction[cInstance])->audMode, (pAudExtraction[cInstance])->audFs );
					SiiDrvRxAudioEnableAAC( cInstance );
				}

			}
			if (IntrStatus[0] & SII_AUD_RX_AUDIO_READY)    //audio ready
			{
				DEBUG_PRINT( MSG_ALWAYS, "=== UNMUTE ===\n");
				//clear all pending interrupts
				IntrStatus[0] = SII_AUD_RX_AUDIO_READY;
				IntrStatus[1] = 0;
				IntrStatus[2] = 0;
				SiiDrvRxAudioClearIntStatus( cInstance, IntrStatus);
				(pAudExtraction[cInstance])->audState = SII_AUD_STATE_ON;
				SiiDrvRxAudioUnmuteReady( cInstance, true); //for ACP chk
				//SiiDrvRxAudioDisableCHSTINT();
				//SiiDrvRxAudioDisableAAC();

			}
	    }

        //check ACP packet
	    if (SiiDrvRxAudioGetNewAcpInt( cInstance ))    //new ACP change
	    {
	    	DEBUG_PRINT( MSG_ALWAYS, "\n\n\nAudio:New Acp\n");
	    	SiiDrvRxAudioOnNewAcp( cInstance, true);
	    	//if it is in No ACP check already, exit it
	    	if((pAudExtraction[cInstance])->audState == SII_AUD_NO_ACP_CHECK)
	    	{
	    		SiiDrvRxAudioNoAcpPacketcheckStart( cInstance, OFF);
	    		(pAudExtraction[cInstance])->audState = SII_AUD_STATE_IDLE;
	    	}

	    }
	    // If audio is ready, and we were in ACP type 2 or above but
	    // have not received an ACP packet, start the 600ms to wait for one.
	    else if (SiiDrvRxAudioGetNoAcpInt( cInstance ))    //no ACP change
	    {
	    	(pAudExtraction[cInstance])->audState = SII_AUD_NO_ACP_CHECK;
	    	SiiPlatformTimerSet( (uint8_t)((uint8_t)TIMER_4+ cInstance) , SII_NOACP_TIMEOUT ,0xFF);//art the timer
	    	SiiDrvRxAudioNoAcpPacketcheckStart( cInstance, ON);
	    }

		(pAudExtraction[cInstance])->audMode = (SiiRxAudioModes_t)SiiDrvRxAudioModeGet( cInstance );
		(pAudExtraction[cInstance])->audLayout = (SiiRxAudioLayouts_t)SiiDrvRxAudioLayoutGet( cInstance );
		(pAudExtraction[cInstance])->audFs = (SiiRxAudioFsValues_t)SiiDrvRxAudioSampleRateGet( cInstance );
		(pAudExtraction[cInstance])->audMclk = (SiiRxAudioMclk_t)SiiDrvRxAudioGetMclk( cInstance );
		memcpy(pAudRx, (pAudExtraction[cInstance]), sizeof(SiiRxAudioInstanceData_t));
		
		IntrStatus[0] = 0;
		IntrStatus[1] = SII_AUD_RX_AACDONE;
		IntrStatus[2] = 0;
	    //clear all pending interrupts
	    SiiDrvRxAudioClearIntStatus( cInstance, IntrStatus);

		SiiDrvRxAudioInterruptClearAssert( cInstance );
		intTrigger = true;
	}

	return intTrigger;
}


void SiiRxAudioNoAcpCheck( BYTE cInstance)
{
 //TODO
	if ( SiiTimerExpired( (uint8_t)((uint8_t)TIMER_4+cInstance) ))  // TO DO
	{
		(pAudExtraction[cInstance])->audState = SII_AUD_STATE_ON;
		//time out,
		if(SiiDrvRxAudioNoAcpPacketcheckConfirm( cInstance ))
		{
			//revert to no ACP action
			DEBUG_PRINT( MSG_DBG, "No Acp Packet\n");

			SiiDrvRxAudioOnNoAcp( cInstance );
		}
		else
		{
			SiiDrvRxAudioOnNewAcp( cInstance, ON);
		}

		SiiDrvRxAudioNoAcpPacketcheckStart( cInstance ,OFF);
	}
}

