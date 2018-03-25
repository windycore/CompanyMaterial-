
#include "CPLDReceiveEvent.h"
#include "si_datatypes.h"
#include "MainApp.h"
#include "sii_common.h"
#include "sii_api.h"
#include "hal.h"
#include "OutputHandle.h"
#include "OutputReceiveEvent.h"


bool_t OutPutVideoProcessHandler (void);
bool_t OutPutVideoRxMsgHandler (SendData_t *pMsg );


bool_t OutPutVideoRxMsgHandler (SendData_t *pMsg )
{

    bool_t  usedMessage         = true;
   bool_t  isDirectAddressed   = !((pMsg->srcDestAddr & mqLCD ) == mqLCD);
    static LED_EVENT sLedEvent;
    // check if user input control has a new keypress
    if (utilExecMsgPresent(mqLCD))
    {
      utilExecMsgRead(mqLCD,
                      (BYTE *)&sLedEvent);
        //DEBUG_PRINT(MSG_ERR, "***USER_INPUT = %d\r\n",  sKeyEvent.eKey);
 /* Add code here to perform a post read operation on the user input queue */
        utilExecMsgClear(mqLCD);


/* ---------------------------- END CUSTOM AREA --------------------------- */
    }
   else
   	return 0;

    switch ( sLedEvent.opcode )
   {
     /*   case sLEDx_FLICKER:
			PsLEDxFLICKER_EVENT   LEDFLICKER;
                     LEDFLICKER=(sLEDxFLICKER_EVENT *)&sLedEvent.args;

			break;*/
	case  sLEDAll_FLICKER:
		   break;
       case sLEDx_OFF:
	   	  dvLedOffSet(1 << (sLedEvent.args[0]));
			break;
	case  sLEDAll_OFF:
		  dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
		   break;
	 case sLEDx_ON:
	 	  dvLedOnSet(1 << (sLedEvent.args[0]));
			break;
	case  sLEDAll_ON:
		    dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
		   break;

        default:
            usedMessage = false;                        // Didn't use the message
            break;
    }

    return( usedMessage );


}



bool_t OutPutVideoProcessHandler (void)
{
	  appInputHDMI14_20DetectAndOutputProcess();
	  appOutputHDCPProcess();
         return true;
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
static bool_t halOutPutVideoInitialize(void)
{
       Init_9396();		//初始化9396
       return true;
}


bool_t Init_OutPutVideo_module(BYTE MsgId)
{
    bool_t      success = true;

  if(!halOutPutVideoInitialize())
  	 success = false;
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,OutPutVideoRxMsgHandler ,true))   //注册事件
  	 success = false;
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,OutPutVideoProcessHandler ,true))  //注册任务
  	 success = false;
  return success;
}









