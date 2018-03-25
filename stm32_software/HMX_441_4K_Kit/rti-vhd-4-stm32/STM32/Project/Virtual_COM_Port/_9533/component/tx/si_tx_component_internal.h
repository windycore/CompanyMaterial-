//************************************************************************************************
//! @file   si_tx_component_internal.h
//! @brief  Transmitter software component (internal definitions)
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//************************************************************************************************/

#ifndef SI_TX_COMPONENT_INTERNAL_H
#define SI_TX_COMPONENT_INTERNAL_H

#include "si_tx_component.h"

// TX Component dependencies from other components
#include "si_edid_tx_component.h"

//-------------------------------------------------------------------------------------------------
// TX manifest constants
//-------------------------------------------------------------------------------------------------

//! @defgroup TX_STATUS_MASKS       Tx Status masks
//! @{
#define SI_TX_STATUS_MASK_RX_SENSE      (BIT0)  //!< Mask for Rx Sense signal
#define SI_TX_STATUS_MASK_HPD           (BIT1)  //!< Mask for Hot Plug Detect signal
//
//! @}


//! @defgroup TX_IRQ_MASKS          Tx Interrupts masks
//! @{
#define SI_TX_IRQ__HPD                   (BIT0) //!< HPD Interrupt, 插上和拔掉都会产生中断
#define SI_TX_IRQ__RX_SENSE              (BIT1) //!< RxSense Interrupt
#define SI_TX_IRQ__HPD_ST                (BIT2) //!< HPD State
#define SI_TX_IRQ__RX_SENSE_ST           (BIT3) //!< RxSense State
#define SI_TX_IRQ__AUDIO_ERR             (BIT4) //!< Audio Error
#define SI_TX_IRQ__SECURITY_CHNG         (BIT5) //!< Security Change
#define SI_TX_IRQ__V_READY               (BIT6) //!< V Ready
#define SI_TX_IRQ__HDCP_CHNG             (BIT7) //!< HDCP Status Change
#define SI_TX_IRQ__P_STABLE              (BIT8) //!< P_STABLE (PLL locked, HDMI Link stable)
#define SI_TX_IRQ__KSV_READY             (BIT9) //!< first KSV is available
//
//! @}

#define TX_MSG_DBG                   (MSG_DBG | DBGF_TS | DBGF_CN), DBG_TX
#define TX_MSG_DBG_PLAIN             (MSG_DBG)
#define TX_MSG_ALWAYS                (MSG_ALWAYS | DBGF_CN), DBG_TX
#define TX_MSG_ALWAYS_PLAIN          (MSG_ALWAYS)
#define TX_MSG_ERR                   (MSG_ERR | DBGF_CN)

#define SI_TX_MIN_OFF_TIME_MS       (101)   //!< Minimum time between TX off and On in ms
#define SI_TX_PWRUP_TO_TMDS_TIME_MS  (11)   //!< Minimum time between powering up the chip and enabling TMDS



//-------------------------------------------------------------------------------------------------
//  Component Instance Data
//-------------------------------------------------------------------------------------------------

typedef struct txInstanceData
{
    txRevision_t     revision;
    txPowerMode_t    powerMode;   // SiiTxStandby():SI_TX_POWER_MODE_STANDBY;   SiiTxResume():SI_TX_POWER_MODE_FULL   SiiTxShutDown():SI_TX_POWER_MODE_OFF
    txStatus_t       status;
    txError_t        lastResult;

    bit_fld_t        isTxOutInHdmiMode             : 1; //!< HDMI mode flag (vs DVI)
    //
    //   1:  跳过一次TmdsTimerHandler(), 随后设为 0
    //   意思应该是:  下一次TMDS 打开需要经过一个主循环
    bit_fld_t        isTmdsBlackoutPauseRequired   : 1; //!< If set, prevents too fast TMDS enabling after disable state
    // 
    //
    // 1:  当timeToTmdsEnable  时间到了, 就会打开TMDS, 随后设为 0
    // 主要用在当需要打开TMDS 时候时间还不满足，设立这个，稍后再打开TMDS
    bit_fld_t        isDelayedTmdsEnableRequested  : 1; //!< Indicates that TMDS enabling is scheduled
    // 标记TMDS  是否打开
    bit_fld_t        isTmdsEnabled                 : 1; //!< TMDS Enable flag
    bit_fld_t        isDeGeneratorEnabled          : 1; //!< DE generator enable flag
    bit_fld_t        isEmbSyncExtractEnabled       : 1; //!< Embedded Sync extraction enable flag

    // 在关掉TMDS 时候设立一个时间点, 下次要打开需要等待这么多时间
    time_ms_t        timeToTmdsEnable;   // 每隔一段时间(SI_TX_TIME_STEP_MIN_MS) 会减小SI_TX_TIME_STEP_MIN_MS
    uint8_t          videoFormatId;
    uint16_t         pixelFrequency;
    txVideoSource_t  videoSource;
    txVideoBusMode_t videoBusMode;

} txInstanceData_t;

// Macros for internal timers operation
#define      IS_UPDATED_TIMER_EXPIRED(T, d)   (!( (T > (d)) ? (T -= (d)) : (T = 0)))
#define      UPDATE_TIMER(T,d)                (void) IS_UPDATED_TIMER_EXPIRED(T, d)


//-------------------------------------------------------------------------------------------------
//  Internal TX Component functions (inward interfaces)
//-------------------------------------------------------------------------------------------------
// Instance data access functions
bool_t       TxIsTmdsEnabled( BYTE cInstance);
bool_t       TxIsRunningInHdmiMode( BYTE cInstance);
bool_t       TxIsRunning( BYTE cInstance);
uint8_t      TxVideoFormatIdGet( BYTE cInstance);
uint16_t     TxPixelFreqMhzGet( BYTE cInstance);
bool_t       TxIsOutInHdmiMode( BYTE cInstance);

// TMDS related functions
void         TxTmdsBlankPause( BYTE cInstance);


#endif // SI_TX_COMPONENT_INTERNAL_H
