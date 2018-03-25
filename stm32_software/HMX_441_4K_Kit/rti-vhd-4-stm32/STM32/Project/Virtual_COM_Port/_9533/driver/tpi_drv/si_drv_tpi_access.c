//***************************************************************************
//! @file     si_drv_tpi_access.c
//! @brief    TPI and Legacy registers access
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_tx_component_internal.h"
#include "si_drv_tpi_access.h"
//TPI: Transmitter Programming interface
 

//-------------------------------------------------------------------------------------------------
//  Module Instance Data
//-------------------------------------------------------------------------------------------------

txAccessInstanceRecord_t txAccessInstRec[INSTANCE_9533][SII_NUM_TX];
txAccessInstanceRecord_t *pTxRegAccess[INSTANCE_9533];// = &txAccessInstRec[0][0];//[kyq Begin: 20140915 17:09:28]

//-------------------------------------------------------------------------------------------------
//  Local Service Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Set a page to access legacy registers through an indirect access mechanism.
//-------------------------------------------------------------------------------------------------

static void SetIndirectLegacyPage(BYTE cInstance, uint8_t currentPage)
{
    if (pTxRegAccess[cInstance]->previouslyAccessedLegacyPage != currentPage)
    {
        SiiRegWrite(cInstance,REG_TPI__REG_SEL, currentPage & MSK_TPI__REG_SEL__PAGE);
        pTxRegAccess[cInstance]->previouslyAccessedLegacyPage = currentPage;
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Set an offset to access legacy registers through an indirect access mechanism.
//-------------------------------------------------------------------------------------------------

static void SetIndirectLegacyOffset(BYTE cInstance, uint8_t offset)
{
    if (pTxRegAccess[cInstance]->previouslyAccessedLegacyOffset != offset)
    {
        SiiRegWrite(cInstance,REG_TPI__REG_OFFSET, offset);
        pTxRegAccess[cInstance]->previouslyAccessedLegacyOffset = offset;
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Extract legacy page local address to be used for indirect accessing.
//!
//! @param[in]  fullAddr - 16-bit full address containing a page code and offset.
//-------------------------------------------------------------------------------------------------

static uint8_t ExtractLegacyPage(uint16_t fullAddr)
{
    uint8_t page;

    switch ( fullAddr & 0xFF00 )
    {
        default:
        case TX_PAGE_L0:
            page = VAL_TPI__REG_SEL__PAGE_L0;
            break;

        case TX_PAGE_L1:
            page = VAL_TPI__REG_SEL__PAGE_L1;
            break;

        case TX_PAGE_2:
            page = VAL_TPI__REG_SEL__PAGE_L2;
            break;
    }

    return page;
}

//-------------------------------------------------------------------------------------------------
//  Module API Functions
//-------------------------------------------------------------------------------------------------
void  SiiDrvTpiAccessStructInstanceSet(BYTE cInstance,uint8_t device_idx)
{
	if(device_idx < SII_NUM_TX)
    {
        pTxRegAccess[cInstance] = &txAccessInstRec[cInstance][device_idx];
    }	
}


//-------------------------------------------------------------------------------------------------
//! @brief      Select Tx device instance in multi-transmitter system.
//!
//! @param[in]  device_idx - zero based index of an instance.
//-------------------------------------------------------------------------------------------------

#if SII_NUM_TX > 1
void SiiDrvTpiAccessInstanceSet( BYTE cInstance, uint8_t device_idx)
{
    if(device_idx < SII_NUM_TX)
    {
        pTxRegAccess[cInstance] = &txAccessInstRec[cInstance][device_idx];
    }
}
#endif // SII_NUM_TX > 1


//-------------------------------------------------------------------------------------------------
//! @brief      Bring TPI access to transmitter into initial operational state.
//!
//!             All initial settings, including the software workarounds
//!             that must be implemented through the legacy registers,
//!             shall be done in this function.
//-------------------------------------------------------------------------------------------------

void SiiDrvTpiAccessInit(BYTE cInstance)
{
    // Set non-existing offset and page to never miss the very first setting
    pTxRegAccess[cInstance]->previouslyAccessedLegacyOffset = 0xFFFF;
    pTxRegAccess[cInstance]->previouslyAccessedLegacyPage   = 0xFF;
    // Legacy TPI mode is set by power on
    pTxRegAccess[cInstance]->isTpiMode = false;

    // Enable TPI access mode for normal operation
    SiiDrvTpiAccessEnable(cInstance,true);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Switch between TPI and Legacy access modes.
//!
//!             Underlying the TPI register set is a broader and more complex internal
//!             register set that is normally hidden from direct access.  This register set
//!             includes the “legacy” registers that are available for implementation
//!             of software workarounds and debugging.
//!
//! @param[in]  isTpi - true, if TPI mode is selected.
//-------------------------------------------------------------------------------------------------

void SiiDrvTpiAccessEnable( BYTE cInstance,bool_t isTpi)
{
    if (isTpi)
    {
        if (!pTxRegAccess[cInstance]->isTpiMode)
        {
            SiiRegWrite(cInstance,REG_TX_TPI__ENABLE, BIT_TX_TPI__ENABLE);
            pTxRegAccess[cInstance]->isTpiMode = true;
        }
    }
    else
    {
        if (pTxRegAccess[cInstance]->isTpiMode)
        {
            SiiRegWrite(cInstance,REG_TPI__REG_SEL, BIT_TPI__REG_SEL__LEGACY_ENABLE);
            pTxRegAccess[cInstance]->isTpiMode = false;
        }
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Write byte to the Legacy registers through a window in TPI page.
//!
//!             Uses indirect access method to write a byte to the Legacy registers
//!             without disabling the TPI mode.
//!
//! @param[in]  regAddr     - Legacy register full address including page and offset,
//! @param[in]  data        - byte to write,
//-------------------------------------------------------------------------------------------------

void SiiDrvTpiIndirectWrite( BYTE cInstance,uint16_t regAddr, uint8_t data)
{
    SiiDrvTpiIndirectBlockWrite(cInstance,regAddr, &data, 1);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Read byte from the Legacy registers through a window in TPI page.
//!
//!             Uses indirect access method to read a byte to the Legacy registers
//!             without disabling the TPI mode.
//!
//! @param[in]  regAddr     - Legacy register full address including page and offset,
//! @return     content of the register
//-------------------------------------------------------------------------------------------------

uint8_t SiiDrvTpiIndirectRead( BYTE cInstance, uint16_t regAddr)
{
    uint8_t data;

    SiiDrvTpiIndirectBlockRead( cInstance , regAddr, &data, 1);
    return data;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Write a block of data to the Legacy registers through a window in TPI page.
//!
//!             Uses indirect access method to write data array to the Legacy registers
//!             without disabling the TPI mode.
//!
//! @param[in]  regAddr     - Legacy register full address including page and offset,
//! @param[in]  pData       - Pointer to an input buffer of bytes,
//! @param[in]  length      - Length of the input buffer
//-------------------------------------------------------------------------------------------------

void SiiDrvTpiIndirectBlockWrite( BYTE cInstance, uint16_t regAddr, uint8_t *pData, uint8_t length)
{
    uint8_t page    = ExtractLegacyPage(regAddr);
    uint8_t offset  = regAddr & 0xFF;

    SetIndirectLegacyPage( cInstance ,page);
    for (; length; length--)
    {
        SetIndirectLegacyOffset( cInstance ,offset);
        SiiRegWrite( cInstance ,REG_TPI__REG_DATA, *pData);
        pData++;
        offset++;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Read a block of data to the Legacy registers through a window in TPI page.
//!
//!             Uses indirect access method to read data array from the Legacy registers
//!             without disabling the TPI mode.
//!
//! @param[in]  regAddr     - Legacy register full address including page and offset,
//! @param[in]  pData       - Pointer to an input buffer of bytes,
//! @param[in]  length      - Length of the input buffer
//-------------------------------------------------------------------------------------------------

void SiiDrvTpiIndirectBlockRead( BYTE cInstance,uint16_t regAddr, uint8_t *pData, uint8_t length)
{
    uint8_t page    = ExtractLegacyPage(regAddr);
    uint8_t offset  = regAddr & 0xFF;

    SetIndirectLegacyPage( cInstance , page);
    for (; length; length--)
    {
        SetIndirectLegacyOffset( cInstance ,offset);
        *pData = SiiRegRead( cInstance ,REG_TPI__REG_DATA);
        pData++;
        regAddr++;
    }
}

