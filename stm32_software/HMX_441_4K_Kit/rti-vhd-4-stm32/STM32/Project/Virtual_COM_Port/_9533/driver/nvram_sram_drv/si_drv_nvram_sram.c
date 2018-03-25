//***************************************************************************
//!file     si_drv_nvram_sram.c
//!brief    Silicon Image NVRAM and SRAM driver.  Contains functions for
//          reading and writing Silicon Image device NVRAM and port SRAMS
//          as well as boot data.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "string.h"         // For memcpy
#include "si_osal_timer.h"
#include "si_common.h"
#include "si_drv_edid_rx_config.h"
#include "si_drv_nvram_sram.h"
#include "si_drv_edid_rx_internal.h"
#include "si_cra.h"
#include "si_device_config.h"
#include "si_regs_rxedid953x.h"
#include "si_regs_pp953x.h"

#include "Mcu.h"
#include "uart.h"

//------------------------------------------------------------------------------
//  Module Data
//! \addtogroup si_drv_nvram_sram
//! @{
//------------------------------------------------------------------------------
NvramDrvInstanceData_t nvramDrvData[INSTANCE_9533];
//NvramDrvInstanceData_t *pDrvNvram = &nvramDrvData;


//-------------------------------------------------------------------------------------------------
//! @brief      Execute the passed NVRAM command.  Does not wait for command to complete.
//!
//! @param[in]  command - NVRAM command
//-------------------------------------------------------------------------------------------------
static bool_t SendNvramCommand (  BYTE cInstance, uint8_t command, bool_t isSynchronous )
{
    SiiTimer_t  programTimer;
    uint8_t     test;

    // Enable Replacing of BSM registers with alternate values
    SiiRegWrite( cInstance,  REG_NVM_BSM_REPLACE, 0x01 );

    SiiRegBitsSet(  cInstance, REG_SYS_RESET_2, BIT_NVM_SRST, true );      // NVM Soft Reset
    SiiRegBitsSet(  cInstance, REG_SYS_RESET_2, BIT_NVM_SRST, false );     // Release NVM soft reset

    SiiRegWrite( cInstance,  REG_NVM_COMMAND, command );    // Start the NVRAM program operation
	test = SiiRegRead(  cInstance, REG_NVM_COMMAND );
	 #if Uart_Debug
	 UART1_SendByte(test);
	 #endif
    if ( isSynchronous )
    {
        SiiOsTimerSet( &programTimer, mS_TO_TICKS(4000) );        //200  Set 4 second timeout
        for ( ;; )
        {
		    test = SiiRegRead( cInstance,  REG_NVM_COMMAND_DONE );
			IWDG_ReloadCounter();//喂狗
            if ( test & BIT_NVM_COMMAND_DONE )
                break;
            if ( SiiOsTimerExpired( programTimer ))
            {
                return( false );
            }
        }
    }

    return( true );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns the status of the last NVRAM command initiated.
//!
//! @returns    true - No command is in process
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvEdidRxIsNvramIdle (   BYTE cInstance  )
{

    return(( SiiRegRead( cInstance,  REG_NVM_COMMAND_DONE ) & BIT_NVM_COMMAND_DONE ) == BIT_NVM_COMMAND_DONE );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns the NVRAM BOOT DONE status.
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvEdidRxIsBootComplete (BYTE cInstance, bool_t isSynchronous )
{
    SiiTimer_t  bootTimer;
    uint8_t     test;

    nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_SUCCESS;

    test = SiiRegRead( cInstance, REG_BSM_STAT ); // BIT2  :1 – NVRAM boot done. When set
    if ( isSynchronous )
    {
        /// Wait for boot loading to be done.
        SiiOsTimerSet( &bootTimer, mS_TO_TICKS(4000));// 200 );  // Wait up to 4 seconds //[kyq Begin: 20150717 11:53:41]
        while (( test & BIT_BOOT_DONE ) == 0 )
        {
	   if (( test & BIT_BOOT_ERROR ) || SiiOsTimerExpired( bootTimer ))
            {
                nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_ERR_FAIL_BOOT;
                break;
            }
            test = SiiRegRead(cInstance,  REG_BSM_STAT );
        }
    }

    return( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Read data from the requested SRAM.
//  Parameters:
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
//  把9533 里的SRAM 读到指定的buffer 中
bool_t SiiDrvNvramSramRead (  BYTE cInstance, SiiSramType_t sramType, uint8_t *pDest, int_t offset, int_t length )
{
    uint8_t ramSelect = 0;

    nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_SUCCESS;
    switch ( sramType )
    {
        case SRAM_P0:
        case SRAM_P1:
        case SRAM_P2:
        case SRAM_P3:
        case SRAM_P4:
        case SRAM_P5:
        case SRAM_VGA:
            ramSelect = (uint8_t)sramType;
            break;
        case SRAM_BOOT:
            ramSelect = BIT_SEL_DEVBOOT;
            break;

        default:
            nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_ERR_INVALID_PARAMETER;
            break;
    }

    if ( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS )
    {
        // Point to offset into selected port SRAM.
        SiiRegModify(  cInstance, REG_EDID_FIFO_SEL, MSK_SEL_EDID_FIFO | BIT_SEL_DEVBOOT, ramSelect );
        SiiRegWrite( cInstance,  REG_EDID_FIFO_ADDR, offset );

        // Read it into the destination.
        SiiRegReadBlock( cInstance,REG_EDID_FIFO_DATA, pDest, length );
    }

    return( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Write data to the requested SRAM.
//  Parameters:
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvNvramSramWrite ( BYTE cInstance,SiiSramType_t sramType, uint8_t *pSrc, int_t offset, int_t length )
{
    uint8_t ramSelect = 0;
	uint16_t i;

    nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_SUCCESS;
    switch (sramType)
    {
        case SRAM_P0:
        case SRAM_P1:
        case SRAM_P2:
        case SRAM_P3:
        case SRAM_P4:
        case SRAM_P5:
        case SRAM_VGA:
            ramSelect = (uint8_t)sramType;
            break;
        case SRAM_BOOT:
            ramSelect = BIT_SEL_DEVBOOT;
            break;

        default:
            nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_ERR_INVALID_PARAMETER;
            break;
    }

    if ( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS )
    {
    	//DEBUG_PRINT( MSG_ERR, "\nNVRAM: In SiiDrvNvramSramWrite() \n" );
        // Point to offset into selected port SRAM.
        SiiRegModify( cInstance, REG_EDID_FIFO_SEL, MSK_SEL_EDID_FIFO | BIT_SEL_DEVBOOT, ramSelect );
        SiiRegWrite(  cInstance,  REG_EDID_FIFO_ADDR, offset );

        // Write data into the destination.
        SiiRegWriteBlock(cInstance, REG_EDID_FIFO_DATA, pSrc, length );  //[kyq add: 20150824 18:52:14]
	 /*	for ( i = 0; i < length; i++ )                                             //[kyq cancel: 20150824 18:52:20]
	    {
	        SiiRegWrite( cInstance,  REG_EDID_FIFO_DATA, pSrc[ i] );
	    }
	    */
    }

    return( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Copy data from NVRAM to SRAM at given input port
//! @param[in]  nvramType - NVRAM_HDMI_EDID, specifies main NVRAM
//!                         NVRAM_VGA_EDID, specifies VGA EDID
//!                         NVRAM_BOOTDATA, specifies Boot Data
//!
//! @param[in]  rxPortIdx - input port index (0-6)
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvNvramCopyToSram ( BYTE cInstance, SiiNvramType_t nvramType, uint8_t rxPortIdx, bool_t isSynchronous )
{
    uint8_t nvramCommand = 0;

    if (rxPortIdx > (SII_INPUT_PORT_COUNT + 1/*VGA*/))
    {
        return( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_ERR_INVALID_PARAMETER );
    }

    nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_SUCCESS;
    switch ( nvramType )
    {
        case NVRAM_HDMI_EDID:
            nvramCommand    = VAL_COPY_EDID;
            break;
        case NVRAM_VGA_EDID:
            nvramCommand    = VAL_COPY_VGA;
            break;
        case NVRAM_BOOTDATA:
            nvramCommand    = VAL_COPY_DEVBOOT;
            break;
        default:
            nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_ERR_INVALID_PARAMETER;
            break;
    }

    if ( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS )
    {
        // Copy the NVRAM data (whatever type) into EDID ram 0.
        SiiRegWrite( cInstance,  REG_NVM_COPYTO, (BIT_NVM_COPYTO_PORT0 << rxPortIdx) );
        if ( !SendNvramCommand(cInstance, nvramCommand, isSynchronous ))
        {
            nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_ERR_FAIL;
        }
    }

    return( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Start a program operation of the specified type on the NVRAM
//! @param[in]  nvramType - NVRAM_HDMI_EDID, specifies main NVRAM
//!                         NVRAM_VGA_EDID, specifies VGA EDID
//!                         NVRAM_BOOTDATA, specifies Boot Data
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvNvramProgram ( BYTE cInstance,  SiiNvramType_t nvramType, bool_t isSynchronous )
{
    uint8_t nvramCommand = 0;

    nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_SUCCESS;
    switch ( nvramType )
    {
        case NVRAM_HDMI_EDID:
            nvramCommand    = VAL_PRG_EDID;
            break;

        case NVRAM_VGA_EDID:
            nvramCommand    = VAL_PRG_VGA;
            break;

        case NVRAM_BOOTDATA:
            nvramCommand    = VAL_PRG_DEVBOOT;
            break;
        default:
            nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_ERR_INVALID_PARAMETER;
            break;
    }

    if ( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS )
    {
    	//DEBUG_PRINT( MSG_ERR, "\nNVRAM: In SiiDrvNvramProgram()-- 1 \n" );
		
        if ( !SendNvramCommand(cInstance, nvramCommand, isSynchronous ))
        {
            nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_ERR_FAIL;
           // DEBUG_PRINT( MSG_ERR, "\nNVRAM: In SiiDrvNvramProgram()--FAILED Here \n" );
        }
        else
        {
        	//DEBUG_PRINT( MSG_ERR,"\nNVRAM: In SiiDrvNvramProgram()--SUCCESS Here \n" );
        }
    }

    return( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Read data from the NVRAM of the specified type.
//! @param[in]  nvramType - NVRAM_HDMI_EDID, specifies main NVRAM
//!                         NVRAM_VGA_EDID, specifies VGA EDID
//!                         NVRAM_BOOTDATA, specifies Boot Data
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
//
// 读取9533 里NVRAM 的EDID / BOOT
// 先把NVRAM 的数据写到SRAM, 再从SRAM 读出
bool_t SiiDrvNvramNonVolatileRead ( BYTE cInstance, SiiNvramType_t nvramType, uint8_t *pDest, int_t offset, int_t length )
{
    SiiSramType_t   sramType = (SiiSramType_t)0;

    nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_SUCCESS;
    switch ( nvramType )
    {
        case NVRAM_HDMI_EDID:
            sramType        = SRAM_P0;
            break;
        case NVRAM_VGA_EDID:
            sramType        = SRAM_P0;
            break;
        case NVRAM_BOOTDATA:
            sramType        = SRAM_BOOT;
            break;
        default:
            nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_ERR_INVALID_PARAMETER;
            break;
    }

    if ( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS )
    {
        if ( SiiDrvNvramCopyToSram( cInstance,nvramType, 0, true ))      // Wait for copy to complete
        {
            // Copy the data from SRAM into the callers destination
            SiiDrvNvramSramRead( cInstance,sramType, pDest, offset, length );
        }
    }

    return( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Program the passed data into the NVRAM of the specified TYPE
//! @param[in]  nvramType - NVRAM_HDMI_EDID, specifies main NVRAM
//!                         NVRAM_VGA_EDID, specifies VGA EDID
//!                         NVRAM_BOOTDATA, specifies Boot Data
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvNvramNonVolatileWrite ( BYTE cInstance,SiiNvramType_t nvramType, uint8_t *pSrc, int_t offset, int_t length )
{
    SiiSramType_t   sramType = (SiiSramType_t)0;

    nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_SUCCESS;
    switch ( nvramType )
    {
        case NVRAM_HDMI_EDID:
            sramType        = SRAM_P0;
            break;
        case NVRAM_VGA_EDID:
            sramType        = SRAM_P0;
            break;
        case NVRAM_BOOTDATA:
            sramType        = SRAM_BOOT;
            break;
        default:
            nvramDrvData[cInstance].lastResultCode = SII_DRV_NVRAM_ERR_INVALID_PARAMETER;
            break;
    }

    if ( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS )
    {
    	//DEBUG_PRINT( MSG_ERR, "\nNVRAM: In SiiDrvNvramNonVolatileWrite() \n" );
        // Load the source data into the appropriate SRAM
        if ( SiiDrvNvramSramWrite( cInstance, sramType, pSrc, offset, length ))
        {
        //	DEBUG_PRINT( MSG_ERR, "\nNVRAM: In SiiDrvNvramNonVolatileWrite()-> SRAM WRITE SUCESS \n" );
            // Start the NVRAM program operation and wait for it to finish.
            SiiDrvNvramProgram( cInstance, nvramType, true );

        	if(nvramType == NVRAM_BOOTDATA)
        	{
        		// Enable Replcing of BSM registers with alternate values
        		SiiRegWrite( cInstance,  REG_NVM_BSM_REPLACE, 0x01 );
        		// Write Second Byte of Boot Data
        		SiiRegWrite( cInstance,  REG_NVM_REPLACE_BYTE1, (/*pSrc[1] | */0x8C) );//(pSrc[1] | 0x80)

#if 0
        		// Write First Byte of Boot Data
        		SiiRegWrite( cInstance,  REG_NVM_REPLACE_BYTE0, 0x5F ); //pSrc[0]



        		// Write Third Byte of Boot Data
        		SiiRegWrite( cInstance,  REG_NVM_REPLACE_BYTE2, 0x06 );


        		// Write Fourth Byte of Boot Data
        		SiiRegWrite( cInstance,  REG_NVM_REPLACE_BYTE3, 0x03 ); //pSrc[3]
#endif
        	}
        }
    }

    return( nvramDrvData[cInstance].lastResultCode == SII_DRV_NVRAM_SUCCESS );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Initialize the NVRAM/SRAM driver
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvNvramInitialize ( BYTE cInstance )
{

    // Clear instance data
    memset( &nvramDrvData[cInstance], 0, sizeof( NvramDrvInstanceData_t ));

    return( true );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns the NVRAM/SRAM driver status flag word
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

uint_t SiiDrvNvramStatus (  BYTE cInstance )
{
    return( nvramDrvData[cInstance].statusFlags );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns the result of the last driver function called
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------

int_t SiiDrvNvramGetLastResult ( BYTE cInstance )
{

    return( nvramDrvData[cInstance].lastResultCode );
}

//------------------------------------------------------------------------------
// Close the Doxygen group.
//! @}
//------------------------------------------------------------------------------
