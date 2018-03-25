//***************************************************************************
//!file     si_drv_repeater.c
//!brief    Silicon Image Repeater Driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_drv_repeater.h"
#include "si_drv_internal.h"
#include "si_gpio.h"
#include "si_regs_tpi953x.h"



//#define NONE_REPT_WITH_TX_ON


uint8_t mainPort = 0x00; //as default
uint8_t inputPort[DevNum][SII_NUM_REPEATER];
uint8_t	pipeNumber[DevNum];
RepeaterTopoMode_t currentTopoMode[DevNum];
bool_t  intStatus[DevNum][SII_NUM_REPEATER];
bool_t  resChngStatus[DevNum][SII_NUM_REPEATER];
bool_t  inputPortToChange[DevNum];

// SWWA_FP1554
#if INC_OSD
#include "si_drv_osd_internal.h"
#endif
// SWWA_FP1554

//-------------------------------------------------------------------------------------------------
//! @brief      get the corresponding register name for the pipe
//!
//-------------------------------------------------------------------------------------------------

static uint16_t GetRegName( BYTE cInstance, uint16_t regName)
{
	uint16_t resultName = 0xFFFF;

	switch(regName)
	{
		case REG_MP_STATE:
				resultName = (pipeNumber[cInstance] == 0) ? REG_MP_STATE : REG_RP_STATE;
				break;
		case REG_MP_GCP_STATUS:
				resultName = (pipeNumber[cInstance] == 0) ? REG_MP_GCP_STATUS : REG_RP_GCP_STATUS;
				break;

	}
	return resultName;
}

//-------------------------------------------------------------------------------------------------
//! @brief      init repeater driver
//!
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterInit( BYTE cInstance)
{
	uint8_t i;
	for (i=0;i<SII_NUM_REPEATER;i++)
	{
		inputPort[cInstance][i]     = SiiPORT_0;  //legal when in Instrp mode.
		intStatus[cInstance][i]     = false;
		resChngStatus[cInstance][i] = false;
	}
	pipeNumber[cInstance] = 0;
	currentTopoMode[cInstance] = SI_RPT_MODE_INITIAL;
	inputPortToChange[cInstance] = false;
	SiiRegWrite(cInstance, REG_HDCP_KSV_FIFO_CTRL, VAL_HDCP_FIFO_VALID);
}

//-------------------------------------------------------------------------------------------------
//! @brief      set repeater mode
//!
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterSetMode( BYTE cInstance, RepeaterTopoMode_t cmode)
{
	if( currentTopoMode[cInstance] != cmode )
	{
		//to clear all the rep bits, application will set it on later with active Tx.
		currentTopoMode[cInstance] =  SI_RPT_MODE_SINGLE_REPEATER;
		SiiDrvRepeaterSetBcapsRepeaterBits( cInstance ,OFF);
		currentTopoMode[cInstance] = cmode;
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief      set instance 
//!
//! @param[in]  selectedInstance - current instance
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterInstanceSet( BYTE cInstance,uint8_t setP)
{
	pipeNumber[cInstance] = setP;
}

//-------------------------------------------------------------------------------------------------
//! @brief      if the rx SCDT is on for the inputs list, or the (main/sub)channel 
//! @param[in]  none
//! @return     true: ON
//!				false: OFF
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterRxSyncIsOn( BYTE cInstance )
{
	return (SiiRegRead( cInstance , GetRegName( cInstance,REG_MP_STATE)) & BIT_SCDT);
}

//-------------------------------------------------------------------------------------------------
//! @brief      if the rx main port SCDT interrupt happened
//! @param[in]  none
//! @return     true: interrupt happened
//!				false: interrupt not happened
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterRxSyncIntr(  BYTE cInstance )
{
	uint8_t intSt = 0;

	intSt = SiiRegRead( cInstance , REG_INT_STATUS_2 ) & ((pipeNumber[cInstance] == 0) ? BIT_SCDT_CHG : BIT_RP_SCDT_CHG);
	
	if (intSt)
	{
		SiiRegWrite( cInstance,  REG_INT_STATUS_2, (pipeNumber[cInstance] == 0) ? BIT_SCDT_CHG : BIT_RP_SCDT_CHG);
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
//! @brief      if the rx main port is authenticated
//! @param[in]  none
//! @return     true: authenticated
//!				false:not authenticated
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterMainPortAuthed(  BYTE cInstance )
{
	bool_t result = false;

	result = (SiiRegRead( cInstance ,GetRegName(cInstance, REG_MP_STATE)) & BIT_DECRYPT_STATUS);

   	return result;
}


//-------------------------------------------------------------------------------------------------
//! @brief      toggle the Rx Hot Plug High or Low
//! @param[in]  true: High
//!				false: Low
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterSwitchRxHotPlug( BYTE cInstance, bool_t switch_on)
{
// SWWA_FP1554
#if INC_OSD
    {
        if ( !switch_on )
        {
            // Turn off OSD_EN so that the loss of TCLK when the RX is shut off will not
            // affect OSDRAM transfer operations.
          //  DEBUG_PRINT( MSG_DBG, "\nSWWA_FP1554: OSD_EN = 0 (RPTR_RX_OFF)\n");
            DrvOsdDisableInternal( cInstance );
        }
    }
#endif
// SWWA_FP1554

	if (currentTopoMode[cInstance] == SI_RPT_MODE_SINGLE_REPEATER)
	{
		if ( switch_on )
		{
		    SiiDrvRxHpeStateSet( cInstance , SiiPORT_ALL, SiiHPD_ACTIVE );
		}
		else
		{
		    SiiDrvRxHpeStateSet(  cInstance ,SiiPORT_ALL, SiiHPD_INACTIVE );
		}
	}
	else if (currentTopoMode[cInstance] == SI_RPT_MODE_MULTIPLE_REPEATER)
	{
		if( switch_on )
		{
		    SiiDrvRxHpeStateSet( cInstance, inputPort[cInstance][pipeNumber[cInstance]], SiiHPD_ACTIVE );
		}
		else
		{
		    SiiDrvRxHpeStateSet( cInstance,inputPort[cInstance][pipeNumber[cInstance]], SiiHPD_INACTIVE );
		}
	}


    DEBUG_PRINT( MSG_DBG, "RX: HPD %s\n", switch_on ? "ON" : "OFF");
/*   if(switch_on)
   {
//
//     SiiRegModify ( REG_SYS_RESET_1, BIT0, SET_BITS);
//     SiiRegModify ( REG_SYS_RESET_1, BIT0, CLEAR_BITS);
//     DEBUG_PRINT( MSG_DBG, "\n\n RX--SOFTRESET IS PERFORMED\n\n");
     SiiRegModify ( REG_TPI__RAR, BIT0, SET_BITS);
     SiiPlatformTimerWait( 500 );
     SiiRegModify ( REG_TPI__RAR, BIT0, CLEAR_BITS);
//     SiiDrvTpiIndirectWrite(REG_TX__TMDS_ZONE_CTRL2, 0x09);
////   SiiTxInitialize();
      DEBUG_PRINT( MSG_DBG, "\n\nTX--SOFTRESET IS PERFORMED\n\n");
//	    SiiDrvRxTermControl(0,false);
//	    SiiPlatformTimerWait( 500 );
//	    SiiDrvRxTermControl(0,true);
   }*/
}

//-------------------------------------------------------------------------------------------------
//! @brief      clear the bstatus of Rx
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterRxHdcpReset( BYTE cInstance)
{
	uint8_t clearStatus[] = {0,0};
    // inform US side about the failure
	SiiDrvRepeaterSetUsBstatus( cInstance ,clearStatus);
}

//-------------------------------------------------------------------------------------------------
//! @brief      if the rx main port has HPD, or is MHD
//! @param[in]  none
//! @return     true: has HPD
//!				false:no HPD
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterIsRxHotPlugAsserted( BYTE cInstance)
{

	if( (SiiRegRead( cInstance ,REG_RX_PORT_SEL) & MSK_MP_PORT_SEL) != SiiSwitchSource1)
	{
		return  (SiiRegRead( cInstance , REG_MP_STATE ) & ( BIT_PWR5V | BIT_MHL ));
	}
	else
	{
		return (SiiPlatformMHLCDSensePinGet( cInstance,  SiiSwitchSource1 )? SiiPortType_HDMI:SiiPortType_MHL);
	}

//	return (SiiRegRead(GetRegName(REG_MP_STATE)) & ( BIT_PWR5V | BIT_MHL));
}

//-------------------------------------------------------------------------------------------------
//! @brief      assign rx bstatus bytes
//! @param[in]  us_bstatus
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterSetUsBstatus( BYTE cInstance,uint8_t us_bstatus[2])
{

    SiiRegWriteBlock( cInstance ,REG_HDCP_SHD_BSTAT1, us_bstatus, 2);
}

//-------------------------------------------------------------------------------------------------
//! @brief      set the bacps repeater bits
//! @param[in]  true: ON
//!				false: OFF
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterSetBcapsRepeaterBits( BYTE cInstance, bool_t switchOn)
{
#ifdef 	NONE_REPT_WITH_TX_ON
 	switchOn = false;
#endif

//IF INSTA PORT MODE, ALL CLEARED OR SET
//if matrix mode, individually set
    if (currentTopoMode[cInstance] == SI_RPT_MODE_MULTIPLE_REPEATER)
	{
		if (switchOn)
		{
			//Enable repeater bit for port 0-3
			if (inputPort[cInstance][pipeNumber[cInstance]] <= SiiPORT_3)
			{
				SiiRegModify( cInstance ,REG_HDCP_BCAPS_SET, 0x02 << (inputPort[cInstance][pipeNumber[cInstance]] * 2), SET_BITS);
			}



		}
		else
		{
			if (inputPort[cInstance][pipeNumber[cInstance]] <= SiiPORT_3)
				SiiRegModify( cInstance ,REG_HDCP_BCAPS_SET, 0x03 << (inputPort[cInstance][pipeNumber[cInstance]] * 2), CLEAR_BITS); //both repeater and FIFO rdy



		}


	}
	else //if(currentTopoMode[cInstance] == SI_RPT_MODE_SINGLE_REPEATER)
	{
		if (switchOn)
		{
			//Enable repeater bit for port 0-3
			SiiRegWrite( cInstance ,REG_HDCP_BCAPS_SET,
					VAL_P3_REPEATER | VAL_P2_REPEATER | VAL_P1_REPEATER | VAL_P0_REPEATER);


		}
		else
		{
			//Disable repeater bit for port 0-3
			SiiRegWrite( cInstance ,REG_HDCP_BCAPS_SET, CLEAR_BITS);


		}
	}

//	if switchOn
	SiiRegModify( cInstance ,REG_HDCP_KSV_FIFO_CTRL, VAL_HDCP_FIFO_VALID, SET_BITS);

}

//-------------------------------------------------------------------------------------------------
//! @brief      toggle the Rx Hot Plug High or Low
//
//! @param[in]  portIndex- the active port
//!				switchOn-
//!						true: High
//!						false: Low
//-------------------------------------------------------------------------------------------------
void SiiDrvRepeaterSetBcapsFifoReady( BYTE cInstance, bool_t switchOn)
{
    uint8_t bcaps_msk = 0;
    uint8_t bcaps_val = 0;
    uint8_t portIndex = inputPort[cInstance][pipeNumber[cInstance]];

    //HW sets ready bit itself when ready.
    //FW sets ready bit manually to ensure when SHA is assigned to Tx,
    //Ready bit still active
    //HW clears the ready bit when new authentication starts


    if(currentTopoMode[cInstance] != SI_RPT_MODE_MULTIPLE_REPEATER)
    {
    	portIndex = SiiPORT_ALL;
    }

	if ( portIndex == SiiPORT_ALL )
	{
		bcaps_val = (switchOn) ? SET_BITS : CLEAR_BITS;
		bcaps_msk = BIT_P0_FIFO_RDY|BIT_P1_FIFO_RDY|BIT_P2_FIFO_RDY|BIT_P3_FIFO_RDY;

		SiiRegModify( cInstance ,REG_HDCP_BCAPS_SET, bcaps_msk, bcaps_val);
		return;
	}

	switch(portIndex)
	{
		case SiiPORT_0:
				bcaps_msk = BIT_P0_FIFO_RDY;
				break;
		case SiiPORT_1:
				bcaps_msk = BIT_P1_FIFO_RDY;
				break;
		case SiiPORT_2:
				bcaps_msk = BIT_P2_FIFO_RDY;
				break;
		case SiiPORT_3:
				bcaps_msk = BIT_P3_FIFO_RDY;
				break;
	}

	bcaps_val = (switchOn) ? SET_BITS : CLEAR_BITS;
	SiiRegModify( cInstance ,REG_HDCP_BCAPS_SET, bcaps_msk, bcaps_val);
}

//-------------------------------------------------------------------------------------------------
//! @brief     reset the Ri' for US to notice the HDCP failure
//-------------------------------------------------------------------------------------------------

void SiiDrvRxHdcpResetRi(  BYTE cInstance )
{
	//uint8_t tempVal;
	//tempVal = SiiRegRead(REG_HDCP_STAT);

	SiiRegWrite( cInstance ,REG_HDCPCTRL, BIT_CLEAR_RI);
	//SiiRegWrite(REG_HDCP_STAT, tempVal);
}
//-------------------------------------------------------------------------------------------------
//! @brief      Initialize repeater for operation in the requested mode.
//!
//! @param[in]  repeaterMode    - true: initialize as a repeater, false: initialize as a sink
//! @return     true: success
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterEnable (  BYTE cInstance, bool_t repeaterMode )
{
	uint8_t intrMask = 0;

	SiiDrvRepeaterSetBcapsRepeaterBits( cInstance ,repeaterMode);

	//find out the active input mask
	intrMask = 0x01<<(4 + inputPort[cInstance][pipeNumber[cInstance]]);
	SiiRegModify( cInstance ,REG_INT_ENABLE_IP2, intrMask, repeaterMode? SET_BITS : CLEAR_BITS);

	//turn on SCDT mask here too
	if (pipeNumber[cInstance] == 0)
	{
		SiiRegModify( cInstance ,REG_INT_ENABLE_2, BIT_SCDT_CHG, repeaterMode? SET_BITS : CLEAR_BITS);
        SiiRegModify( cInstance ,REG_INT_ENABLE_P1, BIT_MP_AV_MUTE_MASK | BIT_MP_AV_UNMUTE_MASK, repeaterMode? SET_BITS : CLEAR_BITS);
	}
	else
	{
		SiiRegModify( cInstance ,REG_INT_ENABLE_2, BIT_RP_SCDT_CHG, repeaterMode? SET_BITS : CLEAR_BITS);
        SiiRegModify( cInstance ,REG_INT_ENABLE_P1, BIT_RP_AV_MUTE_MASK, repeaterMode? SET_BITS : CLEAR_BITS);
        SiiRegModify( cInstance ,REG_INT_ENABLE_P2, BIT_RP_AV_UNMUTE_MASK, repeaterMode? SET_BITS : CLEAR_BITS);
	}

	//none pipe related, shared for all the inputs
	SiiRegModify( cInstance ,REG_INT_ENABLE_IP2, BIT_DECRYPT, repeaterMode? SET_BITS : CLEAR_BITS);

    return( true );
}



//-------------------------------------------------------------------------------------------------
//! @brief      Fill the Ksv to Ksv FIFO.
//! @param[in]  p_data- KSV data
//!				length- KSV length
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterFillUsKsvFifo( BYTE cInstance,uint8_t *p_data, uint16_t length)
{
	SiiRegWriteBlock( cInstance ,REG_KSV_FIFO, p_data, length);
}



//-------------------------------------------------------------------------------------------------
//! @brief      Returns if the main port authentication starts
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterMainPortAuthChanged( BYTE cInstance)
{
	bool_t changeIntr = false;
	bool_t result = false;
	uint8_t regVal;

	regVal=SiiRegRead(  cInstance ,REG_INT_STATUS_IP2 )& MSK_AUTH_CHG_P0_P3;
	changeIntr = regVal & (0x01<<(4 + inputPort[cInstance][pipeNumber[cInstance]]));
	if (changeIntr)
	{
		SiiRegWrite( cInstance ,REG_INT_STATUS_IP2, regVal);
	}
	if (changeIntr)
	{
		result = SiiRegRead( cInstance ,GetRegName(cInstance,REG_MP_STATE)) & BIT_AUTHENED_STATUS;
	}

#ifdef NONE_REPT_WITH_TX_ON
	result = false;
#endif
	return result;

}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns if the Decryption state change interrupt happened
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterAnyInputDecryptionChanged( BYTE cInstance)
{
	bool_t result = false;

	//all the input share one interrupt
	if ( SiiRegRead( cInstance , REG_INT_STATUS_IP2 ) & BIT_DECRYPT )
	{
		result = true;
		SiiRegWrite( cInstance,  REG_INT_STATUS_IP2, BIT_DECRYPT );
	}
#ifdef NONE_REPT_WITH_TX_ON
	result = false;
#endif
	return result;

}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns if the main port AV MUTE status changed, and what it changes to
//
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterMainPortAvMuteChanged( BYTE cInstance, bool_t *muteOn)
{
	bool_t result = false;
	uint8_t regVal;
	uint8_t regVal1;
	uint8_t regMuteBit = 0x01;
	uint8_t regUnMuteBit = 0x02;

	if (pipeNumber[cInstance] == 0)
	{
		regVal = SiiRegRead( cInstance , REG_INT_STATUS_P1 )& (BIT_MP_AV_MUTE | BIT_MP_AV_UNMUTE);
		if (regVal)
		{
			SiiRegWrite( cInstance , REG_INT_STATUS_P1, regVal);
		}
		regVal = regVal >> 4;
	}
	else
	{
		regVal1 = SiiRegRead( cInstance , REG_INT_STATUS_P2 )& BIT_RP_AV_UNMUTE;
		if (regVal1)
		{
			SiiRegWrite( cInstance  ,REG_INT_STATUS_P2, regVal1);
		}
		regVal = SiiRegRead(  cInstance , REG_INT_STATUS_P1 )& BIT_RP_AV_MUTE;
		if (regVal)
		{
			SiiRegWrite( cInstance , REG_INT_STATUS_P1, regVal);
		}
		regVal = (regVal >> 7) | (regVal1 << 1);
	}

	if (regVal)
	{
		if ((regVal & regMuteBit)&&(regVal & regUnMuteBit)) //both set
		{
			if (SiiDrvRepeaterIsUsMuteClr( cInstance ))
			{
				*muteOn = OFF;
			}
			else
			{
				*muteOn = ON;
			}
		}
		else if (regVal & regMuteBit) //mute
		{
			*muteOn = ON;
		}
		else //unmute
		{
			*muteOn = OFF;
		}
		result = true;
	}

	return result;

}

//-------------------------------------------------------------------------------------------------
//! @brief      check if the US cleared the AVMUTE
//! @return  	true: clear
//!				false: set
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterIsUsMuteClr( BYTE cInstance)
{
	uint8_t mute;

	mute = SiiRegRead( cInstance ,GetRegName(cInstance,REG_MP_GCP_STATUS));
	if ((( mute & BIT_GCP_MUTE) == 0) || ( mute & BIT_GCP_UNMUTE) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

//-------------------------------------------------------------------------------------------------
//! @brief      get the active input port
//! @return  	port number
//-------------------------------------------------------------------------------------------------

uint8_t SiiDrvRepeaterGetSelectedPort(  BYTE cInstance )
{
	return inputPort[cInstance][pipeNumber[cInstance]];
}


//-------------------------------------------------------------------------------------------------
//! @brief      check if the US input is HMD mode
//! @return  	true: HMDI
//!				false: DVI
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterInputIsHdmi( BYTE cInstance)
{
	return true; //not used, pass through
}

//-------------------------------------------------------------------------------------------------
//! @brief      check if the US input mode changed
//! @return  	true: changed
//!				false: no change
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvRepeaterInputModeChange(  BYTE cInstance )
{
	return false;
}


//-------------------------------------------------------------------------------------------------
//! @brief      change the active input source
//! @param[in]  portIndex - active input index
//-------------------------------------------------------------------------------------------------

void SiiDrvRepeaterActiveInputSet( BYTE cInstance,uint8_t portIndex)
{
	uint8_t intrMask = 0;

	//enable the HDCP change interrupt mask, will clear when Tx cable out
	//same for both mode. Only care the active input.
	//clear the old port

	intrMask = 0x01<<(4 + inputPort[cInstance][pipeNumber[cInstance]]);
	SiiRegModify( cInstance ,REG_INT_ENABLE_IP2, intrMask, CLEAR_BITS);


	//set the new port
	intrMask = 0x01<<(4 + portIndex);
	SiiRegModify( cInstance ,REG_INT_ENABLE_IP2, intrMask, SET_BITS);

	if (currentTopoMode[cInstance] != SI_RPT_MODE_MULTIPLE_REPEATER)
	{
		inputPortToChange[cInstance] = true;
	}
	else
	{
		SiiDrvRepeaterSetBcapsRepeaterBits( cInstance ,false);//clear the old port rep bit
	}

	inputPort[cInstance][pipeNumber[cInstance]] = portIndex;
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRepeaterNewPortAuthed
// Description: In the instaPort mode, new input port is decrypted or not
//              if yes, need to start HDCP on the Tx
//------------------------------------------------------------------------------

bool_t SiiDrvRepeaterNewPortAuthed(  BYTE cInstance )
{
	if ( SiiDrvRepeaterMainPortAuthed( cInstance ) && inputPortToChange[cInstance] )
	{
		inputPortToChange[cInstance] = false;
		return true;
	}
	return false;

}
//------------------------------------------------------------------------------
// Function:    SiiDrvRepeaterProcessInterrupts
// Description: set the flag when interrupt comes
//              these interrupts are used in repeater only
//------------------------------------------------------------------------------

void SiiDrvRepeaterInterruptsSet( BYTE cInstance,bool_t qOn)
{
	intStatus[cInstance][pipeNumber[cInstance]] = qOn;
}
//------------------------------------------------------------------------------
// Function:    SiiDrvRepeaterResChngStatusSet
// Description: set the flag when resolution change interrupt comes
//              The interrupt is processed elsewhere too.
//------------------------------------------------------------------------------

void SiiDrvRepeaterResChngStatusSet( BYTE cInstance,bool_t qOn)
{
#if (SII_NUM_REPEATER == 1)
	if(pipeNumber[cInstance] == 0)
#endif
		resChngStatus[cInstance][pipeNumber[cInstance]] = qOn;
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRepeaterResChngStatusGet
// Description: get the status of resolution change interrupt flag read previously
//              The interrupt is processed elsewhere too.
//------------------------------------------------------------------------------

bool_t SiiDrvRepeaterResChngStatusGet( BYTE cInstance)
{
    return (resChngStatus[cInstance][pipeNumber[cInstance]]);
}
//------------------------------------------------------------------------------
// Function:    SiiDrvRepeaterProcessInterrupts
// Description: set the flag when interrupt comes
//------------------------------------------------------------------------------
bool_t SiiDrvRepeaterGetInterruptsOn( BYTE cInstance)
{
	return intStatus[cInstance][pipeNumber[cInstance]];
}


