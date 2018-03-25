// file :appSystemMgrAPI.h
#ifndef _APPSYSTEMMGR_H_
#define _APPSYSTEMMGR_H_
#include "mcu.h"
#include "si_datatypes.h"
#include "utilDataMgrAPI.h"
#include "util.h"

//#define PRD_TYPE  "HDBaseT Matrix"
#define PRD_TYPE  "HDMI2.0 Switcher"
#if (MACHINETYPE==MUH44TPR2_N)
#define PRD_NAME "MUH44TPR2_N"
#elif (MACHINETYPE==MUH88E_BYE)
#define PRD_NAME "MUH88E"
#elif(MACHINETYPE==PRD_VHD4)
#define PRD_NAME "VHD-4"
#elif(MACHINETYPE==MUH44E_BYE)
#define PRD_NAME "MUH44E"
#elif(MACHINETYPE==MUH44A_H2)
#define PRD_NAME "MUH44A-H2"
#elif(MACHINETYPE==MUH88A_H2)
#define PRD_NAME "MUH88A-H2"
#endif

void appSystemSettingNodeCallback(eNODE_NAME eNodeID,
                                  eNV_EVENT  eEvent);



#endif

