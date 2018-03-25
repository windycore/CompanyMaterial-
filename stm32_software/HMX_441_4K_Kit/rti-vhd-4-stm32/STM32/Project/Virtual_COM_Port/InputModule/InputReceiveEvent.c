#include "InputReceiveEvent.h"
#include "si_datatypes.h"
#include "MainApp.h"
#include "appSystemMgrAPI.h"

//LedInstanceData_t LedGlobal;

//static uint8_t registeredLedsTaskID = NO_TASK_ID;
	
bool_t InputVideoProcessHandler (void);
bool_t InputVideoRxMsgHandler (SendData_t *pMsg );


 bool_t InputInit(void)
 {
       dv9679AdapterSystemInit();
	   return true;
 }



// ***************************************************************************
// ---------------------------------------------------------------------------
// This API sets up the internal state of component.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
bool_t halInputVideoInitialize(void)
{
     InputInit();
	 return true;
}

bool_t Init_InputVideo_module(BYTE MsgId)
{
    bool_t      success = true;

  if(!halInputVideoInitialize())
  	 success = false;  
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,InputVideoRxMsgHandler ,TRUE))   //注册事件
  	 success = false; 
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,InputVideoProcessHandler ,TRUE))  //注册任务
  	 success = false; 
  return success;
}


bool_t InputVideoRxMsgHandler (SendData_t *pMsg )
{

    bool_t  usedMessage         = false;
   bool_t  isDirectAddressed   = !((pMsg->srcDestAddr & mqINPUT ) == mqINPUT);
   Event_t sInputEvent;
          /* ---------------------------- END CUSTOM AREA --------------------------- */
    if (utilExecMsgPreRead(mqINPUT))
    {
       utilExecMsgDataRead(mqINPUT,
                        &sInputEvent);
        /* Add code here to perform a post read operation on the user input queue */
       // utilExecMsgClear(mqSWITCH);
        /* ---------------------------- END CUSTOM AREA --------------------------- */
    }
  else 
    return usedMessage;

   usedMessage=true;
    switch ( sInputEvent.Head.opcode )
   {
  /*      case sLEDx_FLICKER:
			
			break;
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
     	case  eLEDAllThrough:
		  LedGlobal.LedCntTime=3*2;  //
		  LedGlobal.LedIntervalTime=50;  //
		   break;*/
        default:
            usedMessage = false;                        // Didn't use the message
            break;
    }

    return( usedMessage );


}


 static BYTE cSii9679LoopCNT = 0;
 
bool_t InputVideoProcessHandler (void)
{      
 /*      
         BYTE cInstance = 0;
           if (cSii9679LoopCNT++ > 3)
        {
            for (cInstance = 0; cInstance < INSTANCE_9679; cInstance++)
            {
                dvSiiDrvAdaptTaskExecute(cInstance);
            }
            cSii9679LoopCNT = 0;
        }
*/
    //   appClearInputHDCP(); //no signal set HDCP Clear
        appInputHDCPCompatible();
/*		
    if(!SiiPlatformTimerExpired(etINPUTSCAN))
    {
        return 0; // time not enougth
    }
*/
//    appInputHDCPCheck(); //HDCP check
 //   SiiPlatformTimerSet(etINPUTSCAN,mS_TO_TICKS(INPUT_POLL_PERIOD)/25,0xFF );// delay some time  // use time2: 25ms per tick
 	return true;
}












