//************************************************************************************************
//! @file   si_edid_tx_analyze.h
//! @brief  Downstream EDID analysis
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***********************************************************************************************/

#include <string.h>
#include "si_edid_tx_component.h"
#include "si_edid_tx_internal.h"

#include "Mcu.h"
#include "uart.h"

//-------------------------------------------------------------------------------------------------
//! @brief      Store EDID based data which is to be used during video signal processing.
//!
//-------------------------------------------------------------------------------------------------

void EdidTxAnalyze( BYTE cInstance)
{
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.HdmiMode =     (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.HdmiMode;
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportAudio = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SupportAudio;
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportYCbCr = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SupportYCbCr;
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.RemedyMask = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.RemedyMask;
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportNtsc = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SupportNtsc;
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPal = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SupportPal;
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.Support16x9 = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SupportAspect16_9;
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.Support4x3 = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SupportAspect4_3;
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.Extra0 = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[0];
#if (SI_HDMI_1_4_EDID_PROCESSING == SI_ENABLE)
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.Support3D = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.hdmi_3d_present;
    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.hdmi_3d_structure_all = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.hdmi_3d_structure_all;

#endif // SI_HDMI_1_4_EDID_PROCESSING
    memcpy(&(pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SDTD, (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SDTD, sizeof((pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SDTD));
    {
        uint8_t NumOfSads = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SadCount;
        int i;
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportEncodedAudio = false;
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportDtd = false;
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportHbra = false;
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmMcAudio = false;
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmFs88k = false;
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmFs96k = false;
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmFs176k = false;
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmFs192k = false;

        for (i = 0; i < NumOfSads; i++)
        {
            uint8_t coding = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SAD[i].Common.Coding;
            switch (coding)
            {
                case AUDIO_CODING__LPCM:
                    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmMcAudio = ((pEdidTx[cInstance])->pEdidDecodData->edidInfo.SAD[i].Common.MaxChannel > 2);
                    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmFs88k = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SAD[i].Common.Fs88k;
                    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmFs96k = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SAD[i].Common.Fs96k;
                    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmFs176k = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SAD[i].Common.Fs176k;
                    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportPcmFs192k = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.SAD[i].Common.Fs192k;

                    break;
                case AUDIO_CODING__1BIT:
                    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportDtd = true;
                    break;
                case AUDIO_CODING__DDP: // Dolby Digital +
                case AUDIO_CODING__DTSHD: // DTS-HD
                case AUDIO_CODING__MLP: // Dolby True HD (MAT/MLP)
                    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportHbra = true;
                default:
                    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SupportEncodedAudio = true;
                    break;
            }
        }
    }

    if ((pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[2] & 0x80)
    {
        // if DS latency fields are found

        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.latency[0] = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[3];
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.latency[1] = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[4];

        if ((pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[2] & 0x40)
        {
            // if DS interlaced latency fields are found
            (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.latency[2] = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[5];
            (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.latency[3] = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[6];
        }
        else
        {
            // if DS interlaced latency fields are NOT found
            (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.latency[2] = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[3];
            (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.latency[3] = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[4];
        }
    }
    else
    {
        // Latency fields were not found at DS EDID
        memset((pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.latency, 0, sizeof((pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.latency));
    }

    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.MaxTmdsClk = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.HDMI_VSDB.Extras[1];

    (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.xvYCC_p0 = false;
    if ((pEdidTx[cInstance])->pEdidDecodData->edidInfo.SupportYCbCr)
    {
        (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.xvYCC_p0 = (pEdidTx[cInstance])->pEdidDecodData->edidInfo.xv601 || (pEdidTx[cInstance])->pEdidDecodData->edidInfo.xv709;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Check if the given feature is supported by the given downstream.
//!
//! @param[in]  pEdidAnalysis - pointer to analysis results of a downstream EDID.
//! @param[in]  feature - EDID feature to check.
//!
//! @return     true, if supported, false - otherwise.
//-------------------------------------------------------------------------------------------------
// not  BYTE cInstance
static bool_t DoesDsSupports(edidAnalysis_t *pEdidAnalysis, edidFeature_t feature)
{
    bool_t isSupported = false;

    switch (feature)
    {
        case SI_EDID_TX_SUPPORT_HDMI:
            isSupported = pEdidAnalysis->HdmiMode;
			#if Uart_Debug
			UART1_SendBytes("HDMI MODE TEST\r\n",strlen("HDMI MODE TEST\r\n"));
			UART1_SendByte(isSupported);
			UART1_SendBytes("\r\n",strlen("\r\n"));
			#endif
            break;
        case SI_EDID_TX_SUPPORT_AI:
            //ret = pEdidAnalysis->SupportAi;
            isSupported = (0 != (pEdidAnalysis->Extra0 & 0x80));
            break;
        case SI_EDID_TX_SUPPORT_AUDIO:
            isSupported = pEdidAnalysis->SupportAudio;
            break;
        case SI_EDID_TX_SUPPORT_YCbCr:
            isSupported = pEdidAnalysis->SupportYCbCr;
            break;
        case SI_EDID_TX_SUPPORT_NTSC:
            isSupported = pEdidAnalysis->SupportNtsc;
            break;
        case SI_EDID_TX_SUPPORT_PAL:
            isSupported = pEdidAnalysis->SupportPal;
            break;
        case SI_EDID_TX_SUPPORT_ENCODED_AUDIO:
            isSupported = pEdidAnalysis->SupportEncodedAudio;
            break;
        case SI_EDID_TX_SUPPORT_AR_16x9:
            isSupported = pEdidAnalysis->Support16x9;
            break;
        case SI_EDID_TX_SUPPORT_AR_4x3:
            isSupported = pEdidAnalysis->Support4x3;
            break;
        case SI_EDID_TX_SUPPORT_DEEP_COLOR_30_BIT:
            isSupported = (0 != (pEdidAnalysis->Extra0 & 0x10));
            break;
        case SI_EDID_TX_SUPPORT_DEEP_COLOR_36_BIT:
            isSupported = (0 != (pEdidAnalysis->Extra0 & 0x20));
            break;
        case SI_EDID_TX_SUPPORT_DEEP_COLOR_48_BIT:
            isSupported = (0 != (pEdidAnalysis->Extra0 & 0x40));
            break;
        case SI_EDID_TX_SUPPORT_DEEP_COLOR_YCbCr_444:
            isSupported = (0 != (pEdidAnalysis->Extra0 & 0x08));
            break;
        case SI_EDID_TX_SUPPORT_1BIT_AUDIO:
            isSupported = pEdidAnalysis->SupportDtd;
            break;
        case SI_EDID_TX_SUPPORT_HBR_AUDIO:
            isSupported = pEdidAnalysis->SupportHbra;
            break;
        case SI_EDID_TX_SUPPORT_MC_PCM_AUDIO:
            isSupported = pEdidAnalysis->SupportPcmMcAudio;
            break;
        case SI_EDID_TX_SUPPORT_PCM_FS_88_KHZ:
            isSupported = pEdidAnalysis->SupportPcmFs88k;
            break;
        case SI_EDID_TX_SUPPORT_PCM_FS_96_KHZ:
            isSupported = pEdidAnalysis->SupportPcmFs96k;
            break;
        case SI_EDID_TX_SUPPORT_PCM_FS_176_KHZ:
            isSupported = pEdidAnalysis->SupportPcmFs176k;
            break;
        case SI_EDID_TX_SUPPORT_PCM_FS_192_KHZ:
            isSupported = pEdidAnalysis->SupportPcmFs192k;
            break;
        case SI_EDID_TX_SUPPORT_XVYCC_P0:
            isSupported = pEdidAnalysis->xvYCC_p0;
            break;
        case SI_EDID_TX_SUPPORT_3D:
            isSupported = pEdidAnalysis->Support3D;
            //ret = true; // uncomment for debugging only
            break;
    }

    return isSupported;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Check if the given feature is supported by the downstream.
//!
//! @param[in]  feature - EDID feature to check.
//!
//! @return     true, if supported, false - otherwise.
//-------------------------------------------------------------------------------------------------

bool_t SiiEdidTxDoesDsSupport( BYTE cInstance, edidFeature_t feature)
{
    return (DoesDsSupports(&((pEdidTx[cInstance])->pEdidDecodData->edidAnalysis), feature));
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return current DS device remedy mask.
//!
//! @return     @see TX_EDID_REMEDY_MASKS
//-------------------------------------------------------------------------------------------------
// ���Ȱ취
uint16_t SiiEdidTxDsDeviceRemedyMaskGet( BYTE cInstance)
{
    return (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.RemedyMask;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return maximum TMDS clock rate supported in MHz.
//-------------------------------------------------------------------------------------------------

uint16_t SiiEdidTxDsMaxTmdsClkGet( BYTE cInstance)
{
    return (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.MaxTmdsClk * 5;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Determine how good the given output mode for a DS device is.
//!
//!             This function is used to determine whether specific video mode is supported by
//!             DS device or not. It can be used for the repeater menu to dim non-supported modes.
//!             The function could be used in conjunction with SiiEdidTxScalerIsOutVideoModeSupported
//!             to determine whether or not dim given resolution in a repeater output mode menu.
//!
//! @param[in]  idx       - video mode ID,
//! @param[in]  aspRatio  - aspect ratio.
//!
//! @return     The bigger value is returned, the better the video mode fits for the DS device.
//! @retval     0  - if the mode is not supported,
//! @retval     1  - the mode is available,
//! @retval     2  - the mode is native, but not preferred
//! @retval     3  - preferred mode
//-------------------------------------------------------------------------------------------------

uint8_t SiiEdidTxOutVideoModeRankGet( BYTE cInstance, uint8_t idx, aspectRatio_t aspRatio)
{
    uint8_t rate = 0; // 0 means not supported mode
    int i;
    bool_t ar16x9 = (SI_EDID_TX_ASPECT_RATIO_UNKNOWN_16x9 == aspRatio);
    bool_t ar_unknown = (SI_EDID_TX_ASPECT_RATIO_UNKNOWN == aspRatio);
    for (i=0; i<MAX_NMB_OF_KNOWN_DTDS; i++)
    {
        if ((pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SDTD[i].Valid)
        {
            if ( (ar_unknown || (ar16x9 == (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SDTD[i].Ar16x9)) &&
                (idx == (pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SDTD[i].VideoIdx) )
            {
                if ((pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SDTD[i].PreferredTiming)
                    rate = 3;
                else if ((pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SDTD[i].Native)
                    rate = 2;
                else
                    rate = 1;
            }
        }
    }
    return rate;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return vertical and horizontal metrics of the video image,
//!             according to the EDID data.
//!
//! @param[in]  isAspectRatio16x9 - 16x9 aspect ratio flag,
//! @param[out] pVert             - pointer to vertical size variable (in mm),
//! @param[out] pHoriz            - pointer to horizontal size variable (in mm)
//-------------------------------------------------------------------------------------------------
// NOT BYTE cInstance,
void EdidTxImageSizeGet(decodedEdid_t *pEdidInfo, bool_t isAspectRatio16x9, uint16_t *pVert, uint16_t *pHoriz)
{
    uint16_t v, h;

    // get V size and calculate H size
    v = isAspectRatio16x9 ? pEdidInfo->VSize16x9 : pEdidInfo->VSize4x3;

    if (0 == v) // if the size for given aspect ratio is not saved, try the size for another aspect ratio
    {
        v = isAspectRatio16x9 ? pEdidInfo->VSize4x3 : pEdidInfo->VSize16x9;
    }

    if (0 == v) // if the size is still unknown, make it
    {
        v = DEFAULT__V_SIZE; //90; // because it is easy divisible by 9
    }

    h = v * (isAspectRatio16x9 ? 16 : 12) / 9;

    *pVert  = v;
    *pHoriz = h;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return a copy of short DTDs from the EDID analysis data set.
//!
//! @param[out] pSdtds - pointer to an array to carry MAX_NMB_OF_KNOWN_DTDS of SDTDs.
//-------------------------------------------------------------------------------------------------

void EdidTxDsSdtdsGet( BYTE cInstance, shortDtd_t pSdtds[])
{
    memcpy(pSdtds, &(pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SDTD, sizeof((pEdidTx[cInstance])->pEdidDecodData->edidAnalysis.SDTD));
}

