#ifndef _UTILDATAMGRAPI_H_
#define _UTILDATAMGRAPI_H_
#include "si_common.h"
#include "mcu.h"
#include "si_datatypes.h"
#include "si_timer.h"
#include "MainApp.h"

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

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************

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

bool_t NVRAMInitialize(void);
bool_t NVRAMRxMsgHandler (SendData_t *pMsg );
bool_t NVRAMCtrlEventHandler(void);

#endif // _UTILDATAMGRAPI_H_

