//***************************************************************************
//!file     si_drv_device.h
//!brief    Sii9535 driver header.
//!brief    Component layer file
//!brief    Application layer file
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#ifndef __SI_DRV_DEVICE_H__
#define __SI_DRV_DEVICE_H__
#include "si_common.h"

typedef enum _SiiPipeTypes_t
{
    SiiPipeType_Main        = 0,
    SiiPipeType_Background  = 1
} SiiPipeTypes_t;


bool_t SiiDrvDeviceNonVolatileInitialize (  BYTE cInstance, bool_t forceInit );
bool_t SiiDrvDeviceConfigure(  BYTE cInstance );
uint16_t SiiDrvDeviceIdGet( BYTE cInstance);
uint8_t SiiDrvDeviceRevGet( BYTE cInstance);
bool_t SiiDrvDeviceInitialize ( BYTE cInstance,  bool_t allPowerOn );
bool_t SiiDrvDeviceStandby(  BYTE cInstance );
bool_t SiiDrvDeviceResume(  BYTE cInstance );
bool_t SiiDrvDeviceRelease(  BYTE cInstance );
bool_t SiiDrvDeviceStart (  BYTE cInstance );

enum
{
    SiiDevice_SUCCESS            = 0x00,
    SiiDevice_FAIL_DEVID_READ,
    SiiDevice_FAIL_WRONGDEVICE,
    SiiDevice_FAIL_BOOT,
    SiiDevice_FAIL_NVRAM_INIT,
};

int_t SiiDrvDeviceGetLastResult(  BYTE cInstance );
bool_t SiiDrvDeviceBootComplete(  BYTE cInstance );
bool_t SiiDrvDevicePowerUpBoot(  BYTE cInstance );
bool_t SiiDrvDeviceNvramInitialize( BYTE cInstance,  bool_t forceInit );

typedef enum
{
    DISABLE_VSIF_PACKET_CHK     = 0x0001,
    DISABLE_IEEE_PACKET_CHK     = 0x0002,
    DISABLE_3D_FORMAT_CHK       = 0x0004,
    DISABLE_PACKED_FORMAT_CHK   = 0x0008,
    INSERT_3D_VS                = 0x0010,
    ENABLE_3D_MARKER_SUPPORT    = 0x0020,
    SET_SUBPIPE_OPTIONS         = 0x0040,
    SET_MAINPIPE_OPTIONS        = 0x0080,
} Sii_3dMarkerOptions;

typedef enum
{
    SiiDEV_INPUT_CONNECTED  = 1,
    SiiDEV_BOOT_STATE_MACHINE,
    SiiDEV_NVRAM,
    SiiDEV_ID,
    SiiDEV_REV,
    SiiDEV_ACTIVE_PORT,
    SiiDEV_SELECTED_PORT_BITFIELD
} SiiDrvDeviceInfo_t;

uint_t SiiDrvDeviceInfo(  BYTE cInstance, SiiDrvDeviceInfo_t infoIndex );
bool_t SiiDrvDeviceManageInterrupts(  BYTE cInstance, bool_t fullPowerMode );

typedef enum
{
    SiiDEV_STATUS_RES_STB_CHG   = 0x01,    // Resolution stability has changed
    SiiDEV_STATUS_MP_RES_CHG    = 0x02,    // Main Pipe resolution has changed
    SiiDEV_STATUS_SP_RES_CHG    = 0x04,    // Sub Pipe resolution has changed
    SiiDEV_STATUS_AVI_READY     = 0x08,
    SiiDEV_STATUS_AIF_READY     = 0x10
} SiiDrvDeviceStatus_t;

void SiiDrvDeviceSpResDetectionEnable( BYTE cInstance, bool_t isEnabled);
void SiiDrvDeviceMpResDetectionEnable( BYTE cInstance, bool_t isEnabled);

SiiDrvDeviceStatus_t SiiDrvDeviceStatus (  BYTE cInstance );

void SiiDrvDevice3dMarkerGpioEnable( BYTE cInstance, uint8_t enableFlags );
void SiiDrvDevice3dMarkerConfigure(  BYTE cInstance, uint16_t optionFlags, uint16_t markerDelay720p, uint16_t markerDelay1080p );
void SiiDrvDeviceGet3dMarkerConfiguration(  BYTE cInstance, uint16_t *pOptionFlags, uint16_t *pMarkerDelay720p, uint16_t *pMarkerDelay1080p );

#endif // __SI_DRV_DEVICE_H__
