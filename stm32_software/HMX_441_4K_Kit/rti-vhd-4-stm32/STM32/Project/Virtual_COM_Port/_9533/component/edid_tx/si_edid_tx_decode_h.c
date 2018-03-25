//************************************************************************************************//
//! @file   si_edid_tx_decode_h.c
//! @brief  EDID parsing and decoding. High-level API
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***********************************************************************************************/


#include <string.h>
#include <stdio.h>
#include "si_edid_tx_component.h"
#include "si_edid_tx_internal.h"
#include "si_edid_black_list.h"
#include "si_drv_tpi_ddc_access.h"

#include "Mcu.h"
#include "uart.h"

//-------------------------------------------------------------------------------------------------
//! @brief      Check correctness of the block 0 header.
//!
//! @param[in]  aBlock - pointer to EDID block 0 array
//!
//! @retval     true  - success
//! @retval     false - header is broken
//-------------------------------------------------------------------------------------------------

static bool_t CheckEdidBlock0Header(uint8_t aBlock[EDID_BLOCK_SIZE])
{
    // Check EDID header
    if ( 0 != memcmp(&aBlock[EDID_BL0_ADR__HEADER], &defaultDsEdid[EDID_BL0_ADR__HEADER], EDID_BL0_LEN__HEADER) )
        return false;
    return true;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Check correctness of the block 0 version field.
//!
//! @param[in]  aBlock - pointer to EDID block 0 array
//!
//! @retval     true  - success
//! @retval     false - header is broken
//-------------------------------------------------------------------------------------------------

static bool_t CheckEdidBlock0Version(uint8_t aBlock[EDID_BLOCK_SIZE])
{
    // Check EDID version
    if (aBlock[EDID_BL0_ADR__VERSION] != 1) // only 1.x versions are allowed (not 2.0)
        return false;
    return true;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Decode the passed EDID block 0.
//!
//! @param[in]  aBlock - pointer to EDID block 0 array
//-------------------------------------------------------------------------------------------------

void EdidTxParseBlock0 ( BYTE cInstance, uint8_t aBlock[EDID_BLOCK_SIZE] )
{

#ifdef COLLECT_EXCESSIVE_INFO
    pEdidTx[cInstance]->pEdidDecodData->Revision = aBlock[EDID_BL0_ADR__REVISION];
#else // COLLECT_EXCESSIVE_INFO
    pEdidTx[cInstance]->pEdidDecodData->Revision = aBlock[EDID_BL0_ADR__REVISION];
#endif // COLLECT_EXCESSIVE_INFO

    pEdidTx[cInstance]->pEdidDecodData->edidInfo.ManufacturerId =
        (aBlock[EDID_BL0_ADR__MANUFACTURER_ID] << 8) |
        aBlock[EDID_BL0_ADR__MANUFACTURER_ID+1];
    pEdidTx[cInstance]->pEdidDecodData->edidInfo.ProductId =
        aBlock[EDID_BL0_ADR__PRODUCT_ID] |
        (aBlock[EDID_BL0_ADR__PRODUCT_ID+1] << 8);

#ifdef COLLECT_EXCESSIVE_INFO
    pEdidTx[cInstance]->pEdidDecodData->edidInfo.ShortSerialNumber =
        aBlock[EDID_BL0_ADR__SHORT_SERIAL_NUMBER] |
        (aBlock[EDID_BL0_ADR__SHORT_SERIAL_NUMBER+1] << 8) |
        (aBlock[EDID_BL0_ADR__SHORT_SERIAL_NUMBER+2] << 16) |
        (aBlock[EDID_BL0_ADR__SHORT_SERIAL_NUMBER+3] << 24);
#endif // COLLECT_EXCESSIVE_INFO

    pEdidTx[cInstance]->pEdidDecodData->edidInfo.ManufactureWeek = aBlock[EDID_BL0_ADR__MANUFACTURE_WEEK];
    pEdidTx[cInstance]->pEdidDecodData->edidInfo.ManufactureYear = aBlock[EDID_BL0_ADR__MANUFACTURE_YEAR];
    pEdidTx[cInstance]->pEdidDecodData->edidInfo.EstablishedTiming1 = aBlock[EDID_BL0_ADR__ESTABLISHED_TIMING_1];
    pEdidTx[cInstance]->pEdidDecodData->edidInfo.EstablishedTiming2 = aBlock[EDID_BL0_ADR__ESTABLISHED_TIMING_2];
    pEdidTx[cInstance]->pEdidDecodData->edidInfo.ManufacturerReservedTiming = aBlock[EDID_BL0_ADR__MANUFACTURER_RESERVED_TIMING];

    memcpy( pEdidTx[cInstance]->pEdidDecodData->edidInfo.BasicDispayParameters,
        &aBlock[EDID_BL0_ADR__BASIC_DISPLAY_PARAMS],
        EDID_BL0_LEN__BASIC_DISPLAY_PARAMS );
    memcpy( pEdidTx[cInstance]->pEdidDecodData->edidInfo.Chromaticity,
        &aBlock[EDID_BL0_ADR__CHROMATICITY],
        EDID_BL0_LEN__CHROMATICITY );
    memcpy( pEdidTx[cInstance]->pEdidDecodData->edidInfo.StandardTiming,
        &aBlock[EDID_BL0_ADR__STANDARD_TIMING],
        EDID_BL0_LEN__STANDARD_TIMING );

    if ( 0 == ( pEdidTx[cInstance]->pEdidDecodData->edidInfo.BasicDispayParameters[0] & 0x80) )
    {
        // EDID is marked as for analog TV
        // (some of DVI TVs have this bug)
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.BasicDispayParameters[0] = 0x80;
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__BLOCK_0_VIOLATION;
    }

    EdidTxDecodeDtd( cInstance, &aBlock[EDID_BL0_ADR__DTD1]);
    EdidTxDecodeDtd( cInstance, &aBlock[EDID_BL0_ADR__DTD2]);
    EdidTxDecodeDtd( cInstance, &aBlock[EDID_BL0_ADR__DTD3]);
    EdidTxDecodeDtd( cInstance, &aBlock[EDID_BL0_ADR__DTD4]);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Load block 0 of the selected EDID and verify that it is at least
//!             nominally a valid block 0 EDID.  If not, replace it with the
//!             default.  Decode the result.
//!
//! @param[in]  pEdid - pointer to EDID block 0.
//-------------------------------------------------------------------------------------------------
// 读取TX  Edid Block0, 不对会用defaultDsEdid  edid.
//   中间会读Block1
static void EdidDecodeBlock0 ( BYTE cInstance, uint8_t *pEdid )
{
//    bool_t replaceEdidwithDefault = false;

    for ( ;; )
    {
        // Read the EDID. Give it two chances.

        if ( !EdidTxReadBlock( cInstance,  0, pEdid ))
        {
            if ( !EdidTxReadBlock(  cInstance, 0, pEdid))
            {
                pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__CANNOT_READ_BL0 | EDID_ERROR__DEFAULT_EDID_USED;
                break;
            }
        }

        /* Verify that block 0 of the EDID is nominally a real block 0 EDID.    */

        if ( pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode & EDID_ERROR__CHECK_SUM_MISMATCH )
        {
            if ( !pEdidTx[cInstance]->parseOptions.AllowCheckSumMismatchForBlock0)
            {
                pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__DEFAULT_EDID_USED;
                break;
            }
        }
        if ( !CheckEdidBlock0Header(pEdid))
        {
            pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__INCORRECT_HEADER | EDID_ERROR__DEFAULT_EDID_USED;
        }
        if ( !CheckEdidBlock0Version(pEdid))
        {
            pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__INCORRECT_VERSION | EDID_ERROR__DEFAULT_EDID_USED;
        }
        break;
    }

    // If a checksum error occurred but is not fatal (parseOptions.AllowCheckSumMismatchForBlock0 == true),
    // AND the EDID has more than one block, we can ignore the checksum if the checksum for block 1 is OK.
    // This chunk of code reads in the second block and verifies the checksum.  If it cannot read the second
    // block, or the second block checksum is bad, force use of the default block 0 EDID.

    if (( pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode & EDID_ERROR__CHECK_SUM_MISMATCH ) &&
       !( pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode & EDID_ERROR__DEFAULT_EDID_USED ))
    {
        // Multiple extensions were specified.

        if ( 0 != pEdid[EDID_BL0_ADR__EXTENSION_NMB]) 
        {
            // If block 0 had mismatched CS, ensure that at least one other block has correct CS
            if ( !EdidTxReadBlock( cInstance,  1, pEdid ))
            {
                pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__CANNOT_READ_EXT | EDID_ERROR__DEFAULT_EDID_USED;
            } 

            // If there was a checksum error this time, it's fatal.

            else if ( pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode & EDID_ERROR__CHECK_SUM_MISMATCH )
            {
                pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__DEFAULT_EDID_USED;
            }
            else
            {
                EdidTxReadBlock( cInstance,  0, pEdid );     // No problems with block 1, re-read block 0 into edid array.
            }
        }

        // No extensions, use basic DVI default EDID.

        else
        {
            pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__DEFAULT_EDID_USED;
        }
    }

    // Replace read EDID with default one if suggested.  We assume the default has none
    // of the problems listed above.

    if ( pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode & EDID_ERROR__DEFAULT_EDID_USED )
    {
       DEBUG_PRINT(MSG_ERR,"--- USE defaultDsEdid\r\n");
        memcpy( pEdid, defaultDsEdid, EDID_BLOCK_SIZE);
    }

    // Decode the resulting block 0 EDID

    EdidTxParseBlock0( cInstance,  pEdid );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Load and decode an extension EDID block.
//!
//! @param[in]  blockIdx - EDID block index,
//! @param[in]  pEdid    - pointer to EDID block
//-------------------------------------------------------------------------------------------------

static void EdidDecodeBlockExtension (  BYTE cInstance, uint8_t blockIdx, uint8_t *pEdid )
{
    if ( !EdidTxReadBlock( cInstance,  blockIdx, pEdid))
    {
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__CANNOT_READ_EXT;
        DEBUG_PRINT( MSG_ERR, "\nCannot read EDID Block %d\r\n", (int)blockIdx);
    }
    else
    {
        EdidTxParseBlock(  cInstance , pEdid, blockIdx );
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Parse downstream EDID into an internal structure.
//!             Also performs some basic error checking on the EDID.
//-------------------------------------------------------------------------------------------------

void EdidTxDecodeEdid (  BYTE cInstance )
{
    uint8_t edid[EDID_BLOCK_SIZE];
	int    extensionCount = 0;
    DEBUG_PRINT2( EDID_TX_MSG_DBG, "EdidTxDecodeEdid() %d\n",cInstance );
    // Configuration may force the DVI mode in the output
    pEdidTx[cInstance]->pEdidDecodData->edidInfo.isHdmiMode = !pEdidTx[cInstance]->parseOptions.DviInput;

    // In Sound Bar mode, we take the basic properties, including video, from
    // the local RX EDID and the Audio properties from the downstream (Sound Bar) EDID
	// when we would normally take everything from the downstream EDID

    pEdidTx[cInstance]->isDecodeAudio        = true;
    pEdidTx[cInstance]->isDecodeNonAudio     = true;
    if ( pEdidTx[cInstance]->isSoundBarMode )
    {
        pEdidTx[cInstance]->isDecodeAudio = false;
    }
    
    EdidDecodeBlock0( cInstance, edid);
    memcpy(pEdidTx[cInstance]->pacEdidTxData,edid,EDID_BLOCK_SIZE); //[kyq add: 20150728 15:15:48]
    EdidBlockDataDisplay(cInstance,edid);
    extensionCount = edid[EDID_BL0_ADR__EXTENSION_NMB];

    DEBUG_PRINT( EDID_TX_MSG_DBG, "DS blocks read: %d \n", extensionCount  );

    if ( pEdidTx[cInstance]->isSoundBarMode )
    {
        // If there is at least 1 extension block, decode NON-audio from NVRAM EDID block 1

        if ( extensionCount > 0 )
        {
            EdidDecodeBlockExtension( cInstance, 1, edid );
        }

        // The rest of the decoding is audio only, from the DS EDID
        pEdidTx[cInstance]->isDecodeAudio        = true;
        pEdidTx[cInstance]->isDecodeNonAudio     = false;
    }

    // One extension is simple, just decode it.

    if (( extensionCount == 1)
       /*&&(pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode & EDID_ERROR__CANNOT_READ_EXT == 0)*/) // kyq add
    {
        EdidDecodeBlockExtension(  cInstance,1, edid );
	 memcpy(pEdidTx[cInstance]->pacEdidTxData+EDID_BLOCK_SIZE,edid,EDID_BLOCK_SIZE);//[kyq add: 20150728 15:15:50]
	 EdidBlockDataDisplay(cInstance,edid);

    }

    // If more than 1 extension, the first extension must be a map block,
    // so read it first, then decode the rest of the extensions.

    else if ( extensionCount > 1 )
    {
        uint8_t map[EDID_BLOCK_SIZE];
        if ( !EdidTxReadBlock( cInstance,  1, map ))
        {
            pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__CANNOT_READ_EXT;
        }
        else
        {
            if ( EDID_EXT_TAG__BLOCK_MAP != map[0])
            {
                pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__INCORRECT_MAP_TAG;
            }
            else
            {
                int blockIndex;
                if (extensionCount > 127) // 126 timings extensions + one map extension
                {
                    extensionCount = 127;
                    pEdidTx[cInstance]->pEdidDecodData->edidInfo.ErrorCode |= EDID_ERROR__TOO_MANY_EXTENSIONS;
                }
                // block 0 is the main block
                // block 1 is a map
                // block 2 ... nmb_of_extension+1  are timings extensions
                for (blockIndex = 2; blockIndex <= extensionCount; blockIndex++)
                {
                    if (EDID_EXT_TAG__ADDITIONAL_TIMINGS == map[blockIndex-1])
                    {
                        EdidDecodeBlockExtension(  cInstance,blockIndex, edid );
                    }
                }
            }
        }
    }

    // if no extension, it must be a DVI EDID

    else 
    {
        // DVI monitor case.
        // Very often DVI monitors support more modes then declared in their EDIDs.
        // Probably it is due to they use just one EDID block and there is not enough
        // room to list all the modes.
        // Many existing players know that and allow playing
        // some additional modes even though they are not indicated by the monitor EDID.

        DEBUG_PRINT2( EDID_TX_MSG_DBG, "DS DVI EDID Detected, adding DVI video modes\n" );

        pEdidTx[cInstance]->pEdidDecodData->edidInfo.isHdmiMode = false;

        EdidTxAddDviVideoModes( cInstance);
    }

    EdidTxPostDecode( cInstance );

    // Check for remedies
    if ( pEdidTx[cInstance]->parseOptions.enableRemedies)
    {
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.RemedyMask = EdidTxRemedyMaskBuild( &pEdidTx[cInstance]->pEdidDecodData->edidInfo );
    }

    // make a correction if required
    if ( pEdidTx[cInstance]->pEdidDecodData->edidInfo.RemedyMask & SI_TX_EDID_REMEDY_DO_NOT_USE_YCBCR)
    {
        pEdidTx[cInstance]->pEdidDecodData->edidInfo.SupportYCbCr = false;
    }

}
