
#ifndef _HALPOWERCTRLAPI_H_
#define _HALPOWERCTRLAPI_H_

#include "si_common.h"
#include "util.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************

// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

// ---------------------------------------------------------------------------
// This API sets up the internal state of component..
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void halPowerCtrlInitialize(void);

// ---------------------------------------------------------------------------
// This API is used to get the last power down/power up boot state. In the
// case where user input is detected and processed in the low power monitor
// loop a value will be stored in a CPU register variable that survives the
// boot process. This value is saved in halPowerCtrlInitialize to be used by
// the system power up code to decide weather to display the splash screen and
// finish the boot or to transition into a new power state.
//
// Params: none
//
// Return:
//  The last power state recorded by the low power monitor loop or pwrOFF to
//  signify that the system is coming on from a power fail state.
// ---------------------------------------------------------------------------
//ePOWERSTATE halPowerCtrlGetLastBootPowerState(void);

// ---------------------------------------------------------------------------
// This API is used to prepare the driver layer for a power state change.
// Initially this API has just a single call to the chip driver when preparing
// to enter the ultra low power mode. Upon returning from that call
// dvPowerctrl will be called and it will not return to this API. Only a valid
// wake event will bring the system back to the pwrON power state through the
// boot code.
//
// Params:
//  cRouteId       : Route identifier - for future use.
//  eNewPowerState : The power state to change to..
//
// Return: none
// ---------------------------------------------------------------------------
void halPowerCtrlPowerSet(ePOWERSTATE eNewPowerState);

// ---------------------------------------------------------------------------
// This API will return TRUE if the last suspend low power loop exit was
// caused by user input. This is used in case the system is currently in
// suspend mode and the user hits the power button to turn the device off.
//
// Params: none
//
// Return:
//  TRUE if user input was detected as the reason to leave the low power
//  suspend loop.
// ---------------------------------------------------------------------------
//BOOL halPowerCtrlGetLastSuspendExit(void);

BYTE  get_standby_flag(void);

void  set_clear_standby_flag(BYTE EN);



#endif // _HALPOWERCTRLAPI_H_

