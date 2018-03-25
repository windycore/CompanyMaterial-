#ifndef _UTILDATAMGRAPI_H_
#define _UTILDATAMGRAPI_H_
#include "mcu.h"
#include "si_datatypes.h"
#include "si_timer.h"
// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
#define NVRAM_UPDATE_DELAY       100 // 15  //zzl 300     // Main Loop Polling interval (ms) //[kyq Begin: 20150717 12:03:04]
// ---------------------------------------------------------------------------
// sentinel value indicating all data manager nodes
// ---------------------------------------------------------------------------
#define UPDATE_ALL_NODEID 					nnINVALID_NODE
#ifdef  FACTORY_RESET
#define FACTORY_DATA_BUFFER                 250
#endif

typedef struct
{
    BYTE  acAudioInPortSelect;
    BYTE  acAudioBass;
    BYTE  acAudioTreble;
    BYTE acAudioLoudness;
    BYTE acAudioMute;
    BYTE acAudioVolume;
} INPUTAUDIO_DATA,*PINPUTAUDIO_DATA;

typedef struct _INPUTRESOLUTIONTYPE_t
{
	WORD HorizPixel;
	WORD VertLine;
	WORD PixelClock;
	BYTE Interlace;
} RESOLUTIONTYPE_t;

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************
typedef struct
{
/*
BYTE  cCONFIG_VALID;
BYTE  cEDID_LOADSEL;
BYTE  cDB_VERSION;
BYTE  cUART_MSG_LEVEL;
BYTE  cEDID_VALID;
*/
    BYTE  acEDID_EEPROM_BOOT_VALID[DevNum];
//BYTE  cPowerStatus;
    BYTE  acProduct_Type[17]; // sys_type
    BYTE  acProduct_Name[17]; // sys_name
    BYTE  acIP_Address[4];    // IP_add
//BOOL bHDCP_AUTO[MAX_OUT];    // HDCP_AUTO_FLAG; TRUE: Following input , FALSE: Setting by user
    BOOL bNVARMEDID_Inited; // the user edid is inited??

    BOOL  bUartReturnMessage_EN;           // Re_Message //系统信息反馈状态变?
 //   BOOL Re_Message;   
    BOOL  bSystemLock;                     // System  //系统锁定状态变量, true: lock, false; unlock
    BYTE  cIR_Carrier_Mode;                // IR_carrier_mode  1:红外载波强制 0:红外载波跟随
#ifdef PRD_POWER_ONOFF_MESSAGE
    BYTE  acPowerOnMsgBaudRate[MAX_OUT];    //PWON_baud
    BYTE  acPowerOnMsgLength[MAX_OUT];
    BYTE  acPowerOnMsg[MAX_OUT][PWON_OFF_Lenth]; // PWON_rBuff

    BYTE  acPowerOffMsgBaudRate[MAX_OUT];         //PWOFF_baud
    BYTE  acPowerOffMsgLength[MAX_OUT];
    BYTE  acPowerOffMsg[MAX_OUT][PWON_OFF_Lenth]; // PWOFF_rBuff
#endif
    BYTE  acRemoteBaudRate[MAX_OUT];   //[kyq add: 20150901 9:47:08]
    BYTE  acDefaultEdidProductName[4];           // EDID_name
//BYTE  acIRMatrix[MAX_OUT];                   // IR_before_close
    BYTE  cDebugLevel;
    BYTE  cPOSTEnable;     //The first step of the BIOS is the power-on self test 
   BYTE   acIChannellock[MAX_OUT];//xfd add:20160316 for 44E     // 1  该通道锁定
    BYTE  acLCDLine1DefaultTXT[17];              // LCD1_UPdata
    BYTE  acLCDLine2DefaultTXT[17];              // LCD2_UPdata
} SYSTEM_SETTING_DATA,*PSYSTEM_SETTING_DATA;  // psSystemSettingData


typedef struct
{
    BYTE  acEdidSelect[MAX_IN];                     // 选择哪个EDID， 目前没有用
    BOOL  abInputHDCP_EN[MAX_IN]; // HDCP_IN_EN
    BOOL  abCurrentHDCP[MAX_IN];  // input HDCP detection
    BOOL  abLink[MAX_IN];  // input connect status
    BOOL  abCurrentHDMI20[MAX_IN];  // input HDMI detection
    RESOLUTIONTYPE_t abReslution[MAX_IN];
    INPUTAUDIO_DATA acInputAudio;
}INPUTPORT_MANAGER_DATA,*PINPUTPORT_MANAGER_DATA;  // psInputPortMgrData



typedef struct
{
    BYTE  acInPortSelect[MAX_OUT];             // vedio_before_close  , 当前切换状态
    BYTE  acCloseInPortSelect[MAX_OUT]; //   保存关闭之前的状态
    BYTE  abHDMIAudio_EN[MAX_OUT]; // digit_audio
    BOOL  abHDCP_EN[MAX_OUT]; // HDCP_IN_EN
    BOOL  abHDCP_AUTO[MAX_OUT];
    BYTE  acScenceInPortSelect[SCENCES_MAX][MAX_OUT];             // 场景保存切换关系
#ifdef PRD_IR_OUT
    BYTE  acIRCloseInPortSelect[MAX_OUT]; //   保存关闭之前的状态
    BYTE  acIRInPortSelect[MAX_OUT]; // from remote IR input to local IR out
    BYTE  acScenceIRInPortSelect[SCENCES_MAX][MAX_OUT]; 
	
#endif

}OUTPUTPORT_MANAGER_DATA,*POUTPUTPORT_MANAGER_DATA;  // psOutputPortMgrData
// --------------------------------CONFIGURATION------------------------------
// list of all supported data manager events
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
/* --------------- BEGIN CUSTOM AREA [010] utilDataMgrAPI.h ------------------
 * CONFIGURATION: Add data manager events here */
/* ---------------------------- END CUSTOM AREA --------------------------- */
    nveINITIALIZE,                     // Standard reset (not calibration
                                       // nodes)
    nveRESETALL,                       // Complete reset including calibration
                                       // nodes
    nveINVALID                         // used for enumerated type range
                                       // checking (DO NOT REMOVE)
} eNV_EVENT;

// --------------------------------CONFIGURATION------------------------------
// list of all data manager data nodes
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
 
 
    nnSYSTEM_SETTING_DATA,
    //nnFACTORY_DATA_INFO,
    nnPOWER_MGR_DATA,
    nnOUTPUTPORT_MANAGER_DATA,
    nnINPUTPORT_MAMAGER_DATA,




/* CONFIGURATION: Add data manager nodes here.
 */
/* ---------------------------- END CUSTOM AREA --------------------------- */
    nnINVALID_NODE
} eNODE_NAME;

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************


typedef struct
{
  BYTE                 cIRKeyEnable;
}FACTORY_DATA, *PFACTORY_DATA;



// ---------------------------------------------------------------------------
// This API initializes the Data Manager component, and also calls all client
// callback routines to distribute node ID information to the client
// components.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrInitialize(void);

// ---------------------------------------------------------------------------
// This API calls all client callback routines with a reset event, so the
// client components can reset the data they have stored in non-volatile
// memory.
//
// Params:
//  eResetType : Reset type, sent to node callback functions
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrReset(eNV_EVENT eResetType);

// ---------------------------------------------------------------------------
// This API transfers data stored in the RAM mirror into non-volatile memory.
//
// Params:
//  eNodeID : The node identifier
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUpdate(eNODE_NAME eNodeID);

// ---------------------------------------------------------------------------
// This API locks the specified node for reading and returns a pointer to the
// node data in the RAM mirror.
//
// Params:
//  eNodeID : The node identifier
//
// Return:
//  Address of the node data in RAM
//
// /*@temp@*/ is an annotation for lint noting that the returned pointer
//   should not be deallocated before the calling routine exits.
// ---------------------------------------------------------------------------
/*@temp@*/
PVOID utilDataMgrReadLockData(eNODE_NAME eNodeID);

// ---------------------------------------------------------------------------
// This API locks the specified node for writing and returns a pointer to the
// node data in the RAM mirror.
//
// Params:
//  eNodeID : The node identifier
//
// Return:
//  Address of the node data in RAM.
// ---------------------------------------------------------------------------
PVOID utilDataMgrWriteLockData(eNODE_NAME eNodeID);

// ---------------------------------------------------------------------------
// This API unlocks the specified node.
//
// Params:
//  eNodeID : The node identifier
//  bWrite  : TRUE releases the write lock for this node and task
//            (multi-threaded systems). The node will be marked dirty to force
//            an update to NVRAM.   FALSE releases read lock.
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUnlock(eNODE_NAME eNodeID,
                       BOOL       bWrite);

// ---------------------------------------------------------------------------
// This API enables or inhibits updates of the RAM mirror into non-volatile
// memory.
//
// Params:
//  bInhibit : TRUE to prevent updates, FALSE otherwise
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUpdateInhibit(BOOL bInhibit);

// ---------------------------------------------------------------------------
// This API resets the Calibration node
//
// Params: None
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrResetCalData(void);
bool_t Init_NVRAM_module(BYTE MsgId);


#endif // _UTILDATAMGRAPI_H_

