
#ifndef _OUTPUTHANDLE_H_
#define _OUTPUTHANDLE_H_
#include "si_datatypes.h"
#include "dvTM1623.h"

void appInputHDMI14_20DetectAndOutputProcess(void);
void appInputHDMI14_20DetectAndOutputProcessTemp(BYTE cOut);
void Output5VHigthLow(BYTE cOutput);
void appOutputHDCPProcess(void);
// HDMI �������أ����ұ���״̬��NVRAM,���������̫��ѧ
void appSystemHDMITXAudioOutEnableSet(BYTE cInstance, BOOL bEnable);

#endif