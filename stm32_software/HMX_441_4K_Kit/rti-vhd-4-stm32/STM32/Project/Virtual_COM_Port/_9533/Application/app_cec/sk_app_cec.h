//***************************************************************************
//!file     sk_app_cec.h
//!brief    Application and demo functions for the CEC component
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#if !defined __SKAPP_CEC_H__
#define __SKAPP_CEC_H__

#include "sk_application.h"
#include "si_cec_component.h"
#if INC_CEC_SWITCH
#include "si_cec_switch_api.h"
#endif
#if INC_CEC_SAC
#include "si_sac_main.h"
#endif

extern  uint8_t  appCecSourceLaList[];

typedef struct _SiiAppCecInstanceData_t
{
    int     instanceIndex;  // SkAppCecInstanceSet

    bool_t  isEnumerated;
    bool_t  setStreamPathReceived;  // Used by wakeup for CBUS support
    uint8_t setStreamPathArg0;      // Used by wakeup for CBUS support
    uint8_t setStreamPathArg1;      // Used by wakeup for CBUS support

}   SiiAppCecInstanceData_t;

extern SiiAppCecInstanceData_t appCec[DevNum][SII_NUM_CPI];
extern SiiAppCecInstanceData_t *pAppCec[DevNum];


void    SkAppCecInstanceSet(  BYTE cInstance, int_t newInstance );
bool_t  SkAppDeviceInitCec(  BYTE cInstance );
void    SkAppCecConfigure(  BYTE cInstance, bool_t enable );
void    SkAppTaskCec(  BYTE cInstance );
bool_t  SkAppCecStandby(  BYTE cInstance );
bool_t  SkAppCecResume(  BYTE cInstance, bool_t powerIsOn );

void    SiiCecAppInputPortChangeHandler( BYTE cInstance, uint8_t newInputPortIndex);

void    SkAppCecMessageLogger( SiiCpiData_t *pMsg, int systemType, bool_t isTx );
//bool_t  SkAppCecDemo( RC5Commands_t key );

#endif  //__SKAPP_CEC_H__
