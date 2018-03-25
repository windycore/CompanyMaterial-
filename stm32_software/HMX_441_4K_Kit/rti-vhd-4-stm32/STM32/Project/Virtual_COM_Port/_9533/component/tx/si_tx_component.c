//************************************************************************************************
//! @file   si_tx_component.c
//! @brief  Transmitter software component
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//************************************************************************************************/

#include <string.h>
#include "si_tx_component_internal.h"
#include "si_drv_tpi_access.h"
#include "si_drv_tpi_system.h"
#include "si_tx_video.h"
#include "si_tx_audio.h"
#include "si_tx_avi.h"
#include "si_tx_info_frm.h"
#include "si_tx_hdcp.h"
#include "si_tx_component.h"

#include "main.h"
#include "Mcu.h"
#include "uart.h"
#include "Sk_application.h"
#include "Si_osal_timer.h"

//-------------------------------------------------------------------------------------------------
//  Component Instance Data
//-------------------------------------------------------------------------------------------------

txInstanceData_t txInstRec[INSTANCE_9533][SII_NUM_TX];
txInstanceData_t *pTx[INSTANCE_9533] = 
{
    &txInstRec[0][0],
#if(INSTANCE_9533 > 1 )
    &txInstRec[1][0],
#endif
#if(INSTANCE_9533 > 2 )
    &txInstRec[2][0],
#endif
#if(INSTANCE_9533 > 3 )
    &txInstRec[3][0],
#endif
#if(INSTANCE_9533 > 4 )
    &txInstRec[4][0],
#endif
#if(INSTANCE_9533 > 5 )
    &txInstRec[5][0],
#endif
#if(INSTANCE_9533 > 6 )
    &txInstRec[6][0],
#endif
#if(INSTANCE_9533 > 7 )
    &txInstRec[7][0],
#endif

};


//-------------------------------------------------------------------------------------------------
//  Local service functions
//-------------------------------------------------------------------------------------------------

static const char * ErrorMessageGet(txErrCode_t error)
{
    static const char * aMessage[SI_TX_TOTAL_NUM_OF_ERROR_CODES] = {
            "",
            "instance doesn't exist",
            "invalid parameter",
            "incompatible power mode",
            "HDMI not supported by DS",
            "invalid video source",
            "",
            "same power mode requested"
    };

    if (error < SI_TX_TOTAL_NUM_OF_ERROR_CODES)
    {
        return aMessage[error];
    }
    else
    {
        return aMessage[0];
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Update last result structure and print an error message.
//!
//!             This function shall be used to report any error or warning in the component.
//!
//! @param[in]  code  - error or warning message code
//-------------------------------------------------------------------------------------------------

static void ReportIssue( BYTE cInstance,txErrCode_t code)
{
    if (code == SI_TX_ERROR_CODE_NO_ERROR)
    {
        pTx[cInstance]->lastResult.lastErrorCode = code;
    }
    else
    {
        if (code < SI_TX_WARN_CODE_NO_WARNING)
        {
            pTx[cInstance]->lastResult.lastErrorCode = code;
            pTx[cInstance]->lastResult.errorCount++;

            DEBUG_PRINT(TX_MSG_ERR, "TX last error: %s. Total: %d\n", ErrorMessageGet(code), pTx[cInstance]->lastResult.errorCount);
        }
        else
        {
            DEBUG_PRINT(TX_MSG_DBG, "TX Warning: %s\n", ErrorMessageGet(code));
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Clear previous error code.
//!
//!             This function should be used as a default success reporter for the functions
//!             where ReportIssue() is used.
//-------------------------------------------------------------------------------------------------

static void ClearErrorStatus( BYTE cInstance)
{
    ReportIssue( cInstance ,SI_TX_ERROR_CODE_NO_ERROR);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Attempt to Enable or Disable TMDS output signals.
//!
//!             When asked to enable TMDS, this function either does it immediately or schedules
//!             enabling for later, if off time was too short. TMDS disabling is fulfilled
//!             always immediately.
//!
//! @param[in]  isEnabled - true enables TMDS, false disables TMDS
//-------------------------------------------------------------------------------------------------
//   TX 的TMDS 需要打开或者关闭
//
static void RequestTmdsOutputEnable( BYTE cInstance, bool_t isEnabled)
{
    if (isEnabled)
    {
       // 当上次关掉TMDS 到现在要打开的之间时间还不够
        if(pTx[cInstance]->isTmdsBlackoutPauseRequired || pTx[cInstance]->timeToTmdsEnable)
        {
            // too early to enable TMDS; schedule enabling for later
            pTx[cInstance]->isDelayedTmdsEnableRequested = true;
        }
        else
        {
        //不延时,马上打开
            if (!pTx[cInstance]->isTmdsEnabled)
            {
                SiiDrvTpiTmdsOutputEnable( cInstance, true);
                pTx[cInstance]->isTmdsEnabled = true;

                DEBUG_PRINT(TX_MSG_DBG, "TX TMDS Enabled\n");
                TxHdcpOnOutputEnable( cInstance );

                // re-start info frame and info packets that came before powering up
                //TxAllInfoFramesUpdate();
            }
        }
    }
    else
    {
        if (pTx[cInstance]->isTmdsEnabled)
        {
            SiiDrvTpiTmdsOutputEnable( cInstance, false);
            pTx[cInstance]->isTmdsEnabled = false;

            DEBUG_PRINT(TX_MSG_DBG, "TX TMDS Disabled\n");
        }

        pTx[cInstance]->isTmdsBlackoutPauseRequired = true;
        pTx[cInstance]->timeToTmdsEnable = SI_TX_MIN_OFF_TIME_MS;
        pTx[cInstance]->isDelayedTmdsEnableRequested = false;   //timeToTmdsEnable 时间到不会打开
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Local timer handler implementing delayed TMDS enabling.
//!
//! @param[in]  timeDelta - time in ms elapsed since the last call.
//-------------------------------------------------------------------------------------------------
//    没有消息，没中断隔一段时间执行SI_TX_TIME_STEP_MIN_MS
// -------------> SiiTxTaskProcess()
static void TmdsTimerHandler( BYTE cInstance, time_ms_t delta)
{
    
    if(pTx[cInstance]->isTmdsBlackoutPauseRequired)
    {
        // Just skip one serving.
        // It'll make sure the count down happens at appropriate time.
        pTx[cInstance]->isTmdsBlackoutPauseRequired = false;
    }
    else
    {
       // 比较时间时候超过, 可能要多次循环才能满足条件
       // 意思好像是: 关掉TMDS 后到下一次打开需要得的时间
        if(IS_UPDATED_TIMER_EXPIRED(pTx[cInstance]->timeToTmdsEnable, delta))
        {
        //  delta >  timeToTmdsEnable
        
           // if(pTx[cInstance]->isDelayedTmdsEnableRequested)
            //DEBUG_PRINT( "TmdsTimerHandler()%d\n", pTx[cInstance]->isDelayedTmdsEnableRequested);
            if(pTx[cInstance]->isDelayedTmdsEnableRequested)
            {
                pTx[cInstance]->isDelayedTmdsEnableRequested = false;   //  阻止后面再进入执行
                SiiDrvTpiTmdsOutputEnable( cInstance, true);
                pTx[cInstance]->isTmdsEnabled = true;

                DEBUG_PRINT(TX_MSG_DBG, "TX TMDS Enabled\n");
                //TxTmdsBlankPause(cInstance);//[kyq add for test: 20150828 13:49:15]
                //SiiPlatformTimerWait(500); //[kyq test: 20150828 17:08:39]
                TxHdcpOnOutputEnable( cInstance );

//[kyq Begin: 20150907 14:07:44]
#if 0
                if(!app[cInstance].isTxHdcpRequired[0])
                {
                 SiiDrvTpiHdcpProtectionEnable(cInstance,true);
		   SiiOsTimerWait(1); // also makes time aligning
		   SiiDrvTpiHdcpProtectionEnable(cInstance,false);
                }
#endif
//[kyq End: 20150907 14:07:45]


				
            }
        }
    }

}

//-------------------------------------------------------------------------------------------------
//! @brief      Set parameters of DE generator in order to recover synchronization signals
//!             embedded into the video data, if any.
//!
//! @param[in]  syncSrc - input HV Sync and DE signals descriptor,
//! @param[in]  pSynRg  - pointer to a structure defining the regeneration options.
//!                       If equal to NULL, the default DE settings will be applied.
//-------------------------------------------------------------------------------------------------

static void SyncSignalsRegenerationSet( BYTE cInstance, syncSignals_t syncSrc, syncRegen_t *pSynRg)
{
    // Default values of synchronization signals regeneration structure
    syncRegen_t DefaultSynRg =
    {
            0,
            true,
            false,
            0,
            0,
            0,
            0,
            0,
            false,
            false,
            0,
            0,
            0,
            0
    };

    if(pSynRg == NULL)
    {
        pSynRg = &DefaultSynRg;
    }

    switch (syncSrc)
    {
        default:
        case SI_TX_VIDEO_HVSYNC_DE:
            // Neither generate DE nor extract HV syncs
            SiiDrvTpiDeGenerationEnable( cInstance , false, NULL);
            pTx[cInstance]->isDeGeneratorEnabled = false;

            SiiDrvTpiEmbSyncExtractionEnable( cInstance , false, NULL);
            pTx[cInstance]->isEmbSyncExtractEnabled = false;
            break;

        case SI_TX_VIDEO_DE:
            // Extract HV syncs and not generate DE
            SiiDrvTpiDeGenerationEnable( cInstance , false, NULL);
            pTx[cInstance]->isDeGeneratorEnabled = false;

            SiiDrvTpiEmbSyncExtractionEnable( cInstance , true, pSynRg);
            pTx[cInstance]->isEmbSyncExtractEnabled = true;
            break;

        case SI_TX_VIDEO_HVSYNC:
            // Generate DE and not extract HV syncs
            SiiDrvTpiEmbSyncExtractionEnable( cInstance ,false, NULL);
            pTx[cInstance]->isEmbSyncExtractEnabled = false;

            SiiDrvTpiDeGenerationEnable( cInstance , true, pSynRg);
            pTx[cInstance]->isDeGeneratorEnabled = true;
            break;

        case SI_TX_VIDEO_NONE:
            // Generate DE and extract HV syncs
            SiiDrvTpiEmbSyncExtractionEnable( cInstance ,true, pSynRg);
            pTx[cInstance]->isEmbSyncExtractEnabled = true;

            SiiDrvTpiDeGenerationEnable( cInstance ,true, pSynRg);
            pTx[cInstance]->isDeGeneratorEnabled = true;
            break;
    }

    SiiDrvTpiSyncRegenConfigurationSet( cInstance ,pSynRg);
}


//-------------------------------------------------------------------------------------------------
//  TX instance data access functions and methods for internal use by the component modules
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Return TMDS status.
//-------------------------------------------------------------------------------------------------

bool_t TxIsTmdsEnabled( BYTE cInstance)
{
    return pTx[cInstance]->isTmdsEnabled;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return true if transmitter is powered up and the output is in HDMI mode.
//-------------------------------------------------------------------------------------------------

bool_t TxIsRunningInHdmiMode( BYTE cInstance)
{
    return (pTx[cInstance]->powerMode == SI_TX_POWER_MODE_FULL) && (pTx[cInstance]->isTxOutInHdmiMode);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return true if transmitter is powered up.
//-------------------------------------------------------------------------------------------------

bool_t TxIsRunning( BYTE cInstance)
{
    return (pTx[cInstance]->powerMode == SI_TX_POWER_MODE_FULL);
}

//[kyq Begin: 20151016 15:22:08]
txPowerMode_t TXPowerModeGet(BYTE cInstance)
{
    return pTx[cInstance]->powerMode;
}
//[kyq End: 20151016 15:22:10]

//-------------------------------------------------------------------------------------------------
//! @brief      Return video format ID.
//-------------------------------------------------------------------------------------------------

uint8_t TxVideoFormatIdGet( BYTE cInstance)
{
    return pTx[cInstance]->videoFormatId;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return pixel clock frequency in MHz.
//-------------------------------------------------------------------------------------------------

uint16_t TxPixelFreqMhzGet( BYTE cInstance)
{
    return pTx[cInstance]->pixelFrequency / 100;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return true if output is in HDMI mode.
//-------------------------------------------------------------------------------------------------

bool_t TxIsOutInHdmiMode( BYTE cInstance)
{
    return pTx[cInstance]->isTxOutInHdmiMode;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Temporal disabling of TMDS output for a short period of time.
//!
//!             If TMDS is ON, turn it OFF for at least 100ms.
//!             After the timeout, the output will be enabled.
//!             This function is used in order to reset DS HDCP engine.
//!             Note: the function is non-blocking.
//-------------------------------------------------------------------------------------------------

void TxTmdsBlankPause( BYTE cInstance)
{
    if(pTx[cInstance]->isTmdsEnabled)
    {
        // If TMDS is ON, temporary turn it off.
        RequestTmdsOutputEnable( cInstance , false);

        // Enabling TMDS will be delayed by at least 100ms
        RequestTmdsOutputEnable( cInstance , true);
    }
}



//-------------------------------------------------------------------------------------------------
//  TX API functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Initialization of the HDMI transmitter
//!
//!             Initializes Tx module and sets default parameters of the component.
//!             This function shall be called once and foremost.
//-------------------------------------------------------------------------------------------------

void SiiTxInitialize(BYTE cInstance)
{
    // Initialize the internal data structures
    memset(pTx[cInstance], 0x0, sizeof(txInstanceData_t));
    pTx[cInstance]->videoFormatId = SI_VIDEO_MODE_NON_STD;

    // Software reset and initialization of Tx to bring it to a known initial state
    SiiDrvTpiAccessInit( cInstance );
    SiiDrvTpiSwReset( cInstance );
    SiiDrvTpiInit( cInstance );

    SiiDrvTpiInterruptsEnable( cInstance, true);

    // Downstream device is considered disconnected by default
    pTx[cInstance]->status.isDsConnected = false;

#if (SI_TX_INIT_TO_STANDBY == SI_ENABLE)
    SiiDrvTpiTmdsOutputEnable( cInstance ,false);
    SiiDrvTpiStandby( cInstance );
    pTx[cInstance]->powerMode = SI_TX_POWER_MODE_STANDBY;
#else
    SiiDrvTpiPowerUp( cInstance );
    pTx[cInstance]->powerMode = SI_TX_POWER_MODE_FULL;
#endif

    TxInfoInit( cInstance );
    TxAviInit( cInstance );

    SiiTxAvUnmute( cInstance );

    TxHdcpInit( cInstance );

#if (SI_TX_VIDEO_CONVERSION == SI_ENABLE)
    TxVideoConversionInit( cInstance );
#endif

    TxAudioInit( cInstance  );

    // Retrieve revision information
    pTx[cInstance]->revision.deviceId  = SiiDrvTpiDeviceIdGet( cInstance );
    pTx[cInstance]->revision.tpi       = SiiDrvTpiRevisionGet( cInstance );
    pTx[cInstance]->revision.component = SI_TX_COMP_REVISION;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Returns index of an instance selected in context
//!
//! @return     Index of an instance record
//-------------------------------------------------------------------------------------------------

uint8_t SiiTxInstanceGet(BYTE cInstance)
{
    return (pTx[cInstance] - &txInstRec[cInstance][0]);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Brings an instance referred by given index into current context.
//!
//! @param[in]  instance - index of an instance record
//-------------------------------------------------------------------------------------------------

void SiiTxInstanceSet(BYTE cInstance,uint8_t instance)
{
	if (instance < SII_NUM_TX)
      {
		pTx[cInstance]=&txInstRec[cInstance][instance];
	}
#if (SII_NUM_TX > 1)
    if (instance < SII_NUM_TX)
    {
        pTx[cInstance] = &txInstRec[cInstance][instance];

        // Switch context in submodules
        SiiDrvTpiInstanceSet(instance);
        TxHdcpInstanceSet(instance);
        TxInfoInstanceSet(instance);

#if (SI_TX_VIDEO_CONVERSION == SI_ENABLE)
        TxVideoConversionInstanceSet(instance);
#endif

        TxAviInstanceSet(instance);
        TxAudioInstanceSet(instance);
    }
    else
    {
        ReportIssue(cInstance, SI_TX_ERROR_CODE_INSTANCE_NOT_EXIST);
    }
#endif
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return current TX status flags such as HPD and RxSense.
//!
//! @return     status structure.
//-------------------------------------------------------------------------------------------------

txStatus_t SiiTxStatusGet(BYTE cInstance)
{
    uint8_t interrupts = SiiDrvTpiPendingInterruptsGet( cInstance );
    uint8_t status = SiiDrvTpiPlugStatusGet( cInstance );

    pTx[cInstance]->status.isHotPlugDetected = ((status & SI_TX_STATUS_MASK_HPD) != 0);
    pTx[cInstance]->status.isRxSensed = ((status & SI_TX_STATUS_MASK_RX_SENSE) != 0);

    pTx[cInstance]->status.isIrqPending = (interrupts != 0);

    return pTx[cInstance]->status;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return error code structure.
//!
//!             Can be used to check if the last operation was a success or a failure.
//!             Error status is cleared after the call.
//!
//! @return     structure having the lastly caught error code and total number of errors
//-------------------------------------------------------------------------------------------------

txError_t SiiTxGetLastResult(BYTE cInstance)
{
    txError_t retErr = pTx[cInstance]->lastResult;

    // Clear error status
    pTx[cInstance]->lastResult.lastErrorCode = SI_TX_ERROR_CODE_NO_ERROR;
    pTx[cInstance]->lastResult.errorCount = 0;

    return retErr;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return an identification information for the driver.
//!
//! @return     structure containing revision of Tx component, TPI interface and Tx Device ID
//-------------------------------------------------------------------------------------------------

txRevision_t SiiTxRevisionGet(BYTE cInstance)
{
    return pTx[cInstance]->revision;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Sets DVI or HDMI mode of the output interface.
//!
//!             The output interface must be set during Hot Plug service loop before switching
//!             to the the full power mode.
//!
//! @param[in]  mode - output mode code
//-------------------------------------------------------------------------------------------------

void SiiTxOutputInterfaceSet(BYTE cInstance, txHdmiMode_t mode)
{
    pTx[cInstance]->isTxOutInHdmiMode = false; // default is DVI

    ClearErrorStatus(cInstance);

    switch(mode)
    {
        default:
        case SI_TX_OUTPUT_MODE_DVI:
            // Any downstream device must support DVI mode. No check is needed.
            SiiDrvTpiHdmiOutputModeSet( cInstance, false);
            TxAudioOnHdmiModeChange( cInstance );
            break;

        case SI_TX_OUTPUT_MODE_HDMI:
            // Verify if the downstream device supports HDMI using the EDID info
            if(SiiEdidTxDoesDsSupport( cInstance, SI_EDID_TX_SUPPORT_HDMI))//原版的
            //if(1)
            {
                // DS is an HDMI device
                SiiDrvTpiHdmiOutputModeSet( cInstance, true);
                pTx[cInstance]->isTxOutInHdmiMode = true;

                TxAviInfoFrameUpdate( cInstance );
                TxAudioOnHdmiModeChange( cInstance );
				#if Uart_Debug
				UART1_SendBytes("HDMI MODE\r\n",strlen("HDMI MODE\r\n"));
				#endif
            }
            else
            {
                // DS is a DVI only device
                // so we fail to set HDMI output type
                ReportIssue(cInstance, SI_TX_ERROR_CODE_HDMI_NOT_SUPPORTED_BY_DS);
            }
    }

}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns output interface type (DVI or HDMI).
//!
//! @return     output mode code
//-------------------------------------------------------------------------------------------------

txHdmiMode_t SiiTxOutputInterfaceGet( BYTE cInstance)
{
    return (txHdmiMode_t)(pTx[cInstance]->isTxOutInHdmiMode);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Switches TX into Standby low-power mode.
//!
//!             Brings transmitter into low power mode (D2) when TMDS output is disabled,
//!             but register access is enabled.
//-------------------------------------------------------------------------------------------------

void SiiTxStandby(BYTE cInstance)
{
    if (pTx[cInstance]->powerMode == SI_TX_POWER_MODE_STANDBY)
    {
        ReportIssue(cInstance, SI_TX_WARN_CODE_POWER_MODE_ALREADY_SET);
    }
    else if (pTx[cInstance]->powerMode != SI_TX_POWER_MODE_FULL)
    {
        ReportIssue( cInstance, SI_TX_ERROR_CODE_INCOMPATIBLE_POWER_MODE);
        return;
    }
    //SiiDrvTpiDdcMasterReset(cInstance); //[kyq Begin: 20151009 15:56:59], NO USE
    // Switch to Standby mode
    RequestTmdsOutputEnable( cInstance, false);
    TxHdcpReset( cInstance );
    SiiDrvTpiStandby( cInstance );
    pTx[cInstance]->powerMode = SI_TX_POWER_MODE_STANDBY;
    DEBUG_PRINT(TX_MSG_ALWAYS, "TX Standby Mode\n");   
    ClearErrorStatus( cInstance ); // success
}

//-------------------------------------------------------------------------------------------------
//! @brief      Switches TX to normal power mode.
//!
//!             Brings transmitter into normal power mode (D0) when all parts
//!             of Tx chip are enabled, including TMDS output.
//-------------------------------------------------------------------------------------------------

void SiiTxResume(BYTE cInstance)
{
    DEBUG_PRINT(TX_MSG_ALWAYS, "SiiTxResume\n");
    if (pTx[cInstance]->powerMode == SI_TX_POWER_MODE_FULL)
    {
        ReportIssue(cInstance, SI_TX_WARN_CODE_POWER_MODE_ALREADY_SET);
    }
    else if (pTx[cInstance]->powerMode != SI_TX_POWER_MODE_STANDBY)
    {
        ReportIssue(cInstance,SI_TX_ERROR_CODE_INCOMPATIBLE_POWER_MODE);
        return;
    }

    // Resume Tx
    SiiDrvTpiPowerUp( cInstance );
    pTx[cInstance]->powerMode = SI_TX_POWER_MODE_FULL;
    DEBUG_PRINT(TX_MSG_ALWAYS, "Full Power Mode\n");

    // Make sure there is a delay between powering up the chip and enabling TMDS
    if (pTx[cInstance]->timeToTmdsEnable < SI_TX_PWRUP_TO_TMDS_TIME_MS)
    {
        pTx[cInstance]->timeToTmdsEnable = SI_TX_PWRUP_TO_TMDS_TIME_MS;
        pTx[cInstance]->isTmdsBlackoutPauseRequired = true;
    }

    // Enable interrupts (if previously disabled)
    SiiDrvTpiInterruptsEnable( cInstance, true);
    RequestTmdsOutputEnable( cInstance ,true);
    ClearErrorStatus( cInstance ); // success
}


//-------------------------------------------------------------------------------------------------
//! @brief      Switches TX to power down mode.
//!
//!             Brings transmitter into lowest power mode (D3) when all parts
//!             of Tx chip are disabled. Hardware reset is required to return from this state
//!             as the register access is disabled as well.
//-------------------------------------------------------------------------------------------------
void SiiTxShutDown(BYTE cInstance)
{
    if ((pTx[cInstance]->powerMode == SI_TX_POWER_MODE_FULL) || (pTx[cInstance]->powerMode == SI_TX_POWER_MODE_STANDBY))
    {
        RequestTmdsOutputEnable( cInstance, false);

        // Disable interrupts
        //SiiDrvTpiInterruptsEnable( cInstance, false);

        TxHdcpReset( cInstance );

        SiiDrvTpiPowerDown( cInstance );
        pTx[cInstance]->powerMode = SI_TX_POWER_MODE_OFF;
        DEBUG_PRINT(TX_MSG_ALWAYS, "Power Down Mode\n");

        ClearErrorStatus( cInstance ); // success
    }
    else
    {
        ReportIssue( cInstance, SI_TX_WARN_CODE_POWER_MODE_ALREADY_SET);
    }

}

//-------------------------------------------------------------------------------------------------
//! @brief      Handle pending TX interrupts and run background sub-tasks.
//!
//! @param[in]  elapsedTimeMs - time in milliseconds elapsed since the last call of this function.
//!
//! @return     A message to the application layer.
//! @retval     value - meaning
//-------------------------------------------------------------------------------------------------
// //[kyq Begin: 20140918 12:36:31]  TO DO
//           SiiTxStatusGet(cInstance).isIrqPending/or time loop per 20ms
//             |
//            \|/
//   ------------->SkAppTaskTx()--->dvSii9533Process()
static BOOL m_acHPDLosted[INSTANCE_9533]=
{
   TRUE,
#if(INSTANCE_9533>1)
   TRUE,
#endif
#if(INSTANCE_9533>2)
   TRUE,
#endif
#if(INSTANCE_9533>3)
   TRUE,
#endif
#if(INSTANCE_9533>4)
   TRUE,
#endif
#if(INSTANCE_9533>5)
   TRUE,
#endif
#if(INSTANCE_9533>6)
   TRUE,
#endif

#if(INSTANCE_9533>7)
   TRUE,
#endif
};

//  有中断马上执行
//  没中断隔一段时间执行SI_TX_TIME_STEP_MIN_MS
//       TX 中断的HDCP 处理
//       TX 中断的HOT PLUG 状态处理,返回msg
// 清除TX txInterruptWord 标记
//
//elapsedTimeMs:  上一次和这一次执行函数之间的时间间隔
//                       所以这个时间差一半会比SI_TX_TIME_STEP_MIN_MS 多一点
//                       在有中断时候可能小很多
txFeedbackMsg_t SiiTxTaskProcess( BYTE cInstance, time_ms_t elapsedTimeMs)
{
    txFeedbackMsg_t fbMsg = SI_TX_FB_MSG_NONE;
    uint16_t txInterruptWord;

    txInterruptWord = SiiDrvTpiPendingInterruptsGet( cInstance );// 变量pTpi中读取
	
	//UART1_SendByte((uint8_t)(txInterruptWord>>8));
	//UART1_SendByte((uint8_t)txInterruptWord);

    // Service a request to wake up from power down state
    if(SiiDrvTpiIsWakeUpRequired( cInstance ))
    {
		
	if (!SiiDrvTpiIsPoweredUp( cInstance ))
        {
            // Power up TX first to enable IRQ decoding and the HPD debouncing logic
            SiiDrvTpiPowerUp( cInstance );
        }
	// D0 状态下有HPD 中断,进入StandBY
        else if (txInterruptWord & SI_TX_IRQ__HPD)
        {
            // Once debounced HPD interrupt captured, fall in Standby again
            // Application will decide when and if to Resume the TX upon new HPD arrival
			SiiDrvTpiStandby( cInstance );
            SiiDrvTpiWakeUpRequestClear( cInstance );
        }
    }

    // Process interrupts, if pending
    if (txInterruptWord && TxIsRunning( cInstance ))
    {
        DEBUG_PRINT(TX_MSG_DBG, "TX Ints: 0x%04X\n", (int) txInterruptWord);
        TxHdcpInterruptHandler(cInstance,txInterruptWord);

#if (CHECK_FOR_P_STABLE == SI_ENABLE)
        if(txInterrupts & SI_TX_IRQ__P_STABLE)
        {
            bool_t p_stable = SiiDrvTpiIsHdmiLinkStable(cInstance);
            bool_t pdo = SiiDrvTpiIsTmdsClockStable(cInstance);

            DEBUG_PRINT(MSG_DBG_TX, ("TX: p_stable = %c, pdo = %c\n", p_stable ? 'Y' : 'N', pdo ? 'Y' : 'N'));

            if(p_stable)
            {
                SiiTxRecoverAfterSourceClockInterruption(cInstance);
            }
        }
#endif // CHECK_FOR_P_STABLE
    }


    // If Downstream Plug interrupt event has occurred (HPD or/and RSEN change)
    // or if TX is in one of the transient states: RE-CONNECTING or CONNECTED
    if ((txInterruptWord & (SI_TX_IRQ__HPD | SI_TX_IRQ__RX_SENSE)) ||
        (pTx[cInstance]->status.isSinkReady && (!pTx[cInstance]->status.isDsConnected)) ||
        ((!pTx[cInstance]->status.isSinkReady) && pTx[cInstance]->status.isDsConnected))
    {
        txStatus_t txStat = SiiTxStatusGet( cInstance );// 同时更新 pTx[cInstance]->status

        if (pTx[cInstance]->status.isSinkReady)
        {
            if (pTx[cInstance]->status.isDsConnected)
            {
                // SINK READY State
                if (!txStat.isRxSensed)
                {
                    // If RSEN is lost, the DS must be disconnected
                    fbMsg = SI_TX_FB_MSG_HOT_PLUG_DISCONNECTED;
                    pTx[cInstance]->status.isSinkReady = false;
                    pTx[cInstance]->status.isDsConnected = false;
                    DEBUG_PRINT( TX_MSG_DBG, "PLUG ST: RDY -> DIS\n");
                     m_acHPDLosted[cInstance] = TRUE;
                }
                else /*if (!txStat.isHotPlugDetected)*/
                {
                    // Most likely an HPD glitch, or short H->L->H pulse.
                    // This is a special case. We must react with re-connect
                    // message to handle the pulse properly.
                    fbMsg = SI_TX_FB_MSG_HOT_PLUG_RECONNECT;
                    pTx[cInstance]->status.isDsConnected = false;
                    DEBUG_PRINT( TX_MSG_DBG, "PLUG ST: RDY -> REC\n");
                    RequestTmdsOutputEnable( cInstance ,false);
                }
            }
            else
            {
                // RE-CONNECTION State
                if (txStat.isHotPlugDetected)
                {
                    fbMsg = SI_TX_FB_MSG_HOT_PLUG_CONNECTED;
                    pTx[cInstance]->status.isSinkReady = false;
                    pTx[cInstance]->status.isDsConnected = true;
                    DEBUG_PRINT( TX_MSG_DBG, "PLUG ST: REC -> CON\n");
                }
                else if (!txStat.isRxSensed)
                {
                    fbMsg = SI_TX_FB_MSG_HOT_PLUG_DISCONNECTED;
                    pTx[cInstance]->status.isSinkReady = false;
                    DEBUG_PRINT( TX_MSG_DBG,"PLUG ST: REC -> DIS\n");
                    m_acHPDLosted[cInstance] = TRUE;
                }
            }
        }
        else
        {
            if (pTx[cInstance]->status.isDsConnected)
            {
                // CONNECTED State
                if (txStat.isHotPlugDetected && txStat.isRxSensed)
                {
                    fbMsg = SI_TX_FB_MSG_SINK_READY;
                    pTx[cInstance]->status.isSinkReady = true;
                    DEBUG_PRINT( TX_MSG_DBG, "PLUG ST: CON -> RDY\n");
                    m_acHPDLosted[cInstance] = FALSE;
                }
                else if (!txStat.isHotPlugDetected)
                {
                    fbMsg = SI_TX_FB_MSG_HOT_PLUG_DISCONNECTED;
                    pTx[cInstance]->status.isDsConnected = false;
                    DEBUG_PRINT( TX_MSG_DBG, "PLUG ST: CON -> DIS\n");
                    m_acHPDLosted[cInstance] = TRUE;
                }
            }
            else
            {
                // DISCONNECTED State
               
                {

                    if (txStat.isHotPlugDetected) // add by kyq 20151009, martix swith source will be HPD EVENT
                    {
                        // Plug In conditions: HPD Low -> High, disconnected previously
                        // Hardware ensures debouncing at plug stage
                        if (m_acHPDLosted[cInstance])
                        {
                             fbMsg = SI_TX_FB_MSG_HOT_PLUG_CONNECTED;
                        }
                        else
                        {
                            fbMsg = SI_TX_FB_MSG_HOT_PLUG_CONNECTED2;
                        }

                       
                        pTx[cInstance]->status.isDsConnected = true;
                        DEBUG_PRINT(TX_MSG_DBG, "PLUG ST: DIS -> CON\n");
                    }
                    // add by kyq 20151009
                    else if(!txStat.isHotPlugDetected ||!txStat.isRxSensed)
                    {
                      m_acHPDLosted[cInstance] = TRUE;  // HPD is low
                      
                    }// end add
                      
                }
            }

        }

       // DEBUG_PRINT( TX_MSG_DBG, "HPD: %d  RSEN: %d  isDsConnected: %d isSinkReady: %d\n",
       //         txStat.isHotPlugDetected, txStat.isRxSensed, pTx[cInstance]->status.isDsConnected,  pTx[cInstance]->status.isSinkReady);
    }

    // Reset all interrupt flags to avoid repeated response
    SiiDrvTpiPendingInterruptsClear( cInstance );

    // Service timer handlers
    while (!fbMsg)
    {
    // 在没有消息需要处理下
        TmdsTimerHandler( cInstance, elapsedTimeMs);

        (void) TxHdcpTimerHandler( cInstance, elapsedTimeMs);
        fbMsg = TxHdcpLastMessageGet( cInstance );

        break;
    }

    return fbMsg;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return time in millisecond of requested recall of the SiiTxTaskProcess() function.
//-------------------------------------------------------------------------------------------------

time_ms_t SiiTxTaskRecallTimeMsGet(void)
{
    return 0;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Configures video bus source parameters of the video:
//!             color encoding, pixel clock properties and the bus width.
//!
//! @param[in]  source   - input video interface type,
//! @param[in]  pBusMode - pointer to input video bus configuration structure.
//-------------------------------------------------------------------------------------------------

void SiiTxVideoSourceConfig( BYTE cInstance, txVideoSource_t source, txVideoBusMode_t *pBusMode)
{
    pTx[cInstance]->videoSource = source;
    ClearErrorStatus( cInstance );

    // Switch TX chip to the given source of video
    if(!SiiDrvTpiVideoSourceSelect( cInstance, source))
    {
        ReportIssue(cInstance,SI_TX_ERROR_CODE_INVALID_VIDEO_SOURCE);
        return;
    }

    if (pBusMode != NULL)
    {
        pTx[cInstance]->videoBusMode = *pBusMode;

#if (SI_TX_VIDEO_CONVERSION == SI_ENABLE)
        if (TxIsRunning(cInstance) && (source == SI_TX_VIDEO_SOURCE_VBUS))
        {
            // Update TX path if TX is ON, but
            // do not update if TX is OFF
            // (to reduce unnecessary I2C traffic)
            TxVideoConversionSet( cInstance, pTx[cInstance]->isTxOutInHdmiMode, &(pTx[cInstance]->videoBusMode), true);
        }
        else if (source == SI_TX_VIDEO_SOURCE_HDMI)
        {
            TxVideoBlankingLevelsSet( cInstance, pTx[cInstance]->videoBusMode.colorSpace != SI_TX_VBUS_COLOR_SPACE_RGB,
                                     pTx[cInstance]->videoBusMode.quantRange == SI_TX_VIDEO_QRANGE_FULL);
        }
#endif
    }

}


//-------------------------------------------------------------------------------------------------
//! @brief      Set input video format.
//!
//!             Sets resolution, timings, clock parameters for correct capturing of video on
//!             the input bus. 3D modes are defined using combination of 3D format ID and
//!             standard video format ID.
//!
//! @param[in]  stdFormatId - Standard (CEA-861-D) video format ID.
//!                           If this number is equal to SI_TX_VIDEO_MODE_NON_STD  (0xFF),
//!                           the structure addressed by pFmt parameter has to provide all timings
//!                           and resolution data.
//! @param[in]  pFmt        - pointer to a structure describing video format parameters related
//!                           to a frame geometry and refresh rate.
//! @param[in]  v3dFormatId - ID of 3D frame structure for 3D video modes.
//! @param[in]  syncSrc     - Specifies wired video synchronization signals that are provided
//!                           and not embedded into data.
//! @param[in]  pSynRg      - Pointer to a structure defining the way how synchronization signals
//!                           (VSYNC, HSYNC, DE)  are extracted or regenerated
//!                           from video data stream.
//-------------------------------------------------------------------------------------------------

void SiiTxInputVideoModeSet( BYTE cInstance,uint8_t stdFormatId, videoFormat_t *pFmt, video3dFormat_t v3dFormatId,
                            syncSignals_t syncSrc, syncRegen_t *pSynRg)
{
    bool_t isUseVideoTable = (stdFormatId != SI_VIDEO_MODE_NON_STD);
    ClearErrorStatus( cInstance );

    // In case of non-standard video format, pFmt must be a valid pointer.
    // Caller must provide Sync/DE regeneration parameters, if at least one of the synchronization
    // signals is not present at the video input and has to be generated.
    if (((syncSrc != SI_TX_VIDEO_HVSYNC_DE) && (pSynRg == NULL))  ||
        ((stdFormatId == SI_VIDEO_MODE_NON_STD) && (pFmt == NULL)) ||
        (stdFormatId >= NMB_OF_VIDEO_MODES))
    {
        ReportIssue( cInstance ,SI_TX_ERROR_CODE_INVALID_PARAMETER);
        return;
    }

    pTx[cInstance]->videoFormatId = stdFormatId;

    if (isUseVideoTable)
    {
        const videoMode_t *pVideoMode = &VideoModeTable[stdFormatId];
        bool_t isInterlaced = pVideoMode->Interlaced;
        videoFormat_t videoMode;

        // Create local copy of the table entry to be able to modify the parameters
        // to accommodate 3D video modes
        videoMode.pixelFrequency = pVideoMode->PixClk;
        videoMode.verticalFrequency = pVideoMode->VFreq;
        videoMode.pixelsPerLine = pVideoMode->Total.H;
        videoMode.linesPerFrame = pVideoMode->Total.V;

        // In 3D mode modify pix clock etc.
        switch (v3dFormatId)
        {
            case SI_TX_VIDEO_3D_NONE:               // Don't modify video parameters for non-3D modes
                break;

            case SI_TX_VIDEO_3D_FRAME_PACKING:      // Frame packing: progressive/interlaced
            case SI_TX_VIDEO_3D_LINE_ALTERNATIVE:   // Line alternative: progressive only
            case SI_TX_VIDEO_3D_L_PLUS_DEPTH:       // L + depth: progressive only
                // multiply lines x2; multiply clock x2
                videoMode.linesPerFrame *= 2;
                videoMode.pixelFrequency *= 2;
                break;

            case SI_TX_VIDEO_3D_FIELD_ALTERNATIVE:  // Field alternative: progressive/interlaced
                // multiply clock x2
                videoMode.pixelFrequency *= 2;
                break;

            case SI_TX_VIDEO_3D_SIDE_BY_SIDE_FULL: // Side-by-Side (Full): progressive/interlaced
                // multiply pixel x2; multiply clock x2
                videoMode.pixelsPerLine *= 2;
                videoMode.pixelFrequency *= 2;
                if (isInterlaced)
                {
                    videoMode.linesPerFrame /= 2;
                }
                break;

            case SI_TX_VIDEO_3D_L_PLUS_DEPTH_GFX: // L + depth + graphics + graphics-depth: progressive only
                // multiply lines x4; multiply clock x4
                videoMode.linesPerFrame *= 4;
                videoMode.pixelFrequency *= 4;
                break;

            default: // 2D timing compatible: progressive/interlaced
                if (isInterlaced)
                {
                    videoMode.linesPerFrame /= 2;
                }
        }

        pTx[cInstance]->pixelFrequency = videoMode.pixelFrequency;
        SiiDrvTpiVideoModeDataSet(videoMode.pixelFrequency, videoMode.verticalFrequency, videoMode.pixelsPerLine, videoMode.linesPerFrame);
    }
    else
    {
        pTx[cInstance]->pixelFrequency = pFmt->pixelFrequency;
        SiiDrvTpiVideoModeDataSet(pFmt->pixelFrequency, pFmt->verticalFrequency, pFmt->pixelsPerLine, pFmt->linesPerFrame);
    }

    SyncSignalsRegenerationSet( cInstance ,syncSrc, pSynRg);

    // Release lock from AVI, if it was set
    TxInfoFrameLockSet( cInstance ,SI_INFO_FRAME_AVI, false);
    // Create AVI Packet based on provided video settings
    TxAviFrameCreate( cInstance ,(videoFormatId_t)(pTx[cInstance]->videoFormatId), &(pTx[cInstance]->videoBusMode));

    // Update TX video conversion path as range control depends on video index.
    // Note: changing resolution alone should not affect sending out AVI packet
    // because it changes range control option only.
#if (SI_TX_VIDEO_CONVERSION == SI_ENABLE)
    TxVideoConversionSet( cInstance ,pTx[cInstance]->isTxOutInHdmiMode, &(pTx[cInstance]->videoBusMode), false);
#endif

    // Start AVI transmission.
    // If output is DVI, the packet will be gated by HW logic.
    // Note: updating last AVI packet byte also triggers
    // color space conversion data update.
    TxAviPostAviPacket( cInstance );

}


//-------------------------------------------------------------------------------------------------
//! @brief      Set source of input audio and audio bus configuration.
//!
//!             Configures the interface, such as the I2S input bus,
//!             if the source selected is I2S. pSetting will be ignored for all sources other
//!             than  I2S and DSD and can be set to zero.
//!
//! @param[in]  source             - wired audio source type,
//! @param[in]  pInterfaceSettings - pointer to a structure defining settings for I2S receiver.
//-------------------------------------------------------------------------------------------------

void SiiTxAudioSourceConfig( BYTE cInstance,txAudioSrc_t source, txAudioSrcConfig_t *pInterfaceSettings)
{
    ClearErrorStatus( cInstance );

    if (pInterfaceSettings)
    {
        switch (source)
        {
            case SI_TX_AUDIO_SRC_I2S_L0:
            case SI_TX_AUDIO_SRC_I2S_L1:
            case SI_TX_AUDIO_SRC_DSD_L0:
            case SI_TX_AUDIO_SRC_DSD_L1:
            case SI_TX_AUDIO_SRC_HBRA:
                TxAudioI2SConfigSet( cInstance, source, pInterfaceSettings);
                break;
            case SI_TX_AUDIO_SRC_SPDIF:
                break;
            default:
                ReportIssue( cInstance, SI_TX_ERROR_CODE_INVALID_PARAMETER);
        }
    }
    else
    {
        ReportIssue( cInstance, SI_TX_ERROR_CODE_INVALID_PARAMETER);
    }

}


//-------------------------------------------------------------------------------------------------
//! @brief      Set format of the audio stream.
//!
//! @param[in]  source - wired audio source type,
//! @param[in]  pFmt   - pointer to a structure defining audio parameters.
//-------------------------------------------------------------------------------------------------

void SiiTxInputAudioModeSet( BYTE cInstance, txAudioSrc_t source, audioFormat_t *pFmt)
{
    if (pFmt != NULL)
    {
        // Release lock from AIF, if it was set
        TxInfoFrameLockSet(cInstance,SI_INFO_FRAME_AUDIO, false);
        TxAudioModeSet(cInstance,source, pFmt);
        ClearErrorStatus(cInstance);
    }
    else
    {
        ReportIssue( cInstance, SI_TX_ERROR_CODE_INVALID_PARAMETER);
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Mute HDMI Audio source.
//-------------------------------------------------------------------------------------------------

void SiiTxInputAudioMute( BYTE cInstance)
{
    TxAudioMute( cInstance, true);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Unmute Audio source.
//-------------------------------------------------------------------------------------------------

void SiiTxInputAudioUnmute( BYTE cInstance)
{
    TxAudioMute( cInstance, false);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Does necessary steps to return transmitter to operational state when input video
//!             clock signal has been unsteady due to video source change or error.
//-------------------------------------------------------------------------------------------------

void SiiTxRecoverAfterSourceClockInterruption( BYTE cInstance)
{
    //TxHdcpRequestReauthentication();
    TxAllInfoFramesUpdate( cInstance );
    TxAviInfoFrameUpdate( cInstance );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Set output video parameters.
//!             Currently, only setting the color dithering mode for YCbCr 4:2:2 color space
//!             is supported.
//!
//! @param[in]  mode - pointer to a parameter structure.
//-------------------------------------------------------------------------------------------------

void SiiTxOutputVideoConfig( BYTE cInstance,txVideoOutMode_t *pMode)
{
#if (SI_TX_VIDEO_CONVERSION == SI_ENABLE)
    TxVideoConversionDitheringEnable( cInstance, pMode->isDitheringEnabled);

    if (pMode->isForceColorSpace)
    {
        TxVideoConversionOutColorSpaceSet( cInstance, pMode->colorSpace);
    }
#endif
}


//-------------------------------------------------------------------------------------------------
//! @brief      Mute output Video and Audio.
//-------------------------------------------------------------------------------------------------

void SiiTxAvMute( BYTE cInstance)
{
    TxInfoAvMute( cInstance, true);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Unmute output Video and Audio.
//-------------------------------------------------------------------------------------------------

void SiiTxAvUnmute( BYTE cInstance)
{
    TxInfoAvMute( cInstance, false);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Set InfoFrame of a given type.
//!
//!             This function is typically used in Repeater applications where Rx and Tx systems
//!             are in separate chips, and properly formatted frame structures are transferred
//!             from Rx to Tx by the microcontroller.
//!             @note This function only loads InfoFrame data into the Tx. Actual sending is
//!             triggered by SiiTxInfoFrameEnable() function.
//!
//! @param[in]  ifType - InfoFrame/Packet type,
//! @param[in]  pFrame - pointer to the InfoFrame. The frame must be properly formatted,
//!                      in accordance with HDMI specification
//-------------------------------------------------------------------------------------------------
void SiiTxInfoFrameSet( BYTE cInstance,infoFrameType_t ifType, void *pFrame)
{
    TxInfoFrameLockSet( cInstance,ifType, false); // Unlock info frame for modifications

    switch (ifType)
    {
        // Update audio settings based on new AIF
        case SI_INFO_FRAME_AUDIO:
            TxAudioUpdateFromInfoFrame( cInstance, (uint8_t *)pFrame);
            break;
        default:
            break;
    }

    TxInfoPacketSet( cInstance, ifType, pFrame);
    // Lock explicitly defined info frame, so that it won't be replaced by implicit (generated) data
    TxInfoFrameLockSet( cInstance, ifType, true);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Gets info frame of given type.
//!             @note Not implemented in this version of firmware.
//-------------------------------------------------------------------------------------------------

void * SiiTxInfoFrameGet( BYTE cInstance,infoFrameType_t ifType)
{
    return (void *)NULL;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Enable sending InfoFrame of a given type over HDMI link.
//!
//!             The InfoFrame shall be loaded first by SiiTxInfoFrameSet() function.
//!
//! @param[in]  ifType - InfoFrame/Packet type.
//-------------------------------------------------------------------------------------------------

void SiiTxInfoFrameEnable( BYTE cInstance,infoFrameType_t ifType)
{
    switch (ifType)
    {
        case SI_INFO_FRAME_ACP:
            // determine if audio content protection is required
            TxAudioOnAcpPacketUpdate( cInstance, false);
            break;
        default:
            TxInfoPacketTransmissionEnable( cInstance, ifType, true);
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Disable sending InfoFrame of a given type over HDMI link.
//!
//! @param[in]  ifType - InfoFrame/Packet type.
///-------------------------------------------------------------------------------------------------

void SiiTxInfoFrameDisable( BYTE cInstance,infoFrameType_t ifType)
{
    TxInfoPacketTransmissionEnable( cInstance, ifType, false);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Sets parameters of Tx HDCP engine.
//!
//!
//! @param[in]  isRepeaterMode - true for Repeater mode, false for Source mode;
//! @param[in]  umuteDelayMs   - minimum time delay in milliseconds between successful finishing
//!                              of HDCP authentication and unmuting of AV content on HDMI output.
//!                              Proper setting of this parameter should prevent any transient
//!                              effects to be visible as flicker at the downstream TV;
//!
//! @param[in]  isNotifyRxOnDdcFailure -  This flag makes sense and can be set to true for a
//!                                       repeater application. It makes the HDCP control logic
//!                                       to notify HDMI receiver (by the API call) about
//!                                       HDCP failure in downstream device. Such notification
//!                                       is essential for Rx part of the repeater to be able
//!                                       to handle the error on upstream device side.
//-------------------------------------------------------------------------------------------------

void SiiTxHdcpConfigure( BYTE cInstance,bool_t isRepeaterMode, uint_t umuteDelayMs, bool_t isNotifyRxOnDdcFailure)
{
    TxHdcpConfig( cInstance ,isRepeaterMode, umuteDelayMs, isNotifyRxOnDdcFailure);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Initiate HDCP authentication process with a downstream device.
//!
//!             A/V content will be muted in case of any HDCP failure, while authenticating
//!             and upon its completion while maintaining secure (encrypted) link.
//!             A notification message upon completion of the authentication is
//!             returned by SiiTxTaskProcess().
//-------------------------------------------------------------------------------------------------

void SiiTxHdcpEnable( BYTE cInstance)
{
    TxHdcpStart( cInstance );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Disable established HDCP authentication with downstream device.
//!
//!             This function is used to remove HDCP protection from the A/V content.
//!             The A/V content will not be muted.
//-------------------------------------------------------------------------------------------------

void SiiTxHdcpDisable( BYTE cInstance)
{
    TxHdcpStop( cInstance );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Return physical address of this device provided by the downstream EDID.
//!
//! @return     2-byte physical address.
//-------------------------------------------------------------------------------------------------

uint16_t    SiiTxPhysicalAddressGet( BYTE cInstance)
{
    return SiiEdidTxCecPhysAddrGet( cInstance );
}
