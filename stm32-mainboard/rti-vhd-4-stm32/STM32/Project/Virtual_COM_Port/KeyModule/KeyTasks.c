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
#include "Util.h"
#include "dvPC9539.h"
#include "si_debug.h"
#include "IR.h"
#include "Si_osal_timer.h"
#include "keyhandle.h"
#include "si_timer.h"
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

// ---------------------------------------------------------------------------
// Static data for keypad repeat key processing
// ---------------------------------------------------------------------------
static WORD             m_wLastKeypadKey;
static BYTE             m_cKeypadRptCount;
static BOOL             m_bFirstKeyPressed;
static BOOL m_cKeyEnable= TRUE;
// ***************************************************************************
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
   #if 0
   if(!SiiPlatformTimerExpired(etKEYSCAN))
   {
  	   return 0; // time not enougth
   }
   #endif
	 
    if(SiiPlatformTimerExpired(etUI_ENTER))  //delay time
   {
       wKeycode=keENTER;				//虚拟出按了确定键
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
    PKey_LOCK_EVENT LockEvent;
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
    
    LockEvent=(Key_LOCK_EVENT *)&sKeyEvent.args;
   halUICtrlKeyEnalbe(LockEvent->Enable);
	break;
     case  eInputKeyLock:
  //  PKey_LOCK_EVENT LockEvent;
    LockEvent=(Key_LOCK_EVENT *)&sKeyEvent.args;
   halUICtrlKeyEnalbe(LockEvent->Enable);
	break;
  default:
  	break;

}
    return( usedMessage );


}

        
bool_t KeyProcessHandler (void)
{
   halUICtrlEventHandler(); //key value  obtain
   appUIProcess(); // key operate  fashion
   return true;
}



bool_t Init_Key_module(BYTE MsgId)
{
    bool_t      success = true;
  //注册LED 灯功能
  if(!KeyInitialize())
  	 success = false;  
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,KeyRxMsgHandler ,true))   //注册事件
  	 success = false; 
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,KeyProcessHandler ,true ))  //注册任务,不轮训
  	 success = false; 
  return success;
}
