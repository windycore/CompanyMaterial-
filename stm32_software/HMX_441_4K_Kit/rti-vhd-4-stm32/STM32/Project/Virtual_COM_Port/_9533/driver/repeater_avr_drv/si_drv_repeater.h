//***************************************************************************
//!file     si_drv_repeater.h
//!brief    Silicon Image Repeater Driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DRV_REPEATER_H__
#define __SI_DRV_REPEATER_H__
#include "si_common.h"
#include "si_drv_switch.h"
#include "si_repeater_component_internal.h"

//------------------------------------------------------------------------------
// Driver enums
//------------------------------------------------------------------------------


typedef enum repeaterRxHdmiMode
{
    SI_RPT_INPUT_MODE_DVI       = 0x0,
    SI_RPT_INPUT_MODE_HDMI      = 0x1

} repeaterRxHdmiMode_t;



//------------------------------------------------------------------------------
// conf
//------------------------------------------------------------------------------

bool_t SiiDrvRepeaterEnable( BYTE cInstance, bool_t repeaterMode );
void SiiDrvRepeaterInit( BYTE cInstance);
void SiiDrvRepeaterSetMode( BYTE cInstance,RepeaterTopoMode_t cmode);
void SiiDrvRepeaterInstanceSet( BYTE cInstance,uint8_t p);
//------------------------------------------------------------------------------
// HDCP
//------------------------------------------------------------------------------

bool_t SiiDrvRepeaterMainPortAuthed( BYTE cInstance);
bool_t SiiDrvRepeaterRxSyncIsOn( BYTE cInstance);
void SiiDrvRepeaterSwitchRxHotPlug( BYTE cInstance,bool_t switch_on);
void SiiDrvRepeaterRxHdcpReset( BYTE cInstance);
void SiiDrvRepeaterSetUsBstatus( BYTE cInstance,uint8_t us_bstatus[2]);
void SiiDrvRxHdcpResetRi(  BYTE cInstance );
void SiiDrvRepeaterSetBcapsFifoReady( BYTE cInstance,bool_t switchOn);
void SiiDrvRepeaterSetBcapsRepeaterBits( BYTE cInstance, bool_t switchOn);
bool_t SiiDrvRepeaterIsRxHotPlugAsserted( BYTE cInstance);
uint8_t SiiDrvRepeaterGetSelectedPort( BYTE cInstance);
bool_t SiiDrvRepeaterMainPortAuthChanged( BYTE cInstance);
bool_t SiiDrvRepeaterAnyInputDecryptionChanged( BYTE cInstance);
bool_t SiiDrvRepeaterNewPortAuthed(  BYTE cInstance );

//------------------------------------------------------------------------------
// isr
//------------------------------------------------------------------------------

bool_t SiiDrvRepeaterIsUsMuteClr( BYTE cInstance);
bool_t SiiDrvRepeaterInputIsHdmi( BYTE cInstance);
bool_t SiiDrvRepeaterInputModeChange(  BYTE cInstance );
bool_t SiiDrvRepeaterRxSyncIntr(  BYTE cInstance );
void SiiDrvRepeaterInterruptsSet( BYTE cInstance, bool_t qOn);
bool_t SiiDrvRepeaterGetInterruptsOn( BYTE cInstance);
bool_t SiiDrvRepeaterMainPortAvMuteChanged( BYTE cInstance, bool_t *muteOn);
void SiiDrvRepeaterResChngStatusSet( BYTE cInstance, bool_t qOn);
bool_t SiiDrvRepeaterResChngStatusGet( BYTE cInstance);

//------------------------------------------------------------------------------
// switch
//------------------------------------------------------------------------------
void SiiDrvRepeaterActiveInputSet( BYTE cInstance, uint8_t portIndex);


#endif // __SI_DRVREPEATER_H__


