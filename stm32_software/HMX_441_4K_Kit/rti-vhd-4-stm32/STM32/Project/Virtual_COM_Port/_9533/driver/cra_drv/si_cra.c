//***************************************************************************/
//!file     si_cra.c
//!brief    Silicon Image Device register I/O support.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
// cra: 
#include "string.h"
//#include "si_memsegsupport.h"
#include "si_platform.h"
#include "si_cra.h"
#include "si_cra_internal.h"
#include "si_cra_cfg.h"
#include "si_i2c.h"

#include "Mcu.h"
#include "uart.h"
#include "bsp_i2c.h"

#include "dvPC9539.h"
// Ò³µÄÊµÀý

static const BYTE m_acBusID[INSTANCE_9533] = {
#if((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431))
   eI2CBus9533,
   #if (INSTANCE_9533>1)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>2)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>3)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>4)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>5)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>6)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>7)
   eI2CBus9533,
   #endif
#elif(MACHINETYPE==MUHBT44)
   eI2CBus9533,
   #if (INSTANCE_9533>1)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>2)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>3)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>4)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>5)
   eI2CBus9533,
   #endif
#elif((MACHINETYPE==MUH88E_BYE)||(MACHINETYPE==MUH4K862))
	eI2CBus9533_1, // Ô¶¶Ë1
	#if (INSTANCE_9533>1)
	eI2CBus9533_1, // Ô¶¶Ë1
	#endif
	#if (INSTANCE_9533>2)
	eI2CBus9533_1, // Ô¶¶Ë1
	#endif
	#if (INSTANCE_9533>3)
	eI2CBus9533_1, // Ô¶¶Ë1
	#endif
	#if (INSTANCE_9533>4)
	eI2CBus9533_2, // Ô¶¶Ë1
	#endif
	#if (INSTANCE_9533>5)
	eI2CBus9533_2, // Ô¶¶Ë1
	#endif
	#if (INSTANCE_9533>6)
	eI2CBus9533_2, // Ô¶¶Ë1
	#endif
	#if (INSTANCE_9533>7)
	eI2CBus9533_2, // Ô¶¶Ë1
	#endif
#endif
};

static prefuint_t   l_pageInstance[INSTANCE_9533][SII_CRA_DEVICE_PAGE_COUNT] =
{
{0,0,0,0,0,  0,0,0,0,0,   0,0,0,0,0,  0,0,0,0,0,  0,0,0,0,0,},
#if (INSTANCE_9533>1)
{0},
#endif
#if (INSTANCE_9533>2)
{0},
#endif
#if (INSTANCE_9533>3)
{0},
#endif
#if (INSTANCE_9533>4)
{0},
#endif
#if (INSTANCE_9533>5)
{0},
#endif
#if (INSTANCE_9533>6)
{0},
#endif
#if (INSTANCE_9533>7)
{0},
#endif
}; // have inited
extern  pageConfig_t g_addrDescriptor[SII_CRA_MAX_DEVICE_INSTANCES][SII_CRA_DEVICE_PAGE_COUNT];
extern  SiiReg_t     g_siiRegPageBaseReassign[ INSTANCE_9533][1] ;
extern  SiiReg_t     g_siiRegPageBaseRegs[SII_CRA_DEVICE_PAGE_COUNT];

CraInstanceData_t craInstance[INSTANCE_9533] =
{
    { 0,                          // structVersion
        0,                          // instanceIndex
        RESULT_CRA_SUCCESS, //SII_SUCCESS,                // lastResultCode
        0,
    },                        // statusFlags

#if (INSTANCE_9533>1)
    { 0,                          // structVersion
        0,                          // instanceIndex
        RESULT_CRA_SUCCESS, //SII_SUCCESS,                // lastResultCode
        0,
     },                          // statusFlags
#endif
#if (INSTANCE_9533>2)
    { 0,                          // structVersion
        0,                          // instanceIndex
        RESULT_CRA_SUCCESS, //SII_SUCCESS,                // lastResultCode
        0,
    },                          // statusFlags
#endif
#if (INSTANCE_9533>3)
    { 0,                          // structVersion
        0,                          // instanceIndex
        RESULT_CRA_SUCCESS, //SII_SUCCESS,                // lastResultCode
        0,
     },                          // statusFlags
#endif
#if (INSTANCE_9533>4)
    { 0,                          // structVersion
        0,                          // instanceIndex
        RESULT_CRA_SUCCESS, //SII_SUCCESS,                // lastResultCode
        0, 
    } ,                         // statusFlags
#endif
#if (INSTANCE_9533>5)
    { 0,                          // structVersion
        0,                          // instanceIndex
        RESULT_CRA_SUCCESS, //SII_SUCCESS,                // lastResultCode
        0,
    },                          // statusFlags
#endif
#if (INSTANCE_9533>6)
    { 0,                          // structVersion
        0,                          // instanceIndex
        RESULT_CRA_SUCCESS, //SII_SUCCESS,                // lastResultCode
        0,
    },                          // statusFlags
#endif
#if (INSTANCE_9533>7)
    { 0,                          // structVersion
        0,                          // instanceIndex
        RESULT_CRA_SUCCESS, //SII_SUCCESS,                // lastResultCode
        0,
    },                          // statusFlags
#endif
};

static BYTE hcSii9533BusSelect = 0xff;
void Sii9533HalI2cBusSelect(BYTE cInstance)
{
#if ((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431))
    device_slect_9533(cInstance);
#elif(MACHINETYPE==MUHBT44)
    WORD wData1= 0x1fff;
    WORD wData2= 0x1fff;
    WORD GPIOVaule;
    assert_param(cInstance<INSTANCE_9533);
    /*
    A: BIT0
    B: BIT1
    EN:BIT4
    Enable   B     A     ON Channels
    BIT4     BIT1 BIT0
    L            0   0       Y0  X0
    L            0   1       Y1  X1
    L            1   0       Y2  X2
    L            1   1       Y3  X3
    H            X  X        None
    */
       wData1 = ((WORD) cInstance) <<12;
	wData2 =( (WORD) (cInstance-4)) <<14;
    if (hcSii9533BusSelect == cInstance)
    {
        return 0;
    }
    if (cInstance < INSTANCE_9533)
    {
        dvPC9539Read(epi9539_9679RST,outport0,(uint8_t*)&GPIOVaule);
        GPIOVaule&=~(0x0F<<12);
        GPIOVaule|=wData1;
	 if(cInstance>=4)
	   GPIOVaule|=wData2;	 
        dvPC9539Write(epi9539_9679RST,outport0,GPIOVaule);
       hcSii9533BusSelect = cInstance;
    }	
#elif((MACHINETYPE==MUH88E_BYE)||(MACHINETYPE==MUH4K862))
    device_slect_9533(cInstance);
#endif
}
//-------------------------------------------------------------------------------------------------
//! @brief      Read a byte or bytes from the specified bus and i2c device.
//! @param[in]  busIndex
//! @param[in]  deviceId    - I2C device address, eight-bit format (addr + r/w bit)
//! @param[in]  regAddr     - Eight or sixteen bit starting register address
//! @param[in]  pBuffer     - destination buffer.
//! @param[in]  count       - number of registers (bytes) to read.
//! @return     SiiResultCodes_t status of operation (0 == success).
//-------------------------------------------------------------------------------------------------
static SiiResultCodes_t CraReadBlockI2c ( BYTE cInstance, uint8_t deviceId, uint16_t regAddr, uint8_t *pBuffer, uint16_t count )
{
    SiiResultCodes_t    status;

    eRESULT  eResult = rcSUCCESS;
    Sii9533HalI2cBusSelect(cInstance);
    eResult = dvDeviceBusRead(  m_acBusID[cInstance],deviceId, regAddr, count ,pBuffer, 0);    
    status = (eResult==rcSUCCESS)?SII_SUCCESS:SII_ERR_FAIL;

    return( (SiiResultCodes_t)status );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Write a byte or bytes to the specified bus and i2c device.
//! @param[in]  busIndex
//! @param[in]  deviceId    - I2C device address, eight-bit format (addr + r/w bit)
//! @param[in]  regAddr     - Eight or sixteen bit starting register address
//! @param[in]  pBuffer     - source data buffer.
//! @param[in]  count       - number of registers (bytes) to write (maximum 256).
//! @return     SiiResultCodes_t status of operation (0 == success).
//-------------------------------------------------------------------------------------------------

static SiiResultCodes_t CraWriteBlockI2c (BYTE cInstance, uint8_t deviceId, uint16_t regAddr, uint8_t *pBuffer, uint16_t count )
{
    SiiResultCodes_t    status;

    eRESULT  eResult = rcSUCCESS;

    Sii9533HalI2cBusSelect(cInstance);
   eResult = dvDeviceBusWrite( m_acBusID[cInstance],deviceId, regAddr,count, pBuffer,0 );
   status = (eResult==rcSUCCESS)?SII_SUCCESS:SII_ERR_FAIL;

    return( (SiiResultCodes_t)status );
}

//------------------------------------------------------------------------------
// Function:    SiiCraInitialize
// Description: Initialize the CRA page instance array and perform any register
//              page base address reassignments required.
// Parameters:  none
// Returns:     None
//------------------------------------------------------------------------------
bool_t SiiCraInitialize ( BYTE cInstance )
{
    prefuint_t i, index;
    craInstance[cInstance].lastResultCode = RESULT_CRA_SUCCESS;

    for (i = 0; i < SII_CRA_DEVICE_PAGE_COUNT; i++)
    {
        l_pageInstance[cInstance][i] = 0;
    }
    // Perform any register page base address reassignments
    i = 0;
    while ( g_siiRegPageBaseReassign[cInstance][ i] != 0xFFFF )
    {
        index = g_siiRegPageBaseReassign[cInstance][ i] >> 8;
        if (( index < SII_CRA_DEVICE_PAGE_COUNT ) && ( g_siiRegPageBaseRegs[ index] != 0xFF))
        {
            // The page base registers allow reassignment of the
            // I2C device ID for almost all device register pages.
            SiiRegWrite( cInstance,  g_siiRegPageBaseRegs[ index], g_siiRegPageBaseReassign[cInstance][ index] & 0x00FF );
        }
        else
        {
            craInstance[cInstance].lastResultCode = RESULT_CRA_INVALID_PARAMETER;//SII_ERR_INVALID_PARAMETER;
            break;
        }
        i++;
    }
    return( craInstance[cInstance].lastResultCode == RESULT_CRA_SUCCESS );
}

//------------------------------------------------------------------------------
// Function:    SiiCraGetLastResult
// Description: Returns the result of the last call to a CRA driver function.
// Parameters:  none.
// Returns:     Returns the result of the last call to a CRA driver function
//------------------------------------------------------------------------------
SiiDrvCraError_t /*SiiResultCodes_t*/ SiiCraGetLastResult (  BYTE cInstance )
{
    return( craInstance[cInstance].lastResultCode );
}

//------------------------------------------------------------------------------
// Function:    SiiRegInstanceSet
// Description: Sets the instance for subsequent register accesses.  The register
//              access functions use this value as an instance index of the multi-
//              dimensional virtual address lookup table.
// Parameters:  newInstance - new value for instance axis of virtual address table.
// Returns:     None
//------------------------------------------------------------------------------
bool_t SiiRegInstanceSet ( BYTE cInstance, SiiReg_t virtualAddress, prefuint_t newInstance )
{
    prefuint_t va = virtualAddress >> 8;

    craInstance[cInstance].lastResultCode = RESULT_CRA_SUCCESS;
    if (( va < SII_CRA_DEVICE_PAGE_COUNT) && ( newInstance < SII_CRA_MAX_DEVICE_INSTANCES ))
    {
    //                          [ÄÄ¸ö9533][  µÚ¼¸Ò³]
        l_pageInstance[cInstance][ va ] = newInstance;
        return( true );
    }

    craInstance[cInstance].lastResultCode = RESULT_CRA_INVALID_PARAMETER;//SII_ERR_INVALID_PARAMETER;
    return( false );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Reads a block of data from sequential registers.
//! @param[in]  virtualAddress  - Sixteen bit virtual register address, including device page.
//! @param[in]  pBuffer     - destination buffer.
//! @param[in]  count       - number of registers (bytes) to read.
//! @return     none
//! @note       This function relies on the auto-increment model used by
//! @note       Silicon Image devices.  Because of this, if a FIFO register
//! @note       is encountered before the end of the requested count, the
//! @note       data remaining from the count is read from the FIFO, NOT
//! @note       from subsequent registers.
//-------------------------------------------------------------------------------------------------
void SiiRegReadBlock ( BYTE cInstance, SiiReg_t virtualAddr, uint8_t *pBuffer, uint16_t count )
{
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[ l_pageInstance[cInstance][ virtualAddr]][ virtualAddr];

    switch ( pPage->busType )
    {
        case DEV_I2C_0:
            craInstance[cInstance].lastResultCode = (SiiDrvCraError_t)CraReadBlockI2c(cInstance, (uint8_t)pPage->address, regOffset, pBuffer, count );
            break;
        case DEV_I2C_OFFSET:
            craInstance[cInstance].lastResultCode = (SiiDrvCraError_t)CraReadBlockI2c( cInstance, (uint8_t)pPage->address, regOffset + (uint8_t)(pPage->address >> 8), pBuffer, count );
            break;
        default:
            craInstance[cInstance].lastResultCode = RESULT_CRA_INVALID_PARAMETER;
            break;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Read a one byte register.
//! @param[in]  virtualAddress  - Sixteen bit virtual register address, including device page.
//! @return     eight bit register data.
//-------------------------------------------------------------------------------------------------
uint8_t SiiRegRead (BYTE cInstance, SiiReg_t virtualAddr )
{
    uint8_t             value = 0;
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;  // page Number
    pPage = &g_addrDescriptor[ l_pageInstance[cInstance][ virtualAddr]][ virtualAddr];

    switch ( pPage->busType )
    {
        case DEV_I2C_0:
            craInstance[cInstance].lastResultCode = (SiiDrvCraError_t)CraReadBlockI2c(cInstance, (uint8_t)pPage->address, regOffset, &value, 1 );
            break;
        case DEV_I2C_OFFSET:
            craInstance[cInstance].lastResultCode = (SiiDrvCraError_t)CraReadBlockI2c( cInstance, (uint8_t)pPage->address, regOffset + (uint8_t)(pPage->address >> 8), &value, 1 );
            break;
        default:
            craInstance[cInstance].lastResultCode = RESULT_CRA_INVALID_PARAMETER;
            break;
    }

    return( value );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Writes a block of data to sequential registers.
//! @param[in]  virtualAddress  - Sixteen bit virtual register address, including device page.
//! @param[in]  pBuffer     - source data buffer.
//! @param[in]  count       - number of registers (bytes) to write.
//! @return     none
//! @note       This function relies on the auto-increment model used by
//! @note       Silicon Image devices.  Because of this, if a FIFO register
//! @note       is encountered before the end of the requested count, the
//! @note       data remaining from the count is written to the FIFO, NOT
//! @note       to subsequent registers.
//-------------------------------------------------------------------------------------------------
void SiiRegWriteBlock ( BYTE cInstance, SiiReg_t virtualAddr, uint8_t *pBuffer, uint16_t count )
{
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[l_pageInstance[cInstance][virtualAddr]][virtualAddr];

    switch ( pPage->busType )
    {
        case DEV_I2C_0:
            craInstance[cInstance].lastResultCode = (SiiDrvCraError_t)CraWriteBlockI2c(cInstance,(uint8_t)pPage->address, regOffset, pBuffer, count );
            break;
        case DEV_I2C_OFFSET:
            craInstance[cInstance].lastResultCode = (SiiDrvCraError_t)CraWriteBlockI2c(cInstance,(uint8_t)pPage->address, regOffset + (uint8_t)(pPage->address >> 8), pBuffer, count );
            break;
        default:
            craInstance[cInstance].lastResultCode = RESULT_CRA_INVALID_PARAMETER;
            break;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Write a one byte register.
//! @param[in]  virtualAddress  - Sixteen bit virtual register address, including device page.
//! @param[in]  value           - eight bit data to write to register.
//! @return     None
//-------------------------------------------------------------------------------------------------
void SiiRegWrite(BYTE cInstance, SiiReg_t virtualAddr, uint8_t value )
{
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;
    BYTE                cCRAIndex = 0;

    virtualAddr >>= 8; //µÚ¼¸Ò³
    cCRAIndex = l_pageInstance[cInstance][virtualAddr];
    //pPage = &g_addrDescriptor[l_pageInstance[cInstance][virtualAddr]][virtualAddr];
    pPage = &g_addrDescriptor[cCRAIndex][virtualAddr];
    switch ( pPage->busType )
    {
        case DEV_I2C_0:
        case DEV_I2C_1:
        case DEV_I2C_2:
        case DEV_I2C_3:
            craInstance[cInstance].lastResultCode = (SiiDrvCraError_t)CraWriteBlockI2c( cInstance,(uint8_t)pPage->address, regOffset, &value, 1 );
            break;
        case DEV_I2C_OFFSET:
        case DEV_I2C_1_OFFSET:
        case DEV_I2C_2_OFFSET:
        case DEV_I2C_3_OFFSET:
            craInstance[cInstance].lastResultCode = (SiiDrvCraError_t)CraWriteBlockI2c(cInstance, (uint8_t)pPage->address, regOffset + (uint8_t)(pPage->address >> 8), &value, 1 );
            break;
        default:
            craInstance[cInstance].lastResultCode = RESULT_CRA_INVALID_PARAMETER;
            break;
    }
}

//------------------------------------------------------------------------------
// Function:    SiiRegModify
// Description: Reads the register, performs an AND function on the data using
//              the mask parameter, and an OR function on the data using the
//              value ANDed with the mask. The result is then written to the
//              device register specified in the regAddr parameter.
// Parameters:  regAddr - Sixteen bit register address, including device page.
//              mask    - Eight bit mask
//              value   - Eight bit data to be written, combined with mask.
// Returns:     None
//------------------------------------------------------------------------------
void SiiRegModify ( BYTE cInstance, SiiReg_t virtualAddr, uint8_t mask, uint8_t value)
{
    uint8_t aByte;

    aByte = SiiRegRead( cInstance, virtualAddr );
    aByte &= (~mask);                       // first clear all bits in mask
    aByte |= (mask & value);                // then set bits from value
    SiiRegWrite( cInstance,  virtualAddr, aByte );
}

//------------------------------------------------------------------------------
// Function:    SiiRegBitsSet
// Description: Reads the register, sets the passed bits, and writes the
//              result back to the register.  All other bits are left untouched
// Parameters:  regAddr - Sixteen bit register address, including device page.
//              bits   - bit data to be written
// Returns:     None
//------------------------------------------------------------------------------
void SiiRegBitsSet (BYTE cInstance, SiiReg_t virtualAddr, uint8_t bitMask, bool_t setBits )
{
    uint8_t aByte;

    aByte = SiiRegRead(cInstance, virtualAddr );
    aByte = (setBits) ? (aByte | bitMask) : (aByte & ~bitMask);
    SiiRegWrite( cInstance,  virtualAddr, aByte );
}

//------------------------------------------------------------------------------
// Function:    SiiRegBitsSetNew
// Description: Reads the register, sets or clears the specified bits, and
//              writes the result back to the register ONLY if it would change
//              the current register contents.
// Parameters:  regAddr - Sixteen bit register address, including device page.
//              bits   - bit data to be written
//              setBits- true == set, false == clear
// Returns:     None
//------------------------------------------------------------------------------
void SiiRegBitsSetNew ( BYTE cInstance, SiiReg_t virtualAddr, uint8_t bitMask, bool_t setBits )
{
    uint8_t newByte, oldByte;

    oldByte = SiiRegRead( cInstance,virtualAddr );
    newByte = (setBits) ? (oldByte | bitMask) : (oldByte & ~bitMask);
    if ( oldByte != newByte )
    {
        SiiRegWrite( cInstance,  virtualAddr, newByte );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Reads a block of data from EDID record over DDC link.
//! @param[in]  segmentAddress  - Sixteen bit virtual EDID segment address, including device page.
//! @param[in]  virtualAddress  - Sixteen bit virtual EDID address, including device page.
//! @param[in]  pBuffer         - source data buffer.
//! @param[in]  count           - number of registers (bytes) to read.
//! @return     none
//-------------------------------------------------------------------------------------------------
void SiiRegEdidReadBlock ( BYTE cInstance, SiiReg_t segmentAddr, SiiReg_t virtualAddr, uint8_t *pBuffer, uint16_t count )
{
    //uint8_t             regOffset = (uint8_t)virtualAddr;
    //pageConfig_t        *pPage;
   /* SiiI2cMsg_t         msgs[3];
    int                 msgIndex = 0;

    // Split into register address write and register data read messages.
    if ((segmentAddr & 0xFF) != 0)  // Default segment #0 index should not be sent explicitly
    {
        regOffset = (uint8_t)segmentAddr;
        segmentAddr >>= 8;
        pPage = &g_addrDescriptor[l_pageInstance[cInstance][segmentAddr]][segmentAddr];

        // Write non-zero EDID segment address
        msgs[0].addr        = pPage->address;
        msgs[0].cmdFlags    = SII_MI2C_WR;
        msgs[0].len         = 1;
        msgs[0].pBuf        = &regOffset;
        msgIndex = 1;
    }

    // Read the actual EDID data
    regOffset = (uint8_t)virtualAddr;
    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[l_pageInstance[cInstance][virtualAddr]][virtualAddr];
    msgs[msgIndex].addr        = pPage->address;
    msgs[msgIndex].cmdFlags    = SII_MI2C_WR;
    msgs[msgIndex].len         = 1;
    msgs[msgIndex].pBuf        = &regOffset;
    msgIndex++;
    msgs[msgIndex].addr        = pPage->address;
    msgs[msgIndex].cmdFlags    = SII_MI2C_RD;
    msgs[msgIndex].len         = count;
    msgs[msgIndex].pBuf        = pBuffer;

    // Send the messages
    craInstance[cInstance].lastResultCode = (SiiDrvCraError_t)SiiPlatformI2cTransfer( pPage->busType, &msgs[0], sizeof( msgs ) / sizeof( SiiI2cMsg_t ));*/
}

