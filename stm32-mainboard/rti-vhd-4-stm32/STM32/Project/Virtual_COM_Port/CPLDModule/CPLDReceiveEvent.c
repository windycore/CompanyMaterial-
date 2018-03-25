#include "CPLDReceiveEvent.h"
#include "si_datatypes.h"
#include "MainApp.h"


	
bool_t CpldProcessHandler (void);
bool_t Cpld_ProcessEvent( SendData_t *events );
bool_t CpldRxMsgHandler (SendData_t *pMsg );





bool_t Cpld_ProcessEvent( SendData_t *events )
{
     uint8_t *pMsg;
 /*   if ( (pMsg = osal_msg_receive( mqLED )) != NULL )
    {
        LedRxMsgHandler( (SendData_t *)pMsg );
      // Release the OSAL message
       osal_msg_deallocate( pMsg );
    }*/
     return true;
}
bool_t CpldRxMsgHandler (SendData_t *pMsg )
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



bool_t CpldProcessHandler (void)
{



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
static bool_t halcpldInitialize(void)
{
       CpldInit();
        dvCpldClose();
//#ifdef VHD4_CPLD    // kyq 20151126 ask by dajie
       dvCpldOpenMCUToDB9(1);
//#endif
  return true;

}


bool_t Init_cpld_module(BYTE MsgId)
{
    bool_t      success = true;
  //注册LED 灯功能
  if(!halcpldInitialize())
  	 success = false;  
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,CpldRxMsgHandler ,true))   //注册事件
  	 success = false; 
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,CpldProcessHandler ,true))  //注册任务
  	 success = false; 
  return success;
}









