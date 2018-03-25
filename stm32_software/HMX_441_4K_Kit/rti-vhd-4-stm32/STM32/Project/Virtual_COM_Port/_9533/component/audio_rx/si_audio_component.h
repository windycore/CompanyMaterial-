//***************************************************************************
//!file     si_audio_component.h
//!brief    Silicon Image Rx Audio Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_EX_AUDIO__COMPONENT_H__
#define __SI_EX_AUDIO__COMPONENT_H__
#include "si_audio_rx_config.h"
#include "si_drv_audio.h"

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

#define SII_NOACP_TIMEOUT      600 // 30//600 //ms //[kyq add: 20150717 11:50:23]

// Audio states
typedef enum
{
	SII_AUD_STATE_IDLE,
	SII_AUD_STATE_OFF,
	SII_AUD_STATE_ON,
	SII_AUD_NO_ACP_CHECK,
} SiiRxAudioStates_t;

enum
{
	SII_AUD_RX_FSCHG = BIT0,
	SII_AUD_RX_AUDIO_READY = BIT2,
	SII_AUD_RX_AACDONE = BIT6,	
	SII_AUD_RX_FNCHG = BIT7,
	SII_AUD_CHST_RDY = BIT5,
};

typedef struct
{
    SiiRxAudioStates_t      audState;
    SiiRxAudioModes_t       audMode;
    SiiRxAudioLayouts_t     audLayout;
    SiiRxAudioFsValues_t    audFs;
    SiiRxAudioMclk_t        audMclk;
} SiiRxAudioInstanceData_t;

void    SiiRxAudioInit( BYTE cInstance);
bool_t  SiiRxAudioInstanceSet( BYTE cInstance,int instanceIndex);
bool_t  SiiRxAudioStandby(  BYTE cInstance );
uint8_t SiiRxAudioHandler( BYTE cInstance, SiiRxAudioInstanceData_t *pAudRx);
void    SiiRxAudioNoAcpCheck( BYTE cInstance);

#endif // __SI_EX_AUDIO_COMPONENT_H__
