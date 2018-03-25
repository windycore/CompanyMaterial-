#ifndef _SWITCHREVEIVEREVENT_H_
#define _SWITCHREVEIVEREVENT_H_
#include "si_datatypes.h"
#include "SwitchVideoHandle.h"
#include "util.h"
#include "appUserInterface.h"
#include "MainApp.h"


bool_t Init_SwitchVideo_module(BYTE MsgId);


typedef enum _SwitchOpcodes_t      
{
    // sLEDx_FLICKER   = 0x100,
   //     sIR_Ctrl= 0x200,
   //
    eSwitchDemoMode=0x300,
   eSwitchAllThrough,
   eSwitchxThrough,
   eSwitchAllOpen,
   eSwitchAllClose,
      eSwitchxOpen,
   eSwitchxClose,
   eSwitchxAll,
   eSwitchxTox,
  eSwitchUndo,
   // UART  Messages  =0x500    
}SwitchOpcodes_t;

// ---------------------------------------------------------------------------
// User input event structure (key and repeat count)
// ---------------------------------------------------------------------------
typedef struct
{
    bool_t  Enable;          // Demo  executive 
    
} Demo_EVENT, *PDemo_EVENT;

typedef struct
{
   union{
    BYTE xInPort;   
	BYTE xOutputPort;
   	};
    BYTE xOutNum;
    BYTE xOutPort[MAX_OUT];   
} Switch_EVENT, *pSwitch_EVENT;
#endif