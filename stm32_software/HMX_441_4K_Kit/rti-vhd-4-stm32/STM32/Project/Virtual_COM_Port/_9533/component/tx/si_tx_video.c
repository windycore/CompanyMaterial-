//************************************************************************************************
//! @file   si_tx_video.c
//! @brief  Video Conversion Control
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
#include "si_drv_tpi_system.h"
#include "si_tx_video.h"
#include "si_tx_avi.h"
#include "si_tx_info_frm.h"
#include "si_tx_hdcp.h"
#include "si_edid_tx_component.h"

#if (SI_TX_VIDEO_CONVERSION == SI_ENABLE)
//-------------------------------------------------------------------------------------------------
// Module Instance Data
//-------------------------------------------------------------------------------------------------

txVideoConversionInstanceRecord_t VideoConversionInstRec[INSTANCE_9533][SII_NUM_TX];
txVideoConversionInstanceRecord_t *pVideo[INSTANCE_9533]= 
{
    &VideoConversionInstRec[0][0],
#if(INSTANCE_9533 >1)
    &VideoConversionInstRec[1][0],
#endif
#if(INSTANCE_9533 >2)
    &VideoConversionInstRec[2][0];
#endif

#if(INSTANCE_9533 >3)
    &VideoConversionInstRec[3][0];
#endif

#if(INSTANCE_9533 >4)
    &VideoConversionInstRec[4][0];
#endif

#if(INSTANCE_9533 >5)
    &VideoConversionInstRec[5][0];
#endif

#if(INSTANCE_9533 >6)
    &VideoConversionInstRec[6][0];
#endif
#if(INSTANCE_9533 >7)
    &VideoConversionInstRec[7][0];
#endif
};


//-------------------------------------------------------------------------------------------------
//  Local service functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Figure out the best color depth to provide on HDMI output based on the input video
//!             bus mode and video format.
//!
//! @param[in]  vbusColorDepth   - color bit depth of input video,
//! @param[in]  videoFormatIndex - input video format ID.
//!
//! @return     Color depth of output video after conversion.
//-------------------------------------------------------------------------------------------------

static txVbusColDepth_t DetermineHdmiOutputColorDepth( BYTE cInstance, txVbusColDepth_t vbusColorDepth, uint8_t videoFormatIndex)
{
    txVbusColDepth_t outputColorDepth = SI_TX_VBUS_BITS_PER_COLOR_8;
    uint8_t outputPixelFreqMHz;
    bool_t doesDownstreamSupportColorDepth[SI_TX_VBUS_BITS_PER_COLOR_LENGTH] =
    { 1, }; // 8bit color is always supported
    uint16_t dsMaxTmdsClockMHz;

    // Determine pixel clock frequency of output video
    switch (videoFormatIndex)
    {
        case SI_VIDEO_MODE_NON_STD:
            // Suppose PC mode has no repetition and no MUX formats.
            outputPixelFreqMHz = TxPixelFreqMhzGet( cInstance );
            break;
        default:
            outputPixelFreqMHz = VideoModeTable[videoFormatIndex].PixFreq;
    }

    // check Deep Color restrictions
    doesDownstreamSupportColorDepth[SI_TX_VBUS_BITS_PER_COLOR_16] = SiiEdidTxDoesDsSupport( cInstance, SI_EDID_TX_SUPPORT_DEEP_COLOR_48_BIT);
    doesDownstreamSupportColorDepth[SI_TX_VBUS_BITS_PER_COLOR_12] = SiiEdidTxDoesDsSupport( cInstance, SI_EDID_TX_SUPPORT_DEEP_COLOR_36_BIT);
    doesDownstreamSupportColorDepth[SI_TX_VBUS_BITS_PER_COLOR_10] = SiiEdidTxDoesDsSupport( cInstance, SI_EDID_TX_SUPPORT_DEEP_COLOR_30_BIT);

    dsMaxTmdsClockMHz = SiiEdidTxDsMaxTmdsClkGet(cInstance);

    if (dsMaxTmdsClockMHz == 0)
    {
        dsMaxTmdsClockMHz = 165; // 165 MHz by default
    }

    if (outputPixelFreqMHz > 0)
    {
        // Set the desired output color depth to the maximum allowed by the DS TMDS clock
        if (outputPixelFreqMHz * 2 <= dsMaxTmdsClockMHz)
        {
            // TMDS_CLK = PIX_CLK * 2
            outputColorDepth = SI_TX_VBUS_BITS_PER_COLOR_16;
        }
        else if (outputPixelFreqMHz * 3 <= dsMaxTmdsClockMHz * 2)
        {
            // TMDS_CLK = PIX_CLK * 1.5
            outputColorDepth = SI_TX_VBUS_BITS_PER_COLOR_12;
        }
        else if (outputPixelFreqMHz * 5 <= dsMaxTmdsClockMHz * 4)
        {
            // TMDS_CLK = PIX_CLK * 1.25
            outputColorDepth = SI_TX_VBUS_BITS_PER_COLOR_10;
        }

        // Limit output color depth by the input color depth
        if (outputColorDepth > vbusColorDepth)
        {
            outputColorDepth = vbusColorDepth;
        }

        // Find the highest supported bit depth that is equal or lower the input depth
        while (!doesDownstreamSupportColorDepth[outputColorDepth])
        {
            // Gradually reduce the bit depth to find the first supported one.
            // The cycle will break in the worst case, when outputColorDepth = 0
            outputColorDepth--;
        }
    }

    return outputColorDepth;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Determine if input video range is limited and has to be expanded as required
//!             by the downstream device.
//!
//! @param[in]  isPcVideoFormat - true, if the video format belongs to PC group
//!
//! @retval     true  - expansion is needed
//! @retval     false - expansion is not required
//-------------------------------------------------------------------------------------------------

static bool_t IsVideoRangeExpansionNeeded( BYTE cInstance, bool_t isPcVideoFormat)
{
    bool_t isExpansionNeeded = false;

    if (isPcVideoFormat)
    {
        isExpansionNeeded = true;
    }
    else if (!SiiEdidTxDoesDsSupport( cInstance, SI_EDID_TX_SUPPORT_HDMI)) // for DS DVI devices
    {
        // If TX input is YCbCr (which is always in the Limited Range)
        // and output is DVI (which is always RGB)
        // and the DS device is known as expecting RGB Full Range
        // then enable Limited->Full range expansion.
        if (SiiEdidTxDsDeviceRemedyMaskGet(cInstance) & SI_TX_EDID_REMEDY_DVI_PC_RANGE)
        {
            isExpansionNeeded = true;
        }
    }

    return isExpansionNeeded;
}




//-------------------------------------------------------------------------------------------------
//  TX Video Path API functions
//-------------------------------------------------------------------------------------------------
void TxVideoConversionStructInstanceSet( BYTE cInstance, uint8_t deviceIndex)
{
    if(deviceIndex < SII_NUM_TX)
    {
        pVideo[cInstance] = &VideoConversionInstRec[cInstance][deviceIndex];
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Select Tx device instance in multi-transmitter system.
//!
//! @param[in]  device_idx - zero based index of an instance.
//-------------------------------------------------------------------------------------------------

#if SII_NUM_TX > 1
void TxVideoConversionInstanceSet( BYTE cInstance,uint8_t deviceIndex)
{
    if(deviceIndex < SII_NUM_TX)
    {
        pVideo[cInstance] = &VideoConversionInstRec[cInstance][deviceIndex];
    }
}
#endif // SII_NUM_TX > 1


//-------------------------------------------------------------------------------------------------
//! @brief      Reset Deep Color mode.
//-------------------------------------------------------------------------------------------------

void TxVideoConversionReset( BYTE cInstance)
{
    if(pVideo[cInstance]->isDeepColorMode)
    {
        pVideo[cInstance]->isDeepColorMode = false;
        DEBUG_PRINT(TX_MSG_DBG, "DC Off\n");
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Initialization of video conversion module.
//-------------------------------------------------------------------------------------------------

void TxVideoConversionInit( BYTE cInstance)
{
    assert_param(pVideo[cInstance] != NULL);
    pVideo[cInstance]->isDeepColorMode = false;
    pVideo[cInstance]->outColorSpace = SI_TX_VBUS_COLOR_SPACE_RGB;

    pVideo[cInstance]->prevColorDepth = SI_TX_VBUS_BITS_PER_COLOR_8;
    pVideo[cInstance]->isDcResetNeeded = false;
    pVideo[cInstance]->isDitheringEnabled = false;
    pVideo[cInstance]->isPrevColorSpaceYCbCr = false;
    pVideo[cInstance]->isForceOutColorSpace = false;

    TxVideoBlankingLevelsSet( cInstance ,false, true);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Enable LSB data dithering mode in order to smooth color gradients.
//-------------------------------------------------------------------------------------------------

void TxVideoConversionDitheringEnable( BYTE cInstance,bool_t isEnabled)
{
    pVideo[cInstance]->isDitheringEnabled = isEnabled;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set explicit output color space for video conversion.
//-------------------------------------------------------------------------------------------------

void TxVideoConversionOutColorSpaceSet( BYTE cInstance,txVbusColSpace_t colorSpace)
{
    pVideo[cInstance]->outColorSpace = colorSpace;
    pVideo[cInstance]->isForceOutColorSpace = true;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set video conversion path parameters.
//!
//!             Note: this function may modify TX AVI information.
//!             The function will assert the pVideo[cInstance]->isDeepColorMode flag if switching to
//!             deep color mode is required. The flag will remain set even after switching back
//!             to 8-bit color mode. As required by HDMI specification, if used once, the deep color
//!             mode shall be maintained permanently.
//!
//! @param[in]  isTxOutInHdmiMode - indicates HDMI or DVI mode,
//! @param[in]  pBusMode          - input video bus mode,
//! @param[in]  isUpdateAvi       - true, if this function shall update AVI frame data before
//!                                 returning. This flag should be set to true except the case
//!                                 when TxAviPostAviPacket() function call follows this function
//!                                 right away. In latter case, setting the flag to false prevents
//!                                 duplication of the AVI update operation (harmless anyway).
//-------------------------------------------------------------------------------------------------

void TxVideoConversionSet( BYTE cInstance, bool_t isTxOutInHdmiMode, txVideoBusMode_t *pBusMode, bool_t isUpdateAvi)
{
    txVbusColSpace_t outputColorSpace = SI_TX_VBUS_COLOR_SPACE_RGB;// RGB by default (for compatibility with DVI)
    txVbusColDepth_t outputColorDepth = SI_TX_VBUS_BITS_PER_COLOR_8;
    txDataRangeConversion_t dataRangeConversion = SI_TX_DATA_RANGE_NO_CHANGE;
    bool_t isInputDataRangeFull = true;
    bool_t isOutputDataRangeFull = true;
    bool_t isPcFormat;
    bool_t isOutputColorSpaceYCbCr = true;
    uint8_t videoFormatIndex = TxVideoFormatIdGet(cInstance);
    uint8_t pixelRepetitionFactor;

    DEBUG_PRINT( TX_MSG_DBG,"TX: TxVideoConversionSet()\n");

    // Determine if video format is PC related
    switch (videoFormatIndex)
    {
        case SI_VIDEO_MODE_NON_STD:
        case vm1_640x480p:
            isPcFormat = true;
            isInputDataRangeFull = true; // PC mode is in full range
            break;
        default:
            isPcFormat = false;
    }

    // Determine output color space and bit depth
    if (isTxOutInHdmiMode)
    {
        // Typically, the output color space is equal to the input one.
        // The only exception is when AVI info frame is set explicitly
        if(!pVideo[cInstance]->isForceOutColorSpace)
        {
            outputColorSpace = TxAviColorSpaceGet( cInstance );
        }
        else
        {
            outputColorSpace = pVideo[cInstance]->outColorSpace;
        }

        outputColorDepth = DetermineHdmiOutputColorDepth( cInstance ,pBusMode->bitsPerColor, videoFormatIndex);

        // Deep Color mode entering condition, never turn DC off except by the TxVideoConversionReset()
        if (outputColorDepth > SI_TX_VBUS_BITS_PER_COLOR_8)
        {
            /*
             HDMI 1.3 p93:
             "Once a Source sends a GCP with non-zero CD to a sink, it shall continue sending GCPs with
             non-zero CD at least once per video field even if reverting to 24-bit color, as long as the Sink
             continues to support that color depth."
             */
            pVideo[cInstance]->isDeepColorMode = true;
        }

        // Adjust output color space to RGB in Deep Color mode
        if (pVideo[cInstance]->isDeepColorMode)
        {
            switch (outputColorSpace)
            {
                case SI_TX_VBUS_COLOR_SPACE_RGB:
                case SI_TX_VBUS_COLOR_SPACE_YCBCR422:
                    // YCbCr422 is not supported in deep color
                    outputColorSpace = SI_TX_VBUS_COLOR_SPACE_RGB;
                    break;
                case SI_TX_VBUS_COLOR_SPACE_YCBCR444:
                    if (!SiiEdidTxDoesDsSupport( cInstance, SI_EDID_TX_SUPPORT_DEEP_COLOR_YCbCr_444))
                        outputColorSpace = SI_TX_VBUS_COLOR_SPACE_RGB;
                    break;
            }
        }

#if (SI_TX_ALLOW_YCBCR422_12BIT == SI_ENABLE)
        if((pBusMode->bitsPerColor > SI_TX_VBUS_BITS_PER_COLOR_8) && !(pVideo[cInstance]->isDeepColorMode))
        {
            // If TX input is in Deep Color, but TX cannot send it in deep color,
            // it can transcode it into YCbCr422 (if DS supports it)
            // and send it in analog of 36bit YCbCr.
            if(SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_YCbCr))
            {
                outputColorSpace = SI_TX_VBUS_COLOR_SPACE_YCBCR422;
                // This color space could carry up to 12 bit per channel
            }
        }
#endif // SI_TX_ALLOW_YCBCR422_12BIT
    }

    TxAviColorSpaceSet( cInstance ,outputColorSpace);

    isOutputColorSpaceYCbCr = (outputColorSpace != SI_TX_VBUS_COLOR_SPACE_RGB);

    // Determine if video range expansion or contraction is needed
    switch (pBusMode->colorSpace)
    {
        default: // RGB
            if (isPcFormat && isOutputColorSpaceYCbCr)
            {
                // Full to Limited range compression
                isInputDataRangeFull = true;
                dataRangeConversion = SI_TX_DATA_RANGE_CONTRACT;
            }
            break;

        case SI_TX_VBUS_COLOR_SPACE_YCBCR444:
        case SI_TX_VBUS_COLOR_SPACE_YCBCR422:
            if (!isOutputColorSpaceYCbCr)
            {
                if (IsVideoRangeExpansionNeeded(cInstance,isPcFormat))
                {
                    isInputDataRangeFull = false;
                    dataRangeConversion = SI_TX_DATA_RANGE_EXPAND;
                }
            }
            break;
    }

    SiiDrvTpiInputVideoColorSet( cInstance ,pBusMode->colorSpace, pBusMode->bitsPerColor,
                           (dataRangeConversion == SI_TX_DATA_RANGE_EXPAND));

    SiiDrvTpiOutputVideoColorSet( cInstance ,outputColorSpace, outputColorDepth, TxAviColorimetryGet(cInstance),
                            (dataRangeConversion == SI_TX_DATA_RANGE_CONTRACT),
                            (pBusMode->bitsPerColor > outputColorDepth) && (pVideo[cInstance]->isDitheringEnabled));

    // MUX input requires doubled clock rate
    // Demultiplexing must be applied, if video is muxed YCbCr422 and thus arrives at double clock rate
    SiiDrvTpiYcDemuxEnable( cInstance ,(pBusMode->colorSpace == SI_TX_VBUS_COLOR_SPACE_YCBCR422) &&
                     (pBusMode->clockMode == SI_TX_VBUS_CLOCK_DOUBLE_RATE));

    // Set input video pixel capturing parameters
    pixelRepetitionFactor = TxAviRepetitionGet( cInstance ,videoFormatIndex);

    // TPI chip specific for YCbCr422 MUX mode
    if ((pBusMode->colorSpace == SI_TX_VBUS_COLOR_SPACE_YCBCR422) &&
        (pBusMode->clockMode == SI_TX_VBUS_CLOCK_DOUBLE_RATE))
    {
        pixelRepetitionFactor /= 2;
    }

    SiiDrvTpiPixelCapturingConfig( cInstance ,pBusMode->clockMode, pBusMode->isLatchOnRisingEdge, pixelRepetitionFactor,
                                           (pBusMode->pixelRepetition > SI_TX_VIDEO_SRC_PIXEL_REP_1));

    if (pVideo[cInstance]->prevColorDepth != outputColorDepth)
    {
        TxHdcpRequestReauthentication( cInstance );
        pVideo[cInstance]->prevColorDepth = outputColorDepth;

        DEBUG_PRINT(TX_MSG_DBG, "Color Depth: %s, %s Color Mode\n ",
                (outputColorDepth == SI_TX_VBUS_BITS_PER_COLOR_16) ? "16" :
                (outputColorDepth == SI_TX_VBUS_BITS_PER_COLOR_12) ? "12" :
                (outputColorDepth == SI_TX_VBUS_BITS_PER_COLOR_10) ? "10" : "8",
                (pVideo[cInstance]->isDeepColorMode) ? "Deep" : "Std");
    }

    SiiDrvTpiDeepColorGcpEnable( cInstance ,pVideo[cInstance]->isDeepColorMode);


    // Find out if blanking registers have to be set for full or limited range.
    if (pVideo[cInstance]->isPrevColorSpaceYCbCr != isOutputColorSpaceYCbCr)
    {
        switch (dataRangeConversion)
        {
            default:
            case SI_TX_DATA_RANGE_NO_CHANGE:
                isOutputDataRangeFull = isInputDataRangeFull;
                break;
            case SI_TX_DATA_RANGE_EXPAND:
                isOutputDataRangeFull = true;
                break;
            case SI_TX_DATA_RANGE_CONTRACT:
                isOutputDataRangeFull = false;
                break;
        }

        TxVideoBlankingLevelsSet( cInstance ,isOutputColorSpaceYCbCr, isOutputDataRangeFull);
        pVideo[cInstance]->isPrevColorSpaceYCbCr = isOutputColorSpaceYCbCr;
    }

    // Update AVI frame in the chip to expose new output color space information
    if (isUpdateAvi)
    {
        TxAviInfoFrameUpdate( cInstance );
    }

}

//-------------------------------------------------------------------------------------------------
//! @brief      Set blanking levels in 3 color channels [R,G,B] or [Y,Cb,Cr]
//!             depending on the video color space and color data range.
//!
//! @param[in]  isYCbCr     - true, if color space is YCbCr,
//!                           false, if color space is RGB
//! @param[in]  isFullRange - true, if full data range [0-255] is used for color data coding,
//!                           false, if limited rage [16-235] is used  for color data coding.
//-------------------------------------------------------------------------------------------------

void TxVideoBlankingLevelsSet( BYTE cInstance,bool_t isYCbCr, bool_t isFullRange)
{
    uint8_t blanking[3];

    if(isFullRange)
    {
        blanking[1] = 0;
    }
    else
    {
        blanking[1] = 16;
    }
    if(isYCbCr)
    {
        blanking[0] = blanking[2] = 128;
    }
    else
    {
        blanking[0] = blanking[2] = blanking[1];
    }

    SiiDrvTpiBlankingLevelsSet( cInstance ,blanking);
}

#endif

