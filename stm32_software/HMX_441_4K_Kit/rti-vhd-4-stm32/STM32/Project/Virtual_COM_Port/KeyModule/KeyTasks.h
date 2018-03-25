// ---------------------------------------------------------------------------
// 
//
// $RCSfile: halUICtrlAPI.h,v $
// $Revision: 1.0 $
// $Author: tding $
// $Date: 20150808 9:15:54] $
//
/* ---------------- BEGIN CUSTOM AREA [000] halUICtrlAPI.h -------------------
 * Custom File Revision Information                                         */
/* ---------------------------- END CUSTOM AREA --------------------------- */

#ifndef _HALUICTRLAPI_H_
#define _HALUICTRLAPI_H_

#include "si_Common.h"
#include "MainApp.h"

// ---------------------------------------------------------------------------
// Definitions for key types
// ---------------------------------------------------------------------------
#define HALUICTRL_TYPE_KEYPAD       0
#define HALUICTRL_TYPE_IR           1

// ---------------------------------------------------------------------------
// Key event condition flags
// ---------------------------------------------------------------------------
#define KEY_PRESS          (1 << 0)
#define KEY_HOLD           (1 << 1)
#define KEY_REPEAT         (1 << 2)
#define KEY_RELEASE        (1 << 3)
#define KEY_REL_AFTER      (1 << 4)
// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************
typedef enum
{
    /* CONFIGURATION: Add user input events here */
    keNONE,
    // keep order
    keIN1,
    keIN2,
    keIN3,
    keIN4,
    keIN5,
    keIN6,
    keIN7,
    keIN8,
    keOUT1,
    keOUT2,
    keOUT3,
    keOUT4,
    keOUT5,
    keOUT6,
    keOUT7,
    keOUT8,
    keALL,
    keEDID,
    keCLEAR,
    keENTER,
    keEDID_LONG,
    keENTER_LONG,
    keStandBy,
    // RTI ADD BELOW CODE, must be keep order  
    ke1TO1, // 24
    ke1TO2,
    ke1TO3,
    ke1TO4,
    ke1TO5,
    ke1TO6,
    ke1TO7,
    ke1TO8,
    ke2TO1, // 32
    ke2TO2,
    ke2TO3,
    ke2TO4,
    ke2TO5,
    ke2TO6,
    ke2TO7,
    ke2TO8,
    ke3TO1, // 40
    ke3TO2,
    ke3TO3,
    ke3TO4,
    ke3TO5,
    ke3TO6,
    ke3TO7,
    ke3TO8,
    ke4TO1, // 48
    ke4TO2,
    ke4TO3,
    ke4TO4,
    ke4TO5,
    ke4TO6,
    ke4TO7,
    ke4TO8,
    // RTI ADD BELOW CODE, must be keep order 
    ke5TO1, // 56
    ke5TO2,
    ke5TO3,
    ke5TO4,
    ke5TO5,
    ke5TO6,
    ke5TO7,
    ke5TO8,
    ke6TO1, // 64
    ke6TO2,
    ke6TO3,
    ke6TO4,
    ke6TO5,
    ke6TO6,
    ke6TO7,
    ke6TO8,
    ke7TO1, // 72
    ke7TO2,
    ke7TO3,
    ke7TO4,
    ke7TO5,
    ke7TO6,
    ke7TO7,
    ke7TO8,
    ke8TO1, // 80
    ke8TO2,
    ke8TO3,
    ke8TO4,
    ke8TO5,
    ke8TO6,
    ke8TO7,
    ke8TO8,
    
    ke1TOALL, // 88
    ke2TOALL,
    ke3TOALL,
    ke4TOALL,
#ifdef MODIFY_IR_SWITCH_2017_2_21
	ke5TOALL, // 88
	ke6TOALL,
	ke7TOALL,
	ke8TOALL,
#endif
    keInOut1_LONG,

    ke1DelayTO1, // 93
    ke1DelayTO2,
    ke1DelayTO3,
    ke1DelayTO4,
    ke1DelayTO5,
    ke1DelayTO6,
    ke1DelayTO7,
    ke1DelayTO8,
    ke2DelayTO1, // 101
    ke2DelayTO2,
    ke2DelayTO3,
    ke2DelayTO4,
    ke2DelayTO5,
    ke2DelayTO6,
    ke2DelayTO7,
    ke2DelayTO8,
    ke3DelayTO1, // 109
    ke3DelayTO2,
    ke3DelayTO3,
    ke3DelayTO4,
    ke3DelayTO5,
    ke3DelayTO6,
    ke3DelayTO7,
    ke3DelayTO8,
    ke4DelayTO1, // 117
    ke4DelayTO2,
    ke4DelayTO3,
    ke4DelayTO4,
    ke4DelayTO5,
    ke4DelayTO6,
    ke4DelayTO7,
    ke4DelayTO8,
    ke5DelayTO1, // 125
    ke5DelayTO2,
    ke5DelayTO3,
    ke5DelayTO4,
    ke5DelayTO5,
    ke5DelayTO6,
    ke5DelayTO7,
    ke5DelayTO8,
    ke6DelayTO1, // 133
    ke6DelayTO2,
    ke6DelayTO3,
    ke6DelayTO4,
    ke6DelayTO5,
    ke6DelayTO6,
    ke6DelayTO7,
    ke6DelayTO8,
    ke7DelayTO1, // 141
    ke7DelayTO2,
    ke7DelayTO3,
    ke7DelayTO4,
    ke7DelayTO5,
    ke7DelayTO6,
    ke7DelayTO7,
    ke7DelayTO8,
    ke8DelayTO1, // 149
    ke8DelayTO2,
    ke8DelayTO3,
    ke8DelayTO4,
    ke8DelayTO5,
    ke8DelayTO6,
    ke8DelayTO7,
    ke8DelayTO8,

/* ---------------------------- END CUSTOM AREA --------------------------- */
    keINVALID                          // used for enumerated type range
                                       // checking (DO NOT REMOVE)
} eKEY;



// ---------------------------------------------------------------------------
// User input event structure (key and repeat count)
// ---------------------------------------------------------------------------
typedef struct
{
    eKEY                eKey;          // user input event
    BYTE                cCount;        // event repeat count
    BYTE                cType;         // 0:keypad 1:IR
} KEY_EVENT, *PKEY_EVENT;

// ---------------------------------------------------------------------------
// Key event lookup table entry structure
// ---------------------------------------------------------------------------
typedef struct
{
    WORD                wKeycode;      // keycode received from input device
    BYTE                cConditions;   // bitfield of key event condition
                                       // flags
    eKEY                ePressKey;     // key event generated by PRESS
                                       // condition
    eKEY                eHoldKey;      // key event generated by HOLD or
                                       // REPEAT condition
    eKEY                eReleaseKey;   // key event generated by RELEASE
                                       // condition
    eKEY                eRelAfterKey;  // key event generated by RELEASE_AFTER
                                       // condition
    BYTE                cHoldCount;    // threshold for HOLD or REPEAT event
    BYTE                cRelCount;     // threshold for RELEASE or
                                       // RELEASE_AFTER event
} KEY_LOOKUP;
typedef const KEY_LOOKUP *PCKEY_LOOKUP;

// ---------------------------------------------------------------------------
// This API handles timer events for keypad polling.
//
// Params:
//  eEvent : Type of event
//  cIdent : Event identifier
//
// Return: none
// ---------------------------------------------------------------------------
bool_t halUICtrlEventHandler(void);

// ---------------------------------------------------------------------------
// This API is the interrupt handler for specified infrared receiver.
//
// Params:
//  cIRNum : IR Receiver receiving the interrupt
//
// Return: none
// ---------------------------------------------------------------------------

void halUICtrlKeyEnalbe(BOOL  Enable);
bool_t Init_Key_module(BYTE MsgId);

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
void KeyLedStatuesInitialize(void);
void KeyLedSetAllClose(void);
void KeyLedSetAll(PBYTE ledStatus);
void KeyLedSetOne(BYTE tIndex,BYTE tledStatus);
void KeyLedCloseSetOne(BYTE tIndex,BYTE tledStatus);
BYTE GetKeyLedCloseStatus(BYTE tIndex);
void Set_CloseOutputFlag(BYTE tIndex,BYTE tstatus);
BYTE Get_CloseOutputFlag(BYTE tIndex);
#endif

typedef enum _KeyOpcodes_t       // CEC Messages
{
    // sLEDx_FLICKER   = 0x100,
   //     sIR_Ctrl= 0x200,
   //switch  Messages  = 0x300
   // Key  Messages  =0x400
       eKeyLock=0x400,
       eInputKeyLock,
}KeyOpcodes_t;

// ---------------------------------------------------------------------------
// User input event structure (key and repeat count)
// ---------------------------------------------------------------------------
typedef struct
{
    bool_t  Enable;          // Demo  executive 
    
} Key_LOCK_EVENT, *PKey_LOCK_EVENT;

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
extern BYTE KeyInput[8];
#endif

bool_t KeyInitialize(void);
bool_t KeyRxMsgHandler (SendData_t *pMsg );
bool_t halUICtrlEventHandler(void);

#endif // _HALUICTRLAPI_H_


