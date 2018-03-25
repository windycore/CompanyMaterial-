//************************************************************************************************
//! @file   si_drv_tpi_system.h
//! @brief  Low level TPI operations. This module directly talks to TPI registers.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2012, Silicon Image, Inc.  All rights reserved.
//************************************************************************************************/

#ifndef SI_TX_TPI_SYSTEM_H
#define SI_TX_TPI_SYSTEM_H

//-------------------------------------------------------------------------------------------------
//  Module Instance Data
//-------------------------------------------------------------------------------------------------

                                            //! TX TPI instance record
// set by  SiiDrvTpiProcessInterrupts()  ---> SiiDrvDeviceManageInterrupts()
typedef struct txTpiInstanceRecord
{
//  只有在D0 下才会赋值
    uint16_t txInterruptWord;       //!< Pending TX interrupt flags and , (ignore status bits)

// 只要有中断就会赋值
    uint8_t  plugStatus;            //!< Current status of HPD & RSEN
    //
    // 有中断产生,不是在D0 状态下设立起来1
    //          只有在D0 下才会清中断
    // 在循环 SiiTxTaskProcess 中 如果是1: 
    //                        不是D0状态,会把芯片设为D0
    //                              有HPD 中断会设为STANDBY,清除为 0 
    // SI_TX_FB_MSG_SINK_READY:
    // StartTx()--> SiiTxResume() 设为D0
    bool_t   isWakeUpRequired;      //!< Indicates a request to switch into full power mode, processby--> SiiTxTaskProcess

} txTpiInstanceRecord_t;

//-------------------------------------------------------------------------------------------------
//  TPI System API functions
//-------------------------------------------------------------------------------------------------

// General control group
#if (SII_NUM_TX > 1)
void     SiiDrvTpiInstanceSet( BYTE cInstance,uint8_t deviceIndex);
#endif

void SiiDrvTpiStructInstanceSet( BYTE cInstance,uint8_t deviceIndex); 
int      SiiDrvTpiInstanceGet(  BYTE cInstance );
void     SiiDrvTpiInit( BYTE cInstance);
void     SiiDrvTpiSwReset( BYTE cInstance);
void     SiiDrvTpiInterruptsEnable( BYTE cInstance,bool_t isInterruptsEnabled);

void     SiiDrvTpiPowerUp( BYTE cInstance);
void     SiiDrvTpiStandby( BYTE cInstance);
void     SiiDrvTpiPowerDown( BYTE cInstance);
bool_t   SiiDrvTpiIsPoweredUp( BYTE cInstance);


void     SiiDrvTpiTmdsOutputEnable( BYTE cInstance,bool_t isEnabled);
void     SiiDrvTpiHdmiOutputModeSet( BYTE cInstance,bool_t isHdmi);
uint8_t  SiiDrvTpiPlugStatusGet( BYTE cInstance);
uint16_t SiiDrvTpiDeviceIdGet( BYTE cInstance);
uint8_t  SiiDrvTpiRevisionGet( BYTE cInstance);

void     SiiDrvTpiProcessInterrupts( BYTE cInstance);
bool_t   SiiDrvTpiGetHpdState( BYTE cInstance);
uint16_t SiiDrvTpiPendingInterruptsGet( BYTE cInstance);
void     SiiDrvTpiPendingInterruptsClear( BYTE cInstance);
bool_t   SiiDrvTpiIsWakeUpRequired( BYTE cInstance);
void     SiiDrvTpiWakeUpRequestClear( BYTE cInstance);

bool_t   SiiDrvTpiIsHdmiLinkStable( BYTE cInstance);
void 	 SiiDrvTpiAudioInEnable( BYTE cInstance);

void     SiiDrvTpiAudioSamplePacketSourceSet( BYTE cInstance, bool_t isTpi);
void     SiiDrvTpiCtsPacketSourceSet( BYTE cInstance, bool_t isTpi);
void     SiiDrvTpiAvMuteSourceSet( BYTE cInstance, bool_t isTpi);

// Video group
bool_t   SiiDrvTpiVideoSourceSelect( BYTE cInstance, txVideoSource_t source);
void     SiiDrvTpiBlankingLevelsSet( BYTE cInstance, uint8_t levels[3]);
void     SiiDrvTpiVideoModeDataSet( uint16_t pixelClk, uint16_t vertFreqHz, uint16_t totalPixelsPerLine, uint16_t totalLines);
void     SiiDrvTpiPixelCapturingConfig( BYTE cInstance, txVbusClockMode_t clockMode, bool_t isLatchOnRisingEdge, uint8_t pixelRepetitionFactor, bool_t isInputReplicated);
void     SiiDrvTpiInputVideoColorSet( BYTE cInstance, txVbusColSpace_t colorSpace, txVbusColDepth_t bitsPerColor, bool_t isDataRangeExpansionRequired);
void     SiiDrvTpiOutputVideoColorSet( BYTE cInstance, txVbusColSpace_t colorSpace,  txVbusColDepth_t colorDepth, txVbusColorimetry_t colorimetry, bool_t isDataRangeCompressionRequired, bool_t isDitheringEnabled);
void     SiiDrvTpiDeepColorGcpEnable( BYTE cInstance, bool_t isEnabled);
void     SiiDrvTpiAudioClkDividerSet( BYTE cInstance, uint16_t tmdsClock, bool_t isSrcSpdif);
void     SiiDrvTpiAudioClkToggle( BYTE cInstance);

// DE and HV Sync regeneration group
void     SiiDrvTpiSyncRegenConfigurationSet( BYTE cInstance, syncRegen_t *pSynRg);
void     SiiDrvTpiDeGenerationEnable( BYTE cInstance, bool_t isEnabled, syncRegen_t *pSynRg);
void     SiiDrvTpiEmbSyncExtractionEnable( BYTE cInstance, bool_t isEnabled, syncRegen_t *pSynRg);
void     SiiDrvTpiYcDemuxEnable( BYTE cInstance, bool_t isEnabled);

// Audio group
void     SiiDrvTpiAudioI2sConfigSet( BYTE cInstance, txAudioSrcConfig_t *pI2sSettings, bool_t isHbr, bool_t isAudioEncoded);
void     SiiDrvTpiAudioSampleConfigSet( BYTE cInstance, audioSampleSize_t smpSize, audioSampleRate_t smpRate, bool_t isHbr);
void     SiiDrvTpiAudioInterfaceSet( BYTE cInstance, txAudioSrc_t source);
void     SiiDrvTpiAudioDisable( BYTE cInstance);
void     SiiDrvTpiAudioMuteSet( BYTE cInstance, bool_t isMuted);
void     SiiDrvTpiAudioStatusChannelSet( BYTE cInstance, uint8_t aStatusChannel[5]);
void     SiiDrvTpiAudioI2sPinsToFifoAssign( BYTE cInstance, audioSpkAlloc_t chanAlloc, bool_t isAutoDownsmp);
void     SiiDrvTpiAudioRxSource( BYTE cInstance, bool_t isEnabled, bool_t isMainPipe);

// Info Frames group
void     SiiDrvTpiAviFrameStart( BYTE cInstance, uint8_t *pAviFrame, uint8_t length, uint8_t checksum);
void     SiiDrvTpiAviFrameUpdate( BYTE cInstance);
void     SiiDrvTpiAvMuteSendingEnable( BYTE cInstance, bool_t isMuted);
void     SiiDrvTpiInfoFrameEnable( BYTE cInstance, bool_t isEnabled, bool_t isAll, infoFrameType_t ifType);
bool_t   SiiDrvTpiInfoFrameIsEnabled( BYTE cInstance, infoFrameType_t ifType);
void     SiiDrvTpiInfoFrameLoad( BYTE cInstance, infoFrameType_t ifType, uint8_t *pFrame, uint8_t length, bool_t isChecksumIncluded);

// HDCP group
void     SiiDrvTpiHdcpProtectionEnable( BYTE cInstance, bool_t isEnabled);
uint8_t  SiiDrvTpiHdcpStatusGet( BYTE cInstance);
bool_t   SiiDrvTpiHdcpIsPart2Done( BYTE cInstance);
bool_t   SiiDrvTpiHdcpIsAuthenticationGood( BYTE cInstance);
void     SiiDrvTpiHdcpDynamicAuthenticationEnable( BYTE cInstance, bool_t isEnabled);
void     SiiDrvTpiHdcpEncriptionEnable( BYTE cInstance, bool_t isEnable);
bool_t   SiiDrvTpiIsDownstreamHdcpAvailable( BYTE cInstance);
void     SiiDrvTpiBksvGet( BYTE cInstance, uint8_t aBksv[5]);
bool_t   SiiDrvTpiKsvListPortionSizeGet( BYTE cInstance, uint8_t *pBytesToRead);
bool_t   SiiDrvTpiIsZeroKsvListReady( BYTE cInstance);
void     SiiDrvTpiKsvListGet( BYTE cInstance, uint8_t *pBuffer, uint8_t length);
void     SiiDrvTpiBStatusGet( BYTE cInstance, uint8_t *pDsBStatus);
uint8_t  SiiDrvTpiBcapsGet( BYTE cInstance);
void     SiiDrvTpiHdcpRi128Comp( BYTE cInstance, uint8_t value);
void	 SiiDrvTpiHdcpR0CalcConfigSet(  BYTE cInstance, int isVsyncBase );
#endif //  SI_TX_TPI_SYSTEM_H
