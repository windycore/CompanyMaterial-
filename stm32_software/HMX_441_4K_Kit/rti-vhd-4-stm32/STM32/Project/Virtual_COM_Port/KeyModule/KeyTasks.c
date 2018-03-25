//
// $RCSfile: halUICtrl.c,v $
// $Revision: 1.3 $
// $Author: kong $
// $Date:  20150807 22:35:28] $
//
// ---------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>> COPYRIGHT NOTICE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// This is an unpublished work.
// ---------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>>>>> WARRANTEE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#include "KeyTasks.h"
#include "UtilDataMgrAPI.h"
#include "si_timer.h"
#include "Util.h"
#include "dvPC9539.h"
#include "si_debug.h"
#include "IR.h"
#include "si_debugger_hdmigear.h"
#include "Si_osal_timer.h"
#include "keyhandle.h"
//--------------------------------CONFIGURATION-------------------------------
// Polling period and keycode read when no key is pressed
//--------------------------------CONFIGURATION-------------------------------
#define KEYPAD_POLL_PERIOD   100L // 100ms (10Hz) polling rate

// These values are multipled by KEYPAD_POLL_PERIOD to determine the
// actual keypad polling time, depending on the keypad polling state.
#define KEYPAD_DEFAULT_TIME  1
#define KEYPAD_DEBOUNCE_TIME 2
//--------------------------------CONFIGURATION-------------------------------
// Polling period and max count for key repeat
//--------------------------------CONFIGURATION-------------------------------
#define KEYPAD_REPEAT_TIME   1      // polling period multiple
#define KEYPAD_REPEAT_MAX    255

/* ---------------------------- END CUSTOM AREA --------------------------- */
#if (MACHINETYPE==MUH4K862)
static const KEY_LOOKUP m_asKeypadEventTable[] = {
/* ----------------- BEGIN CUSTOM AREA [020] halUICtrl.c ---------------------
 * CONFIGURATION: Key event lookup table for keypad */
    //
    { ke1DelayTO1, KEY_PRESS,       			 ke1DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO2, KEY_PRESS,                    ke1DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO3, KEY_PRESS,                    ke1DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO4, KEY_PRESS,                    ke1DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO5, KEY_PRESS,                    ke1DelayTO5,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO6, KEY_PRESS,                    ke1DelayTO6,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO7, KEY_PRESS,                    ke1DelayTO7,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO8, KEY_PRESS,                    ke1DelayTO8,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO1, KEY_PRESS,                    ke2DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO2, KEY_PRESS,                    ke2DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO3, KEY_PRESS,                    ke2DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO4, KEY_PRESS,                    ke2DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO5, KEY_PRESS,                    ke2DelayTO5,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO6, KEY_PRESS,                    ke2DelayTO6,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO7, KEY_PRESS,                    ke2DelayTO7,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO8, KEY_PRESS,                    ke2DelayTO8,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO1, KEY_PRESS,                    ke3DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO2, KEY_PRESS,                    ke3DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO3, KEY_PRESS,                    ke3DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO4, KEY_PRESS,                    ke3DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO5, KEY_PRESS,                    ke3DelayTO5,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO6, KEY_PRESS,                    ke3DelayTO6,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO7, KEY_PRESS,                    ke3DelayTO7,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO8, KEY_PRESS,                    ke3DelayTO8,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO1, KEY_PRESS,                    ke4DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO2, KEY_PRESS,                    ke4DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO3, KEY_PRESS,                    ke4DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO4, KEY_PRESS,                    ke4DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO5, KEY_PRESS,                    ke4DelayTO5,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO6, KEY_PRESS,                    ke4DelayTO6,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO7, KEY_PRESS,                    ke4DelayTO7,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO8, KEY_PRESS,                    ke4DelayTO8,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke5DelayTO1, KEY_PRESS,                    ke5DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke5DelayTO2, KEY_PRESS,                    ke5DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke5DelayTO3, KEY_PRESS,                    ke5DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke5DelayTO4, KEY_PRESS,                    ke5DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke5DelayTO5, KEY_PRESS,                    ke5DelayTO5,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke5DelayTO6, KEY_PRESS,                    ke5DelayTO6,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke5DelayTO7, KEY_PRESS,                    ke5DelayTO7,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke5DelayTO8, KEY_PRESS,                    ke5DelayTO8,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke6DelayTO1, KEY_PRESS,                    ke6DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke6DelayTO2, KEY_PRESS,                    ke6DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke6DelayTO3, KEY_PRESS,                    ke6DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke6DelayTO4, KEY_PRESS,                    ke6DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke6DelayTO5, KEY_PRESS,                    ke6DelayTO5,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke6DelayTO6, KEY_PRESS,                    ke6DelayTO6,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke6DelayTO7, KEY_PRESS,                    ke6DelayTO7,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke6DelayTO8, KEY_PRESS,                    ke6DelayTO8,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke7DelayTO1, KEY_PRESS,                    ke7DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke7DelayTO2, KEY_PRESS,                    ke7DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke7DelayTO3, KEY_PRESS,                    ke7DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke7DelayTO4, KEY_PRESS,                    ke7DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke7DelayTO5, KEY_PRESS,                    ke7DelayTO5,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke7DelayTO6, KEY_PRESS,                    ke7DelayTO6,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke7DelayTO7, KEY_PRESS,                    ke7DelayTO7,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke7DelayTO8, KEY_PRESS,                    ke7DelayTO8,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke8DelayTO1, KEY_PRESS,                    ke8DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke8DelayTO2, KEY_PRESS,                    ke8DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke8DelayTO3, KEY_PRESS,                    ke8DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke8DelayTO4, KEY_PRESS,                    ke8DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke8DelayTO5, KEY_PRESS,                    ke8DelayTO5,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke8DelayTO6, KEY_PRESS,                    ke8DelayTO6,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke8DelayTO7, KEY_PRESS,                    ke8DelayTO7,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke8DelayTO8, KEY_PRESS,                    ke8DelayTO8,     keNONE,     keNONE, keNONE, 0, 0 },// 
    //{ keOUT1,      KEY_PRESS,       			 keOUT1,     keNONE,     keNONE, keNONE, 0, 0 },
    //{ keOUT2,      KEY_PRESS,                    keOUT2,     keNONE,     keNONE, keNONE, 0, 0 },
    //{ keOUT3,      KEY_PRESS,                    keOUT3,     keNONE,     keNONE, keNONE, 0, 0 },
    //{ keOUT4,      KEY_PRESS,                    keOUT4,     keNONE,     keNONE, keNONE, 0, 0 },
    //{ keOUT5,      KEY_PRESS,       			 keOUT5,     keNONE,     keNONE, keNONE, 0, 0 },
    //{ keOUT6,      KEY_PRESS,                    keOUT6,     keNONE,     keNONE, keNONE, 0, 0 },
    //{ keOUT7, KEY_PRESS,                    keOUT7,     keNONE,     keNONE, keNONE, 0, 0 },
    //{ keOUT8, KEY_PRESS,                    keOUT8,     keNONE,     keNONE, keNONE, 0, 0 },
    //{ keALL, KEY_PRESS,                     keALL,      keNONE,     keNONE, keNONE, 0, 0 },
    { keEDID, KEY_PRESS | KEY_HOLD,         keEDID,     keEDID_LONG,keNONE, keNONE, 10, 0 },
    //{ keCLEAR, KEY_PRESS,                   keCLEAR,    keNONE,     keNONE, keNONE, 0, 0 },
    { keENTER, KEY_PRESS | KEY_HOLD,        keENTER,    keENTER_LONG,keNONE, keNONE, 10, 0 },
    { keNONE, KEY_PRESS | KEY_HOLD,         keNONE,     keInOut1_LONG,keNONE, keNONE, 110, 0 },
/* ---------------------------- END CUSTOM AREA --------------------------- */
};
#elif (MACHINETYPE==MHUB4K431)
static const KEY_LOOKUP m_asKeypadEventTable[] = {
/* ----------------- BEGIN CUSTOM AREA [020] halUICtrl.c ---------------------
 * CONFIGURATION: Key event lookup table for keypad */
    //
    { ke1DelayTO1, KEY_PRESS,       			 ke1DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO2, KEY_PRESS,                    ke1DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO3, KEY_PRESS,                    ke1DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke1DelayTO4, KEY_PRESS,                    ke1DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO1, KEY_PRESS,                    ke2DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO2, KEY_PRESS,                    ke2DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO3, KEY_PRESS,                    ke2DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke2DelayTO4, KEY_PRESS,                    ke2DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO1, KEY_PRESS,                    ke3DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO2, KEY_PRESS,                    ke3DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO3, KEY_PRESS,                    ke3DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke3DelayTO4, KEY_PRESS,                    ke3DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO1, KEY_PRESS,                    ke4DelayTO1,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO2, KEY_PRESS,                    ke4DelayTO2,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO3, KEY_PRESS,                    ke4DelayTO3,     keNONE,     keNONE, keNONE, 0, 0 },// 
    { ke4DelayTO4, KEY_PRESS,                    ke4DelayTO4,     keNONE,     keNONE, keNONE, 0, 0 },// 
    //{ keALL, KEY_PRESS,                     keALL,      keNONE,     keNONE, keNONE, 0, 0 },
    { keEDID, KEY_PRESS | KEY_HOLD,         keEDID,     keEDID_LONG,keNONE, keNONE, 10, 0 },
    //{ keCLEAR, KEY_PRESS,                   keCLEAR,    keNONE,     keNONE, keNONE, 0, 0 },
    { keENTER, KEY_PRESS | KEY_HOLD,        keENTER,    keENTER_LONG,keNONE, keNONE, 10, 0 },
    { keNONE, KEY_PRESS | KEY_HOLD,         keNONE,     keInOut1_LONG,keNONE, keNONE, 110, 0 },
/* ---------------------------- END CUSTOM AREA --------------------------- */
};
#else
static const KEY_LOOKUP m_asKeypadEventTable[] = {
/* ----------------- BEGIN CUSTOM AREA [020] halUICtrl.c ---------------------
 * CONFIGURATION: Key event lookup table for keypad */
    //
    { keIN1, KEY_PRESS,       			    keIN1 ,      keNONE,   keNONE, keNONE, 0, 0 },// 
    { keIN2, KEY_PRESS,                           keIN2,        keNONE,   keNONE, keNONE, 0, 0 },// 
    { keIN3, KEY_PRESS,                           keIN3,        keNONE,   keNONE, keNONE, 0, 0 },
    { keIN4, KEY_PRESS,                           keIN4,        keNONE,   keNONE, keNONE, 0, 0 },
    { keIN5, KEY_PRESS,       			    keIN5 ,      keNONE,   keNONE, keNONE, 0, 0 },// 
    { keIN6, KEY_PRESS,                           keIN6,        keNONE,   keNONE, keNONE, 0, 0 },// 
    { keIN7, KEY_PRESS,                           keIN7,        keNONE,   keNONE, keNONE, 0, 0 },
    { keIN8, KEY_PRESS,                           keIN8,        keNONE,   keNONE, keNONE, 0, 0 },
    { keOUT1, KEY_PRESS,       			   keOUT1,      keNONE,   keNONE, keNONE, 0, 0 },
    { keOUT2, KEY_PRESS,                           keOUT2,     keNONE,   keNONE, keNONE, 0, 0 },
    { keOUT3, KEY_PRESS,                           keOUT3,     keNONE,   keNONE, keNONE, 0, 0 },
    { keOUT4, KEY_PRESS,                           keOUT4,     keNONE,   keNONE, keNONE, 0, 0 },
    { keOUT5, KEY_PRESS,       			   keOUT5,      keNONE,   keNONE, keNONE, 0, 0 },
    { keOUT6, KEY_PRESS,                           keOUT6,     keNONE,   keNONE, keNONE, 0, 0 },
    { keOUT7, KEY_PRESS,                           keOUT7,     keNONE,   keNONE, keNONE, 0, 0 },
    { keOUT8, KEY_PRESS,                           keOUT8,     keNONE,   keNONE, keNONE, 0, 0 },
    { keALL, KEY_PRESS,                           keALL,        keNONE,   keNONE, keNONE, 0, 0 },
    { keEDID, KEY_PRESS | KEY_HOLD,        keEDID,     keEDID_LONG,   keNONE, keNONE, 10, 0 },
    { keCLEAR, KEY_PRESS,                           keCLEAR,   keNONE,   keNONE, keNONE, 0, 0 },
    { keENTER, KEY_PRESS | KEY_HOLD,                keENTER,   keENTER_LONG,   keNONE, keNONE, 10, 0 },
    { keNONE, KEY_PRESS | KEY_HOLD,                keNONE,   keInOut1_LONG,   keNONE, keNONE, 110, 0 },
/* ---------------------------- END CUSTOM AREA --------------------------- */
};
#endif
// ---------------------------------------------------------------------------
// Static data for keypad repeat key processing
// ---------------------------------------------------------------------------
static WORD             m_wLastKeypadKey;
static BYTE             m_cKeypadRptCount;
static BOOL             m_bFirstKeyPressed;
static BOOL m_cKeyEnable;
// ***************************************************************************
#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
BYTE KeyInput[8]={0,0,0,0,0,0,0,0};
BYTE CloseKeyInput[8]={0,0,0,0,0,0,0,0};
BYTE CloseOutputFlag[8]={false,false,false,false,false,false,false,false};
#endif

// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
// ---------------------------------------------------------------------------
// Process a key press or key hold input and write a key event to the input
// queue if the input matches a condition in the key lookup table.
//
// Params:
//  pcRptCount  : pointer to repeat key counter
//  psKeyLookup : pointer to key lookup table entry for key
//  bRepeat     : TRUE if the key is held down, FALSE otherwise
//
// Return:
//  True: a meaningful key is pressed. FALSE: none key is pressed
// ---------------------------------------------------------------------------
static BYTE KeyPressProcess(PCBYTE       pcRptCount,
                            PCKEY_LOOKUP psKeyLookup,
                            BOOL         bRepeat,
                            BOOL         bIRKey)
{
    KEY_EVENT           sKeyEvent;
    // Save whether the key is IR or keypad
    sKeyEvent.cType = bIRKey ? HALUICTRL_TYPE_IR :
                               HALUICTRL_TYPE_KEYPAD;
    if (bRepeat)
    {
        if (psKeyLookup->cConditions & KEY_REPEAT &&
            *pcRptCount > psKeyLookup->cHoldCount)
        {
            sKeyEvent.eKey = psKeyLookup->eHoldKey;
            sKeyEvent.cCount = *pcRptCount - psKeyLookup->cHoldCount;
            if(m_cKeyEnable)
          {
            utilExecMsgSend(mqUSER_INPUT,
                            (BYTE *)&sKeyEvent);
          }
        }
        else if (psKeyLookup->cConditions & KEY_HOLD &&
                 *pcRptCount == psKeyLookup->cHoldCount)
        {
            sKeyEvent.eKey = psKeyLookup->eHoldKey;
            sKeyEvent.cCount = 0;
  
            if(m_cKeyEnable)
          
           {
            utilExecMsgSend(mqUSER_INPUT,
                            (BYTE *)&sKeyEvent);
           }
        }
    }
    else
    {
        if (psKeyLookup->cConditions & KEY_PRESS)
        {
            sKeyEvent.eKey = psKeyLookup->ePressKey;
            sKeyEvent.cCount = 0;
            if(m_cKeyEnable)
 
          {
            utilExecMsgSend(mqUSER_INPUT,
                            (BYTE *)&sKeyEvent);
          }
            m_bFirstKeyPressed = FALSE;//[kyq add: 20080306 18:15:03]
        }
    }
    return (sKeyEvent.eKey != keNONE);
}
// ---------------------------------------------------------------------------
// Process a key release input and write a key event to the input queue if the
// input matches a condition in the key lookup table.
//
// Params:
//  pcRptCount  : pointer to repeat key counter
//  psKeyLookup : pointer to key lookup table entry for key
//
// Return: none
// ---------------------------------------------------------------------------
static void KeyReleaseProcess(PCBYTE       pcRptCount,
                              PCKEY_LOOKUP psKeyLookup,
                              BOOL         bIRKey)
{
    KEY_EVENT           sKeyEvent;
    // Save whether the key is IR or keypad
    sKeyEvent.cType = bIRKey ? HALUICTRL_TYPE_IR :
                               HALUICTRL_TYPE_KEYPAD;
    // check for RELEASE condition
    if ((psKeyLookup->cConditions & KEY_RELEASE) &&
        (*pcRptCount <= psKeyLookup->cRelCount))
    {
        sKeyEvent.eKey = psKeyLookup->eReleaseKey;
        sKeyEvent.cCount = 0;
        if(m_cKeyEnable)
           
       {
        utilExecMsgSend(mqUSER_INPUT,
                        (BYTE *)&sKeyEvent);
      }
    }
    // check for RELEASE_AFTER condition
    else if ((psKeyLookup->cConditions & KEY_REL_AFTER) &&
             (*pcRptCount > psKeyLookup->cRelCount))
    {
        sKeyEvent.eKey = psKeyLookup->eRelAfterKey;
        sKeyEvent.cCount = 0;
        if(m_cKeyEnable)          
       {
        utilExecMsgSend(mqUSER_INPUT,
                        (BYTE *)&sKeyEvent);
       }
    }
}

// ---------------------------------------------------------------------------
// This API sets up the internal state of component.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
bool_t KeyInitialize(void)
{
    // initialize static data
  
    m_wLastKeypadKey = KEYPAD_NO_KEY;
    m_cKeypadRptCount = 0;
    m_bFirstKeyPressed = TRUE;
    // set up initial keypad poll timer event
SiiPlatformTimerSet(etKEYSCAN,mS_TO_TICKS(KEYPAD_POLL_PERIOD)/25,0xFF ); // use time2: 25ms per tick

 return 1;
}
//#pragma argsused
// ---------------------------------------------------------------------------
// This API handles timer events for keypad polling.
//
// Params:
//  eEvent : Type of event
//  cIdent : Event identifier
//
// Return: none
// ---------------------------------------------------------------------------
// 如果放在中断中调用要考虑资源冲突问题
bool_t halUICtrlEventHandler(void)
{
    BYTE                cIndex;
    WORD                wKeycode = KEYPAD_NO_KEY;
    BYTE                cDebounce;
    eRESULT           eResult = rcSUCCESS;
 /* Add code here to read GPIO field(s) and assemble keycode */
 // if(0 == gTimerCounters[etKEYSCAN])
    if(!SiiPlatformTimerExpired(etKEYSCAN))
    {
        return 0; // time not enougth
    }
    if(SkMonUartInProcess())
    {
        SiiPlatformTimerSet(etKEYSCAN,mS_TO_TICKS(KEYPAD_POLL_PERIOD)/25,0xFF );// delay some time  // use time2: 25ms per tick
        return 0;    
    }
	 
    if(SiiPlatformTimerExpired(etUI_ENTER))  //delay time
    {
        wKeycode=keENTER;
    }
    else
        wKeycode =(WORD) Key_Scan(); 			// 获取按键值

	
    cDebounce = KEYPAD_DEFAULT_TIME;
    cIndex = 0;
    if (KEYPAD_NO_KEY != wKeycode)
    {
        if (wKeycode == m_wLastKeypadKey)
        {
            if (m_cKeypadRptCount < KEYPAD_REPEAT_MAX)
            {
                m_cKeypadRptCount ++;
            }
            cDebounce = KEYPAD_REPEAT_TIME;
        }
        else 
        {
            m_cKeypadRptCount = 0;
            cDebounce = KEYPAD_DEBOUNCE_TIME;
        }
        while (cIndex < (sizeof(m_asKeypadEventTable) / sizeof(KEY_LOOKUP)))
        {
            if (m_asKeypadEventTable[cIndex].wKeycode == wKeycode)
            {
                KeyPressProcess(&m_cKeypadRptCount,
                                &m_asKeypadEventTable[cIndex],
                                m_cKeypadRptCount > 0,
                                FALSE);
                break;
            }
            cIndex++;
        }
    }
    else
    {
        m_cKeypadRptCount = 0;
        if (KEYPAD_NO_KEY != m_wLastKeypadKey)
        {
            while (cIndex <
                   (sizeof(m_asKeypadEventTable) / sizeof(KEY_LOOKUP)))
            {
                if (m_asKeypadEventTable[cIndex].wKeycode == m_wLastKeypadKey)
                {
                    KeyReleaseProcess(&m_cKeypadRptCount,
                                      &m_asKeypadEventTable[cIndex],
                                      FALSE);
                    break;
                }
                cIndex++;
            }
        }
    }
    m_wLastKeypadKey = wKeycode;
    // set up next keypad poll timer event
    SiiPlatformTimerSet(etKEYSCAN,mS_TO_TICKS(KEYPAD_POLL_PERIOD) * cDebounce/25,0xFF); // use time2: 25ms per tick
    return 1;
}
void halUICtrlKeyEnalbe(BOOL  Enable)
{
   m_cKeyEnable = Enable;
}

bool_t KeyRxMsgHandler (SendData_t *pMsg )
{

    bool_t  usedMessage=false     ;
    Event_t sKeyEvent;
          /* ---------------------------- END CUSTOM AREA --------------------------- */
    if (utilExecMsgPreRead(mqKEY))
    {
       utilExecMsgDataRead(mqKEY,
                        &sKeyEvent);
        /* Add code here to perform a post read operation on the user input queue */
    //    utilExecMsgClear(mqKEY);
        /* ---------------------------- END CUSTOM AREA --------------------------- */
    }
  else 
    return usedMessage;

//  ASSERT((pMsg->srcDestAddr & mqSWITCH ) == mqSWITCH);  // no check address of the module

    usedMessage         = true;
    switch(sKeyEvent.Head.opcode)
    {
        case  eKeyLock:
            {
                PKey_LOCK_EVENT LockEvent;
                LockEvent=(Key_LOCK_EVENT *)&sKeyEvent.args;
                halUICtrlKeyEnalbe(LockEvent->Enable);
            }
            break;
        case  eInputKeyLock:
            {
                PKey_LOCK_EVENT LockEvent;
                LockEvent=(Key_LOCK_EVENT *)&sKeyEvent.args;
                halUICtrlKeyEnalbe(LockEvent->Enable);
            }
            break;
        default:
            break;

    }
    return( usedMessage );


}

bool_t Init_Key_module(BYTE MsgId)
{
    bool_t      success = true;
  //注册LED 灯功能
  if(!KeyInitialize())
  	 success = false;  
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,KeyRxMsgHandler ,true))   //注册事件
  	 success = false; 
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,halUICtrlEventHandler ,false ))  //注册任务,不轮训
  	 success = false; 
  return success;
}

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
extern void Get_LastInPortSelect(PBYTE pValue);
extern void Get_CloseInputPort(PBYTE pValue,PBYTE pValue2);
extern void Set_Tm1623ValueChangeFlag(bool_t nflag);
void KeyLedStatuesInitialize(void)
{
    Get_LastInPortSelect(KeyInput);
    Set_Tm1623ValueChangeFlag(true);
    Get_CloseInputPort(CloseKeyInput,CloseOutputFlag);
    #if 0
    if (m_eNextPowerState == pwrSTANDBY)
    {
        set_clear_standby_flag(1);
    }
    else if (m_eNextPowerState == pwrOFF || m_eNextPowerState == pwrSOFTOFF)
    {
        set_clear_standby_flag(1);
    }
    #endif
}

void KeyLedSetAllClose(void)
{
    BYTE i;
    for(i=0;i<8;i++)
    {
        KeyInput[i] = 0xFF;
    }
    Set_Tm1623ValueChangeFlag(true);
}

void KeyLedSetAll(PBYTE ledStatus)
{
    BYTE i;
    for(i=0;i<8;i++)
        KeyInput[i] = ledStatus[i];
    Set_Tm1623ValueChangeFlag(true);
}

void KeyLedSetOne(BYTE tIndex,BYTE tledStatus)
{
    KeyInput[tIndex] = tledStatus;
    Set_Tm1623ValueChangeFlag(true);
}

void KeyLedCloseSetOne(BYTE tIndex,BYTE tledStatus)
{
    CloseKeyInput[tIndex] = tledStatus;
}

BYTE GetKeyLedCloseStatus(BYTE tIndex)
{
    return CloseKeyInput[tIndex];
}

void Set_CloseOutputFlag(BYTE tIndex,BYTE tstatus)
{
    CloseOutputFlag[tIndex] = tstatus;
}

BYTE Get_CloseOutputFlag(BYTE tIndex)
{
    return CloseOutputFlag[tIndex];
}
#endif


