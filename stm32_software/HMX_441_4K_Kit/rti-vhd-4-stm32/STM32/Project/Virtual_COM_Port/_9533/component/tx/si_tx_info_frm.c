//************************************************************************************************
//! @file   si_tx_info_frm.c
//! @brief  Info frame and HDMI packet control
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//************************************************************************************************/


#include "si_tx_component_internal.h"
#include "si_tx_info_frm.h"
#include "si_drv_tpi_system.h"
#include "si_tx_audio.h"
#include "si_edid_tx_internal.h"


//-------------------------------------------------------------------------------------------------
// Module Instance Data
//-------------------------------------------------------------------------------------------------

txInfoRecord_t InfoInstRec[INSTANCE_9533][SII_NUM_TX];
txInfoRecord_t *pInfoFrm[INSTANCE_9533] = 
{
	&InfoInstRec[0][0],
#if (INSTANCE_9533>1)
       &InfoInstRec[1][0],
#endif
#if (INSTANCE_9533>2)
       &InfoInstRec[2][0],
#endif
#if (INSTANCE_9533>3)
       &InfoInstRec[3][0],
#endif
#if (INSTANCE_9533>4)
       &InfoInstRec[4][0],
#endif
#if (INSTANCE_9533>5)
       &InfoInstRec[5][0],
#endif

#if (INSTANCE_9533>6)
       &InfoInstRec[6][0],
#endif

#if (INSTANCE_9533>7)
       &InfoInstRec[7][0],
#endif

};

//-------------------------------------------------------------------------------------------------
//  Local service functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Check if an InfoFrame has been loaded into chip.
//!
//! @param[in]  ifType - Info Frame type.
//-------------------------------------------------------------------------------------------------

static bool_t IsInfoFrameLoaded( BYTE cInstance, infoFrameType_t ifType)
{
    switch (ifType)
    {
        case SI_INFO_FRAME_AVI:
            return pInfoFrm[cInstance]->isLoadedAvi;
        case SI_INFO_FRAME_AUDIO:
            return pInfoFrm[cInstance]->isLoadedAudio;
        case SI_INFO_FRAME_SPD:
            return pInfoFrm[cInstance]->isLoadedSpd;
        case SI_INFO_FRAME_MPEG:
            return pInfoFrm[cInstance]->isLoadedMpeg;
        case SI_INFO_FRAME_ISRC1:
            return pInfoFrm[cInstance]->isLoadedIsrc1;
        case SI_INFO_FRAME_ISRC2:
            return pInfoFrm[cInstance]->isLoadedIsrc2;
        case SI_INFO_FRAME_ACP:
            return pInfoFrm[cInstance]->isLoadedAcp;
        case SI_INFO_FRAME_GBD:
            return pInfoFrm[cInstance]->isLoadedGbd;
        case SI_INFO_FRAME_VSIF:
            return pInfoFrm[cInstance]->isLoadedVsif;
    }

    return false;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Set the InfoFrame loaded flag.
//!
//! @param[in]  ifType - Info Frame type.
//-------------------------------------------------------------------------------------------------

static void InfoFrameLoadedFlagSet( BYTE cInstance, infoFrameType_t ifType, bool_t isLoaded)
{
    switch (ifType)
    {
        case SI_INFO_FRAME_AVI:
            pInfoFrm[cInstance]->isLoadedAvi = isLoaded;
            break;
        case SI_INFO_FRAME_AUDIO:
            pInfoFrm[cInstance]->isLoadedAudio = isLoaded;
            break;
        case SI_INFO_FRAME_SPD:
            pInfoFrm[cInstance]->isLoadedSpd = isLoaded;
            break;
        case SI_INFO_FRAME_MPEG:
            pInfoFrm[cInstance]->isLoadedMpeg = isLoaded;
            break;
        case SI_INFO_FRAME_ISRC1:
            pInfoFrm[cInstance]->isLoadedIsrc1 = isLoaded;
            break;
        case SI_INFO_FRAME_ISRC2:
            pInfoFrm[cInstance]->isLoadedIsrc2 = isLoaded;
            break;
        case SI_INFO_FRAME_ACP:
            pInfoFrm[cInstance]->isLoadedAcp = isLoaded;
            break;
        case SI_INFO_FRAME_GBD:
            pInfoFrm[cInstance]->isLoadedGbd = isLoaded;
            break;
        case SI_INFO_FRAME_VSIF:
            pInfoFrm[cInstance]->isLoadedVsif = isLoaded;
            break;
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Identify Info Frames that require checksum to be sent as a part of a packet.
//!
//! @param[in]  ifType - Info Frame type.
//!
//! @return     indication of whether or not the checksum is required.
//-------------------------------------------------------------------------------------------------
// Not  BYTE cInstance
static bool_t IsChecksumRequired(  infoFrameType_t ifType)
{
    bool_t hasChecksum;

    switch (ifType)
    {
        case SI_INFO_FRAME_AVI:
        case SI_INFO_FRAME_SPD:
        case SI_INFO_FRAME_AUDIO:
        case SI_INFO_FRAME_VSIF:
            hasChecksum = true;
            break;
        default:
            hasChecksum = false;
    }
    return hasChecksum;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Calculate checksum of a packet fields.
//!
//! @param[in]  pFrame  - pointer to an InfoFrame packet.
//! @param[in]  version - InfoFrame version as per HDMI specification.
//! @param[in]  length  - number of bytes in the InfoFrame body (excluding header).
//!
//! @return     checksum byte.
//-------------------------------------------------------------------------------------------------
// NOT  BYTE cInstance
static uint8_t CalculateChecksum(uint8_t *pFrame, uint8_t version, uint8_t length)
{
    uint8_t i;
    uint8_t checksum = pFrame[IF_TITLE_INDEX] + version + length;

    for (i = 0; i < length; i++)
    {
        checksum += pFrame[IF_HEADER_LENGTH + i];
    }

    return (0 - checksum);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return true, if audio is being transmitted to a device ensuring
//!             audio content protection.
//-------------------------------------------------------------------------------------------------

static bool_t IsProtectedAudioTransmitting( BYTE cInstance)
{
    return (SiiEdidTxDoesDsSupport( cInstance , SI_EDID_TX_SUPPORT_AI) && TxAudioIsTransmitting(cInstance));
}


//-------------------------------------------------------------------------------------------------
//! @brief      Create SPD InfoFrame locally based on the data from NVRAM.
//-------------------------------------------------------------------------------------------------
// not  BYTE cInstance
static void ComposeDefaultSpdInfoFrame(uint8_t *pFrame)
{
    pFrame[IF_TITLE_INDEX] = SI_INFO_FRAME_SPD;
    pFrame[IF_VERSION_INDEX] = 1;
    pFrame[IF_LENGTH_INDEX] = IF_MIN_SPD_LENGTH;
    pFrame[IF_CHECKSUM_INDEX] = 0; // to be replaced later

    // TODO:OEM - Fill up vendor specific source product description info frame
    //pFrame[IF_DATA_INDEX] = 0;
    //pFrame[IF_DATA_INDEX + 1] = 0;
    // ...

    DEBUG_PRINT2(TX_MSG_DBG, "TxInfo: SPD Created\n");
}


//-------------------------------------------------------------------------------------------------
//  TX Info Frame manipulation functions
//-------------------------------------------------------------------------------------------------
void TxInfoStructInstanceSet( BYTE cInstance, uint8_t deviceIndex)
{
    if(deviceIndex < SII_NUM_TX)
    {
        pInfoFrm[cInstance] = &InfoInstRec[cInstance][deviceIndex];
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Select Tx device instance in multi-transmitter system.
//!
//! @param[in]  deviceIndex - zero based index of an instance.
//-------------------------------------------------------------------------------------------------

#if SII_NUM_TX > 1
void TxInfoInstanceSet( BYTE cInstance, uint8_t deviceIndex)
{
    if(deviceIndex < SII_NUM_TX)
    {
        pInfoFrm[cInstance] = &InfoInstRec[cInstance][deviceIndex];
    }
}
#endif // SII_NUM_TX > 1


//-------------------------------------------------------------------------------------------------
//! @brief      Initialization of Info Frame module.
//-------------------------------------------------------------------------------------------------

void TxInfoInit( BYTE cInstance)
{
    memset(pInfoFrm[cInstance], 0, sizeof(txInfoRecord_t));
    // Make sure all info packets aren't being sent
    TxInfoAllPacketsStop( cInstance);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Enable/Disable transmission of Info frames that must be already loaded to the chip.
//!
//! @param[in]  ifType    - InfoFrame type,
//! @param[in]  isEnabled - true, if the InfoFrame has to be enabled,
//!                         false, otherwise.
//-------------------------------------------------------------------------------------------------

void TxInfoPacketTransmissionEnable( BYTE cInstance, infoFrameType_t ifType, bool_t isEnabled)
{
    // Enabling requires that frame has been loaded,
    // disabling does not.
    if (!isEnabled || IsInfoFrameLoaded( cInstance ,ifType))
    {
        SiiDrvTpiInfoFrameEnable( cInstance , isEnabled, false, ifType);
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Terminate sending all InfoFrames/Packets.
//-------------------------------------------------------------------------------------------------

void TxInfoAllPacketsStop( BYTE cInstance)
{
    SiiDrvTpiInfoFrameEnable( cInstance ,false, true, (infoFrameType_t)0); // disable all
}


//-------------------------------------------------------------------------------------------------
//! @brief      Load InfoFrame/Packet data.The frame transmission state remains intact
//!             (enabled or disabled).
//!
//!             Exception is the AVI InfoFrame that is enabled all the time.
//!             For Info Frame types, pFrame should be a pointer to
//!             a structure starting with 4 byte header.
//!
//! @param[in]  ifType - InfoFrame type,
//! @param[in]  pFrame - pointer to the InfoFrame data.
//!
//! @retval     true   - loaded successfully,
//! @retval     false  - failed to load the frame.
//-------------------------------------------------------------------------------------------------

bool_t TxInfoPacketSet( BYTE cInstance,infoFrameType_t ifType, uint8_t *pFrame)
{
    bool_t  isSuccess = true;
    uint8_t checksum;
    uint8_t frameLength;
    uint8_t aSpdFrameBuf[IF_HEADER_LENGTH + IF_MIN_SPD_LENGTH];

    // Only re-enable locked IFs and quit
    if (TxIsInfoFrameLocked( cInstance ,ifType))
    {
        SiiDrvTpiInfoFrameEnable( cInstance ,true, false, ifType);
        return true;
    }

    // Check pointer validity and try to create default IF,
    // when the pointer is NULL, if possible
    if (!pFrame)
    {
        if (ifType == SI_INFO_FRAME_SPD)
        {
            // (pFrame == NULL) means a request of using default SPD packet
            ComposeDefaultSpdInfoFrame(aSpdFrameBuf);
            pFrame = aSpdFrameBuf;
        }
        else
        {
            return false; // failure to provide valid pointer
        }
    }

    // Verify if downstream device supports the InfoFrame,
    // do specific actions for a particular type of InfoFrames
    switch (ifType)
    {
        case SI_INFO_FRAME_AVI:
            // AVI is a specially treated case
            checksum = CalculateChecksum(pFrame, 2, IF_MIN_AVI_LENGTH);
            SiiDrvTpiAviFrameStart(cInstance,&pFrame[IF_DATA_INDEX], IF_MIN_AVI_LENGTH, checksum);
            pInfoFrm[cInstance]->isLoadedAvi = true;
            pFrame = NULL; // avoid further processing
            isSuccess = true;
            break;

        case SI_INFO_FRAME_ACP:
            if (!IsProtectedAudioTransmitting( cInstance ))
            {
                pFrame = NULL;
                isSuccess = false;
            }
            break;

        case SI_INFO_FRAME_GBD:
            if (!SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_XVYCC_P0))
            {
                pFrame = NULL;
                isSuccess = false;
            }
            break;

        case SI_INFO_FRAME_VSIF:
            if (!SiiEdidTxDoesDsSupport( cInstance ,SI_EDID_TX_SUPPORT_3D))
            {
                pFrame = NULL;
                isSuccess = false;
            }
            break;

        case SI_INFO_FRAME_ISRC1:
        case SI_INFO_FRAME_ISRC2:
            if (!IsProtectedAudioTransmitting( cInstance ))
            {
                // Disable transmission as ISRC frames shall be stopped
                // if Audio protection is not supported by DS device
                SiiDrvTpiInfoFrameEnable( cInstance ,false, false, ifType);
            }
            break;
        default:
            break;
    }

    // Load InfoFrame to the chip
    if (pFrame)
    {
        if (IsChecksumRequired(ifType))
        {
            // checksum field is present in the packet
            frameLength = pFrame[IF_LENGTH_INDEX];
            checksum = CalculateChecksum(pFrame, pFrame[IF_VERSION_INDEX], frameLength);
            pFrame[IF_CHECKSUM_INDEX] = checksum;
            SiiDrvTpiInfoFrameLoad(cInstance,ifType, pFrame, frameLength, true);
        }
        else
        {
            // If checksum is not used, assume that provided frame has
            // complete set of bytes (including stuffing bytes) up to the maximum size
            frameLength = IF_MAX_LENGTH;
            SiiDrvTpiInfoFrameLoad(cInstance,ifType, pFrame, frameLength, false);
        }

        InfoFrameLoadedFlagSet(cInstance,ifType, true); // Mark this InfoFrame as loaded
    }

    return isSuccess;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Mute AV content display on sink device.
//!
//! @param[in]  isMuted - true, if AV shall be muted.
//-------------------------------------------------------------------------------------------------

void TxInfoAvMute( BYTE cInstance, bool_t isMuted)
{
    SiiDrvTpiAvMuteSendingEnable(cInstance, isMuted);
    pInfoFrm[cInstance]->isAvMuted = isMuted;
    DEBUG_PRINT(TX_MSG_DBG, "AV %s\n", isMuted ? "MUTED" : "UNMUTED");
}


//-------------------------------------------------------------------------------------------------
//! @brief      Update all loaded InfoFrames as they could have been filled
//!             while the chip was in power down.
//!
//!             AVI packet is not updated by this function;
//!             AVI update occurs in TxAviInfoFrameUpdate().
//!             Updating of the audio buffer is not needed as it is done
//!             in TxAudioOnHdmiModeChange().
//-------------------------------------------------------------------------------------------------

void TxAllInfoFramesUpdate( BYTE cInstance)
{
    uint8_t i = 0;
    uint8_t framesToUpdate[] =
    {
            SI_INFO_FRAME_AVI,
            SI_INFO_FRAME_SPD,
            SI_INFO_FRAME_AUDIO,
            SI_INFO_FRAME_MPEG,
            SI_INFO_FRAME_ISRC1,
            SI_INFO_FRAME_ISRC2,
            SI_INFO_FRAME_ACP,
            SI_INFO_FRAME_GBD,
            SI_INFO_FRAME_VSIF,
            0                       // marker of list end
    };

    while (framesToUpdate[i])
    {
        if (IsInfoFrameLoaded( cInstance , (infoFrameType_t)(framesToUpdate[i]))) // skip unloaded frames
        {
            SiiDrvTpiInfoFrameEnable( cInstance , true, false, (infoFrameType_t)framesToUpdate[i]);
        }
        i++;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Update AVI info frame in the chip to apply any changes made in AVI structure.
//-------------------------------------------------------------------------------------------------

void TxAviInfoFrameUpdate( BYTE cInstance)
{
    SiiDrvTpiAviFrameUpdate( cInstance );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Check if an InfoFrame has been locked (no implicit modification allowed)
//!
//! @param[in]  ifType - Info Frame type.
//-------------------------------------------------------------------------------------------------

bool_t TxIsInfoFrameLocked( BYTE cInstance, infoFrameType_t ifType)
{
    switch (ifType)
    {
        case SI_INFO_FRAME_AVI:
            return pInfoFrm[cInstance]->isLockedAvi;
        case SI_INFO_FRAME_AUDIO:
            return pInfoFrm[cInstance]->isLockedAudio;
        case SI_INFO_FRAME_SPD:
            return pInfoFrm[cInstance]->isLockedSpd;
        case SI_INFO_FRAME_MPEG:
            return pInfoFrm[cInstance]->isLockedMpeg;
        case SI_INFO_FRAME_ISRC1:
            return pInfoFrm[cInstance]->isLockedIsrc1;
        case SI_INFO_FRAME_ISRC2:
            return pInfoFrm[cInstance]->isLockedIsrc2;
        case SI_INFO_FRAME_ACP:
            return pInfoFrm[cInstance]->isLockedAcp;
        case SI_INFO_FRAME_GBD:
            return pInfoFrm[cInstance]->isLockedGbd;
        case SI_INFO_FRAME_VSIF:
            return pInfoFrm[cInstance]->isLockedVsif;
    }

    return false;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Set the InfoFrame locking flag.
//!
//! @param[in]  ifType - Info Frame type.
//-------------------------------------------------------------------------------------------------

void TxInfoFrameLockSet( BYTE cInstance, infoFrameType_t ifType, bool_t isLocked)
{
    switch (ifType)
    {
        case SI_INFO_FRAME_AVI:
            pInfoFrm[cInstance]->isLockedAvi = isLocked;
            break;
        case SI_INFO_FRAME_AUDIO:
            pInfoFrm[cInstance]->isLockedAudio = isLocked;
            break;
        case SI_INFO_FRAME_SPD:
            pInfoFrm[cInstance]->isLockedSpd = isLocked;
            break;
        case SI_INFO_FRAME_MPEG:
            pInfoFrm[cInstance]->isLockedMpeg = isLocked;
            break;
        case SI_INFO_FRAME_ISRC1:
            pInfoFrm[cInstance]->isLockedIsrc1 = isLocked;
            break;
        case SI_INFO_FRAME_ISRC2:
            pInfoFrm[cInstance]->isLockedIsrc2 = isLocked;
            break;
        case SI_INFO_FRAME_ACP:
            pInfoFrm[cInstance]->isLockedAcp = isLocked;
            break;
        case SI_INFO_FRAME_GBD:
            pInfoFrm[cInstance]->isLockedGbd = isLocked;
            break;
        case SI_INFO_FRAME_VSIF:
            pInfoFrm[cInstance]->isLockedVsif = isLocked;
            break;
    }
}
