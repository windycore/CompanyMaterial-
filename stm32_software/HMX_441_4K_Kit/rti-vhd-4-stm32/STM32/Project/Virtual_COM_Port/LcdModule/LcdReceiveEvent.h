#ifndef _LEDREVEIVCE_H_
#define _LEDREVEIVCE_H_
#include "si_datatypes.h"
#include "LedHandle.h"
#include "util.h"
#include "appUserInterface.h"
#include "OSAL.h"


#define MAX_LED_CMD_SIZE   16


typedef struct _LcdInstanceData_t
{
  //bool_t  *pLedDrivceInit();  
   unsigned char led;
}LcdInstanceData_t;



typedef struct 
{
    uint8_t         srcDestAddr;            // Source in upper nibble, dest in lower nibble
    eUI_STATE opcode;
    uint8_t         args[ MAX_LED_CMD_SIZE ];
    uint8_t         argCount;
    //SiiCpiTxState_t txState;
} LCD_EVENT, *PLCD_EVENT;

bool_t Init_Lcd_module(BYTE MsgId);
bool_t halLcdInitialize(void);
bool_t Lcd_ProcessEvent( SendData_t *events );
bool_t LcdProcessHandler (void);


#endif
