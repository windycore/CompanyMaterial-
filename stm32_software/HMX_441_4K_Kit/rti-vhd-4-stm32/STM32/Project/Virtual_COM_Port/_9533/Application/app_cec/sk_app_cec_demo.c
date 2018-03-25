//***************************************************************************
//!file     sk_app_cbus_demo.c
//!brief
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_app_cec.h"
#include "si_edid_tx_component.h"


//------------------------------------------------------------------------------
// Function:    DemoCbus
// Description: CBUS demo command options
// Returns:
//------------------------------------------------------------------------------

static char *pCecDemoMenu [] =
{
    "\nCEC Demo",
    tenDashes,
    ccDisplayThisMenuMsg,
    returnToMainDemoMsg,
    "1 - Configure as PLayback device",
    "2 - Configure as Audio System",
#if INC_CEC_SWITCH
    "3 - Configure as Audio System and Switch",
#endif
#if INC_CEC_SAC
    "4 - Configure as Audio System and SAC",
#endif
#if INC_CEC_SWITCH && INC_CEC_SAC
    "5 - Configure as Ausio sytem with swith and SAC"
#endif
    "6 - Configure as TV",
    "7 - One Touch Play",
    NULL
};

void CecConfDeviceType ( BYTE cInstance, SiiCecDeviceTypes_t devicetype, SiiCecLogicalAddresses_t deviceLogicalAddr, bool_t isCecAsSwitch, bool_t isAudioSysWithSac)
{
    uint16_t 	cecPa;
    SkAppCecInstanceSet(  cInstance, CEC_INSTANCE_AVR );
    SiiEdidTxInstanceSet( cInstance, 0);
    SkAppTxInstanceSet( cInstance, 0);
    // Can't initialize if valid physical address not assigned
    cecPa = SiiEdidTxCecPhysAddrGet( cInstance, );
    if (cecPa == 0xFFFF || !SiiCecInitialize( cInstance,  cecPa, devicetype ))
    {
        PrintAlways( "\nERROR: CEC device type could not be set \n" );
        return;
    }
    SiiCecSetDeviceLA( cInstance, deviceLogicalAddr);
#if INC_CEC_SWITCH
    if(isCecAsSwitch)
    {
       	// Initialize CEC Switch component and enable its base service
        SiiCecSwitchConfig();
        if ( !SiiCecCallbackRegisterParser(  cInstance, SiiCecSwitchTaskProcess, false ))
        {
            PrintAlways( "\nERROR: CEC switch task registering failed \n" );
            return;
        }
        if ( SiiCecSwitchSrvStart())
        {
            PrintAlways( "\nERROR: Launching CEC/Switch Active and Base Services \n" );
        }
    }
#endif
#if INC_CEC_SAC
    if(isAudioSysWithSac)
    {
        // Initialize CEC System Audio Control component and enable its base service
        // Can't initialize SAC if valid physical address not assigned
        SiiCecSacConfig(0, NULL, 0); // take default format list
        if ( !SiiCecCallbackRegisterParser( cInstance,  SiiCecSacTaskProcess, false ))
        {
            PrintAlways( "\nERROR: CEC SAC task registering failed \n" );
            return;
        }
        if ( SiiCecSacSrvStart())
        {
            PrintAlways( "\nERROR: Launching CEC/SAC Active and Base Services \n" );
        }
    }
#endif
}

bool_t SkAppCecDemo ( RC5Commands_t key )
{

    switch ( key )
    {
        case RC_CC_SUBTITLE:
            PrintMenu( pCecDemoMenu );
            break;

        case RC5_0:
            app.demoMenuLevel = 0;
            PrintAlways( demoMenuExit );
            break;

        case RC5_1:
         	 // Get our DEV CAPS
            PrintAlways( "\n\nCEC BEING CONFIGURED AS PLAYBACK DEVICE \n" );
            CecConfDeviceType(CEC_DT_PLAYBACK,CEC_LOGADDR_PLAYBACK2, false, false);
            break;

        case RC5_2:
            PrintAlways( "\n\nCEC BEING CONFIGURED AS AUDIO SYSTEM \n" );
            CecConfDeviceType(CEC_DT_AUDIO_SYSTEM,CEC_LOGADDR_AUDSYS,false, false);
            break;

        case RC5_3:
            PrintAlways( "\n\nCEC BEING CONFIGURED AS AUDIO SYSTEM AND SWITCH \n" );
            CecConfDeviceType(CEC_DT_AUDIO_SYSTEM,CEC_LOGADDR_AUDSYS,true, false);
            break;

        case RC5_4:
            PrintAlways( "\n\nCEC BEING CONFIGURED AS AUDIO SYSTEM AND SAC \n" );
            CecConfDeviceType(CEC_DT_AUDIO_SYSTEM,CEC_LOGADDR_AUDSYS,false, true);
            break;

        case RC5_5:
            PrintAlways( "\n\nCEC BEING CONFIGURED AS AUDIO SYSTEM WITH SWITCH AND SAC \n" );
            CecConfDeviceType(CEC_DT_AUDIO_SYSTEM,CEC_LOGADDR_AUDSYS,true, true);
			break;

        case RC5_6:
            PrintAlways( "\n\nCEC BEING CONFIGURED AS TV \n" );
            CecConfDeviceType(CEC_DT_TV,CEC_LOGADDR_TV,true, true);
			break;

        case RC5_7:
            PrintAlways( "\n\nSENDING ONETOUCH PLAY COMMAND \n" );
            app.txSendOneTouch = true;
            break;
        default:
            break;
    }

    return( false );
}

