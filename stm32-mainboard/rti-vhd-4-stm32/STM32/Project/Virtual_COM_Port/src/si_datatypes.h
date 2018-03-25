//***************************************************************************/
//!file     si_datatypes.h
//!brief    Silicon Image data type header (conforms to C99).
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DATATYPES_H__
#define __SI_DATATYPES_H__

#include "limits.h"
#include <stdint.h>
#include <stdarg.h>
#ifdef   __ICCARM__
#include "stm32f10x.h"// kyq
#include "stdbool.h"  // kyq
#endif
#include "board.h"



#define MUH44E_BYE  	1
#define MUH88E_BYE  	2
#define MUH44TPR2_N  	3
#define MUH44F  			4
#define NDS_UM44   		5
#define NDS_UHM44   		6
#define  PRD_VHD4 		7
#define  MUH44A_H2 		8
#define  MUH88A_H2 		9

#define  MACHINETYPE   MUH44A_H2
//#define  MACHINETYPE   MUH88A_H2

#define  UART1_DEBUG   0


#if (MACHINETYPE==MUH44E_BYE)
#define INSTANCE_9533  4   // 9533 incstance
#define INSTANCE_9679   4
#define ADN4604_NUM_INSTANCES 1
#define PCA9539_NUM_INSTANCES   1
#define NV24LC32A
#elif (MACHINETYPE==MUH88E_BYE)
#define INSTANCE_9533  8  // 9533 incstance
#define INSTANCE_9679  8
#define ADN4604_NUM_INSTANCES 2
#define PCA9539_NUM_INSTANCES   2
#define AT24CM01_1024
#define PRD_POWER_ONOFF_MESSAGE
#define PRD_IR_OUT  1
//#define  HDMI2_HDMI1
#elif (MACHINETYPE==MUH44A_H2)
#define INSTANCE_9396_IN			4
#define INSTANCE_9396_OUT			4
#define SII9396_DevNum				8
#define M21452_NUM_INSTANCES 		2		//«–ªª–æ∆¨M21452
#define PCA9539_NUM_INSTANCES   	1
#define AT24CM01_1024
#define SWITCH_CMD_COTINUE
#elif (MACHINETYPE==MUH88A_H2)
#define INSTANCE_9396_IN			8
#define INSTANCE_9396_OUT			8
#define SII9396_DevNum				16
#define M21452_NUM_INSTANCES 		4		//«–ªª–æ∆¨M21452
#define PCA9539_NUM_INSTANCES   	1
#define AT24CM01_1024
#define SWITCH_CMD_COTINUE
#elif(MACHINETYPE==MUH44TPR2_N)
#define PRD_POWER_ONOFF_MESSAGE
#define PRD_IR_OUT  1
#elif(MACHINETYPE==PRD_VHD4)
#define VHD4_CPLD
#endif

#define DevNum   SII9396_DevNum    	// 9533 incstance

#define		INC_ARC 	 		0

#if (MACHINETYPE==MUH88E_BYE||MACHINETYPE==PRD_VHD4)
#define		INC_CEC 			1
#elif(MACHINETYPE==MUH44E_BYE||MACHINETYPE==MUH44TPR2_N)
#define		INC_CEC 			0
#endif

#define		INC_CDC 	 		0
#define		INC_CBUS 	 		0
#define		FPGA_BUILD 	 		0

#define		INC_RTPI	 		0
#define		INC_BENCH_TEST	 	0
#define		INC_OSD		 		0
#define		INC_CBUS_WAKEUP	 	0
#define		INC_SETUP	 		0

#define		INC_AUDIO_RX	 	1
#define		INC_EDID	 		1
#define		INC_TX		 		1
#define		INC_DIAG	 		0
#define		INC_REPEATER_AVR 	1

#define		INC_VIDEO_TABLES 	1
#define		INC_EDID_TX	 		1
#define		INC_DEBUGGER	 	0
#define		INC_CEC_SAC      	0
#define		INC_CEC_SWITCH   	0
#define 	       INC_CEC_LOGGER	0  // CEC MESSAGE

#define		INC_THX 	 		1
#define		INC_HEAC	 		1
#define		INC_AUDIO_MIX	 	1
#define		INC_GPIO 	 		1
#define		INC_MSW		 		0

#define		INC_NVRAM	 		1
#define		INC_IV				0

#define		INC_LED				1
#define		INC_KEY				1
#define		INC_UART			1
#define		INC_CPLD			1
#define		INC_IR				1

#define		INC_SWITCH_VIDEO			1
#define		INC_OUTPUT_VIDEO			1
#define		INC_LCD						1
#define          INC_INPUT_VIDEO  			1

// Use these two types most of the time, such as for index variables,
// etc. when you want the native integer type and don't
// care what size it is (within reason; most int types are 16
// or 32 bit, not 8 bit -- even on the 8051)
// On 8051 compilers, these types can be defined as 8-bit to
// use the most efficient data size for that processor.  When you
// absolutely need an integer of at least 16 bits, use the native type int
typedef unsigned char  BYTE, *PBYTE;
typedef unsigned short WORD, *PWORD;
//typedef unsigned long   FWORD, *FWORD;  // 64BIT
typedef const BYTE *PCBYTE;
typedef short INT, *PINT, **PPINT;   // 16BIT
typedef unsigned  int DWORD;              // 32BIT
typedef void VOID, *PVOID;


typedef signed int      int_t;
typedef unsigned int    uint_t;
//typedef char            int_t;    // Eight-bit processor only
//typedef unsigned char   uint_t;   // Eight-bit processor only
typedef int             prefuint_t; // Platform-specific efficient integers
typedef unsigned char tBoolean;

/* C99 defined data types.  */

/*typedef unsigned char  uint8_t;		// Don't use this unless you truly need an unsigned 8 bit variable
typedef unsigned short uint16_t;	// Don't use this unless you truly need an unsigned 16 bit variable
typedef unsigned long  uint32_t;    // Don't use this unless you truly need an unsigned 32 bit variable


typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed long    int32_t;	   */

typedef unsigned long   clock_time_t;   // The clock type used for returning system ticks (1ms).
typedef unsigned short  time_ms_t;      // Time in milliseconds

#define CLOCK_TIME_MAX  ULONG_MAX       // Maximum value of time type

// gcc -pedantic does not allow bit field operation, so to avoid the warning, we need the following trick
typedef unsigned char   bit_fld_t;    // bit field type used in structures
typedef unsigned short  bit_fld16_t;

/* Emulate C99/C++ bool type    */
#ifdef   __ICCARM__ // //[kyq Begin: 20150714 10:43:43]

typedef bool bool_t;
typedef  bool_t   BOOL;  // kyq

#else

#ifdef __cplusplus

typedef bool bool_t; TEST

#else

typedef enum
{
    false   = 0,
    true    = !(false)
}
bool_t;

#endif // __cplusplus

#define  bool   bool_t

#endif

#define  TRUE	true
#define  FALSE  false

#if !defined ON
#define ON  true
#define OFF false
#endif
#define   PON    1
#define   POFF   0

#define SET_BITS    0xFF
#define CLEAR_BITS  0x00

#define ROM              // 8051 type of ROM memory
#define XDATA          // 8051 type of external memory

#define PACKED

#define ABS_DIFF(A, B) ((A>B) ? (A-B) : (B-A))


//[kyq Begin: 20150717 11:22:33]
// 1HZ = 1s
// 1000 HZ = 1ms
#define TICK_TIME_mS   (1000u/OS_CFG_TICK_RATE_HZ)  // “ª∏ˆTICK ∂‡…Ÿms
#define mS_TO_TICKS(t) (WORD)(((DWORD)(t)+TICK_TIME_mS-1L) / TICK_TIME_mS)


//extern uint8_t DeviceID;

//------------------------------------------------------------------------------
// Configuration defines used by hal_config.h
//------------------------------------------------------------------------------

//#define ENABLE      (1)
//#define DISABLE     (0)
#define BIT(x)  (1<<x)
#define YES  1


#define BIT0 		0x01
#define BIT1 		0x02
#define BIT2 		0x04
#define BIT3 		0x08
#define BIT4 		0x10
#define BIT5 		0x20
#define BIT6 		0x40
#define BIT7 		0x80
#define BIT8        0x0100
#define BIT9        0x0200
#define BIT10       0x0400
#define BIT11       0x0800
#define BIT12       0x1000
#define BIT13       0x2000
#define BIT14       0x4000
#define BIT15       0x8000

//------------------------------------------------------------------------------
// Language extending macros
//------------------------------------------------------------------------------

// Test chain macro
// Note: "switch" operator must be used with caution as PASS_IF includes "break"
#define BEGIN_TEST do{
#define PASS_IF(a) {if(!(a)) break;}
#define FAIL_IF(a) {if(a) break;}
#define END_TEST }while(false);

//------------------------------------------------------------------------------
// Configuration defines used by hal_config.h
//------------------------------------------------------------------------------

//#define ENABLE      (0xFF)
//#define DISABLE     (0x00)
#define FLG_SHORT               0x01    // Used for Ri Short Read
#define FLG_NOSTOP             0x02    // Don't release IIC Bus
#define FLG_CONTD               0x04    // Continued from previous operation
#define FLG_WADDR              0x08   // reg addr is word type
#define FLG_9679I2C             0x10   // 9679 MCU I2C

typedef enum
{
    rcSUCCESS,
    rcERROR,
    rcINVALID
}
eRESULT;

typedef enum _SiiResultCodes_t
{
    SII_SUCCESS      = 0,           // Success.
    SII_ERR_FAIL,                   // General failure.
    SII_ERR_INVALID_PARAMETER,      //
    SII_ERR_IN_USE,                 // Module already initialized.
    SII_ERR_NOT_AVAIL,              // Allocation of resources failed.
} SiiResultCodes_t;

typedef void *SiiInst_t;

#define SI_ENABLE       TRUE
#define SI_DISABLE      FALSE





// ---------------------------------------------------------------------------
// Definitions for key types
// ---------------------------------------------------------------------------
#define HALUICTRL_TYPE_KEYPAD       0
#define HALUICTRL_TYPE_IR           1


#define PRD_TX_STANBY_DELY  1
#define PRD_NO_SIGNAL_TX_STANDBY  1
#endif  // __SI_DATATYPES_H__
