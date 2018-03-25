//************************************************************************************************
//! @file   si_edid_tx_component.c
//! @brief  Silicon Image EDID Tx Component
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***********************************************************************************************/

#include "string.h"
#include <stdio.h>
#include "si_edid_tx_component.h"
#include "si_edid_tx_internal.h"
#include "si_tx_config.h"
#include "si_scaler_tables.h"
#include "si_video_tables.h"
#include "si_drv_nvram_sram.h"
#include "si_cra.h"


#include "main.h"
#include "Mcu.h"
#include "uart.h"
#include "BSP_I2C.h"
#include "Si_osal_timer.h"

//------------------------------------------------------------------------------
//  EdidTx Component Instance Data
//------------------------------------------------------------------------------
//extern uint8_t g_tempData[256];
//extern uint8_t g_edidFlashEdidTable [ 256 ];
extern uint8_t g_Init_EdidFlashEdidTable [ 256 ];


EdidTxDecodData_t    edidDecodData[INSTANCE_9533][SII_NUM_TX];
EdidTxInstanceData_t edidTxInstance[INSTANCE_9533][SII_NUM_EDID_TX];
EdidTxInstanceData_t *pEdidTx[INSTANCE_9533] = 
{
    &edidTxInstance[0][0],
#if(INSTANCE_9533>1)
    &edidTxInstance[1][0],
#endif

#if(INSTANCE_9533>2)
    &edidTxInstance[2][0],
#endif

#if(INSTANCE_9533>3)
    &edidTxInstance[3][0],
#endif

#if(INSTANCE_9533>4)
    &edidTxInstance[4][0],
#endif

#if(INSTANCE_9533>5)
    &edidTxInstance[5][0],
#endif
#if(INSTANCE_9533>6)
    &edidTxInstance[6][0],
#endif
#if(INSTANCE_9533>7)
    &edidTxInstance[7][0],
#endif

};

// 用于保存EDID 数据
BYTE m_acEdidTXData[INSTANCE_9533][256];  //[kyq Begin: 20150728 14:50:26]

//-------------------------------------------------------------------------------------------------
//  Local service functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Print EDID TX related error messages.
//!
//! @param[in]  errors - error mask. @see TX_EDID_ERROR_MASKS
//-------------------------------------------------------------------------------------------------

static void PrintErrors(BYTE cInstance, uint32_t errors)
{
    if (errors)
    {
        DEBUG_PRINT(EDID_TX_MSG_DBG, "EDID Errors: ");
        if (errors & EDID_ERROR__INCORRECT_VERSION)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "INCORRECT VERSION ");
        if (errors & EDID_ERROR__CHECK_SUM_MISMATCH)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "CHECK SUM MISMATCH ");
        if (errors & EDID_ERROR__INCORRECT_HEADER)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "INCORRECT HEADER ");
        if (errors & EDID_ERROR__DEFAULT_EDID_USED)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "DEFAULT EDID USED ");
        if (errors & EDID_ERROR__CANNOT_READ_BL0)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "CANNOT READ BL0 ");
        if (errors & EDID_ERROR__CANNOT_READ_EXT)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "CANNOT READ EXT ");
        if (errors & EDID_ERROR__INCORRECT_MAP_TAG)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "INCORRECT MAP TAG ");
        if (errors & EDID_ERROR__TOO_MANY_EXTENSIONS)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "TOO MANY EXTENSIONS ");
        if (errors & EDID_ERROR__EXT_BLOCK_VIOLATION)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "EXT BLOCK VIOLATION ");
        if (errors & EDID_ERROR__BLOCK_0_VIOLATION)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "BLOCK 0 VIOLATION ");
        if (errors & EDID_ERROR__DATA_COLLECT_VIOLATION)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "DATA COLLECTION VIOLATION ");
        if (errors & EDID_ERROR__NOT_ENOUGH_ROOM)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "NOT ENOUGH ROOM ");
        if (errors & EDID_ERROR__FOUND_UNSUPPORTED_DTD)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "FOUND UNSUPPORTED DTD ");
        if (errors & EDID_ERROR__DTD_VIOLATION)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "DTD VIOLATION ");
        if (errors & EDID_ERROR__INTERNAL_ERROR)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "INTERNAL ERROR ");
        if (errors & EDID_ERROR__CANNOT_WRITE)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "CANNOT WRITE ");
        if (errors & EDID_ERROR__RANGE_LIMITATION)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "RANGE LIMITATION ");
        if (errors & EDID_ERROR__NO_SPACE_FOR_DBC)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "NO SPACE FOR DBC ");
        if (errors & EDID_ERROR__INCORRECT_AR_IN_DTD_FIX)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "INCORRECT AR IN DTD FIX ");
        if (errors & EDID_ERROR__RANGE_LIMITS_CORRECTED)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "RANGE LIMITS CORRECTED ");
        if (errors & EDID_ERROR__INCORRECT_PICTURE_SIZE)
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "INCORRECT PICTURE SIZE ");
        DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "\n");
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Print a list of features supported by DS.
//!
//-------------------------------------------------------------------------------------------------

static void PrintFeatures( BYTE cInstance)
{
#if (API_DEBUG_CODE==1)
    if(DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "DS Device Supports: "))
    {

        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_HDMI))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "HDMI ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_AI))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "AI ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_ENCODED_AUDIO))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "Encoded_Audio ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_AUDIO))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "PCM_Audio ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_1BIT_AUDIO))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "DSD ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_HBR_AUDIO))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "HBRA ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_YCbCr))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "YCbCr ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_NTSC))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "NTSC ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_PAL))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "PAL ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_AR_16x9))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "16x9 ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_AR_4x3))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "4x3 ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_DEEP_COLOR_30_BIT))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "DC30 ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_DEEP_COLOR_36_BIT))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "DC36 ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_DEEP_COLOR_48_BIT))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "DC48 ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_DEEP_COLOR_YCbCr_444))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "DC_Y444 ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_XVYCC_P0))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "xvYCC ");}
        if (SiiEdidTxDoesDsSupport(cInstance, SI_EDID_TX_SUPPORT_3D))
        {DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "3D ");}
        DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "\n");
    }
    if (SiiEdidTxDsDeviceRemedyMaskGet( cInstance ))
        DEBUG_PRINT(EDID_TX_MSG_DBG, "DS Remedy Mask: 0x%04X\n", (int) SiiEdidTxDsDeviceRemedyMaskGet(cInstance));
#endif
}



//------------------------------------------------------------------------------
// Function:    EdidFixDecodedEdid
// Description: Modifies decoded EDID information in the parsing structure filled
//              by the EdidTxDecodeEdid() function.  Fixes some common
//              problems found when reading downstream EDIDs.
//------------------------------------------------------------------------------

static void EdidFixDecodedEdid (  BYTE cInstance )
{
    bool_t use_default_cec_pa = false;

    // Attempt to recover CEC PA when DS EDID is not readable

    if ( pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode & EDID_ERROR__DEFAULT_EDID_USED )
    {
/*#if (SI_USE_CEC == ENABLE)
        uint16_t old_pa = SiiCecGetDevicePA();

        if ((0 != old_pa) && (0xFFFF != old_pa))
        {
            DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "Attempt to recover CEC PA- old CEC PA is used\n" );
            parsing->HDMI_VSDB.CEC.a = (old_pa >> 12) & 0x0F;
            parsing->HDMI_VSDB.CEC.b = (old_pa >> 8) & 0x0F;
            parsing->HDMI_VSDB.CEC.c = (old_pa >> 4) & 0x0F;
            parsing->HDMI_VSDB.CEC.d = old_pa & 0x0F;
        }
        else
#endif // SI_USE_CEC  */
        {
            use_default_cec_pa = true;
        }
    }

    // 080403
    // A fix for downstream DVI devices.
    if (0 == pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.CEC.a)
        use_default_cec_pa = true;

    if (use_default_cec_pa)
    {
        DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "CEC PA is defaulted to 1000\n" );
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.CEC.a = 1;
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.CEC.b = 0;
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.CEC.c = 0;
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.CEC.d = 0;
    }

    // 080403
    // A fix for DVI devices declaring audio or other HDMI features.
    // Example: Dell W1900 DVI Monitor declares audio support.
    if ( !pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.HdmiMode )
    {
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.SupportAudio = false;
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.SupportYCbCr = false;
        //memset(pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.Extras, 0, sizeof(pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.Extras));
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.SadCount = 0;
    }
}





//-------------------------------------------------------------------------------------------------
//  EDID TX API functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Returns index of an instance selected in context
//!
//! @return     Index of an instance record
//-------------------------------------------------------------------------------------------------

uint8_t SiiEdidTxInstanceGet( BYTE cInstance)
{
    return (pEdidTx[cInstance] - &edidTxInstance[cInstance][0]);
}



void SiiEdidTxStructInstanceSet( BYTE cInstance,uint8_t instance)
{
    if (instance < SII_NUM_EDID_TX)
    {
        pEdidTx[cInstance] = &edidTxInstance[cInstance][instance];
	 pEdidTx[cInstance]->pEdidDecodData = &edidDecodData[cInstance][instance];
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Brings an instance referred by given index into current context.
//!
//! @param[in]  instance - index of an instance record
//-------------------------------------------------------------------------------------------------

void SiiEdidTxInstanceSet( BYTE cInstance, uint8_t instance)
{
#if (SII_NUM_EDID_TX > 1)
    if (instance < SII_NUM_EDID_TX)
    {
        pEdidTx[cInstance] = &edidTxInstance[cInstance][instance];
    }
#endif
}


//-------------------------------------------------------------------------------------------------
//! @brief      Switch EDID input to come from hooked TX component.
//!
//! @param[in]  input - EDID TX input accepting the EDID data from TX. Should be different
//!                     for different TX instances in multi-input model.
//! @return     success flag.
//-------------------------------------------------------------------------------------------------

bool_t SiiEdidTxInputSet( BYTE cInstance, uint8_t input)
{
    bool_t isSuccess = true;

    pEdidTx[cInstance]->lastResultCode = SI_TX_EDID_ERROR_CODE_NO_ERROR;

    if(input < pEdidTx[cInstance]->numOfTx)
    {
        pEdidTx[cInstance]->pEdidDecodData = &edidDecodData[cInstance][input];
    }
    else
    {
        pEdidTx[cInstance]->lastResultCode = SI_TX_EDID_ERROR_CODE_BAD_INPUT_INDEX;
        isSuccess = false;
    }

    return isSuccess;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Configures the EDID TX component to operate with the feature set
//!             of the system video and audio, as well as sets system EDID options.
//!
//! @param[in]  numOfTx          - Number of transmitters providing EDID per the component instance
//! @param[in]  videoFeatureMask - Video option mask. @see TX_EDID_VIDEO_CFG_MASKS
//! @param[in]  audioFeatureMask - Audio option mask. @see TX_EDID_AUDIO_CFG_MASKS
//! @param[in]  miscOptionsMask  - Miscellaneous EDID option mask. @see TX_EDID_MISC_CFG_MASKS
//!
//! @return     true if the configuration was successful, or false if some
//!             failure occurred
//-------------------------------------------------------------------------------------------------

bool_t SiiEdidTxConfigure ( BYTE cInstance, uint8_t  numOfTx, uint16_t videoFeatureMask,
                            uint16_t audioFeatureMask, uint16_t miscOptionsMask)
{
    pEdidTx[cInstance]->numOfTx = numOfTx;

    // Limits of this device
    if (videoFeatureMask & SI_TX_EDID_VIDEO_CFG_ENABLE_MHL_LIMITS)
    {
        pEdidTx[cInstance]->edidConfig.maxHorizKhz = MHL__MAX_H_KHZ, // Max Horiz. Freq
        pEdidTx[cInstance]->edidConfig.minHorizKhz = MHL__MIN_H_KHZ, // Min Horiz. Freq
        pEdidTx[cInstance]->edidConfig.maxVertHz = MHL__MAX_V_HZ, // Max Vertical Freq
        pEdidTx[cInstance]->edidConfig.minVertHz = MHL__MIN_V_HZ, // Min Vertical Freq
        pEdidTx[cInstance]->edidConfig.maxPixClock10Mhz = MHL__MAX_PIX_CLK_10MHZ; // Pixel Clock in 10MHz
    }
    else
    {
        pEdidTx[cInstance]->edidConfig.maxHorizKhz = VIDEO__MAX_H_KHZ, // Max Horiz. Freq
        pEdidTx[cInstance]->edidConfig.minHorizKhz = VIDEO__MIN_H_KHZ, // Min Horiz. Freq
        pEdidTx[cInstance]->edidConfig.maxVertHz = VIDEO__MAX_V_HZ, // Max Vertical Freq
        pEdidTx[cInstance]->edidConfig.minVertHz = VIDEO__MIN_V_HZ, // Min Vertical Freq
        pEdidTx[cInstance]->edidConfig.maxPixClock10Mhz = VIDEO__MAX_PIX_CLK_10MHZ; // Pixel Clock in 10MHz
    }


    pEdidTx[cInstance]->usEdidBlockCount   = 2;

    pEdidTx[cInstance]->isSoundBarMode     = (0 != (miscOptionsMask & SI_TX_EDID_CFG_OPTIONS_SOUNDBAR_MODE));
    pEdidTx[cInstance]->isHdmi3DEnabled    = (0 != (videoFeatureMask & SI_TX_EDID_VIDEO_CFG_ENABLE_3D));
    pEdidTx[cInstance]->isXvYccProfile0    = (0 != (videoFeatureMask & SI_TX_EDID_VIDEO_CFG_ENABLE_XVYCC));

    pEdidTx[cInstance]->parseOptions.AllowCheckSumMismatchForBlock0 = (0 != (videoFeatureMask & SI_TX_EDID_VIDEO_CFG_ALLOW_BAD_CSUM_FOR_BLOCK0));
    pEdidTx[cInstance]->parseOptions.AllowPcModesOnScalerBypass = (0 != (videoFeatureMask & SI_TX_EDID_VIDEO_CFG_ALLOW_PC_MODES_NO_SCALER));
    pEdidTx[cInstance]->parseOptions.BypassScaler  = (0 != (videoFeatureMask & SI_TX_EDID_VIDEO_CFG_BYPASS_SCALER ));
    pEdidTx[cInstance]->parseOptions.OutIdx        = SII_PREFERRED_OUT_IDX;
    pEdidTx[cInstance]->parseOptions.AudioBypass   = (0 != (audioFeatureMask & SI_TX_EDID_AUDIO_CFG_BYPASS));
    pEdidTx[cInstance]->parseOptions.DviInput      = (0 != (miscOptionsMask & SI_TX_EDID_CFG_OPTIONS_FORCE_DVI_INPUT));
    pEdidTx[cInstance]->parseOptions.enableRemedies    = (0 != (videoFeatureMask & SI_TX_EDID_VIDEO_CFG_ENABLE_REMEDIES));

    pEdidTx[cInstance]->isDeepColor3036      = (0 != (videoFeatureMask & SI_TX_EDID_VIDEO_CFG_DC30_36));
    pEdidTx[cInstance]->isDeepColorOvrd      = (0 != (videoFeatureMask & SI_TX_EDID_VIDEO_CFG_DC_OVRD));
    pEdidTx[cInstance]->isSpdifOnly          = (0 != (audioFeatureMask & SI_TX_EDID_AUDIO_CFG_SPDIF_INPUT));

    return( true );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Returns a status flag word containing EdidTx-specific
//!             information about the state of the device.
//! @return     Sixteen-bit status flags word for the EdidTx Component
//-------------------------------------------------------------------------------------------------

uint16_t SiiEdidTxStatusGet (  BYTE cInstance )
{

    return( pEdidTx[cInstance]->statusFlags );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Places the EdidTx component into standby mode if available
//!
//!             This is a dummy call. EDID component doesn't control any hardware
//!             directly.
//!
//! @return     Success flag.
//-------------------------------------------------------------------------------------------------

bool_t SiiEdidTxStandby ( void )
{

    return( true );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Causes operation of the EdidTx component to return to the state
//!             it was prior to the previous call to SiiEdidTxStandby.
//!             Dummy call.
//-------------------------------------------------------------------------------------------------

bool_t SiiEdidTxResume ( void )
{

    return( true );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Returns the result of the last SiiEdidTxxxx function called.
//!
//-------------------------------------------------------------------------------------------------

txEdidErrCode_t SiiEdidTxGetLastResult (  BYTE cInstance )
{

    return( pEdidTx[cInstance]->lastResultCode );
}


//-------------------------------------------------------------------------------------------------
//! @brief      EDID Tx component initialization.
//!
//! @return     Success flag.
//-------------------------------------------------------------------------------------------------

bool_t SiiEdidTxInitialize (  BYTE cInstance )
{
//    uint16_t EDID_MANUFACTURER_ID,EDID_PRODUCT_ID=0;
//    uint16_t EDID_Temp[2] = {0,0};
    
    memset(pEdidTx[cInstance], 0, sizeof(EdidTxInstanceData_t));
    memset(m_acEdidTXData[cInstance],0,256); //[kyq ADD: 20150728 14:47:13]
/*  搞不懂什么意思
	if((cInstance==0)||(cInstance==1)||(cInstance==2)||(cInstance==3))//||(DeviceID==4)||(DeviceID==5))
	{
		Read_EDID(INPUT_EDID_add + 256*cInstance,g_tempData);
		EDID_Temp[0]= g_tempData[8];
		EDID_Temp[0]<<= 8;
		EDID_Temp[1]= EDID_Temp[0]|g_tempData[9];
		EDID_MANUFACTURER_ID= EDID_Temp[1];
		EDID_Temp[0]= g_tempData[10];
		EDID_Temp[0]<<= 8;
		EDID_Temp[1]= EDID_Temp[0]|g_tempData[11];
		EDID_PRODUCT_ID= EDID_Temp[1];
	}
	else 
	{	
		EDID_Temp[0]= init_edidFlashEdidTable[8];
		EDID_Temp[0]<<= 8;
		EDID_Temp[1]= EDID_Temp[0]|init_edidFlashEdidTable[9];
		EDID_MANUFACTURER_ID= EDID_Temp[1];
		EDID_Temp[0]= init_edidFlashEdidTable[10];
		EDID_Temp[0]<<= 8;
		EDID_Temp[1]= EDID_Temp[0]|init_edidFlashEdidTable[11];
		EDID_PRODUCT_ID= EDID_Temp[1];
	}
*/
    // By default one-TX configuration is set
    pEdidTx[cInstance]->numOfTx = 1;
    pEdidTx[cInstance]->pEdidDecodData = &edidDecodData[cInstance][0];
    memset(pEdidTx[cInstance]->pEdidDecodData, 0, sizeof(EdidTxDecodData_t));
    pEdidTx[cInstance]->pEdidDecodData->isActive = false;

    // Credentials of this device
    pEdidTx[cInstance]->edidConfig.mfgId = EDID__MANUFACTURER_ID; // Two byte manufacturer ID
    pEdidTx[cInstance]->edidConfig.productId = EDID__PRODUCT_ID; // Two byte product ID
    pEdidTx[cInstance]->edidConfig.serialNumber = 0; // Four byte serial number (may be last 4 digits)
    pEdidTx[cInstance]->edidConfig.mfgWeek = SET_MANUFACTURER_WEEK; // week manufactured
    pEdidTx[cInstance]->edidConfig.mfgYear = SET_MANUFACTURER_YEAR; // year manufactured

    // Default limits of this device
    pEdidTx[cInstance]->edidConfig.maxHorizKhz = DEFAULT__MAX_H_KHZ, // Max Horiz. Freq
    pEdidTx[cInstance]->edidConfig.minHorizKhz = DEFAULT__MIN_H_KHZ, // Min Horiz. Freq
    pEdidTx[cInstance]->edidConfig.maxVertHz = DEFAULT__MAX_V_HZ, // Max Vertical Freq
    pEdidTx[cInstance]->edidConfig.minVertHz = DEFAULT__MIN_V_HZ, // Min Vertical Freq
    pEdidTx[cInstance]->edidConfig.maxPixClock10Mhz = DEFAULT__MAX_PIX_CLK_10MHZ; // Pixel Clock in 10MHz
    pEdidTx[cInstance]->edidConfig.maxTmdsClock5Mhz = DEFAULT__MAX_TMDS_CLK_5MHZ; // maxTmdsClock in 5Mhz units
    pEdidTx[cInstance]->pacEdidTxData=m_acEdidTXData[cInstance];//[kyq add: 20150728 15:00:44]
    return (true);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Initialize the EDID decode info structure
//!
//!             Call before starting SiiEdidTxProcessEdid().
//!
//-------------------------------------------------------------------------------------------------

static void EdidTxInitProcessing (  BYTE cInstance )
{
    memset( &pEdidTx[cInstance]->pEdidDecodData->edidInfo, 0, sizeof( decodedEdid_t ));
    pEdidTx[cInstance]->pEdidDecodData->SVD_idx    = 0;
    pEdidTx[cInstance]->pEdidDecodData->nmbOfDecodedExtBlocks = 0;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Creates new EDID data for upstream based on all active TX EDIDs.
//!             Call upon TX connection or disconnection.
//-------------------------------------------------------------------------------------------------

static void EdidTxCreateUsEdid( BYTE cInstance )
{
    uint8_t i;
    bool_t  isFirstActiveTx = true;
    EdidTxDecodData_t *pEdidDecodData;

    DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "DS EDIDs Attached: %d\n", pEdidTx[cInstance]->numOfActiveTx);

    // If more than one parsed EDID input exists, merge them into a "greatest common denominator" data
    // in order to leave only those capability declarations that are supported by all
    // active transmitters
    for (i = 0; i < pEdidTx[cInstance]->numOfTx; i++)
    {
        pEdidDecodData = &edidDecodData[cInstance][i];
        if (pEdidDecodData->isActive)
        {
            if (!isFirstActiveTx)
            {
                // Apply the Feature Meet Filter (FMF)
                EdidTxFeatureMeetFilter( cInstance, &(pEdidDecodData->edidInfo));
            }
            else
            {
                // Copy first active edidInfo into output edidInfo
                memcpy(&pEdidTx[cInstance]->edidInfo, &(pEdidDecodData->edidInfo), sizeof(decodedEdid_t));
                // Make sure other active TXs added through the FMF
                isFirstActiveTx = false;
            }
        }
    }

    EdidTxConvert(cInstance);
    PrintErrors( cInstance,pEdidTx[cInstance]->edidInfo.ErrorCode ); // zzl

    EdidTxCompose(cInstance);
    PrintErrors(cInstance, pEdidTx[cInstance]->edidInfo.ErrorCode ); // zzl
}


//-------------------------------------------------------------------------------------------------
//! @brief      Decode, analyze, convert input EDID, and compose a new EDID.
//!
//!             This function shall be called upon downstream HPD event.
//!
//! @param[in]  input - EDID TX input accepting the EDID data from TX. Should be different
//!                     for different TX instances in multi-input model.
//-------------------------------------------------------------------------------------------------

void SiiEdidTxProcessEdid ( BYTE cInstance, uint8_t input )
{
    if (SiiEdidTxInputSet(cInstance,input))
    {
        if (!pEdidTx[cInstance]->pEdidDecodData->isActive)
        {
            pEdidTx[cInstance]->numOfActiveTx++;
            // Activate instance when EDID processing is requested
            memset(pEdidTx[cInstance]->pEdidDecodData, 0, sizeof(EdidTxDecodData_t));
            pEdidTx[cInstance]->pEdidDecodData->isActive = true;
        }

        EdidTxInitProcessing( cInstance );
        EdidTxDecodeEdid( cInstance );
        SiiDrvTpiDdcErrorsPrint( cInstance );// zzl
        EdidFixDecodedEdid( cInstance );

        EdidTxAnalyze( cInstance );
        PrintErrors(cInstance,pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode);	//zzl

        // Print a list of supported features
        PrintFeatures( cInstance );	 // zzl

        // Make US EDID based on already parsed data, if any, and the newly attached DS EDID
        EdidTxCreateUsEdid(cInstance);
/*  搞不懂
				if(cInstance<4)
				{
					Read_EDID(INPUT_EDID_add + 256*cInstance,g_tempData);
					SiiRegWrite (cInstance, 0x0010, 0x00 );//将热插拔拉低 
					delay_nms(100);
					//SiiDrvNvramSramWrite( SRAM_P2, init_edidFlashEdidTable, 0, EDID_TABLE_LEN );//更新EDID数据
					SiiDrvNvramSramWrite(  cInstance, SRAM_P2, g_tempData, 0, EDID_TABLE_LEN );//更新EDID数据
					SiiRegWrite ( cInstance,0x0010, 0x55 );//将热插拔拉高，这样的话再次拉高那么信号源就会重新读取
				}
				else
				{			
						SiiRegWrite (cInstance, 0x0010, 0x00 );//将热插拔拉低 
						delay_nms(100);
						SiiDrvNvramSramWrite( cInstance,  SRAM_P2, init_edidFlashEdidTable, 0, EDID_TABLE_LEN );//更新EDID数据			
						SiiRegWrite (cInstance, 0x0010, 0x55 );//将热插拔拉高，这样的话再次拉高那么信号源就会重新读取				
				}
*/
				
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Remove one of the input EDIDs and re-compose US EDID.
//!
//!             This function shall be called upon downstream disconnection event.
//!
//! @param[in]  input - EDID TX input accepting the EDID data from TX. Should be different
//!                     for different TX instances in multi-input model.
//-------------------------------------------------------------------------------------------------

void SiiEdidTxDismissEdid(  BYTE cInstance, uint8_t input )
{
    if (SiiEdidTxInputSet( cInstance, input))
    {
        pEdidTx[cInstance]->lastResultCode = SI_TX_EDID_ERROR_CODE_NO_US_EDID_CREATED;

        if (pEdidTx[cInstance]->pEdidDecodData->isActive)
        {
////            if (pEdidTx[cInstance]->numOfActiveTx > 0)// for melbourne we have only one TX.
////            {
//                pEdidTx[cInstance]->numOfActiveTx--;
//            //}
            // Deactivate instance when TX is disconnected
            pEdidTx[cInstance]->pEdidDecodData->isActive = false;
            // Re-make US EDID based on already parsed data if at least one active DS EDID is left
//            if (pEdidTx[cInstance]->numOfActiveTx > 0)
//            {
//                EdidTxCreateUsEdid();
//                pEdidTx[cInstance]->lastResultCode = SI_TX_EDID_ERROR_CODE_NO_ERROR;
//                return;
//            }
        }

    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return CEC physical address of downstream device.
//!
//-------------------------------------------------------------------------------------------------

uint16_t SiiEdidTxCecPhysAddrGet( BYTE cInstance)
{
   uint16_t  wPhysAddr = 0;
   
    wPhysAddr =
	((((uint16_t) pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.CEC.a) << 12) |
             (((uint16_t) pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.CEC.b) << 8)  |
             (((uint16_t) pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.CEC.c) << 4)  |
             ((uint16_t)  pEdidTx[cInstance]->pEdidDecodData->edidInfo.HDMI_VSDB.CEC.d));
	DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "---SiiEdidTxCecPhysAddrGet() = 0x%04x\n",wPhysAddr);
	return wPhysAddr;

}


void EdidBlockDataDisplay(BYTE cInstance, uint8_t *pEdidData)
{
    uint8_t i;
    
    for (i = 0; i<EDID_BLOCK_SIZE; i++)
    {
	  DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, "%02x ",(int)pEdidData[i]);
        if (0 == ((i+1)%16))
        {
		DEBUG_PRINT2(EDID_TX_MSG_DBG_PLAIN, ("\r\n"));
        }
    }
}
//-------------------------------------------------------------------------------------------------
//! @brief      Read the specified single block from the specified EDID source.
//!
//! @param[in]  blockIndex - zero based EDID block number,
//! @param[in]  pEdidBlock - pointer to a buffer for the EDID block data
//!
//! @return     Success flag.
//-------------------------------------------------------------------------------------------------
//通过DDC 通道读取TX 端的EDID
bool_t EdidTxReadBlock (  BYTE cInstance, int blockIndex, uint8_t *pEdidBlock )
{
    bool_t success = false;
	//uint16_t i=0;
    BYTE i = 0;
   uint8_t *pEdidBlocktmp =  pEdidBlock;
   for(i = 0; i< 5; i++)
   {
    // NOTE: correct TX instance must be selected for the read to happen from the right DDC bus
    success = (SI_TX_DDC_ERROR_CODE_NO_ERROR == SiiDrvTpiDdcReadBlock(  cInstance ,blockIndex / 2,
                                                        (blockIndex & 1) ? 128 : 0,
                                                        pEdidBlock, EDID_BLOCK_SIZE ));	 
	if(success)
		break;
	else
		SiiOsTimerWait(2); // 10
	assert_param(pEdidBlock == pEdidBlocktmp );// for test
	DEBUG_PRINT(MSG_ERR,"---SiiDrvTpiDdcReadBlock() Fail\r\n");
   }
	
#if 0  // 搞不懂什么意思
	success = true;
	if((cInstance==0)||(cInstance==1)||(cInstance==2)||(cInstance==3))
	{
		if(blockIndex==0)
		{
			Read_EDID(INPUT_EDID_add + 256*cInstance,g_tempData);
			for(i=0;i<EDID_BLOCK_SIZE;i++)
			{
				*(pEdidBlock+i)= init_edidFlashEdidTable [ i ];//g_tempData [ i ];
			}
		//		UART1_SendBytes("use port edid\r\n",strlen("use port edid\r\n"));
		}
		else if(blockIndex==1)
		{
			Read_EDID(INPUT_EDID_add + 256*cInstance,g_tempData);
			for(i=0;i<EDID_BLOCK_SIZE;i++)
			{
				*(pEdidBlock+i)= init_edidFlashEdidTable [ 128+i ];//g_tempData [ 128+i ];//
			}	
		//	UART1_SendBytes("use port edid\r\n",strlen("use port edid\r\n"));
		}
	}
	else 
	{
	 	
		/*success = (SI_TX_DDC_ERROR_CODE_NO_ERROR == SiiDrvTpiDdcReadBlock( blockIndex / 2,
                                                        (blockIndex & 1) ? 128 : 0,
                                                       pEdidBlock, EDID_BLOCK_SIZE ));*/
		
		
		if(blockIndex==0)
		{
			//Read_EDID(INPUT_EDID_add + 256*DeviceID,g_tempData);
			for(i=0;i<EDID_BLOCK_SIZE;i++)
			{
				*(pEdidBlock+i)= init_edidFlashEdidTable [ i ];				
			}
		//		UART1_SendBytes("use internal edid\r\n",strlen("use internal edid\r\n"));
		}
		else if(blockIndex==1)
		{
			//Read_EDID(INPUT_EDID_add + 256*DeviceID,g_tempData);
			for(i=0;i<EDID_BLOCK_SIZE;i++)
			{
				*(pEdidBlock+i)= init_edidFlashEdidTable [ 128+i ];
			}	
		//	UART1_SendBytes("use internal edid\r\n",strlen("use internal edid\r\n"));
		}
	
		
		success = true;
	}
	
	#if Uart_Debug
	if(blockIndex==0)
	{
		UART1_SendBytes("EDID0:\r\n",strlen("EDID0:\r\n"));
		for(i=0;i<EDID_BLOCK_SIZE;i++)
		{
			UART1_SendByte(*(pEdidBlock+i));
		}
		UART1_SendBytes("\r\n",strlen("\r\n"));
	}
	else if(blockIndex==1)
	{
		UART1_SendBytes("EDID1:\r\n",strlen("EDID1:\r\n"));
		for(i=0;i<EDID_BLOCK_SIZE;i++)
		{
			UART1_SendByte(*(pEdidBlock+128+i));
		}
		UART1_SendBytes("\r\n",strlen("\r\n"));	
	}
	#endif
#endif
    if ( success )
    {
        // Verify checksum
        if ( !EdidTxCheckBlockCheckSum( pEdidBlock ))
        {
            pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__CHECK_SUM_MISMATCH;
        }
    }
   else
   	{
           DEBUG_PRINT(EDID_TX_MSG_DBG_PLAIN, "***[%d]EdidTxReadBlock() FAIL\r\n",cInstance);
   	}

    return( success );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Write an EDID block prepared by the EDID Composer to Rx EDID memory that
//!             is exposed to the upstream.
//!
//!             IMPORTANT: This function requires pEdidBlock to point to 128 byte array (1 EDID block).
//!             Some bytes of this array are going to be modified (CEC PA and Check Sum).
//!
//! @param[in]  blockNumber - zero based EDID block number,
//! @param[in]  pEdidBlock  - pointer to a 128-byte size buffer of EDID block,
//!
//! @return     success flag.
//-------------------------------------------------------------------------------------------------

bool_t EdidTxWriteBlockToRx(BYTE cInstance, int blockNumber, uint8_t *pEdidBlock, uint8_t cecPhysAddrLocation)
{
    bool_t status = SiiTxEdidCbBlockWrite( cInstance ,blockNumber, pEdidBlock, cecPhysAddrLocation);

    return( status );
}


// TODO
void SiiTxEdidGetEdidData(BYTE cInstance,PBYTE cpEdid)
{
   //EdidTxDecodeEdid(cInstance);
   memcpy(cpEdid,pEdidTx[cInstance]->pacEdidTxData,256);
}


