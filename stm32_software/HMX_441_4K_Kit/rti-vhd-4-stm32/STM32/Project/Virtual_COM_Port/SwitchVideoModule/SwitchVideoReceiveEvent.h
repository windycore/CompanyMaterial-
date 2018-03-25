#ifndef _SWITCHREVEIVEREVENT_H_
#define _SWITCHREVEIVEREVENT_H_
#include "si_datatypes.h"
#include "SwitchVideoHandle.h"
#include "util.h"
#include "appUserInterface.h"
#include "MainApp.h"


bool_t Init_SwitchVideo_module(BYTE MsgId);

bool_t halSwitchVideoInitialize(void);
bool_t SwitchVideoRxMsgHandler (SendData_t *pMsg );
bool_t SwitchVideoProcessHandler (void);
/*  eVideoSWITCH_HEX :
Port switch command package length is 13byte:
[0xa5+0x5b+0x02+0x03+input port(1~4)+0x00+output port(1~4)+0x00+0x00+0x00+0x00+0x00+checksum]
All you need to change is just ¡°input port¡±, ¡±output port¡±, ¡±checksum¡±
 
Checksum = 0x100 ¨C (0xa5+0x5b+0x02+0x03+input port+0x00+output port+0x00+0x00+0x00+0x00+0x00)
 
For example: Set output 1 form input 2 command:
A5 5B 02 03 02 00 01 00 00 00 00 00 F8
*/
typedef enum{
	eVideoSWITCH_OFF=0x20,
	eVideoSWITCH_ON,
	eVideoSWITCH_HEX=0x80,
	eVideoSWITCH_INVALID,
}VideoSWITCH_e;


/*
typedef enum _SwitchOpcodes_t      
{
    // sLEDx_FLICKER   = 0x100,
   //     sIR_Ctrl= 0x200,
   //switch  Messages  = 0x300
   eSwitchDemoMode=0x300,
   eSwitchAllThrough,
   eSwitchUndo,
   // Key  Messages  =0x400
    
}SwitchOpcodes_t;
*/
// ---------------------------------------------------------------------------
// User input event structure (key and repeat count)
// ---------------------------------------------------------------------------
typedef struct
{
    bool_t  Enable;          // Demo  executive 
    
} Demo_EVENT, *PDemo_EVENT;

typedef struct
{
    BYTE xInPort;   
    BYTE xOutNum;
    BYTE xOutPort[MAX_OUT];   
} Switch_EVENT, *pSwitch_EVENT;
#endif
