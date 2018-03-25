#ifndef _HALUSERIR_H_
#define _HALUSERIR_H_
#include "IR.h"
#include "si_datatypes.h"
#include "si_timer.h"
#include "MainApp.h"
#include "KeyHandle.h"
bool_t halIRKeyProcess (void);
void halUICtrlIRKeyEnalbe(bool_t Enable);

//--------------------------------CONFIGURATION-------------------------------
// Minimum time between packets and keycode received when key is released
//--------------------------------CONFIGURATION-------------------------------
#define IR_PACKET_PERIOD    100L // 100ms (10Hz) packet rate
#define IR_POLL_PERIOD   100L // 100ms (10Hz) polling rate

#define IR_NO_KEY           0x00

typedef enum _IROpcodes_t      // Messages
{
    // sLEDx_FLICKER   = 0x100,
        eIR_Ctrl= 0x200,
        eIR_Delay,
        eIRLock,
   //switch  Messages  = 0x300
   // Key  Messages  =0x400
     
     
}IROpcodes_t;

// ---------------------------------------------------------------------------
// User input event structure (key and repeat count)
// ---------------------------------------------------------------------------
typedef struct
{
    bool_t  Enable;          // Demo  executive 
    
} IR_LOCK_EVENT, *PIR_LOCK_EVENT;

#endif