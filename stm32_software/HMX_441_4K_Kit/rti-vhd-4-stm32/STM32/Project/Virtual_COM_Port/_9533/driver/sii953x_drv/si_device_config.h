//***************************************************************************
//!file     si_device_config.h
//!brief    Sii9535 Device configuration file.
//!brief    Driver layer internal file
//          This file supplies the configuration of resources on the
//          Sii9535 device, including how many of each port, MHD channel,
//          ARC, HEC, etc.
//
//          This file is to be included in each driver-level module.  The
//          information contained in the file is propagated up to the
//          component level via driver API functions.
//
//          This file obtains some specific board implementation information
//          that may affect the configuration and usage of the actual device
//          resources via board API functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#ifndef __SI_DEVICECONFIG_H__
#define __SI_DEVICECONFIG_H__
#include "si_common.h"
#include "si_drv_msw_cfg.h"

//------------------------------------------------------------------------------
//  Compile-time build options
//------------------------------------------------------------------------------

#define USE_INTERNAL_MUTE       0       // Set to 1 to use SiI9535 muting
#define DO_EDID_INIT            1       // Set to 1 to perform EDID initialization during boot process  //[kyq CHANGE: 20150804 21:47:47]
#define MHL_WORKAROUND          0       // Set to 1 to enable MHL workaround

#define IS_RX                   1       // Set to 1 to if code is for Sink device
#define IS_TX                   0       // Set to 1 to if code is for Source device


//------------------------------------------------------------------------------
//  Matrix Switch Component SiI9535-specific
//------------------------------------------------------------------------------

#define SWITCH_LINK_CALLBACK        SI_ENABLE


#endif // __SI_DEVICECONFIG_H__
