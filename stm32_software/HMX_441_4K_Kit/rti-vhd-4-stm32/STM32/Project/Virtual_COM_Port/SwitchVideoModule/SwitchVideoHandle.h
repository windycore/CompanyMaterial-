
#ifndef _SWITCHVEDIOHANDLE_H_
#define _SWITCHVEDIOHANDLE_H_
#include "si_datatypes.h"
#include "utilDataMgrAPI.h"
#include "appSystemMgrAPI.h"
#include "Uart.h"
#include "dvADN4604.h"


//extern BYTE sFirstRun ;

//[kyq Begin: 20151028 11:58:48]
//
//  cInput: inputportnumber begin from 0
//  bEnable:  1: Disable output 
//
void appSystemSwitcherPortMuteSet(BYTE cInput, BOOL bEnable);
void appSystemSwitcherOutPortMute(BYTE cOutput, BOOL bEnable);

// 把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void appSystemSwitcherSet(BYTE cInput, BYTE cPortNumber, ...);
void appSystemSwitcherSet2(BYTE cInput, BYTE cPortNumber, ...);

void appSystemSwitcherPortSet(BYTE cInput, BYTE cPortNumber, PBYTE cOutPortList);



// 把 NVRAM 里保存的切换关系设置到
//
//
void appSystemSwitcherSetStart(void);
//恢复上一次的切换关系
void appSystemSwitcherUndo(void);

//关闭所有的输出
void appSystemSwitcherOutportCloseAll(void);

void appSystemSwitcherOutportForceCloseAll(void);
//恢复关闭前状态
void appSystemSwitcherOutportRecoverAll(void);
//强制恢复之前的状态
void appSystemSwitcherOutportForceRecoverAll(void);
//保存当前切换状态到指定的场景里
void appSystemSwitcherScenceSave(BYTE cScence);

//调用指定的场景
void appSystemSwitcherScenceCall(BYTE cScence);

//删除指定的场景
void appSystemSwitcherScenceDel(BYTE cScence);

bool_t SwitchVideoinit(void);

void Get_LastInPortSelect(PBYTE pValue);
#endif
