#include "CPLDHandle.h"
#include <stdio.h>
#include "si_datatypes.h"
#include <stdarg.h>
#include <string.h>

 BYTE m_acLastIRInPortSelect[MAX_OUT]=
		{0xff,
	#if(MAX_OUT>1)
	  0xff,
	#endif
	 #if(MAX_OUT>2)
	  0xff,
	#endif
	  #if(MAX_OUT>3)
	  0xff,
	#endif
	#if(MAX_OUT>4)
	  0xff,
	#endif
	#if(MAX_OUT>5)
	  0xff,
	#endif
	#if(MAX_OUT>6)
	  0xff,
	#endif
	#if(MAX_OUT>7)
	  0xff,
	#endif
	
	}; // 保存旧的切换关系

#ifdef PRD_IR_OUT
// 注意这里的INPUT 是指IR OUT 1~4
//切换远端红外过来到本机的IR_OUT 端口
// 把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void appSystemIRSwitcherSet(BYTE cInput, BYTE cPortNumber, ...)
{
    va_list         args;
    va_list         args2;
    BYTE i;
    BYTE cOutPort;
    // Start the varargs processing.
    va_start(args, cPortNumber);
    va_start(args2, cPortNumber);  //[kyq edid: 20150820 14:42:20]
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
   memset(m_acLastIRInPortSelect, 0xff, MAX_OUT); //[kyq Begin: 20150924 16:04:02] // clear first
    for (i = 0; i < cPortNumber; i++)
    {
        cOutPort = va_arg(args, BYTE);
        assert_param(cOutPort < MAX_OUT);
        m_acLastIRInPortSelect[cOutPort] = psOutputPortMgrData->acIRInPortSelect[cOutPort];
        psOutputPortMgrData->acIRInPortSelect[cOutPort] = cInput;
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    dvCPLDIRSwitcherSet(cInput, cPortNumber, args2);
    va_end(args);
    va_end(args2);
}
// 注意这里的INPUT 是指IR OUT 1~4
// 把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0,MAX_IN will be mute the output
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void appSystemIRSwitcherPortSet(BYTE cInput, BYTE cPortNumber, PBYTE cOutPortList)
{
    BYTE cOutPort;
    BYTE i;
    // Start the varargs processing.
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    memset(m_acLastIRInPortSelect, 0xff, MAX_OUT); //[kyq Begin: 20150924 16:04:02] // clear first
    for (i = 0; i < cPortNumber; i++)
    {
        cOutPort = cOutPortList[i];
        assert_param(cOutPort < MAX_OUT);
        m_acLastIRInPortSelect[cOutPort] = psOutputPortMgrData->acIRInPortSelect[cOutPort];
        psOutputPortMgrData->acIRInPortSelect[cOutPort] = cInput;
        //psOutputPortMgrData->abHDMIAudio_EN[i] = TRUE;
        //psOutputPortMgrData->abHDCP_EN[i] = TRUE;
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    //dvADN4604SwitcherPortSet(cInput, cPortNumber, cOutPortList);
     dvCPLDIRSwitcherPortSet(cInput, cPortNumber, cOutPortList);
}
void appSystemIRSwitcherSetStart(void)
{
    BYTE cInput, cInSel, cOut;
    BYTE cCNT = 0;
    BYTE cOutList[MAX_OUT];
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
    for (cInput = 0; cInput <= MAX_IN; cInput++)  // add MAX_IN , for mute function
    {
        cCNT = 0;
        for (cOut = 0; cOut < MAX_OUT; cOut++)
        {
            cInSel = psOutputPortMgrData->acIRInPortSelect[cOut];
            if (cInput == cInSel)
            {
                //m_acLastInPortSelect[cOut]=cInput;
                cOutList[cCNT] = cOut;
                cCNT++;
            }
        }
        if (cCNT>0)
        {
           // dvADN4604SwitcherPortSet( cInput, cCNT, cOutList);
           dvCPLDIRSwitcherPortSet(cInput, cCNT, cOutList);
        }
        
    }
	utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      FALSE);
}
#endif


// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************
// ---------------------------------------------------------------------------
// This API sets up the internal state of component.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
 bool_t CpldInit(void)
{
        dvCpldInit();		//初始化LCD
    return true;
}
