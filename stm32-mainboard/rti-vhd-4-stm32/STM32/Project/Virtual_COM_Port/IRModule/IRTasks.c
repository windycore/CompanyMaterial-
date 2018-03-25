
#include "IRhandle.h"
#include "IRTasks.h"
#include "KeyTasks.h"
#include "Si_osal_timer.h"

static const KEY_LOOKUP m_asIREventTable[] = {
/* ----------------- BEGIN CUSTOM AREA [030] halUICtrl.c ---------------------
 * CONFIGURATION: Key event lookup table for infrared remote */
#if  ( MACHINETYPE == MUH44E_BYE )

	{0x04, KEY_PRESS, keIN1,   	keNONE, keNONE, keNONE, 0, 0},
	{0x47, KEY_PRESS, keIN2,   	keNONE, keNONE, keNONE, 0, 0},
	{0x5A, KEY_PRESS, keIN3,	keNONE, keNONE, keNONE, 0, 0},
	{0x02, KEY_PRESS, keIN4,   	keNONE, keNONE, keNONE, 0, 0},

	{0x1F, KEY_PRESS, keOUT1,   keNONE, keNONE, keNONE, 0, 0},
	{0x0F, KEY_PRESS, keOUT2,	keNONE, keNONE, keNONE, 0, 0},
	{0x58, KEY_PRESS, keOUT3,   keNONE, keNONE, keNONE, 0, 0},
	{0x15, KEY_PRESS, keOUT4,	keNONE, keNONE, keNONE, 0, 0},

	{0x06, KEY_PRESS, keALL,    keNONE, keNONE, keNONE, 0, 0},
	//{0xE0, KEY_PRESS, keEDID,	keNONE, keNONE, keNONE, 0, 0},
	{0x45, KEY_PRESS, keCLEAR,  keNONE, keNONE, keNONE, 0, 0},
	//{0x70, KEY_PRESS, keENTER,	keNONE, keNONE, keNONE, 0, 0},
	{0x07, KEY_PRESS|KEY_HOLD, keEDID,  keEDID_LONG, keNONE, keNONE, 10, 0},
	{0x0E, KEY_PRESS|KEY_HOLD, keENTER,	keENTER_LONG, keNONE, keNONE, 10, 0},
	{0x42, KEY_PRESS, keStandBy,keNONE, keNONE, keNONE, 0, 0},
	
#elif  (MACHINETYPE==MUH88E_BYE)

	{0x04, KEY_PRESS, keIN1,   	keNONE, keNONE, keNONE, 0, 0},
	{0x46, KEY_PRESS, keIN2,   	keNONE, keNONE, keNONE, 0, 0},
	{0x47, KEY_PRESS, keIN3,	keNONE, keNONE, keNONE, 0, 0},
	{0x5A, KEY_PRESS, keIN4,   	keNONE, keNONE, keNONE, 0, 0},
	{0x43, KEY_PRESS, keIN5,   	keNONE, keNONE, keNONE, 0, 0},
	{0x02, KEY_PRESS, keIN6,   	keNONE, keNONE, keNONE, 0, 0},
	{0x03, KEY_PRESS, keIN7,	keNONE, keNONE, keNONE, 0, 0},
	{0x41, KEY_PRESS, keIN8,   	keNONE, keNONE, keNONE, 0, 0},

	{0x1F, KEY_PRESS, keOUT1,   keNONE, keNONE, keNONE, 0, 0},
	{0x5D, KEY_PRESS, keOUT2,	keNONE, keNONE, keNONE, 0, 0},
	{0x0F, KEY_PRESS, keOUT3,   keNONE, keNONE, keNONE, 0, 0},
	{0x58, KEY_PRESS, keOUT4,	keNONE, keNONE, keNONE, 0, 0},
	{0x59, KEY_PRESS, keOUT5,   keNONE, keNONE, keNONE, 0, 0},
	{0x15, KEY_PRESS, keOUT6,	keNONE, keNONE, keNONE, 0, 0},
	{0x54, KEY_PRESS, keOUT7,   keNONE, keNONE, keNONE, 0, 0},
	{0x0A, KEY_PRESS, keOUT8,	keNONE, keNONE, keNONE, 0, 0},
	
	{0x06, KEY_PRESS, keALL,    keNONE, keNONE, keNONE, 0, 0},
	//{0xE0, KEY_PRESS, keEDID,	keNONE, keNONE, keNONE, 0, 0},
	{0x45, KEY_PRESS, keCLEAR,  keNONE, keNONE, keNONE, 0, 0},
	//{0x70, KEY_PRESS, keENTER,	keNONE, keNONE, keNONE, 0, 0},
	{0x07, KEY_PRESS|KEY_HOLD, keEDID,  keEDID_LONG, keNONE, keNONE, 10, 0},
	{0x0E, KEY_PRESS|KEY_HOLD, keENTER,	keENTER_LONG, keNONE, keNONE, 10, 0},
	{0x42, KEY_PRESS, keStandBy,keNONE, keNONE, keNONE, 0, 0},
	
#elif  (MACHINETYPE==MUH44A_H2)

	{0x04, KEY_PRESS, keIN1,   	keNONE, keNONE, keNONE, 0, 0},
	{0x46, KEY_PRESS, keIN2,   	keNONE, keNONE, keNONE, 0, 0},
	{0x47, KEY_PRESS, keIN2,	keNONE, keNONE, keNONE, 0, 0},
	{0x5A, KEY_PRESS, keIN3,   	keNONE, keNONE, keNONE, 0, 0},
	{0x43, KEY_PRESS, keIN5,   	keNONE, keNONE, keNONE, 0, 0},
	{0x02, KEY_PRESS, keIN4,   	keNONE, keNONE, keNONE, 0, 0},
	{0x03, KEY_PRESS, keIN7,	keNONE, keNONE, keNONE, 0, 0},
	{0x41, KEY_PRESS, keIN8,   	keNONE, keNONE, keNONE, 0, 0},

	{0x1F, KEY_PRESS, keOUT1,   keNONE, keNONE, keNONE, 0, 0},
	{0x5D, KEY_PRESS, keOUT2,	keNONE, keNONE, keNONE, 0, 0},
	{0x0F, KEY_PRESS, keOUT2,   keNONE, keNONE, keNONE, 0, 0},
	{0x58, KEY_PRESS, keOUT3,	keNONE, keNONE, keNONE, 0, 0},
	{0x59, KEY_PRESS, keOUT5,   keNONE, keNONE, keNONE, 0, 0},
	{0x15, KEY_PRESS, keOUT4,	keNONE, keNONE, keNONE, 0, 0},
	{0x54, KEY_PRESS, keOUT7,   keNONE, keNONE, keNONE, 0, 0},
	{0x0A, KEY_PRESS, keOUT8,	keNONE, keNONE, keNONE, 0, 0},
	
	{0x06, KEY_PRESS, keALL,    keNONE, keNONE, keNONE, 0, 0},
	{0x45, KEY_PRESS, keCLEAR,  keNONE, keNONE, keNONE, 0, 0},
	//{0x07, KEY_PRESS|KEY_HOLD, keEDID,  keEDID_LONG, keNONE, keNONE, 10, 0},
	{0x07, KEY_PRESS, keEDID,	keNONE, keNONE, keNONE, 0, 0},
	{0x0E, KEY_PRESS|KEY_HOLD, keENTER,	keENTER_LONG, keNONE, keNONE, 10, 0},
	{0x42, KEY_PRESS, keStandBy,keNONE, keNONE, keNONE, 0, 0},

#elif  (MACHINETYPE==MUH88A_H2)

	{0x04, KEY_PRESS, keIN1,   	keNONE, keNONE, keNONE, 0, 0},
	{0x46, KEY_PRESS, keIN2,   	keNONE, keNONE, keNONE, 0, 0},
	{0x47, KEY_PRESS, keIN3,	keNONE, keNONE, keNONE, 0, 0},
	{0x5A, KEY_PRESS, keIN4,   	keNONE, keNONE, keNONE, 0, 0},
	{0x43, KEY_PRESS, keIN5,   	keNONE, keNONE, keNONE, 0, 0},
	{0x02, KEY_PRESS, keIN6,   	keNONE, keNONE, keNONE, 0, 0},
	{0x03, KEY_PRESS, keIN7,	keNONE, keNONE, keNONE, 0, 0},
	{0x41, KEY_PRESS, keIN8,   	keNONE, keNONE, keNONE, 0, 0},

	{0x1F, KEY_PRESS, keOUT1,   keNONE, keNONE, keNONE, 0, 0},
	{0x5D, KEY_PRESS, keOUT2,	keNONE, keNONE, keNONE, 0, 0},
	{0x0F, KEY_PRESS, keOUT3,   keNONE, keNONE, keNONE, 0, 0},
	{0x58, KEY_PRESS, keOUT4,	keNONE, keNONE, keNONE, 0, 0},
	{0x59, KEY_PRESS, keOUT5,   keNONE, keNONE, keNONE, 0, 0},
	{0x15, KEY_PRESS, keOUT6,	keNONE, keNONE, keNONE, 0, 0},
	{0x54, KEY_PRESS, keOUT7,   keNONE, keNONE, keNONE, 0, 0},
	{0x0A, KEY_PRESS, keOUT8,	keNONE, keNONE, keNONE, 0, 0},
	
	{0x06, KEY_PRESS, keALL,    keNONE, keNONE, keNONE, 0, 0},
	{0x45, KEY_PRESS, keCLEAR,  keNONE, keNONE, keNONE, 0, 0},
	//{0x07, KEY_PRESS|KEY_HOLD, keEDID,  keEDID_LONG, keNONE, keNONE, 10, 0},
	{0x07, KEY_PRESS, keEDID,	keNONE, keNONE, keNONE, 0, 0},
	{0x0E, KEY_PRESS|KEY_HOLD, keENTER,	keENTER_LONG, keNONE, keNONE, 10, 0},
	{0x42, KEY_PRESS, keStandBy,keNONE, keNONE, keNONE, 0, 0},
    // RTI ADD THOSE CODE
    
#elif (MACHINETYPE==PRD_VHD4)

	{0x19, KEY_PRESS, ke1TO1,   	keNONE, keNONE, keNONE, 0, 0},
	{0x78, KEY_PRESS, ke1TO2,   	keNONE, keNONE, keNONE, 0, 0},
	{0x1C, KEY_PRESS, ke1TO3,	    keNONE, keNONE, keNONE, 0, 0},
	{0x1D, KEY_PRESS, ke1TO4,   	keNONE, keNONE, keNONE, 0, 0},
	{0x24, KEY_PRESS, ke2TO1,   	keNONE, keNONE, keNONE, 0, 0},
	{0x25, KEY_PRESS, ke2TO2,   	keNONE, keNONE, keNONE, 0, 0},
	{0x26, KEY_PRESS, ke2TO3,	    keNONE, keNONE, keNONE, 0, 0},
	{0x27, KEY_PRESS, ke2TO4,   	keNONE, keNONE, keNONE, 0, 0},
	{0x2D, KEY_PRESS, ke3TO1,   	keNONE, keNONE, keNONE, 0, 0},
	{0x2E, KEY_PRESS, ke3TO2,   	keNONE, keNONE, keNONE, 0, 0},
	{0x2F, KEY_PRESS, ke3TO3,	    keNONE, keNONE, keNONE, 0, 0},
	{0x30, KEY_PRESS, ke3TO4,   	keNONE, keNONE, keNONE, 0, 0},
	{0x35, KEY_PRESS, ke4TO1,   	keNONE, keNONE, keNONE, 0, 0},
	{0x36, KEY_PRESS, ke4TO2,   	keNONE, keNONE, keNONE, 0, 0},
	{0x37, KEY_PRESS, ke4TO3,	    keNONE, keNONE, keNONE, 0, 0},
	{0x38, KEY_PRESS, ke4TO4,   	keNONE, keNONE, keNONE, 0, 0},
	{0x75, KEY_PRESS, ke1TOALL,   	keNONE, keNONE, keNONE, 0, 0},
	{0x11, KEY_PRESS, ke2TOALL,   	keNONE, keNONE, keNONE, 0, 0},
	{0x74, KEY_PRESS, ke3TOALL,	    keNONE, keNONE, keNONE, 0, 0},
	{0x73, KEY_PRESS, ke4TOALL,   	keNONE, keNONE, keNONE, 0, 0},
	
#endif
/* ---------------------------- END CUSTOM AREA --------------------------- */
};


// ---------------------------------------------------------------------------
// Static data for IR remote repeat key processing
// ---------------------------------------------------------------------------
static WORD             m_wLastIRKey;
static BYTE             m_cIRRptCount;
static BOOL             m_cIRKeyEnable = TRUE;
static BOOL m_bFirstIRKeyPressed;
/************************************************************
extern data
****************************************************************/
extern BYTE	 IRKey;


static bool_t halUserIRInitialize(void);
static bool_t IRRxMsgHandler (SendData_t *pMsg );
static BYTE IRKeyPressProcess(PCBYTE       pcRptCount,
                            PCKEY_LOOKUP psKeyLookup,
                            BOOL         bRepeat,
                            BOOL         bIRKey);

                            
bool_t Init_IR_module(BYTE MsgId)
{
    bool_t      success = true;
  //注册LED 灯功能
  if(!halUserIRInitialize())
  	 success = false;  
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,IRRxMsgHandler ,true))   //注册事件
  	 success = false; 
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,halIRKeyProcess ,true))  //注册任务
  	 success = false; 
  return success;
}

// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************
// ---------------------------------------------------------------------------
// This API sets up the internal state of component.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
static bool_t halUserIRInitialize(void)
{
    // initialize static data
    m_wLastIRKey = 0;
    m_cIRRptCount = 0;
     m_cIRKeyEnable = TRUE;
    IRinitialize();
  // SiiPlatformTimerSet(etKEYSCAN,mS_TO_TICKS(KEYPAD_POLL_PERIOD)/25,0xFF ); // use time2: 25ms per tick
   SiiPlatformTimerSet(etIRSCAN,mS_TO_TICKS(IR_POLL_PERIOD*2),0xFF);
    return true;
}


bool_t IRRxMsgHandler (SendData_t *pMsg )
{

    bool_t  usedMessage=false     ;
    Event_t sIREvent;
    PIR_LOCK_EVENT LockEvent;
          /* ---------------------------- END CUSTOM AREA --------------------------- */
    if (utilExecMsgPreRead(mqIR))
    {
       utilExecMsgDataRead(mqIR,
                        &sIREvent);
        /* Add code here to perform a post read operation on the user input queue */
    //    utilExecMsgClear(mqKEY);
        /* ---------------------------- END CUSTOM AREA --------------------------- */
    }
  else 
    return usedMessage;

//  ASSERT((pMsg->srcDestAddr & mqSWITCH ) == mqSWITCH);  // no check address of the module

    usedMessage         = true;
    switch(sIREvent.Head.opcode)
    {
       case  eIRLock:
        LockEvent=(IR_LOCK_EVENT *)&sIREvent.args;
        halUICtrlIRKeyEnalbe(LockEvent->Enable);
            break;
    
    
      default:
            break;
  
    }
    return( usedMessage );


}


// ---------------------------------------------------------------------------
// Process keycode from infrared receiver
//
// Params:
//  wKeycode : keycode
//
// Return: none
// ---------------------------------------------------------------------------
bool_t halIRKeyProcess (void)
{
    WORD wKeycode = IRKey;
	BYTE                cIndex = 0;
	if(!(SiiPlatformTimerExpired(etIRSCAN)))
	   return 0;
	
    IRKey = IR_DUMMY;
   
    if(wKeycode!=IR_DUMMY)
    {
	    if (wKeycode == m_wLastIRKey &&( !SiiTimerExpired(etIR_REPEAT)))
	    {
	        m_cIRRptCount += m_cIRRptCount < 255 ? 1 : 0;
	    }
	    else
	    {
	        m_cIRRptCount = 0;
	    }
		
    	    while (cIndex < (sizeof(m_asIREventTable) / sizeof(KEY_LOOKUP)))
	    {
	        if (m_asIREventTable[cIndex].wKeycode == wKeycode)
	        {
	            IRKeyPressProcess(&m_cIRRptCount,
	                            &m_asIREventTable[cIndex],
	                            m_cIRRptCount > 0,
	                            TRUE);
	            SiiPlatformTimerSet(etIR_REPEAT, mS_TO_TICKS((IR_PACKET_PERIOD*10)),0xFF);
	           
	            break;
	        }
	        cIndex++;
	    }
    }
    m_wLastIRKey = wKeycode;
	SiiPlatformTimerSet(etIRSCAN,mS_TO_TICKS(IR_POLL_PERIOD*2),0xFF);
  return 1;
}


void halUICtrlIRKeyEnalbe(bool_t Enable)
{
   m_cIRKeyEnable = Enable;
}

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
static BYTE IRKeyPressProcess(PCBYTE       pcRptCount,
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
            if(m_cIRKeyEnable)
          {
            utilExecMsgSend(mqUSER_INPUT,
                            (BYTE *)&sKeyEvent);
          }
        }
        else if (psKeyLookup->cConditions & KEY_HOLD &&
                  *pcRptCount > psKeyLookup->cHoldCount)
        {
            sKeyEvent.eKey = psKeyLookup->eHoldKey;
            sKeyEvent.cCount = 0;
  
            if(m_cIRKeyEnable)
          
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
            if(m_cIRKeyEnable)
 
          {
            utilExecMsgSend(mqUSER_INPUT,
                            (BYTE *)&sKeyEvent);
          }
            m_bFirstIRKeyPressed = FALSE;//[kyq add: 20080306 18:15:03]
        }
    }
    return (sKeyEvent.eKey != keNONE);
}

