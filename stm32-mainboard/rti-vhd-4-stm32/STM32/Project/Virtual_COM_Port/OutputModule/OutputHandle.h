
#ifndef _OUTPUTHANDLE_H_
#define _OUTPUTHANDLE_H_
#include "si_datatypes.h"
#include "dvTM1623.h"

void appInputHDMI14_20DetectAndOutputProcess(void);
void appInputHDMI14_20DetectAndOutputProcessTemp(BYTE cOut);
void Output5VHigthLow(BYTE cOutput);
void appOutputHDCPProcess(void);
// HDMI 声音开关，并且保存状态到NVRAM,这个函数不太科学
void appSystemHDMITXAudioOutEnableSet(BYTE cInstance, BOOL bEnable);

#endif