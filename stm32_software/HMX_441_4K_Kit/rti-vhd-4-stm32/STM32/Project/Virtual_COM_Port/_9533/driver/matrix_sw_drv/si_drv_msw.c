//**************************************************************************
//!file     si_drv_msw.c
//!brief    SiI9535 Matrix Port Switch driver functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "string.h"
#include "si_drv_msw_internal.h"
#include "si_drv_msw_cfg.h"
#include "si_drv_internal.h"
#include "si_regs_rxedid953x.h"
//#if INC_CBUS
#include "si_regs_mhl953x.h"
//#endif
#include "si_drv_device.h"
#include "si_drv_switch.h"
#include "si_gpio.h"
#include "sk_application.h"

#include "Mcu.h"
#include "uart.h"
//------------------------------------------------------------------------------
//  Driver Data
//------------------------------------------------------------------------------

SwitchDrvInstanceData_t drvSwitchInstance[DevNum][SII_NUM_SWITCH];
SwitchDrvInstanceData_t *pDrvSwitch[DevNum] =
{
    &drvSwitchInstance[0][0],
	
#if(DevNum>1)
    &drvSwitchInstance[1][0],
#endif
#if(DevNum>2)
    &drvSwitchInstance[2][0],
#endif
#if(DevNum>3)
    &drvSwitchInstance[3][0],
#endif
#if(DevNum>4)
    &drvSwitchInstance[4][0],
#endif
#if(DevNum>5)
    &drvSwitchInstance[5][0],
#endif


};

SwitchDrvCommonData_t drvSwitchCommon[DevNum];

SiiMswRegisterConfData_t  drvSwitchRegisterConf[DevNum];
SiiMswInstanceCommonData_t drvCommonData[DevNum];

//------------------------------------------------------------------------------
// Switch Driver Register initialization list
//------------------------------------------------------------------------------

static uint8_t  videoSourceSelects [SiiSwitchSourceCount] =
{
    VAL_MUX_SEL_HDMI_MP,
    VAL_MUX_SEL_HDMI_MP,
    VAL_MUX_SEL_HDMI_MP,
#if SII_DEV_9535
    VAL_MUX_SEL_HDMI_MP,
#endif
    VAL_MUX_SEL_INTERNAL
};


//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchInstanceSet
// Description: Set the component global data pointer to the requested instance.
// Parameters:  instanceIndex
// Returns:     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------

bool_t SiiDrvSwitchInstanceSet ( BYTE cInstance, uint_t instanceIndex )
{
    // Always use instance 0 for CRA
    SiiRegInstanceSet(  cInstance ,PP_PAGE, 0 );

    if ( instanceIndex < SII_NUM_SWITCH )
    {
        pDrvSwitch[cInstance] = &drvSwitchInstance[cInstance][ instanceIndex];
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchInstanceGet
// Description: Return the current instance index.
// Parameters:  none
// Returns:     instance index.
//------------------------------------------------------------------------------

int_t SiiDrvSwitchInstanceGet (  BYTE cInstance )
{
    return( pDrvSwitch[cInstance]->instanceIndex );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchInitialize
// Description:
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//              if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvSwitchInitialize (  BYTE cInstance )
{
    // Clear instance data
    memset( pDrvSwitch[cInstance], 0, sizeof( SwitchDrvInstanceData_t ));

    // Set instance-specific values.
    if ( pDrvSwitch[cInstance] == &drvSwitchInstance[cInstance][0])
    {
        pDrvSwitch[cInstance]->instanceIndex           = 0;
        pDrvSwitch[cInstance]->mswZoneEnabled          = true;
        pDrvSwitch[cInstance]->mswVideoSource          = SiiSwitchSource2;
        pDrvSwitch[cInstance]->mswAudioZoneSource      = SiiSwitchSource2;
        pDrvSwitch[cInstance]->mpInfoFrameMask         = 0xFF; //Enable New VSIF indicator on MP
        pDrvSwitch[cInstance]->zoneTracking            = true;
    }
#if (SII_NUM_SWITCH > 1)
    else
    {
        pDrvSwitch[cInstance]->instanceIndex           = 1;
        pDrvSwitch[cInstance]->mswZoneEnabled          = true;
        pDrvSwitch[cInstance]->mswVideoSource          = SiiSwitchSource0;
        pDrvSwitch[cInstance]->mswAudioZoneSource      = SiiSwitchSource0;
        pDrvSwitch[cInstance]->mpInfoFrameMask         = 0xFF; //Enable New VSIF indicator on RP
        pDrvSwitch[cInstance]->zoneTracking            = true;
    }
#endif
    drvCommonData[cInstance].pipeSel[pDrvSwitch[cInstance]->instanceIndex] = SiiSwitchHdmiPipeMain;

    if ( pDrvSwitch[cInstance]->instanceIndex == 0 )
    {
        // Clear the hardware Auto-HPD mask to default (All HDMI),
        SiiRegModify(  cInstance ,REG_PHYS_HPD_DISABLE, VAL_FW_HPE_MASK, CLEAR_BITS );

        // Set the end point for the EDID DDC disable timer to 1200ms
        SiiRegWrite( cInstance,  REG_IP_HPE_EDID_DDC_END, 0x0C );

        // Enable the interrupts for this driver
        SiiRegBitsSet(  cInstance ,REG_INT_ENABLE_P3, BIT_P3_ENABLE_MP_NEW_ALL | BIT_P3_ENABLE_MP_NO_ALL, true ); // MP
        SiiRegBitsSet(  cInstance ,REG_INT_ENABLE_P4, BIT_RP_NEW_AVI, false ); // RP: disabled by default
        SiiRegBitsSet(  cInstance ,REG_INT_ENABLE_5, 0xFF, true );

        // Enable both Tx0 and Tx1 by default
        SiiRegWrite( cInstance,  REG_TX_VIDEO_SRC, BIT_TX0_PIPE_EN);
        drvSwitchRegisterConf[cInstance].sourceControl |= BIT_TX0_PIPE_EN; //same as initial values
        drvSwitchRegisterConf[cInstance].tmdsPortSelect = 0;
    }

    // Enable Video clock counter
    SiiRegInstanceSet(  cInstance , PP_PAGE_AUDIO, pDrvSwitch[cInstance]->instanceIndex);
    SiiRegBitsSet( cInstance , REG_XPCLK_ENABLE, BIT_XPCLK_ENABLE, SET_BITS);

    return( true );
}

//------------------------------------------------------------------------------
//! @brief      Place the switch driver in standby state.
//------------------------------------------------------------------------------
bool_t SiiDrvSwitchStandby (  BYTE cInstance )
{
    // Disable interrupts used by this driver
    SiiDrvDeviceSpResDetectionEnable( cInstance ,false);
    SiiRegBitsSet(  cInstance ,REG_INT_ENABLE_P3, BIT_P3_ENABLE_MP_NEW_ALL | BIT_P3_ENABLE_MP_NO_ALL, false );
    SiiRegBitsSet(  cInstance ,REG_INT_ENABLE_P4, BIT_RP_NEW_AVI, false );
    SiiRegBitsSet(  cInstance ,REG_INT_ENABLE_5, 0xFF, false );

    // Clear outstanding interrupts
    SiiRegWrite( cInstance,  REG_INT_STATUS_P3, BIT_P3_ENABLE_MP_NEW_ALL | BIT_P3_ENABLE_MP_NO_ALL );
    SiiRegWrite( cInstance,  REG_INT_STATUS_P4, BIT_RP_NEW_AVI );
    SiiRegWrite( cInstance,  REG_INT_STATUS_5, 0xFF );

    // No longer want to respond to sources plugged into our ports.
    return( SiiDrvRxHpeStateSet(  cInstance ,SiiPORT_ALL, SiiHPD_INACTIVE ) );
}

//------------------------------------------------------------------------------
//! @brief      Restore normal switch driver operation.
//------------------------------------------------------------------------------
bool_t SiiDrvSwitchResume (  BYTE cInstance )
{

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchStatus
// Description: Returns a status flag word containing Switch driver-specific
//              information about the state of the device.
// Parameters:  mask - 0x0000:   Return the entire status word without clearing driver status flags
//                     non-zero: Return the status bits indicated by the set mask bits.
//                               If a mask bit refers to one of the switch interrupt flags covered
//                               (except the main switch INT flag), the bit is cleared in the driver
//                               status.  The main switch INT status bit is NOT cleared until
//                               ALL switch interrupt flag bits have been read.
// Returns:     Sixteen-bit status flags word for the Switch Driver
//------------------------------------------------------------------------------

SwitchStatusFlags_t SiiDrvSwitchStatus (  BYTE cInstance, SwitchStatusFlags_t mask )
{
    SwitchStatusFlags_t intClearMask;
    SwitchStatusFlags_t statusFlags = pDrvSwitch[cInstance]->statusFlags;

    if ( mask )
    {
        // Return only the flags they are interested in

        statusFlags &= mask;

        // Clear the interrupt flags that were requested (except main interrupt flag).

        intClearMask = (SwitchStatusFlags_t)(mask & SiiSWITCH_INTFLAGS);
        pDrvSwitch[cInstance]->statusFlags &= ~intClearMask;

        // If all interrupt flags are cleared, clear the main interrupt flag

        if (( pDrvSwitch[cInstance]->statusFlags & (SiiSWITCH_INTFLAGS | SiiSWITCH_SCDT_CHANGE |
                SiiSWITCH_MP_INFOFRAME_INTR_VALID | SiiSWITCH_SP_INFOFRAME_INTR_VALID)) == 0 )
        {
            pDrvSwitch[cInstance]->statusFlags &= ~SiiSWITCH_INT;
        }
    }

    return( statusFlags );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchIfInterruptStatusGet
// Description: Returns the last Infoframe interrupt Status data retrieved by
//              the Switch ISR.
// Parameters:  pMpIntStatus, pSpIntStatus - pointers to return data buffer (1 byte).
// Returns:     true if new interrupt status data is available, false if not.
//              pData - Destination for interrupt status data.
//------------------------------------------------------------------------------

bool_t  SiiDrvSwitchIfInterruptStatusGet ( BYTE cInstance,  uint8_t *pMpIntStatus, uint8_t *pSpIntStatus)
{

    *pMpIntStatus = pDrvSwitch[cInstance]->mpInfoFrameInterruptStatus;
    *pSpIntStatus = pDrvSwitch[cInstance]->spInfoFrameInterruptStatus;
    if ( pDrvSwitch[cInstance]->statusFlags & (SiiSWITCH_MP_INFOFRAME_INTR_VALID | SiiSWITCH_SP_INFOFRAME_INTR_VALID))
    {
        if ( pDrvSwitch[cInstance]->statusFlags & SiiSWITCH_MP_INFOFRAME_INTR_VALID )
        {
            pDrvSwitch[cInstance]->statusFlags &= ~SiiSWITCH_MP_INFOFRAME_INTR_VALID;
        }

        if ( pDrvSwitch[cInstance]->statusFlags & SiiSWITCH_SP_INFOFRAME_INTR_VALID )
        {
            pDrvSwitch[cInstance]->statusFlags &= ~SiiSWITCH_SP_INFOFRAME_INTR_VALID;
        }

        // If all interrupt flags are cleared, clear the main interrupt flag
        // (we KNOW the above interrupt has been cleared, check the rest.)

        if (( pDrvSwitch[cInstance]->statusFlags & (SiiSWITCH_INTFLAGS | SiiSWITCH_SCDT_CHANGE )) == 0 )
        {
            pDrvSwitch[cInstance]->statusFlags &= ~SiiSWITCH_INT;
        }
        return( true );
    }
    return( false );
}


//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchStateGet
// Description: Get the selected state type information.
// Parameters:  stateType   - Type of state returned.
// Returns:     Returns an eight bit state value of the selected type.
//------------------------------------------------------------------------------

uint8_t SiiDrvSwitchStateGet ( BYTE cInstance, SiiDrvSwitchStateTypes_t stateType )
{
    uint8_t stateValue;

    stateValue = 0;
    switch ( stateType )
    {
        case SiiSwitch_SELECTED_INPUT_CONNECT_STATE:
            stateValue = drvSwitchCommon[cInstance].mpCableIn;
            break;
        case SiiSwitch_INPUT_PORTS_CONNECT_STATE:

            // Include both HDMI and MHD.
            stateValue  = SiiRegRead(  cInstance, REG_PWR5V_STATUS ) & MSK_PWR5V_ALL;
#if INC_CBUS
            stateValue  |= SiiRegRead(  cInstance, REG_HDMIM_CP_PAD_STAT ) & MSK_MHL_PORT_ALL;
#endif
            break;
        case SiiSwitch_SELECTED_PORT:       // For main pipe only

            stateValue  = SiiRegRead( cInstance,  REG_RX_PORT_SEL ) & MSK_MP_PORT_SEL;
            break;
        case SiiSwitch_SELECTED_PORT_SUB:   // For sub-pipe only

            stateValue  = (SiiRegRead( cInstance,  REG_RX_PORT_SEL ) & MSK_RP_PORT_SEL) >> VAL_RP_SHIFT;
            break;
#if INC_RTPI
        case  SiiSwitch_TX_VIDEO_SRC:		// For TX video Source

        	stateValue  = SiiRegRead( cInstance, REG_TX_VIDEO_SRC);
        	break;
#endif
    }

    return( stateValue );
}

#if (SWITCH_LINK_CALLBACK == SI_DISABLE)
//------------------------------------------------------------------------------
// Function:    SwitchCallback
// Description: Call the specified indirect callback function if it has been
//              registered.
//------------------------------------------------------------------------------
/*static void SwitchCallback ( int callbackType, uint16_t statusFlags )
{
    void (*pCallback)(uint16_t) = 0;

    switch ( callbackType )
    {
    case CALLBACK_STATUS_CHANGE:

        pCallback = pDrvSwitch[cInstance]->pCallbackStatusChange;
        break;
    }

    if ( pCallback )
    {
        (pCallback)( statusFlags );
    }
}  */
#endif

//-------------------------------------------------------------------------------------------------
//! @brief      Disable both TX outputs (not a mute)
//!
//! @remarks    This function is a 'cross-instance' function that affects both instances at once.
//-------------------------------------------------------------------------------------------------
void SiiDrvSwitchOutputEnable (  BYTE cInstance,bool_t isEnable )
{
    SiiSwitchZoneEnables_t  zoneEnable;
    uint8_t sourceControl;

    pDrvSwitch[cInstance]->lastResultCode = SII_SUCCESS;
    sourceControl = drvSwitchRegisterConf[cInstance].sourceControl & ~(BIT_TX0_PIPE_EN);

    if ( isEnable )
    {
        zoneEnable = (SiiSwitchZoneEnables_t)0;
        if ( drvSwitchInstance[cInstance][0].mswZoneEnabled )
        {
            zoneEnable |= SiiSwitchZone1Enabled;
        }
#if (SII_NUM_SWITCH > 1)
        if ( drvSwitchInstance[cInstance][1].mswZoneEnabled )
        {
            zoneEnable |= SiiSwitchZone2Enabled;
        }
#endif
        sourceControl |= (zoneEnable << VAL_PIPE_EN_SHIFT);
    }
    drvSwitchRegisterConf[cInstance].sourceControl = sourceControl;
    SiiDrvSwitchZoneConfigSet( cInstance );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Enable or disable zone tracking mode
//
//! @remarks    This function is a 'cross-instance' function that affects both instances at once.
//-------------------------------------------------------------------------------------------------
void SiiDrvSwitchZoneTrackingSet(  BYTE cInstance, bool_t isEnabled )
{

    drvSwitchInstance[cInstance][0].zoneTracking = isEnabled;
#if (SII_NUM_SWITCH > 1)
    drvSwitchInstance[cInstance][1].zoneTracking = isEnabled;
#endif
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return zone tracking mode state.
//
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvSwitchZoneTrackingGet( BYTE cInstance )
{
    return( pDrvSwitch[cInstance]->zoneTracking );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return zone enable state for this instance
//
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvSwitchZoneEnableGet( BYTE cInstance  )
{
    return( pDrvSwitch[cInstance]->mswZoneEnabled );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set the current zone (instance) enable flag
//
//-------------------------------------------------------------------------------------------------
void SiiDrvSwitchZoneEnableSet ( BYTE cInstance ,bool_t isEnable )
{
    pDrvSwitch[cInstance] ->lastResultCode = SII_SUCCESS;

    pDrvSwitch[cInstance] ->mswZoneEnabled = isEnable;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Configure the matrix switch according to the current source and
//! @brief      zone settings.
//
//! @remarks    This function is a 'cross-instance' function that affects both instances at once.
//-------------------------------------------------------------------------------------------------
void SiiDrvSwitchConfigureMatrixSwitch (BYTE cInstance, bool_t forceNonMatrix, bool_t portSelectOnly )
{
    uint8_t sourceControl;
    uint8_t tmdsPortSelect;
    uint8_t mpPortSelect;
    uint8_t tx0Source = 0;
    uint8_t zone1Source;     // HDMI transmitter zones
    uint8_t zoneA1Source;   // Audio extraction zones
#if (SII_NUM_SWITCH > 1)
    uint8_t spPortSelect;
    uint8_t tx1Source = 0;
    uint8_t zone2Source;     // HDMI transmitter zones
    uint8_t zoneA2Source;   // Audio extraction zones
#endif
    uint8_t rxPort;

    pDrvSwitch[cInstance]->lastResultCode = SII_SUCCESS;

    zone1Source = drvSwitchInstance[cInstance][0].mswVideoSource;
    zoneA1Source = drvSwitchInstance[cInstance][0].mswAudioZoneSource;
#if (SII_NUM_SWITCH > 1)
    zone2Source = drvSwitchInstance[cInstance][1].mswVideoSource;
    zoneA2Source = drvSwitchInstance[cInstance][1].mswAudioZoneSource;
#endif
    // Get current hardware settings and clear the parts we're interested in.
    tmdsPortSelect  = SiiRegRead(cInstance, REG_RX_PORT_SEL) & MASK_PORT_SEL;
    // TX0 Pipe enable.
   // 0 ¨C Disable TX0 pipe
   // 1 ¨C Enable TX0 pipe
    sourceControl   = SiiRegRead( cInstance,REG_TX_VIDEO_SRC ) & ~(BIT_TX0_PIPE_EN);

    // Zone 1 source and/or audio zone 1 source
    if (zone1Source < SiiSwitchSourceInternalVideo)
    {
        tx0Source = VAL_MUX_SEL_HDMI_MP;
        mpPortSelect = zone1Source;
    }
    else
    {
        tx0Source = videoSourceSelects[zone1Source];
        mpPortSelect = zoneA1Source;
    }
#if (SII_NUM_SWITCH > 1)
    // Zone 2 source and/or audio zone 2 source
    if (zone2Source < SiiSwitchSourceInternalVideo)
    {
        tx1Source = VAL_MUX_SEL_HDMI_RP;
        spPortSelect = zone2Source;
    }
    else
    {
        tx1Source = videoSourceSelects[zone2Source];
        spPortSelect = zoneA2Source;
    }
#endif

    tmdsPortSelect = (tmdsPortSelect & ~MSK_MP_PORT_SEL) | (mpPortSelect << VAL_MP_SHIFT);
#if (SII_NUM_SWITCH > 1)
    tmdsPortSelect = (tmdsPortSelect & ~MSK_RP_PORT_SEL) | (spPortSelect << VAL_RP_SHIFT);
#endif
    //mpPortSelect=spPortSelect;
    // If the two TMDS sources are different ports
   // if ((mpPortSelect != spPortSelect))
    if(0)//for melboune we don't have two separtate Tx.so we dont have Matrix mode.By default non matrix has to work.Due to this Pip is not working..
    {
        // Disable Rx ports that are not active sources, otherwise enable
        for (rxPort = SiiPORT_0; rxPort < SII_INPUT_PORT_COUNT; ++rxPort)
        {
            if ((rxPort != zone1Source) &&(rxPort != zoneA1Source)
#if (SII_NUM_SWITCH > 1)
            		&& (rxPort != zone2Source) && (rxPort != zoneA2Source)
#endif
            		)
            {
                //SiiDrvRxTermControl(rxPort, false);
                //SiiDrvRxHdcpDdcControl(rxPort, false);
            }
            else
            {
                SiiDrvRxTermControl(cInstance,rxPort, true);
                SiiDrvRxHdcpDdcControl(cInstance,rxPort, true);
            }
        }

        // Enable SP resolution change detection
        SiiDrvDeviceSpResDetectionEnable(cInstance,true);
    }
    else
    {
   	    //   DEBUG_PRINT(MSG_ALWAYS,"\n Non-Matrix mode \n");
		if ((tx0Source < VAL_MUX_SEL_INTERNAL))
		{
			// If both TMDS sources are the same port or one of the sources is non-TMDS,
			// disable matrix mode and point pipe0 at TXs associated with TMDS input
		if (tx0Source < VAL_MUX_SEL_INTERNAL)
		{
			tx0Source = VAL_MUX_SEL_HDMI_MP;
		}
#if (SII_NUM_SWITCH > 1)
		if (tx1Source < VAL_MUX_SEL_INTERNAL)
		{
			tx1Source = VAL_MUX_SEL_HDMI_MP;
		}
#endif
            // Enable all ports to allow PA and IPV functions
            for (rxPort = SiiPORT_0; rxPort < SII_INPUT_PORT_COUNT; ++rxPort)
            {
                SiiDrvRxTermControl(cInstance,rxPort, true);
                SiiDrvRxHdcpDdcControl(cInstance,rxPort, true);
            }
        }
        // Disable SP resolution change detection
        SiiDrvDeviceSpResDetectionEnable(cInstance,false);
    }

    // Zone 1 source becomes tx0 source
    // Zone 2 source becomes tx1 source
    // Clear out previous TX0 and TX1 source muxes and add in new values
#if (SII_NUM_SWITCH > 1)
    sourceControl = sourceControl & ~(MSK_MUX_SEL_TX0 | MSK_MUX_SEL_TX1);
#else
    sourceControl = sourceControl & ~(MSK_MUX_SEL_TX0);
#endif

    sourceControl |= (tx0Source << VAL_MUX_SEL_TX0_SHIFT);

#if (SII_NUM_SWITCH > 1)
    sourceControl |= (tx1Source << VAL_MUX_SEL_TX1_SHIFT);
#endif
    // enable pipe
    sourceControl |= (BIT_TX0_PIPE_EN);


    if ( pDrvSwitch[cInstance]->lastResultCode == SII_SUCCESS )
    {
        // Update database for next call to SiiDrvSwitchOutputEnable()
    	drvCommonData[cInstance].pipeSel[0] = (SiiSwitchHdmiPipe_t)((tx0Source > VAL_MUX_SEL_HDMI_RP) ? SiiSwitchHdmiPipeNone : tx0Source);
#if (SII_NUM_SWITCH > 1)
    	drvCommonData[cInstance].pipeSel[1] = (tx1Source > VAL_MUX_SEL_HDMI_RP) ? SiiSwitchHdmiPipeNone : tx1Source;
#endif
        // Don't change source configuration (not Rx port selection!) if in IP mode
    	if (!portSelectOnly)
    	{
    	    drvSwitchRegisterConf[cInstance].sourceControl = sourceControl;
    	}
    	drvSwitchRegisterConf[cInstance].tmdsPortSelect = tmdsPortSelect;
    }
}


void SiiDrvSwitchZoneConfigGet ( BYTE cInstance, SiiMswInstanceCommonData_t *pZoneInfo )
{
	*pZoneInfo  = drvCommonData[cInstance];
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set the current zone configuration data to the registers to be effective.
//! @remarks    This function is a 'cross-instance' function that affects both instances at once.
//-------------------------------------------------------------------------------------------------
void SiiDrvSwitchZoneConfigSet (  BYTE cInstance )
{
    uint8_t i;
    uint8_t eqDataNM[4] = {0x20, 0x40, 0x43, 0x53};

	// Non-matrix mode
	for (i = 0; i < 4; i++)
	{
		SiiRegWrite( cInstance, REG_A1_EQ_DATA0 + i, eqDataNM[i]);
	}
	SiiRegWrite( cInstance, REG_TMDS1_CNTL, 0x00);

    SiiRegWrite( cInstance,  REG_RX_PORT_SEL, drvSwitchRegisterConf[cInstance].tmdsPortSelect );
	SiiRegWrite( cInstance,  REG_TX_VIDEO_SRC, drvSwitchRegisterConf[cInstance].sourceControl );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Select the requested video source into the current instance.
//  Parameters:
//!
//! @remarks    This function is a 'cross-instance' function that affects both instances
//!             at once if zone tracking mode is enabled.
//-------------------------------------------------------------------------------------------------
SiiDrvSwitchResults_t SiiDrvSwitchSourceSelect ( BYTE cInstance, SiiSwitchSource_t newSource)
{
    pDrvSwitch[cInstance]->lastResultCode = SII_SUCCESS;

    // Set new source for this instance and update the zone muxes and matrix enable if needed.
    // SiiDrvSwitchConfigureMatrixSwitch() must be called to apply changes to hardware
    if ( pDrvSwitch[cInstance]->zoneTracking )
    {
        drvSwitchInstance[cInstance][0].mswVideoSource = newSource;
#if (SII_NUM_SWITCH > 1)
        drvSwitchInstance[cInstance][1].mswVideoSource = newSource;
#endif
    }
    else
    {
        pDrvSwitch[cInstance]->mswVideoSource = newSource;
    }

    return((SiiDrvSwitchResults_t)( pDrvSwitch[cInstance]->lastResultCode ));
}

//-------------------------------------------------------------------------------------------------
//! @brief      Select the requested video source into the current instance.
//!
//! @param[in]  newAudioZoneSource - source for local audio zone (only TMDS sources are acceptable)
//!
//! @remarks    This function is a 'cross-instance' function that affects both instances
//!             at once if zone tracking mode is enabled.
//-------------------------------------------------------------------------------------------------
SiiDrvSwitchResults_t SiiDrvSwitchAudioZoneSourceSet (  BYTE cInstance, SiiSwitchSource_t newAudioZoneSource )
{
    pDrvSwitch[cInstance]->lastResultCode = SII_SUCCESS;

#if SII_DEV_9535
    if ( newAudioZoneSource <= SiiSwitchSource3 )
#else
    if ( newAudioZoneSource <= SiiSwitchSource2 )
#endif
    {

        if ( pDrvSwitch[cInstance]->zoneTracking )
        {
            drvSwitchInstance[cInstance][0].mswAudioZoneSource = newAudioZoneSource;
#if (SII_NUM_SWITCH > 1)
            drvSwitchInstance[cInstance][1].mswAudioZoneSource = newAudioZoneSource;
#endif
        }
        else
        {
            pDrvSwitch[cInstance]->mswAudioZoneSource = newAudioZoneSource;
        }
    }
    else
    {
        pDrvSwitch[cInstance]->lastResultCode = SII_ERR_INVALID_PARAMETER;
    }

    return((SiiDrvSwitchResults_t)( pDrvSwitch[cInstance]->lastResultCode ));
}

//-------------------------------------------------------------------------------------------------
//! @brief      Get the video source selected in the current instance.
//  Parameters:
//!
//-------------------------------------------------------------------------------------------------
SiiSwitchSource_t SiiDrvSwitchSourceGet (  BYTE cInstance )
{
    pDrvSwitch[cInstance]->lastResultCode = SII_SUCCESS;
    return( pDrvSwitch[cInstance]->mswVideoSource );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Get the audio zone source selected in the current instance.
//  Parameters:
//!
//-------------------------------------------------------------------------------------------------
SiiSwitchSource_t SiiDrvSwitchAudioZoneSourceGet (  BYTE cInstance )
{
    pDrvSwitch[cInstance]->lastResultCode = SII_SUCCESS;
    return( pDrvSwitch[cInstance]->mswAudioZoneSource );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns state of matrix enable logic
//  Parameters:
//!
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvSwitchMatrixModeEnableGet (  BYTE cInstance )
{
    return (false);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Select the requested audio source into the current instance.
//  Parameters:
//!
//! @remarks    This function is a 'cross-instance' function that affects both instances
//!             at once if zone tracking mode is enabled.
//-------------------------------------------------------------------------------------------------
SiiDrvSwitchResults_t SiiDrvSwitchAudioSourceSelect ( BYTE cInstance, SiiSwitchAudioSource_t newSource, SiiSwitchSource_t newPort)
{
    uint16_t reg;
    uint8_t pipeSel;
	#if Uart_Debug
	uint8_t a=0;
	#endif

    pDrvSwitch[cInstance]->lastResultCode = SII_SUCCESS;
	 //DEBUG_PRINT( MSG_DBG, "SiiDrvSwitchAudioSourceSelect %x %x %x\n", pDrvSwitch[cInstance]->instanceIndex,newSource,newPort);
    switch(pDrvSwitch[cInstance]->instanceIndex)
    {
        default:
        case 0:
            reg = REG_PACKET_MUX0_CTRL;
            break;
        case 1:
            reg = REG_PACKET_MUX1_CTRL;
            break;
    }

    switch (newSource)
    {
        default:
        case SiiSwitchAudioSourceDefault:
           	pipeSel = BIT_PIPE_SUB_SELECT;
        	SiiRegWrite( cInstance,  REG_PAUTH_RPOVR, 0 );
            break;
        case SiiSwitchAudioSourceMainPipe:

        	//SiiRegWrite( cInstance,  REG_PAUTH_RPOVR, (BIT_FORCE_RP_SELECTION | (BIT_FORCE_RP_PORT << newPort) ) );
        	// Enable Audio Mixing
        	//SiiRegWrite( cInstance,  REG_PAUTH_RP_AOVR, (BIT_FORCE_RP_PORT << newPort) );

            pipeSel = BIT_PIPE_SELECTION_ENABLE;
            break;
        case SiiSwitchAudioSourceSubPipe:
        	//SiiRegWrite( cInstance,  REG_PAUTH_RPOVR, (BIT_FORCE_RP_SELECTION | (BIT_FORCE_RP_PORT << newPort) ) );
        	// Enable Audio Mixing
          //  SiiRegWrite( cInstance,  REG_PAUTH_RP_AOVR, (BIT_FORCE_RP_PORT << newPort) );
#if INC_IPV
        	 if ( !app[cInstance].ipvPipEnabled )
        	 {
        		// SiiRegWrite( cInstance,  REG_IP_CONFIG_47, CLEAR_BITS );
        		  SiiRegModify(REG_IP_CONFIG_47, MSK_ALL_PORTS, 0x01 << newPort);
        		  SiiRegModify( REG_IP_CONFIG_43, BGND_DISABLE, true );
        		  SiiRegBitsSet( REG_INT_ENABLE_P4, BIT_RP_NEW_AVI, true );
        	 }
#endif
        	pipeSel =  BIT_PIPE_SUB_SELECT | BIT_PIPE_SELECTION_ENABLE;
        	break;
    }
    SiiRegModify( cInstance, reg, BIT_PIPE_SELECTION_ENABLE | BIT_PIPE_SUB_SELECT, pipeSel);

	#if Uart_Debug
	UART1_SendBytes("AudioSourceSelect\r\n",strlen("AudioSourceSelect\r\n"));
	a=SiiRegRead ( cInstance,  reg );
	UART1_SendByte(a);
	UART1_SendBytes("\r\n",strlen("\r\n"));
	#endif

    return(SiiDrvSwitchResults_t)(( pDrvSwitch[cInstance]->lastResultCode ));
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchPortsProcessTermCtrl
// Description: sets the termination control appropriately
// Parameters:
// Returns:     void
//------------------------------------------------------------------------------

void SiiDrvSwitchPortsProcessTermCtrl( BYTE cInstance)
{
	uint8_t i;
	for( i = 0; i < SII_INPUT_PORT_COUNT; i++)
	{
		if( (drvSwitchCommon[cInstance].rPwrIntrPorts >> i) & BIT0 )
		{
            SiiDrvRxTermControl( cInstance,  i, true );
		}
	}
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchSelectedPorttype
// Description: returns the currently selected port type for main pipe
// Parameters:
// Returns:     port type: MHL or HDMI
//------------------------------------------------------------------------------

SiiPortType_t SiiDrvSwitchSelectedPorttype( BYTE cInstance)
{
	if( (SiiRegRead( cInstance, REG_RX_PORT_SEL) & MSK_MP_PORT_SEL) != SiiSwitchSource1)
	{
		return ((SiiRegRead( cInstance, REG_MP_STATE) & BIT_MHL)? SiiPortType_MHL:SiiPortType_HDMI);
	}
	else
	{
		return (SiiPlatformMHLCDSensePinGet( cInstance,  SiiSwitchSource1 )? SiiPortType_HDMI:SiiPortType_MHL);
	}
}

//-------------------------------------------------------------------------------------------------
//! @brief      Parse the switch driver interrupts.
//  Parameters:
//!
//-------------------------------------------------------------------------------------------------

void SiiDrvSwitchProcessInterrupts( BYTE cInstance, uint8_t globalIntStatus )
{
    uint8_t     intStatus;
    uint16_t    statusFlags = 0;                // generate a new set every time
    bool_t      mpCableIn;

    if ( globalIntStatus & BIT_INTR_GRP1 )
    {
        // Check plug/unplug interrupt for the main pipe (selected port).
        // Set when the PWR5V signal for the port connected 
        intStatus = SiiRegRead( cInstance,  REG_INT_STATUS_5 );
        if (intStatus )
        {
            drvSwitchCommon[cInstance].rPwrIntrPorts = intStatus & MSK_5VPWR_CHG_ALL_PORTS;
            if (intStatus & BIT_5VPWR_CHG_MP)
            {
             // Main pipe TMDS input port select.
            	if( (SiiRegRead( cInstance, REG_RX_PORT_SEL) & MSK_MP_PORT_SEL) != SiiSwitchSource1)
				{
            		mpCableIn =  (SiiRegRead(  cInstance, REG_MP_STATE ) & ( BIT_PWR5V | BIT_MHL ));
            		//DEBUG_PRINT(  "MP Cable Source %d\n", (SiiRegRead(cInstance,REG_RX_PORT_SEL) & MSK_MP_PORT_SEL) );
				}
				else
				{
					mpCableIn =  (SiiRegRead(cInstance, REG_MP_STATE ) & ( BIT_PWR5V ));
					DEBUG_PRINT( MSG_STAT | DBGF_TS, "MP Cable Source 1: %d\n", mpCableIn );
					mpCableIn |= (SiiRegRead(cInstance,REG_CBUS_BUS_STATUS ) & BIT_BUS_CONNECTED);
					DEBUG_PRINT( MSG_STAT | DBGF_TS, "MP Cable Source 1: %d\n", mpCableIn );
				}

                // Update the main pipe port cable state.
            	if ( mpCableIn)
                {
                    drvSwitchCommon[cInstance].mpCableIn = true;
                    DEBUG_PRINT( MSG_STAT | DBGF_TS, "MP Cable IN\n" );
                }
                else
                {
                    drvSwitchCommon[cInstance].mpCableIn = false;
                    DEBUG_PRINT( MSG_STAT | DBGF_TS, "MP Cable OUT\n" );
                }
            }

            if (intStatus & BIT_5VPWR_CHG_RP)
            {
                // Update the sub pipe port cable state.
                if ( !(SiiRegRead( cInstance,  REG_RP_STATE ) & ( BIT_MHL )))
                {
                    DEBUG_PRINT( MSG_STAT | DBGF_TS, "SP Cable OUT\n" );
                }
                else
                {
                    DEBUG_PRINT( MSG_STAT | DBGF_TS, "SP Cable IN\n" );
                }
            }

            statusFlags |= SiiSWITCH_CABLE_CHANGE;
        }
        SiiRegWrite( cInstance,  REG_INT_STATUS_5, intStatus );
    }

    if ( globalIntStatus & BIT_INTR_GRP_PA )   // PA interrupts
    {
        // Handle CBUS/CDC Main Pipe port HPE requests from pre-authentication hardware.
        // NOTE:    For RP requests, the HPE must be triggered by the firmware as soon as
        //          possible after receiving this interrupt; before the RP switches to another port.
        intStatus = SiiRegRead(  cInstance, REG_INT_STATUS_IP1 ) & (BIT_MP_CEC_HPE | BIT_RP_CEC_HPE);
        if ( intStatus )
        {
            SiiRegWrite( cInstance,  REG_INT_STATUS_IP1, intStatus );
            if ( intStatus & BIT_MP_CEC_HPE)
            {
                DEBUG_PRINT( SW_DBG, "Trigger HPE for MP\n" );
                SiiDrvRxHpeStateSet( cInstance,  SiiPORT_Pipe0, SiiHPD_TOGGLE );
            }

            if ( intStatus & BIT_RP_CEC_HPE)
            {
                DEBUG_PRINT( SW_DBG, "Trigger HPE for RP\n" );
                SiiDrvRxHpeStateSet( cInstance,  SiiPORT_Pipe1, SiiHPD_TOGGLE );
            }
        }

        SwitchInfoframeProcessInterrupts( cInstance );     // Check RX infoframe changes
    }

    // Notify upper layer that a switch module interrupt occurred
    if ( statusFlags )
    {
        pDrvSwitch[cInstance]->statusFlags |= ( statusFlags | SiiSWITCH_INT );
    }

}


//-------------------------------------------------------------------------------------------------
//! @brief      Return measured TMDS clock frequency in Main or Sub pipes
//!
//! @param[in]  isMainPipe - true for main pipe
//! @return     TMDS clock frequency in 10^4 Hz units
//-------------------------------------------------------------------------------------------------

int_t SiiDrvSwitchTmdsClockGet( BYTE cInstance, bool_t isMainPipe )
{
    int32_t xpclk;

    // Read measured TMDS clock frequency
    SiiRegInstanceSet( cInstance,  PP_PAGE_AUDIO, isMainPipe ? 0 : 1 );
    // Freeze TMDS counter before reading its MSB&LSB portions to avoid loss of precision
    SiiRegBitsSet( cInstance, REG_XPCLK_ENABLE, BIT_XPCLK_ENABLE, CLEAR_BITS);
    xpclk = SiiRegRead( cInstance, REG_XPCLK_CNT_0) | (SiiRegRead( cInstance, REG_XPCLK_CNT_1) << 8);
    // Enable the TMDS counter counter back again
    SiiRegBitsSet( cInstance, REG_XPCLK_ENABLE, BIT_XPCLK_ENABLE, SET_BITS);
    return (2700 * 2048 / xpclk);
}
