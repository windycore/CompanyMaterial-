//***************************************************************************
//!file     si_eeprom.c
//!brief    Silicon Image Starter Kit EEPROM interface
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_common.h"
#include "si_platform.h"
#include "si_eeprom.h"
#include "si_i2c.h"
#include "dvNVRAM.h"
//------------------------------------------------------------------------------
// Function:    SiiPlatformEepromWriteBlock
// Description:	Write a block of data to the EEPROM
// Parameters:  addr
// Returns:     true if successful, false if an error occurred
//
// Note:		The Microchip 24LC128 uses an internal 64-byte
//              page buffer, and block writes must be aligned.
// Note:		A write is not complete until the EE_PROM begins ACKing
//				again.
// Note:		PollForAck cannot be done on Stellaris I2C controller
// Note:		The SiiPlatformI2cWriteBlock() function updates lastResultCode and i2CError
//------------------------------------------------------------------------------
bool_t SiiPlatformEepromWriteBlock ( WORD addr, BYTE *pWrData, WORD nBytes )
{
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesWrite(addr,nBytes,pWrData);
assert_param(eResult == rcSUCCESS);
return (eResult== rcSUCCESS) ;

#if 0
    bool_t        success = true;
    uint16_t    i, packetLen;

    if ( addr & (32-1))
    {
        packetLen = addr & (32-1);
        if ( packetLen > nBytes )
            packetLen = nBytes;

        // Write individual bytes until we get to a 32 byte boundary
        // or run out of bytes.

        for ( i = 0; (( i < packetLen ) && success ); i++ )
        {
            success = SiiPlatformI2cWriteBlock16( 0, EEPROM_I2C_ADDRESS, addr, pWrData, 1 );
            addr++;
            pWrData++;
        }
        nBytes -= packetLen;
    }

    // Write the remaining aligned packets.

    for ( i = 0; (( i < nBytes ) && success ); i++ )
    {
        packetLen = 32;
        if ( packetLen > nBytes )
            packetLen = nBytes;
        success = SiiPlatformI2cWriteBlock16( 0, EEPROM_I2C_ADDRESS, addr, pWrData, packetLen );
        addr    += packetLen;
        pWrData += packetLen;
        nBytes  -= packetLen;
    }

    return( success );
#endif
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformEepromWriteByte
// Description:	Write a single byte of data to the on-board EEPROM
// Parameters:  addr
//				value
// Returns:
// Note:		The SiiPlatformEepromWriteBlock() function updates lastResultCode and i2CError
//------------------------------------------------------------------------------

bool_t SiiPlatformEepromWriteByte ( WORD addr, BYTE value )
{
	//SiiPlatformEepromWriteBlock( addr, &value, 1 );
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesWrite(addr,1,&value);
assert_param(eResult == rcSUCCESS);
return (eResult == rcSUCCESS);

}

//------------------------------------------------------------------------------
// Function:    SiiPlatformEepromReadBlock
// Description:	Read a block of data from the on-board EEPROM
// Parameters:  addr
//				pRdData
//				nBytes
// Returns:
//------------------------------------------------------------------------------

bool_t SiiPlatformEepromReadBlock ( WORD addr, BYTE *pRdData, WORD nBytes )
{
	//return( SiiPlatformI2cReadBlock16( 0, EEPROM_I2C_ADDRESS, addr, pRdData, nBytes ));
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesRead(addr,nBytes,pRdData);
ASSERT(eResult == rcSUCCESS);
return (eResult == rcSUCCESS);
	
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformEepromReadByte
// Description:	Read a byte of data from the on-board EEPROM
// Parameters:  addr
// Returns:     Data byte read from EEPROM
//------------------------------------------------------------------------------

BYTE SiiPlatformEepromReadByte (WORD addr )
{
/*
    uint8_t value;

    SiiPlatformI2cReadBlock16( 0, EEPROM_I2C_ADDRESS, addr, &value, 1 );
    return( value );
    */
BYTE cReadData;
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesRead(addr,1,&cReadData);
ASSERT(eResult == rcSUCCESS);
return cReadData;
}
