#include "UartRs232Tasks.h"


bool_t UserUARTRxMsgHandler (SendData_t *pMsg )
{
	//pMsg = pMsg;
	return true;
}

// ---------------------------------------------------------------------------
// Process keycode from infrared receiver
//
// Params:
//  wKeycode : keycode
//
// Return: none
// ---------------------------------------------------------------------------
bool_t halUserUARTProcess (void)
{
   Uart1_process();
  return 1;
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
bool_t halUserUARTInitialize(void)
{
	UserUARTinitialize();
	return true;
}

bool_t Init_UerUART_module(BYTE MsgId)
{
    bool_t      success = true;

  if(!halUserUARTInitialize())
  	 success = false;  
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,UserUARTRxMsgHandler ,FALSE))   //注册事件
  	 success = false; 
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,halUserUARTProcess ,FALSE))  //注册任务
  	 success = false; 
  return success;
}