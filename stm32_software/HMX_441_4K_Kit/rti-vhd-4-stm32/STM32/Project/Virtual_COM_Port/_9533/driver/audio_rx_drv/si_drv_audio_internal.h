//***************************************************************************
//!file     si_drv_cbus_internal.h
//!brief    Silicon Image CBUS Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DRV_AUDIO_EX_INTERNAL_H__
#define __SI_DRV_AUDIO_EX_INTERNAL_H__
#include "si_device_config.h"
#include "si_drv_audio.h"

// Internal only
void    SiiDrvRxAudioGetIntStatus( BYTE cInstance, uint8_t *pStatus);
void    SiiDrvRxAudioClearIntStatus(BYTE cInstance, uint8_t *pStatus);
void    SiiDrvRxAudioInit (  BYTE cInstance );
uint8_t SiiDrvRxAudioIsInterruptAssert( BYTE cInstance);
void    SiiDrvRxAudioInterruptClearAssert( BYTE cInstance);
uint8_t SiiDrvRxAudioGetMclk( BYTE cInstance);
void    SiiDrvRxAudioUnmuteReady( BYTE cInstance,bool_t qOn);
bool_t  SiiDrvRxAudioGetNewAcpInt( BYTE cInstance);
void    SiiDrvRxAudioOnNoAcp( BYTE cInstance);
bool_t  SiiDrvRxAudioGetNoAcpInt( BYTE cInstance);
void    SiiDrvRxAudioOnNewAcp(BYTE cInstance, bool_t qOn);
void    SiiDrvRxAudioNoAcpPacketcheckStart(BYTE cInstance, bool_t qOn);
bool_t  SiiDrvRxAudioNoAcpPacketcheckConfirm( BYTE cInstance);
bool_t  SiiDrvRxAudioAcpCheckDisturbance( BYTE cInstance);

void    SiiDrvRxAudioInternalMute(BYTE cInstance, bool_t qOn);

#endif // __SI_DRV_AUDIO_EX_INTERNAL_H__
