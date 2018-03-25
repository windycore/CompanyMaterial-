//************************************************************************************************
//! @file   si_tx_audio.c
//! @brief  Audio control
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
#include "si_tx_audio.h"
#include "si_tx_info_frm.h"
#include "si_video_tables.h"
#include "si_drv_tpi_system.h"
#include "si_edid_tx_internal.h"

#include "Mcu.h"
#include "uart.h"

#define AUDIO_STATUS_CHANNEL_LEN 5
#define CHST1 0
#define CHST2 1
#define CHST3 2
#define CHST4 3
#define CHST5 4


//-------------------------------------------------------------------------------------------------
// Module Instance Data
//-------------------------------------------------------------------------------------------------

TxAudioInstanceRecord_t AudioInstRec[INSTANCE_9533][SII_NUM_TX];
TxAudioInstanceRecord_t *pAudio[INSTANCE_9533] = 
{
    &AudioInstRec[0][0],
#if (INSTANCE_9533 >1 )
    &AudioInstRec[1][0],
#endif
#if (INSTANCE_9533 >2 )
    &AudioInstRec[2][0],
#endif
#if (INSTANCE_9533 >3 )
    &AudioInstRec[3][0],
#endif
#if (INSTANCE_9533 >4 )
    &AudioInstRec[4][0],
#endif
#if (INSTANCE_9533 >5 )
    &AudioInstRec[5][0],
#endif
#if (INSTANCE_9533 >6 )
    &AudioInstRec[6][0],
#endif
#if (INSTANCE_9533 >7 )
    &AudioInstRec[7][0],
#endif
};


//-------------------------------------------------------------------------------------------------
//  Local service functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//! @brief      Check if the current input audio mode is SPDIF or DSD.
//!
//! @retval     true  - SPDIF or DSD,
//! @retval     false - other modes
//-------------------------------------------------------------------------------------------------

static bool_t IsSpdifOrDsd( BYTE cInstance)
{
    bool_t isSpdifDsd;

    switch (pAudio[cInstance]->audioSrcMode)
    {
        case SI_TX_AUDIO_SRC_SPDIF:
        case SI_TX_AUDIO_SRC_DSD_L0:
        case SI_TX_AUDIO_SRC_DSD_L1:
            isSpdifDsd = true;
            break;
        default:
            isSpdifDsd = false;
    }
    return isSpdifDsd;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Translates audio source mode into I2S group index.
//!
//! @param[in]  source - audio source.
//!
//! @return     I2S group index.
//-------------------------------------------------------------------------------------------------
// not  BYTE cInstance
static txAudioSrcGrp_t I2sSourceGroupGet(txAudioSrc_t source)
{
    txAudioSrcGrp_t group;

    switch (source)
    {
        default:
        case SI_TX_AUDIO_SRC_I2S_L0:
        case SI_TX_AUDIO_SRC_I2S_L1:
            group = TX_AUDIO_SRC_GRP_I2S;
            break;

        case SI_TX_AUDIO_SRC_DSD_L0:
        case SI_TX_AUDIO_SRC_DSD_L1:
            group = TX_AUDIO_SRC_GRP_DSD;
            break;

        case SI_TX_AUDIO_SRC_HBRA:
            group = TX_AUDIO_SRC_GRP_HBR;

    }

    return group;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return maximum allowed I2S sample rate per channel for current video mode.
//!
//! @return     Sample frequency in kHz.
//-------------------------------------------------------------------------------------------------

static audioSampleRate_t MaxSampleRateForCurrentVideoModeGet( BYTE cInstance)
{
    uint8_t maxSampleRate = 192; // 192 kHz is maximum Fs in HDMI 1.4a per I2S channel
    audioSampleRate_t smpRate = SI_TX_AUDIO_SMP_RATE_192_KHZ;
    uint8_t videoId = TxVideoFormatIdGet( cInstance );

    if (videoId != SI_VIDEO_MODE_NON_STD)
    {
        // It is theoretically possible that Standard Definition resolutions
        // have Deep Color mode and in this case it would be possible
        // to pass higher rate audio, but it is not typical case.
        // It is OK to ignore it and use pixel clock instead of TMDS clock
        // for such checking here.
        maxSampleRate = VideoModeTable[videoId].MaxAudioSR8Ch;
    }

    // Convert into frequency codes
    switch (maxSampleRate)
    {
        case 32:
            smpRate = SI_TX_AUDIO_SMP_RATE_32_KHZ;
            break;
        case 44:
            smpRate = SI_TX_AUDIO_SMP_RATE_44_KHZ;
            break;
        case 48:
            smpRate = SI_TX_AUDIO_SMP_RATE_48_KHZ;
            break;
        case 88:
            smpRate = SI_TX_AUDIO_SMP_RATE_88_KHZ;
            break;
        case 96:
            smpRate = SI_TX_AUDIO_SMP_RATE_96_KHZ;
            break;
        case 176:
            smpRate = SI_TX_AUDIO_SMP_RATE_176_KHZ;
            break;
        case 192:
            smpRate = SI_TX_AUDIO_SMP_RATE_192_KHZ;
            break;
    }

    return smpRate;
}



//-------------------------------------------------------------------------------------------------
//! @brief      Configure I2S interface to properly capture the input audio samples.
//-------------------------------------------------------------------------------------------------
#if (SI_TX_AUDIO_I2S_IN_BUS == SI_ENABLE)
static void I2sConfigSet( BYTE cInstance)
{
    bool_t isHbr = (pAudio[cInstance]->audioSrcMode == SI_TX_AUDIO_SRC_HBRA);
    bool_t isAudioEncoded = (pAudio[cInstance]->audioFormat.encoding > SI_TX_AUDIO_ENC_PCM);
    txAudioSrcGrp_t srcGroupIndex = I2sSourceGroupGet(pAudio[cInstance]->audioSrcMode);

    if (pAudio[cInstance]->audioSrcMode != SI_TX_AUDIO_SRC_SPDIF)
    {
        SiiDrvTpiAudioI2sConfigSet( cInstance ,&(pAudio[cInstance]->audioInterface[srcGroupIndex]), isHbr, isAudioEncoded);
    }
    else
    {
        // Use stream header information for SPDIF audio
        pAudio[cInstance]->audioFormat.smpRate = SI_TX_AUDIO_SMP_RATE_UNKNOWN;
        pAudio[cInstance]->audioFormat.smpSize = SI_TX_AUDIO_SMP_SIZE_UNKNOWN;
    }

    SiiDrvTpiAudioSampleConfigSet( cInstance ,pAudio[cInstance]->audioFormat.smpSize, pAudio[cInstance]->audioFormat.smpRate, isHbr);
}
#endif

//-------------------------------------------------------------------------------------------------
//! @brief      Connect multi-channel audio to I2S pipes and enable those pipes for capturing.
//!
//!             At least one I2S channel should be enabled
//!             (Equal to 2 DSD channels enabling).
//-------------------------------------------------------------------------------------------------

static void AudioChannelsMap( BYTE cInstance)
{
    audioSpkAlloc_t channelsMap;

    switch (pAudio[cInstance]->audioSrcMode)
    {
        case SI_TX_AUDIO_SRC_HBRA:
            channelsMap = SI_TX_AUDIO_SPK_FL_FR_LFE_FC_RL_RR_RC;
            break;

        case SI_TX_AUDIO_SRC_DSD_L1:
        case SI_TX_AUDIO_SRC_I2S_L1:
            // I2S or DSD input
            // Assume that I2S is in PCM format, otherwise it should not
            // have layout1 bit set.
            // In any case, if I2S input is encoded and layout1 is set,
            // there is no harm because encoded stream uses only I2S
            // channel 0 which is always enabled. In case of encoded signal
            // it does not matter what other channel carry.
            // The only case when encoded audio takes more than one I2S channel
            // is HBRA and all input audio channels are enabled in this case.
            channelsMap = pAudio[cInstance]->audioFormat.spkMap;
            break;

        default:
            channelsMap = SI_TX_AUDIO_SPK_FL_FR; // Stereo option is valid for SPDIF and L0 layouts
    }

    SiiDrvTpiAudioI2sPinsToFifoAssign( cInstance ,channelsMap, pAudio[cInstance]->isDownSampled);

}

//-------------------------------------------------------------------------------------------------
//! @brief      Check if PCM stereo audio with higher sample rates can be downsampled to basic audio
//!             should such sample rates be not supported by downstream.
//-------------------------------------------------------------------------------------------------

static void AudioDownsamplingUpdate( BYTE cInstance)
{
    bool_t isFsSupported = true;

    // Check if stereo downsamping is an applicable option (2ch PCM, Fs > 48kHz)
    if ((pAudio[cInstance]->audioFormat.encoding == SI_TX_AUDIO_ENC_PCM) && (pAudio[cInstance]->audioFormat.channelCount <= 2))
    {
        switch (pAudio[cInstance]->audioFormat.smpRate)
        {
            case SI_TX_AUDIO_SMP_RATE_88_KHZ:
                isFsSupported = SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_PCM_FS_88_KHZ);
                break;
            case SI_TX_AUDIO_SMP_RATE_96_KHZ:
                isFsSupported = SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_PCM_FS_96_KHZ);
                break;
            case SI_TX_AUDIO_SMP_RATE_176_KHZ:
                isFsSupported = SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_PCM_FS_176_KHZ);
                break;
            case SI_TX_AUDIO_SMP_RATE_192_KHZ:
                isFsSupported = SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_PCM_FS_192_KHZ);
                break;
            default:
                isFsSupported = true;
                break;
        }
    }
    pAudio[cInstance]->isDownSampled = !isFsSupported;
}


//-------------------------------------------------------------------------------------------------
//! @brief      The function verifies if audio transmission is permitted
//!             according to the current status.
//!
//! @retval     true  - if audio transmission is allowed,
//! @retval     false - otherwise.
//-------------------------------------------------------------------------------------------------

static bool_t IsAudioTransmissionAllowed( BYTE cInstance)
{
	bool_t isAllowed = false;
	txAudioSrc_t audioSrcMode = pAudio[cInstance]->audioSrcMode;
	txAudioSrcGrp_t audioSrcGroup = I2sSourceGroupGet(audioSrcMode);

	BEGIN_TEST
	{
		PASS_IF(pAudio[cInstance]->audioSrcMode != SI_TX_AUDIO_SRC_NONE);
		PASS_IF(SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_AUDIO));
		PASS_IF(SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_AI) || (!pAudio[cInstance]->isAudioContentProtected))
		PASS_IF(TxIsOutInHdmiMode( cInstance ));



		if(audioSrcGroup == TX_AUDIO_SRC_GRP_I2S)
		{
			if(pAudio[cInstance]->audioFormat.encoding > SI_TX_AUDIO_ENC_PCM)
			{
			    PASS_IF(SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_ENCODED_AUDIO));
			}
		}
		// Note: since it is not possible to know if audio signal coming in
		// through SPDIF is PCM or encoded, we cannot prevent encoded
		// stream going into PCM-only sink.
		// For this reason, it is always preferable using I2S input,
		// so the verification above works.

		if(audioSrcMode == SI_TX_AUDIO_SRC_I2S_L1)
		{
			PASS_IF(pAudio[cInstance]->audioFormat.smpRate <= MaxSampleRateForCurrentVideoModeGet( cInstance ));
		}
		else if(audioSrcGroup == TX_AUDIO_SRC_GRP_DSD)
		{
			// if incoming stream is DSD (1 bit audio)
		    PASS_IF(SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_1BIT_AUDIO)); // be sure DS device support it
		}
		else if(audioSrcMode == SI_TX_AUDIO_SRC_HBRA)
		{
			// if incoming stream is HBR (High Bit Ratio) Audio
		    PASS_IF(SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_HBR_AUDIO)); // be sure DS device support it
		}

		isAllowed = true; // if all tests have passed
	}
	END_TEST

	return isAllowed;
}

static audioSampleRate_t SmpRateToDownsampledSmpRate( BYTE cInstance, audioSampleRate_t smpRate)
{
    audioSampleRate_t outRate = smpRate;

    if (pAudio[cInstance]->isDownSampled)
    {
        switch (smpRate)
        {
            default:
                outRate = smpRate;
                break;
            case SI_TX_AUDIO_SMP_RATE_88_KHZ:
            case SI_TX_AUDIO_SMP_RATE_176_KHZ:
                outRate = SI_TX_AUDIO_SMP_RATE_44_KHZ; // 88.2 - DS -> 44.1
                break;
            case SI_TX_AUDIO_SMP_RATE_96_KHZ:
                outRate = SI_TX_AUDIO_SMP_RATE_32_KHZ; // 96 - DS -> 32
                break;
            case SI_TX_AUDIO_SMP_RATE_192_KHZ:
                outRate = SI_TX_AUDIO_SMP_RATE_48_KHZ; // 192 - DS -> 48
                break;
        }
    }
    return outRate;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Create an Audio Info Frame based on provided audio settings and update it
//!             in the chip.
//!
//! @param[in]  param - meaning
//!
//! @return     What's returned.
//! @retval     value - meaning
//-------------------------------------------------------------------------------------------------

static void AudioInfoFrameUpdate( BYTE cInstance)
{
    uint8_t aAudioInfoFrame[AUDIO_INFO_FRAME_LENGTH];

    memset(aAudioInfoFrame, 0, AUDIO_INFO_FRAME_LENGTH); // Initialize the info frame

    // Fill up the packet's header
    aAudioInfoFrame[0] = 0x84; // Packet type
    aAudioInfoFrame[1] = 0x01; // Version
    aAudioInfoFrame[2] = 0x0A; // Length of Audio Info Frame (embedded in the packet)
    aAudioInfoFrame[3] = 0;    // Check sum will be calculated by IF sending function

    aAudioInfoFrame[4] = ((pAudio[cInstance]->audioFormat.channelCount > 1) && (pAudio[cInstance]->audioFormat.channelCount <= 8 )) ?
                          (pAudio[cInstance]->audioFormat.channelCount - 1) : 0; // PB 1

    if (I2sSourceGroupGet(pAudio[cInstance]->audioSrcMode) == TX_AUDIO_SRC_GRP_DSD)
    {
        aAudioInfoFrame[5] = 2 << 2;  // PB 2 (SF2-0) - 44.1 kHz for DSD
    }
    else
    {
        aAudioInfoFrame[5] = 0; // Sample Rate in AIF must be 0 for SPDIF & PCM
    }


    aAudioInfoFrame[7] =  pAudio[cInstance]->audioFormat.spkMap; // PB 4
    aAudioInfoFrame[8] = (pAudio[cInstance]->audioFormat.isDwnMxInhibit ? 0x8 : 0)
                      | ((pAudio[cInstance]->audioFormat.lShift & 0xF) << 3)
                      |  (pAudio[cInstance]->audioFormat.lfePbLevel & 0x3); // PB 5

    TxInfoPacketSet( cInstance ,SI_INFO_FRAME_AUDIO, aAudioInfoFrame);
}

// not  BYTE cInstance
static uint8_t SmpRateToStatChanCodeConvert(audioSampleRate_t smpRate)
{
    uint8_t scCode;

    switch(smpRate)
    {
        default:
        case SI_TX_AUDIO_SMP_RATE_UNKNOWN:
            scCode = 0x1;
            break;
        case SI_TX_AUDIO_SMP_RATE_32_KHZ:
            scCode = 0x3;
            break;
        case SI_TX_AUDIO_SMP_RATE_44_KHZ:
            scCode = 0x0;
            break;
        case SI_TX_AUDIO_SMP_RATE_48_KHZ:
            scCode = 0x2;
            break;
        case SI_TX_AUDIO_SMP_RATE_88_KHZ:
            scCode = 0x8;
            break;
        case SI_TX_AUDIO_SMP_RATE_96_KHZ:
            scCode = 0xA;
            break;
        case SI_TX_AUDIO_SMP_RATE_176_KHZ:
            scCode = 0xC;
            break;
        case SI_TX_AUDIO_SMP_RATE_192_KHZ: // Mean 768 kHz for HBR Audio (4 channels by 192 kHz)
            scCode = 0xE;
            break;
    }

    return scCode;
}

// not  BYTE cInstance
static uint8_t SmpSizeToStatChanCodeConvert(audioSampleSize_t smpSize)
{
    uint8_t scCode;

    switch (smpSize)
    {
        default:
        case SI_TX_AUDIO_SMP_SIZE_UNKNOWN:
            scCode = 0x0;
            break;
        case SI_TX_AUDIO_SMP_SIZE_16_BIT:
            scCode = 0x2;
            break;
        case SI_TX_AUDIO_SMP_SIZE_20_BIT:
            scCode = 0xA;
            break;
        case SI_TX_AUDIO_SMP_SIZE_24_BIT:
            scCode = 0xB;
            break;
    }

    return scCode;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Create status channel frame and update it in the chip.
//-------------------------------------------------------------------------------------------------

static void AudioStatusChannelUpdate( BYTE cInstance)
{
    uint8_t aStatusChannel[AUDIO_STATUS_CHANNEL_LEN];

    // Set status channel data for all modes except SPDIF and DSD
    // as they provide all status information in headers
    if (!IsSpdifOrDsd( cInstance ))
    {

        // Fill up the status channel data
        aStatusChannel[CHST1] = (pAudio[cInstance]->audioFormat.encoding > SI_TX_AUDIO_ENC_PCM) ? 2 : 0;
        aStatusChannel[CHST2] = pAudio[cInstance]->audioFormat.catCode;
        aStatusChannel[CHST3] = (pAudio[cInstance]->audioFormat.srcNumber & 0xF)
                             | ((pAudio[cInstance]->audioFormat.channelCount & 0xF) << 4);
        aStatusChannel[CHST4] = SmpRateToStatChanCodeConvert(SmpRateToDownsampledSmpRate( cInstance, pAudio[cInstance]->audioFormat.smpRate))
                             | ((pAudio[cInstance]->audioFormat.clkAccuracy & 0xF) << 4);
        aStatusChannel[CHST5] = SmpSizeToStatChanCodeConvert(pAudio[cInstance]->audioFormat.smpSize)
                             | ((pAudio[cInstance]->audioFormat.origSmpRate & 0xF) << 4);

        SiiDrvTpiAudioStatusChannelSet( cInstance ,aStatusChannel);
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Verifies if audio transmission is permitted in current state of the transmitter
//!             and downstream device. Depending on the result of the check, audio transmission
//!             gets enabled or disabled, audio info frame and status channel information get
//!             updated, and hardware capture logic gets configured.
//-------------------------------------------------------------------------------------------------

static void UpdateAll( BYTE cInstance)
{
	if(IsAudioTransmissionAllowed( cInstance ))
	{
	    // Verify if Audio downsampling is necessary
	    AudioDownsamplingUpdate( cInstance );

	    // Update accompanying information
	    AudioInfoFrameUpdate( cInstance );
        AudioStatusChannelUpdate( cInstance );

        // Set audio capture parameters
		AudioChannelsMap( cInstance );

#if (SI_TX_AUDIO_I2S_IN_BUS == SI_ENABLE)
		I2sConfigSet( cInstance );
#endif

		// Set interface type and enable audio transmission
        SiiDrvTpiAudioInterfaceSet( cInstance ,pAudio[cInstance]->audioSrcMode);

		// ISRC1/2 packets could be skipped if the packets were received
		// before audio gets enabled.
		// Request receiving them again.
		if(!pAudio[cInstance]->isAudioEnabled) // if audio transition from disabled to enabled
		{
		    TxInfoPacketTransmissionEnable( cInstance ,SI_INFO_FRAME_ISRC1, true);
		    TxInfoPacketTransmissionEnable( cInstance ,SI_INFO_FRAME_ISRC2, true);
		}

		pAudio[cInstance]->isAudioEnabled = true;
	}
	else
	{
        if (pAudio[cInstance]->audioSrcMode != SI_TX_AUDIO_SRC_NONE)
        {
            DEBUG_PRINT( TX_MSG_DBG,"Unsupported Audio Format (%d)! Audio disabled.\n", pAudio[cInstance]->audioSrcMode);
        }
		// Turn audio off
        TxInfoPacketTransmissionEnable( cInstance ,SI_INFO_FRAME_ISRC1, false);
        TxInfoPacketTransmissionEnable( cInstance ,SI_INFO_FRAME_ISRC2, false);
	    	TxInfoPacketTransmissionEnable( cInstance , SI_INFO_FRAME_AUDIO, false);
		SiiDrvTpiAudioDisable( cInstance );
		pAudio[cInstance]->isAudioEnabled = false;
	}
}


//-------------------------------------------------------------------------------------------------
//  TX Audio API functions
//-------------------------------------------------------------------------------------------------
void  TxAudioStructInstanceSet( BYTE cInstance,uint8_t deviceIndex)
{
    if(deviceIndex < SII_NUM_TX)
    {
        pAudio[cInstance] = &AudioInstRec[cInstance][deviceIndex];
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Select Tx device instance in multi-transmitter system.
//!
//! @param[in]  device_idx - zero based index of an instance.
//-------------------------------------------------------------------------------------------------

#if SII_NUM_TX > 1
void TxAudioInstanceSet( BYTE cInstance, uint8_t deviceIndex)
{
    if(deviceIndex < SII_NUM_TX)
    {
        pAudio[cInstance] = &AudioInstRec[cInstance][deviceIndex];
    }
}
#endif // SII_NUM_TX > 1


//-------------------------------------------------------------------------------------------------
//! @brief      Initialization of Audio module
//-------------------------------------------------------------------------------------------------

void TxAudioInit( BYTE cInstance)
{
    memset(pAudio[cInstance], 0, sizeof(TxAudioInstanceRecord_t));
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return true if audio transmission is enabled.
//-------------------------------------------------------------------------------------------------

bool_t TxAudioIsTransmitting( BYTE cInstance)
{
	return pAudio[cInstance]->isAudioEnabled;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set audio interface configuration for one of the I2S group sources.
//!
//!             The I2S sources are either I2S, DSD or HBRA.
//!
//! @param[in]  source             - audio source type,
//! @param[in]  pInterfaceSettings - pointer to I2S settings.
//-------------------------------------------------------------------------------------------------

void TxAudioI2SConfigSet( BYTE cInstance, txAudioSrc_t source, txAudioSrcConfig_t *pInterfaceSettings)
{
    txAudioSrcGrp_t groupIndex = I2sSourceGroupGet(source);
    memcpy(&pAudio[cInstance]->audioInterface[groupIndex], pInterfaceSettings, sizeof(txAudioSrcConfig_t));
}


//-------------------------------------------------------------------------------------------------
//! @brief      Set format of the input audio stream.
//!
//! @param[in]  audioSourceMode - wired audio source type,
//! @param[in]  pFmt            - pointer to a structure defining audio parameters.
//-------------------------------------------------------------------------------------------------

void TxAudioModeSet( BYTE cInstance, txAudioSrc_t audioSourceMode, audioFormat_t *pFmt)
{
	pAudio[cInstance]->audioSrcMode = audioSourceMode;
	memcpy(&pAudio[cInstance]->audioFormat, pFmt, sizeof(audioFormat_t));

	UpdateAll( cInstance );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Mute/Unmute audio source.
//-------------------------------------------------------------------------------------------------

void TxAudioMute( BYTE cInstance,bool_t isMute)
{
    SiiDrvTpiAudioMuteSet( cInstance ,isMute);
}



//-------------------------------------------------------------------------------------------------
//! @brief      Implement reaction of Audio module on content protection status change.
//!
//! @param[in]  isAudioProtected - true, if Audio content must be protected
//-------------------------------------------------------------------------------------------------

void TxAudioOnAcpPacketUpdate( BYTE cInstance,bool_t isAudioProtected)
{
		pAudio[cInstance]->isAudioContentProtected = isAudioProtected;
		UpdateAll( cInstance );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Implement reaction of Audio module on HDMI/DMI mode transition event.
//-------------------------------------------------------------------------------------------------

void TxAudioOnHdmiModeChange( BYTE cInstance)
{
	UpdateAll( cInstance );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Update internal audio setting based on Audio InfoFrame.
//!
//! @param[in]  pAif - pointer to new AIF
//-------------------------------------------------------------------------------------------------

void TxAudioUpdateFromInfoFrame( BYTE cInstance, uint8_t *pAif)
{
    uint8_t field;

    if ((pAif[0] == 0x84) &&
        (pAif[1] == 0x01)) // Packet type and version should be correct
    {
        // PB 1
        pAudio[cInstance]->audioFormat.channelCount = (pAif[4] & 0xF0) >> 4;
        if (pAudio[cInstance]->audioFormat.channelCount > 0)
        {
            pAudio[cInstance]->audioFormat.channelCount++;
        }

        field =  pAif[4] & 0x07;
        if ((field != SI_TX_AUDIO_ENC_UNKNOWN)  ||  (pAudio[cInstance]->audioSrcMode == SI_TX_AUDIO_SRC_SPDIF))
        {
            // Change audio encoding only if it's specified explicitly for non SPDIF audio
            pAudio[cInstance]->audioFormat.encoding = (audioEncoding_t)field;
        }

        // PB 2
        field = (pAif[5] & 0x1C) >> 2;
        if ((field != SI_TX_AUDIO_SMP_RATE_UNKNOWN)  ||  (pAudio[cInstance]->audioSrcMode == SI_TX_AUDIO_SRC_SPDIF))
        {
            // Change audio sample rate only if it's specified explicitly for non SPDIF audio
            pAudio[cInstance]->audioFormat.smpRate = (audioSampleRate_t)field;
        }

        field = pAif[5] & 0x03;
        if ((field != SI_TX_AUDIO_SMP_SIZE_UNKNOWN)  ||  (pAudio[cInstance]->audioSrcMode == SI_TX_AUDIO_SRC_SPDIF))
        {
            // Change audio sample rate only if it's specified explicitly for non SPDIF audio
            pAudio[cInstance]->audioFormat.smpSize =(audioSampleRate_t) field;
        }


        // PB 4
        pAudio[cInstance]->audioFormat.spkMap = (audioSpkAlloc_t)pAif[7];

        // PB 5
        pAudio[cInstance]->audioFormat.isDwnMxInhibit = (pAif[8] & 0x80) >> 7;
        pAudio[cInstance]->audioFormat.lShift = (pAif[8] & 0x78) >> 3;
        pAudio[cInstance]->audioFormat.lShift = pAif[8] & 0x03;
    }

    UpdateAll( cInstance );
}

