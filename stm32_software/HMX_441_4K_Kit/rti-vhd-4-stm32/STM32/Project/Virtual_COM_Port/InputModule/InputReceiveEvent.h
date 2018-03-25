#ifndef _INPUTREVEIVCE_H_
#define _INPUTREVEIVCE_H_
#include "si_app_adapter_rx.h"
#include "MainApp.h"

#define INPUT_POLL_PERIOD   3000L // 3s (10Hz) polling rate

bool_t Init_InputVideo_module(BYTE MsgId);
bool_t halInputVideoInitialize(void);
bool_t InputVideoRxMsgHandler (SendData_t *pMsg );
bool_t InputVideoProcessHandler (void);

#endif
