//***************************************************************************
//!file     si_common.h
//!brief    Silicon Image common definitions header.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#ifndef __SI_COMMON_H__
#define __SI_COMMON_H__
#include "si_datatypes.h"
#include "si_platform.h"
#include "si_debug.h"
#include "board.h" //[kyq Begin: 20150717 12:41:39]
//[kyq End: 20150717 11:22:35]
// Standard result codes are in the range of 0 - 4095
#define YES                         1
#define NO                          0
//------------------------------------------------------------------------------
//  Basic system functions
//------------------------------------------------------------------------------
//uint8_t SiiTimerExpired( uint8_t timer );
//long    SiiTimerElapsed( uint8_t index );
//void    SiiTimerSet( uint8_t index, uint16_t m_sec );
//void    SiiTimerInit( void );
#endif  // __SI_COMMON_H__
