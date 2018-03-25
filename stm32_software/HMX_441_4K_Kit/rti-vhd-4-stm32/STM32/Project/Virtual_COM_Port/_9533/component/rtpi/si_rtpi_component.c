//***************************************************************************/
//!file     si_rtpi_component.c
//!brief    Silicon Image RTPI Component.
//          RTPI component presents the API for interfacing Silicon Image chip
//          for repeater operations.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************

#include "string.h"
#include "si_rtpi_internal.h"
#include "si_drv_switch.h"
#include "si_drv_audio.h"
#include "si_eeprom.h"

#include "Mcu.h"
#include "uart.h"

//------------------------------------------------------------------------------
//  RTPI Component Instance Data  RTPI:Repeater Programming Interface
//------------------------------------------------------------------------------

SiiRtpiInstanceData_t   rtpiInstance[INSTANCE_9533][SII_NUM_RTPI];
SiiRtpiInstanceData_t   *pRtpi[INSTANCE_9533] =
{
    &rtpiInstance[0][0],
#if(INSTANCE_9533>1)
    &rtpiInstance[1][0],
#endif
#if(INSTANCE_9533>2)
    &rtpiInstance[2][0],
#endif
#if(INSTANCE_9533>3)
    &rtpiInstance[3][0],
#endif
#if(INSTANCE_9533>4)
    &rtpiInstance[4][0],
#endif
#if(INSTANCE_9533>5)
    &rtpiInstance[5][0],
#endif
#if(INSTANCE_9533>6)
    &rtpiInstance[6][0],
#endif
#if(INSTANCE_9533>7)
    &rtpiInstance[7][0],
#endif

};

SiiRtpiBusSetup_t       siiTpiInputBusSetupVal[DevNum];
SiiRtpiSystemControl_t  siiTpiSystemControlVal[DevNum];

// RTPI Registers.
uint8_t rtpiRegs[DevNum][RTPI_MAX_REGISTERS];   // RTPI Registers.
uint8_t rtpiWriteBuf[DevNum][RTPI_CMD_BUFFER_SIZE][2];
uint8_t rtpiWrPtr[DevNum] = {0};
uint8_t rtpiRdPtr[DevNum] = {0};

//------------------------------------------------------------------------------
//  External Data
//------------------------------------------------------------------------------

extern uint8_t rtpiInitRegs[RP_L__TOTAL_REGS];      // from platform

//------------------------------------------------------------------------------
//! @brief      Set the component global data pointer to the requested instance.
//! @param[in]  instanceIndex
//! @return     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------
bool_t SiiRtpiInstanceSet (  BYTE cInstance,uint_t instanceIndex )
{

    if ( instanceIndex < SII_NUM_RTPI )
    {
        (pRtpi[cInstance]) = &rtpiInstance[cInstance][instanceIndex];
        (pRtpi[cInstance])->rtpiInstance = instanceIndex;
        return( true );
    }
    return( false );
}

//------------------------------------------------------------------------------
// Description: Return the current instance index.
// Parameters:  none
// Returns:     Always 0.
//------------------------------------------------------------------------------

int_t SiiRtpiInstanceGet (  BYTE cInstance )
{
    return( (int_t)(pRtpi[cInstance])->rtpiInstance );
}

//------------------------------------------------------------------------------
// Read/Write functions.
//------------------------------------------------------------------------------

void SiiRpForceWriteByte( BYTE cInstance,uint8_t address, uint8_t d)
{
    rtpiRegs[cInstance][address] =  d ;
}
//-------------------------------------------------------------------------------------------------
//! @brief      I2C slave port write. External master is writing to our registers.
//!             Callback from platform I2C slave port interrupt handler.
//!             Can also be used internally to update register.
//!
//! @param[in]  regAddr     8-bit register address
//! @param[in]  regData     8-bit data from external I2C master
//-------------------------------------------------------------------------------------------------
void SiiPlatformI2cCbSlaveWriteByte( BYTE cInstance, /*int busIndex,*/ uint8_t regAddr, uint8_t regData )
{
    if ( regAddr < RP_A__RUNTIME_REGS ) return;
    if ( regAddr >= RP_L__TOTAL_REGS ) return;

    // Place register write info into our register write ring buffer
    rtpiWriteBuf[cInstance][ rtpiWrPtr[cInstance] ][RTPI_CMD_DATA] = regData;
    rtpiWriteBuf[cInstance][ rtpiWrPtr[cInstance] ][RTPI_CMD_ADDR] = regAddr;
    rtpiWrPtr[cInstance]++;

    if ( rtpiWrPtr[cInstance] >= RTPI_CMD_BUFFER_SIZE)
    {
        rtpiWrPtr[cInstance] = 0;
    }

    // If it is an execute command, clear the ready bit right away.
    if(regAddr == RP_A__CMD_CMD)
    {
        SiiRpForceWriteByte( cInstance, RP_A__CMD_STATUS, 0);  // Clear ready bit.
    }

    // If it is an interrupt register, clear the interrupt right away.
    else if(regAddr == RP_A__INTERRUPT)
    {
    	SiiRtpiClearInt( cInstance, regData);
    }
    else if(regAddr == RP_A__INTERRUPT2)
    {
        SiiRtpiClearInt( cInstance, regData<<8);
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      I2C slave port read. External master is reading from our registers
//!             Callback from platform I2C slave port interrupt handler.
//!
//! @param[in]  regAddr     8-bit register address
//! @return                 8-bit data for external I2C master
//-------------------------------------------------------------------------------------------------
uint8_t SiiPlatformI2cCbSlaveReadByte( BYTE cInstance,  uint8_t regAddr )
{
    uint8_t regValue = 0;

    switch ( regAddr )
    {
        case 0x24:
            regValue = SiiRtpiCbUpdateSource(  cInstance, 0, 0 );   // Read source value
            break;
        case 0x2A:
            regValue = SiiDrvSwitchStateGet(  cInstance, SiiSwitch_INPUT_PORTS_CONNECT_STATE );
            break;

        default:

            if ( regAddr < RP_L__TOTAL_REGS )
            {
                regValue = rtpiRegs[cInstance][ regAddr ];
            }
            break;
    }
    DEBUG_PRINT( MSG_DBG, "RTPI Read [%02X]: %02X\n", regAddr, regValue );

    return( regValue );
}

void SiiRpWriteBlock( BYTE cInstance, uint8_t address, uint8_t *p_data, uint8_t length)
{
    int i;
    for( i=address; i<address+length; i++ )
    {
    	SiiRpForceWriteByte( cInstance,  i, *(p_data++) );
    }
}

void SiiRpReadBlock( BYTE cInstance,uint8_t address, uint8_t *p_data, uint8_t length)
{
    int i;
    for( i=address; i<address+length; i++ )
    {
        *(p_data++) = SiiPlatformI2cCbSlaveReadByte( cInstance,  i );
    }
}

void SiiRtpiClearInt(  BYTE cInstance,uint16_t val )
{
    uint8_t rtpiInt;
    rtpiInt = SiiPlatformI2cCbSlaveReadByte( cInstance,  RP_A__INTERRUPT );
    rtpiInt &= ~(uint8_t)val;
    SiiRpForceWriteByte(  cInstance, RP_A__INTERRUPT, rtpiInt);

    rtpiInt = SiiPlatformI2cCbSlaveReadByte( cInstance,  RP_A__INTERRUPT2 );
    rtpiInt &= ~(uint8_t)(val>>8);
    SiiRpForceWriteByte( cInstance,  RP_A__INTERRUPT2, rtpiInt);
}

void SiiRtpiSetInt(  BYTE cInstance, uint16_t val )
{
    uint8_t rtpiInt;
    rtpiInt = SiiPlatformI2cCbSlaveReadByte( cInstance,  RP_A__INTERRUPT );
    rtpiInt |= (uint8_t)val;
    SiiRpForceWriteByte(  cInstance, RP_A__INTERRUPT, rtpiInt);

    rtpiInt = SiiPlatformI2cCbSlaveReadByte( cInstance,  RP_A__INTERRUPT2 );
    rtpiInt |= (uint8_t)(val>>8);
    SiiRpForceWriteByte(  cInstance, RP_A__INTERRUPT2, rtpiInt);

}

//------------------------------------------------------------------------------
// Function:    SiiRtpiInitialize
// Description: Attempts to initialize the RTPI
// Returns:     TRUE if no problem
//------------------------------------------------------------------------------
bool_t SiiRtpiSetInterrupt( BYTE cInstance, uint8_t intRegAddress, uint8_t intRegValue )
{
    bool_t  success = true;
    uint8_t rtpiInt;

    switch ( intRegAddress )
    {
        case RP_A__CEC_STAT:
        case RP_A__INTERRUPT:
        case RP_A__INTERRUPT2:
            rtpiInt = SiiPlatformI2cCbSlaveReadByte( cInstance,  intRegAddress );
            rtpiInt |= (uint8_t)intRegValue;
            SiiRpForceWriteByte( cInstance,  intRegAddress, rtpiInt );
            break;
        default:
            success = false;
            break;
    }

    return( success );
}

void SiiRtpiPort( BYTE cInstance, uint8_t port, uint8_t flag)
{
	uint8_t inputSelector = SiiPlatformI2cCbSlaveReadByte( cInstance,  RP_A__BOARD_OPTIONS2);
	inputSelector &= RP_M__BOARD_OPTIONS2__USE_ONBOARD_INPUT_SELECTOR;

	if (inputSelector == 0 && flag == 0)
	{
		SiiRpForceWriteByte( cInstance,  RP_A__INPUT_NMB, port);
	}
	else if (inputSelector == 1 && flag == 1)
	{
		SiiRpForceWriteByte( cInstance,  RP_A__INPUT_NMB, port);
	}
}

void SiiRtpiPortConnect( BYTE cInstance,uint8_t pwr5vStatus)
{
	uint8_t mask = 0;
	uint8_t regAValue = SiiDrvSwitchStateGet( cInstance, SiiSwitch_SELECTED_PORT);

	uint8_t val = 0x00;

	//DEBUG_PRINT( MSG_DBG, " %x %x  \n",  pwr5vStatus, regAValue);

	mask = 1 << regAValue;

	if (pwr5vStatus & mask)
	{
	    val = (regAValue << 4);
	}
	SiiRpForceWriteByte(  cInstance, RP_A__PIPE_STATUS, val);

}
void SiiRtpiOnAudioStatusChange( BYTE cInstance, uint8_t formatVal)
{
    uint8_t aIfData[5];
    uint8_t *pIfData = &aIfData[0];

	SiiRtpiSetInt( cInstance, RP_M__INTERRUPT__AUDIO_STATUS_CHANGED);
	SiiRpForceWriteByte( cInstance, RP_A__RX_AUDIO_FORMAT, formatVal);
	SiiDrvRxAudioChstGet( cInstance, pIfData);
	SiiRpWriteBlock( cInstance, RP_A__RX_I2S_CHST1, pIfData, 5);
}

void SiiRtpiOnAudioInfoFrameChange(  BYTE cInstance )
{
    uint8_t aIfData[31];
    uint8_t *pIfData = &aIfData[0];
    uint8_t len;

    SiiRtpiSetInt( cInstance, RP_M__INTERRUPT__AUDIO_FORMAT);
    SiiDrvSwitchInfoFrameGet( cInstance, true, IF_AIF, pIfData, &len);

    SiiRpForceWriteByte( cInstance, RP_A__RX_AUDIO_INFO_FRAME1, pIfData[4]);
    SiiRpForceWriteByte( cInstance, RP_A__RX_AUDIO_INFO_FRAME4, pIfData[7]);
    SiiRpForceWriteByte( cInstance, RP_A__RX_AUDIO_INFO_FRAME5, pIfData[8]);
}

void SiiRtpiOnAviInfoFrameChange(  BYTE cInstance )
{
    uint8_t aIfData[31];
    uint8_t *pIfData = &aIfData[0];
    uint8_t len;

	SiiRtpiSetInt( cInstance, RP_M__INTERRUPT__AVI_RECEIVED);
    SiiDrvSwitchInfoFrameGet( cInstance, true, IF_AVI, pIfData, &len);
	SiiRpWriteBlock( cInstance, RP_A__RX_AVI1, pIfData + 4, 5);
}

void SiiRtpiOnPwrStateChange( BYTE cInstance, bool_t qOn)
{
	SiiRpForceWriteByte( cInstance, RP_A__RX_OUTPUT, RP_M__RX_OUTPUT_PWR & qOn);
}


void SiiRtpiLoadEepromInit(  BYTE cInstance )
{
    //uint8_t buf[5];
    int i;
    //SiiPlatformEepromReadBlock(0, buf, 5);

    for( i=5; i<0x0F; i++ )
    {
        SiiRpForceWriteByte( cInstance,  i, SiiPlatformEepromReadByte( i ) );
    }
}


//------------------------------------------------------------------------------
// Function:    SiiRtpiInitialize
// Description: Attempts to initialize the RTPI
// Returns:     TRUE if no problem
//------------------------------------------------------------------------------
bool_t SiiRtpiInitialize ( BYTE cInstance, uint8_t instance )
{
    int i;
	//char *strVersion = "x.xx.xx";

    int decimal = 0;

    // Set instance value.
    SiiRtpiInstanceSet( cInstance, instance);

    for (i=0; i < RTPI_CMD_BUFFER_SIZE; i++)
    {
        rtpiWriteBuf[cInstance][i][RTPI_CMD_ADDR] = 0;
        rtpiWriteBuf[cInstance][i][RTPI_CMD_DATA] = 0;
    }

    //DEBUG_PRINT(MSG_ALWAYS,"SiiRtpiInitialize %x %x\n", (pRtpi[cInstance]), (pRtpi[cInstance])->rtpi_regs[10] );

    // Clear instance data
    memset( (pRtpi[cInstance]), 0, sizeof( SiiRtpiInstanceData_t ));

    // Set configuration registers to their default state
    memcpy( &rtpiRegs[cInstance][0], &rtpiInitRegs[0], RP_L__TOTAL_REGS);
#ifdef PRD_EEPROM
    SiiRtpiLoadEepromInit();
#endif

    // FW version (x.xx.xx)

    /*
    if (strcmp( buildVersion, strVersion ) != 0 )
    {
        decimal = (buildVersion[0] - 0x30);
        decimal = (decimal * 10) + (buildVersion[2] - 0x30);
        decimal = (decimal * 10) + (buildVersion[3] - 0x30);
        decimal = (decimal * 10) + (buildVersion[5] - 0x30);
        decimal = (decimal * 10) + (buildVersion[6] - 0x30);
    }
	*/

    rtpiRegs[cInstance][2] = decimal & 0xFF;
    rtpiRegs[cInstance][3] = (decimal >> 8) & 0xFF;

    return( true );
}

//------------------------------------------------------------------------------
//! @brief      Process updates to RTPI registers by external devices/programs.
//------------------------------------------------------------------------------
void SiiRtpiProcessRtpi(  BYTE cInstance )
{
    uint8_t val, offset;

    if (rtpiRdPtr[cInstance] == rtpiWrPtr[cInstance]) return;

    while (rtpiWriteBuf[cInstance][rtpiRdPtr[cInstance]][RTPI_CMD_ADDR] != 0)
    {
        offset = rtpiWriteBuf[cInstance][rtpiRdPtr[cInstance]][RTPI_CMD_ADDR];
        val = rtpiWriteBuf[cInstance][rtpiRdPtr[cInstance]][RTPI_CMD_DATA];

//#if DEBUG_PRINT == 1
        if ((pRtpi[cInstance])->debugPrintMode == 0)
        {
            DEBUG_PRINT( MSG_DBG, "SiiRtpiProcessRtpi: [%02X]: %02X(in) ", offset, val);
        }
//#endif // DEBUG_PRINT

        switch( offset )
        {
            case 0x18: val = RtpiRegs0x18( cInstance,  val ); break;

            case 0x1C: val = RtpiRegs0x1C( cInstance,  val ); break;

            case 0x20: val = RtpiRegs0x20(  cInstance, val ); break;
            case 0x21: val = RtpiRegs0x21(  cInstance, val ); break;

            case 0x23: val = RtpiRegs0x23(  cInstance, val ); break;
            case 0x24: val = RtpiRegs0x24(  cInstance, val ); break;

            case 0x28: val = RtpiRegs0x28(  cInstance, val ); break;

            case 0x2B: val = RtpiRegs0x2B(  cInstance, val ); break;

            case 0x2C: val = RtpiRegs0x2C(  cInstance, val ); break;
            case 0x2D: val = RtpiRegs0x2D(  cInstance, val ); break;

            case 0x30: val = RtpiRegs0x30(  cInstance, val ); break;

            case 0x33: val = RtpiRegs0x33(  cInstance, val ); break;
            case 0x34: val = RtpiRegs0x34(  cInstance, val ); break;
            case 0x35: val = RtpiRegs0x35(  cInstance, val ); break;
            case 0x36: val = RtpiRegs0x36(  cInstance, val ); break;
            case 0x37: val = RtpiRegs0x37(  cInstance, val ); break;
            case 0x38: val = RtpiRegs0x38(  cInstance, val ); break;

            case 0x6A: val = RtpiRegs0x6A(  cInstance, val ); break;
            case 0x6B: val = RtpiRegs0x6B(  cInstance, val ); break;
            case 0x6C: val = RtpiRegs0x6C(  cInstance, val ); break;
            case 0x6D: val = RtpiRegs0x6D(  cInstance, val ); break;
            case 0x6E: val = RtpiRegs0x6E(  cInstance, val ); break;
            case 0x6F: val = RtpiRegs0x6F(  cInstance, val ); break;

            default:   val = RtpiRegsDummy(  cInstance, val); break;
        }

//#if DEBUG_PRINT == 1
        if ((pRtpi[cInstance])->debugPrintMode == 0)
        {
            DEBUG_PRINT( MSG_DBG, ": %02X(out)\n",  val);
        }
//#endif // DEBUG_PRINT

        rtpiRegs[cInstance][offset] = val;

        rtpiWriteBuf[cInstance][rtpiRdPtr[cInstance]][RTPI_CMD_ADDR] = 0;
        rtpiWriteBuf[cInstance][rtpiRdPtr[cInstance]][RTPI_CMD_DATA] = 0;
        rtpiRdPtr[cInstance]++;
        if (rtpiRdPtr[cInstance] >= RTPI_CMD_BUFFER_SIZE)
        {
            rtpiRdPtr[cInstance] = 0;
        }
        if (rtpiRdPtr[cInstance] >= rtpiWrPtr[cInstance])
        {
            rtpiRdPtr[cInstance] = rtpiWrPtr[cInstance];
        break;
        }
    }
}

