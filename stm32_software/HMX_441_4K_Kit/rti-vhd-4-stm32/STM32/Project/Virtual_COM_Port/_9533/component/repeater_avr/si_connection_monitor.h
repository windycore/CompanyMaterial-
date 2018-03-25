//------------------------------------------------------------------------------
// Project: HDMI Repeater
// Copyright (C) 2002-2009, Silicon Image, Inc.  All rights reserved.
//
// No part of this work may be reproduced, modified, distributed, transmitted,
// transcribed, or translated into any language or computer format, in any form
// or by any means without written permission of: Silicon Image, Inc.,
// 1060 East Arques Avenue, Sunnyvale, California 94085
//------------------------------------------------------------------------------

#ifndef SIICONNECTIONMONITOR_H
#define SIICONNECTIONMONITOR_H

#include "si_common.h"

#define AV_MUTE_ALL_CLEAR					0x00
#define AV_MUTE_TX_IN_HDCP					0x01
#define AV_MUTE_TX_NOT_READY				0x02
#define AV_MUTE_RX_REQUEST					0x04




void RepeaterConDisableInput( BYTE cInstance, bool_t disable_hot_plug_also);
clock_time_t RepeaterConnectionMonitor(  BYTE cInstance,clock_time_t  msThisDiff );
void SiiRepeaterConnectionMonitorInit(  BYTE cInstance );
void RepeaterConAvMuteSet( BYTE cInstance, uint8_t cause_mask, bool_t qOn );
void SiiRepeaterConInstanceSet ( BYTE cInstance, uint8_t instance );

void SiiRepeaterConStructInstanceSet ( BYTE cInstance, uint8_t instance );

#endif // SIICONNECTIONMONITOR_H

