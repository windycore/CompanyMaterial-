#ifndef _LEDREVEIVCE_H_
#define _LEDREVEIVCE_H_
#include "si_datatypes.h"
#include "LedHandle.h"
#include "util.h"
#include "appUserInterface.h"
#include "MainApp.h"

#define MAX_LED_CMD_SIZE   16


typedef struct _LedInstanceData_t
{
   DWORD  tLedValue;  //Lighting 
   u8 LedIntervalTime;   //50ms*LedIntervalTime= real time
   u8 LedCntTime;   //filere 
}LedInstanceData_t;

bool_t Init_Led_module(BYTE MsgId);


typedef enum   _LEDOpcodes_t     // CEC Messages
{
    //*****************************************************************//
    sLEDx_FLICKER   = 0x100,    // Special opcode for SiiDrvCpiWrite()
    sLEDAll_FLICKER ,
    sLEDx_OFF       ,
    sLEDAll_OFF     ,
    sLEDx_ON           ,
    sLEDAll_ON         ,
    sLEDAllLightOut ,  
    sLEDxLightOut,
    eLEDAllThrough,
    eLEDAllOpen,
    eLEDAllClose,
     eLEDxOpen,
    eLEDxClose,
    eLEDSwitchxAll,
    eLEDChannelSwitch,
    sLEDx_ChannelState,
    sLEDx_STOP_FLICKER,
   //     sIR_Ctrl= 0x200,
   //switch  Messages  = 0x300
   // Key  Messages  =0x400
        // UART  Messages  =0x500
}LEDOpcodes_t;


/*
typedef struct
{
    eLED_NAME  LedNum;     
    u8  LedIntervalTime;    //50ms*LedIntervalTime= real time
    u8 LedCout;    
} sLEDxFLICKER_EVENT, *PsLEDxFLICKER_EVENT;
*/
typedef struct 
{
    uint8_t         srcDestAddr;            // Source in upper nibble, dest in lower nibble
    eUI_STATE opcode;
    uint8_t         args[ MAX_LED_CMD_SIZE ];
    uint8_t         argCount;
    //SiiCpiTxState_t txState;
} LED_EVENT, *PLED_EVENT;

/*************************************************************************/
typedef struct
{
    
	union    //common memory  
	 {
	    uint8_t  LedInputPort;  
	    uint8_t    LedOutputPort; 
	 };
    uint8_t  LedxNum;  	
    uint8_t  LedOutputPortList[MAX_OUT];  
} CHANNELSWITCH_EVENT, *pCHANNELSWITCH_EVENT;
#endif