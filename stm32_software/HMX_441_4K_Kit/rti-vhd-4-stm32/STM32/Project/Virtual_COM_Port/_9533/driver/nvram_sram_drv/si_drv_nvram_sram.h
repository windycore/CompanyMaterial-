//***************************************************************************
//!file     si_drv_nvram_sram.h
//!brief    Silicon Image NVRAM and SRAM driver.  Contains functions for
//          reading and writing Silicon Image device NVRAM and port SRAMS
//          as well as boot data.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DRVNVRAM_SRAM_H__
#define __SI_DRVNVRAM_SRAM_H__
#include "si_common.h"

//------------------------------------------------------------------------------
//  EDID RX Driver Manifest Constants
//------------------------------------------------------------------------------

typedef enum _SiiDrvNvramResultCodes_t
{
    SII_DRV_NVRAM_SUCCESS      = 0,         // Success.
    SII_DRV_NVRAM_ERR_FAIL,                 // General failure.
    SII_DRV_NVRAM_ERR_INVALID_PARAMETER,    //
    SII_DRV_NVRAM_ERR_IN_USE,               // Module already initialized.
    SII_DRV_NVRAM_ERR_NOT_AVAIL,            // Allocation of resources failed.
    SII_DRV_NVRAM_ERR_FAIL_BOOT,
    SII_DRV_NVRAM_ERR_FAIL_NVRAM_INIT,

} SiiDrvNvramResultCodes_t;

#define EDID_TABLE_LEN              256
#define EDID_BLOCK_LEN              128
//#define EDID_VGA_TABLE_LEN          128
#define EDID_DEVBOOT_LEN            64

typedef enum _SiiNvramEdidMode_t
{
    SII_NVRAM_LEGACY_EDID_MODE,
    SII_NVRAM_TWO_EDID_MODE,
} SiiNvramEdidMode_t;

typedef enum
{
    NVRAM_HDMI_EDID,
    NVRAM_VGA_EDID,
    NVRAM_BOOTDATA,
} SiiNvramType_t;

typedef enum
{
    SRAM_P0,
    SRAM_P1,
    SRAM_P2,
    SRAM_P3,
    SRAM_P4,
    SRAM_P5,
    SRAM_VGA,
    SRAM_BOOT,
} SiiSramType_t;

enum
{
    TX1_IS_SUB_PIPE,
    TX1_IS_MAIN_PIPE,
    TX_REG_INIT_PATTERN_1,
    TX_REG_INIT_PATTERN_2
};

enum
{
    EDID_FROM_REG,
    EDID_FROM_NVRAM,
};


enum
{
    MAIN_PIPE_IS_TX0,
    MAIN_PIPE_IS_TX1,
};

enum
{
    SII_CEC_PA_CS_TX0,
    SII_CEC_PA_CS_TX1,
};

enum
{
    SII_CEC_PA_CS_NVRAM,
    SII_CEC_PA_CS_REG,
    SII_CEC_PA_CS_NONE,
};

enum
{
    SII_VAL_PA_CS_NVRAM,
    SII_VAL_PA_CS_TX0,
    SII_VAL_PA_CS_TX1,
};

typedef struct _SiiEdidInfo_t
{
    uint8_t paL;            //Physical address
    uint8_t paH;            //Physical address
    uint8_t paPointer;      //Point to where the PA resides.
    uint8_t cs;             //CheckSum
} SiiEdidInfo_t;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

typedef struct
{
    uint8_t     nvm_config;
    uint8_t     edid_valid;
    uint8_t     nvm_version;
    uint8_t     edid_copy_dest;
    uint8_t     hpd_hw_ctrl;
    uint8_t     ddc_filter_sel;
    uint8_t     wakeup_source;
    uint8_t     edid_vga_copy_dest;
    uint8_t     spare1[2];
    uint8_t     cec_pa_addr;
    uint8_t     spare2;
    uint8_t     cec_pad_h_ch0;
    uint8_t     cec_pad_l_ch0;
    uint8_t     cec_pad_h_ch1;
    uint8_t     cec_pad_l_ch1;
    uint8_t     cec_pad_h_ch2;
    uint8_t     cec_pad_l_ch2;
    uint8_t     cec_pad_h_ch3;
    uint8_t     cec_pad_l_ch3;
    uint8_t     cec_pad_h_ch4;
    uint8_t     cec_pad_l_ch4;
    uint8_t     cec_pad_h_ch5;
    uint8_t     cec_pad_l_ch5;
    uint8_t     spare4[4];
    uint8_t     checksum_ch0;
    uint8_t     checksum_ch1;
    uint8_t     checksum_ch2;
    uint8_t     checksum_ch3;
    uint8_t     checksum_ch4;
    uint8_t     checksum_ch5;
    uint8_t     spare6[2];
    uint8_t     mhdPortSelect;
    uint8_t     mhdVersionSupported;
    uint8_t     miscControl0;
    uint8_t     spare7;
    uint8_t     miscControl1;
    uint8_t     spare8[24];

} SiiDeviceBootData_t;

//------------------------------------------------------------------------------
//  Edid table declarations - must be exported by application code.
//------------------------------------------------------------------------------

//extern  uint8_t g_edidFlashEdidTable[ EDID_TABLE_LEN ];
//extern const uint8_t gEdidFlashEdidVgaTable [ EDID_VGA_TABLE_LEN ];
extern SiiDeviceBootData_t gEdidFlashDevBootData;

//------------------------------------------------------------------------------
//  Function Templates
//------------------------------------------------------------------------------

bool_t  SiiDrvNvramInitialize(  BYTE cInstance );
uint_t  SiiDrvNvramStatus(  BYTE cInstance );
int_t   SiiDrvNvramGetLastResult(  BYTE cInstance );

bool_t  SiiDrvNvramSramRead(  BYTE cInstance, SiiSramType_t sramType, uint8_t *pDest, int_t offset, int_t length );
bool_t  SiiDrvNvramSramWrite(  BYTE cInstance, SiiSramType_t sramType, uint8_t *pSrc, int_t offset, int_t length );

bool_t  SiiDrvEdidRxIsNvramIdle(  BYTE cInstance );
bool_t  SiiDrvEdidRxIsBootComplete(  BYTE cInstance, bool_t isSynchronous );
bool_t  SiiDrvNvramCopyToSram(  BYTE cInstance, SiiNvramType_t nvramType, uint8_t rxPortIdx, bool_t isSynchronous );
bool_t  SiiDrvNvramProgram( BYTE cInstance,  SiiNvramType_t nvramType, bool_t isSynchronous );

// Legacy blocking functions
bool_t  SiiDrvNvramNonVolatileRead( BYTE cInstance,  SiiNvramType_t nvramType, uint8_t *pDest, int_t offset, int_t length );
bool_t  SiiDrvNvramNonVolatileWrite(  BYTE cInstance, SiiNvramType_t nvramType, uint8_t *pSrc, int_t offset, int_t length );

//------------------------------------------------------------------------------
//  EDID Function Templates
//------------------------------------------------------------------------------

int_t   SiiDrvNvramEdidBlockChecksumCalc( uint8_t *pData  );
void    SiiDrvNvramEdidModeSet( BYTE cInstance,  SiiNvramEdidMode_t newMode );
bool_t  SiiDrvNvramEdidBlockWrite(  BYTE cInstance, SiiSramType_t txSelect, uint8_t *pSrc, int_t offset );

#endif      // __SI_DRVNVRAM_SRAM_H__
