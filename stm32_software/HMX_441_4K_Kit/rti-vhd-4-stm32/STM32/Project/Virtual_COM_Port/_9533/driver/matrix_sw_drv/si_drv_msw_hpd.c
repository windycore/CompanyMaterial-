//**************************************************************************
//!file     si_drv_hpd.c
//!brief    SiI9535 HPD-Rx Driver functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_drv_device.h"
#include "si_drv_internal.h"
#include "si_drv_switch.h"
#include "si_regs_rxedid953x.h"
#include "si_regs_mhl953x.h"

#if INC_CDC
#   include "si_cdc_component.h"
#endif

#include "Mcu.h"
#include "uart.h"

//-------------------------------------------------------------------------------------------------
//! @brief      Determine the pipe that the specified port is attached to, if any.
//!             attached to the specified pipe.
//!
//! @param[in]  portIndex - SiiPORT_x           - Rx port (0-3).
//!
//! @return     Pipe that is connected to the specified port, or -1 if none.
//-------------------------------------------------------------------------------------------------
int_t SiiDrvRxPipeGet (  BYTE cInstance, int_t portIndex )
{
    int_t           i, pipe;
    uint8_t         rpMask;

    if ( portIndex == (SiiRegRead(  cInstance, REG_RX_PORT_SEL ) & MSK_MP_PORT_SEL))
    {
        pipe = 0;
    }
    else
    {
        // Compare the portIndex to the current roving port by converting
        // from the one-hot roving port select status register(s).
        pipe = -1;
        rpMask = (SiiRegRead(  cInstance, REG_SELECTED_PORT_0 ) & ROVE_PIPE_MASK) >> 4;
        rpMask |= (SiiRegRead(  cInstance, REG_SELECTED_PORT_1 ) & ROVE_PIPE_MASK2);
        for ( i = 0; i < SII_INPUT_PORT_COUNT; i++ )
        {
            if (( 0x01 << i ) == rpMask )
            {
                pipe = 1;
                break;
            }
        }
    }

    return( pipe );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Determine the port that is attached to the specified pipe.
//!
//! @param[in]  pipe    - 0: Main pipe
//!                       1: Roving pipe
//! @return     port index for specified pipe
//-------------------------------------------------------------------------------------------------
int_t SiiDrvRxPipePortGet ( BYTE cInstance, int_t pipe )
{
    int_t           i, portIndex;
    uint8_t         rpMask;

    if ( pipe == 0 )
    {
        portIndex = SiiRegRead( cInstance , REG_RX_PORT_SEL ) & MSK_MP_PORT_SEL;
    }
    else
    {
        // Convert the current roving port by from a one-hot bit value
        // to an index.
        portIndex = 0;
        rpMask = (SiiRegRead(  cInstance ,REG_SELECTED_PORT_0 ) & ROVE_PIPE_MASK) >> 4;
        rpMask |= (SiiRegRead(  cInstance ,REG_SELECTED_PORT_1 ) & ROVE_PIPE_MASK2);
        for ( i = 0; i < SII_INPUT_PORT_COUNT; i++ )
        {
            if (( 0x01 << i ) == rpMask )
            {
                portIndex = i;
                break;
            }
        }
    }

    return( portIndex );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Trigger an automatic hot plug event sequence (ON-OFF) on the port
//!             attached to the specified pipe.
//!
//! @param[in]  pipe    - 0: HPE on port attached to main pipe
//!                       1: HPE on port attached to roving pipe
//! @param[in]  noHpd   - true: do not include physical HPD, false - include physical HPD
//-------------------------------------------------------------------------------------------------
void SiiDrvRxPipeHpeTrigger ( BYTE cInstance,  int_t pipe, bool_t noHpd )
{
    uint8_t triggerMask;

    triggerMask = pipe ? (BIT_OVR_RP | BIT_TRIGGER_RP) : (BIT_OVR_MP | BIT_TRIGGER_MP);
    if ( noHpd )
    {
        // Set end time of HPD HPE timer to 0 so that the auto-HPE hardware
        // will NOT controlled during HPE sequence.
        SiiRegWrite( cInstance,  REG_IP_HPE_HPD_END, 0x00 );
    }
    else
    {
        // Set end time of HPD HPE timer to 1200ms.
        SiiRegWrite( cInstance,  REG_IP_HPE_HPD_END, 0x0C );
    }

    // Toggle the HW HPE for the selected pipe, leave HPE in automatic hardware control
    SiiRegModify( cInstance,  REG_HPE_TRIGGER, triggerMask, SET_BITS );
    SiiRegModify(  cInstance, REG_HPE_TRIGGER, triggerMask, CLEAR_BITS );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Add or remove the specified HEC port in the Auto-HPD Software Interrupt mask.
//!             The type of port decides how we handle Hot Plug Events (HPE) on that port.
//!             If it is regular HDMI, we do standard a Hot Plug. Otherwise, when the
//!             pre-authentication logic determines that the port needs an HPE, it fires off a
//!             SW HPE interrupt and lets the software send the appropriate HPE message over
//!             CDC(CEC) or CBUS(MHL).
//!
//! @param[in]  portIndex - SiiPORT_x           - Rx port (0-3).
//!                         SiiPORT_ALL         - All ports are acted on simultaneously.
//! @param[in]  setEnable - true - enable, false - disable
//! @remarks    This function is a 'cross-instance' function that affects both instances at once.
//-------------------------------------------------------------------------------------------------
void SiiDrvRxAutoHpeConfigure (  BYTE cInstance, int_t portIndex, SiiPortType_t portType )
{
    uint8_t portMask, mask;

    // Clear the mask bit position in the Auto-HPD mask register (default to HDMI)
    mask        = (1 << portIndex) << 2;
    portMask    = (SiiRegRead(  cInstance, REG_PHYS_HPD_DISABLE ) & ~mask);

    // If NOT an HDMI port, set the corresponding bit in the HEC/CBUS port HPD mask
    // set for both instances
    drvSwitchInstance[cInstance][0].portType[ portIndex] = portType;
#if (SII_NUM_SWITCH > 1)
    drvSwitchInstance[cInstance][1].portType[ portIndex] = portType;
#endif
    if ( portType != SiiPortType_HDMI )
    {
        portMask |= mask;
    }

    SiiRegBitsSet(  cInstance, REG_INT_ENABLE_IP1, BIT_MP_CEC_HPE|BIT_RP_CEC_HPE, (portType != SiiPortType_HDMI) );
    SiiRegModify(  cInstance, REG_PHYS_HPD_DISABLE, VAL_FW_HPE_MASK, portMask );   // Update the hardware

    if ( ( portType == SiiPortType_MHL ) && (portIndex == 1) )
    {
    	//SiiRegWrite(REG_CDSENSE_CTRL, BIT_MHL_SEL_CDSENSE1);
    	SiiRegBitsSet( cInstance, REG_CDSENSE_CTRL,BIT3,0x01);
    }//
    else if ( ( portType == SiiPortType_HDMI ) && (portIndex == 1) )
    {
    	SiiRegBitsSet( cInstance, REG_CDSENSE_CTRL,BIT3,0x00);
    }
    else
    {
    }

    if ( ( portType == SiiPortType_MHL ) && (portIndex == 0) )
    {
    	SiiRegBitsSet( cInstance, REG_CDSENSE_CTRL,BIT2|BIT0, 0x03);
    }
    else if ( ( portType == SiiPortType_HDMI ) && (portIndex == 0) )
    {
    	SiiRegBitsSet( cInstance, REG_CDSENSE_CTRL,BIT2|BIT0, 0x00);
    }
    else
    {
    }

   	//DEBUG_PRINT(MSG_ALWAYS, ("CBus Driver:: SiiDrvRxAutoHpeConfigure() REG_CDSENSE_CTRL written \n"));
}

//-------------------------------------------------------------------------------------------------
//! @brief      Enable or disable the the EDID DDC bus for a Rx port.
//!
//! @param[in]  portIndex - SiiPORT_x           - Rx port (0-3).
//!                         SiiPORT_ALL         - All ports are acted on simultaneously.
//! @param[in]  setEnable - true - enable, false - disable
//-------------------------------------------------------------------------------------------------
void SiiDrvRxEdidDdcControl ( BYTE cInstance,  uint8_t portIndex, bool_t setEnable )
{
    uint8_t enableVal, enableMask;

    enableVal = setEnable ? SET_BITS : CLEAR_BITS;

    if ( portIndex == SiiPORT_ALL )     // All ports at once
    {
        enableMask = VAL_EN_DDC_ALL;
    }
    else                                // Port 0 - 3
    {
        enableMask = (BIT_EDDC_EN0 << portIndex);
    }

    SiiRegModify(  cInstance, RX_EDID_DDC_EN, enableMask, enableVal );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Enable or disable the the HDCP DDC bus for a Rx port.
//!
//! @param[in]  portIndex - SiiPORT_x           - Rx port (0-3).
//!                         SiiPORT_ALL         - All ports are acted on simultaneously.
//! @param[in]  setEnable - true - enable, false - disable
//-------------------------------------------------------------------------------------------------
void SiiDrvRxHdcpDdcControl ( BYTE cInstance,  uint8_t portIndex, bool_t setEnable )
{
    uint8_t enableVal, enableMask;

    enableVal = setEnable ? SET_BITS : CLEAR_BITS;

    if ( portIndex == SiiPORT_ALL )     // All ports at once
    {
        enableMask = VAL_DDC_PORT_ALL_EN;
    }
    else                                // Port 0 - 3
    {
        enableMask = (BIT_DDC0_EN << portIndex);
    }

    SiiRegModify(  cInstance, REG_RX_HDCP_DDC_EN, enableMask, enableVal );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set the physical state of the RX termination for a Rx port.
//!             If the port is configured as MHL the termination is set to tri-state on enable
//!
//! @param[in]  portIndex - SiiPORT_x           - Rx port (0-3).
//! @param[in]  setEnable - true - enable, false - disable
//!
//! @warning    The 'portIndex' parameter value 0xFF should not be used unless
//! @warning    all ports are HDMI1.3/a (not MHL or CDC)
//-------------------------------------------------------------------------------------------------
void SiiDrvRxTermControl (  BYTE cInstance, int_t portIndex, bool_t setEnable )
{
    uint8_t enableVal, rPwrStatus;
    uint8_t enableMask = 0;

    // MHL port termination is handled in hardware automatically if 
    // NVRAM location 0x28 bit 5 is set, which is recommended, so
    // we will leave the register alone for MHL ports.
/*    if ( pDrvSwitch[cInstance]->portType[ portIndex] == SiiPortType_MHL )
    {
        return;
    }
*/

    if ( pDrvSwitch[cInstance]->portType[ portIndex] == SiiPortType_MHL )
    {
    	// If MHL port, enable value is different
   		enableVal = setEnable ? (0x01 << (portIndex * 2)) : VAL_TERM_ALL_OFF;
	    enableMask  = (MSK_TERM << ((portIndex % 4) * 2)); //added by mahesh - remove if nvram works

//	    SiiRegWrite( cInstance,  REG_RX_TMDS_TERM_0, 0x01 );

	    //SiiRegWrite(REG_HDMIM_MODE_OVWR, 0x00); // Enable MHL 3X

	    if(portIndex == 0)
	    {
	    	SiiRegModify(  cInstance, REG_RX_TMDS_TERM_0, 0x03, 0x01 );
			//SiiRegWrite( cInstance,  REG_TMDS0_CTRL2, 0x20);
			if(SiiRegRead( cInstance, REG_HDMIM_MODE_OVWR) & BIT_MHL_1X_EN)
				SiiRegWrite( cInstance,  REG_TMDS0_CTRL2, REG_MHL_TEST_SOFT_CTRL_1X);
			else
				SiiRegWrite( cInstance,  REG_TMDS0_CTRL2, REG_MHL_TEST_SOFT_CTRL_3X);

			// PLL configuration for port 0
			SiiRegWrite( cInstance,  REG_TMDS0_CTRL3, 0x00);
	    }
	    else
	    {
	    	SiiRegModify(  cInstance, REG_RX_TMDS_TERM_0, 0x0C, 0x04 );
			SiiRegWrite( cInstance,  REG_TMDS0_CTRL2, 0x20);

			// PLL configuration for port 1
			SiiRegWrite( cInstance,  REG_TMDS1_CTRL3, REG_HDMI_RX1_PLL_CONFIG);

			if(SiiRegRead( cInstance, REG_HDMIM_MODE_OVWR) & BIT_MHL_1X_EN)
				SiiRegWrite( cInstance,  REG_TMDS1_CTRL2, REG_HDMI_RX1_MHL_TEST_1X);
			else
				SiiRegWrite( cInstance,  REG_TMDS1_CTRL2, REG_HDMI_RX1_MHL_TEST_3X);
	    }

   	    //DEBUG_PRINT(MSG_ALWAYS,("\n MHL TERMINATION ***************************Mask: %02X val: %02X \n",enableMask,  enableVal));
    }
	else
	{
		// if the port is not connected we don't want to terminate that one.
		// However, if setEnable is false we don't care and just unterminate the port, there
		// are cases when you have to unterminate the port in Matrix Switch mode even when the
		// port is connected to source
   	    SiiRegWrite( cInstance,  REG_TMDS0_CTRL2, 0x00);
    //   DEBUG_PRINT( MSG_ERR, "\n******SiiPortType_HDMI******");
   	    // PLL configuration
   	    SiiRegWrite( cInstance,  REG_TMDS0_CTRL3, 0x00);

		if(setEnable)
		{
			// see if the port is connected to a source or not
			rPwrStatus = SiiRegRead( cInstance,  REG_PWR5V_STATUS ) & MSK_PWR5V_ALL;
			if(!((rPwrStatus >> portIndex) & BIT0))
			{
				setEnable = false;
			}
		}


		/*
		DEBUG_PRINT( MSG_DBG, "SiiDrvRxTermControl , portIndex:: %02X, setEnable:: %02X\n\n\n",
				portIndex, setEnable );
		*/

		enableVal = setEnable ? VAL_TERM_ALL_ON : VAL_TERM_ALL_OFF;

		// Move enable value bits to correct position
		enableMask  = (MSK_TERM << ((portIndex % 4) * 2));
	    SiiRegModify(  cInstance, REG_RX_TMDS_TERM_0, enableMask, enableVal );
	    //DEBUG_PRINT(MSG_ALWAYS,"\n HDMI TERMINATION ***********************************88888\n");
   	   // DEBUG_PRINT(MSG_ALWAYS,"\n HDMI TERMINATION ***************************Mask: %02X val: %02X \n",enableMask,  enableVal);

	}
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set the physical state of the Hot Plug Detect (HPD) signal for a Rx port.
//!             If the port is configured as MHL the HPD line is set to tri-state on enable
//!
//! @param[in]  portIndex - SiiPORT_x           - Rx port (0-3).
//!                         SiiPORT_ALL         - All ports are acted on simultaneously.
//! @param[in]  setActive - true - set HPD to active (high or tri-state), false - set HPD to low state
//-------------------------------------------------------------------------------------------------
void SiiDrvRxHpdControl (  BYTE cInstance, uint8_t portIndex, bool_t setActive )
{
    uint8_t     enableVal;
    SiiReg_t    ctrlAddress;

    enableVal = setActive ?  VAL_HP_PORT_ALL_HI : VAL_HP_PORT_ALL_LO;

    if ( portIndex == SiiPORT_ALL ) // All at once, must be HDMI ports.
    {
        SiiRegWrite( cInstance,  REG_HP_CTRL1, enableVal );
    }
    else
    {
        // If MHL port, enable means tri-state
        if ( pDrvSwitch[cInstance]->portType[ portIndex] == SiiPortType_MHL )
        {
            enableVal = setActive ? VAL_HP_PORT_ALL_MHL : enableVal;
        }

        // Find correct control register and update.
        ctrlAddress = REG_HP_CTRL1;
        SiiRegModify(  cInstance, ctrlAddress, (VAL_HP_PORT0_MASK << (portIndex * 2)), enableVal );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set the Hot Plug Event (HPE) state of a single Rx port.
//!             If the port is configured as MHL or HEC/CDC, the appropriate set state function
//!             is called instead of physically changing the pin state.
//!
//! @param[in]  portIndex - SiiPORT_x           - Rx port (0-3).
//!                       - SiiPORT_Pipe0       - Port attached to pipe 0
//!                       - SiiPORT_Pipe1       - Port attached to pipe 1
//! @param[in]  newState  - Same as SiiDrvRxHpeStateSet
//!
//! @return     true - success  false - some failure occurred
//-------------------------------------------------------------------------------------------------
static bool_t HpeStateSetSinglePort (  BYTE cInstance, int_t portIndex, SiiDrvHpdActions_t newState )
{
    bool_t  success = true;
    int_t   pipe = 0;

    // Validate portIndex parameter and determine real portIndex if a pipe requested.
    switch ( portIndex )
    {
        case SiiPORT_Pipe0:
        case SiiPORT_Pipe1:
            pipe = portIndex - SiiPORT_Pipe0;
            portIndex = SiiDrvRxPipePortGet( cInstance,  pipe );
            break;
        default:
            if ( portIndex >= SII_INPUT_PORT_COUNT )
            {
                return( false );
            }

            // Get the associated pipe (for TOGGLE command)
            pipe = SiiDrvRxPipeGet( cInstance,  portIndex );
            break;
    }

    // Perform physical register control common to all types of ports (HDMI/MHL/CDC)
    // This means everything except HPD signaling
    switch ( newState )
    {
        // HDCP access enabled, RX Term enabled, EDID access enabled
        case SiiHPD_ACTIVE:
            SiiDrvRxHdcpDdcControl( cInstance,  portIndex, true );
            SiiDrvRxTermControl(  cInstance, portIndex, true );
            SiiDrvRxEdidDdcControl( cInstance,  portIndex, true );
            break;

		// HDCP access disabled, RX Term disabled, EDID access disabled
		case SiiHPD_INACTIVE:
			SiiDrvRxHdcpDdcControl( cInstance,  portIndex, false );
			SiiDrvRxTermControl(  cInstance, portIndex, false );
			SiiDrvRxEdidDdcControl( cInstance,  portIndex, false );
			break;
		case SiiHPD_TOGGLE:
			break;
		// EDID access enabled
		case SiiHPD_ACTIVE_EDID:
			SiiDrvRxEdidDdcControl(  cInstance, portIndex, true );
			break;
		// EDID access disabled
		case SiiHPD_INACTIVE_EDID:
			SiiDrvRxEdidDdcControl(  cInstance, portIndex, false );
			break;
		default:
			break;
		}

    // Perform HPD signaling appropriate to the port type.
    switch ( pDrvSwitch[cInstance]->portType[ portIndex] )
    {
        case SiiPortType_HDMI:
            switch ( newState )
            {
                case SiiHPD_ACTIVE:
                    SiiDrvRxHpdControl(  cInstance, portIndex, true );
                    break;
                case SiiHPD_INACTIVE:
                    SiiDrvRxHpdControl( cInstance,  portIndex, false );
                    break;
                case SiiHPD_TOGGLE:

                    //Use auto-HPE hardware
                    SiiDrvRxPipeHpeTrigger(  cInstance, pipe, false );
                    break;
                default:
                    success = false;
                    break;
            }

            break;
#if INC_CBUS
        case SiiPortType_MHL:
            success = SiiMhlCbHpdSetState(  cInstance, portIndex, newState );
            break;
#endif
#if INC_CDC
        case SiiPortType_HEC:
            if ( newState == SiiHPD_TOGGLE )
            {
                // For HEC ports, we want to physically toggle all but the HPD line
                SiiDrvRxPipeHpeTrigger(  cInstance, pipe, true );
            }
            success = SiiCdcHpdSetState(  cInstance, portIndex, newState );
            break;
#endif
        default:
            success = false;
            break;
    }

    return( success );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set the Hot Plug Event (HPE) state of the specified Rx port or ports.
//!             An HPE affects the port HPD signal, HDCP DDC access, EDID DDC access, and Rx termination.
//!
//! @param[in]  portIndex - SiiPORT_x           - Rx port (0-3).
//!                       - SiiPORT_Pipe0       - Port attached to pipe 0
//!                       - SiiPORT_Pipe1       - Port attached to pipe 1
//!                       - SiiPORT_ALL         - All ports
//! @param[in]  newState  - SiiHPD_INACTIVE     - HPD HI, HDCP, EDID, RX Term disabled
//!                       - SiiHPD_ACTIVE       - HPD HI, HDCP, EDID, RX Term enabled
//!                       - SiiHPD_TOGGLE       - Toggle all controls off then on
//!                                             - Note that this affects ONLY a port attached to a
//!                                               pipe. Any other port will be ignored.
//!                       - SiiHPD_INACTIVE_EDID- EDID access disabled
//!                       - SiiHPD_ACTIVE_EDID  - EDID access enabled
//!
//! @return     true - success  false - some failure occurred
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvRxHpeStateSet ( BYTE cInstance, int_t portIndex, SiiDrvHpdActions_t newState )
{
    bool_t  success = true;
    bool_t  portResult;
    uint8_t i;

    switch ( portIndex )
    {
        case SiiPORT_0:
        case SiiPORT_1:
        case SiiPORT_2:
        case SiiPORT_3:
        case SiiPORT_Pipe0:
        case SiiPORT_Pipe1:
            success = HpeStateSetSinglePort( cInstance , portIndex, newState );
            break;

        case SiiPORT_ALL:
            for ( i = 0; i < SII_INPUT_PORT_COUNT; i++ )
            {
                // Set the state for this port
                portResult = HpeStateSetSinglePort(  cInstance , i, newState );
                // Accumulate success or failure
                success =  portResult && success;
            }
            break;

        default:
            success = false;
            break;
    }

    return( success );
}

