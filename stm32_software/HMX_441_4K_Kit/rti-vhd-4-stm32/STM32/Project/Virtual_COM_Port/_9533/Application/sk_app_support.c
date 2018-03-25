//***************************************************************************
//!file     sk_app_support.c
//!brief    Silicon Image SiI953x Starter Kit Application support functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "string.h"
#include "si_i2c.h"
#include "si_timer.h"
//#include "si_flash_update.h"
#include "si_eeprom.h"
#include "si_platform.h"
//#include "si_debugger_hdmigear.h"
#include "si_gpio.h"

#include "sk_application.h"
#include "sk_app_edid.h"
#if INC_CBUS
#include "sk_app_cbus.h"
#endif
#if INC_CEC
#include "sk_app_cec.h"
#endif
#include "si_drv_nvram_sram.h"
#include "si_drv_device.h"

//#define  INC_RTPI  1
#if INC_RTPI
#include "si_rtpi_component.h"
#endif

#include "main.h"
#include "Mcu.h"
#include "uart.h"
#include "si_logger.h"
#include "appSystemMgrAPI.h"
// add const //[kyq edit: 20150716 20:29:48]
const char tenDashes [] = "----------------\n";
const char asterisks [] = "****************************************************\n";

const char demoMenuExit [] = "\n Demo Menu -- Press TOGGLE key to exit Demo mode\n";
const char setupSelectMenu [] = "\n Setup Level -- Press TOGGLE key to select Setup Menu\n";
const char ccDisplayThisMenuMsg [] = "[CC] - Display this menu";
const char returnToSetupMenuMsg [] = "0 - Return to main Setup Menu";
const char returnToMainDemoMsg [] = "0 - Return to main Demo Menu";

//-------------------------------------------------------------------------------------------------
//! @brief      DEBUG_PRINT( MSG_ALWAYS ) helpers
//-------------------------------------------------------------------------------------------------
void PrintMenu ( char *pMenu[] )
{
    int i = 0;
    while ( pMenu[i])
    {
        DEBUG_PRINT2( MSG_ALWAYS, "%s\r\n", pMenu[i]);
        i++;
    }
}

void PrintStringCheckMark ( char *pString, bool_t isChecked )
{
    DEBUG_PRINT2( MSG_ALWAYS, "%s %c\r\n", pString, (isChecked) ? '*' : ' ');
}

void PrintStringOnOff ( char *pString, bool_t isOn )
{
    DEBUG_PRINT2( MSG_ALWAYS, "%s (%s)\r\n", pString, (isOn) ? "ON" : "OFF" );
}

void PrintStringYesNo ( char *pString, bool_t isYes )
{
    DEBUG_PRINT2( MSG_ALWAYS, "%s %s\r\n", pString, (isYes) ? "YES" : "NO" );
}

void PrintAlways ( char *pStr )
{
    DEBUG_PRINT2( MSG_ALWAYS, pStr );
}

//------------------------------------------------------------------------------
// Function:    SkAppDisplayData
// Description: Display the passed buffer in ASCII-HEX block format
//------------------------------------------------------------------------------

void SkAppDisplayData ( uint8_t msgLevel, uint8_t *pData, uint16_t length )
{
    uint16_t    i = 0;
    uint8_t     x, y;
    for ( y = 0; i < length; y++ )
    {
        DEBUG_PRINT2( msgLevel, "\n%02X: ", i );
        for ( x = 0; (i < length) && (x < 16); x++ )
        {
            DEBUG_PRINT2( msgLevel, " %02X", (uint16_t)pData[ i++] );
        }
    }
    DEBUG_PRINT2( msgLevel, "\n" );
}

//------------------------------------------------------------------------------
//! @brief  Initialize any uninitialized areas of the EEPROM
//-----------------------------------------------------------------------------c
bool_t SkAppEepromInitialize(BYTE cInstance, bool_t useDefaults, bool_t forceInit )
{
    bool_t updated = FALSE;
#if 0
    BOOL  bBootValid = FALSE;
    PSYSTEM_SETTING  psNVRAMData;
    // uint8_t initBuffer [256];
    psNVRAMData = utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    bBootValid =  psNVRAMData->acEDID_EEPROM_BOOT_VALID[cInstance];
    // Re-initialize the EEPROM the database is wrong or corrupt, or we're being forced.
    if (!bBootValid || forceInit )
    {
        // 写了后会设立EE_DEVBOOTDATA_VALID
        SkAppEdidWrite(cInstance, EDID_EEPROM_BOOT, (uint8_t *)&gEdidFlashDevBootData );
        updated = true;
    }
    //  if (/*( SiiPlatformEepromReadByte( EE_EDID_VALID ) != CONFIG_VALID ) ||*/ forceInit )
    //  {
    SkAppEdidWrite( cInstance,EDID_EEPROM, g_edidFlashEdidTable );
    updated = true;
    // }
    /*
    if (( SiiPlatformEepromReadByte( EE_EDIDVGA_VALID ) != CONFIG_VALID ) || forceInit )
    {
        SkAppEdidWrite( EDID_EEPROM_VGA, gEdidFlashEdidVgaTable );
        updated = true;
    }
    */
#endif
    return( updated );
}

//------------------------------------------------------------------------------
//! @brief  Update EEPROM control flags byte
//------------------------------------------------------------------------------
void SkAppEepromUpdateControl (BYTE cInstance, uint8_t mask, uint8_t value )
{
    uint8_t controlByte;
    //controlByte = SiiPlatformEepromReadByte( EE_FW_CONTROL );
    // SiiPlatformEepromWriteByte( EE_FW_CONTROL, (controlByte & mask) | value );
}

//------------------------------------------------------------------------------
//! @brief  Update EEPROM control flags byte
//! @note   Currently handles only EEFLAG_ROTARY_ENABLE flag
//------------------------------------------------------------------------------
bool_t SiiRtpiCbUpdateControl ( BYTE cInstance,uint8_t mask, uint8_t value )
{
    uint8_t controlByte, oldControlByte;
    oldControlByte = 0;//SiiPlatformEepromReadByte( EE_FW_CONTROL ); // TODO
    controlByte = oldControlByte;
    if ( mask & EEFLAG_ROTARY_ENABLE )
    {
        app[cInstance].rotaryDisabled = (( value & EEFLAG_ROTARY_ENABLE ) == 0 );
        controlByte = (oldControlByte & ~EEFLAG_ROTARY_ENABLE) | ( value & EEFLAG_ROTARY_ENABLE );
    }
    if ( controlByte != oldControlByte )
    {
        // SiiPlatformEepromWriteByte( EE_FW_CONTROL, (controlByte & mask) | value );
    }
    return( true );
}

#if INC_RTPI
//------------------------------------------------------------------------------
//! @brief  Support RRTPI register INPUT_NMB (0x24)
//------------------------------------------------------------------------------
uint8_t SiiRtpiCbUpdateSource(  BYTE cInstance, uint8_t functionSelect, uint8_t val)
{
    switch ( functionSelect )
    {
    case 0:                                     // Read current source value
        val = app[cInstance].newSource[app[cInstance].currentZone];
        break;
    case 1:                                     // Write source value
        app[cInstance].newSource[app[cInstance].currentZone] =(SiiSwitchSource_t) val;
        break;
    case 2:                                     // Read immediate value of RSW (not debounced)
        val = SiiPlatformReadRotary( true );
        break;
    }
    return( val );
}
#endif // INC_RTPI

//------------------------------------------------------------------------------
// Function:    SkAppUiUpdate
// Description:
//------------------------------------------------------------------------------

/*void SkAppUiUpdate ( void )
{
    bool_t isEnabledBySwitch;

    app.rotarySwitch = SiiPlatformReadRotary(false);
#if INC_RTPI
    if (app.rotarySwitch != RSW_NO_CHANGE)
        SiiRtpiPort(app.rotarySwitch, 1);
#endif

    isEnabledBySwitch = SiiPlatformSwitchGet(UI_FLASH_UPDATE);
    if (app.fwUpdEnabled != isEnabledBySwitch)
    {
        app.fwUpdEnabled = isEnabledBySwitch;
        DEBUG_PRINT(MSG_ALWAYS,"%s\n", app.fwUpdEnabled  ? "RESET MCU to enter F/W update mode." : "Canceled." );
    }

    isEnabledBySwitch = SiiPlatformSwitchGet(UI_AUTO_SWITCH);
    if (app.autoSwitch != isEnabledBySwitch)
    {   // Auto port switching enable change has detected
        app.autoSwitch = isEnabledBySwitch;
        DEBUG_PRINT(MSG_ALWAYS,"Rx Port Switching: %s\n", app.autoSwitch  ? "AUTO" : "MANUAL" );
    }

    isEnabledBySwitch = !SiiPlatformSwitchGet(UI_RC_ENABLE);
    if (app.irEnable != isEnabledBySwitch)
    {   // RC enable switch change has detected
        app.irEnable = isEnabledBySwitch;
        PrintStringOnOff( "IR Remote Control:", app.irEnable );
    }

    isEnabledBySwitch = SiiPlatformSwitchGet(UI_MHL_ENABLE);
    if (app.isEdidMhlLimits != isEnabledBySwitch)
    {   // MHL switch change has detected
        app.isEdidMhlLimits = isEnabledBySwitch;
        // The change will be applied when EdidTx gets re-initialized
    }

    isEnabledBySwitch = !SiiPlatformSwitchGet(UI_REPEATER_ENABLE);
    // Repeater enable can also be toggled by firmware, so
    // only change the real repeater enable if the switch
    // position has changed.
    if ((app.wasRepEnabledBySwitch != isEnabledBySwitch))
    {
        app.wasRepEnabledBySwitch = isEnabledBySwitch;

        app.repeaterEnable = isEnabledBySwitch;
        SkAppRepeaterConfigure(app.repeaterEnable);
    }

    // see if THX demo is enabled
    app[cInstance].isThxDemo = SiiPlatformSwitchGet(UI_THX_ENABLE);

    // Several switch state changes need to call into the device
    // drivers, and as such should not be called when in standby.
    if ( app.powerState == APP_POWERSTATUS_ON )
    {
#if INC_CEC
        SkAppCecConfigure(!SiiPlatformSwitchGet(UI_CEC_ENABLE));
#endif
    }
}  */

//------------------------------------------------------------------------------
// Function:    SkAppInstaPortLedUpdate
// Description:
//------------------------------------------------------------------------------

void SkAppInstaPortLedUpdate (  BYTE cInstance, uint8_t portBits )
{
    SiiPlatformGpioLedPorts(portBits);
}

//------------------------------------------------------------------------------
// Function:    SkAppBlinkTilReset
// Description:
//-----------------------------------------------------------------------------c
/*
void SkAppBlinkTilReset ( uint8_t errorLedPattern )
{

    SiiPlatformGpioLedStatus1( true );      // Signal that port LEDs signify an error

    for ( ;; )
    {
       // SkRemoteRequestHandler();

        SiiPlatformGpioLedPorts( errorLedPattern );
        SiiPlatformTimerWait( 300 );
        errorLedPattern ^= 0xFF;
       // SkAppTaskIrRemote( true );
    }

}
*/

//------------------------------------------------------------------------------
// Function:    SkAppInitialize
// Description:
//-----------------------------------------------------------------------------c

void SkAppInitInstanceData (  BYTE cInstance )
{
    int i;
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
    PSYSTEM_SETTING_DATA psSystemSettingData;
    psSystemSettingData  = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    memset( &app[cInstance], 0, sizeof( AppInstanceData_t ));
    app[cInstance].powerState              = APP_POWERSTATUS_ON ;
    app[cInstance].oldPowerState           = APP_POWERSTATUS_STANDBY;
#if INC_BENCH_TEST
    app[cInstance].modeSelect              = SK_MODE_BTST;
#else
    app[cInstance].modeSelect              = SK_MODE_TV;
#endif
    app[cInstance].irEnable                = false;
    app[cInstance].irUartKey             = 0xFF;
    app[cInstance].cecEnable               = false;
    app[cInstance].cecPrint                = true;
    app[cInstance].cecEnum                 = false;
    app[cInstance].edidLoad                = 0;
#ifdef USE_FULL_ASSERT
    app[cInstance].dbgMsgLevel             = MSG_PRINT_ALL;
#else
    app[cInstance].dbgMsgLevel   = psSystemSettingData->cDebugLevel;
#endif
    app[cInstance].dbgMsgColoring          = true;
    app[cInstance].arcDemoEnabled          = false;
    app[cInstance].arcDemoSelect           = 0;
    for (i = 0 ; i < SII_NUM_TX; i++)
    {
        /*app[cInstance].isTxRepeaterMode[i]     = false;
        app[cInstance].isTxBypassMode[i]       = false;
        app[cInstance].isTxHdcpRequired[i]     = false; */
        app[cInstance].isTxRepeaterMode[i]     = false;
        app[cInstance].isTxBypassMode[i]       = true;//false;
        app[cInstance].isTxHdcpRequired[i]     = true;
    }
    app[cInstance].v3dMode                 = SI_TX_VIDEO_3D_NONE;
    app[cInstance].vSyncMode               = SI_TX_VIDEO_HVSYNC_DE;
#if INC_IV
    app[cInstance].isExtTpg                = true;
    app[cInstance].tpgVideoPattern         = SI_ETPG_PATTERN_COLORBAR;
    app[cInstance].tpgVideoFormat          = SI_TPG_FMT_VID_480_60;
    app[cInstance].tpgClockSrc             = SI_TPG_CLK_XCLK;
#endif
    for (i = 0 ; i < SII_NUM_TX; i++)
    {
        app[cInstance].currentVideoSrc[i]      = SI_TX_VIDEO_SOURCE_HDMI;
        app[cInstance].currentVideoFormat[i]   = vm4_1280x720p;
        app[cInstance].currentAudioType[i]     = SI_TX_AUDIO_SRC_I2S_L0;
    }
    app[cInstance].isEdidMhlLimits         = false;
    app[cInstance].numOfEdidTxInst         = 1;
    app[cInstance].rotarySwitch            = RSW_NO_CHANGE;
    app[cInstance].lastRotarySwitch        = RSW_NO_CHANGE;
    app[cInstance].zoneEnabled[0]          = true;
    app[cInstance].zoneEnabled[1]          = true;
    app[cInstance].newZoneEnabled[0]       = true;
    app[cInstance].newZoneEnabled[1]       = true;
    app[cInstance].currentSource[0]        = (SiiSwitchSource_t)0xFF;
    app[cInstance].currentSource[1]        = (SiiSwitchSource_t)0xFF;
    app[cInstance].currentZone             = 0;
    app[cInstance].audioZoneSource[0]      = SiiSwitchSource2;
    app[cInstance].audioZoneSource[1]      = SiiSwitchSource2;
#if 0
    if(DeviceID==6)
    {
        if(IntOrExt_audio[DeviceID-6]==1)            //选择外部音频插入
        {
            app[DeviceID].currentAudioType[0]= SI_TX_AUDIO_SRC_I2S_L0;
            app[DeviceID].newAudioSource[0] = SiiSwitchAudioSourceExternal;
        }
        else
        {
            app[DeviceID].currentAudioType[0]= SI_TX_AUDIO_SRC_NONE;
            app[cInstance].newAudioSource[0]       = SiiSwitchAudioSourceDefault;
        }
    }
    else  if(DeviceID==7)
    {
        if(IntOrExt_audio[DeviceID-6]==1)            //选择外部音频插入
        {
            app[DeviceID].currentAudioType[0]= SI_TX_AUDIO_SRC_I2S_L0;
            app[DeviceID].newAudioSource[0] = SiiSwitchAudioSourceExternal;
        }
        else
        {
            app[DeviceID].currentAudioType[0]= SI_TX_AUDIO_SRC_NONE;
            app[DeviceID].newAudioSource[0] = SiiSwitchAudioSourceDefault;
        }
    }
    else
    {
        app[DeviceID].currentAudioType[0]= SI_TX_AUDIO_SRC_NONE;
        app[DeviceID].newAudioSource[0] = SiiSwitchAudioSourceDefault;
    }
#endif
    app[cInstance].newAudioSource[0]       = SiiSwitchAudioSourceDefault;
    app[cInstance].newAudioSource[1]       = SiiSwitchAudioSourceDefault;
    app[cInstance].currentAudioSource[0]   = SiiSwitchAudioSourceDefault;
    app[cInstance].currentAudioSource[1]   = SiiSwitchAudioSourceDefault;
    app[cInstance].audioSampleRate[0]      = SI_TX_AUDIO_SMP_RATE_48_KHZ;
    app[cInstance].audioSampleRate[1]      = SI_TX_AUDIO_SMP_RATE_48_KHZ;
    app[cInstance].isAudioExtractionEnabled[0]= false;
    app[cInstance].isAudioExtractionEnabled[1]= false;
#if INC_IPV
    app[cInstance].ipvAllowed              = true;
    app[cInstance].ipvEnabled              = false;
    app[cInstance].ipvPipEnabled           = false;
#endif
    app[cInstance].isThxDemo                = false;
    app[cInstance].msThxDelay               = 0;
    app[cInstance].msThxStart               = 0;
    app[cInstance].markerOptionFlagsMp     = ENABLE_3D_MARKER_SUPPORT;
    app[cInstance].markerOptionFlagsRp     = ENABLE_3D_MARKER_SUPPORT;
    app[cInstance].isCecAsSwitch = false;
    app[cInstance].txTmdsFreqRange[0] = 3; //< 300MHz
#if PRD_TX_STANBY_DELY
    app[cInstance].bTXStandByDelayStart = false;
#endif
    app[cInstance].bHDMIAudioEn = psOutputPortMgrData->abHDMIAudio_EN[utilInstanceToOutputPort(cInstance)];
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      FALSE);
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                      FALSE);
}
//------------------------------------------------------------------------------
//! @brief  Initialize the application layer
//-----------------------------------------------------------------------------c
bool_t SkAppInitialize ( BYTE cInstance )
{
    //uint8_t flags;
    SkAppInitInstanceData(cInstance);
    SiiOsTimerSet( &app[cInstance].appTimer, mS_TO_TICKS(DEM_POLLING_DELAY) );      // Reset poll timer
    // Check starter kit EEPROM and initialize it if necessary.
    if ( SkAppEepromInitialize( cInstance,true, false ))
    {
        PrintAlways( "Settings EEPROM initialized to defaults.\n");
    }
    // Load control flags from EEPROM.
    /*flags = SiiPlatformEepromReadByte( EE_FW_CONTROL );
    app.irEnable        = ((flags & EEFLAG_IR_ENABLE) == EEFLAG_IR_ENABLE );
    app.rotaryDisabled  = ((flags & EEFLAG_ROTARY_ENABLE) != EEFLAG_ROTARY_ENABLE );
    app.cecPrint        = ((flags & EEFLAG_CEC_PRINT) == EEFLAG_CEC_PRINT );
    app.cecEnum         = ((flags & EEFLAG_CEC_ENUM) == EEFLAG_CEC_ENUM );
    app.dbgMsgColoring  = ((flags & EEFLAG_DBG_MSG_COLORING) == EEFLAG_DBG_MSG_COLORING );
    app.edidLoad        = SiiPlatformEepromReadByte( EEPROM_EDID_LOADSEL );
    app.dbgMsgLevel     = SiiPlatformEepromReadByte( EE_UART_MSG_LEVEL );

    // CEC switch closed (CEC OFF) setting overrides an EEPROM CEC ON setting.
    app.cecEnable       = !SiiPlatformSwitchGet(UI_CEC_ENABLE);  */
    SiiPlatformLoggerColorEnable( app[cInstance].dbgMsgColoring );
    SiiPlatformLoggerMessageLevelSet( app[cInstance].dbgMsgLevel );
    app[cInstance].autoSwitch      = 0;
    app[cInstance].newSource[app[cInstance].currentZone] = SiiSwitchSource2; // ZZL 用这个变量切换输入通道 Read immediate value of RSW (not debounced)
    // PrintStringYesNo( "\nCEC Enabled:  ", app.cecEnable );
    //PrintStringYesNo( "IR Enabled:   ", app[cInstance].irEnable );
    //PrintAlways( "\n" );
    return( true );
}

