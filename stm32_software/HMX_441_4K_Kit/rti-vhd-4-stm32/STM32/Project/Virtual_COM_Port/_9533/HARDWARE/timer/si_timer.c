//***************************************************************************
//!file     si_timer.c
//!brief    Platform timer system, ARM (Stellaris) version.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_common.h"
#include "si_timer.h"
#include "stm32f10x.h"
#include "KeyTasks.h"
#include "uart.h"


//------------------------------------------------------------------------------
// Array of timer values
//------------------------------------------------------------------------------
volatile unsigned long g_ulTickCounter = 0;
IndexTime_t  gTimers[TIMER_COUNT];
volatile uint16_t gTimerElapsed;
volatile uint16_t gElapsedTick;
volatile uint16_t gTimerElapsedGranularity;

volatile uint16_t gTimerElapsed1;
volatile uint16_t gElapsedTick1;
volatile uint16_t gTimerElapsedGranularity1;


//extern uint16_t SignalInScan;
//extern uint16_t  swflag_delay;
extern WORD  g_wUSART1_TIMER;
extern BOOL   g_bUSART1_DataReceived; // vu8 uart_data_in_start = 0; //有数据在接收
extern BOOL   g_bUSART1_DataComplete; // TRUE: the RX message is complete vu8 uart_data_in_end = 0; //数据接收完成

void TIM2_IRQHandler(void)   //
{
    BYTE i = 0;
    TIM_ClearFlag(TIM2, TIM_FLAG_Update); //一定要清中断
   for( i=  SYS_TICK_TIME_COUNT; i<   TIMER_COUNT; i++)   // only update etKEYSCAN
   {
   	if ( gTimers[i].gTimerCounters > 0)
        {
            gTimers[i].gTimerCounters--;
   	 }
	
   }

   /*  here my be case some error for other ISR process*/
  // * 使用外部中断不够灵敏
  if(0 == gTimers[etKEYSCAN].gTimerCounters)
  {
  	halUICtrlEventHandler();
  }

}

#ifdef MODIFY_IR_SWITCH_2017_2_21
WORD g_LEDFlashBlankTimeCnt = 0;//LED闪烁时间间隔
bool g_bSwitcherFlag = true;//开关灯
bool g_bLEDUpdateMainLoopFlag = true;//是否刷新主循环LED显示
bool g_bLEDBlankTimeOKFlag = false;//LED闪烁时间间隔到标志
bool g_bFlashSpillTimeFlag = false;//闪烁超时标志
WORD g_wFlashSpillTimeDelay = 0;//闪烁超时时间
#if (MACHINETYPE == MUH4K862)
extern bool g_bFlashOutPortSelect[8];
#elif (MACHINETYPE==MHUB4K431)
extern bool g_bFlashOutPortSelect[4];
#endif
extern void LightColumnOutPortLed_Process(BYTE Output, bool bLight);
#endif
//系统定时器  1ms
#ifdef MODIFY_IR_SWITCH_2017_2_21
void LEDFlash(bool bLignt)
{
	BYTE i =0;
	if(bLignt)
	{
		#if (MACHINETYPE==MHUB4K431)
		for(i = 0; i < 4; i++)
		{
			if(g_bFlashOutPortSelect[i])
				LightColumnOutPortLed_Process(i, bLignt);
		}	
		#elif(MACHINETYPE==MUH4K862)
		for(i = 0; i < 8; i++)
		{
			if(g_bFlashOutPortSelect[i])
				LightColumnOutPortLed_Process(i, bLignt);
		}
		#endif
	}
	else
	{		
		#if (MACHINETYPE==MHUB4K431)
		for(i = 0; i < 4; i++)
		{
			if(g_bFlashOutPortSelect[i])
				LightColumnOutPortLed_Process(i, bLignt);
		}
		#elif(MACHINETYPE==MUH4K862)
		for(i = 0; i < 8; i++)
		{
			if(g_bFlashOutPortSelect[i])
				LightColumnOutPortLed_Process(i, bLignt);
		}
		#endif
	}
}
#endif
void SysTick_Handler(void)
{
    uint8_t i;
	uint16_t tempValue;
	
	#ifdef MODIFY_IR_SWITCH_2017_2_21
	if(g_wFlashSpillTimeDelay > 0)
	{
		g_wFlashSpillTimeDelay--;
		if(g_wFlashSpillTimeDelay == 0)
			g_bFlashSpillTimeFlag = true;
	}
	
	if(g_LEDFlashBlankTimeCnt > 0)
	{
		g_LEDFlashBlankTimeCnt--;
		if(g_LEDFlashBlankTimeCnt == 0)
		{
			//g_bLEDBlankTimeOKFlag = true;
			g_bSwitcherFlag = !g_bSwitcherFlag;
			g_LEDFlashBlankTimeCnt = 400;
		}
	}
	#endif
    g_ulTickCounter++;              // Increment the system tick count.
                                    // Decrement all active timers in array
    for (i = 0; i < SYS_TICK_TIME_COUNT; i++)
    {
        if (gTimers[i].gTimerCounters > 0)
        {
            gTimers[i].gTimerCounters--;
        }
 
    }
    gElapsedTick++;
	tempValue = gTimerElapsedGranularity;	// wf8421 20160810 两个volatile 变量之间不能直接比较，会发生不可预见的错误
    //if (gElapsedTick == gTimerElapsedGranularity)
	if(gElapsedTick == tempValue)
    {
        gTimerElapsed++;
        gElapsedTick = 0;
    }
    gElapsedTick1++;
	tempValue = gTimerElapsedGranularity1;	// wf8421 20160810 两个volatile 变量之间不能直接比较，会发生不可预见的错误
    if (gElapsedTick1 == tempValue)
    //if (gElapsedTick1 == gTimerElapsedGranularity1)
    {
        gTimerElapsed1++;
        gElapsedTick1 = 0;
    }
  
  	for (i = 0; i < MAX_OUT; i++)
    {
    		if(OutHDCP_TimeDelay[i] > 0)   OutHDCP_TimeDelay[i] --;
    }

//--------------------------------------------
  if(g_wUSART1_TIMER>0)
  	g_wUSART1_TIMER--;
  else 
  	{
          if(g_bUSART1_DataReceived)
          	{
                g_bUSART1_DataComplete = TRUE;
		  g_bUSART1_DataReceived = FALSE;
          	} 
  	}
 //---------------------------------------------
}


//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerSysTicksGet
// Description: Returns the current number of system ticks since we started
// Parameters:
// Returns:
//------------------------------------------------------------------------------
//  1ms
clock_time_t SiiPlatformTimerSysTicksGet(void)
{
    return((clock_time_t)g_ulTickCounter);
}
clock_time_t SiiPlatformTimerGet(eTIMER eTimer)
{
    return((clock_time_t)gTimers[eTimer].gTimerCounters);
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerSet
// Description:
//------------------------------------------------------------------------------

void SiiPlatformTimerSet( uint8_t index, uint16_t m_sec, uint8_t cnt_time)
{
   // ROM_SysTickIntDisable();
    switch ( index )
    {
    

    case ELAPSED_TIMER:
        gTimerElapsedGranularity = m_sec;
        gTimerElapsed = 0;
        gElapsedTick = 0;
        break;
    case ELAPSED_TIMER1:
        gTimerElapsedGranularity1 = m_sec;
        gTimerElapsed1 = 0;
        gElapsedTick1 = 0;
        break;
    default:
//  case TIMER_0:
//  case TIMER_1:
//  case TIMER_2:
//  case TIMER_3:
//  case TIMER_4:
//  case TIMER_5:
        if (index<TIMER_COUNT)
        {
          gTimers[index].gTimerCounters= m_sec;
	   gTimers[index].gTimer= m_sec;
	//   if(cnt_time==0xff)
	//    gTimers[index].gTimeCnt=1 ;
	//   else
	    gTimers[index].gTimeCnt=cnt_time;	
        }
        
        break;
    }
   // ROM_SysTickIntEnable();
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerExpired
// Description: Returns > 0 if specified timer has expired.
//------------------------------------------------------------------------------

uint8_t SiiPlatformTimerExpired ( uint8_t timer )		   //定时时间到函数
{
    if ( timer < TIMER_COUNT )
    {
        if(gTimers[timer].gTimerCounters == 0)
        {
            if(gTimers[timer].gTimeCnt>0)
            {
                gTimers[timer].gTimerCounters=gTimers[timer].gTimer;
                gTimers[timer].gTimeCnt--;
                return 1;//定时时间到时返回1
            }
        }
    }

    return( 0 );										   //定时时间还没到时返回0
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerElapsed
// Description: Returns current timer tick.  Rollover depends on the
//              granularity specified in the SetTimer() call.
//------------------------------------------------------------------------------

uint16_t SiiPlatformTimerElapsed ( uint8_t index )
{
    uint16_t elapsedTime;

  //  ROM_SysTickIntDisable();
    if ( index == ELAPSED_TIMER )
        elapsedTime = gTimerElapsed;
    else
        elapsedTime = gTimerElapsed1;

 //   ROM_SysTickIntEnable();
    return( elapsedTime );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerWait
// Description: Waits for the specified number of milliseconds, using timer 0.
//------------------------------------------------------------------------------

void SiiPlatformTimerWait ( uint16_t m_sec )
{
    SiiPlatformTimerSet( 0, m_sec ,0xff);		   //设置时间

    while ( !SiiPlatformTimerExpired( 0 ) )		//等待时间到达
    {
        ;  //empty wait loop
    }
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerInit
// Description:
// Parameters:
// Returns:
//------------------------------------------------------------------------------
//[kyq Begin: 20140915 18:09:38]  TODO
void SiiPlatformTimerInit (void)
{
    int i;

    //initializer timer counters in array

    for ( i = 0; i < TIMER_COUNT; i++ )
    {
        gTimers[i].gTimerCounters= 0;
	 gTimers[i].gTimeCnt=0;
    }
    gTimerElapsed  = 0;
    gTimerElapsed1 = 0;
    gElapsedTick   = 0;
    gElapsedTick1  = 0;
    gTimerElapsedGranularity   = 0;
    gTimerElapsedGranularity1  = 0;

    // Configure SysTick for a 1ms periodic interrupt.

//    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKHZ);
 //   ROM_SysTickEnable();
 //   ROM_SysTickIntEnable();
}

