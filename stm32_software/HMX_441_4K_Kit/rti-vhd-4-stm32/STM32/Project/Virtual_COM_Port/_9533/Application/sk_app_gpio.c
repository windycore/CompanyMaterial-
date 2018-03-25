//***************************************************************************
//!file     sk_app_gpio.c
//!brief
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#if INC_CBUS
#include "sk_application.h"
#include "sk_app_cbus.h"
#include "si_drv_gpio.h"
#include "si_gpio.h"
//------------------------------------------------------------------------------
// Function:    SkAppDeviceInitGpio
// Description: Perform any board-level initialization required at the same
//              time as GPIO component initialization
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

bool_t SkAppDeviceInitGpio ( void )
{
    return( SiiDrvGpioInitialize() );
}

//------------------------------------------------------------------------------
// Function:    SkAppTaskGpio
// Description: Wrapper for the GPIO Component at the application level
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

void SkAppTaskGpio ( uint8_t instance )
{
    uint8_t     pinState[2];
    uint16_t    status;
    status = SiiDrvGpioStatus( cInstance );    // Determine who triggered the interrupt
    if ( status || (instance == 1) )
    {
        pinState[0] = SiiDrvGpioPinRead( SII_GPIO_PIN_0 );
        pinState[1] = (SiiPlatformMHLCDSensePinGet( cInstance,  1 )? SiiPortType_HDMI : SiiPortType_MHL);
//        pinState[1] = SiiDrvGpioPinRead( SII_GPIO_PIN_1 );
        if ( pinState[instance] )
        {
            //DEBUG_PRINT( MSG_DBG, "SkAppTaskGpio:: MHL channel 0 cable connect %s.\n", ( status & SII_GPIO_PIN_0 ) ? "true" : "false" );
            pAppCbus->cbusEnable = true;
        }
        else
        {
            //DEBUG_PRINT( MSG_DBG, "SkAppTaskGpio:: MHL channel 1 cable connect %s.\n", ( status & SII_GPIO_PIN_1 ) ? "true" : "false" );
            pAppCbus->cbusEnable = false;
        }
    }
}

#else
char appGpio;   // Avoids compiler warning of empty translation unit
#endif          // #if INC_CBUS == 1

