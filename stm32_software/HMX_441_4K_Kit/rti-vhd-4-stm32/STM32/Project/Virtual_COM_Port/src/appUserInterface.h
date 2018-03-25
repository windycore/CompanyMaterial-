// ---------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>> COPYRIGHT NOTICE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ---------------------------------------------------------------------------
// 
//
//
// This is an unpublished work.
// ---------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>>>>>>>>> WARRANTEE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
/* -------------- BEGIN CUSTOM AREA [000] appUserInterface.h -----------------
 * Custom File Revision Information                                         */
//
// $RCSfile: appUserInterface.h,v $
// $Revision: 1.0 $
// $Author: kyq $
// $Date: 20150808 12:42:39] $
//
/* ---------------------------- END CUSTOM AREA --------------------------- */

#ifndef _APPUSERINTERFACE_H
#define _APPUSERINTERFACE_H

#include "si_Common.h"
#include "SwitchVideohandle.h"

// ***************************************************************************
// *************** START OF PUBLIC TYPE AND SYMBOL DEFINITIONS ***************
// ***************************************************************************
// This file does not contain any public non-configurable options.

// ***************************************************************************
// ******************* START OF PUBLIC CONFIGURATION SECTION *****************
// ***************************************************************************

// --------------------------------CONFIGURATION------------------------------
// list of possible user interface states
// --------------------------------CONFIGURATION------------------------------
typedef enum
{
    uiIDLE = 0,
/* -------------- BEGIN CUSTOM AREA [010] appUserInterface.h -----------------
 * CONFIGURATION: Add user interface states here */
    uiSPLASH, 
    uiSWITCH_VIDEO,
    uiUART_SWITCH_VIDEO,                 // 串口命令切换,by zhu
    uiUART_SWITCH_VIDEO2,                // 串口命令切换, 显示和按键一样, by kyq
    uiUART_SWITCH_IR,                    // 串口红外切换指令处理                
    uiSWITCH_INFO,                       // 查询单个端口的切换关系，按键会亮灯
    uiEDID_EXTERN_MANAGE,
    uiEDID_INTERN_MANAGE,
    uiSYSTEM_INCONNECT_INFO,    // 输入是否接信号
    uiUART_INCONNECT_INFO,     
    uiSYSTEM_OUTCONNECT_INFO, // 输出是否接信号
    uiUART_OUTCONNECT_INFO,
    uiSYSTEM_SWITCH_INFO,          // 所有输入和输出的对应关系
    uiUART_SWITCH_INFO,             //  所有端口的切换关系
    uiSYSTEM_IN_HDCP_INFO,
    uiUART_IN_HDCP_INFO,
    uiSYSTEM_OUT_HDCP_INFO,
    uiUART_OUT_HDCP_INFO,
    
    uiSYSTEM_OUT_RESOLUTION_INFO,
    uiUART_OUT_RESOLUTION_INFO,
    uiUART_MESSAGE,
   // uiSYSTEM_INFO,
    uiSYSTEM_LOCK_INFO,
    //uiSYSTEM_UNLOCK,
    uiSYSTEM_NAME,
    uiSYSTEM_VERSION,
    uiSYSTEM_MESSAGE_ONOFF_INFO,
    uiUART_SWITCH_PORT_INFO,   		// 串口发送status指令查询单个端口切换关系，按键 不会亮灯
    uiUART_SWITCH_ALL_PORT_INFO,   		// 串口发送status指令查询所有端口切换关系，按键 不会亮灯
    uiUART_OUT_HDMIAUDIO_INFO,
    uiUART_IN_HDCPENABLE_INFO,    // 用户设定HDCP 使能状态
    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    uiUART_OUT_LINK_INFO,
    #endif
    uiUART_IP_INFO,
    uiUART_SYSTEM_TYPE,
    uiFACTORY_SETTING,    // 内部使用
/*************************************************************/
     eSwitchDemoMode=0x300, 
     eSwitchAllThrough, 
     eSwitchxThrough, 
     eSwitchxAll, 
     eSwitchxTox, 
    eSwitchUndo,

    uiTEST,
/* ---------------------------- END CUSTOM AREA --------------------------- */
    uiINVALID,                          // Used for enumerated type range
                                       // checking (DO NOT REMOVE)
} eUI_STATE;

typedef enum
{
    ulChannelSwitch,
    ulAllThrough,
    ulAllClose,
    ulAllOpen,
    ulToAll,
    ulThrough,
    ulClose,
    ulOpen,
/* ---------------------------- END CUSTOM AREA --------------------------- */
    ulINVALID,                          // Used for enumerated type range
                                       // checking (DO NOT REMOVE)
} eUartLed_STATE;

/* -------------- BEGIN CUSTOM AREA [020] appUserInterface.h -----------------
 * Add custom includes, defines and types here */
/* ---------------------------- END CUSTOM AREA --------------------------- */


// ***************************************************************************
// ******************** END OF PUBLIC CONFIGURATION SECTION ******************
// ***************************************************************************

// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

// ---------------------------------------------------------------------------
// Change the state of the user interface to the specified value.
//
// Params:
//  State : state to transition to
//
// Return: none
//
// Notes:
//  This API muxt ONLY be called from within the User Interface component,
//  and only from appUIxxxEvent routines.
// ---------------------------------------------------------------------------
void appUIStateChange(eUI_STATE State);


void appUartSwitchLedStateChange(BYTE IN, BYTE OUT, eUartLed_STATE State);
//
//  IN -  输入口, begin 0
//  acOutPortList: 输出端口数
//  acPortList:   输出端口号 , begin 0
//
//
void appUartSwitchInfoSet(BYTE cInPort, BYTE cPortCount,PBYTE acOutPortList,BOOL bAVorIR);

void appUartSwitchSatusSet(BYTE cInPort, BYTE cPortCount,PBYTE acOutPortList);

// mars add---------------------------------------------------------------------------
// Get the state of the user interface.
//
// Params: none
//  
// Return: eState : state to transition to
// ---------------------------------------------------------------------------
eUI_STATE  appUIStateGet(void);
eUI_STATE  appUINextStateGet(void);
// ---------------------------------------------------------------------------
// Set a flag indicating that the user interface graphics need to be updated
//
// Params: none
//
// Return: none
//
// Notes:
//  This API muxt ONLY be called from within the User Interface component,
//  and only from appUIxxxEvent routines.
// ---------------------------------------------------------------------------
void appUIUpdateSet(void);

// ---------------------------------------------------------------------------


void appUIKeyReset(BYTE cState);


void appUIIRKeyEnableSet(BOOL bEnable);

/* ---------------------------- END CUSTOM AREA --------------------------- */
// ---------------------------------------------------------------------------
// This API sets up the internal state of this component.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void appUIInitialize(void);

// ---------------------------------------------------------------------------
// This API performs periodic processing for the user interface.  It checks
// for user input (e.g. keypad or infrared) and updates the state of the user
// interface appropriately.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void appUIProcess(void);

// ---------------------------------------------------------------------------
// This API is accessed by the Data Manager component to allow initialization
// and resetting of this component抯 persistent data node by handling external
// events triggered by the persistent data storage.
//
// Params:
//  eNodeID : Node identifier that is saved internally to access this node
//  eEvent  : Identifier of the external event. Event can be  nveRESETALL or
//            nveINITIALIZE
//
// Return: none
// ---------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Draw On-Screen Display (OSD) graphics for the progress of the Auto Scan
//
// Params:
//  nPercent       : The percentage done of the auto scan
//
// Return: <none>
//----------------------------------------------------------------------------
void appUIProgressAutoScanShow(INT nPercent);

//----------------------------------------------------------------------------
// Enable or disable the UI
//
// Params:
//  bEnable  : TRUE to Enable the UI, FALSE to disable
//
// Return: <none>
//----------------------------------------------------------------------------
void appUIEnable(BOOL bEnable);
// ---------------------------------------------------------------------------
// Enable or disable the UI
//
// Params:
//  bEnable : TRUE to enable the UI (reset the current state), FALSE to disable
//
// Return: none
//
// Notes:
// ---------------------------------------------------------------------------

void appUIExit(void);// 
// ---------------------------------------------------------------------------
// Return TURE if current UI state is IDLE
//
// Params: none
//
// Return: 
//
// Notes:
// ---------------------------------------------------------------------------

BOOL appUIStateIsIdle(void);
// ---------------------------------------------------------------------------
// Return none
//
// Params: message to display
//
// Return: 
//
// Notes:
// ---------------------------------------------------------------------------
//
// 把串口命令中响应后的LCD 显示设置进来
void appUIUartMessageSet(PBYTE pcLine1Message, PBYTE pcLine2Message,BOOL bSendMessage);

// 显示某个端口的切换信息
void appUIUartMessageSwitcherPortSet(BYTE cOutPort);

void appUIDemoSwitcherInfoSet(BYTE cOutPort);
#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
void InitLinkStatue(void);
void QueryLinkStatue(void);
#endif

#if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
void Set_PreLinkStatus(WORD u16Value);
WORD Get_PreLinkStatus(void);
#endif

#endif // _APPUSERINTERFACE_H

