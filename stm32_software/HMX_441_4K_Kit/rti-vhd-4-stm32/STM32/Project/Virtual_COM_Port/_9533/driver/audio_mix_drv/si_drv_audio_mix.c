//***************************************************************************
//! @file     si_drv_audio_mix.c
//! @brief    Audio Mixer driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_drv_device.h"
#include "si_regs_audio_mix953x.h"
#include "si_drv_internal.h"
#include "si_drv_audio_mix.h"
#include "si_regs_tpi953x.h"

#include "main.h"
#include "Mcu.h"
#include "uart.h"

#define SII_NUM_TX  1

//-------------------------------------------------------------------------------------------------
//! @brief      Select Tx device instance in multi-transmitter system.
//!
//! @param[in]  device_idx - zero based index of an instance.
//-------------------------------------------------------------------------------------------------

void SiiDrvRxAudioMixInstanceSet( BYTE cInstance, uint8_t deviceIndex)
{
#if SII_NUM_TX > 1
    if(deviceIndex < SII_NUM_TX)
    {
        SiiRegInstanceSet( cInstance , PP_PAGE_AUDIO_MIXER, deviceIndex );
    }
#endif
}


//-------------------------------------------------------------------------------------------------
//! @brief      Set pass through mode.
//!
//! @param[in]  isAudioPassEnabled     - Audio packets will pass through, if true
//! @param[in]  isNonAudioPassEnabled  - Non-audio packets will pass through, if true
//                                                         - spec: HDMI Packet (excludes audio packet) pass through enable
//-------------------------------------------------------------------------------------------------

void SiiDrvRxAudioMixPassThroughConfig( BYTE cInstance,bool_t isAudioPassEnabled, bool_t isNonAudioPassEnabled)
{
	//SiiRegModify( REG_TPI__AUDIO_CFG, (BIT_TPI__AUDIO_CFG__MUTE | VAL_TPI__AUDIO_CFG__SPDIF | VAL_TPI__AUDIO_CFG__I2S), BIT_TPI__AUDIO_CFG__MUTE);
    SiiRegModify( cInstance , REG_TX_AUD_MIX_CTRL1, BIT_TX_AUD_MIX_PASS_AUD_PKT |
                  BIT_TX_AUD_MIX_PASS_NAUD_PKTS | BIT_TX_AUD_MIX_DROP_GEN1,
                    (isAudioPassEnabled ? BIT_TX_AUD_MIX_PASS_AUD_PKT : 0) |
                    (isNonAudioPassEnabled ? BIT_TX_AUD_MIX_PASS_NAUD_PKTS : 0) |
                    /* SWWA: 23624 Always drop NULL packets. They may get in the way of mixer operation */
                    BIT_TX_AUD_MIX_DROP_GEN1);

    SiiRegModify(  cInstance ,REG_TX_AUD_MIX_CTRL0, BIT_TX_AUD_MIX_DROP_AIF | BIT_TX_AUD_MIX_DROP_SPDIF | BIT_TX_AUD_MIX_DROP_CTS,
                    (isAudioPassEnabled ? 0: (BIT_TX_AUD_MIX_DROP_AIF | BIT_TX_AUD_MIX_DROP_SPDIF | BIT_TX_AUD_MIX_DROP_CTS)) );

    SiiRegModify(  cInstance ,REG_TX_AUD_MIX_CTRL3, BIT_TX_AUD_MIX_AUD_TYPE_OVR, (isAudioPassEnabled ? 0: BIT_TX_AUD_MIX_AUD_TYPE_OVR));
}

#if 0
void SiiDrvRxDigitalAudioPwoffStatue(BYTE cInstance)
{
	if(cInstance==6)
	{
		if(digit_audio[cInstance-6]==0)	//¹Ø±ÕÉùÒô
		{
			SiiDrvRxAudioMixPassThroughConfig(cInstance,false, true);
		}
		else
		{
			SiiDrvRxAudioMixPassThroughConfig( cInstance ,true, true);
		}
	}
	else if(cInstance==7)
	{
		if(digit_audio[cInstance-6]==0)	//¹Ø±ÕÉùÒô
		{
			SiiDrvRxAudioMixPassThroughConfig(cInstance,false, true);
		}
		else
		{
			SiiDrvRxAudioMixPassThroughConfig(cInstance,true, true);
		}
	}
	else if(cInstance==8)
	{
		if(digit_audio[cInstance-6]==0)	//¹Ø±ÕÉùÒô
		{
			SiiDrvRxAudioMixPassThroughConfig(cInstance,false, true);
		}
		else
		{
			SiiDrvRxAudioMixPassThroughConfig(cInstance,true, true);
		}
	}
	else if(cInstance==9)
	{
		if(digit_audio[ cInstance -6]==0)	//¹Ø±ÕÉùÒô
		{
			SiiDrvRxAudioMixPassThroughConfig( cInstance, false, true);
		}
		else
		{
			SiiDrvRxAudioMixPassThroughConfig( cInstance, true, true);
		}
	}
	else if( cInstance ==4)
	{
		if(digit_audio[ cInstance -4]==0)	//¹Ø±ÕÉùÒô
		{
			SiiDrvRxAudioMixPassThroughConfig( cInstance, false, true);
		}
		else
		{
			SiiDrvRxAudioMixPassThroughConfig( cInstance, true, true);
		}
	}
	else if( cInstance ==5)
	{
		if(digit_audio[ cInstance -4]==0)	//¹Ø±ÕÉùÒô
		{
			SiiDrvRxAudioMixPassThroughConfig( cInstance, false, true);
		}
		else
		{
			SiiDrvRxAudioMixPassThroughConfig( cInstance, true, true);
		}
	}
	else
	{
		 SiiDrvRxAudioMixPassThroughConfig( cInstance, true, true);
	}
}
#endif
// void SiiDrvRxDigitalAudioPwoffStatue(uint8_t DeviceID)
// {
// 	if(DeviceID==8)
// 	{
// 		if(digit_audio[DeviceID-8]==0)	//¹Ø±ÕÉùÒô
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(false, true);
// 		}
// 		else
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(true, true);
// 		}
// 	}
// 	else if(DeviceID==9)
// 	{
// 		if(digit_audio[DeviceID-8]==0)	//¹Ø±ÕÉùÒô
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(false, true);
// 		}
// 		else
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(true, true);
// 		}
// 	}
// 	else if(DeviceID==10)
// 	{
// 		if(digit_audio[DeviceID-8]==0)	//¹Ø±ÕÉùÒô
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(false, true);
// 		}
// 		else
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(true, true);
// 		}
// 	}
// 	else if(DeviceID==11)
// 	{
// 		if(digit_audio[DeviceID-8]==0)	//¹Ø±ÕÉùÒô
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(false, true);
// 		}
// 		else
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(true, true);
// 		}
// 	}
// 	else if(DeviceID==12)
// 	{
// 		if(digit_audio[DeviceID-12]==0)	//¹Ø±ÕÉùÒô
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(false, true);
// 		}
// 		else
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(true, true);
// 		}
// 	}
// 	else if(DeviceID==13)
// 	{
// 		if(digit_audio[DeviceID-12]==0)	//¹Ø±ÕÉùÒô
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(false, true);
// 		}
// 		else
// 		{
// 			SiiDrvRxAudioMixPassThroughConfig(true, true);
// 		}
// 	}
// 	else
// 	{
// 		 SiiDrvRxAudioMixPassThroughConfig(true, true);
// 	}
// }

//-------------------------------------------------------------------------------------------------
//! @brief      drop GCP packet.
//!
//! @param[in]
//-------------------------------------------------------------------------------------------------

void SiiDrvRxAudioMixGcpPassThroughConfig( BYTE cInstance, bool_t qOn)
{
    SiiRegModify(  cInstance ,REG_TX_AUD_MIX_CTRL0, BIT_TX_AUD_MIX_DROP_GCP,
                    (qOn ? 0: BIT_TX_AUD_MIX_DROP_GCP) );
}


