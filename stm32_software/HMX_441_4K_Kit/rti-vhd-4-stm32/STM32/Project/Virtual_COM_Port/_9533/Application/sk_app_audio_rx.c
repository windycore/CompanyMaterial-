//***************************************************************************
//!file     sk_app_audio_rx.c
//!brief    Wraps platform and device functions for the AUDIO_RX component
//!         and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "string.h"
#include "sk_application.h"

#include "si_audio_component.h"
#include "si_audio_dac.h"
#include "si_drv_gpio.h"
#include "si_idt_5v9885t.h"
#include "si_gpio.h"
#if INC_CEC_SAC
#include "si_sac_main.h"
#endif
#if INC_CEC_SWITCH
#include "si_cec_switch_api.h"
#endif

#include "cea931_keys.h"

//#define INC_RTPI   1
#if INC_RTPI
#include "si_rtpi_component.h"
#include "si_i2c.h"
#endif

#include "si_cra.h"
#include "Mcu.h"
#include "uart.h"

SiiRxAudioInstanceData_t RxAudStatus[DevNum];
SiiRxAudioInstanceData_t RxAudState[DevNum][2];


enum
{
    SII_MATRIX_MUX_DISABLE,
    SII_MATRIX_MUX_ENABLE,
};


//------------------------------------------------------------------------------
// Function:    SkAppRxAudioConfigLayout
// Description:
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------
static void RxAudioConfigLayout( BYTE cInstance, uint8_t audLayout)
{
    if (audLayout == SII_AUD_MULTI_CHANNEL)
    {
        SiiDrvGpioPinType( cInstance, SII_GPIO_PIN_1, SII_GPIO_ALT_SD1);
        SiiDrvGpioPinType( cInstance, SII_GPIO_PIN_2, SII_GPIO_ALT_SD2);
        SiiDrvGpioPinType( cInstance, SII_GPIO_PIN_3, SII_GPIO_ALT_SD3);
    }
    else
    {
        SiiDrvGpioPinType( cInstance, SII_GPIO_PIN_1, SII_GPIO_STANDARD);
        SiiDrvGpioPinType( cInstance, SII_GPIO_PIN_2, SII_GPIO_STANDARD);
        SiiDrvGpioPinType( cInstance, SII_GPIO_PIN_3, SII_GPIO_STANDARD);
    }
}

//------------------------------------------------------------------------------
// Function:    SkAppRxAudioMuteSet
// Description:
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------
static void RxAudioMuteSet( BYTE cInstance, uint8_t chn, bool_t mute )
{
    if (mute) //mute
    {
        //SiiPlatformAudioDacControl(chn, SII_AUD_POWER_DOWN);
        RxAudioConfigLayout( cInstance, SII_AUD_TWO_CHANNEL);
    }
    // If SAC system is unmuted and we're not just passing through the audio,
    // then allow the unmute to occur
    else if ( !app[cInstance].isAudioByPass
#if INC_CEC_SAC
              && !SiiCecSacIsMuted( cInstance )
#endif
            )
    {
        switch (RxAudState[cInstance][chn].audMode)
        {
        case SII_AUD_HBRA_MODE:
            //DEBUG_PRINT( MSG_DBG, "HBRA\n");
            //SiiPlatformAudioDacControl(chn, SII_AUD_POWER_DOWN);
            RxAudioConfigLayout( cInstance, SII_AUD_MULTI_CHANNEL);
            break;
        case SII_AUD_DSD_MODE:
            //DEBUG_PRINT( MSG_DBG, "DSD\n");
            //SiiPlatformAudioDacControl(chn, SII_AUD_DSD_MODE);
            RxAudioConfigLayout( cInstance, SII_AUD_MULTI_CHANNEL);
            break;
        case SII_AUD_PCM_MODE:
            //DEBUG_PRINT( MSG_DBG, "PCM\n");
            //Program the external PLL for generate the MCLK.
            if (chn == SII_AUD_RX_SUB)
            {
                //DEBUG_PRINT( MSG_ALWAYS, "Generate the MCLK for subpipe: %d\n", RxAudState[chn].audFs);
                SiiPlatformExtnalAudioPllSetting( RxAudState[cInstance][chn].audMclk, RxAudState[cInstance][chn].audFs);
            }
            //SiiPlatformAudioDacControl(chn, SII_AUD_PCM_MODE);
            if (RxAudState[cInstance][chn].audLayout)
            {
                //DEBUG_PRINT( MSG_DBG, "SII_AUD_MULTI_CHANNEL\n");
                RxAudioConfigLayout( cInstance, SII_AUD_MULTI_CHANNEL);
            }
            else
            {
                //DEBUG_PRINT( MSG_DBG, "SII_AUD_TWO_CHANNEL\n");
                RxAudioConfigLayout( cInstance, SII_AUD_TWO_CHANNEL);
            }
            break;
        default:
            break;
        }
#if INC_RTPI
        {
            uint8_t val = 0;
            val |= RxAudState[cInstance][chn].audLayout & RP_M__RX_AUDIO_FORMAT__LAYOUT1;
            switch (RxAudState[cInstance][chn].audMode)
            {
            case SII_AUD_HBRA_MODE:
                val |= RP_M__RX_AUDIO_FORMAT__HBR;
                break;
            case SII_AUD_DSD_MODE:
                val |= RP_M__RX_AUDIO_FORMAT__DSD;
                break;
            default:
                break;
            }
            /*these two must be there for audio to be ready to unmute*/
            val |= RP_M__RX_AUDIO_FORMAT__CHST_RDY | RP_M__RX_AUDIO_FORMAT__AUD_IF_RDY;
            SiiRtpiOnAudioStatusChange( cInstance, val);
            SiiRtpiOnAudioInfoFrameChange( cInstance );
        }
#endif
    }
}

//------------------------------------------------------------------------------
// Function:    SkAppDeviceInitAudio
// Description:  Init the audio extraction for main channel only
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

bool_t SkAppDeviceInitAudio (  BYTE  cInstance  )
{
    uint8_t chn;
    //Enable audio extraction
    SiiPlatformGpioAudioInOut(true);
    SiiDrvGpioPinType( cInstance, SII_GPIO_PIN_7, SII_GPIO_ALT_I2S_WS0_OUT);
    RxAudioConfigLayout( cInstance, SII_AUD_TWO_CHANNEL);
    for ( chn = 0; chn < SII_NUM_RX_AUDIO; chn++ )
    {
        SiiRxAudioInstanceSet( cInstance, chn);
        SiiRxAudioInit( cInstance );
        // Enable SPDIF in MP and disable it in SP (in favor of I2S stereo output)
        SiiDrvRxSpdifOutEnable( cInstance, chn == 0);
    }
    //Only enable main pipe int mask
    SiiDrvRxAudioInstanceSet( cInstance, SII_AUD_RX_MAIN);
    SiiDrvRxAudioIntMaskEnable( cInstance, 1);
    return true;
}

//------------------------------------------------------------------------------
#if (INC_CEC_SAC == 1)
//-------------------------------------------------------------------------------------------------
//! @brief      CEC Callback processing messages from CEC/SAC sub-system.
//!
//! @param[in]  fbMsg - message code
//!
//! @retval     true  - if the message was recognized and processed
//! @retval     false - if the message is irrelevant and wasn't processed
//-------------------------------------------------------------------------------------------------
bool_t SiiCbCecSacMessageHandler(uint8_t fbMsg)
{
    bool_t   usedMessage = true;
    /* Process feedback messages.       */
    switch (fbMsg)
    {
    default:
    case CEC_SAC_FB_MSG_NONE:
        // nothing to do
        usedMessage = false;
        break;
    case CEC_SAC_FB_MSG_SAC_INITIATE_DONE:
        DEBUG_PRINT(MSG_STAT, "SAC Initiation task finished\n");
        break;
    case CEC_SAC_FB_MSG_SAC_FMT_DISC_DONE:
        DEBUG_PRINT(MSG_STAT, "SAC Audio Format Discovery task finished\n");
        break;
    case CEC_SAC_FB_MSG_MUTE:
    case CEC_SAC_FB_MSG_UNMUTE:
    case CEC_SAC_FB_MSG_SAC_STATUS_CHANGED:
        // For us, this means that audio is either muted or unmuted
        RxAudioMuteSet( 0, SiiCecSacIsMuted());         // Unmute speakers
        break;
    case CEC_SAC_FB_MSG_SAC_VOLUME_CHANGED:
        DEBUG_PRINT(MSG_STAT, "SAC volume has changed\n");
#if INC_OSD
        SkAppOsdVolumeUpdate( SiiCecSacVolumeGet() );
#endif
        break;
    case CEC_SAC_FB_MSG_ERR_LA_NOT_ASSIGNED:
        DEBUG_PRINT(MSG_STAT, "Audio System Logical Address not assigned\n");
        SiiCecSacSrvStop(); // Stop SAC operation
        break;
    case CEC_SAC_FB_MSG_WAKEUP_UNMUTE:
        // Come out of standby mode
        DEBUG_PRINT(MSG_STAT, "Standby mode Off Request\n");
        SiiCbCecStandby(false);
        RxAudioMuteSet( 0, false );      // Unmute speakers
        break;
    case CEC_SAC_FB_MSG_ERR_NONSAC_CMD:
        usedMessage = false;
        break;
    }
    return( usedMessage );
}
#endif

#if INC_CEC_SAC
//------------------------------------------------------------------------------
//! @brief      System Audio Control command processing
//! @param[in]  cmdData - CEC SAC key code
//! @retval     true  - if the cmd was recognized and processed
//! @retval     false - if the cmd wasn't processed
//------------------------------------------------------------------------------
bool_t  SkAppAudioProcessSacCommand ( uint8_t cmdData )
{
    uint8_t sacMsg      = CEC_SAC_FB_MSG_NONE;
    // If System Audio Mode is enabled, process volume controls locally
    if ( SiiCecSacIsSysAudioModeEnabled())
    {
        switch ( cmdData )
        {
        case CEA931_KEY_MUTE:
        case CEA931_KEY_MUTE_FUNC:
            PrintAlways( "CBUS MUTE received\n" );
            SiiCecSacMute( CEC_SAC_MUTE_ON );
            sacMsg = CEC_SAC_FB_MSG_MUTE;
            break;
        case CEA931_KEY_UN_MUTE_FUNC:
            PrintAlways( "CBUS UN-MUTE received\n" );
            SiiCecSacMute( CEC_SAC_MUTE_OFF );
            sacMsg = CEC_SAC_FB_MSG_UNMUTE;
            break;
        case CEA931_KEY_VOL_UP:
            PrintAlways( "CBUS VOL UP received\n" );
            SiiCecSacVolumeStep( true );
            sacMsg = CEC_SAC_FB_MSG_SAC_VOLUME_CHANGED;
            break;
        case CEA931_KEY_VOL_DOWN:
            PrintAlways( "CBUS VOL DOWN received\n" );
            SiiCecSacVolumeStep( false );
            sacMsg = CEC_SAC_FB_MSG_SAC_VOLUME_CHANGED;
            break;
        default:
            break;
        }
    }
    // If message was consumed, tell our local System Audio Controller about it
    return( SiiCbCecSacMessageHandler( sacMsg ));
}
#endif
//------------------------------------------------------------------------------
// Function:    SkAppTaskAudio
// Description:
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

void SkAppTaskAudio( BYTE cInstance )
{
    uint8_t chn;
    //CheckStreamMuxEnable();     // Determine if one or two instances are enabled.
    for( chn = 0; chn < 1 ; chn++)
    {
        SiiRxAudioInstanceSet(cInstance,chn);
        if (SiiRxAudioHandler(cInstance,&RxAudStatus[cInstance]))
        {
            if (memcmp(&RxAudState[cInstance][chn], &RxAudStatus[cInstance], sizeof(SiiRxAudioInstanceData_t)))
            {
                memcpy(&RxAudState[cInstance][chn], &RxAudStatus[cInstance], sizeof(SiiRxAudioInstanceData_t));
                switch (RxAudStatus[cInstance].audState)
                {
                case SII_AUD_STATE_OFF:
#if Uart_Debug
                    UART1_SendBytes("SII_AUD_STATE_OFF\r\n",strlen("SII_AUD_STATE_OFF\r\n"));
#endif
                    RxAudioMuteSet(  cInstance, chn, ON );  // Configure audio layout as 2ch and turn off audio hardware
                    break;
                case SII_AUD_STATE_ON:
#if Uart_Debug
                    UART1_SendBytes("SII_AUD_STATE_ON\r\n",strlen("SII_AUD_STATE_ON\r\n"));
#endif
                    RxAudioMuteSet(  cInstance, chn, OFF ); // Configure audio layout and hardware as appropriate for mode.
                    break;
                default:
                    break;
                }
                if (chn == 1) // sub pipe
                {
                    SiiDrvRxSpdifOutEnable(cInstance,app[cInstance].isSubPipeAudioExtSpdif);
                }
            }
        }
        if ( RxAudState[cInstance][chn].audState == SII_AUD_NO_ACP_CHECK )
        {
            SiiRxAudioNoAcpCheck(cInstance);
        }
    }
}

#if INC_RTPI
//------------------------------------------------------------------------------
// Function:    RtpiCbAudioByPassSet
// Description:
// Parameters:  isByPass
//              -true: audio by pass repeater,
//              -false:audio played by repeater
// Returns:     none
//------------------------------------------------------------------------------
void SiiRtpiCbAudioByPassSet( BYTE cInstance,bool_t isByPass )
{
    app[cInstance].isAudioByPass = isByPass;
}
#endif //#if INC_RTPI

