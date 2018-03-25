
#ifndef _IR_H_
#define _IR_H_
#include "si_datatypes.h"       
#define IR_INDEXDUMMY         	0xff

#define	IR_DATA_LENGTH		   32
#define	IR_DEVICE_LENGTH           16
#define	IR_DEVICE_CODE		   0xff40 
#define	IR_DEVICE_CODE_htx	   0xb44b
#define	IR_DUMMY	           0xff    

//Pulse mesurement
#define	IR_LEADER_LOW		   90
#define	IR_LEADER_HIGH		   45

#define	IR_LOGIC0_LOW 		   6
#define	IR_LOGIC0_HIGH		   5
#define	IR_LOGIC1_LOW 		   6
#define	IR_LOGIC1_HIGH		   17

#define	IR_REPEAT_LOW		   90  // // 9.04 ms
#define	IR_REPEAT_HIGH		   22  // 2.24 ms

#define	IR_ENDPULSE		   6

//Operator define
// 13.5 ms
#define	IR_LEADER_MAX			(IR_LEADER_LOW+IR_LEADER_HIGH+10)//IR_LOGIC0_HIGH==IR_LOGIC1_HIGH    135
#define	IR_LEADER_MIN			(IR_LEADER_LOW+IR_LEADER_HIGH-10)//IR_LOGIC0_HIGH==IR_LOGIC1_HIGH    125

#define	IR_LOGIC0_MAX			(IR_LOGIC0_LOW+IR_LOGIC0_HIGH+4) //IR_LOGIC0_HIGH==IR_LOGIC1_HIGH    15
#define	IR_LOGIC0_MIN			(IR_LOGIC0_LOW+IR_LOGIC0_HIGH-4)//IR_LOGIC0_HIGH==IR_LOGIC1_HIGH     7

#define	IR_LOGIC1_MAX			(IR_LOGIC1_LOW+IR_LOGIC1_HIGH+4) //IR_LOGIC0_HIGH==IR_LOGIC1_HIGH   //27
#define	IR_LOGIC1_MIN			(IR_LOGIC1_LOW+IR_LOGIC1_HIGH-4)//IR_LOGIC0_HIGH==IR_LOGIC1_HIGH    //19
// 11.2 ms 
#define	IR_REPEAT_MAX			(IR_REPEAT_LOW+IR_REPEAT_HIGH+5)//IR_LOGIC0_HIGH==IR_LOGIC1_HIGH   //  117
#define	IR_REPEAT_MIN			(IR_REPEAT_LOW+IR_REPEAT_HIGH-5)//IR_LOGIC0_HIGH==IR_LOGIC1_HIGH   // 107
//efine	IR_AUTOREPEAT_RESET	1100     // 110 ms   real is 108 ms 
/* for we use 200 ( unit is 0.1 ms) to measure so when this value is 
    1100 then 1100/200 = 5  the real value is 1000 this may be cause repeat fail
   _by kyq 20040421
  */
#define	IR_AUTOREPEAT_RESET	1200     // 110 ms   real is 108 ms 

// 红外外部中断
#define  INT_IR_PRE_PRTY                       0
#define  INT_IR_SUB_PRTY                       0

void  IREdgeInterrupt(void) ;
void  IRTimerInt(void);
//void IR_received(void);
bool_t IRInit(void);
#endif

