//************************************************************************************************
//! @file   si_edid_tx_fmf.c
//! @brief  EDID Feature Meet Filter
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***********************************************************************************************/


#include "si_edid_tx_component.h"
#include "si_edid_tx_internal.h"
#include "si_edid_tx_fmf.h"

#define MIN(a,b)  (((a) > (b)) ? (b) : (a))
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))

static int Median(int x, int y)
{
    return (x + (y - x)/2);
}

static int Abs(int x)
{
    return (x > 0 ? x : -x);
}

//-------------------------------------------------------------------------------------------------
//! @brief      EDID Feature Meet Filter merges current and input edidInfo structures
//!             obtained from TX EDIDs
//!             into one edidInfo structure that includes capability declarations
//!             supported by both TX devices.
//!
//! @param[in]  pEdidInfo - pointer to an edidInfo structure.
//-------------------------------------------------------------------------------------------------

void EdidTxFeatureMeetFilter( BYTE cInstance, decodedEdid_t *pEdidInfo)
{
    uint8_t i;
    uint8_t j;
    uint8_t jm;
    int_t   matchDisparity;
    bool_t  isMatchFound;

    // Merge errors
    (pEdidTx[cInstance])->edidInfo.ErrorCode |= pEdidInfo->ErrorCode;

    // Strongest constrains win
    (pEdidTx[cInstance])->edidInfo.RangeLimits.MinV_Hz = MAX((pEdidTx[cInstance])->edidInfo.RangeLimits.MinV_Hz, pEdidInfo->RangeLimits.MinV_Hz);
    (pEdidTx[cInstance])->edidInfo.RangeLimits.MaxV_Hz = MIN((pEdidTx[cInstance])->edidInfo.RangeLimits.MaxV_Hz, pEdidInfo->RangeLimits.MaxV_Hz);
    (pEdidTx[cInstance])->edidInfo.RangeLimits.MinH_kHz = MAX((pEdidTx[cInstance])->edidInfo.RangeLimits.MinH_kHz, pEdidInfo->RangeLimits.MinH_kHz);
    (pEdidTx[cInstance])->edidInfo.RangeLimits.MaxH_kHz = MIN((pEdidTx[cInstance])->edidInfo.RangeLimits.MaxH_kHz, pEdidInfo->RangeLimits.MaxH_kHz);
    (pEdidTx[cInstance])->edidInfo.RangeLimits.MaxPixClk_10MHz = MIN((pEdidTx[cInstance])->edidInfo.RangeLimits.MaxPixClk_10MHz,
                                                        pEdidInfo->RangeLimits.MaxPixClk_10MHz);

    (pEdidTx[cInstance])->edidInfo.DescriptorValid.RangeLimits &= pEdidInfo->DescriptorValid.RangeLimits;

    // Take minimal supported dimensions (except invalid 0 size)
    if ((pEdidTx[cInstance])->edidInfo.VSize4x3 != 0)
    {
        if (pEdidInfo->VSize4x3 != 0)
        {
            (pEdidTx[cInstance])->edidInfo.VSize4x3 = MIN((pEdidTx[cInstance])->edidInfo.VSize4x3, pEdidInfo->VSize4x3);
        }
    }
    else
    {
        if (pEdidInfo->VSize4x3 != 0)
        {
            (pEdidTx[cInstance])->edidInfo.VSize4x3 = pEdidInfo->VSize4x3;
        }
    }

    if ((pEdidTx[cInstance])->edidInfo.VSize16x9 != 0)
    {
        if (pEdidInfo->VSize16x9 != 0)
        {
            (pEdidTx[cInstance])->edidInfo.VSize16x9 = MIN((pEdidTx[cInstance])->edidInfo.VSize16x9, pEdidInfo->VSize16x9);
        }
    }
    else
    {
        if (pEdidInfo->VSize16x9 != 0)
        {
            (pEdidTx[cInstance])->edidInfo.VSize16x9 = pEdidInfo->VSize16x9;
        }
    }

    // Max. Horizontal Image Size
    (pEdidTx[cInstance])->edidInfo.BasicDispayParameters[1] = MIN( (pEdidTx[cInstance])->edidInfo.BasicDispayParameters[1], pEdidInfo->BasicDispayParameters[1]);

    // Max. Vertical Image Size
    (pEdidTx[cInstance])->edidInfo.BasicDispayParameters[2] = MIN( (pEdidTx[cInstance])->edidInfo.BasicDispayParameters[2], pEdidInfo->BasicDispayParameters[2]);

    // Gamma (averaged)
    (pEdidTx[cInstance])->edidInfo.BasicDispayParameters[3] = Median((pEdidTx[cInstance])->edidInfo.BasicDispayParameters[3], pEdidInfo->BasicDispayParameters[3]);

    // Leave common features
    (pEdidTx[cInstance])->edidInfo.BasicDispayParameters[4] &= pEdidInfo->BasicDispayParameters[4];

/*  These doesn't make sense to merge. Use from first device.
    Chromaticity[10];
*/
    // Find identical standard timings and leave only them
    for (i = 0; i < 16; i += 2)
    {
        // If standard timing is used
        if (!(((pEdidTx[cInstance])->edidInfo.StandardTiming[i] == 0x1) && ((pEdidTx[cInstance])->edidInfo.StandardTiming[i+1] == 0x1)))
        {
            isMatchFound = false;
           // Check if it will survive
            for (j = 0; j < 16; j += 2)
            {
                if ( (((pEdidTx[cInstance])->edidInfo.StandardTiming[i] == pEdidInfo->StandardTiming[j]) &&
                      ((pEdidTx[cInstance])->edidInfo.StandardTiming[i+1] == pEdidInfo->StandardTiming[j+1])))
                {
                    isMatchFound = true;
                    break;
                }
            }
            // and clean it, if not
            if (!isMatchFound)
            {
                (pEdidTx[cInstance])->edidInfo.StandardTiming[i] = 0x1;
                (pEdidTx[cInstance])->edidInfo.StandardTiming[i+1] = 0x1;
            }
        }
    }

    // Invalidate DTDs that don't have counterparts
    for (i = 0; i < MAX_NMB_OF_UNKNOWN_DTDS; i++)
    {
        if ((pEdidTx[cInstance])->edidInfo.DTD[i].Valid)
        {
            // Find a match first
            isMatchFound = false;
            for (j = 0; (j < MAX_NMB_OF_UNKNOWN_DTDS) && !isMatchFound; j++)
            {
                // Save preferred timing flag temporarily
                uint8_t prefTmp = pEdidInfo->DTD[j].PreferredTiming;
                // make it equal to not base the comparison criteria on the difference in this flag
                pEdidInfo->DTD[j].PreferredTiming = (pEdidTx[cInstance])->edidInfo.DTD[i].PreferredTiming;
                // Comparison is based on all fields of the dtd_t structure
                // except the preferred timing flag
                if(memcmp(&((pEdidTx[cInstance])->edidInfo.DTD[i]), &(pEdidInfo->DTD[j]), sizeof(dtd_t)) == 0)
                {
                    isMatchFound = true;
                }
                pEdidInfo->DTD[j].PreferredTiming = prefTmp; // recover the flag
            }

            if(!isMatchFound)
            {
                (pEdidTx[cInstance])->edidInfo.DTD[i].Valid = false;
            }
            else
            {   // If match is found, synchronize preferred timing flag
                (pEdidTx[cInstance])->edidInfo.DTD[i].PreferredTiming &= pEdidInfo->DTD[j-1].PreferredTiming;
            }
        }
    }

    // Invalidate SDTDs that don't have counterparts
    for (i = 0; i < MAX_NMB_OF_KNOWN_DTDS; i++)
    {
        if ((pEdidTx[cInstance])->edidInfo.SDTD[i].Valid)
        {
            // Find a match first
            isMatchFound = false;
            for (j = 0; (j < MAX_NMB_OF_KNOWN_DTDS) && !isMatchFound; j++)
            {
                if (((pEdidTx[cInstance])->edidInfo.SDTD[i].Vic == pEdidInfo->SDTD[j].Vic) &&
                    ((pEdidTx[cInstance])->edidInfo.SDTD[i].VideoIdx == pEdidInfo->SDTD[j].VideoIdx) &&
                    ((pEdidTx[cInstance])->edidInfo.SDTD[i].Ar16x9 == pEdidInfo->SDTD[j].Ar16x9))
                {
                    isMatchFound = true;
                }
            }

            if(!isMatchFound)
            {
                (pEdidTx[cInstance])->edidInfo.SDTD[i].Valid = false;
            }
            else
            {   // If match is found, synchronize flags
                (pEdidTx[cInstance])->edidInfo.SDTD[i].Native &= pEdidInfo->SDTD[j-1].Native;
                (pEdidTx[cInstance])->edidInfo.SDTD[i].PreferredTiming &= pEdidInfo->SDTD[j-1].PreferredTiming;
            }

        }
    }

    // Invalidate SADs that don't have counterparts
    for (i = 0; i < (pEdidTx[cInstance])->edidInfo.SadCount; i++)
    {
        // Find a match first
        isMatchFound = false;
        matchDisparity = 0xFFFF; // Match disparity. The lower the disparity, the better the match
        // if matchDisparity = 0, it is the best possible match, so we stop the scan
        for (j = 0, jm = 0; (j < pEdidInfo->SadCount) && (matchDisparity != 0); j++)
        {
            // Scan all SADs to find not only first but the best match (Ex.: [LPCM 2ch, LPCM 8ch] vs [LPCM 8ch, LPCM 2ch])
            if ((pEdidTx[cInstance])->edidInfo.SAD[i].Common.Coding == pEdidInfo->SAD[j].Common.Coding)
            {
                int_t newDisparity;
                // Calculate disparity to the match equal to difference in number of channels
                newDisparity = Abs((pEdidTx[cInstance])->edidInfo.SAD[i].Common.MaxChannel - pEdidInfo->SAD[j].Common.MaxChannel);
                if (newDisparity < matchDisparity)
                {
                    isMatchFound = true;
                    jm = j;
                    matchDisparity = newDisparity;
                }
            }
        }

        if(!isMatchFound)
        {
            // Invalidate SAD by setting all bytes to 0
            memset(&((pEdidTx[cInstance])->edidInfo.SAD[i]), 0, sizeof(sad_t));
        }
        else
        {
            // Set minimal supported number of channels
            (pEdidTx[cInstance])->edidInfo.SAD[i].Common.MaxChannel = MIN((pEdidTx[cInstance])->edidInfo.SAD[i].Common.MaxChannel,
                                                             pEdidInfo->SAD[jm].Common.MaxChannel);
            // Remove mutually unsupported sample rates
            (pEdidTx[cInstance])->edidInfo.SAD[i].ByteData.Byte2 &= pEdidInfo->SAD[jm].ByteData.Byte2;

            if ((pEdidTx[cInstance])->edidInfo.SAD[i].Common.Coding <= 1) // PCM
            {
                // Remove mutually unsupported bit widths
                (pEdidTx[cInstance])->edidInfo.SAD[i].ByteData.Byte3 &= pEdidInfo->SAD[jm].ByteData.Byte3;

            }
            else if ((pEdidTx[cInstance])->edidInfo.SAD[i].Common.Coding <= 8) // AC-3 -- ATRAC
            {
                // Take minimal supported MaxBitRate
                (pEdidTx[cInstance])->edidInfo.SAD[i].ByteData.Byte3 = MIN((pEdidTx[cInstance])->edidInfo.SAD[i].ByteData.Byte3,
                                                              pEdidInfo->SAD[jm].ByteData.Byte3);
            }
            else  // One Bit Audio -- WMA Pro
            {
                // Set to default
                (pEdidTx[cInstance])->edidInfo.SAD[i].ByteData.Byte3 = 0;
            }
        }

    }

    // Remove speakers not supported by all peers
    (pEdidTx[cInstance])->edidInfo.SPK.Data &= pEdidInfo->SPK.Data;

    // Formats supported by all DS devices are the only survivors
    (pEdidTx[cInstance])->edidInfo.EstablishedTiming1 &= pEdidInfo->EstablishedTiming1;
    (pEdidTx[cInstance])->edidInfo.EstablishedTiming2 &= pEdidInfo->EstablishedTiming2;
    (pEdidTx[cInstance])->edidInfo.ManufacturerReservedTiming &= pEdidInfo->ManufacturerReservedTiming;

    // Clear HDMI signature if not all devices have it
    (pEdidTx[cInstance])->edidInfo.HDMI_VSDB.HdmiMode &= pEdidInfo->HDMI_VSDB.HdmiMode;

    // Feature flags (Support_AI, DCxx, DVI_dual) set in both devices survive
    (pEdidTx[cInstance])->edidInfo.HDMI_VSDB.Extras[0] &= pEdidInfo->HDMI_VSDB.Extras[0];
    // Max TMDS clock
    (pEdidTx[cInstance])->edidInfo.HDMI_VSDB.Extras[1] = MIN(pEdidInfo->HDMI_VSDB.Extras[1], (pEdidTx[cInstance])->edidInfo.HDMI_VSDB.Extras[1]);
#if (SI_HDMI_1_4_EDID_PROCESSING == SI_ENABLE)
    // Disable support of 3D video if one of the peers doesn't support it
    (pEdidTx[cInstance])->edidInfo.HDMI_VSDB.hdmi_3d_present &= pEdidInfo->HDMI_VSDB.hdmi_3d_present;
    // Leave only mutually supported 3d Formats
    (pEdidTx[cInstance])->edidInfo.HDMI_VSDB.hdmi_3d_structure_all &= pEdidInfo->HDMI_VSDB.hdmi_3d_structure_all;
    // Disable 3D support if two devices have nothing in common
    if (!(pEdidTx[cInstance])->edidInfo.HDMI_VSDB.hdmi_3d_present || !(pEdidTx[cInstance])->edidInfo.HDMI_VSDB.hdmi_3d_structure_all)
    {
        // clean all 3D structures
        (pEdidTx[cInstance])->edidInfo.HDMI_VSDB.hdmi_3d_present = 0;
        (pEdidTx[cInstance])->edidInfo.HDMI_VSDB.hdmi_3d_structure_all = 0;
        (pEdidTx[cInstance])->edidInfo.HDMI_VSDB.image_size = 0;
    }

#endif
    // Merge remedy masks
    (pEdidTx[cInstance])->edidInfo.RemedyMask |= pEdidInfo->RemedyMask;

    // Declare support of a feature only if all DS devices support it
    (pEdidTx[cInstance])->edidInfo.SupportYCbCr &= pEdidInfo->SupportYCbCr;
    (pEdidTx[cInstance])->edidInfo.SupportAudio &= pEdidInfo->SupportAudio;
    (pEdidTx[cInstance])->edidInfo.SupportUnderscan &= pEdidInfo->SupportUnderscan;
    (pEdidTx[cInstance])->edidInfo.SupportPal &= pEdidInfo->SupportPal;
    (pEdidTx[cInstance])->edidInfo.SupportNtsc &= pEdidInfo->SupportNtsc;
    (pEdidTx[cInstance])->edidInfo.SupportAspect4_3 &= pEdidInfo->SupportAspect4_3;
    (pEdidTx[cInstance])->edidInfo.SupportAspect16_9 &= pEdidInfo->SupportAspect16_9;
    (pEdidTx[cInstance])->edidInfo.xv601 &= pEdidInfo->xv601;
    (pEdidTx[cInstance])->edidInfo.xv709 &= pEdidInfo->xv709;

    // Switch to DVI mode if at least one channel must switch
    (pEdidTx[cInstance])->edidInfo.isHdmiMode &= pEdidInfo->isHdmiMode;

}
