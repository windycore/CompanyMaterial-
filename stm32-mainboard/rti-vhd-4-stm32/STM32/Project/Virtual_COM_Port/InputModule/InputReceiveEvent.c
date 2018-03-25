#include "InputReceiveEvent.h"
#include "si_datatypes.h"
#include "MainApp.h"
#include "InputHandle.h"
#include "si_timer.h"
#include "sii_api.h"
	
bool_t InputVideoProcessHandler (void);
bool_t InputVideoRxMsgHandler (SendData_t *pMsg );


 bool_t InputInit(void)
 {
    SiiPlatformTimerSet(etINPUT_HDCP,mS_TO_TICKS(1000) ,0xFF);   // 1s循环一次，重新设置时间
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
static bool_t halInputVideoInitialize(void)
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
   BYTE     cIndex = 0;
   unsigned char InEdidValue[MAX_IN]={0};

   #if 1
   if(!SiiPlatformTimerExpired(etEDIDPROC_HPD))
   {
	   return usedMessage; 		// time not enougth
   }
   #endif
    /* ---------------------------- END CUSTOM AREA --------------------------- */
    if (utilExecMsgPreRead(mqINPUT))
    {
	   utilExecMsgDataRead(mqINPUT,
                        &sInputEvent);
        /* Add code here to perform a post read operation on the user input queue */
          utilExecMsgClear(mqINPUT);
        /* ---------------------------- END CUSTOM AREA --------------------------- */
    }
  else 
    return usedMessage;

   usedMessage=true;
    switch ( sInputEvent.Head.opcode )
   {
	case  eInputHpdProc:
		   if(sInputEvent.Head.argCount==MAX_IN+1)			//所有输入端口要进行热插拔的拉低再拉高动作
		   {
			   if(sInputEvent.args[MAX_IN]==1)				//所有输入端口要进行热插拔的拉低再拉高动作
		           {
                             for(cIndex=0;cIndex<MAX_IN;cIndex++)
                             {
                                     SiiSetRXHpdLow2High(cIndex);
                             }
                         }
		   }
		   else  if(sInputEvent.Head.argCount==MAX_IN+2)			//Uart或GUI管理时的某些输入端口要进行热插拔的拉低再拉高动作
		   {
			   if(sInputEvent.args[MAX_IN]==MAX_IN)						//Uart或GUI管理时的某些输入端口要进行热插拔的拉低再拉高动作
		           {
				 GetUartInEdidValue(InEdidValue);
				 for(cIndex=0;cIndex<MAX_IN;cIndex++)
                             {
					if(InEdidValue[cIndex]!=0xff)
					{
						InEdidValue[cIndex]= 0xff;
						SiiSetRXHpdLow2High(cIndex);
					}
                             }
				 ClearUartInEdidValue();
                         }
		   }
		   else if(sInputEvent.Head.argCount==MAX_IN)
		   {
			   for(cIndex=0;cIndex<MAX_IN;cIndex++)
			   {
			   	   if(sInputEvent.args[cIndex]==1)			//判断哪个输入端口需要进行热插拔的拉低再拉高动作
			   	   {
					   sInputEvent.args[cIndex]=0;
					   SiiSetRXHpdLow2High(cIndex);
				   }
			   }
		   }
		   memset(&sInputEvent.args[0],0,MAX_CMD_SIZE);
		   break;

        default:
            usedMessage = false;                        // Didn't use the message
            break;
    }

    return( usedMessage );


}
 
bool_t InputVideoProcessHandler (void)
{      
 	appInputHDCPCompatible();
	appInputPortDialSwitchEdidDataDetect();
        return true;
}












