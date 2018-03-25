//***************************************************************************
//!file     sk_app_switch.c
//!brief    Wraps platform and device functions for the SWITCH component
//          and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"
#include "si_tx_component.h"
#include "si_repeater_component.h"

#include "si_drv_device.h"
#include "si_drv_tpi_system.h"
#include "si_drv_switch.h"
#include "si_drv_audio.h"

//#define   INC_RTPI 1
#if INC_RTPI
#include "si_rtpi_component.h"
#endif

#if INC_CBUS
#include "sk_app_cbus.h"
#endif
#include "si_tx_video.h"

#include "Mcu.h"
#include "uart.h"
#include "dvPC9539.h"
#include "si_timer.h"
void SkAppHdcpR0DelaySetting(  BYTE cInstance, uint8_t vic );

//-------------------------------------------------------------------------------------------------
//! @brief      Determine the next available HDMI/MHL input (circular) for display.
//!             Starts from the passed current port and skips any unconnected
//!             ports.
//!
//! @param[in]  currentPort - Rx port (0-3).
//! @param[in]  isForward   - Next port is given by increment (true), or decrement (false) of port's index
//!
//! @return     Next sequential active HDMI/MHL port.
//-------------------------------------------------------------------------------------------------
uint8_t SkAppSwitchGetNextPort ( BYTE cInstance, uint8_t currentPort, bool_t isForward )
{
    uint8_t i, stateValue;

    stateValue = SiiDrvSwitchStateGet(  cInstance, SiiSwitch_INPUT_PORTS_CONNECT_STATE );

    for ( i = 0; i < SII_INPUT_PORT_COUNT; i++ )
    {
        if (isForward)
        {
            currentPort = (currentPort + 1) % SII_INPUT_PORT_COUNT;
        }
        else
        {
            currentPort = (currentPort == 0) ? (SII_INPUT_PORT_COUNT - 1) : (currentPort - 1);
        }

            /* Only choose port if it is plugged in.    */

        if ( stateValue & ( 0x01 << currentPort))
        {
            break;
        }
    }

    return( currentPort );
}

#if 1
//powon or powoff +5v
//-------------------------------------------------------------------------------------------------
//! @brief      Determine the next available HDMI/MHL input (circular) for display.
//!             Starts from the passed current port and skips any unconnected
//!             ports.
//!
//! @param[in]  currentPort  onsignal   in the  output
//! @param[in]  currentPowoff  - Next port is given by increment (true), or decrement (false) of port's index
//!
//! 
//-------------------------------------------------------------------------------------------------
uint8_t SkAppPowOffPort ( BYTE cInstance, uint8_t currentPowoff )
{

    return 0;
}
#endif
//-------------------------------------------------------------------------------------------------
//! @brief      Submit routing settings to the switch hardware.
//-------------------------------------------------------------------------------------------------

static void UpdateFinalZoneConf( BYTE cInstance )
{
    uint8_t zone;
    bool_t txSourcechange[] = {false, false};
    SiiSwitchSource_t rptRxSource[2];
	SiiMswInstanceCommonData_t switchZoneInfo;

	SiiDrvSwitchZoneConfigGet( cInstance,&switchZoneInfo );

    for (zone = 0; zone < SII_NUM_TX; ++zone)
    {
        if (app[cInstance].newSource[zone] < SiiSwitchSourceInternalVideo)
        {
            rptRxSource[zone] = app[cInstance].newSource[zone];
        }
        else
        {
            // Enable repeater if audio comes from HDMI Rx
            // and if video source is of non-HDMI type
            if ((app[cInstance].newAudioSource[zone] == SiiSwitchAudioSourceMainPipe) ||
                (app[cInstance].newAudioSource[zone] == SiiSwitchAudioSourceSubPipe))
            {
                switchZoneInfo.pipeSel[zone] = (app[cInstance].newAudioSource[zone] == SiiSwitchAudioSourceMainPipe) ?
                                                 SiiSwitchHdmiPipeMain : SiiSwitchHdmiPipeSub;
                rptRxSource[zone] = app[cInstance].audioZoneSource[(app[cInstance].newAudioSource[zone] == SiiSwitchAudioSourceSubPipe)? 1:0];
                txSourcechange[zone] = true;
            }
            else
            {
                // Disable repeater if audio is not from HDMI Rx
                switchZoneInfo.pipeSel[zone] = SiiSwitchHdmiPipeNone;
            }
        }

        // Deactivate Repeater on disabled zones
        if (!app[cInstance].newZoneEnabled[zone])
        {
           switchZoneInfo.pipeSel[zone] = SiiSwitchHdmiPipeNone;
        }
    }

    // Tell repeater about the change
	SkAppRepeaterSourceConfig(  cInstance, rptRxSource, switchZoneInfo, txSourcechange );
	SiiDrvSwitchZoneConfigSet( cInstance );
}
//------------------------------------------------------------------------------
// Function:    UpdateResInfo
// Description: Display the resolution info for the port we're switching to.
//              If we were an actual AVR, we could use this information to prepare
//              our receiver hardware for the new format.
//------------------------------------------------------------------------------
//
//   输入分辨率变化后更新输出端的AVI, SWMING
//
static void UpdateResInfo (BYTE cInstance, int_t zone, SiiSwitchSource_t source )
{
    int_t  horiz, vert, tmdsClock = 0;
    bool_t isInterlaced, isHdmi;
    int_t  isMainPipe = SiiDrvSwitchZoneTrackingGet(cInstance) || (zone == 0);

    // Don't do it if in auto-switch mode, and can't handle internal video
    if (( app[cInstance].newSource[0] ) >= SII_INPUT_PORT_COUNT )
    {
        return;
    }

    if ( SiiDrvSwitchPortInfoGet( cInstance,source, isMainPipe, &tmdsClock, &horiz, &vert, &isInterlaced, &isHdmi ))
    {
       // 判断有信号
       DEBUG_PRINT( MSG_STAT, "app.isTxDisconnectedImmed =%d\n",app[cInstance].isTxDisconnectedImmed);
    	if((app[cInstance].isresolutionVaild == false)&&(app[cInstance].isTxDisconnectedImmed==true))
    	{ 
    		if (!SiiDrvTpiIsPoweredUp( cInstance ))
    		{
    			SiiDrvTpiPowerUp( cInstance );
    		}
    	}
        if(!app[cInstance].autoSwitch)
        {
            DEBUG_PRINT(MSG_STAT, asterisks);
            if ((horiz | vert) == 0)
            {
                DEBUG_PRINT(MSG_STAT, "** Zone %d: linked to %s source %d (Not CEA-861D) **\n",
                            zone + 1, isHdmi ? "" : "DVI", source);
            }
            else
            {
                DEBUG_PRINT(MSG_STAT, "** Zone %d: linked to %s source %d (%4d x %4d%c)\n"
                            "** Video Clock: %d\n",
                            zone + 1, isHdmi ? "" : "DVI",
                            source, horiz, vert,
                            isInterlaced ? 'i' : 'p', tmdsClock);
            }
            DEBUG_PRINT(MSG_STAT, asterisks);
//[kyq Begin: 20151028 13:40:40]

        if(horiz>100 && vert>100)
        {
//[kyq End: 20151028 13:40:41]
            app[cInstance].isresolutionVaild = true;
            //[kyq Begin: 20150813 16:02:22]
            app[cInstance].HorizPixel = horiz;
            app[cInstance].VertLine = vert;
            app[cInstance].TmdsClock = tmdsClock;
            app[cInstance].bInterlace = isInterlaced;
            //[kyq End: 20150813 16:02:23]
    //[kyq Begin: 20150916 12:28:32]
    #if PRD_NO_SIGNAL_TX_STANDBY
            if (SiiTxStatusGet(cInstance).isHotPlugDetected)
                //StartTx(cInstance, 0);
                SiiTxResume(cInstance);          // enable TMDS
    #endif
        }
    //[kyq Begin: 20151028 13:42:37]
        else
        {
              app[cInstance].isresolutionVaild = false;
      	      app[cInstance].HorizPixel = horiz;
      	      app[cInstance].VertLine = vert;
      	      app[cInstance].TmdsClock = tmdsClock;
              app[cInstance].bInterlace = false;
        #if PRD_NO_SIGNAL_TX_STANDBY
             SiiTxStandby(cInstance);
           //dvSii9533PowerControl(cInstance, APP_POWERSTATUS_ON_TO_STANDBY);// no input interrupt can't wakeup
        #endif
	    }
	if(SiiPlatformTimerGet(etPWON_5V_DELAY)==0)
	    Output5VConctrol(cInstance   ,app[cInstance].isresolutionVaild);//[hyj End: 20160517 14:25:15]
	    //Output5VConctrol(WORD OutputChannel  ,BYTE OnOff)//  1表示开   0表示低
        //[kyq End: 20151028 13:42:39]

        }
    }
    else if(!app[cInstance].autoSwitch)    //  判断无信号
    {
        DEBUG_PRINT( MSG_STAT, asterisks );
        DEBUG_PRINT( MSG_STAT, "** Zone %d: linked to source %d (unknown format)    **\n"
                               "** Video Clock: %d\n", zone + 1, source, tmdsClock );
        DEBUG_PRINT( MSG_STAT, asterisks );
        app[cInstance].isresolutionVaild = false;
//[kyq Begin: 20150813 16:06:19]
	 app[cInstance].HorizPixel = horiz;
	 app[cInstance].VertLine = vert;
	 app[cInstance].TmdsClock = tmdsClock;
       app[cInstance].bInterlace = false;
//[kyq End: 20150813 16:06:20]
  if(SiiPlatformTimerGet(etPWON_5V_DELAY)==0)
          Output5VConctrol(cInstance   ,app[cInstance].isresolutionVaild);//[hyj End: 20160517 14:25:15]
//[kyq Begin: 20150915 20:47:07]
#if PRD_NO_SIGNAL_TX_STANDBY
    SiiTxStandby(cInstance);
     //dvSii9533PowerControl(cInstance, APP_POWERSTATUS_ON_TO_STANDBY);// no input interrupt can't wakeup
#endif
//[kyq End: 20150915 20:47:09]

    }

#if ((INC_BENCH_TEST == SI_DISABLE) || (BENCH_TEST_STATIC_300MHZ == SI_DISABLE))
        // Provide pixel clock frequency info to the TX to let it adjust its TMDS termination
        if (source < SiiSwitchSourceInternalVideo)
        {
            uint8_t aAvi[18];
            uint8_t aviLen;

            SiiDrvSwitchInfoFrameGet( cInstance, isMainPipe, IF_AVI, aAvi, &aviLen);
            SkAppTxAdjustVideoSettings(cInstance,zone, aAvi, aviLen);

            SkAppTxAdjustAnalogSettings(cInstance,zone, tmdsClock);
        }

        //SkAppTxAdjustAudioClkDivider( cInstance, zone, tmdsClock);
#endif

}

//-------------------------------------------------------------------------------------------------
//! @brief      Detects new connection request to a non HDMI source (IV or PV) and
//!             configures TX and/or TPG accordingly.
//-------------------------------------------------------------------------------------------------
void SkAppServiceNonHdmiVideo( BYTE cInstance,bool_t forceResChange )
{
    uint8_t i;
    bool_t isZoneChange;
    bool_t isZoneEnable;
    bool_t isRepeaterModeChange = false;
    bool_t isEnterIv = ((app[cInstance].newSource[0] == SiiSwitchSourceInternalVideo) || (app[cInstance].newSource[1] == SiiSwitchSourceInternalVideo)) &&
                       ((app[cInstance].currentSource[0] != SiiSwitchSourceInternalVideo) && (app[cInstance].currentSource[1] != SiiSwitchSourceInternalVideo));
    bool_t isQuitIv  = ((app[cInstance].newSource[0] != SiiSwitchSourceInternalVideo) && (app[cInstance].newSource[1] != SiiSwitchSourceInternalVideo)) &&
                       ((app[cInstance].currentSource[0] == SiiSwitchSourceInternalVideo) || (app[cInstance].currentSource[1] == SiiSwitchSourceInternalVideo));

    isEnterIv = (forceResChange) ? true : isEnterIv;

    // TPG control

    if (isEnterIv || isQuitIv)
    {
#if INC_IV
    	SkAppTpgEnable(cInstance,isEnterIv);
#endif
    }

    // IV, PV and Repeater/Bypass mode control
    for (i = 0; i < SII_NUM_TX; ++i)
    {
        // Set Repeater mode and Bypass mode
        // Repeater must be enabled for Non HDMI video when accompanied audio comes from HDMI Rx
    	if(app[cInstance].repeaterEnable) // condition added to enable tx hdcp in non repeter mode, when isTxHdcpRequired is true.
    	{
    		isRepeaterModeChange = app[cInstance].isTxRepeaterMode[i]; // store old repeater mode flag
    		app[cInstance].isTxRepeaterMode[i] = ( app[cInstance].newSource[i] < SiiSwitchSourceInternalVideo) ||
    				(app[cInstance].newAudioSource[i] == SiiSwitchAudioSourceMainPipe) || (app[cInstance].newAudioSource[i] == SiiSwitchAudioSourceSubPipe);
    		isRepeaterModeChange = (app[cInstance].isTxRepeaterMode[i] != isRepeaterModeChange); // set the change flag
    	}
    	else
    	{
    		app[cInstance].isTxRepeaterMode[i] = false;
    	}
	//[kyq Begin: 20150915 15:16:17]
	//test only
    	app[cInstance].isTxBypassMode[i] =  (app[cInstance].newSource[i] < SiiSwitchSourceInternalVideo);
    	//[kyq End: 20150915 15:16:20]

        // Service IV
        // Find out if Zone switches to or from the video source (change detection)
        isZoneChange = forceResChange || isRepeaterModeChange ||
                       ((app[cInstance].currentSource[i] != SiiSwitchSourceInternalVideo) && (app[cInstance].newSource[i] == SiiSwitchSourceInternalVideo)) ||
                       ((app[cInstance].currentSource[i] == SiiSwitchSourceInternalVideo) && (app[cInstance].newSource[i] != SiiSwitchSourceInternalVideo));
        // Find out if Zone is connected to the video source (status detection)
        isZoneEnable = (app[cInstance].newSource[i] == SiiSwitchSourceInternalVideo);
        if (isZoneChange)
        {
#if INC_IV
            SkAppInternalVideoConfig(cInstance,i, isZoneEnable);
#endif
        }
    }

}

#if 0
//-------------------------------------------------------------------------------------------------
//! @brief      Enable OSD Background video.
//!
//!             OSD BG Video is 480p blue screen using internal clock source
//!             and required no HDCP. Main use is for OSD menus to be visible
//!             when no active video source is connected.
//-------------------------------------------------------------------------------------------------

static void OsdBackgroundVideoEnable( BYTE cInstance, bool_t isEnable)
{
    if (isEnable)
    {
#if INC_IV
        // Memorize current source selection and VPG settings
       app[ cInstance ].tpgClockSrc = SI_TPG_CLK_XCLK;
       app[ cInstance ].tpgVideoPattern = SI_ETPG_PATTERN_BLUE;
       app[ cInstance ].tpgVideoFormat = SI_TPG_FMT_VID_480_60;
#endif
       app[ cInstance ].isTxHdcpRequired[0] = true;

//
      app[ cInstance ].newSource[0]    = SiiSwitchSourceInternalVideo;
    }
    else
    {
#if INC_IV
       app[ cInstance ].tpgClockSrc = SI_TPG_CLK_MAIN_PIPE;
#endif
       app[ cInstance ].isTxHdcpRequired[0] = true;
       app[ cInstance ].newSource[0] = (SiiSwitchSource_t)SiiDrvSwitchStateGet( cInstance, SiiSwitch_SELECTED_PORT);
    }

    DEBUG_PRINT( MSG_DBG, "OSD BG Mode %s\n", isEnable ? "ON" : "OFF");
}
#endif
//------------------------------------------------------------------------------
// Function:    SkAppSwitchPortUpdate
// Description: Calls the appropriate Silicon Image functions for switching
//              to a new port.
//------------------------------------------------------------------------------

void SkAppSwitchPortUpdate ( BYTE cInstance )
{
    static int ccnt = 0;

    uint8_t i, numZones = SII_NUM_SWITCH;
#if (SII_NUM_SWITCH > 1)
    bool_t  reenableOutput[SII_NUM_SWITCH] = {false, false};
    bool_t  isZoneSourceChanged[SII_NUM_SWITCH] = {false, false};
    bool_t  isQuitMatrix, isEnterMatrix, isZoneEnableChange;
    bool_t  isAudioOvlChange[SII_NUM_SWITCH] = {false, false};
#else
    bool_t  reenableOutput[SII_NUM_SWITCH] = {false};
    bool_t  isZoneSourceChanged[SII_NUM_SWITCH] = {false};
    bool_t  isZoneEnableChange= false;
    bool_t  isAudioOvlChange[SII_NUM_SWITCH] = {false};
#endif
    // Force common source selection if in zone tracking mode
    if (SiiDrvSwitchZoneTrackingGet(cInstance))
    {
        app[cInstance].newSource[1] = app[cInstance].newSource[0];
    }

    // Prepare important conditions
    isAudioOvlChange[0] =(bool_t)( (app[cInstance].currentAudioSource[0] != app[cInstance].newAudioSource[0])||(app[cInstance].currentaudExtSource != app[cInstance].newaudExtSource));
#if (SII_NUM_SWITCH > 1)
    isAudioOvlChange[1] = (app[cInstance].currentAudioSource[1] != app[cInstance].newAudioSource[1]);
#endif
    isZoneSourceChanged[0] = (app[cInstance].currentSource[0] != app[cInstance].newSource[0]) || isAudioOvlChange[0];
#if (SII_NUM_SWITCH > 1)
#if INC_IPV
    if(!app[cInstance].ipvPipEnabled)
    	isZoneSourceChanged[1] = (app[cInstance].currentSource[1] != app[cInstance].newSource[1]) || isAudioOvlChange[1];
    else
    {
    	isZoneSourceChanged[1] = false;
        numZones = 1;
    }
#else
    isZoneSourceChanged[1] = (app[cInstance].currentSource[1] != app[cInstance].newSource[1]) || isAudioOvlChange[1];
#endif
#endif
#if (SII_NUM_SWITCH > 1)
    isQuitMatrix = (app[cInstance].currentSource[0] != app[cInstance].currentSource[1]) && (app[cInstance].newSource[0] == app[cInstance].newSource[1]);
    isEnterMatrix = (app[cInstance].currentSource[0] == app[cInstance].currentSource[1]) && (app[cInstance].newSource[0] != app[cInstance].newSource[1]);
#endif

#if (SII_NUM_SWITCH > 1)
    isZoneEnableChange = (app[cInstance].newZoneEnabled[0] != app[cInstance].zoneEnabled[0]) || (app[cInstance].newZoneEnabled[1] != app[cInstance].zoneEnabled[1]) ;
#else
    isZoneEnableChange = (app[cInstance].newZoneEnabled[0] != app[cInstance].zoneEnabled[0]);
#endif

#if (SII_NUM_SWITCH > 1)
    // Re-initialize EDID Tx Component whenever topology changes
    if (isEnterMatrix)
    {
        // Into matrix mode
        SkAppDeviceInitEdidTx(app[cInstance].isAudioByPass);
    }
    else if (isQuitMatrix)
    {
        // Out of matrix mode
        SkAppDeviceInitEdidTx(app[cInstance].isAudioByPass);
    }
#endif
    // Check Zone 1 & 2 for change
    for (i = 0; i < numZones; ++i)
    {
        if (isZoneSourceChanged[i])
        {
            reenableOutput[i] = app[cInstance].newZoneEnabled[i];
            SiiDrvSwitchInstanceSet( cInstance,  i );

            // The new Audio Zone Source is defined by transmitter zone source
            // selection unless the TX zone source is of non-TMDS type
            if (app[cInstance].newSource[i] >= SiiSwitchSourceInternalVideo)       // Non-TMDS source
            {
#if INC_IV
		if ( !app[cInstance].isOsdBackgroundMode )
#else
              if(true)
#endif
                {
			SiiDrvSwitchAudioZoneSourceSet( cInstance, SiiDrvSwitchZoneTrackingGet( cInstance ) ?
					                                                        app[cInstance].audioZoneSource[0] : app[cInstance].audioZoneSource[i]);
                }
                else
                {
                    // Do not change audio zone source if in background mode because this may
                    // cause the video source to be switched to whatever source was associated
                    // with the audio source.
                }
            }
            else
            {
                // If a TMDS source, the audio source is the same as the video source.
               #if Uart_Debug
			   UART1_SendBytes("TMDS source\r\n",strlen("TMDS source\r\n"));
			   #endif
			   SiiDrvSwitchAudioZoneSourceSet(cInstance,app[cInstance].newSource[i]);
            }

            SiiDrvSwitchSourceSelect( cInstance,app[cInstance].newSource[i]);

            // Force audio change as well, if Video source has changed
            isAudioOvlChange[i] = true;
        }

        if(isAudioOvlChange[i])
        {
#if INC_IPV
            // In PIP mode make sure audio is inserted from sub pipe, not from main pipe
            if ((app[cInstance].newAudioSource[i] == SiiSwitchAudioSourceMainPipe) && app[cInstance].ipvPipEnabled)
            {
                app[cInstance].newAudioSource[i] = SiiSwitchAudioSourceSubPipe;
            }
#endif
//            // In VPG & PV modes make sure audio is inserted from main pipe, not from sub pipe
//            if ((app.newSource[i] >= SiiSwitchSourceInternalVideo) && (app.newAudioSource[i] == SiiSwitchAudioSourceSubPipe))
//            {
//                app.newAudioSource[i] = SiiSwitchAudioSourceMainPipe;
//            }
        }
    }

    // Do actual switching and inform repeater about the change
#if INC_IPV
    if ( !app[cInstance].ipvPipEnabled )
#endif
    {
        if ((isZoneSourceChanged[0] ||
#if (SII_NUM_SWITCH > 1)
        		isZoneSourceChanged[1] ||
#endif
        		isZoneEnableChange))
        {
	     SiiDrvSwitchConfigureMatrixSwitch( cInstance,false, false );
            UpdateFinalZoneConf(cInstance);
        }
    }


    // If either zone was changed there is more work to do
    if ( reenableOutput[0]
#if (SII_NUM_SWITCH > 1)
    		 || reenableOutput[1]
#endif
    		               )
    {
#if INC_IPV
        if ( app[cInstance].ipvPipEnabled )
        {
            if ( SkAppIpvPipModeEnable( app[cInstance].pipSource ))
            {
                app[cInstance].ipvPipEnabled = true;
                app[cInstance].ipvEnabled = false;
            }
        }
#endif
        if ( !app[cInstance].autoSwitch )
        {
            // Zone 1 changes get reported via CEC and get displayed on OSD
            if ( reenableOutput[0] )
            {
#if INC_CEC
                if ( ( app[cInstance].newSource[0] ) < SII_INPUT_PORT_COUNT )
                {
                    SkAppCecInstanceSet(  cInstance, CEC_INSTANCE_AVR );
                    SiiCecAppInputPortChangeHandler( cInstance, app[cInstance].newSource[0]);
                }
#endif
                DEBUG_PRINT( MSG_DBG,"Zone 1: Switch to source %d complete\n", app[cInstance].newSource[0] );
            }
        }
        else
        {
            if (( ccnt % 30 ) == 0 )
            {
                DEBUG_PRINT( MSG_DBG, "\n" );
            }
            DEBUG_PRINT( MSG_DBG, "%d", app[cInstance].newSource[0]);
            ccnt++;
        }
		#if Uart_Debug
		UART1_SendBytes("reenableOutput\r\n",strlen("reenableOutput\r\n"));
		#endif
        // Control Internal Video Generator and TXs, if necessary
        SkAppServiceNonHdmiVideo( cInstance, false);

        app[cInstance].currentSource[0] = app[cInstance].newSource[0];
#if (SII_NUM_SWITCH > 1)
        app[cInstance].currentSource[1] = app[cInstance].newSource[1];
#endif
        app[cInstance].currentaudExtSource = app[cInstance].newaudExtSource;
    }

    // Service Audio switchings
#if (SII_NUM_SWITCH > 1)
    if(isAudioOvlChange[0] || isAudioOvlChange[1])
#else
    if(isAudioOvlChange[0])
#endif
    {
        // Audio Overlay processing
        for (i = 0; i < numZones; ++i)
        {
            if (isAudioOvlChange[i])
            {
                bool_t isTmdsSource = app[cInstance].currentSource[i] < SiiSwitchSourceInternalVideo;

                SkAppTxInstanceSet(cInstance, i);
                SiiDrvRxAudioMixInstanceSet(cInstance,i);

                switch(app[cInstance].newAudioSource[i])
                {
                    case SiiSwitchAudioSourceMainPipe:
						#if Uart_Debug
						UART1_SendBytes("AudioSourceMainPipe\r\n",strlen("AudioSourceMainPipe\r\n"));
                        #endif
						SiiDrvTpiAudioRxSource( cInstance, true, true);
                        // Copy AIF to TX
                        SkAppTxCopyAif( cInstance, i, true);
                        SiiDrvRxAudioInstanceSet( cInstance, 0);
                        SiiDrvRxAudioFifoRedirSet( cInstance, true);
                        // Disable bypassing audio packet by audio mixer
                        SiiDrvRxAudioMixPassThroughConfig(cInstance, false, isTmdsSource);
                        SiiTxInputAudioUnmute( cInstance );
                        // Switch audio MUX
                        SiiDrvSwitchInstanceSet( cInstance , i);
                        SiiDrvSwitchAudioSourceSelect(cInstance,app[cInstance].newAudioSource[i], app[cInstance].audioZoneSource[i]);
                        break;

                    case SiiSwitchAudioSourceSubPipe:
			#if Uart_Debug
						UART1_SendBytes("AudioSourceSubPipe\r\n",strlen("AudioSourceSubPipe\r\n"));
                        #endif
			   SiiDrvTpiAudioRxSource( cInstance, true, false);
                        // Copy AIF to TX
                        SkAppTxCopyAif( cInstance, i, false);
                        SiiDrvRxAudioInstanceSet( cInstance, 1);
                        SiiDrvRxAudioFifoRedirSet( cInstance, true);
                        // Disable bypassing audio packet by audio mixer
                        //SiiDrvRxAudioMixPassThroughConfig(cInstance, true, isTmdsSource);  // org code
                        SiiDrvRxAudioMixPassThroughConfig(cInstance,app[cInstance].bHDMIAudioEn,isTmdsSource);
                        SkAppExtAudioInsertConfig(cInstance,i, false);
                        SiiTxInputAudioUnmute( cInstance );
                        // Switch audio MUX
                        SiiDrvSwitchInstanceSet( cInstance, i);
                        SiiDrvSwitchAudioSourceSelect(cInstance,app[cInstance].newAudioSource[i], app[cInstance].audioZoneSource[i]);
                        break;

                    case SiiSwitchAudioSourceExternal:
						#if Uart_Debug
						UART1_SendBytes("AudioSourceExternal\r\n",strlen("AudioSourceExternal\r\n"));
                        #endif
						SiiDrvTpiAudioRxSource( cInstance, false, true);
                        SiiDrvRxAudioInstanceSet( cInstance, app[cInstance].currentAudioSource[i] == SiiSwitchAudioSourceSubPipe);
                        SiiDrvRxAudioFifoRedirSet( cInstance, false);
                        if(isTmdsSource)
                        {
                            // Allow video pass through, but audio will be local
                            SiiDrvRxAudioMixPassThroughConfig(cInstance, false, true);
                            SkAppExtAudioInsertConfig(cInstance,i, true);
                        }
                        else
                        {
                            // Both audio and video are local
                            SiiDrvRxAudioMixPassThroughConfig(cInstance, false, false);
                        }
                        break;

                    default:
						#if Uart_Debug
						UART1_SendBytes("AudioSourceDefault\r\n",strlen("AudioSourceDefault\r\n"));
                        #endif
						SiiDrvTpiAudioRxSource( cInstance, false, true);
                        SiiDrvTpiAudioInEnable( cInstance );
                        SiiDrvRxAudioInstanceSet( cInstance, app[cInstance].currentAudioSource[i] == SiiSwitchAudioSourceSubPipe);
                        SiiDrvRxAudioFifoRedirSet( cInstance, false);
                        if(isTmdsSource)
                        {
                            //SiiDrvRxAudioMixPassThroughConfig(cInstance, true, true);// org code
                            SiiDrvRxAudioMixPassThroughConfig(cInstance,app[cInstance].bHDMIAudioEn,true);
                            SkAppExtAudioInsertConfig(cInstance,i, false);
                        }
                        else
                        {
                            SiiDrvRxAudioMixPassThroughConfig(cInstance, false, false);
                        }
                        // Switch audio MUX
                        SiiDrvSwitchInstanceSet(cInstance,i);
                        SiiDrvSwitchAudioSourceSelect(cInstance,app[cInstance].newAudioSource[i], app[cInstance].audioZoneSource[0]);
                        break;
                }
            }
        }

        app[cInstance].currentAudioSource[0] = app[cInstance].newAudioSource[0];
#if (SII_NUM_SWITCH > 1)
        app[cInstance].currentAudioSource[1] = app[cInstance].newAudioSource[1];
#endif
    }

    if ( isZoneEnableChange )
    {
        #if Uart_Debug
		UART1_SendBytes("isZoneEnableChange\r\n",strlen("isZoneEnableChange\r\n"));
		#endif
		app[cInstance].zoneEnabled[0] = app[cInstance].newZoneEnabled[0];
#if (SII_NUM_SWITCH > 1)
        app[cInstance].zoneEnabled[1] = app[cInstance].newZoneEnabled[1];
#endif
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Initialize both instances of the Matrix Switch
//-------------------------------------------------------------------------------------------------

bool_t SkAppDeviceInitMsw (  BYTE cInstance )
{

    SiiDrvSwitchInstanceSet( cInstance , 0 );
    SiiDrvSwitchInitialize( cInstance );
#if (SII_NUM_SWITCH > 1)
    SiiDrvSwitchInstanceSet( 1 );
    SiiDrvSwitchInitialize();
#endif
    return( true);
}

//------------------------------------------------------------------------------
// Function:    SkAppTaskSwitch
// Description: Switch control task.  Emulate a task with a blocking semaphore
//              waiting for a device interrupt.
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------
//
//  RX 端的一些中断处理
//
void SkAppTaskSwitch ( BYTE cInstance )
{
    uint8_t     ifMpFlags;
    uint8_t     ifSpFlags;
    uint16_t    statusFlags;
    uint8_t     portConnFlags;
    uint8_t		selectedPort;

    // Check the application Switch interrupt flag to see if
    // anything happened that involves the switch
    if ( app[cInstance].switchInterrupt )     // SET BY SkAppTaskSiiDevice 
    {
        statusFlags = SiiDrvSwitchStatus( cInstance,SiiSWITCH_INTFLAGS );

    	if(statusFlags & SiiSWITCH_CABLE_CHANGE)
    	{
    		SiiDrvSwitchPortsProcessTermCtrl( cInstance );
    		portConnFlags = SiiDrvSwitchStateGet( cInstance, SiiSwitch_INPUT_PORTS_CONNECT_STATE);
    		DEBUG_PRINT( MSG_DBG,"Rx Port Cable: %d%d%d%d\n", (portConnFlags & BIT3) != 0, (portConnFlags & BIT2) != 0,
    		                                                   (portConnFlags & BIT1) != 0, (portConnFlags & BIT0) != 0);
#if INC_RTPI
    		SiiRtpiPortConnect( cInstance, portConnFlags);
#endif
    	}

        // Info frame change checks
        if ( SiiDrvSwitchIfInterruptStatusGet( cInstance,&ifMpFlags, &ifSpFlags ))
        {
            if ( ifMpFlags & (SiiSwitch_NO_AIF | SiiSwitch_NO_AVI | SiiSwitch_NO_VSI))
            {
                DEBUG_PRINT( MSG_DBG, "MP: Missing InfoFrames: %02X\n", ( ifMpFlags & (SiiSwitch_NO_AIF | SiiSwitch_NO_AVI | SiiSwitch_NO_VSI)));
            }
            if ( ifMpFlags & (SiiSwitch_NEW_AIF | SiiSwitch_NEW_AVI | SiiSwitch_NEW_VSI))
            {
                //DEBUG_PRINT( MSG_ALWAYS, "VSIF new Frames recognised\n");
                DEBUG_PRINT( MSG_DBG, "MP: Changed InfoFrames: %02X\n", ( ifMpFlags & (SiiSwitch_NEW_AIF | SiiSwitch_NEW_AVI | SiiSwitch_NEW_VSI)));
                SkAppTxInfoFrameUpdate(cInstance,true, ifMpFlags & SiiSwitch_NEW_AVI, ifMpFlags & SiiSwitch_NEW_AIF,  ifMpFlags & SiiSwitch_NEW_VSI);
            }
            if ( ifSpFlags & (SiiSwitch_NEW_AIF | SiiSwitch_NEW_AVI | SiiSwitch_NEW_VSI))
            {
                DEBUG_PRINT( MSG_DBG, "SP: Changed InfoFrames: %02X\n", ( ifSpFlags & (SiiSwitch_NEW_AIF | SiiSwitch_NEW_AVI | SiiSwitch_NEW_VSI)));
#if INC_IPV
                // SWWA: 23593 - Reset IPV scaler when AVI in sub-pipe changes
                if (app[cInstance].ipvPipEnabled)
                {
                    // Re-enabling restarts down-scaler
                    SiiIpvRefresh();
                }
#endif
                SkAppTxInfoFrameUpdate(cInstance,false, (bool_t)(ifSpFlags & SiiSwitch_NEW_AVI), ifSpFlags & SiiSwitch_NEW_AIF,  ifSpFlags & SiiSwitch_NEW_VSI);
            }
			if( (ifMpFlags & SiiSwitch_NO_AVI) || (ifMpFlags & SiiSwitch_NEW_AVI))
			{
				uint8_t avi[18], len;
				len = sizeof(avi);
				SiiDrvSwitchInfoFrameGet( cInstance, (bool_t)1,IF_AVI,avi,&len);
				// avi[4] bit6, bit5 for Y1 and Y0 of color value
				//    2'b00 = RGB
				//    2'b01 = YCbCr422
				//    2'b10 = YCbCr444
				//    2'b11 = Reserved
				//
				// avi[8] is the VIC value
				//    Low frame rate VICs are
				//    60: 720p 24Hz
				//    61: 720p 25Hz
				//    32: 1080p 24Hz
				//    33: 1080p 25Hz
				// setup TPI R0 delay setting
			//	TxVideoBlankingLevelsSet( (((avi[4]>>5) & 0x03) != 0)? 1 : 0, 1); // always full range. because we have bug in blank register setting

				SkAppHdcpR0DelaySetting( cInstance,  avi[8] );

			}
        }

        app[cInstance].switchInterrupt = false;
    }

    if(app[cInstance].resChangeIntr[0]) //MP
    {
        UpdateResInfo( cInstance,  0, app[cInstance].newSource[0] );
        //UpdateResInfo( 1, app.newSource[1] );
        app[cInstance].resChangeIntr[0] = false;
#if INC_OSD
        SkAppOsdShowInfo();
        if ( app[cInstance].isOsdMenuEnabled )
        {
            // Conditions may have changed for currently displayed menu
            // so check for updates.
            SkAppOsdMenuUpdate();
        }
#endif
    }

    selectedPort = SiiDrvSwitchStateGet( cInstance, SiiSwitch_SELECTED_PORT);
    // Check if OSD background is required in MP
   /* if(!SiiDrvSwitchStateGet(SiiSwitch_SELECTED_INPUT_CONNECT_STATE) && !SkAppCbusIsMhlCableConnected(selectedPort))
    {
        // Only apply OSD BG mode if video source selection is of TMDS type
        if (app.currentSource[0] < SiiSwitchSourceInternalVideo)
        {
            app.isOsdBackgroundMode = true;
            OsdBackgroundVideoEnable(true);
        }
        // For IV mode, only switch to OSD BG if APLL1/720p mode is in use
        else if ((app.currentSource[0] == SiiSwitchSourceInternalVideo) && (app.tpgClockSrc == SI_TPG_CLK_P1))
        {
            app.isOsdBackgroundMode = true;
            OsdBackgroundVideoEnable(true);
            // Change source to TMDS port, this will trigger OSD BG mode update.
            // Also, the IV mode will be off when port clock returns
            app.newSource[0] = SkAppSwitchGetNextPort(1, false);
        }
    }
    else
    {
    	if((selectedPort == SI_MHL_PORT0) || (selectedPort == SI_MHL_PORT1))
		{
    		if(SkAppCbusIsMhlCableConnected(selectedPort))
    		{
				if(SkAppSourceIsConnectedCbusPort(selectedPort) && app.isOsdBackgroundMode)
				{
					// Quit OSD background mode
					app.isOsdBackgroundMode = false;
					OsdBackgroundVideoEnable(false);
				}
				if(!SkAppSourceIsConnectedCbusPort(selectedPort) && !app.isOsdBackgroundMode)
				{
					// Quit OSD background mode
					app.isOsdBackgroundMode = true;
					OsdBackgroundVideoEnable(true);
				}
    		}
    		else if(app.isOsdBackgroundMode)
			{
				// Quit OSD background mode
				app.isOsdBackgroundMode = false;
				OsdBackgroundVideoEnable(false);
			}
		}
    	else if(app.isOsdBackgroundMode)
        {
            // Quit OSD background mode
            app.isOsdBackgroundMode = false;
            OsdBackgroundVideoEnable(false);
        }
    } */
}

void SkAppHdcpR0DelaySetting( BYTE cInstance,  uint8_t vic )
{
	DEBUG_PRINT(MSG_DBG,"\n\n\r SkAppHdcpR0DelaySetting = vic:%x\n\n\r",vic);
	//	  60: 720p 24Hz
	//	  61: 720p 25Hz
	//	  32: 1080p 24Hz
	//	  33: 1080p 25Hz
	if( vic == 0 ){ // in case of DVI or HDMI 4k video, set to absolute time.
		SiiDrvTpiHdcpR0CalcConfigSet(cInstance, false);
		return;
	}
	if( vic<=31 ){ // most 60Hz and 50Hz
		SiiDrvTpiHdcpR0CalcConfigSet(cInstance, true);
		return;
	}
	if( vic<=34 ){ // 1080p 24, 25, 30 Hz
		SiiDrvTpiHdcpR0CalcConfigSet(cInstance, false);
		return;
	}
	if( vic<=59 ){ // high frequencies
		SiiDrvTpiHdcpR0CalcConfigSet(cInstance, false);
		return;
	}
	if( vic<=62 ){ // low frequency staff
		SiiDrvTpiHdcpR0CalcConfigSet(cInstance, false);
		return;
	}
	if( vic<=64 ){ // high frequency staff
		SiiDrvTpiHdcpR0CalcConfigSet(cInstance, false);
		return;
	}
	// others
	SiiDrvTpiHdcpR0CalcConfigSet(cInstance, false);


	return;
}

