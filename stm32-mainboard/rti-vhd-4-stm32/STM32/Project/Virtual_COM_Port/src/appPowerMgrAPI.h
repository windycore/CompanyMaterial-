#ifndef __APPWINDOWMGRPOPAPI_H__
#define __APPWINDOWMGRPOPAPI_H__
#include "si_datatypes.h"
#include "utilDataMgrAPI.h"
//#include "halPowerCtrlAPI.h"
#include "util.h"



//--------------------------------CONFIGURATION-------------------------------
// Power Manager parameter storage used by data manager
//--------------------------------CONFIGURATION-------------------------------
typedef struct
{
    ePOWERSTATE         eLastPwrState;
    //BOOL                bPwrMgmtEnabled;
    //BYTE                cLastDPMSWindowChecked;
    WORD                wUnitOnTime;
    WORD                wOnTime;
    WORD                wUnitOnDays;
/* ---------------------------- END CUSTOM AREA --------------------------- */
} POWER_MANAGER_DATA,*PPOWER_MANAGER_DATA;

void appPowerMgrNodeCallback(eNODE_NAME eNodeID,
                             eNV_EVENT  eEvent);

void appPowerMgrInitialize(void);
void appPowerMgrProcess(void);
void appPowerMgrPowerStateSet(ePOWERSTATE eNewState);
ePOWERSTATE  appPowerMgrLastPowerStateGet(void);


#endif


