//***************************************************************************
//!file     sk_app_edid.c
//!brief    Wraps board and device functions for the EDID component
//          and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_eeprom.h"
#include "sk_application.h"
#include "sk_app_edid.h"

#include "si_drv_tpi_ddc_access.h"
#include "si_drv_nvram_sram.h"
extern uint8_t g_Init_EdidFlashEdidTable[];
//------------------------------------------------------------------------------
//  Function:       SkAppEdidRead
//  Description:    Read the selected EDID source into the passed array.
//------------------------------------------------------------------------------
//  Return:
//              TRUE: sucess
//              FALSE: fail
bool_t SkAppEdidRead (  BYTE cInstance,uint8_t source, uint8_t *pDest )
{
    ddcComErr_t ddcResult;
    bool_t      success = true;
    switch (source)
    {
    case EDID_RAM_0:
    case EDID_RAM_1:
    case EDID_RAM_2:
    case EDID_RAM_3:
    case EDID_RAM_4:
    case EDID_RAM_5:
        success = SiiDrvNvramSramRead(  cInstance, (SiiSramType_t)(SRAM_P0 + source), pDest, 0, EDID_TABLE_LEN );
        break;
        /*
        case EDID_RAM_VGA:
        success = SiiDrvNvramSramRead( SRAM_VGA, pDest, 0, EDID_VGA_TABLE_LEN );
        break;*/
    case EDID_RAM_BOOT:
        success = SiiDrvNvramSramRead(  cInstance, SRAM_BOOT, pDest, 0, EDID_DEVBOOT_LEN );
        break;
    case EDID_NVRAM:
        success = SiiDrvNvramNonVolatileRead( cInstance,  NVRAM_HDMI_EDID, pDest, 0, EDID_TABLE_LEN );
        break;
    case EDID_NVRAM_BOOT:
        success = SiiDrvNvramNonVolatileRead(  cInstance, NVRAM_BOOTDATA, pDest, 0, EDID_DEVBOOT_LEN );
        break;
        /*
        case EDID_NVRAM_VGA:
        success = SiiDrvNvramNonVolatileRead( NVRAM_VGA_EDID, pDest, 0, EDID_VGA_TABLE_LEN );
        break;
        */
    case EDID_EEPROM:
        //success = SiiPlatformEepromReadBlock( EE_EDID_OFFSET, pDest, EDID_TABLE_LEN );
        ASSERT_ALWAYS();
        break;
    case EDID_EEPROM_BOOT:
        //success = SiiPlatformEepromReadBlock( EE_DEVBOOT_OFFSET, pDest, EDID_DEVBOOT_LEN );
        ASSERT_ALWAYS();
        break;
        /*
        case EDID_EEPROM_VGA:
        success = SiiPlatformEepromReadBlock( EE_EDIDVGA_OFFSET, pDest, EDID_VGA_TABLE_LEN );
        break;*/
    case EDID_TV:
        ddcResult = SiiDrvTpiDdcReadBlock(  cInstance, 0, 0, pDest, EDID_TABLE_LEN );
        success = (ddcResult == SI_TX_DDC_ERROR_CODE_NO_ERROR);
        break;
//        case EDID_VGA:
//            success = SkI2cReadVgaEdidBlock( 0, pDest, EDID_TABLE_LEN );
//            break;
    case EDID0_EEPROM:
        //success = SiiPlatformEepromReadBlock( EE_EDID0_OFFSET, pDest, EDID_TABLE_LEN );
        ASSERT_ALWAYS();
        break;
    case EDID1_EEPROM:
        //success = SiiPlatformEepromReadBlock( EE_EDID1_OFFSET, pDest, EDID_TABLE_LEN );
        ASSERT_ALWAYS();
        break;
    case EDID2_EEPROM:
        //success = SiiPlatformEepromReadBlock( EE_EDID2_OFFSET, pDest, EDID_TABLE_LEN );
        ASSERT_ALWAYS();
        break;
    case EDID3_EEPROM:
        //success = SiiPlatformEepromReadBlock( EE_EDID3_OFFSET, pDest, EDID_TABLE_LEN );
        ASSERT_ALWAYS();
        break;
    default:
        DEBUG_PRINT( MSG_DBG, "\nEDID Read source not supported: %d\n", (uint16_t)source );
        break;
    }
    return( success );
}

//------------------------------------------------------------------------------
//  Function:       SkAppEdidWrite
//  Description:    Write the selected EDID target from the passed array.
//------------------------------------------------------------------------------
//
bool_t SkAppEdidWrite (BYTE cInstance, uint8_t target, uint8_t *pSource )
{
    ddcComErr_t ddcResult;
    bool_t      success = true;
    switch (target)
    {
    case EDID_RAM_0:
    case EDID_RAM_1:
    case EDID_RAM_2:
    case EDID_RAM_3:
    case EDID_RAM_4:
    case EDID_RAM_5:
        success = SiiDrvNvramSramWrite(cInstance, (SiiSramType_t)(SRAM_P0 + target), pSource, 0, EDID_TABLE_LEN );
        break;
        /*
        case EDID_RAM_VGA:
        success = SiiDrvNvramSramWrite( SRAM_VGA, pSource, 0, EDID_VGA_TABLE_LEN );
        break;*/
    case EDID_RAM_BOOT:
        success = SiiDrvNvramSramWrite(cInstance, SRAM_BOOT, pSource, 0, EDID_DEVBOOT_LEN );
        break;
    case EDID_NVRAM:
        success = SiiDrvNvramNonVolatileWrite(cInstance, NVRAM_HDMI_EDID, pSource, 0, EDID_TABLE_LEN );
        break;
    case EDID_NVRAM_BOOT:
        success = SiiDrvNvramNonVolatileWrite( cInstance,NVRAM_BOOTDATA, pSource, 0, EDID_DEVBOOT_LEN );
        break;
        /*
        case EDID_NVRAM_VGA:
        success = SiiDrvNvramNonVolatileWrite( NVRAM_VGA_EDID, pSource, 0, EDID_VGA_TABLE_LEN );
        break;*/
    case EDID_EEPROM:
    case EDID0_EEPROM:
        //success = SiiPlatformEepromWriteBlock( EE_EDID_OFFSET, pSource, EDID_TABLE_LEN );
        //SiiPlatformEepromWriteByte( EE_EDID_VALID, CONFIG_VALID );
        ASSERT_ALWAYS();
        break;
    case EDID_EEPROM_BOOT:
        //success = SiiPlatformEepromWriteBlock( EE_DEVBOOT_OFFSET, pSource, EDID_DEVBOOT_LEN );
        //SiiPlatformEepromWriteByte( EE_DEVBOOTDATA_VALID, CONFIG_VALID );
        ASSERT_ALWAYS();
        break;
        /*
        case EDID_EEPROM_VGA:
        success = SiiPlatformEepromWriteBlock( EE_EDIDVGA_OFFSET, pSource, EDID_VGA_TABLE_LEN );
        SiiPlatformEepromWriteByte( EE_EDIDVGA_VALID, CONFIG_VALID );
        break;*/
    case EDID_TV:
        ddcResult = SiiDrvTpiDdcWriteBlock(cInstance, 0, pSource, EDID_TABLE_LEN );
        success = (ddcResult == SI_TX_DDC_ERROR_CODE_NO_ERROR);
        break;
//        case EDID_VGA:
//            success = SkI2cWriteVgaEdidBlock( 0, pSource, EDID_TABLE_LEN);
//            break;
    case EDID1_EEPROM:
        //success = SiiPlatformEepromWriteBlock( EE_EDID1_OFFSET, pSource, EDID_TABLE_LEN );
        //SiiPlatformEepromWriteByte( EE_EDID1_VALID, CONFIG_VALID );
        ASSERT_ALWAYS();
        break;
    case EDID2_EEPROM:
        //success = SiiPlatformEepromWriteBlock( EE_EDID2_OFFSET, pSource, EDID_TABLE_LEN );
        //SiiPlatformEepromWriteByte( EE_EDID2_VALID, CONFIG_VALID );
        ASSERT_ALWAYS();
        break;
    case EDID3_EEPROM:
        //success = SiiPlatformEepromWriteBlock( EE_EDID3_OFFSET, pSource, EDID_TABLE_LEN );
        //SiiPlatformEepromWriteByte( EE_EDID3_VALID, CONFIG_VALID );
        ASSERT_ALWAYS();
        break;
    default:
        DEBUG_PRINT( MSG_DBG,  "\n-- EDID Write target not supported: %d --\n", (uint16_t)target );
        break;
    }
    return( success );
}

//------------------------------------------------------------------------------
// Function:    SkAppCmpData
// Description: Display the passed buffer in ASCII-HEX block format
//------------------------------------------------------------------------------

void SkAppCmpData ( uint8_t msgLevel, uint8_t *pData, uint8_t type)
{
    uint16_t    i = 0;
    uint8_t     stop = 0;
    uint16_t    length = 0;
    const uint8_t *pRom = 0;
    switch (type)
    {
    default:
    case EDID_NVRAM:
        length = EDID_TABLE_LEN;
        pRom = g_Init_EdidFlashEdidTable;//g_edidFlashEdidTable;
        break;
    case EDID_NVRAM_BOOT:
        length = sizeof(SiiDeviceBootData_t);
        pRom = (uint8_t *) &gEdidFlashDevBootData;
        break;
        /*
                case EDID_NVRAM_VGA:
                    length = EDID_VGA_TABLE_LEN;
                    pRom = gEdidFlashEdidVgaTable;
                    break;
        */
    }
    for ( i = 0; i < length; i++ )
    {
        if (pData[i] != pRom[i])
        {
            DEBUG_PRINT2( msgLevel, "\n%03d: %02X %02X", i, (uint16_t)pData[ i],  pRom[i] );
            if (stop == 5)
            {
                break;
            }
            stop++;
        }
    }
    // stop ? DEBUG_PRINT( msgLevel, "\n ---- is NOT match\n") : DEBUG_PRINT( msgLevel, " ---- is Ok" );
}


extern uint8_t g_Init_EdidFlashEdidTable[];
void  SkAppDefaultEdidDataProductNameSet(PBYTE pcData)
{
    WORD sys_ID = 0x07ff;
    BYTE crc = 0;
    BYTE i;
// for (i = 0; i < 4; i++)  EDID_name[i] = i2c_EE_read_one_byte(sys_EDID_add + i);
    //对EDID来说，名字改了要从新计算校验位
    if (pcData[2] >= 'A')
    {
        sys_ID &= 0xffe0; //清掉低5位
        sys_ID |= pcData[2] - 0x40;
    }
    if (pcData[1] >= 'A')
    {
        sys_ID &= 0xfc1f; //清掉中5位
        sys_ID |= ((pcData[1] - 0x40) << 5);
    }
    if (pcData[0] >= 'A')
    {
        sys_ID &= 0x03ff; //清掉高5位
        sys_ID |= ((pcData[0] - 0x40) << 10);
    }
    g_Init_EdidFlashEdidTable[8] = sys_ID >> 8;
    g_Init_EdidFlashEdidTable[9] = sys_ID;
    /*
    for(i=0;i<13;i++)//显示器名称按EDID标准最长是13字节（共18字节描述，前5字节为类型描述）
    {
      if(sys_name[i] != 0)  init_edidFlashEdidTable[95+i] = sys_name[i];
      else  break ;
    }
    for(j=i;j<13;j++)  init_edidFlashEdidTable[95+j] = 0;
    */
    for (i = 0; i < 127; i++)
    {
        crc += g_Init_EdidFlashEdidTable[i];
    }
    g_Init_EdidFlashEdidTable[127] = ~(crc - 1);
}


