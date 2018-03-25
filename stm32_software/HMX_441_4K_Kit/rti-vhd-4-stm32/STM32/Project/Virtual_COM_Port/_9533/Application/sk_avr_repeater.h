//***************************************************************************
//!file     si_avr_repeater.h
//!brief    SiI9535 Starter Kit firmware main module.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#ifndef _SI_AVR_REPEATER_H_
#define _SI_AVR_REPEATER_H_
//#include "GitHash.h"
#include "sk_application.h"
//------------------------------------------------------------------------------
//  Application Data
//------------------------------------------------------------------------------

void dvSii9533Init(void);


void dvSii9533Process(BYTE cInstance);

// 得到当前9533 TX HDCP 是否打开
BOOL dvSii9533TXHDCPGet(BYTE cInstance);
// 使能9533 的TX HDCP
void dvSii9533TXHDCPSetStart(BYTE cInstance,BOOL bEnable);
// 得到分辨率
BOOL  dvSii9533RXResolutionGet(BYTE cInstance,INT* pHorizPixel, INT *pVertLine,INT *pTmdsClock,bool_t* pbInterlace);
//
void dvSii9533TXAudioMixPassThroughConfig(BYTE cInstance,BOOL isAudioPassEnabled,BOOL isNonAudioPassEnabled);
void dvSii9533TXAudioMixPassThroughConfigStart(BYTE cInstance);


void dvSii9533PowerControl(  BYTE cInstance,SiiAppPowerStatus_t newState );

void dvSii9533HardWareReset(void);


#endif


