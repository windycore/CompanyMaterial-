
#ifndef _UARTRS232_H_
#define _UARTRS232_H_
#include "si_datatypes.h"
#include "MainApp.h"
#include "Uart.h"

bool_t Init_UerUART_module(BYTE MsgId);

bool_t halUserUARTInitialize(void);
bool_t UserUARTRxMsgHandler (SendData_t *pMsg );
bool_t halUserUARTProcess (void);

#endif
