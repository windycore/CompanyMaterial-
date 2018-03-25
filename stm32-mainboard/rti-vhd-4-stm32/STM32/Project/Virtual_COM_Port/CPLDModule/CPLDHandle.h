#ifndef _CPLDHANDLE_H_
#define _CPLDHANDLE_H_
#include "si_datatypes.h"
#include "dvCPLD.h"
#include "util.h"
#include "appSystemMgrAPI.h"

#ifdef PRD_IR_OUT
//切换远端红外过来到本机的IR_OUT 端口
// 把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void appSystemIRSwitcherSet(BYTE cInput, BYTE cPortNumber, ...);
// 注意这里的INPUT 是指IR OUT 1~4
// 把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0,MAX_IN will be mute the output
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void appSystemIRSwitcherPortSet(BYTE cInput, BYTE cPortNumber, PBYTE cOutPortList);

void appSystemIRSwitcherSetStart(void);


#endif
 bool_t CpldInit(void);
#endif