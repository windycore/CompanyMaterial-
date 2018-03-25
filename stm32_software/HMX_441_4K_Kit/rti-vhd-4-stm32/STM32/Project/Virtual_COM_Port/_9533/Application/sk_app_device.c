//***************************************************************************
//!file     sk_app_device.c
//!brief    Silicon Image SiI9535 Starter Kit Application device functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "string.h"
#include "Mcu.h"
#include "sk_application.h"
#include "si_gpio.h"
#include "uart.h"

#define INC_AUDIO_RX 1

#if INC_CEC
#include "si_cec_component.h"
#endif
#if INC_CBUS
#include "sk_app_cbus.h"
#endif
#include "si_repeater_component.h"
#include "si_tx_component.h"
#if INC_AUDIO_RX
#include "si_audio_component.h"
#endif
#include "si_cra.h"
#include "si_drv_device.h"
#include "si_drv_nvram_sram.h"
#include "si_drv_repeater.h"
#include "si_drv_switch.h"
#include "si_drv_gpio.h"
#include "si_drv_tpi_system.h"

#if (FPGA_BUILD == SI_ENABLE)
//#include "si_fpga.h"
#endif

//#define   INC_RTPI  1
#if INC_RTPI
#include "si_rtpi_component.h"
#endif

extern const char lSignonMsg [];


//------------------------------------------------------------------------------
//! @brief  Initialize the various components of the SiI9535 device
//------------------------------------------------------------------------------
bool_t SkAppDeviceInitialize (  BYTE cInstance )
{
#if INC_CEC
#ifndef POWER_UP_ONLY
    int     i;
#endif
#endif
    bool_t  success = false;
    do
    {
        // Initialize device drivers.
        if ( !SiiCraInitialize( cInstance ))
        {
            break;
        }
        if ( !SiiDrvNvramInitialize( cInstance ))
        {
            break;    // 变量初始化
        }
        // 芯片9533 启动，BOOT, 部分寄存器初始化
        if ( !SiiDrvDeviceInitialize(cInstance,  app[cInstance].powerState == APP_POWERSTATUS_ON))
        {
            DEBUG_PRINT( MSG_ERR, "\n9533[%d] SiiDrvDeviceInitialize ERROR!",cInstance );
            break;
        }
#ifndef POWER_UP_ONLY
#if (FPGA_BUILD == SI_ENABLE)
        // SiiPlatformFpgaInit();
#endif
#if ( SII_POWER_TYPE != APP_POWERTYPE_FULL )
        if ( app[cInstance].powerState == APP_POWERSTATUS_ON )
#endif
        {
            if ( !SkAppDeviceInitMsw( cInstance ))
            {
                break;    // Initialize before TX
            }
            if ( !SkAppDeviceInitTx(cInstance))
            {
                break;    // Initialize before the others
            }
            if ( !SkAppDeviceInitRepeater( cInstance,true ))
            {
                break;
            }
#if INC_CBUS
            if ( !SkAppDeviceInitCbus())
            {
                break;
            }
#endif
            if ( !SiiDrvDeviceRelease( cInstance ))
            {
                break;
            }
            if ( !SkAppDeviceInitAudio( cInstance ))
            {
                break;
            }
#if INC_OSD
            SkAppDeviceInitOsd();   // Do not stop if this init fails
#endif
#if INC_IPV
            if ( !SkAppTaskIpvInitialize())
            {
                break;
            }
#endif
            if ( !SkAppDeviceInitThx( cInstance ))
            {
                break;
            }
        }
        if ( !SkAppDeviceInitEdidTx( cInstance,true ))
        {
            break;
        }
#if INC_CEC
#if ( SII_POWER_TYPE != APP_POWERTYPE_FULL )
        if ( app[cInstance].powerState == APP_POWERSTATUS_ON )
#endif
        {
            // Check for a downstream device and get it's EDID
            // if available so that we can assign a CEC physical address
            // for ourselves.  This requires power to the PWD domain
            // of the SiI9535.
            DEBUG_PRINT(MSG_ALWAYS, "CECInit:*************\r\n");
            for( i = 0; i < SII_NUM_TX; i++ )
            {
                // Get physical state of HPD without bothering the rest of
                // the driver component.
                SkAppTxInstanceSet( cInstance , i);
                if ( SiiDrvTpiGetHpdState( cInstance ))
                {
                    SkAppProcessTxEdid( cInstance, i);
                    DEBUG_PRINT(MSG_ALWAYS, "CEC_edid_process:*************\r\n");
                }
            }
        }
        if ( !SkAppDeviceInitCec( cInstance ))
        {
            break;    // Must initialize AFTER Tx1
        }
        DEBUG_PRINT(MSG_ALWAYS, "CEC_Init_successful:*************\r\n" );
#endif
#if INC_ARC
        if ( app[cInstance].powerState == APP_POWERSTATUS_ON )
        {
            if ( !SkAppDeviceInitArc())
            {
                break;    // Must initialize AFTER CEC
            }
        }
#endif
#if INC_CBUS
        if ( !SkAppDeviceInitGpio())
        {
            break;
        }
#endif
#if INC_CDC
        if ( !SkAppInitCdc())
        {
            break;
        }
#endif
#if INC_RTPI
        if ( !SiiRtpiInitialize(cInstance,0))
        {
            break;
        }
#endif
#endif
        success = true;
    }
    while (0);      // Do the above only once
#ifndef POWER_UP_ONLY
    DEBUG_PRINT( MSG_ALWAYS, "\nDeviceInit: %04X r%02X  ",
                 SiiDrvDeviceInfo(  cInstance, SiiDEV_ID ), SiiDrvDeviceInfo(  cInstance, SiiDEV_REV ));
    if ( success )
    {
        PrintAlways( "successful\n" );
    }
    else
    {
        DEBUG_PRINT( MSG_ERR, "!!!!Failure, halting...\n" );
    }
    // Don't call SiiDrvDeviceStart() unless prior stages were successful
    if ( success && !SiiDrvDeviceStart(cInstance))
    {
        success = false;
        DEBUG_PRINT( MSG_ERR, "!!!!Device Driver Start failure, halting...\n");
    }
#endif
    return( success );
}

//------------------------------------------------------------------------------
//! @brief  Put chip into lowest power mode that can be done with power still on
//------------------------------------------------------------------------------
void SkAppLowPowerStandby (  BYTE cInstance )
{
    int i;
#if ( SII_POWER_TYPE != APP_POWERTYPE_MAIN )
    SiiRxAudioStandby(cInstance);
    SiiDrvSwitchStandby(cInstance);
#if INC_ARC
    SiiDrvArcStandby();
#endif
#if INC_IPV
    SiiDrvIpvStandby();
#endif
#if INC_CBUS
//    SiiDrvCbusStandby();
    SkAppCbusStandby();
#endif
    // Shut down both sub-instances of the repeater
    for (i=0; i<SII_NUM_REPEATER; i++)
    {
        SiiRepeaterInstanceSet(cInstance,i);
        SiiDrvRepeaterEnable(cInstance,false);
    }
    SiiDrvDeviceStandby(cInstance);  // Last standby called is always the device main driver.
#endif
}

//------------------------------------------------------------------------------
// Function:    AppDeviceStandby
// Description: Places the device into standby mode if available
// Parameters:  none
// Returns:     true if the device entered standby mode successfully, or false
//              if some failure occurred.
//------------------------------------------------------------------------------
//
//---> SkAppDevicePower()
static void AppDeviceStandby (  BYTE cInstance )
{
    bool_t failure = false;
    PrintAlways( "********************************\n");
    PrintAlways( "** DEVICE STANDBY (POWER OFF) **\n");
    PrintAlways( "********************************\n");
    // Place all components/drivers in standby
#if INC_CEC
    failure = !SkAppCecStandby( cInstance ) || failure;  // Do this first because it sends a CEC Standby message
#endif
#if INC_CBUS
    failure = !SkAppCbusStandby() || failure;
#endif
#if INC_CDC == 1
    failure = !SiiCdcStandby( cInstance ) || failure;
#endif
    SiiTxStandby(cInstance);
    SkAppLowPowerStandby(cInstance);
    if ( failure )
    {
        DEBUG_PRINT( MSG_ERR, "\n!!!!!! Error occurred entering Standby !!!!!!\n");
    }
}

//------------------------------------------------------------------------------
//! @brief  Causes operation of the device to return to the state it was
//!         prior to the previous call to SiiDeviceStandby.
//! @return true if the device successfully exited standby mode , or false
//!         if some failure occurred.
//! @note   TX module is not restored to operation by this function.  When
//!         the first HOTPLUG detect interrupt occurs following the power on
//!         state, the TX module will automatically be restarted
//------------------------------------------------------------------------------
static void AppDeviceResume (  BYTE cInstance )
{
    bool_t failure = false;
    PrintAlways( "********************************\n");
    PrintAlways( "** DEVICE RESUME (POWER ON)   **\n");
    PrintAlways( "********************************\n");
    // Power is back on, so restore any configuration information that
    // was saved at the last standby.
    failure = !SiiDrvDeviceResume(cInstance) || failure;   // First resume called is always the device main driver.
#if INC_CEC
    failure = !SkAppCecResume( cInstance ,true) || failure;
#endif
#if INC_CDC == 1
    failure = !SiiCdcResume() || failure;
#endif
#if INC_CBUS
    failure = !SkAppCbusResume() || failure;
#endif
    // Tx will resume when a downstream hotplug is detected
    if ( failure )
    {
        DEBUG_PRINT( MSG_ERR, "\n!!!!!! Error occurred during Resume !!!!!!\n");
    }
}

//------------------------------------------------------------------------------
//! @brief  Monitor and implement device power requests.
//!         If any module has changed the app.powerState value, apply
//!         the appropriate action for the current application power state.
//!         This state may have been modified by the physical UI, the remote
//!         control, a CEC or CBUS message, or some other external influence.
//------------------------------------------------------------------------------
//  --> dvSii9533PowerControl()
//
void SkAppDevicePower ( BYTE cInstance )
{
    // Check for physical power button change.
#if (FPGA_BUILD == SI_DISABLE)
    if (SiiPlatformGpioPowerVCC5())
#else
    if (true)
#endif
    {
        // Power button has been pressed, toggle the power state.
        // Since we're the power button, don't modify the power
        // state if it is in a transitional stage.
        if ( app[cInstance].powerState == APP_POWERSTATUS_ON )
        {
#if (FPGA_BUILD == SI_DISABLE)
            app[cInstance].powerState = APP_POWERSTATUS_ON_TO_STANDBY;
#endif
        }
        else if (( app[cInstance].powerState == APP_POWERSTATUS_STANDBY ) || ( app[cInstance].powerState == APP_POWERSTATUS_INITIAL ))
        {
            app[cInstance].powerState = APP_POWERSTATUS_STANDBY_TO_ON;
        }
    }
    // If power state was changed by somebody, act on it.
    if ( app[cInstance].powerState != app[cInstance].oldPowerState )
    {
        /*
        PrintAlways( "\n====================================================\n");
        DEBUG_PRINT( MSG_ALWAYS, "==  Main Power: %-7s                           ==\n",
            ( app[cInstance].powerState == APP_POWERSTATUS_STANDBY_TO_ON )? "ON     " : "STANDBY");
        DEBUG_PRINT( MSG_ALWAYS, "==  %s%s                      ==\n", lSignonMsg, buildVersion );
        PrintAlways( "====================================================\n");
        */
        switch ( app[cInstance].powerState )
        {
        case APP_POWERSTATUS_STANDBY_TO_ON:
#if ( SII_POWER_TYPE != APP_POWERTYPE_FULL )
            // Turn on the device power
            SiiPlatformGpioSetPower(SII_GPIO_POWER_ALL, ON );
#endif
#ifdef POWER_UP_ONLY
            app[cInstance].powerState = APP_POWERSTATUS_ON;
#else // #ifndef POWER_UP_ONLY
            if ( SkAppInitialize( cInstance ))
            {
                app[cInstance].powerState = APP_POWERSTATUS_ON;
                if ( SkAppDeviceInitialize( cInstance ))
                {
                    AppDeviceResume( cInstance );
                    break;
                }
            }
            // Initialization failed somehow, let the user know it.
            //SkAppBlinkTilReset( ERRLED_BAD_INIT );
#endif //#ifdef POWER_UP_ONLY
            break;
        case APP_POWERSTATUS_ON_TO_STANDBY:
            AppDeviceStandby( cInstance );
            app[cInstance].powerState = APP_POWERSTATUS_STANDBY;
#if ( SII_POWER_TYPE != APP_POWERTYPE_FULL )
            // Power down the device
            SiiPlatformGpioSetPower(SII_GPIO_POWER_ALL, OFF );
#endif
            break;
        case APP_POWERSTATUS_ON:
        case APP_POWERSTATUS_STANDBY:
        default:
            break;
        }
        app[cInstance].oldPowerState     = app[cInstance].powerState;
#ifndef POWER_UP_ONLY
#if INC_RTPI
        SiiRtpiOnPwrStateChange( cInstance, (APP_POWERSTATUS_ON == app[cInstance].oldPowerState));
#endif //INC_RTPI
#endif //#ifndef POWER_UP_ONLY
    }
}




#if INC_RTPI
//-------------------------------------------------------------------------------------------------
//! @brief      Callback for the RTPI component to call and set power state.
//-------------------------------------------------------------------------------------------------
bool_t SiiRtpiCbPowerControl(  BYTE cInstance,int newState )
{
    switch ( newState )
    {
    case 0:
        app[cInstance].powerState = APP_POWERSTATUS_ON_TO_STANDBY;
        break;
    case 1:
        app[cInstance].powerState = APP_POWERSTATUS_STANDBY_TO_ON;
        break;
    case 2:
        app[cInstance].powerState = APP_POWERSTATUS_ON_TO_STANDBY;
        SkAppDevicePower(cInstance);
        app[cInstance].powerState = APP_POWERSTATUS_STANDBY_TO_ON;
        break;
    }
    SkAppDevicePower(cInstance);
    return( true );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Callback for the RTPI power state.
//-------------------------------------------------------------------------------------------------
uint8_t SiiRtpiCbPowerState ( BYTE cInstance )
{
    return(( app[cInstance].powerState != APP_POWERSTATUS_ON ) ? 0 : 1 );
}
#endif // INC_RTPI

//-------------------------------------------------------------------------------------------------
//! @brief      Callback for the OSD component to obtain the specified zone resolution.
//-------------------------------------------------------------------------------------------------
bool_t  SiiOsdCbGetOutputRes( BYTE cInstance ,int zone, int_t *pHoriz, int_t *pVert, bool_t *pIsInterlaced )
{
    bool_t  success;
    success = true;
    // If HDMI is source for zone, use the selected port resolution
    if ( app[cInstance].currentSource[zone] < SiiSwitchSourceInternalVideo )
    {
        if ( !SiiDrvSwitchPortInfoGet( cInstance,  app[cInstance].currentSource[zone], (zone == 0), NULL, pHoriz, pVert, pIsInterlaced, NULL )) // Get current port info
        {
            success = false;
        }
    }
    // Otherwise, use the current video mode of the TPG source.
    else
    {
        if ( app[cInstance].currentVideoFormat[zone] < vm_numVideoModes)
        {
            *pHoriz         = VideoModeTable[ app[cInstance].currentVideoFormat[zone]].Active.H;
            *pVert          = VideoModeTable[ app[cInstance].currentVideoFormat[zone]].Active.V;
            *pIsInterlaced  = VideoModeTable[ app[cInstance].currentVideoFormat[zone]].Interlaced;
        }
        else
        {
            success = false;
        }
    }
    if ( !success )
    {
        DEBUG_PRINT( MSG_DBG, "\nUnable to get output resolution for OSD\n");
    }
    return( success );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Monitors SiI953x device interrupts and notifies drivers that have received
//!             an interrupt.
//
//   --->dvSii9533Process()
//-------------------------------------------------------------------------------------------------
void SkAppTaskSiiDevice( BYTE cInstance )
{
    uint16_t statusFlags;
    // Check for any SiI953x interrupt.
    if ( SiiDrvDeviceManageInterrupts( cInstance, app[cInstance].powerState == APP_POWERSTATUS_ON ))
    {
        //没有外部中断脚,每次都执行
        //UART1_SendBytes("APP_POWERSTATUS_ON \r\n",strlen("APP_POWERSTATUS_ON \r\n"));
#if INC_CEC  //2014009
        SiiDrvCpiInstanceSet(  cInstance, CPI_INSTANCE_AVR );
        if ( SiiDrvCpiStatus( cInstance ) & SiiCPI_CEC_INT)
        {
            app[cInstance].cecInterruptRx = true;
        }
#if 0
        SiiDrvCpiInstanceSet( CPI_INSTANCE_AUXTX );
        if ( SiiDrvCpiStatus() & SiiCPI_CEC_INT)
        {
            app[cInstance].cecInterruptTx = true;
        }
#endif
#endif //#if INC_CEC
        statusFlags = SiiDrvDeviceStatus(cInstance);// deviceDrvInstance[cInstance].statusFlags
        // Resolution stable event is available in MP only and is filtered from intermittent events
        if ( statusFlags & (SiiDEV_STATUS_RES_STB_CHG | SiiDEV_STATUS_MP_RES_CHG) )
        {
            app[cInstance].resChangeIntr[0] = true;
#if INC_RTPI
            SiiRtpiSetInt(cInstance,RP_M__INTERRUPT__RESOLUTION_DETECTED);
#endif
        }
        // SP has only resolution change events, so react on them, even though
        // those events may happen more than one time during the resolution stabilization stage
        if ( statusFlags & SiiDEV_STATUS_SP_RES_CHG )
        {
            app[cInstance].resChangeIntr[1] = true;
        }
#if INC_RTPI
        if ( statusFlags & SiiDEV_STATUS_AVI_READY )
        {
            SiiRtpiOnAviInfoFrameChange( cInstance );
        }
        if ( statusFlags & SiiDEV_STATUS_AIF_READY )
        {
            SiiRtpiOnAudioInfoFrameChange( cInstance );
        }
#endif
#if INC_CBUS
        {
            uint8_t i;
            for (i=0; i < MHL_MAX_CHANNELS; i++)
            {
                SkAppCbusInstanceSet( i );
                if ( SiiDrvCbusStatus() & SiiCBUS_INT )     // CBUS interrupts
                {
                    pAppCbus->cbusInterrupt = true;
                }
            }
        }
#endif
        // Check Switch interrupts but don't clear them.
        statusFlags = SiiDrvSwitchStatus(  cInstance, SiiSWITCH_PEEK_STATUS );  // SET BY SwitchInfoframeProcessInterrupts()
        if ( statusFlags & SiiSWITCH_INT )
        {
            app[cInstance].switchInterrupt = true;   // WILL BE RUN SkAppTaskSwitch()
        }
    }
}


