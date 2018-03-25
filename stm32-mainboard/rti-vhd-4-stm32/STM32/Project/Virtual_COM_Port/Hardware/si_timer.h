//***************************************************************************
//!file     si_timer.h
//!brief    Platform timer system, ARM (Stellaris) version.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#ifndef __SI_TIMER_H__
#define __SI_TIMER_H__

//------------------------------------------------------------------------------
// Defines for setting up the system clock.
//------------------------------------------------------------------------------
//
// Define how many clock ticks in one second.
// Note:  This should match the value of SYSTICKHZ in the main program.
//
#define CLOCK_CONF_SECOND       1000         // 100 == 10ms

#define SYSTICKHZ               CLOCK_CONF_SECOND
#define SYSTICKMS               (1000 / SYSTICKHZ)      // Number of ms per SYSTICK
#define SYSTICKUS               (1000000 / SYSTICKHZ)
#define SYSTICKNS               (1000000000 / SYSTICKHZ)


#define ELAPSED_TIMER               0xFF
#define ELAPSED_TIMER1              0xFE  // CdcMsgHandler
//[kyq Begin: 20150804 19:49:05]
typedef enum
{
    etNVRAM_UPDATE, // NVRAM update
    etUI_CLEAR,
    etUI_ENTER,
    etUI_FLASHING,
    //etLED_FLASHING,
    etPOWERCNT,
    //etKEYSCAN,
    etIRSCAN,
    etIR_REPEAT,
    etHDMI_VERSION,
    etEDIDPROC_HPD,
    etOUTPUT_HDCP,
    etINPUT_HDCP,
    etINPORTMAG_EDID,

    TIMER_0,   // DO NOT USE - reserved for TimerWait()
    TIMER_1,  // DEM_POLLING_DELAY
    TIMER_2,
    TIMER_3,   // CbusWakeupSequence             // instance 0,      1,    2,   3,   4,   5
    TIMER_4,   //  AudioNoAcpCheck   //                            10     11   12  13   14  15

    //etSYS_TIME = TIMER_4 + DevNum+1,  //16
    etSYS_TIME = TIMER_4 +1,
    etKEYSCAN = etSYS_TIME,               // TIME2 to count
    etINVALID,  // not remove

}eTIMER;
#define   SYS_TICK_TIME_COUNT   etSYS_TIME
#define TIMER_COUNT   etINVALID
//[kyq End: 20150804 19:49:10]

/*
#define TIMER_0                     0   // DO NOT USE - reserved for TimerWait()
#define TIMER_1                     1 // DEM_POLLING_DELAY
#define TIMER_2                     2
#define TIMER_3                     3 // CbusWakeupSequence
#define TIMER_4                     4 //  AudioNoAcpCheck
//#define TIMER_5                     5 //  AudioNoAcpCheck
#define TIMER_COUNT                 (4+DevNum)
*/



//-------------------------------------------------------------------------------------------------
//  Module Instance Data
//-------------------------------------------------------------------------------------------------

typedef struct 
{
 uint16_t gTimerCounters;
 uint16_t gTimer;
 uint8_t gTimeCnt;
 
} IndexTime_t;



uint8_t      SiiPlatformTimerExpired ( uint8_t timer );
uint16_t     SiiPlatformTimerElapsed ( uint8_t index );
void         SiiPlatformTimerWait ( uint16_t m_sec );
void SiiPlatformTimerSet( uint8_t index, uint16_t m_sec, uint8_t cnt_time);
void         SiiPlatformTimerInit ( void);
clock_time_t SiiPlatformTimerSysTicksGet (void);
clock_time_t SiiPlatformTimerGet (eTIMER eTimer);


#endif

