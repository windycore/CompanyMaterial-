//***************************************************************************
//!file     sk_app_edid.h
//!brief
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_APPEDID_H__
#define __SI_APPEDID_H__

#include "si_drv_nvram_sram.h"

//------------------------------------------------------------------------------
// Application EDID data
//------------------------------------------------------------------------------

//extern uint8_t g_edidFlashEdidTable [ EDID_TABLE_LEN ];
//extern const uint8_t gEdidFlashEdidVgaTable [ EDID_VGA_TABLE_LEN ];
extern SiiDeviceBootData_t gEdidFlashDevBootData;

void SkAppInitEdid(  BYTE cInstance );
bool_t SkAppEdidRead( BYTE cInstance,  uint8_t source, uint8_t *pDest );
bool_t SkAppEdidWrite(  BYTE cInstance, uint8_t target, uint8_t *pSource );
void  SkAppDefaultEdidDataProductNameSet(PBYTE pcData);
#endif  // __SI_APPEDID_H__

