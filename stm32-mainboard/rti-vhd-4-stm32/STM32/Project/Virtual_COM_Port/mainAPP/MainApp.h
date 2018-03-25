#if !defined __APP_MAIN_H__
#define __APP_MAIN_H__
#include "si_datatypes.h"
#include "dv74ls138.h"
/*************************************/
#include "LedReceiveEvent.h"
#include "LcdReceiveEvent.h"
#include "IRTasks.h"
#include "UartRs232Tasks.h"
#include "InputReceiveEvent.h"
#include "CPLDReceiveEvent.h"
#include "OutputReceiveEvent.h"
/******************************************/
#include "utilDataMgrAPI.h"
#include "appSystemMgrAPI.h"

#define MAX_CMD_SIZE   16
#define MAX_MAINAPP_TASKS               12  // Number of CEC Task handlers that can
#define MAX_MAINAPP_PARSERS           MAX_MAINAPP_TASKS   // 最大处理进程模块
// Task ID not initialized
#define NO_TASK_ID 0xFF

/*
typedef enum _SiiCecDeviceTypes_t   // CEC device types
{
    CEC_DT_TV,
    CEC_DT_RECORDING_DEVICE,
    CEC_DT_RSVD,
    CEC_DT_TUNER,
    CEC_DT_PLAYBACK,
    CEC_DT_AUDIO_SYSTEM,
    CEC_DT_PURE_CEC_SWITCH,
    CEC_DT_VIDEO_PROCESSOR,

    CEC_DT_COUNT
} SiiCecDeviceTypes_t;
*/
typedef enum _System_Opcodes_t       // Messages
    {
    //SWITCH_CHANEL             = 0x100,    // Special opcode for SiiDrvCpiWrite()
          fagag=0
    } System_Opcodes_t;


typedef enum _SendComandState_t
{
    IDLE          = 0,
    WAITCMD,
    SENDING,
    SENDACKED,
    SENDFAILED,
    TIMEOUT
} SendComandState_t;



//-------------------------------------------------------------------------------
// CPI data structures
//-------------------------------------------------------------------------------

typedef struct _CommandStatus_t
{
    uint8_t         rxState;  
    SendComandState_t txState; 
    uint8_t         Error;
    uint16_t        msgId;

} CommandStatus_t;
//-------------------------------------------------------------------------------
// send data structures
//-------------------------------------------------------------------------------

typedef struct _SendData_t
{
    uint8_t         srcDestAddr;            // Source in upper nibble, dest in lower nibble
    System_Opcodes_t opcode;
    uint8_t         args[ MAX_CMD_SIZE ];
    uint8_t         argCount;
    //SiiCpiTxState_t txState;
} SendData_t;

typedef bool_t (*SendMsgHandler)( SendData_t* pMsg);

typedef struct _MainAppMsgHandlers_t
{
    bool_t      callAlways;
    eMESSAGE_QUEUES DriverMsgId;
    bool_t      (*pMsgHandler)( SendData_t * );
	
} MainAppMsgHandlers_t;

typedef struct _MainAppTask_t
{
    bool_t      callAlways;
    eMESSAGE_QUEUES DriverMsgId;
    bool_t       (*pTasks)( );  // registed by SiiCecCallbackRegisterTask()
	
} MainAppTask_t;
//------------------------------------------------------------------------------
//  APP Component Instance Data
//------------------------------------------------------------------------------

typedef struct _MainAppInstanceData_t
{
   
    // External message handlers
    // 根据模块注册不同的消息处理函数
    MainAppMsgHandlers_t messageHandlers[MAX_MAINAPP_PARSERS]; 

    // External Task Handlers

    MainAppTask_t  TasksHandlers[MAX_MAINAPP_TASKS]; 

}	MainAppInstanceData_t;


bool_t Init_UerUART_module(BYTE MsgId);
bool_t Init_Lcd_module(BYTE MsgId);
bool_t Init_NVRAM_module(BYTE MsgId);
bool_t Init_Key_module(BYTE MsgId);
bool_t Init_Led_module(BYTE MsgId);
bool_t Init_IR_module(BYTE MsgId);
bool_t Init_cpld_module(BYTE MsgId);
bool_t Init_InputVideo_module(BYTE MsgId);
bool_t Init_SwitchVideo_module(BYTE MsgId);
bool_t Init_OutPutVideo_module(BYTE MsgId);

bool_t  AppModuleRegisterInit(void );
void AppTaskMsgLoop (  void );
void AppTasKeyLedMsgLoop (  void  );
bool_t AppCallbackRegisterParser (eMESSAGE_QUEUES  DriverMsgId ,SendMsgHandler pCallBack,   bool_t callAlways);
int_t AppCallbackRegisterTask ( eMESSAGE_QUEUES  DriverMsgId,bool_t (*pCallBack)(), bool_t callAlways );

#endif
