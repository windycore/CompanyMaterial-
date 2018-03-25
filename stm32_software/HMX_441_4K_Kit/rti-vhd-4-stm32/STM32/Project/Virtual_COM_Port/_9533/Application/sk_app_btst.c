//***************************************************************************
//!file     sk_app_btst.c
//!brief    SK 9535 Starter Kit firmware -  Automated Bench Test
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_drv_internal.h"
#include "si_regs_audio953x.h"

#include "sk_application.h"
#include "si_platform.h"
#include "si_gpio.h"

#include "si_drv_audio.h"
#include "si_drv_device.h"
#if INC_CEC
#include "si_cec_component.h"
#endif
#if INC_IPV
#include "si_ipv_component.h"
#endif
#if INC_OSD
#include "si_osd_component.h"
#endif
#include "si_drv_tpi_system.h"


//------------------------------------------------------------------------------
// Module data
//------------------------------------------------------------------------------
#if (BENCH_TEST_STATIC_300MHZ == SI_DISABLE)
typedef enum testId
{
    BTEST_MSW_M0_S1,
    BTEST_MSW_M1_S0,
    BTEST_MSW_M2_S3,
    BTEST_MSW_M3_S2,
    BTEST_AUD_EXTR_M_S,
    BTEST_OSD,
    BTEST_IPV,
    BTEST_IPV_PIP,
    BTEST_IV_COLORBAR,
    BTEST_IV_CHESS,
    BTEST_IV_RAMP,
    BTEST_AUDIO_INSERTION,
    BTEST_AUDIO_INSERTION_MP,
    BTEST_AUDIO_INSERTION_SP,
    BTEST_AUDIO_INSERTION_EXT,
    BTEST_SK_MODE_TV,

    NUM_OF_BENCH_TESTS
} testId_t;

static clock_time_t bTestDuration[NUM_OF_BENCH_TESTS] =
{
    20000,     //BTEST_MSW_M0_S1
    20000,     //BTEST_MSW_M1_S0
    20000,     //BTEST_MSW_M2_S3
    20000,     //BTEST_MSW_M3_S2
    50000,     //BTEST_AUD_EXTR_M_S
    20000,     //BTEST_OSD
    30000,     //BTEST_IPV
    30000,     //BTEST_IPV_PIP
    10000,     //BTEST_IV_COLORBAR
    10000,     //BTEST_IV_CHESS
    10000,     //BTEST_IV_RAMP
    30000,     //BTEST_AUDIO_INSERTION
    30000,     //BTEST_AUDIO_INSERTION_MP
    30000,     //BTEST_AUDIO_INSERTION_SP
    30000,     //BTEST_AUDIO_INSERTION_EXT
    30000 ,     //BTEST_SK_MODE_TV
};

static bool_t isSkipTestFor9533[NUM_OF_BENCH_TESTS] =
{
    true,      //BTEST_MSW_M0_S1
    true,      //BTEST_MSW_M1_S0
    true,      //BTEST_MSW_M2_S3
    true,      //BTEST_MSW_M3_S2
    true,      //BTEST_AUD_EXTR_M_S
    false,     //BTEST_OSD
    false,     //BTEST_IPV
    false,     //BTEST_IPV_PIP
    false,     //BTEST_IV_COLORBAR
    false,     //BTEST_IV_CHESS
    false,     //BTEST_IV_RAMP
    false,     //BTEST_AUDIO_INSERTION
    false,     //BTEST_AUDIO_INSERTION_MP
    false,     //BTEST_AUDIO_INSERTION_SP
    false,     //BTEST_AUDIO_INSERTION_EXT
    false,     //BTEST_SK_MODE_TV
};


static char *pBenchTestNames [] =
{
    "MSW Test. Main:0, Sub:1...",
    "MSW Test. Main:1, Sub:0...",
    "MSW Test. Main:2, Sub:3...",
    "MSW Test. Main:3, Sub:2...",
    "Audio Extraction Test...",
    "OSD Test...",
    "IPV Test...",
    "IPV PIP TEST"
    "Internal Video Test: Color Bars.",
    "Internal Video Test: Chess Field.",
    "Internal Video Test: Gray Levels Ramp.",
    "Audio Insertion Ext SPDIF Test",
    "Audio Insertion Main Pipe Test",
    "Audio Insertion Sub Pipe Test",
    "Audio Insertion External SPDIF Test",
    "TV Mode Test",
};



#else // if BENCH_TEST_STATIC_300MHZ == SI_ENABLE
typedef enum testId
{
    BTEST_MSW_MX_SY,
    BTEST_MSW_MY_SX,

    NUM_OF_BENCH_TESTS
} testId_t;

static clock_time_t bTestDuration[NUM_OF_BENCH_TESTS] =
{
    30000,     //BTEST_MSW_M0_S1
    30000,     //BTEST_MSW_M1_S0
};

static bool_t isSkipTestFor9533[NUM_OF_BENCH_TESTS] =
{
    true,      //BTEST_MSW_M0_S1
    true,      //BTEST_MSW_M1_S0
};
#endif



//-------------------------------------------------------------------------------------------------
//! @brief      DEBUG_PRINT helpers
//-------------------------------------------------------------------------------------------------
#if (BENCH_TEST_STATIC_300MHZ == SI_DISABLE)
static void PrintBenchTestName ( int testNo )
{
    DEBUG_PRINT( MSG_ALWAYS, "%s\n", pBenchTestNames[ testNo] );
}
#endif

static void PrintHashLine( void )
{
    PrintAlways( "###########################################################################\n" );
}

//------------------------------------------------------------------------------
// Function:    SkAppBenchTestIrRemote
// Description: Parse remote control keys for the Bench Test App.
// Parameters:  fullAccess - true if allowed to respond to all normal keys
//                           false if only allowed to respond to power keys
// Returns:     none
//------------------------------------------------------------------------------
void SkAppBenchTestIrRemote ( RC5Commands_t key, bool_t fullAccess )
{
}

//------------------------------------------------------------------------------
// Function:    SkAppBenchTest
// Description: CP 953x Bench Test Automation
//------------------------------------------------------------------------------

void SkAppBenchTest (  BYTE cInstance )
{
//    int_t rxPort[2];
    static testId_t test = 0; // the very first test in testId_t
    static bool_t isFirstEntry = true;
    static SiiIpvTheme_t ipvTheme = 0;
    // SiI9573 version of the chip doesn't have sub-pipe, so some of the tests must be skipped
    bool_t is9533 = (SiiDrvDeviceInfo( cInstance,  SiiDEV_ID ) != 0x9535);
    /****************************************************************************************/
    /* Display Port Processor State                                                         */
    /****************************************************************************************/
    test = BTEST_IV_COLORBAR;
    SkAppInstaPortLedUpdate(  cInstance, SiiDrvDeviceInfo( cInstance, SiiDEV_SELECTED_PORT_BITFIELD));
    if ( !SiiOsTimerExpired( app.appTimer ))
    {
        return;
    }
    /*    if (is9533 && isSkipTestFor9533[test])
        {
            test++;
            PrintAlways( "Test Skipped for SiI9573.\n" );
            return;
        }
        else*/ if (test < NUM_OF_BENCH_TESTS)
    {
        SiiOsTimerSet( &app.appTimer, bTestDuration[test]);
    }
#if (BENCH_TEST_STATIC_300MHZ == SI_ENABLE)
    // Set TX TMDS settings for 300 MHz range statically for this type of test
    SkAppTxAdjustAnalogSettings( cInstance, 0, 30000);
    SkAppTxAdjustAnalogSettings( cInstance, 1, 30000);
    // If Switch 5 is ON, static InstaPort mode will be selected
    if (SiiPlatformSwitchGet(SW_5))
    {
        uint8_t rxPort;
        SkAppIpvEnable( false );
        rxPort = SiiPlatformReadRotary(true);
        SkAppMswDemoSetSources(rxPort, rxPort);
        DEBUG_PRINT( MSG_ALWAYS, "InstaPort 300 MHZ Test...\nMain port: %d\n", rxPort );
        SiiOsTimerSet( &app.appTimer, 10000);
        return; // skip all other tests
    }
#endif
    PrintHashLine();
    switch(test)
    {
#if (BENCH_TEST_STATIC_300MHZ == SI_ENABLE)
    case   BTEST_MSW_MX_SY:
        SkAppIpvEnable( false );
        rxPort[0] = SkAppSwitchGetNextPort( 5, true );
        rxPort[1] = SkAppSwitchGetNextPort( rxPort[0], true );
        SkAppMswDemoSetSources(rxPort[0], rxPort[1]);
        DEBUG_PRINT( MSG_ALWAYS, "MSW Test. Main:%d, Sub:%d\n", app.newSource[0], app.newSource[1]);
        test++; // move to next test
        break;
    case   BTEST_MSW_MY_SX:
        // swap sources
        SkAppMswDemoSetSources(app.currentSource[1], app.currentSource[0]);
        DEBUG_PRINT( MSG_ALWAYS, "MSW Test. Main:%d, Sub:%d\n", app.newSource[0], app.newSource[1]);
        test++; // move to next test
        break;
#else
    case   BTEST_MSW_M0_S1:
        // Clean up previous test
        SiiIpvEnable( false );
        PrintBenchTestName( BTEST_MSW_M0_S1 );
        SkAppMswDemoSetSources( 0, 1 );
        test++; // move to next test
        break;
    case   BTEST_MSW_M1_S0:
        PrintBenchTestName( BTEST_MSW_M1_S0 );
        SkAppMswDemoSetSources( 1, 0 );
        test++; // move to next test
        //test+=4;
        break;
    case   BTEST_MSW_M2_S3:
        PrintBenchTestName( BTEST_MSW_M2_S3 );
        SkAppMswDemoSetSources( 2, 3 );
        test++; // move to next test
        break;
    case   BTEST_MSW_M3_S2:
        PrintBenchTestName( BTEST_MSW_M3_S2 );
        SkAppMswDemoSetSources( 3, 2 );
        test++; // move to next test
        break;
    case   BTEST_AUD_EXTR_M_S:
        SkAppMswDemoSetSources( 2, 4 ); // MSW should be enabled
        PrintBenchTestName( BTEST_AUD_EXTR_M_S );
        SiiPlatformGpioAudioInOut(true);
        SiiDrvRxAudioInstanceSet( cInstance, 0);
        test++; // move to next test
        break;
#if INC_OSD
    case   BTEST_OSD:
        // Cleanup previous test
        SkAppMswDemoSetSources(1, 1);
        PrintBenchTestName( BTEST_OSD );
        if (isFirstEntry)
        {
            SkAppOsdMainMenu();
            isFirstEntry = false;
        }
        else
        {
            SiiDrvOsdEnable( true );
        }
        test++; // move to next test
        break;
#endif
    case   BTEST_IPV:
#if INC_OSD
        SiiDrvOsdEnable( false );
#endif
        PrintBenchTestName( BTEST_IPV );
        SiiIpvThemeSet( ipvTheme );
        SiiIpvEnable( true );
        test++; // move to next test
        break;
    case   BTEST_IPV_PIP:
        SkAppIpvPipModeEnable(SiiSwitchSource1);
//          while(1);
        test++; // move to next test
        break;
    case   BTEST_IV_COLORBAR:
        PrintBenchTestName( BTEST_IV_COLORBAR );
        app.newSource[0] = app.newSource[1] = SiiSwitchSourceInternalVideo;
        app.isTxHdcpRequired[0]     = false;
        SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_COLORBAR);
        app.newAudioSource[0]       = SiiSwitchAudioSourceDefault;
        app.newAudioSource[1]       = SiiSwitchAudioSourceDefault;
        test++; // move to next test
        break;
    case   BTEST_IV_CHESS:
        PrintBenchTestName( BTEST_IV_CHESS );
        app.newSource[0] = app.newSource[1] = SiiSwitchSourceInternalVideo;
        //app.isTxHdcpRequired[0]     = false;
        SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_CHESS);
        app.newAudioSource[0]       = SiiSwitchAudioSourceDefault;
        app.newAudioSource[1]       = SiiSwitchAudioSourceDefault;
        test++; // move to next test
        break;
    case   BTEST_IV_RAMP:
        PrintBenchTestName( BTEST_IV_RAMP );
        app.newSource[0] = app.newSource[1] = SiiSwitchSourceInternalVideo;
        //app.isTxHdcpRequired[0]     = false;
        SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_RAMP);
        app.newAudioSource[0]       = SiiSwitchAudioSourceDefault;
        app.newAudioSource[1]       = SiiSwitchAudioSourceDefault;
        test++; // move to next test
        break;
    case   BTEST_AUDIO_INSERTION:
        PrintBenchTestName( BTEST_AUDIO_INSERTION );
        // Clean up previous test
        app.newSource[0] = app.newSource[1] = SiiSwitchSource1;
        //app.isTxHdcpRequired[0]     = false;
        //SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_COLORBAR);
        app.newAudioSource[0]       = SiiSwitchAudioSourceExternal;
        app.newAudioSource[1]       = SiiSwitchAudioSourceExternal;
        SiiTxOutputInterfaceSet(SI_TX_OUTPUT_MODE_HDMI);
        // Set interface type and enable audio transmission
        SiiDrvTpiAudioInterfaceSet(SI_TX_AUDIO_SRC_SPDIF);
        test++;
        break;
    case   BTEST_AUDIO_INSERTION_MP:
        PrintBenchTestName( BTEST_AUDIO_INSERTION_MP );
        // Clean up previous test
        app.newSource[0] = app.newSource[1] = SiiSwitchSourceInternalVideo;
        app.isTxHdcpRequired[0]     = false;
        SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_COLORBAR);
        app.newAudioSource[0]       = SiiSwitchAudioSourceMainPipe;
        app.newAudioSource[1]       = SiiSwitchAudioSourceMainPipe;
        SiiTxOutputInterfaceSet(SI_TX_OUTPUT_MODE_HDMI);
        // Set interface type and enable audio transmission
        SiiDrvTpiAudioInterfaceSet(SI_TX_AUDIO_SRC_SPDIF);
        test++;
        break;
    case   BTEST_AUDIO_INSERTION_SP:
        PrintBenchTestName( BTEST_AUDIO_INSERTION_SP );
        // Clean up previous test
        app.newSource[0] = app.newSource[1] = SiiSwitchSourceInternalVideo;
        app.isTxHdcpRequired[0]     = false;
        SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_RAMP);
        app.newAudioSource[0]       = SiiSwitchAudioSourceSubPipe;
        app.newAudioSource[1]       = SiiSwitchAudioSourceSubPipe;
        SiiTxOutputInterfaceSet(SI_TX_OUTPUT_MODE_HDMI);
        // Set interface type and enable audio transmission
        SiiDrvTpiAudioInterfaceSet(SI_TX_AUDIO_SRC_SPDIF);
        test++;
        break;
    case   BTEST_AUDIO_INSERTION_EXT:
        PrintBenchTestName( BTEST_AUDIO_INSERTION_EXT );
        // Clean up previous test
        app.newSource[0] = app.newSource[1] = SiiSwitchSourceInternalVideo;
        app.isTxHdcpRequired[0]     = false;
        SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_CHESS);
        app.newAudioSource[0]       = SiiSwitchAudioSourceExternal;
        app.newAudioSource[1]       = SiiSwitchAudioSourceExternal;
        SiiTxOutputInterfaceSet(SI_TX_OUTPUT_MODE_HDMI);
        // Set interface type and enable audio transmission
        SiiDrvTpiAudioInterfaceSet(SI_TX_AUDIO_SRC_SPDIF);
        test++;
        break;
    case  BTEST_SK_MODE_TV:
        PrintBenchTestName( BTEST_SK_MODE_TV );
        SkAppTv();
        test++;
        break;
#endif
    default:
        PrintAlways( "BENCH TEST IS OVER.\n" );
        //   test = BTEST_IPV;
        //test = 0;
        break;
    }
    PrintHashLine();
}


