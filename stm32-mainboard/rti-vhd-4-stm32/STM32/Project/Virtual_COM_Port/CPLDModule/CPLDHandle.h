#ifndef _CPLDHANDLE_H_
#define _CPLDHANDLE_H_
#include "si_datatypes.h"
#include "dvCPLD.h"
#include "util.h"
#include "appSystemMgrAPI.h"

#ifdef PRD_IR_OUT
//�л�Զ�˺��������������IR_OUT �˿�
// �������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void appSystemIRSwitcherSet(BYTE cInput, BYTE cPortNumber, ...);
// ע�������INPUT ��ָIR OUT 1~4
// �������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0,MAX_IN will be mute the output
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void appSystemIRSwitcherPortSet(BYTE cInput, BYTE cPortNumber, PBYTE cOutPortList);

void appSystemIRSwitcherSetStart(void);


#endif
 bool_t CpldInit(void);
#endif