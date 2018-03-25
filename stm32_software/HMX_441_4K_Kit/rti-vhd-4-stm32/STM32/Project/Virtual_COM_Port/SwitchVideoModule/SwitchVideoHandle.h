
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

// �������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void appSystemSwitcherSet(BYTE cInput, BYTE cPortNumber, ...);
void appSystemSwitcherSet2(BYTE cInput, BYTE cPortNumber, ...);

void appSystemSwitcherPortSet(BYTE cInput, BYTE cPortNumber, PBYTE cOutPortList);



// �� NVRAM �ﱣ����л���ϵ���õ�
//
//
void appSystemSwitcherSetStart(void);
//�ָ���һ�ε��л���ϵ
void appSystemSwitcherUndo(void);

//�ر����е����
void appSystemSwitcherOutportCloseAll(void);

void appSystemSwitcherOutportForceCloseAll(void);
//�ָ��ر�ǰ״̬
void appSystemSwitcherOutportRecoverAll(void);
//ǿ�ƻָ�֮ǰ��״̬
void appSystemSwitcherOutportForceRecoverAll(void);
//���浱ǰ�л�״̬��ָ���ĳ�����
void appSystemSwitcherScenceSave(BYTE cScence);

//����ָ���ĳ���
void appSystemSwitcherScenceCall(BYTE cScence);

//ɾ��ָ���ĳ���
void appSystemSwitcherScenceDel(BYTE cScence);

bool_t SwitchVideoinit(void);

void Get_LastInPortSelect(PBYTE pValue);
#endif
