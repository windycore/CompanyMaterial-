//***************************************************************************
//!file     si_platform.c
//!brief    Silicon Image SiI9535 Starter Kit board platform.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_common.h"
#include "si_platform.h"
#include "si_eeprom.h"
#include "si_gpio.h"
#include "si_timer.h"
//#include "si_uartstdio.h"
//#include "si_usb_serial.h"
#include "si_i2c.h"
//#include "si_infrared.h"
//#include "si_flash_update.h"

//#include "si_arm.h"

//------------------------------------------------------------------------------
//  Board Component Instance Data
//------------------------------------------------------------------------------
/*
BoardInstanceData_t boardInstance =
{
    0,                          // structVersion
    PLATFORM_SUCCESS,//SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
    false,                      // i2CError;
    false,                      // i2cErrorCode
    MSG_PRINT_ALL,              // dbgMsgLevel

    false,                      // i2c16bitAddr
    false,                      // i2cNoInterimStops
    false,                      // hardwareInt
};
*/
BoardInstanceData_t platform;
/*
static const uint32_t l_debounceTimes [ 16 ] =
{
    DEBOUNCE_0, DEBOUNCE_1, DEBOUNCE_2, DEBOUNCE_3,
    DEBOUNCE_4, DEBOUNCE_5, DEBOUNCE_6, DEBOUNCE_7,
    DEBOUNCE_8, DEBOUNCE_9, DEBOUNCE_10, DEBOUNCE_11,
    DEBOUNCE_12, DEBOUNCE_13, DEBOUNCE_14, DEBOUNCE_15
};
*/
//------------------------------------------------------------------------------
// Function:    SiiPlatformPowerSwitchGet
// Description: Detect the state of the power pushbutton switch.
// Parameters:
// Returns:     true if state has changed. false if state has not been changed.
//------------------------------------------------------------------------------

bool_t SiiPlatformPowerSwitchGet (void )
{
   // static bool_t   debouncing = false;
  //  static bool_t   oldPin = false;
   // bool_t      newPin;
    bool_t  changePin = false;
   // static uint32_t   firstTime;
   // uint32_t         thisTime;

    // Detecting the s1 switch debouncing
  /*  newPin = (ROM_GPIOPinRead( GPIO_PORTB_BASE, GPIO_PIN_1) & GPIO_PIN_1) == 0;
    if (oldPin != newPin)
    {
        while (1)
        {
            newPin = (ROM_GPIOPinRead( GPIO_PORTB_BASE, GPIO_PIN_1) & GPIO_PIN_1) == 0;
            //Button just has been pressed.
            if (newPin && !debouncing)
            {
                firstTime = SiiPlatformTimerSysTicksGet();
                debouncing = true;
            }
            else if (debouncing)
            {
                thisTime = SiiPlatformTimerSysTicksGet();
                if (( thisTime - firstTime ) >= 10 )
                {
                    if (newPin)
                    {
                        debouncing = true;
                        firstTime = SiiPlatformTimerSysTicksGet();
                    }
                    else
                    {
                        debouncing = false;
                        changePin = true;       //!changePin;
                        break;
                    }
                }
            }
        }
    }
    oldPin = newPin;   */
    return changePin;
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformReadRotary
// Description: Read the current value of the rotary switch, with debounce
// Parameters:  iWantItNow  - true: return GPIO value without debounce
// Returns:     Debounced position or 0xFF if no change
//------------------------------------------------------------------------------

uint8_t SiiPlatformReadRotary ( uint8_t iWantItNow )
{
  /*  static uint32_t firstTime = 0;
    uint32_t        thisTime;
    static bool_t   debouncing = false;
    static uint8_t  oldPins = RSW_NO_CHANGE;
    uint8_t         newPins;

        // Get Rotary switch GPIO pins and determine value.

    newPins = ~ROM_GPIOPinRead( GPIO_ROTARY_SWITCH_BASE, GPIO_ROTARY_SWITCH_MASK ) & GPIO_ROTARY_SWITCH_MASK;
    newPins >>= GPIO_ROTARY_SWITCH_SHIFT;
    if ( iWantItNow )
    {
        return( newPins );
    }

    if ( newPins != oldPins ) // port change detected
    {
        oldPins = newPins;
        firstTime = SiiPlatformTimerSysTicksGet();
        debouncing = true;
    }
    else if ( debouncing )
    {
        thisTime = SiiPlatformTimerSysTicksGet();
        //if (( thisTime - firstTime ) >= l_debounceTimes[ newPins ] )
        {
            debouncing = false;
            return( newPins );
        }
    }
			 */
    ASSERT_ALWAYS();
    return( RSW_NO_CHANGE );                // indicate no change
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformSwitchGet
// Description: This function virtualizes the platform DIP slide switches for
//              the application.  The application only know SW1, SW_2, etc.
// Parameters:  DIP slide switch to access.
// Returns:     State of the specified switch: true - closed, false - open.
//------------------------------------------------------------------------------

bool_t SiiPlatformSwitchGet ( SkSwitchNumbers_t switchNum )
{
    bool_t closed=0;

  /*  switch ( switchNum )
    {
        case SW_1:  closed = SiiPlatformGpioReadSwitch( SW2_1 ); break;
        case SW_2:  closed = SiiPlatformGpioReadSwitch( SW2_2 ); break;
        case SW_3:  closed = SiiPlatformGpioReadSwitch( SW2_3 ); break;
        case SW_4:  closed = SiiPlatformGpioReadSwitch( SW2_4 ); break;
        case SW_5:  closed = SiiPlatformGpioReadSwitch( SW2_5 ); break;
        case SW_6:  closed = SiiPlatformGpioReadSwitch( SW2_6 ); break;
        case SW_7:  closed = SiiPlatformGpioReadSwitch( SW2_7 ); break;
        default:
            closed = false;
            break;
    }
		  */
    return( closed );
}

//------------------------------------------------------------------------------
//! @brief  Return state of Siimon bus request pin
//! @return true - bus requested. false - bus NOT requested
//------------------------------------------------------------------------------
bool_t SiiPlatformSiimonBusRequest ( void )
{
#if INC_CBUS_WAKEUP
    return( false );
#else
   // return( ROM_GPIOPinRead( GPIO_PORTC_BASE, GPIO_PIN_7 ) == 0 );
   return( true );
#endif
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformSiimonBusGrant
// Description: Grant the bus to SiMon
// Parameters:
// Returns:     none
//------------------------------------------------------------------------------
void SiiPlatformSiimonBusGrant ( bool_t grant )
{
#if INC_CBUS_WAKEUP
    grant = false;
#else
    if ( !grant )
    {
       // ROM_GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6,  GPIO_PIN_6);
       // ROM_IntMasterEnable();            // Unfreeze IRQ handlers
    }
    else
    {
       // ROM_IntMasterDisable();           // Freeze IR & GPIO interrupt handlers
       // ROM_GPIOPinWrite( GPIO_PORTC_BASE, GPIO_PIN_6, 0 );
    }
#endif
}


//------------------------------------------------------------------------------
// Function:    SiiPlatformInitialize
// Description: Initialize the platform hardware
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//              if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiPlatformInitialize ( BYTE cInstance )
{
	// ASSERT_ALWAYS();
	// Set the clocking to run from the PLL at 50MHz
  /*  ROM_SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SiiPlatformUartInit(0, 19200);
    SiiPlatformI2cInit( I2C_INDEX0, I2C_MASTER, 0, 100 );
    SiiPlatformI2cInit( I2C_INDEX1, I2C_SLAVE, 0x40, 0 );
    SiiPlatformGpioInit();	*/
   // SiiPlatformTimerInit(cInstance);
   /* SiiPlatformInfraRedInit();

    if (SiiPlatformGpioReadSwitch( SW2_6 ))
    {
        // Update program flash if needed.
        SiiPlatformFirmwareUpdate(SI_FLASH_USB_DFU_BOOT_LOADER);
        // This is the dead end.
    }
    else
    {
        SiiPlatformUsbInit(); // Initialize USB->UART module
    }

    SiiPlatformInterruptEnable(cInstance);
    ROM_IntMasterEnable();


#if (FPGA_BUILD == SI_ENABLE)
    {
    int_t   i;		 */

    /* FPGA Initialization. */

  /*  for ( i = 6; i >= 0; i-- )
    {
        DEBUG_PRINT( MSG_ALWAYS, "%d...", i );
        SiiPlatformTimerWait( 1000 );
    }
    }
#endif		 */

    return( true );
}


//------------------------------------------------------------------------------
// Function:    SiiPlatformInterruptEnable
// Description: Interrupt enable for all sources
// Parameters:  none
//------------------------------------------------------------------------------

void SiiPlatformAllInterruptsEnable ( void )
{
  //  ROM_IntMasterEnable();
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformInterruptDisable
// Description: Interrupt disable for all sources
// Parameters:  none
//------------------------------------------------------------------------------

void SiiPlatformAllInterruptsDisable ( void )
{
   // ROM_IntMasterDisable();
}


//------------------------------------------------------------------------------
// Function:    SiiPlatformInterruptEnable
// Description: Interrupt enable for Device H/W INT pin
// Parameters:  none
//------------------------------------------------------------------------------

void SiiPlatformInterruptEnable(BYTE cInstance )
{
    //ROM_GPIOPinIntEnable( GPIO_INT_PORT_BASE, GPIO_INT_PIN_IDX );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformInterruptDisable
// Description: Interrupt disable for Device H/W INT pin
// Parameters:  none
//------------------------------------------------------------------------------

void SiiPlatformInterruptDisable (  BYTE cInstance )
{
   // ROM_GPIOPinIntDisable( GPIO_INT_PORT_BASE, GPIO_INT_PIN_IDX );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformInterruptHandler
// Description: Interrupt handler for device H/W INT pin
// Parameters:  none
//------------------------------------------------------------------------------

void SiiPlatformInterruptHandler ( void )
{
  /*  ROM_IntDisable(GPIO_INT_PORT);
    if (( ROM_GPIOPinIntStatus( GPIO_INT_PORT_BASE, true ) & GPIO_INT_PIN_IDX ))
    {
        ROM_GPIOPinIntClear( GPIO_INT_PORT_BASE, GPIO_INT_PIN_IDX );
        platform.hardwareInt[cInstance] = true;
    }
    ROM_IntEnable(GPIO_INT_PORT);		*/
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformInterruptPinStateGet
// Description: Return false if h/w interrupt line is low (active),
//              or true otherwise (no pending events)
// Parameters:  none
//------------------------------------------------------------------------------

bool_t SiiPlatformInterruptPinStateGet ( void )
{
    bool_t closed = false;
	//return( ROM_GPIOPinRead( GPIO_INT_PORT_BASE, GPIO_INT_PIN_IDX ) != 0 );
	return( closed );
}


//------------------------------------------------------------------------------
// Function:    SiiPlatformInterruptStateGet
// Description: Return true if h/w interrupt has fired
// Parameters:  none
//------------------------------------------------------------------------------

bool_t SiiPlatformInterruptStateGet (  BYTE cInstance )
{
    bool_t isIntPending = platform.hardwareInt[cInstance];
    return( isIntPending );
}


//------------------------------------------------------------------------------
// Function:    SiiPlatformInterruptClear
// Description: Clears interrupt flag. Call when all events got processed.
// Parameters:  none
//------------------------------------------------------------------------------

void SiiPlatformInterruptClear (  BYTE cInstance )
{
    platform.hardwareInt[cInstance] = false;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return result of last I2C operation
//  Parameters:
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

uint32_t SiiPlatformLastI2cStatus (  BYTE cInstance )
{

    return( platform.i2cErrorCode[cInstance] );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns the result of the last platform function called
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

int_t SiiPlatformGetLastResult (  BYTE cInstance )
{

    return( platform.lastResultCode[cInstance] );
}
