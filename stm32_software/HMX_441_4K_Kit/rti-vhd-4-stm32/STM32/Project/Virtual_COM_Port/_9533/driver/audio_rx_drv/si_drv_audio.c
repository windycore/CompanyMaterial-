/******************************************************************************/
//!file     si_drv_audio.c
//!brief    SiI9589 Audio Driver.
//
// NOTE: This driver has a dependency on the Switch driver for the 9535
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2007-2012, Silicon Image, Inc.  All rights reserved.
/******************************************************************************/

#include "string.h"
#include "si_drv_audio_internal.h"
#include "si_audio_enums.h"
#include "si_drv_internal.h"
#include "si_drv_switch.h"
#include "si_audio_component.h"
#include "si_regs_audio953x.h"
#include "si_regs_pp953x.h"

#define SI_AUDIO_NO_ACP_WORKAROUND		false

#include "Mcu.h"
#include "uart.h"

typedef struct
{
	uint8_t instanceIndex;
	uint8_t intStatus;	
	bool_t  vsifCheckedByOthers;
	bool_t  unmuteIntrHappened;
	bool_t  newAcpIntrHappened;
	bool_t  isSpdifOutEnabled;
} AudDrvInstanceData_t;

AudDrvInstanceData_t RxAudioIntStatus[DevNum][2];
AudDrvInstanceData_t *pDrvRxAud[DevNum];

#define SII_AUDIO_INSTANCES
#define REG_TEMP_ACP_HEADER 	REG_RP_VISF_CONTENT_HEADER


//-------------------------------------------------------------------------------------------------
static uint32_t GetCts( BYTE cInstance)
{
	uint8_t cts_1;
	uint8_t cts_2;
	uint8_t cts_3;

	cts_1 = SiiRegRead( cInstance, REG_CTS_HVAL1_P0);
	cts_2 = SiiRegRead( cInstance, REG_CTS_HVAL2_P0);
	cts_3 = SiiRegRead( cInstance, REG_CTS_HVAL3_P0);
	return (cts_3 << 16) | (cts_2 << 8) | cts_1;
}

//-------------------------------------------------------------------------------------------------
static uint32_t GetN( BYTE cInstance)
{
	uint8_t n_1;
	uint8_t n_2;
	uint8_t n_3;

	n_1 = SiiRegRead( cInstance, REG_N_HVAL1_P0);
	n_2 = SiiRegRead( cInstance, REG_N_HVAL2_P0);
	n_3 = SiiRegRead( cInstance, REG_N_HVAL3_P0);
	return (n_3 << 16) | (n_2 << 8) | n_1;
}



//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioFifoRedirSet
// Description:  Redirect Audio FIFO for RX extraction or TX insertion
// Parameters:   isTxInsertion - TX insertion, if true
//------------------------------------------------------------------------------

void SiiDrvRxAudioFifoRedirSet( BYTE cInstance,bool_t isTxInsertion)
{
    SiiRegModify( cInstance, REG_AUDP_CTRL, BIT_REDIR_AUD_FIFO, isTxInsertion ? VAL_AUD_FIFO_TX_INSERT : VAL_AUD_FIFO_RX_EXTR);
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioGetMclk
// Description:  return the audio Mclk for DSD, HBRA and PCM
// Parameters:  
// Returns:     0: 128, 1:256, 2:384, 4: 512
//------------------------------------------------------------------------------
uint8_t SiiDrvRxAudioGetMclk( BYTE cInstance)
{
	uint8_t mclk;
	uint8_t audType;

    audType = SiiRegRead( cInstance, REG_AUDIOAUDP_STAT);
    mclk = SiiRegRead( cInstance, REG_AUDIOAAC_MCLK_SEL);

	if (audType & BIT_HBRA_ON)
	{
		mclk = (mclk >> MSK_MCLK_HBRA_SHIFT) & MSK_MCLK;
	} 
	else if (audType & BIT_DSD_STATUS)
	{
		mclk = (mclk >> MSK_MCLK_DSD_SHIFT) & MSK_MCLK;
	}
	else
	{
		mclk = (mclk >> MSK_MCLK_PCM_SHIFT) & MSK_MCLK;
	}	
	return mclk;
}


//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioSampleRateGet
// Description:  return the audio Fs
// Parameters:  
// Returns:     
//------------------------------------------------------------------------------
uint8_t SiiDrvRxAudioSampleRateGet( BYTE cInstance)
{
	uint8_t audFs;

    audFs = SiiRegRead( cInstance, REG_AUDIO_FS) & MSK_AUD_FS;

	switch (audFs)
	{
		case SII_AUD_CHST4_FS_44:
			return SII_AUD_FS_44_1;
		case SII_AUD_CHST4_FS_88:
			return SII_AUD_FS_88_2;
		case SII_AUD_CHST4_FS_176:
			return SII_AUD_FS_176_4;
		case SII_AUD_CHST4_FS_48:
			return SII_AUD_FS_48;
		case SII_AUD_CHST4_FS_96:
			return SII_AUD_FS_96;
		case SII_AUD_CHST4_FS_192:
			return SII_AUD_FS_192;	
		case SII_AUD_CHST4_FS_32:
			return SII_AUD_FS_32;
	}
	return SII_AUD_FS_48;
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioModeGet
// Description:  return the audio mode: PCM or DSD or HBRA
// Parameters:  
// Returns:     1: HBRA, 2: DSD, 3: PCM
//------------------------------------------------------------------------------
uint8_t SiiDrvRxAudioModeGet( BYTE cInstance)
{
	uint8_t audRx;

    audRx = SiiRegRead( cInstance, REG_AUDIOAUDP_STAT);

	if (audRx & BIT_HBRA_ON)
	{
		return SII_AUD_HBRA_MODE;
	}
	else if (audRx & BIT_DSD_STATUS)
	{
		return SII_AUD_DSD_MODE;
	}
	else
	{
		return SII_AUD_PCM_MODE;
	}
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioLayoutGet
// Description:  Audio Layout: multi-channel or two channel.
// Parameters:  none
// Returns:  0: two channels 
//          1: multi channels 
//------------------------------------------------------------------------------
uint8_t SiiDrvRxAudioLayoutGet( BYTE cInstance)
{
	uint8_t audRx;

    audRx = SiiRegRead( cInstance, REG_AUDIOAUDP_STAT);

	if (audRx & BIT_AUDIO_LAYOUT)
	{
		return SII_AUD_MULTI_CHANNEL;
	}

	return SII_AUD_TWO_CHANNEL;
		
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioReset
// Description:  Reset the audio
// Parameters:  
// Returns:     n/a
//------------------------------------------------------------------------------
void SiiDrvRxAudioReset( BYTE cInstance,uint8_t audResetType)
{
	SiiRegModify(  cInstance, REG_AUD_RST, audResetType, SET_BITS);
	SiiRegModify(  cInstance, REG_AUD_RST, audResetType, CLEAR_BITS);
}


//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioMute
// Description:  Mute/Unmute the audio
// Parameters:
// Returns:     n/a
//------------------------------------------------------------------------------

void SiiDrvRxAudioMute( BYTE cInstance,bool_t isMuted)
{
    SiiRegModify(  cInstance, REG_AUDIOHDMI_MUTE, BIT_AUDM, isMuted ? SET_BITS : CLEAR_BITS);
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioSoftMute
// Description:  Mute/Unmute the audio
// Parameters:
// Returns:     n/a
//------------------------------------------------------------------------------

void SiiDrvRxAudioSoftMute( BYTE cInstance,bool_t isMuted)
{
    SiiRegModify(  cInstance, REG_AUDIOAUD_CTRL, BIT_MUTE_MODE , isMuted ? SET_BITS : CLEAR_BITS);
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioACRControl
// Description:  
// Parameters:  
// Returns:     n/a
//------------------------------------------------------------------------------
void SiiDrvRxAudioACRControl( BYTE cInstance, uint8_t bitFields, uint8_t ctrl )
{
	SiiRegModify(  cInstance, REG_AUDIOACR_CTRL1, bitFields, ctrl ? SET_BITS : CLEAR_BITS);
}


//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioGetIntStatus
// Description:  Get Audio Int status of main or sub channel
// Parameters:  
// Returns:     n/a
//------------------------------------------------------------------------------
void SiiDrvRxAudioGetIntStatus( BYTE cInstance, uint8_t *pStatus)
{
	SiiRegReadBlock(  cInstance, REG_AUDIO_INTR11, pStatus, 3);
}


//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioClearIntStatus
// Description:  clear Audio Int status of main or sub channel
// Parameters:  pStatus points to the interrupt bits that going to be clear
// Returns:     void
//------------------------------------------------------------------------------
void SiiDrvRxAudioClearIntStatus( BYTE cInstance, uint8_t *pStatus)
{
	SiiRegWriteBlock(  cInstance, REG_AUDIO_INTR11 , pStatus, 3);
}


//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioInit
// Description:  Initialize the audio for both main and sub channel
// Parameters:  
// Returns:     n/a
//------------------------------------------------------------------------------
void SiiDrvRxAudioInit (  BYTE cInstance )
{
    uint8_t intStatus[3] = { 0xFF, 0xFF, 0xFF };

    memset(&RxAudioIntStatus[cInstance][0], 0x00, sizeof(AudDrvInstanceData_t));
    memset(&RxAudioIntStatus[cInstance][1], 0x00, sizeof(AudDrvInstanceData_t));

	SiiRegWrite( cInstance,  REG_APLL_POLE, VAL_AACR_CFG1_VALUE);  //set pll config #1
	SiiRegWrite( cInstance,  REG_APLL_CLIP, VAL_AACR_CFG2_VALUE);  //set pll config #2
	SiiRegWriteBlock( cInstance , REG_AUDIO_INTR11, intStatus, 3); //clear the audio status

    intStatus[0] = (BIT_SYNC_DETECT| BIT_CKDT_DETECT | BIT_CABLE_UNPLUG); // BIT_CABLE_UNPLUG removed in order to make audio working with MHL                     
    intStatus[1] = (BIT_HDMI_MODE_CHANGED | BIT_AUDIO_FIFO_UNDERUN | BIT_AUDIO_FIFO_OVERRUN | BIT_FS_CHANGED |
                     BIT_H_RES_CHANGED);
    intStatus[2] = (BIT_V_RES_CHANGED);

    SiiRegWriteBlock(  cInstance ,REG_AUDIOAEC_EN1, intStatus, 3);
	SiiRegModify(cInstance,REG_INT_ENABLE_IP4, BIT_MP_NEW_ACP, SET_BITS);

    intStatus[0] = BIT_AAC_UNMUTE_GOT_CTS | BIT_AAC_UNMUTE_GOT_NEW_AUDIO;
    intStatus[1] = BIT_AAC_UNMUTE_GOT_HDMI | BIT_AAC_UNMUTE_GOT_FIFO_UNDER | BIT_AAC_UNMUTE_GOT_FIFO_OVER;
    intStatus[2] = 0x17;

    SiiRegWriteBlock( cInstance ,REG_AUDIOAEC3_CTRL, intStatus, 3);
    SiiRegModify( cInstance ,REG_AUDIOAAC_MCLK_SEL, 0xC0, 0xC0);    //add for unmute timing    

    SiiRegWrite( cInstance,  REG_AUDIOAUD_CTRL,  VAL_AUD_CTRL );
    SiiRegModify( cInstance , REG_AUDIO_I2S_CTRL2, BIT_MCLKEN, BIT_MCLKEN);  //enable MCLK    
    SiiRegModify(  cInstance ,REG_AUDIOHDMI_MUTE, BIT_AUDM, CLEAR_BITS);   
    
    SiiRegModify(  cInstance,  REG_AUDIOAEC_CTRL,  BIT_AAC_EN | BIT_AAC_ALL | BIT_AAC_ACR_EN | BIT_AAC_OE /*0xB7*/, SET_BITS);        //enable auto video/audio configuration
    // Do not set BIT_AAC_ALL
    //SiiRegModify( REG_AUDIOAEC_CTRL,   BIT_AAC_EN|BIT_AAC_ACR_EN | BIT_AAC_OE /*0xB7*/, SET_BITS);        //enable auto video/audio configuration

	SiiDrvRxAudioInitMclk( cInstance );
	//SiiDrvRxAudioEnableCHSTINT();
}

//------------------------------------------------------------------------------
// @brief   Place Audio RX driver in standby
//------------------------------------------------------------------------------
bool_t SiiDrvRxAudioStandby (  BYTE cInstance )
{
    uint8_t intStatus[3] = { 0xFF, 0xFF, 0xFF };

    //Disable interrupts used by the audio driver
    SiiRegBitsSet( cInstance,  REG_INT_ENABLE_IP4, BIT_MP_NEW_ACP, false );
    SiiDrvRxAudioIntMaskEnable(  cInstance ,false );

    // Clear outstanding interrupts
    SiiRegWrite( cInstance,  REG_INT_STATUS_IP4, BIT_MP_NEW_ACP );
    SiiRegWrite( cInstance,  REG_INT_STATUS_IP5, BIT_RP_NEW_ACP );
    SiiDrvRxAudioClearIntStatus(  cInstance, intStatus );

    // Tri-state audio outputs for standby mode
    SiiRegWrite( cInstance,  REG_AU_PD_SYS, 0 );
    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioInterruptMaskEnable
// Description:  Enable the mask to assert the global (PIN) interrupts.
// Parameters:  
// Returns:     n/a
//------------------------------------------------------------------------------
void SiiDrvRxAudioIntMaskEnable ( BYTE cInstance, bool_t isEnable )
{
    uint8_t intStatus[3] = { 0x00, 0x00, 0x00 };

    // Enable/Disable AAC hardware function
    SiiRegModify(cInstance,REG_AUDIOAEC_CTRL, BIT_AAC_EN, (isEnable) ? SET_BITS : CLEAR_BITS);

    if (isEnable)
    {
		intStatus[0] = BIT_AAC_UNMUTE;
		intStatus[1] = BIT_AAC_DONE | BIT_FNCHG;
//		intStatus[1] = BIT_AAC_DONE;
//		intStatus[2] = BIT_FSCHG;
		//SiiRegWrite(REG_AUDIO_CHAN_SEL, 0x0);
		//SiiRegWrite(REG_AUDRX_CTRL_P1, 0x30);
    }
	SiiRegWriteBlock( cInstance,REG_AUDIO_INTR11_MASK,  intStatus, 3);
	SiiRegModify(cInstance,REG_INT_ENABLE_IP5, BIT_RP_NEW_ACP, isEnable ? SET_BITS : CLEAR_BITS);
	SiiRegModify(cInstance,REG_AUDIOACR_CTRL3, VAL_CTS_THRESH_MASK, 0x04 << VAL_CTS_THRESH_SHIFT);  //set ACR interrupt threshold
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioControl
// Description:   
// Parameters:  
// Returns:     n/a
//------------------------------------------------------------------------------
//void SiiDrvRxAudioControl ( SiiRxAudioControl_t audSel )
//{
//	uint8_t bitCtrl = CLEAR_BITS;
//
//	switch (audSel)
//	{
//	    case SII_AUD_SUB_MULTI_CHANNEL:
//	        bitCtrl = SET_BITS;
//	    case SII_AUD_MAIN_MULTI_CHANNEL:
//	        SiiRegModify(REG_AUDIO_CHAN_SEL, BIT_MULTI_CHN_OUTPUT, bitCtrl);
//	        break;
//
//	    case SII_AUD_PIN_INSERTION:
//	        bitCtrl = SET_BITS;
//	    case SII_AUD_PIN_EXTRACTION:
//	        SiiRegModify(REG_AUDIO_CHAN_SEL, BIT_PIN_DIR, bitCtrl);
//	        break;
//
//	    case SII_AUD_SWAP:
//	        bitCtrl = SET_BITS;
//	    case SII_AUD_NORMAL:
//	        SiiRegModify(REG_AUDIO_CHAN_SEL, BIT_MULTI_CHN_OUTPUT, bitCtrl);
//	        break;
//
//	    default:
//	        break;
//	}
//}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxSpdifOutEnable
// Description:  Enable or disable SPDIF output.
//               NOTE: In Sub Pipe, disabling SPDIF changes the pin assignment to I2S.
// Parameters:
// Returns:     n/a
//------------------------------------------------------------------------------

void SiiDrvRxSpdifOutEnable( BYTE cInstance, bool_t isEnabled)
{
    SiiRegBitsSet( cInstance ,REG_AUDIOAUD_CTRL, BIT_SPEN, isEnabled);
    (pDrvRxAud[cInstance])->isSpdifOutEnabled = isEnabled;
}


//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioIsInterruptAssert
// Description:  return the status of the interrupts
// Parameters:  
// Returns:     n/a
//------------------------------------------------------------------------------
uint8_t SiiDrvRxAudioIsInterruptAssert(  BYTE cInstance )
{
	return (pDrvRxAud[cInstance])->intStatus;
}


//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioInterruptClearAssert
// Description:  clear the interrupts
// Parameters:  
// Returns:     n/a
//------------------------------------------------------------------------------
void SiiDrvRxAudioInterruptClearAssert( BYTE cInstance)
{
	(pDrvRxAud[cInstance])->intStatus = 0;
}

//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioProcessInterrupts
// Description:  Initialize the audio state machine and audio hardware start up
//				for both main and sub channel
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------
void SiiDrvRxAudioProcessInterrupts (  BYTE cInstance )
{   
	(pDrvRxAud[cInstance])->intStatus = 1;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Configure Audio PLL to either generate fixed Fout = 11/4 * Fin
//!             or automatically defined frequency. The fixed frequency can be used for
//!             internal video generation (74.25 MHz = 11/4 * 27 MHz).
//!             The auto mode is for audio extraction purposes.
//!
//! @param[in]  isOvrd - generate manually set frequency, if true.
//-------------------------------------------------------------------------------------------------

void SiiDrvRxAudioPllConfig( BYTE cInstance,bool_t isOvrd)
{
    SiiRegModify( cInstance, REG_APLL_POLE, MSK_APLL_MANUAL_N, isOvrd ? VAL_APLL_MANUAL_N_11 : 0); // manual value for N = 11
    SiiRegModify( cInstance, REG_APLL_OVR_CTRL,  BIT_APLL_MANUAL_M_4 | BIT_APLL_OVR_N | BIT_APLL_OVR_M,
                 BIT_APLL_MANUAL_M_4 |                              /* manual value for M = 4 */
                (isOvrd ? (BIT_APLL_OVR_N | BIT_APLL_OVR_M) : 0));  /* choose manual value for N and M */
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusInstanceGet
// Description: Return the current instance index.
// Parameters:  none
// Returns:     instance index.
//------------------------------------------------------------------------------

uint8_t SiiDrvRxAudioInstanceGet (  BYTE cInstance )
{
    return (pDrvRxAud[cInstance])->instanceIndex;
}


//------------------------------------------------------------------------------
// Function:    SiiDrvRxAudioInstanceSet
// Description: Set the component global data pointer to the requested instance.
// Parameters:  instanceIndex
// Returns:     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------

bool_t SiiDrvRxAudioInstanceSet ( BYTE cInstance, uint_t instanceIndex )
{
    SiiRegInstanceSet( cInstance , PP_PAGE_AUDIO, instanceIndex );
    SiiRegInstanceSet(  cInstance ,PP_PAGE_AUDIO_INTS, instanceIndex );
    (pDrvRxAud[cInstance]) = &RxAudioIntStatus[cInstance][instanceIndex];
    (pDrvRxAud[cInstance])->instanceIndex = instanceIndex;

    return( true );

}
//------------------------------------------------------------------------------
//! @brief  Verify that the actual audio input Fs matches the value programmed
//!         into the divider hardware and update if necessary.
//------------------------------------------------------------------------------
uint8_t SiiDrvRxAudioCheckFsAndFix (  BYTE cInstance )
{
	int_t tmds_clk_10kHz ;
	uint8_t fs_code_per_channel;
	uint16_t fs_frequency_from_chst;
	uint16_t fs_calculated_100Hz;
	uint32_t n;
	uint32_t cts;
	uint8_t i;

	if((SiiRegRead( cInstance, REG_AUDIO_INTR11) & (BIT_GOTAUD|BIT_GOTCTS)) != (BIT_GOTAUD|BIT_GOTCTS))
	{
	    //no aud, clear the override bit
	    SiiRegModify( cInstance,  REG_AUDIOACR_CTRL1, BIT_FS_SEL, CLEAR_BITS);
        return 0;
	}
    //not DSD mode
	else if ((SiiDrvRxAudioModeGet( cInstance )==SII_AUD_PCM_MODE)||
            (SiiDrvRxAudioModeGet( cInstance )==SII_AUD_HBRA_MODE))
    {
		fs_frequency_from_chst = SiiRegRead( cInstance, REG_AUDIOTCLK_FS) & MSK__TCLKFS;
        tmds_clk_10kHz = SiiDrvSwitchTmdsClockGet( cInstance, ((pDrvRxAud[cInstance])->instanceIndex==0));
        n = GetN( cInstance );
        cts = GetCts( cInstance );

        if( (n != 0) && (cts != 0) ){
        	// Calculate the actual input Fs
        	// Note: the order of operations is optimized for the maximum precision.
        	// Overflow should not occur during the operations.
        	// tmds_clk_10kHz - assume maximum value 30,000 (15bit) for 300 MHz
        	// n - assume maximum value 192kHz*128/300=81000 (17bit)
        	// tmds_clk_10kHz*n should fit into 32 bits
        	// Maximum fs_calculated_100Hz is 192000/100=1920 fits into 16bit
	        fs_calculated_100Hz = (n*tmds_clk_10kHz/cts)*100 /128;

        	// Find closest standard audio Fs.
        	for(i = 0; i < AUDIO_FS_LIST_LENGTH; i++)
        	{
        		if (fs_calculated_100Hz < audio_fs_list[i].max_Fs)
        		{
        			break;
        		}
        	}
        	fs_code_per_channel = audio_fs_list[i].code_value;
        }else
        {
        	fs_code_per_channel = fs_frequency_from_chst;
        }

        if(fs_code_per_channel == fs_frequency_from_chst)
        {
            SiiRegModify( cInstance,  REG_AUDIOACR_CTRL1, BIT_FS_SEL, CLEAR_BITS);//no override
            return fs_code_per_channel;
        }
        // If calculated Fs and channel status Fs do not match,
        else
#if 0
        	if (
            // AND either the calculated Fs does not match the programmed Fs OR
            // we're not USING the programmed Fs, then...
            ((fs_code_per_channel != (SiiRegRead( REG_AUDIOFREQ_SVAL) & MSK_SWFS)) ||
            (SiiRegRead(REG_AUDIOACR_CTRL1)& BIT_FS_SEL) != BIT_FS_SEL))

#endif
        {
/*
            DEBUG_PRINT(MSG_ALWAYS,"Fs(calculated=%d ) not match (ch4 = %d): tmds=%d, n=%d, cts=%d, fs_cal=%d\n",
                    fs_code_per_channel, fs_frequency_from_chst, tmds_clk_10kHz, n, cts, fs_calculated_100Hz);
*/

            // Program the Fs code that matches the calculated Fs and
            // tell the hardware to use the programmed value.
            SiiRegModify( cInstance, REG_AUDIOFREQ_SVAL, MSK_SWFS, fs_code_per_channel);
            SiiRegModify( cInstance, REG_AUDIOACR_CTRL1, BIT_FS_SEL, SET_BITS);
			return fs_code_per_channel;
        }
    }
	else
    {
	    SiiRegModify( cInstance, REG_AUDIOACR_CTRL1, BIT_FS_SEL, CLEAR_BITS);//clear for DSD mode
    	return SiiDrvRxAudioSampleRateGet( cInstance );
    }
}

void SiiDrvRxAudioInternalMute( BYTE cInstance,bool_t qOn)
{
	if(qOn)
	{
		(pDrvRxAud[cInstance])->unmuteIntrHappened = false;
        SiiDrvRxAudioCheckFsAndFix( cInstance );
	}
}

void SiiDrvRxAudioUnmuteReady( BYTE cInstance, bool_t qOn)
{
	(pDrvRxAud[cInstance])->unmuteIntrHappened = qOn;
	if(qOn)
	{
	    // Enable audio output and verify the Fs
        //SiiDrvRxAudioCheckFsAndFix();

	    SiiRegModify( cInstance,  REG_AUDIOAEC_CTRL,  BIT_AAC_OE, SET_BITS);
	}

}

bool_t SiiDrvRxAudioGetNewAcpInt( BYTE cInstance)
{
	bool_t result;
	if((pDrvRxAud[cInstance])->instanceIndex == 0)
	{
		result = (SiiRegRead( cInstance, REG_INT_STATUS_IP4) & BIT_MP_NEW_ACP) == BIT_MP_NEW_ACP;
		SiiRegWrite( cInstance, REG_INT_STATUS_IP4, BIT_MP_NEW_ACP);
	}
	else
	{
		result = (SiiRegRead( cInstance, REG_INT_STATUS_IP5) & BIT_RP_NEW_ACP) == BIT_RP_NEW_ACP;
		SiiRegWrite( cInstance, REG_INT_STATUS_IP5, BIT_RP_NEW_ACP);
	}

	(pDrvRxAud[cInstance])->newAcpIntrHappened = result;

	return result;
}

static uint8_t AcpTypeGet( BYTE cInstance)
{
    uint8_t reg;
    uint8_t typeData;

    reg = SiiRegRead( cInstance, REG_PORT_INFOFRAME_SELECT);
    //here instead of the instance set function
	//could be changed by other modules
    // Select main or sub pipe to be source of the info frame data
    SiiRegModify(  cInstance, REG_PORT_INFOFRAME_SELECT, BIT_RD_PIPE_SEL,
                   (((pDrvRxAud[cInstance])->instanceIndex == 0) ? CLEAR_BITS : SET_BITS));
    // Read requested IF data
    typeData = SiiRegRead( cInstance, REG_SP_ACP_HEADER + 1);
    // Recover port selection settings
    SiiRegWrite( cInstance, REG_PORT_INFOFRAME_SELECT, reg);
    return (typeData);

}

void SiiDrvRxAudioOnNoAcp( BYTE cInstance)
{
	//turn on digital output
    SiiRegBitsSet( cInstance, REG_AUDIOAUD_CTRL, BIT_SPEN, (pDrvRxAud[cInstance])->isSpdifOutEnabled);
}


bool_t SiiDrvRxAudioGetNoAcpInt( BYTE cInstance)
{
//Audio Unmute happens
	if ((pDrvRxAud[cInstance]) -> unmuteIntrHappened)
	{
		(pDrvRxAud[cInstance]) -> unmuteIntrHappened = false; //clear status
		//no New ACP interrupt
		if(!(pDrvRxAud[cInstance])->newAcpIntrHappened)
		//ACP buffer has type 2 there already
		{
			if(AcpTypeGet( cInstance )>1)
				return true;
		}
	}
	return false;
}


void SiiDrvRxAudioOnNewAcp( BYTE cInstance, bool_t qOn)
{
	if(qOn && (AcpTypeGet( cInstance )>1))
	{
		//disable the auto OE
		SiiRegModify( cInstance, REG_AUDIOAEC_CTRL, BIT_AAC_OE, CLEAR_BITS);
		//turn off digital output
		SiiRegBitsSet( cInstance, REG_AUDIOAUD_CTRL, BIT_SPEN, false);
		// Enable SDx channels while in manual mode
		SiiRegModify( cInstance, REG_AUDIO_I2S_CTRL2, MSK_MULTI_CHANNELS, (SiiDrvRxAudioLayoutGet(cInstance) == SII_AUD_MULTI_CHANNEL) ? MSK_MULTI_CHANNELS : MSK_TWO_CHANNELS );
	    SiiRegModify(  cInstance, REG_AUDIO_I2S_CTRL2, BIT_MCLKEN, BIT_MCLKEN);  //keeps MCLK SW settings.
	}
	else
	{
		SiiRegModify( cInstance, REG_AUDIOAEC_CTRL, BIT_AAC_OE, SET_BITS);
		//turn on digital output if it was enabled by configuration
        SiiRegBitsSet( cInstance, REG_AUDIOAUD_CTRL, BIT_SPEN, (pDrvRxAud[cInstance])->isSpdifOutEnabled);
	}

}

void SiiDrvRxAudioNoAcpPacketcheckStart( BYTE cInstance, bool_t qOn)
{

#if (SI_AUDIO_NO_ACP_WORKAROUND == true)
	if(qOn) //start
	{
		SiiRegModify( cInstance, REG_CLR_PACKET_BUFFER, BIT_VSI_ID_CHK_EN, CLEAR_BITS);
		SiiRegWrite( cInstance, REG_VSIF_CAPTURE_HEADER, 0x04); //for ACP use
	}
	else
	{
		SiiRegWrite( cInstance, REG_VSIF_CAPTURE_HEADER, 0x81); //restore
		SiiRegModify( cInstance, REG_CLR_PACKET_BUFFER, BIT_VSI_ID_CHK_EN, SET_BITS);
	}
#endif //(SI_AUDIO_NO_ACP_WORKAROUND == true)

}

bool_t SiiDrvRxAudioNoAcpPacketcheckConfirm( BYTE cInstance)
{
	//if no ACP shows up, return true,
    bool_t result = false;

#if (SI_AUDIO_NO_ACP_WORKAROUND == SI_ENABLE)
    uint8_t reg;

    reg = SiiRegRead( cInstance, REG_PORT_INFOFRAME_SELECT);
    //set the pipe to read
    SiiRegModify(  cInstance, REG_PORT_INFOFRAME_SELECT,
    		(((pDrvRxAud[cInstance])->instanceIndex == 0) ? CLEAR_BITS : BIT_RD_CS_PIPE_SEL)
    		|BIT_CS_PORT_PIPE_SEL, SET_BITS );
    // Read requested IF data
    result = SiiRegRead( cInstance,  REG_RP_VISF_CONTENT_HEADER) != 0x04;
    // Recover port selection settings
    SiiRegWrite( cInstance, REG_PORT_INFOFRAME_SELECT, reg);
#else
    // Read ACP data
    result = (SiiRegRead( cInstance,  REG_SP_ACP_HEADER) != 0x04);

#endif //#if (SI_AUDIO_NO_ACP_WORKAROUND == true)

	return (result);
}

void SiiDrvRxAudioVsifCheckedByOthers( BYTE cInstance)
{
	(pDrvRxAud[cInstance])->vsifCheckedByOthers = true;
}

bool_t SiiDrvRxAudioAcpCheckDisturbance( BYTE cInstance)
{
	bool_t result =	(pDrvRxAud[cInstance])->vsifCheckedByOthers;
	(pDrvRxAud[cInstance])->vsifCheckedByOthers = false;

	return result;

}

void SiiDrvRxAudioChstGet( BYTE cInstance, uint8_t *pChst)
{

	*pChst++ = SiiRegRead( cInstance, REG_CHST1_P0);
	*pChst++ = SiiRegRead( cInstance, REG_CHST2_P0);
	*pChst++ = SiiRegRead( cInstance, REG_CHST3_P0);
	*pChst++ = SiiRegRead( cInstance, REG_CHST4_P0);
	*pChst = SiiRegRead( cInstance, REG_CHST5_P0);

}

void SiiDrvRxAudioEnableAAC( BYTE cInstance)
{
	SiiRegModify(  cInstance, REG_AUDIOAEC_CTRL,  BIT_AAC_ALL, SET_BITS);
}
void SiiDrvRxAudioDisableAAC( BYTE cInstance)
{
	SiiRegModify( cInstance,  REG_AUDIOAEC_CTRL,  BIT_AAC_ALL, CLEAR_BITS);
}
void SiiDrvRxAudioEnableCHSTINT( BYTE cInstance)
{
	SiiRegModify( cInstance,  REG_AUDIO_INTR11_MASK + 2, BIT_FSCHG, SET_BITS);
}
void SiiDrvRxAudioDisableCHSTINT( BYTE cInstance)
{
	SiiRegModify(  cInstance, REG_AUDIO_INTR11_MASK + 2, BIT_FSCHG, CLEAR_BITS);
}

void SiiDrvRxAudioSetMclk( BYTE cInstance, uint8_t audMode, uint8_t audFs){
	uint8_t cMclksel, bAcrInit;
	bAcrInit = 0;
	cMclksel = SiiRegRead( cInstance, REG_AUDIOAAC_MCLK_SEL);

	if(audMode == SII_AUD_DSD_MODE){
		if( (cMclksel & (MSK_MCLK << MSK_MCLK_DSD_SHIFT)) != (SII_AUD_MCLK_512 << MSK_MCLK_DSD_SHIFT) ){
			SiiRegModify(  cInstance, REG_AUDIOAAC_MCLK_SEL, MSK_MCLK << MSK_MCLK_DSD_SHIFT, SII_AUD_MCLK_512 << MSK_MCLK_DSD_SHIFT);
			bAcrInit = 1;
		}
		SiiRegModify(  cInstance, REG_AUDIOFREQ_SVAL, MSK_SWMCLK, VAL_SWMCLK_512);	// ACR Audio Frequency Register
	}
	else if(audMode == SII_AUD_HBRA_MODE)
	{
		switch(audFs){
		case SII_AUD_CHST4_FS_176:
		case SII_AUD_CHST4_FS_192:
			if( (cMclksel & (MSK_MCLK << MSK_MCLK_HBRA_SHIFT)) != (SII_AUD_MCLK_128 << MSK_MCLK_HBRA_SHIFT) ){
				SiiRegModify( cInstance,  REG_AUDIOAAC_MCLK_SEL, MSK_MCLK << MSK_MCLK_HBRA_SHIFT, SII_AUD_MCLK_128 << MSK_MCLK_HBRA_SHIFT);
				bAcrInit = 1;
			}
			SiiRegModify( cInstance,  REG_AUDIOFREQ_SVAL, MSK_SWMCLK, VAL_SWMCLK_128);	// ACR Audio Frequency Register
			break;
		case SII_AUD_CHST4_FS_88:
		case SII_AUD_CHST4_FS_96:
			if( (cMclksel & (MSK_MCLK << MSK_MCLK_HBRA_SHIFT)) != (SII_AUD_MCLK_256 << MSK_MCLK_HBRA_SHIFT) ){
				SiiRegModify( cInstance,  REG_AUDIOAAC_MCLK_SEL, MSK_MCLK << MSK_MCLK_HBRA_SHIFT, SII_AUD_MCLK_256 << MSK_MCLK_HBRA_SHIFT);
				bAcrInit = 1;
			}
			SiiRegModify( cInstance,  REG_AUDIOFREQ_SVAL, MSK_SWMCLK, VAL_SWMCLK_256);	// ACR Audio Frequency Register
			break;
		case SII_AUD_CHST4_FS_32:
		case SII_AUD_CHST4_FS_44:
		case SII_AUD_CHST4_FS_48:
		default:
			if( (cMclksel & (MSK_MCLK << MSK_MCLK_HBRA_SHIFT)) != (SII_AUD_MCLK_512 << MSK_MCLK_HBRA_SHIFT) ){
				SiiRegModify(  cInstance, REG_AUDIOAAC_MCLK_SEL, MSK_MCLK << MSK_MCLK_HBRA_SHIFT, SII_AUD_MCLK_512 << MSK_MCLK_HBRA_SHIFT);
				bAcrInit = 1;
			}
			SiiRegModify( cInstance,  REG_AUDIOFREQ_SVAL, MSK_SWMCLK, VAL_SWMCLK_512);	// ACR Audio Frequency Register
			break;
		}
	}
	else
	{	/* SII_AUD_PCM_MODE */
		switch(audFs){
		case SII_AUD_CHST4_FS_176:
		case SII_AUD_CHST4_FS_192:
			if( (cMclksel & (MSK_MCLK << MSK_MCLK_PCM_SHIFT)) != (SII_AUD_MCLK_128 << MSK_MCLK_PCM_SHIFT) ){
				SiiRegModify(  cInstance, REG_AUDIOAAC_MCLK_SEL, MSK_MCLK << MSK_MCLK_PCM_SHIFT, SII_AUD_MCLK_128 << MSK_MCLK_PCM_SHIFT);
				bAcrInit = 1;
			}
			SiiRegModify(  cInstance, REG_AUDIOFREQ_SVAL, MSK_SWMCLK, VAL_SWMCLK_128);	// ACR Audio Frequency Register
			break;
		case SII_AUD_CHST4_FS_88:
		case SII_AUD_CHST4_FS_96:
			if( (cMclksel & (MSK_MCLK << MSK_MCLK_PCM_SHIFT)) != (SII_AUD_MCLK_256 << MSK_MCLK_PCM_SHIFT) ){
				SiiRegModify(  cInstance, REG_AUDIOAAC_MCLK_SEL, MSK_MCLK << MSK_MCLK_PCM_SHIFT, SII_AUD_MCLK_256 << MSK_MCLK_PCM_SHIFT);
				bAcrInit = 1;
			}
			SiiRegModify(  cInstance, REG_AUDIOFREQ_SVAL, MSK_SWMCLK, VAL_SWMCLK_256);	// ACR Audio Frequency Register
			break;
		case SII_AUD_CHST4_FS_32:
		case SII_AUD_CHST4_FS_44:
		case SII_AUD_CHST4_FS_48:
		default:
			if( (cMclksel & (MSK_MCLK << MSK_MCLK_PCM_SHIFT)) != (SII_AUD_MCLK_512 << MSK_MCLK_PCM_SHIFT) ){
				SiiRegModify( cInstance,  REG_AUDIOAAC_MCLK_SEL, MSK_MCLK << MSK_MCLK_PCM_SHIFT, SII_AUD_MCLK_512 << MSK_MCLK_PCM_SHIFT);
				bAcrInit = 1;
			}
			SiiRegModify( cInstance,  REG_AUDIOFREQ_SVAL, MSK_SWMCLK, VAL_SWMCLK_512);	// ACR Audio Frequency Register
			break;
		}
	}
	if( bAcrInit ){ // if MCLK ratio changed...
		SiiDrvRxAudioReset( cInstance, BIT_ACRRST);
	}
}

void SiiDrvRxAudioInitMclk( BYTE cInstance)
{
	SiiRegWrite( cInstance, REG_AUDIOAAC_MCLK_SEL,0xc0 | (SII_AUD_MCLK_512 << MSK_MCLK_DSD_SHIFT));
	SiiRegModify( cInstance,  REG_AUDIOFREQ_SVAL, MSK_SWMCLK, VAL_SWMCLK_128);	// ACR Audio Frequency Register

	SiiDrvRxAudioReset( cInstance, BIT_ACRRST);
}
