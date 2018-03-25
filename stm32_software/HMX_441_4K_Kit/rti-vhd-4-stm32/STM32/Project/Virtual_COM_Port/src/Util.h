// FILE: Util.h
// power by kyq
// this file will be impleter the util funtion
#ifndef _UTIL_H_
#define _UTIL_H_
#include "si_common.h"
#include "Mcu.h"
#define MAX_CMD_SIZE   16

typedef enum
{
/* -------------- BEGIN CUSTOM AREA [060] utilExecutiveAPI.h -----------------
 * CONFIGURATION: Add message ques here.  NOTE : If message queues are added or
   removed, then corresponding changes must be made to the eMutex definition */
    mqUSER_INPUT,
    mqLED    ,
    mqKEY  ,
    mqUART  ,
    mqCPLD   ,
    mqIR ,
    mqLCD,
    mqINPUT  ,
    mqSWITCH  ,
    mqOUTPUT ,
    mqNVRAM,
/* ---------------------------- END CUSTOM AREA --------------------------- */
    mqINVALID                          // Used for enumerated type range
                                       // checking (DO NOT REMOVE)
} eMESSAGE_QUEUES;
// ---------------------------------------------------------------------------
// Status information structure for message queues (read and write indices)
// ---------------------------------------------------------------------------
typedef struct
{
    BYTE                cWriteIndex;   // index where last entry was written
                                       // to queue
    BYTE                cReadIndex;    // index where next entry will be read
                                       // from queue
    BYTE                cQueueEmpty;   // flag indicating that queue is empty
} MSG_QUEUE_INFO, *PMSG_QUEUE_INFO;

// ---------------------------------------------------------------------------
// Status information structure for message queues (read and write indices)
// ---------------------------------------------------------------------------
typedef struct
{
    BYTE                *pWrite;   // index where last entry was written
                                       // to queue
    BYTE                *pRead;    // index where next entry will be read
                                       // from queue
    BYTE                cEmpty;   // flag indicating that queue is empty
} MSG_QUEUE_Information, *PMSG_QUEUE_Information;
// ---------------------------------------------------------------------------
// Configuration information for message queues (size and location)
// ---------------------------------------------------------------------------
typedef struct
{
    BYTE                cQLength;      // number of messages in queue
    BYTE                cMsgSize;      // message size in bytes
    WORD                wOffset;       // start of queue in memory
} MSG_QUEUE_CONFIG;
typedef const MSG_QUEUE_CONFIG *PCMSG_QUEUE_CONFIG;

// ---------------------------------------------------------------------------
// This type defines all of the possible system power states.
// ---------------------------------------------------------------------------
typedef enum
{
    pwrON,                             // Full power all system components
                                       // active.
    pwrSTANDBY,                        // off screen blank but otherwise
                                       // all active.
    //pwrSUSPEND,                        // DPMS low power mode slow wake
   //                                    // latency or TBD.
    pwrOFF,                            // Power down mode.
    pwrSOFTOFF,                        // Lowest power while monitoring
                                       // inputs.
    pwrSOFTRESET,                      // Restart after soft reset.
/* --------------- BEGIN CUSTOM AREA [005] utilPowerTypes.h ------------------
*/
/* ---------------------------- END CUSTOM AREA --------------------------- */
    pwrINVALID
} ePOWERSTATE;

typedef enum _MasgOpcodes_t       // Messages
{
    SWITCH_CHANEL             = 0x100,    // Special opcode for SiiDrvCpiWrite()

} MasgOpcodes_t;

typedef struct 
{
    uint32_t    	DestAddr;            // Source in upper nibble, dest in lower nibble
    //MasgOpcodes_t opcode;
	uint16_t 		opcode;
	uint8_t         argCount;
} Event_Head_t, *pEvent_Head_t;

typedef struct 
{
    Event_Head_t  Head;
    uint8_t         args[MAX_CMD_SIZE ];
  
} Event_t, *pEvent_t;

typedef enum  eEDIDSELECT_e
{
// Must keep order
// 由g_Init_EdidFlashEdidTable 初始化到NVRAM, 经过修改保存后，开机由NVRAM 读到g_Init_EdidFlashEdidTable
   edsEDID_Default,  // 默认的edid,  没有经过修改的话和原始的g_Init_EdidFlashEdidTable 一样。
   // 输入端口保存的各自EDID数据
   //IN1~IN8 是INTERN1~ EXTERN8 中的一个副本
   edsEDID_IN1,
   edsEDID_IN2,
   edsEDID_IN3,
   edsEDID_IN4,

#if(MAX_IN>4)
   edsEDID_IN5,
   edsEDID_IN6,
#if(MAX_IN>6)
   edsEDID_IN7,
   edsEDID_IN8,
#endif

#endif


    edsEDID_INTERN1,  // 5
    edsEDID_INTERN2,
    edsEDID_INTERN3,
    edsEDID_INTERN4,
    edsEDID_INTERN5,
    edsEDID_INTERN6,
#if ((MACHINETYPE==MUH4K862)||(MACHINETYPE==MHUB4K431))//(MAX_OUT>6)
    edsEDID_INTERN7,
    //edsEDID_INTERN8,
    //edsEDID_EXTERN9,
    //edsEDID_EXTERN10,
#endif

// no nvram save, read from RX when need
   edsEDID_EXTERN1,  // 10
   edsEDID_EXTERN2,
   edsEDID_EXTERN3,
   edsEDID_EXTERN4,
#if(MAX_OUT>4)
   edsEDID_EXTERN5,
   edsEDID_EXTERN6,
#if(MAX_OUT>6)
   edsEDID_EXTERN7,
   edsEDID_EXTERN8,
#endif
#endif
   
/* ---------------------------- END CUSTOM AREA --------------------------- */
    eedsINVALID
} eEDIDSELECT;


//软件延时函数
void delay_nus(uint16_t time);
void delay_nms(uint16_t count);




// ---------------------------------------------------------------------------
// This API sends a message to the specified queue.
//
// Params:
//  eQueue  : Queue to send message to
//  pacData : Pointer to message data
//
// Return: none
// ---------------------------------------------------------------------------
void utilExecMsgSend(eMESSAGE_QUEUES eQueue,
                   PBYTE           pacData);
void utilExecOlnyMsgSend(eMESSAGE_QUEUES eQueue,
                     Event_t      pacData);
 void utilExecListMsgSend(BYTE cPortModules, va_list pArgs,BYTE cMsgTypes,PBYTE   pacData);

// ---------------------------------------------------------------------------
// This API reads a message from the specified queue.
//
// Params:
//  eQueue  : Queue to read message from
//  pacData : Pointer where message data will be stored
//
// Return:
//  rcSUCCESS if a message was read, rcERROR if queue was empty
// ---------------------------------------------------------------------------
eRESULT utilExecMsgRead(eMESSAGE_QUEUES eQueue,
                        PBYTE           pacData);

eRESULT utilExecMsgDataRead(eMESSAGE_QUEUES eQueue,
                        pEvent_t EventData);
// ---------------------------------------------------------------------------
// This API clears all messages from the specified queue.
//
// Params:
//  eQueue : Queue to clear
//
// Return: none
// ---------------------------------------------------------------------------
void utilExecMsgClear(eMESSAGE_QUEUES eQueue);


// ---------------------------------------------------------------------------
// This API checks if a message is present in the specified queue.
//
// Params:
//  eQueue : Queue to check for messages
//
// Return:
//  TRUE if there is a message in the queue, FALSE otherwise
// ---------------------------------------------------------------------------
BOOL utilExecMsgPresent(eMESSAGE_QUEUES eQueue);
BOOL utilExecMsgPreRead(eMESSAGE_QUEUES eQueue);

// 端口号到实例转换
// cPort: from 0
BYTE utilOutputPortToInstance(BYTE cPort);

BYTE utilInstanceToOutputPort(BYTE cInstance);

void utilVariableInit(void);

#endif
