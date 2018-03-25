//***************************************************************************
//! @file     sk_app_tx.c
//! @brief    Wraps board and device functions for the TX component
//            and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#define INC_AUDIO_RX    1
//#define INC_RTPI      1
#include "sk_application.h"
#include "si_tx_component.h"
#include "si_drv_tpi_system.h"
#include "si_edid_tx_component.h"
#include "si_drv_nvram_sram.h"
#if INC_CEC
#include "si_cec_component.h"
#include "sk_app_cec.h"
#endif
#if INC_AUDIO_RX
#include "si_drv_audio.h"
#endif
#include "si_drv_audio_mix.h"
#include "si_drv_switch.h"
#include "si_osal_timer.h"
#if INC_OSD
#include "si_osd_component.h"
#endif
#include "si_drv_tpi_system.h"
#include "si_tx_hdcp.h"
#include "main.h"
#include "Mcu.h"
#include "uart.h"
#include "si_drv_tpi_ddc_access.h"
//-------------------------------------------------------------------------------------------------
// Manifest constants
//-------------------------------------------------------------------------------------------------
#define BUG_1 0
// Minimal time delay (in ms) that has to have elapsed before repeated call of
// the TX Task Process function
#define SI_TX_TIME_STEP_MIN_MS       20
extern int_t SiiDrvRxPipePortGet ( BYTE cInstance, int_t pipe );
//-------------------------------------------------------------------------------------------------
//! @brief      Configure current TX audio and video mode
//!
//! @param[in]  videoSrc    - source of video for TX
//! @param[in]  videoFormat - format of the video from source
//! @param[in]  audioSrc    - source of audio for TX
//-------------------------------------------------------------------------------------------------
static void InputAudVidConfig(BYTE cInstance, txVideoSource_t videoSrc, videoFormatId_t videoFormat,
                              txAudioSrc_t audioSrc, audioSampleRate_t audioSampRate,
                              bool_t isNonHdmiAudio)
{
    txVideoBusMode_t    busMode;
    audioFormat_t       audioFormat;
    txAudioSrcConfig_t  audioI2sConfig;
    uint8_t txInstance;
    uint8_t Ri128CompValue[2][3]= {{0x0, 0x0, 0x7f},{0x0, 0x7f, 0x7f}};
    // Reset audio format descriptor
    memset(&audioFormat, 0, sizeof(audioFormat_t));
    // Get instance index
    txInstance = SiiTxInstanceGet(cInstance);
    // VIDEO --------------------------------------------------------------
    switch (videoSrc)
    {
    case SI_TX_VIDEO_SOURCE_PGEN:
        // Set input Video for TPG Source
        busMode.clockMode = SI_TX_VBUS_CLOCK_NORMAL;
        busMode.colorSpace = SI_TX_VBUS_COLOR_SPACE_RGB;
        busMode.bitsPerColor = SI_TX_VBUS_BITS_PER_COLOR_8;
        busMode.colorimetry = SI_TX_VIDEO_COLORIMETRY_ITU601;
        busMode.pixelRepetition = SI_TX_VIDEO_SRC_PIXEL_REP_1;
        busMode.isLatchOnRisingEdge = false;
        SiiTxVideoSourceConfig( cInstance ,videoSrc, &busMode);
        SiiTxInputVideoModeSet( cInstance ,videoFormat, NULL, SI_TX_VIDEO_3D_NONE, SI_TX_VIDEO_HVSYNC_DE, NULL);
#if Uart_Debug
        UART1_SendBytes("SiiTxInputVideoModeSet\r\n",strlen("SiiTxInputVideoModeSet\r\n"));
#endif
        SiiDrvTpiHdcpRi128Comp(cInstance,Ri128CompValue[txInstance][SI_TX_VIDEO_SOURCE_PGEN]);
        break;
    case SI_TX_VIDEO_SOURCE_HDMI:
    default:
        SiiTxVideoSourceConfig( cInstance, SI_TX_VIDEO_SOURCE_HDMI, NULL);
        if(!isNonHdmiAudio)
        {
            SiiTxInputAudioModeSet(cInstance,SI_TX_AUDIO_SRC_NONE, &audioFormat);
        }
        SiiDrvTpiHdcpRi128Comp(cInstance,Ri128CompValue[txInstance][SI_TX_VIDEO_SOURCE_HDMI]);
        break;
    }
    // AUDIO --------------------------------------------------------------
    if (videoSrc != SI_TX_VIDEO_SOURCE_HDMI || isNonHdmiAudio)
    {
        if(audioSrc != SI_TX_AUDIO_SRC_NONE)
        {
            switch (audioSrc)
            {
            default:
            case SI_TX_AUDIO_SRC_SPDIF:
            case SI_TX_AUDIO_SRC_I2S_L0:
                // Set Audio parameters
                audioFormat.channelCount = 2;
                audioFormat.encoding = SI_TX_AUDIO_ENC_PCM;
                audioFormat.smpRate = audioSampRate;
                audioFormat.smpSize = SI_TX_AUDIO_SMP_SIZE_24_BIT;
                audioFormat.isDwnMxInhibit = false;
                audioFormat.lShift = 0;
                audioFormat.lfePbLevel = 0;
                audioFormat.spkMap = SI_TX_AUDIO_SPK_FL_FR;
                // I2S only parameters
                audioI2sConfig.mclkMult = SI_TX_AUDIO_I2S_MCLK_MULT_256;
                audioI2sConfig.sampleEdge = SI_TX_AUDIO_I2S_SCK_EDGE_FALLING;
                audioI2sConfig.wsPolarity = SI_TX_AUDIO_I2S_WS_LOW;
                audioI2sConfig.sdJustify = SI_TX_AUDIO_I2S_SD_JUSTIF_LEFT;
                audioI2sConfig.sdDirection = SI_TX_AUDIO_I2S_SD_DIR_MSB;
                audioI2sConfig.sdFirstBitShifted = SI_TX_AUDIO_I2S_SD_BIT0_SHIFTED;
                break;
            case SI_TX_AUDIO_SRC_I2S_L1:
                // Set Audio parameters
                audioFormat.channelCount = 8;
                audioFormat.encoding = SI_TX_AUDIO_ENC_PCM;
                audioFormat.smpRate = audioSampRate;
                audioFormat.smpSize = SI_TX_AUDIO_SMP_SIZE_24_BIT;
                audioFormat.isDwnMxInhibit = false;
                audioFormat.lShift = 0;
                audioFormat.lfePbLevel = 0;
                audioFormat.spkMap = SI_TX_AUDIO_SPK_FL_FR_LFE_FC_RL_RR_RLC_RRC;
                // I2S only parameters
                audioI2sConfig.mclkMult = SI_TX_AUDIO_I2S_MCLK_MULT_256;
                audioI2sConfig.sampleEdge = SI_TX_AUDIO_I2S_SCK_EDGE_FALLING;
                audioI2sConfig.wsPolarity = SI_TX_AUDIO_I2S_WS_LOW;
                audioI2sConfig.sdJustify = SI_TX_AUDIO_I2S_SD_JUSTIF_LEFT;
                audioI2sConfig.sdDirection = SI_TX_AUDIO_I2S_SD_DIR_MSB;
                audioI2sConfig.sdFirstBitShifted = SI_TX_AUDIO_I2S_SD_BIT0_SHIFTED;
                break;
            case SI_TX_AUDIO_SRC_HBRA:
                // Set Audio parameters
                audioFormat.channelCount = 8;
                audioFormat.encoding = SI_TX_AUDIO_ENC_DTSHD;
                audioFormat.smpRate = SI_TX_AUDIO_SMP_RATE_192_KHZ;
                audioFormat.smpSize = SI_TX_AUDIO_SMP_SIZE_24_BIT;
                audioFormat.isDwnMxInhibit = false;
                audioFormat.lShift = 0;
                audioFormat.lfePbLevel = 0;
                audioFormat.spkMap = SI_TX_AUDIO_SPK_FL_FR_LFE_FC_RL_RR_RLC_RRC;
                // I2S only parameters
                audioI2sConfig.mclkMult = SI_TX_AUDIO_I2S_MCLK_MULT_128;
                audioI2sConfig.sampleEdge = SI_TX_AUDIO_I2S_SCK_EDGE_FALLING;
                audioI2sConfig.wsPolarity = SI_TX_AUDIO_I2S_WS_LOW;
                audioI2sConfig.sdJustify = SI_TX_AUDIO_I2S_SD_JUSTIF_LEFT;
                audioI2sConfig.sdDirection = SI_TX_AUDIO_I2S_SD_DIR_MSB;
                audioI2sConfig.sdFirstBitShifted = SI_TX_AUDIO_I2S_SD_BIT0_SHIFTED;
                break;
            }
            //SiiDrvTpiAudioClkDividerSet(0, (audioSrc == SI_TX_AUDIO_SRC_SPDIF));
            SiiTxAudioSourceConfig(cInstance,audioSrc, &audioI2sConfig);
            SiiTxInputAudioModeSet(cInstance,audioSrc, &audioFormat);
            SiiTxInputAudioUnmute( cInstance );
        }
    }
    else
    {
        SiiTxInputAudioModeSet( cInstance, SI_TX_AUDIO_SRC_NONE, &audioFormat);
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Configure TX instance to start sending HDCP protected
//!             (if required) A/V content.
//!
//! @param[in]  instance - TX instance
//!
//-------------------------------------------------------------------------------------------------
static void StartTx( BYTE cInstance, uint8_t instance)
{
    SiiTxOutputInterfaceSet( cInstance, SI_TX_OUTPUT_MODE_HDMI);
    if (SiiTxGetLastResult( cInstance ).lastErrorCode == SI_TX_ERROR_CODE_HDMI_NOT_SUPPORTED_BY_DS)
    {
        SiiTxOutputInterfaceSet( cInstance, SI_TX_OUTPUT_MODE_DVI);
    }
    InputAudVidConfig( cInstance, app[cInstance].currentVideoSrc[instance],
                       app[cInstance].currentVideoFormat[instance],
                       app[cInstance].currentAudioType[instance],
                       app[cInstance].audioSampleRate[instance], false);
    DEBUG_PRINT(MSG_DBG,"-----------StartTx ------------\r\n");
    SiiTxResume( cInstance );          // enable TMDS
#if INC_OSD
//    SiiDrvOsdStateUpdate();
#endif
    // Copy AIF from selected Rx source if in audio overlay mode
    if ((app[cInstance].currentAudioSource[instance] == SiiSwitchAudioSourceMainPipe) ||
            (app[cInstance].currentAudioSource[instance] == SiiSwitchAudioSourceSubPipe))
    {
        bool_t isMainPipeAudio =  (app[cInstance].currentAudioSource[instance] == SiiSwitchAudioSourceMainPipe);
        SiiDrvTpiAudioRxSource( cInstance, true, isMainPipeAudio);
        // Copy AIF to TX
        SkAppTxCopyAif( cInstance, instance, isMainPipeAudio);
        SiiDrvRxAudioInstanceSet( cInstance, isMainPipeAudio);
        SiiDrvRxAudioFifoRedirSet( cInstance, true);
        SiiTxInputAudioUnmute( cInstance );
    }
    else
    {
        SiiDrvTpiAudioRxSource( cInstance, false, true);
        SiiDrvRxAudioInstanceSet( cInstance, 0);
        SiiDrvRxAudioFifoRedirSet( cInstance, false);
        SiiDrvRxAudioInstanceSet( cInstance, 1);
        SiiDrvRxAudioFifoRedirSet( cInstance, false);
    }
    SiiTxRecoverAfterSourceClockInterruption( cInstance );
    if (!app[cInstance].isTxRepeaterMode[instance])
    {
        if (app[cInstance].isTxHdcpRequired[instance])
        {
            SiiRepeaterForceShaSet( cInstance, instance, true); // Tie SHA engine to resuming TX
            SiiTxAvMute(cInstance);
            //SiiTxAvUnmute( cInstance ); //[PTNCODE]
            DEBUG_PRINT(  MSG_DBG, "-------SiiTxHdcpEnable 555555\n ");
            SiiTxHdcpEnable( cInstance );
            SiiDrvTpiDdcBusSet(cInstance,TRUE);//[kyq ADD: 20151009 15:26:36]
        }
        else
        {
            DEBUG_PRINT( MSG_DBG,"-----SiiTxHdcpDisable---666---\n");
            SiiTxHdcpDisable( cInstance );
            SiiDrvTpiDdcBusSet(cInstance,FALSE);//[kyq ADD: 20151009 15:26:36]
            SiiTxAvUnmute( cInstance );
        }
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Configures TX instance to stop sending A/V content.
//!
//! @param[in]  instance - TX instance
//!
//-------------------------------------------------------------------------------------------------
static void StopTx( BYTE cInstance, uint8_t instance)
{
    app[cInstance].currentVideoSrc[instance] = SI_TX_VIDEO_SOURCE_HDMI;
    app[cInstance].currentVideoFormat[instance] = vm1_640x480p; // default setting, has no effect
    //app.currentAudioType[instance] = SI_TX_AUDIO_SRC_NONE;
    InputAudVidConfig( cInstance, app[cInstance].currentVideoSrc[instance],
                       app[cInstance].currentVideoFormat[instance],
                       app[cInstance].currentAudioType[instance],
                       app[cInstance].audioSampleRate[instance], false);
    DEBUG_PRINT(MSG_DBG,"-----------StopTx--------\r\n");
    if (!app[cInstance].isTxRepeaterMode[instance])
    {
        if (app[cInstance].isTxHdcpRequired[instance])
        {
            DEBUG_PRINT( MSG_DBG,"-----SiiTxHdcpDisable---777---\n");
            SiiTxHdcpDisable( cInstance );
            SiiRepeaterForceShaSet(cInstance, instance, false); // release SHA
        }
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Configures TX to start or stop sending A/V content.
//!
//! @param[in]  instance  - TX instance (0 or 1)
//! @param[in]  isEnabled - determines if to start (true) or stop (false) transmitters
//!
//-------------------------------------------------------------------------------------------------
static void EnableTransmitter( BYTE cInstance, uint8_t instance, bool_t isEnabled)
{
    SiiDrvRxAudioMixInstanceSet( cInstance, instance);
    SkAppTxInstanceSet( cInstance, instance);
    if (isEnabled)
    {
        // Disable HDCP in case previous mode left it enabled
        SiiTxHdcpDisable( cInstance );
        // Disable bypassing audio packet by audio mixer
        SiiDrvRxAudioMixPassThroughConfig( cInstance, false, false);
        if (SiiTxStatusGet( cInstance ).isSinkReady)
        {
            StartTx( cInstance, instance);
        }
    }
    else
    {
        StopTx( cInstance, instance);
        // Enable bypassing audio packet by audio mixer
        //
        //SiiDrvRxAudioMixPassThroughConfig( cInstance, true, true);  // org code
        SiiDrvRxAudioMixPassThroughConfig(cInstance,app[cInstance].bHDMIAudioEn,true);
    }
}
//-------------------------------------------------------------------------------------------------
//  TX application level API functions
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//! @brief      Set instance of Tx and dependent components (EdidTx).
//!
//!             This function must be used instead of direct call to the component Tx API
//!             instance setting function.
//!
//! @param[in]  instance - instance of the current TX
//-------------------------------------------------------------------------------------------------
void SkAppTxInstanceSet( BYTE cInstance, uint8_t instance)
{
    SiiTxInstanceSet( cInstance, instance);
    if (app[cInstance].numOfEdidTxInst == 2)
    {
        // "I" topology case.
        // In 2 instance mode of EdidTx, only single Tx per EdidTx instance is serviced
        // So that there is 1:1 relationship between Tx and EdidTx component instances
        SiiEdidTxInstanceSet( cInstance ,instance);
    }
    else
    {
        // Select EDID TX input for the case of "Y" topology
        // when one EdidTx component service both transmitters
        // Limitation: TX branches in "Y" topology shall have instance indexes [0,1,...]
        SiiEdidTxInputSet( cInstance ,instance);
    }
}
#if INC_IV
//-------------------------------------------------------------------------------------------------
//! @brief      Enable or Disable internal video pattern generator.
//-------------------------------------------------------------------------------------------------
void SkAppTpgEnable ( BYTE cInstance, bool_t isEnabled )
{
    if (isEnabled)
    {
        // Configure Audio PLLs if TPG will uses their output as clock source
        SiiDrvRxAudioInstanceSet( cInstance, 0);
        SiiDrvRxAudioPllConfig( cInstance, app[cInstance].tpgClockSrc == SI_TPG_CLK_P0);
        SiiDrvRxAudioInstanceSet( cInstance, 1);
        SiiDrvRxAudioPllConfig( cInstance, app[cInstance].tpgClockSrc == SI_TPG_CLK_P1);
        // Enable TPG (or ETPG)
        SiiDrvTpgVideoFormatSet( cInstance, app[cInstance].tpgVideoFormat, app[cInstance].tpgClockSrc);
        if (app[cInstance].isExtTpg)
        {
            SiiDrvExtTpgPatternSet( cInstance, app[cInstance].tpgVideoPattern);
            SiiDrvExtTpgEnable( cInstance, true);
        }
        else
        {
            SiiDrvTpgEnable( cInstance, true);
        }
    }
    else
    {
        // Release audio PLLs if used
        SiiDrvRxAudioInstanceSet( cInstance, 0);
        SiiDrvRxAudioPllConfig( cInstance, false);
        SiiDrvRxAudioInstanceSet( cInstance, 1);
        SiiDrvRxAudioPllConfig( cInstance, false);
        // SiiDrvTpgVideoFormatSet(app[cInstance].tpgVideoFormat, app[cInstance].tpgClockSrc);
        // Disable TPG (or ETPG)
        if (app[cInstance].isExtTpg)
        {
            SiiDrvExtTpgEnable( cInstance, false);
        }
        else
        {
            SiiDrvTpgEnable( cInstance, false);
        }
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Configure TX to show test pattern and transmit external audio.
//-------------------------------------------------------------------------------------------------
void SkAppInternalVideoConfig (  BYTE cInstance, uint_t txInstance, bool_t isTxEnabled)
{
    videoFormatId_t videoFormat;
    // Convert to proper video format for TPG video
    switch (app[cInstance].tpgVideoFormat)
    {
    default:
    case SI_TPG_FMT_VID_720_60:
        videoFormat = vm4_1280x720p;
        break;
    case SI_TPG_FMT_VID_576_50:
        videoFormat = vm17_18_720x576p;
        break;
    case SI_TPG_FMT_VID_480_60:
        videoFormat = vm2_3_720x480p;
        break;
    }
    app[cInstance].currentVideoSrc[txInstance] = SI_TX_VIDEO_SOURCE_PGEN;
    app[cInstance].currentVideoFormat[txInstance] = videoFormat;
    EnableTransmitter( cInstance, txInstance, isTxEnabled);
#if INC_OSD
    // Adjust OSD font for the resolution, if TX0 switches in TPG mode
    if (txInstance == 0)
    {
        if (isTxEnabled)
        {
            SiiDrvOsdResolutionChange( cInstance,  videoFormat, 0, 0 );
            if ( app[cInstance].isOsdMenuEnabled )
            {
                SiiOsdEnableOsdDisplay(  cInstance, true );
            }
        }
    }
#endif //#if INC_OSD
}
#endif
//-------------------------------------------------------------------------------------------------
//! @brief      Configure TX to transmit external audio (video is bypassed).
//-------------------------------------------------------------------------------------------------
void SkAppExtAudioInsertConfig( BYTE cInstance, int_t txInstance, bool_t isEnabled)
{
    uint8_t audioSampleRate;
    audioSampleRate = SiiDrvRxAudioSampleRateGet(cInstance);
    //DEBUG_PRINT(MSG_DBG, "\n\n audioSampleRate =%x\n\n",audioSampleRate);
    app[cInstance].audioSampleRate[txInstance] = (audioSampleRate_t)(audioSampleRate + 1);
    app[cInstance].currentVideoSrc[txInstance]  = SI_TX_VIDEO_SOURCE_HDMI;
    //DEBUG_PRINT(MSG_DBG, "\n\n SkAppExtAudioInsertConfig =%x %x %x %x %x\n\n", app.currentVideoSrc[txInstance],app.currentVideoFormat[txInstance],app.currentAudioType[txInstance], app.audioSampleRate[txInstance],txInstance);
    InputAudVidConfig( cInstance ,app[cInstance].currentVideoSrc[txInstance],
                       app[cInstance].currentVideoFormat[txInstance],
                       app[cInstance].currentAudioType[txInstance],
                       app[cInstance].audioSampleRate[txInstance], isEnabled);
}
//-------------------------------------------------------------------------------------------------
//! @brief      TX Edid component initialization.
//!
//! @param[in]  isMatrixMode - indicate if device is in matrix mode (two independent pipes)
//! @retval     true - if initialization was successful.
//-------------------------------------------------------------------------------------------------
bool_t SkAppDeviceInitEdidTx ( BYTE cInstance, bool_t isAudioByPass)
{
    bool_t   success = false;
    uint16_t videoFeatureMask, audioFeatureMask, miscOptionsMask;
    uint8_t  numOfTxPerEdidTxInst;
    uint8_t  instance;
    // In non-matrix mode there is only one Rx source providing content for 1 or 2 Tx.
    // Therefore, only one instance of EDID Tx component servicing up to 2 Tx is required.
    // If 2 transmitters are connected to one Rx, a merged EDID structure is created.
    // The merged EDID includes the capability declarations common for both TXs
    app[cInstance].numOfEdidTxInst = 1;
    numOfTxPerEdidTxInst = 1;
    // In case of the joint TX EDID, it's more convenient to use legacy Rx EDID mode
    // (One common EDID is shared across all ports)
    SiiDrvNvramEdidModeSet( cInstance, SII_NVRAM_LEGACY_EDID_MODE);
    DEBUG_PRINT(MSG_DBG, "EDID Tx Component has %d instances.\n", app[cInstance].numOfEdidTxInst);
    for (instance = 0; instance < app[cInstance].numOfEdidTxInst; ++instance)
    {
        SiiEdidTxInstanceSet( cInstance, instance);
        if ( SiiEdidTxInitialize( cInstance ))
        {
            videoFeatureMask =
                SI_TX_EDID_VIDEO_CFG_ALLOW_BAD_CSUM_FOR_BLOCK0          |
                SI_TX_EDID_VIDEO_CFG_ALLOW_PC_MODES_NO_SCALER           |
                SI_TX_EDID_VIDEO_CFG_BYPASS_SCALER                      |
                SI_TX_EDID_VIDEO_CFG_ENABLE_REMEDIES                    |
                SI_TX_EDID_VIDEO_CFG_ENABLE_3D                          |
                SI_TX_EDID_VIDEO_CFG_ENABLE_XVYCC                       |
                SI_TX_EDID_VIDEO_CFG_DC30_36                            |
                (app[cInstance].isEdidMhlLimits ? SI_TX_EDID_VIDEO_CFG_ENABLE_MHL_LIMITS : 0);
            audioFeatureMask = 0;
            if ( isAudioByPass )
            {
                audioFeatureMask |= SI_TX_EDID_AUDIO_CFG_BYPASS;// | SI_TX_EDID_AUDIO_CFG_SPDIF_INPUT;
            }
            else
            {
                audioFeatureMask &= ~SI_TX_EDID_AUDIO_CFG_BYPASS;
            }
            miscOptionsMask     = 0;
            if ( SiiEdidTxConfigure(  cInstance, numOfTxPerEdidTxInst, videoFeatureMask, audioFeatureMask, miscOptionsMask ))
            {
                success = true;
            }
        }
    }
    DEBUG_PRINT(MSG_ALWAYS, "MHL limits to TX EDID: %s\n", app[cInstance].isEdidMhlLimits ? "Y" : "N");
    return( success );
}
#if INC_RTPI
//-------------------------------------------------------------------------------------------------
//! @brief      Call back RTPI function to initialize EDID Tx.
//-------------------------------------------------------------------------------------------------
void SiiRtpiCbDeviceInitTx( BYTE cInstance,bool_t isMatrixmode, bool_t isAudioByPass)
{
    SkAppDeviceInitEdidTx( cInstance, isAudioByPass);
}
#endif // INC_RTPI
//-------------------------------------------------------------------------------------------------
//! @brief      TX component initialization.
//!
//! @retval     true - if initialization was successful.
//-------------------------------------------------------------------------------------------------
bool_t SkAppDeviceInitTx( BYTE cInstance)
{
    uint8_t instance;
    txRevision_t txRevision;
    //PrintAlways( "Transmitter Revision:\n");
    for (instance = 0; instance < SII_NUM_TX; instance++)
    {
        SkAppTxInstanceSet( cInstance ,instance);
        SiiTxInitialize( cInstance );
        SiiTxHdcpConfigure(cInstance,false, 10, false);
        txRevision = SiiTxRevisionGet( cInstance );
        DEBUG_PRINT(MSG_ALWAYS, "\tTX%d [COMP: %X, DEV: %X, TPI: %X]\n", instance,
                    txRevision.component, txRevision.deviceId, txRevision.tpi);
        // Configure Audio Mixer to bypass audio and other packets
        SiiDrvRxAudioMixInstanceSet( cInstance ,instance);
        //SiiDrvRxAudioMixPassThroughConfig( cInstance ,true, true); // org code
        SiiDrvRxAudioMixPassThroughConfig( cInstance , app[cInstance].bHDMIAudioEn, true);
        //SiiDrvRxDigitalAudioPwoffStatue(cInstance);//[PTNCODE]
        // GCP packets shall never be passed through
        // as AVMUTE status must be controlled by repeater (not the upstream source only)
        SiiDrvRxAudioMixGcpPassThroughConfig( cInstance ,false);
    }
    return (true);
}
//只判断输出端9533是否有信号输入
#if 0 // Ptn Add
uint8_t HaveOrNoSignal(uint8_t DeviceID)
{
    uint8_t a;
    if((DeviceID>=0)&&(DeviceID<10))
    {
        a=SiiRegRead ( 0x0006 );
        if((a&0x01)==0x00)
        {
            return false;
        }
        else if((a&0x03)==0x03)
        {
            return true;
        }
        else
        {
            return true;
        }
    }
    return true;
}
#endif
//-------------------------------------------------------------------------------------------------
//! @brief      TX main task routine.
//!
//!             TX related background tasks run here.
//-------------------------------------------------------------------------------------------------
// app[cInstance].powerState != APP_POWERSTATUS_ON
//  ---> dvSii9533Process()--->main()
void SkAppTaskTx(BYTE cInstance)
{
    uint8_t instance;
    txFeedbackMsg_t     fbMsg = SI_TX_FB_MSG_NONE;
    static clock_time_t lastTimeMs[INSTANCE_9533][SII_NUM_TX];
    clock_time_t        currentTimeMs = SiiOsTimerTotalElapsed();
    clock_time_t        timeStep;
    for (instance = 0; instance < SII_NUM_TX; instance++)
    {
        timeStep = SkTimeDiffMs(lastTimeMs[cInstance][instance], currentTimeMs);
        SkAppTxInstanceSet(cInstance, instance);
        // Call Task Processor periodically, but not too frequently
        // However, call it right away if TX interrupt is pending
        if (SiiTxStatusGet(cInstance).isIrqPending || (timeStep > SI_TX_TIME_STEP_MIN_MS))
        {
            fbMsg = SiiTxTaskProcess(cInstance, timeStep);
            // React on feedback messages
            if(fbMsg!= SI_TX_FB_MSG_NONE)
            {
                DEBUG_PRINT(MSG_ALWAYS, "Dev[%d] fbMsg = ",cInstance);
            }
            switch (fbMsg)
            {
            default:
            case SI_TX_FB_MSG_NONE:
#if INC_CEC
                if (app[cInstance].cecEnable && app[cInstance].txSendOneTouch)
                {
                    // Wake up TV and tell it we're the one!
                    // Keep trying until task queue is free
                    SkAppCecInstanceSet(cInstance,  CEC_INSTANCE_AVR);
                    if (SiiCecOneTouchPlay(cInstance))
                    {
                        app[cInstance].txSendOneTouch = false;
                    }
                    app[cInstance].txSendOneTouch = false;  // [kyq Begin: 20150906 17:18:37], some time it will be always set
                }
#endif
                break;
            case SI_TX_FB_MSG_HDCP_AUTH_DONE:
                //SiiTxAvUnmute(cInstance); // [PTNCODE]
                SkAppTxHdcpAuthDone(cInstance);
                SiiRepeaterForceShaSet(cInstance, instance, false); // release SHA
                DEBUG_PRINT(MSG_ALWAYS, "TX%d: HDCP Auth SUCCESSFUL\n", instance);
                SiiDrvTpiAvMuteSourceSet(cInstance, TRUE); //[kyq ADD for TEST: 20150828 17:19:52]
                //SiiDrvTpiAvMuteSourceSet(cInstance,FALSE); //[kyq ADD for TEST: 20150828 17:19:52]
                break;
            case SI_TX_FB_MSG_ERR_HDCP_AUTH_FAILED:
                DEBUG_PRINT(MSG_ALWAYS, "TX%d: HDCP Auth FAILED!\n", instance);
                if (!app[cInstance].isTxBypassMode[instance])
                {
                    // Mute AV if HDCP failed
                    // (unmuting will be done after successful HDCP authentication)
                    SiiTxAvMute(cInstance);
                }
                break;
            case SI_TX_FB_MSG_HOT_PLUG_RECONNECT:
#if PRD_TX_STANBY_DELY
                app[cInstance].bTXStandByDelayStart = FALSE;
#endif
                // Put special handling for short HPD disconnection here, if required.
                DEBUG_PRINT(MSG_ALWAYS, "TX%d: HPD Reconnecting\n", instance);
                app[cInstance].isTxDisconnectedImmed = false;
                if (app[cInstance].isTxBypassMode[instance])
                {
                    // Propagate HPD Toggle event to the upstream
                    SkAppTxHpdConnection(cInstance, SI_RPT_TX_HPD_TOGGLE);
                }
                break;
            case SI_TX_FB_MSG_HOT_PLUG_CONNECTED:
            case SI_TX_FB_MSG_HOT_PLUG_CONNECTED2:  // add by kyq 20151009
#if PRD_TX_STANBY_DELY
                app[cInstance].bTXStandByDelayStart = FALSE;
#endif
                DEBUG_PRINT(MSG_ALWAYS, "TX%d: HPD Connected\n", instance);
                app[cInstance].isTxDisconnectedImmed = true;
                if (app[cInstance].isTxRepeaterMode[instance])
                {
                    SkAppTxHpdConnection(cInstance, OFF);
                }
//[kyq Begin: 20150827 21:39:05]
#if BUG_1
                else   // Bug 33051 - [CP9533_FW] MHL1/2:Video color is incorrect after hotplug DS HDMI cable of 9533
                {
                    SiiDrvRepeaterSwitchRxHotPlug(OFF);
                }
#endif
//[kyq End: 20150827 21:39:09]
// Read & process DS EDID, if a new DS device has been connected
                if ((SiiTxStatusGet(cInstance).isHotPlugDetected)&& (fbMsg == SI_TX_FB_MSG_HOT_PLUG_CONNECTED))
                {
                    SkAppProcessTxEdid(cInstance, instance);
                #if INC_CEC
                    DEBUG_PRINT(MSG_ALWAYS, "Got HPD and EDID Then CEC will Enumerte Devices\n");
                    if (app[cInstance].cecEnable)
                    {
                        // Get our CEC physical address (possible new DS EDID)
                        // and allocate a (possibly) new logical address.
                        SkAppCecInstanceSet(cInstance,  CEC_INSTANCE_AVR);
                        SiiCecEnumerateDevices(cInstance,  appCecSourceLaList);
                        SiiCecEnumerateDeviceLa(cInstance,  appCecSourceLaList);
                        #if(MACHINETYPE!=PRD_VHD4&&MACHINETYPE!=MUH88E_BYE&&(MACHINETYPE!=MUH4K862)&&(MACHINETYPE!=MHUB4K431))
//  RTI VHD-4 not to send OneTouch Command kyq 20151019
                        app[cInstance].txSendOneTouch = true;
                        #endif
                        SiiCecSetDevicePA(cInstance, SiiTxPhysicalAddressGet(cInstance));
                    }
                #endif
                }
                break;
            case SI_TX_FB_MSG_HOT_PLUG_DISCONNECTED:
            #if PRD_TX_STANBY_DELY
                app[cInstance].msTXStandByDelay =  SiiOsTimerTotalElapsed();
                app[cInstance].bTXStandByDelayStart = TRUE;
            #else
// org code
                DEBUG_PRINT(MSG_ALWAYS, "TX%d: HPD Disconnected\n", instance);
                SiiTxStandby(cInstance);
                SkAppDismissTxEdid(cInstance, instance);
                if (app[cInstance].isTxRepeaterMode[instance])
                {
                    SkAppTxHpdConnection(cInstance, OFF);
                }
                #if BUG_1
                else    // Bug 33051 - [CP9533_FW] MHL1/2:Video color is incorrect after hotplug DS HDMI cable of 9533
                {
                    SiiDrvRepeaterSwitchRxHotPlug(OFF);
                }
                #endif
                #if INC_CEC
                if (app[cInstance].cecEnable && (instance == 0))
                {
                    // Set sink mode CEC
                    SkAppCecInstanceSet(cInstance, CEC_INSTANCE_AVR);
                    SiiCecSetDevicePA(cInstance,  0x0000);
                }
                #endif  //INC_CEC
            #endif
                break;
            case SI_TX_FB_MSG_SINK_READY:
#if PRD_TX_STANBY_DELY
                app[cInstance].bTXStandByDelayStart = FALSE;
#endif
                app[cInstance].isTxDisconnectedImmed = false;
                DEBUG_PRINT(MSG_ALWAYS, "TX%d: Downstream sink is ready and Tx Bypasmode is %d\n", instance, (int)app[cInstance].isTxBypassMode[instance]);
                /*if (app[DeviceID].isTxBypassMode[instance])
                {
                    StopTx(instance);
                }
                else*/
                {
                    StartTx(cInstance, instance);
                }
                if (app[cInstance].isTxRepeaterMode[instance])
                {
                    SkAppTxHpdConnection(cInstance, (rptHpdTxEvent_t)ON);
                }
#if BUG_1
                else    // Bug 33051 - [CP9533_FW] MHL1/2:Video color is incorrect after hotplug DS HDMI cable of 9533
                {
                    SiiDrvRepeaterSwitchRxHotPlug(ON);
                }
#endif
                break;
            }
            lastTimeMs[cInstance][instance] = currentTimeMs;
        }
    #if PRD_TX_STANBY_DELY
        if (app[cInstance].bTXStandByDelayStart)
        {
            //app[cInstance].msTXStandByDelay =  50;
            timeStep = SkTimeDiffMs(app[cInstance].msTXStandByDelay, currentTimeMs);
            if ((timeStep > 50)&&(!SiiDrvTpiGetHpdState(cInstance))) //[kyq add: 20151016 17:38:21]
            {
                app[cInstance].bTXStandByDelayStart = FALSE;
                DEBUG_PRINT(MSG_ALWAYS, "TX%d: HPD REALY Disconnected\n", instance);
                SiiTxStandby(cInstance);
                SkAppDismissTxEdid(cInstance, instance);
                if (app[cInstance].isTxRepeaterMode[instance])
                {
                    SkAppTxHpdConnection(cInstance, OFF);
                }
            #if BUG_1
                else    // Bug 33051 - [CP9533_FW] MHL1/2:Video color is incorrect after hotplug DS HDMI cable of 9533
                {
                    SiiDrvRepeaterSwitchRxHotPlug(OFF);
                }
            #endif
                #if INC_CEC
                if (app[cInstance].cecEnable && (instance == 0))
                {
                    // Set sink mode CEC
                    SkAppCecInstanceSet(cInstance, CEC_INSTANCE_AVR);
                    SiiCecSetDevicePA(cInstance,  0x0000);
                }
                #endif  //INC_CEC
            }
        }
    #endif
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Process EDID from newly connected transmitter.
//!
//! @param[in]  txInstance - instance of the current TX
//-------------------------------------------------------------------------------------------------
void SkAppProcessTxEdid( BYTE cInstance,uint8_t txInstance)
{
    if (app[cInstance].numOfEdidTxInst == 2)
    {
        // In 2 instance mode of EdidTx, only single Tx is serviced
        SiiEdidTxInstanceSet( cInstance , txInstance);
        SiiEdidTxProcessEdid( cInstance,0);
    }
    else
    {
        // In 1 instance mode of EdidTx, both Tx instances are serviced
        SiiEdidTxProcessEdid(cInstance,txInstance);
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Get rid of EDID from disconnected transmitter.
//!
//! @param[in]  txInstance - instance of the current TX
//-------------------------------------------------------------------------------------------------
void SkAppDismissTxEdid( BYTE cInstance, uint8_t txInstance)
{
    uint8_t rxPort;
    if (app[cInstance].numOfEdidTxInst == 2)
    {
        // Only one EDID exist in 2 pipe mode
        SiiEdidTxInstanceSet( cInstance, txInstance);
        SiiEdidTxDismissEdid( cInstance, 0);
        // The Rx EDID RAM associated with the disconnected Tx
        // must be replaced with NVRAM EDID
        SiiDrvNvramCopyToSram(  cInstance, NVRAM_HDMI_EDID, txInstance, true );
    }
    else
    {
        // Try to re-make US EDID from other TX EDID.
        // If all TXs are disconnected, this function won't be able to create a new EDID
        SiiEdidTxDismissEdid( cInstance, txInstance);
        if (SiiEdidTxGetLastResult( cInstance ) == SI_TX_EDID_ERROR_CODE_NO_US_EDID_CREATED)
        {
            // Replace DS EDID in all RX ports by the default EDID,
            // if both TXs are disconnected
            for (rxPort = 0; rxPort < SII_INPUT_PORT_COUNT; ++rxPort)
            {
                SiiDrvNvramCopyToSram(  cInstance, NVRAM_HDMI_EDID, rxPort, true );
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Copy AIF from Rx pipe to Tx.
//!
//!             This function is used in audio overlay mode, when audio stream gets extracted
//!             from main or sub pipe and inserted into Tx.
//!
//! @param[in]  txInstance - instance of the current TX
//! @param[in]  isMainPipe - source pipe
//-------------------------------------------------------------------------------------------------
void SkAppTxCopyAif( BYTE cInstance, uint8_t txInstance, bool_t isMainPipe)
{
    uint8_t pIfData[32];
    uint8_t len;
    uint8_t audioMode;
    uint8_t audioLayout;
    uint8_t audioSampleRate;
    SkAppTxInstanceSet(cInstance, txInstance);
    // Get source audio format
    SiiDrvRxAudioInstanceSet( cInstance, isMainPipe ? 0 : 1);
    audioMode = SiiDrvRxAudioModeGet( cInstance );
    audioLayout = SiiDrvRxAudioLayoutGet( cInstance );
    audioSampleRate = SiiDrvRxAudioSampleRateGet( cInstance );
    switch (audioMode)
    {
    case SII_AUD_HBRA_MODE:
        app[cInstance].currentAudioType[txInstance] = SI_TX_AUDIO_SRC_HBRA;
        break;
    case SII_AUD_DSD_MODE:
        app[cInstance].currentAudioType[txInstance] = (audioLayout == SII_AUD_MULTI_CHANNEL) ?  SI_TX_AUDIO_SRC_DSD_L1 :
                SI_TX_AUDIO_SRC_DSD_L0 ;
        break;
    default:
    case SII_AUD_PCM_MODE:
        app[cInstance].currentAudioType[txInstance] = (audioLayout == SII_AUD_MULTI_CHANNEL) ?  SI_TX_AUDIO_SRC_I2S_L1 :
                SI_TX_AUDIO_SRC_I2S_L0 ;
        break;
    }
    app[cInstance].audioSampleRate[txInstance] = (audioSampleRate_t)(audioSampleRate + 1);
    // Set up Tx to transmit audio of given format
    InputAudVidConfig(cInstance,
                      app[cInstance].currentVideoSrc[txInstance],
                      app[cInstance].currentVideoFormat[txInstance],
                      app[cInstance].currentAudioType[txInstance],
                      app[cInstance].audioSampleRate[txInstance], true);
    // Copy AIF to TX
    SiiDrvSwitchInfoFrameGet( cInstance, isMainPipe, IF_AIF, pIfData, &len);
    SiiTxInfoFrameSet( cInstance, SI_INFO_FRAME_AUDIO, pIfData);
    SiiTxInfoFrameEnable(cInstance,SI_INFO_FRAME_AUDIO);
}
//-------------------------------------------------------------------------------------------------
//! @brief      Copy Vsif from Rx pipe to Tx.
//!
//!             This function is used in audio overlay mode, when audio stream gets extracted
//!             from main or sub pipe and inserted into Tx.
//!
//! @param[in]  txInstance - instance of the current TX
//! @param[in]  isMainPipe - source pipe
//-------------------------------------------------------------------------------------------------
void SkAppTxCopyVsif( BYTE cInstance, uint8_t txInstance, bool_t isMainPipe)
{
    uint8_t pIfData[32];
    uint8_t len;
    SkAppTxInstanceSet(cInstance, txInstance);
    // Copy Vsi to TX
    SiiDrvSwitchInfoFrameGet( cInstance, isMainPipe, IF_VSI, pIfData, &len);
    SiiTxInfoFrameSet( cInstance, SI_INFO_FRAME_VSIF, pIfData);
    SiiTxInfoFrameEnable( cInstance, SI_INFO_FRAME_VSIF);
}
//-------------------------------------------------------------------------------------------------
//! @brief      Adjust TMDS analog settings (termination impedance, etc.) based on pixel clock
//!             of video coming from Rx in bypass mode.
//!
//! @param[in]  txInstance - transmitter instance (zone)
//! @param[in]  pixClock   - video TMDS clock frequency in 10kHz units
//-------------------------------------------------------------------------------------------------
void SkAppTxAdjustAnalogSettings( BYTE cInstance, int_t txInstance, int_t pixClock)
{
    SiiTxInstanceSet( cInstance, txInstance);
    switch (app[cInstance].txTmdsFreqRange[txInstance])
    {
    default:
    case 0: // AUTO (default)
        // use provided pixClock value
        break;
    case 1: // 30 MHz < 165 MHz
        pixClock = 7500;
        break;
    case 2: // < 225 MHz
        pixClock = 17000;
        break;
    case 3: // < 300 MHz
        pixClock = 23000;
        break;
    }
    SiiDrvTpiVideoModeDataSet(pixClock, 0,0,0);
}
//-------------------------------------------------------------------------------------------------
//! @brief      Adjust audio clock divider settings based on tmds clock
//!             of video coming from Rx in bypass mode.
//!
//! @param[in]  txInstance - transmitter instance (zone)
//! @param[in]  tmdsClock   - video TMDS clock frequency in 10kHz units
//-------------------------------------------------------------------------------------------------
void SkAppTxAdjustAudioClkDivider( BYTE cInstance, int_t txInstance, int_t tmdsClock)
{
    bool_t toSet;
    toSet = (app[cInstance].currentAudioType[txInstance] == SI_TX_AUDIO_SRC_SPDIF);
    SiiTxInstanceSet( cInstance, txInstance);
    //SiiDrvTpiAudioClkDividerSet(tmdsClock, toSet); // TODO
}
//-------------------------------------------------------------------------------------------------
//! @brief      Adjust video settings in HDMI bypass mode based on Rx AVI InfoFrame
//!
//! @param[in]  txInstance - transmitter instance (zone)
//! @param[in]  pAvi       - pointer to Rx AVI InfoFrame
//! @param[in]  aviLen     - length of Rx AVI InfoFrame in bytes
//-------------------------------------------------------------------------------------------------
void SkAppTxAdjustVideoSettings( BYTE cInstance, int_t txInstance, uint8_t *pAvi, uint8_t aviLen)
{
    txVideoBusMode_t    busMode;
    if (app[cInstance].isTxBypassMode[txInstance])
    {
        if (aviLen < 7)
        {
            // Default to RGB
            busMode.colorSpace = SI_TX_VBUS_COLOR_SPACE_RGB;
            busMode.quantRange = SI_TX_VIDEO_QRANGE_DEFAULT;
        }
        else
        {
            // Take from AVI
            busMode.colorSpace = (txVbusColSpace_t)((pAvi[4] & 0x60) >> 5);
            busMode.quantRange =(txVbusQuantRange_t)( (pAvi[6] & 0x0C) >> 2);
        }
        SiiTxInstanceSet(cInstance,txInstance);
        SiiTxVideoSourceConfig( cInstance, SI_TX_VIDEO_SOURCE_HDMI, &busMode);
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Adjust video settings in HDMI bypass mode based on Rx AVI InfoFrame
//!
//! @param[in]  txInstance - transmitter instance (zone)
//! @param[in]  pAvi       - pointer to Rx AVI InfoFrame
//! @param[in]  aviLen     - length of Rx AVI InfoFrame in bytes
//-------------------------------------------------------------------------------------------------
void SkAppTxInfoFrameUpdate( BYTE cInstance, bool_t isMainPipeSrc, bool_t isAvi, bool_t isAud, bool_t isVsi)
{
    int_t instance = 0;
    bool_t isForwardVsif = false;
    if (isAud)
    {
        for (instance = 0 ; instance < SII_NUM_TX; instance++)
        {
            if ((!app[cInstance].isTxBypassMode[instance]) && (app[cInstance].currentAudioSource[instance] == (isMainPipeSrc ? SiiSwitchAudioSourceMainPipe : SiiSwitchAudioSourceSubPipe)))
                // Copy AIF from selected Rx source if in audio overlay mode
            {
                SkAppTxInstanceSet(cInstance, instance);
                // Copy AIF to TX
                SkAppTxCopyAif( cInstance, instance, isMainPipeSrc);
                SiiTxRecoverAfterSourceClockInterruption( cInstance );
            }
        }
    }
    if(isVsi)
    {
        DEBUG_PRINT( MSG_ALWAYS, "VSIF new Frames ready to pass to TX\n");
        switch(SiiDrvSwitchSelectedPorttype(cInstance))
        {
        case SiiPortType_HDMI:
            if (!app[cInstance].isTxBypassMode[0])
            {
                isForwardVsif = true;
            }
            break;
        case SiiPortType_MHL:
            DEBUG_PRINT( MSG_ALWAYS, "VSIF new Frames are from MHL\n");
            isForwardVsif = true;
            break;
        default:
            break;
        }
        if ( isForwardVsif )
        {
            SkAppTxInstanceSet(cInstance, instance);
            //copy VSIF to TX
            SkAppTxCopyVsif( cInstance, instance, isMainPipeSrc);
            DEBUG_PRINT( MSG_ALWAYS, "VSIF new Frames Copied to Tx\n");
        }
    }
}
//-------------------------------------------------------------------------------------------------
// Callbacks from TX module (Source: HDCP)
//
// TX Component has repeater links that must be satisfied even if no repeater exists
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//! @brief      TX callback function. Informs App/Repeater that downstream HDCP authentication
//!             is already done. Only upstream has to be authenticated.
//-------------------------------------------------------------------------------------------------
void SiiTxCbHdcpUsOnlyAuthentication( BYTE cInstance)
{
    SkAppRepeaterHdcpUsOnlyAuthentication( cInstance );
}
//-------------------------------------------------------------------------------------------------
//! @brief      TX callback function. Informs App/Repeater if downstream HDCP is available.
//!
//! @param[in]  isAcknowledged - true:  DS HDCP responded, false: no DDC acknowledge at DS
//-------------------------------------------------------------------------------------------------
void SiiTxCbHdcpDsDdcAck( BYTE cInstance,bool_t isAcknowledged)
{
    SkAppRepeaterHdcpDsDdcAck( cInstance ,isAcknowledged);
}
//-------------------------------------------------------------------------------------------------
//! @brief      TX callback function. Send BSTATUS from TX to App/Repeater for processing.
//!
//! @param[in]  dsBstatus - 2-byte array carrying BSTATUS data
//-------------------------------------------------------------------------------------------------
bool_t SiiTxCbHdcpProcessDsBstatus( BYTE cInstance, uint8_t dsBstatus[2])
{
    return SkAppRepeaterHdcpProcessDsBstatus( cInstance ,dsBstatus);
}
//-------------------------------------------------------------------------------------------------
//! @brief      TX callback function. Send 20-bit BKSV from TX to App/Repeater in order to
//!             finalize 2nd part of repeater HDCP authentication.
//!
//! @param[in]  aDsBksv - 5-byte array carrying downstream BKSV data
//-------------------------------------------------------------------------------------------------
void SiiTxCbHdcpFinishUsPart2( BYTE cInstance, uint8_t aDsBksv[5])
{
    SkAppRepeaterHdcpFinishUsPart2( cInstance, aDsBksv);
}
//-------------------------------------------------------------------------------------------------
//! @brief      TX callback function. Forward portion of KSV list to App/Repeater in order to
//!             do black listing.
//!
//! @param[in]  pKsvBuffer - pointer to a buffer containing a portion of KSV list
//! @param[in]  len - length of the buffer (number of bytes)
//-------------------------------------------------------------------------------------------------
void SiiTxCbHdcpProcessKsvListPortion( BYTE cInstance, uint8_t *pKsvBuffer, uint8_t len)
{
}
//-------------------------------------------------------------------------------------------------
//! @brief      TX callback function. Informs App/Repeater if BCAPS FIFO is ready.
//!
//! @param[in]  isFifoReady - BCAPS FIFO ready flag.
//-------------------------------------------------------------------------------------------------
void SiiTxCbHdcpSetUsRepeaterBcaps( BYTE cInstance, bool_t isFifoReady)
{
    SkAppRepeaterHdcpSetBcapsFifoRdyBits( cInstance ,isFifoReady);
}
//-------------------------------------------------------------------------------------------------
//! @brief      TX callback function. Informs App/Repeater that TX is about to receive KSV List.
//-------------------------------------------------------------------------------------------------
bool_t SiiTxCbHdcpPrepareForKsvListProcessing( BYTE cInstance)
{
    return (SkAppRepeaterHdcpPrepareForKsvListProcessing( cInstance ));
}
//-------------------------------------------------------------------------------------------------
//! @brief      TX callback function. Informs App/Repeater that TX is about to receive KSV List.
//-------------------------------------------------------------------------------------------------
bool_t SiiTxCbHdcpPrepareFor0KsvListProcessing( BYTE cInstance)
{
    return(SkAppRepeaterHdcpPrepareFor0KsvListProcessing( cInstance ));
}
//-------------------------------------------------------------------------------------------------
//! @brief      TX callback function. Informs App/Repeater about HDCP failure.
//!
//! @param[in]  isFatal - true:  fatal HDCP issue, TX won't retry,
//!                       false: minor issue, TX will retry to authenticate
//-------------------------------------------------------------------------------------------------
void SiiTxCbHdcpInformUsDeviceAboutFailure( BYTE cInstance, bool_t isFatal)
{
    SkAppRepeaterHdcpInformUsDeviceAboutFailure( cInstance, isFatal);
}
//-------------------------------------------------------------------------------------------------
// Callback functions from Repeater module
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//! @brief      Repeater callback function. Enables Tx in repeater mode.
//!             Repeater is expected to set instance for Tx App before calling this function.
//-------------------------------------------------------------------------------------------------
void SiiRepeaterCbTxResume( BYTE cInstance)
{
    int_t txInstance = SiiTxInstanceGet( cInstance );
    if(app[cInstance].isTxRepeaterMode[txInstance])
    {
        if (app[cInstance].isTxBypassMode[txInstance])
        {
            SiiTxResume( cInstance );
            SiiTxOutputInterfaceSet( cInstance, SI_TX_OUTPUT_MODE_HDMI);
            if (SiiTxGetLastResult( cInstance ).lastErrorCode == SI_TX_ERROR_CODE_HDMI_NOT_SUPPORTED_BY_DS)
            {
                SiiTxOutputInterfaceSet( cInstance, SI_TX_OUTPUT_MODE_DVI);
            }
            //SiiDrvRxAudioMixPassThroughConfig( cInstance, true, true);
            SiiDrvRxAudioMixPassThroughConfig(cInstance,app[cInstance].bHDMIAudioEn,true);
            //SiiDrvRxDigitalAudioPwoffStatue(cInstance); //[PTNCODE]
            if(app[cInstance].newAudioSource[txInstance] == SiiSwitchAudioSourceExternal)
            {
                SiiDrvRxAudioMixPassThroughConfig( cInstance, false, true);
            }
            else
            {
                //SiiDrvRxAudioMixPassThroughConfig( cInstance, true, true); // or code
                SiiDrvRxAudioMixPassThroughConfig(cInstance,app[cInstance].bHDMIAudioEn,true);
                //SiiDrvRxDigitalAudioPwoffStatue(cInstance);//[PTNCODE]
            }
        }
        else
        {
            EnableTransmitter( cInstance, txInstance, true);
        }
        DEBUG_PRINT(MSG_DBG, "Tx App%d: Resume from RPTR. Bypass: %d\n", txInstance, app[cInstance].isTxBypassMode[txInstance]);
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Repeater callback function. Disables Tx in repeater mode
//!             Repeater is expected to set instance for Tx App before calling this function.
//-------------------------------------------------------------------------------------------------
void SiiRepeaterCbTxStandby( BYTE cInstance)
{
    int_t txInstance = SiiTxInstanceGet( cInstance );
    if(app[cInstance].isTxRepeaterMode[txInstance])
    {
        SkAppTxInstanceSet( cInstance, txInstance);
        if (app[cInstance].isTxBypassMode[txInstance])
        {
            SiiTxStandby( cInstance );
        }
        else
        {
            SiiTxStandby( cInstance );
            //EnableTransmitter(txInstance, false);
        }
        DEBUG_PRINT(MSG_DBG, "Tx App%d: Standby from RPTR. Bypass: %d\n", txInstance, app[cInstance].isTxBypassMode[txInstance]);
    }
}
//-------------------------------------------------------------------------------------------------
// Callback from EDID TX module
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//! @brief      Distribute generated EDID from TX to all input port EDID SRAMs.
//!
//!             Works in Passthrough mode only. CEC Physical Address isn't inherited from DS
//!             because DS CEC network is separated from the US CEC. The upstream will consider
//!             SiI953x as a CEC root.
//!
//! @param[in]  blockIndex          - zero based EDID block number,
//! @param[in]  pEdidBlock          - pointer to an EDID block,
//! @param[in]  cecPhysAddrLocation - shift (in bytes) of the CEC physical address first byte
//!                                   relative to the EDID Block 1 beginning.
//!
//! @return     success flag.
//-------------------------------------------------------------------------------------------------
bool_t SiiTxEdidCbBlockWrite( BYTE cInstance, uint_t blockIndex, uint8_t* const pEdidBlock, uint8_t cecPhysAddrLocation)
{
    bool_t  isSuccess = true;
    uint8_t rxTxChannel = 0;  // Index of Rx-Tx channel serviced by dedicated instance of EDID Tx component
    if (app[cInstance].isTxBypassMode[SiiTxInstanceGet( cInstance )])
    {
        rxTxChannel = SiiEdidTxInstanceGet( cInstance );
        // This function finds Physical Address location by internal parsing of CEA extension block
        isSuccess = SiiDrvNvramEdidBlockWrite ( cInstance , (SiiSramType_t)rxTxChannel, pEdidBlock, blockIndex);
        if (!isSuccess)
        {
            DEBUG_PRINT( MSG_ERR, "EDID Write Error!\n");
        }
    }
    return isSuccess;
}
