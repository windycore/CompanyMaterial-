#include "SwitchVideoHandle.h"
#include "si_osal_timer.h"
#include "appUserInterface.h"
static BYTE m_acLastInPortSelect[MAX_OUT]=
{
    0xff,
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
//eSYSEM_STATE m_eSystemState;
extern  BYTE m_acLastIRInPortSelect[MAX_OUT];
static BOOL m_bDemoMode = FALSE;

//关闭所有的输出, 包括IR 关闭
void appSystemSwitcherOutportCloseAll(void)
{
    BYTE   cOut;
    BYTE cCNT = 0;
    BYTE cOutList[MAX_OUT];
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    for (cOut = 0; cOut < MAX_OUT; cOut++)
    {
        if((psOutputPortMgrData->acInPortSelect[cOut])<MAX_IN)
        {
            psOutputPortMgrData->acCloseInPortSelect[cOut] =  psOutputPortMgrData->acInPortSelect[cOut];
	     m_acLastInPortSelect[cOut] = psOutputPortMgrData->acInPortSelect[cOut];
            psOutputPortMgrData->acInPortSelect[cOut]=MAX_IN;
        }
#ifdef PRD_IR_OUT
        if((psOutputPortMgrData->acIRInPortSelect[cOut])<MAX_IN)
        {
            psOutputPortMgrData->acIRCloseInPortSelect[cOut] =  psOutputPortMgrData->acIRInPortSelect[cOut];
            psOutputPortMgrData->acIRInPortSelect[cOut]=MAX_IN;
        }
#endif
        cOutList[cOut]  =cOut;
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    dvM21452SwitcherClosePort(MAX_OUT,cOutList); // close all
    #else
    dvADN4604SwitcherClosePort(MAX_OUT,cOutList); // close all
    #endif
#ifdef PRD_IR_OUT
    appSystemIRSwitcherSet(MAX_OUT,MAX_OUT,0,1,2,3,4,5,6,7); // close all
#endif
}

//强制关闭
void appSystemSwitcherOutportForceCloseAll(void)
{
    BYTE   cOut;
    BYTE cCNT = 0;
    BYTE cOutList[MAX_OUT];
    for (cOut = 0; cOut < MAX_OUT; cOut++)
    {
        cOutList[cOut]    =cOut;
    }
    #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    dvM21452SwitcherClosePort(MAX_OUT,cOutList); // close all
    #else
    dvADN4604SwitcherClosePort(MAX_OUT,cOutList); // close all
    #endif
#ifdef PRD_IR_OUT
    //  appSystemIRSwitcherSet(MAX_OUT,MAX_OUT,0,1,2,3,4,5,6,7); // close all
    dvCPLDIRSwitcherSet(MAX_OUT,MAX_OUT,0,1,2,3,4,5,6,7);
#endif
}

//[kyq Begin: 20151028 11:58:48]
//
//  cInput: inputportnumber begin from 0
//  bEnable:  1: Disable output
//
void appSystemSwitcherPortMuteSet(BYTE cInput, BOOL bEnable)
{
    BYTE acOutPort[MAX_OUT];
    BYTE i,j =0;
    // Start the varargs processing.
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
    for( i= 0; i< MAX_OUT; i++)
    {
        if(psOutputPortMgrData->acInPortSelect[i] == cInput)
        {
            acOutPort[j] = i;
            j++;
        }
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      FALSE);
    if(j>0)
    {
	 #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
	 dvM21452SwitcherPortSet((bEnable?MAX_IN: cInput), j, acOutPort);
	 #else
	 dvADN4604SwitcherPortSet((bEnable?MAX_IN: cInput), j, acOutPort);
	 #endif
    }
}

//huang
//
//  coutput: outputportnumber
//  bEnable:  1: Disable output
//
void appSystemSwitcherOutPortMute(BYTE cOutput, BOOL bEnable)
{
    ASSERT(cOutput<MAX_OUT);
    if(cOutput<MAX_OUT)
    {
        #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    	 dvM21452SwitcherClosePort(1,&cOutput);    // close
    	 #else
    	 dvADN4604SwitcherClosePort(1,&cOutput);    // close
    	 #endif
    }
}

// 把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void appSystemSwitcherSet(BYTE cInput, BYTE cPortNumber, ...)
{
    va_list         args;
    // va_list         args2;
    BYTE i;
    BYTE cOutPort;
    bool_t fSwitchChang=0;
    BYTE    OutPortList[MAX_OUT];
    BYTE PortNumber=0;
    // Start the varargs processing.
    va_start(args, cPortNumber);
//   va_start(args2, cPortNumber);  //[kyq edid: 20150820 14:42:20]
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    //memset(m_acLastInPortSelect, 0xff, MAX_OUT); 				//[kyq Begin: 20150924 16:04:02] // clear first
    for (i = 0; i < cPortNumber; i++)
    {
        cOutPort = va_arg(args, BYTE);
        assert_param(cOutPort < MAX_OUT);
	 m_acLastInPortSelect[cOutPort] = psOutputPortMgrData->acInPortSelect[cOutPort];
        if(cInput==psOutputPortMgrData->acInPortSelect[cOutPort])
        {
            continue;
        }
        OutPortList[PortNumber]=cOutPort;
        PortNumber++;
        psOutputPortMgrData->acInPortSelect[cOutPort] = cInput;
    }
    if(PortNumber!=0)
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                          TRUE);
    else
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                          FALSE);
    //  dvADN4604SwitcherSet( cInput, cPortNumber, args2);
    #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    dvM21452SwitcherPortSet(cInput, PortNumber, OutPortList);
    #else
    dvADN4604SwitcherPortSet(cInput, PortNumber, OutPortList);
    #endif
    va_end(args);
    //  va_end(args2);
}

//[kyq End: 20151028 11:58:51]
// 把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0,MAX_IN will be mute the output
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void appSystemSwitcherPortSet(BYTE cInput, BYTE cPortNumber, PBYTE cOutPortList)
{
    BYTE cOutPort;
    BYTE i;
    BYTE    OutPortList[MAX_OUT];
    BYTE PortNumber=0;
    // Start the varargs processing.
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    //memset(m_acLastInPortSelect, 0xff, MAX_OUT); //[kyq Begin: 20150924 16:04:02] // clear first
    for (i = 0; i < cPortNumber; i++)
    {
        cOutPort = cOutPortList[i];
        assert_param(cOutPort < MAX_OUT);
	 m_acLastInPortSelect[cOutPort] = psOutputPortMgrData->acInPortSelect[cOutPort];
        if(cInput==psOutputPortMgrData->acInPortSelect[cOutPort])
        {
            continue;
        }
        OutPortList[PortNumber]=cOutPort;
        PortNumber++;
        psOutputPortMgrData->acInPortSelect[cOutPort] = cInput;
        //psOutputPortMgrData->abHDMIAudio_EN[i] = TRUE;
        //psOutputPortMgrData->abHDCP_EN[i] = TRUE;
    }
    if(PortNumber!=0)
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                          TRUE);
    else
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                          FALSE);
    #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    dvM21452SwitcherPortSet(cInput, PortNumber, OutPortList);
    #else
    dvADN4604SwitcherPortSet(cInput, PortNumber, OutPortList);
    #endif
}

// 把 NVRAM 里保存的切换关系设置到
//
//
void appSystemSwitcherSetStart(void)
{
    BYTE cInput, cInSel, cOut,i=0;
    BYTE cCNT = 0;
    BYTE cOutList[MAX_OUT];
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
    for (cInput = 0; cInput <= MAX_IN; cInput++)  // add MAX_IN , for mute function
    {
        cCNT = 0;
        for (cOut = 0; cOut < MAX_OUT; cOut++)
        {
            cInSel = psOutputPortMgrData->acInPortSelect[cOut];
            if (cInput == cInSel)
            {
                //m_acLastInPortSelect[cOut]=cInput;
                cOutList[cCNT] = cOut;
                cCNT++;
            }
        }
        if (cCNT>0)
        {
	     #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    	     dvM21452SwitcherPortSet(cInput, cCNT, cOutList);
    	     #else
    	     dvADN4604SwitcherPortSet(cInput, cCNT, cOutList);
    	     #endif
        }
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      FALSE);
}

//恢复上一次的切换关系
void appSystemSwitcherUndo(void)
{
    BYTE cInput, cInSel, cOut,i=0;
    BYTE cCNT = 0;
    BYTE cOutList[MAX_OUT];
    BYTE cTemp;
    BYTE cIRInSel;
    BYTE cIROutList[MAX_OUT];
    BYTE cCNT2 = 0;
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);

    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    for (cOut = 0; cOut < MAX_OUT; cOut++)
    {
        if (m_acLastInPortSelect[cOut] != 0xff)
        {
            cTemp = psOutputPortMgrData->acInPortSelect[cOut] ;
            psOutputPortMgrData->acInPortSelect[cOut] = m_acLastInPortSelect[cOut];
            m_acLastInPortSelect[cOut]= cTemp;
        }
#ifdef PRD_IR_OUT
        if (m_acLastIRInPortSelect[cOut] != 0xff)
        {
            cTemp = psOutputPortMgrData->acIRInPortSelect[cOut] ;
            psOutputPortMgrData->acIRInPortSelect[cOut] = m_acLastIRInPortSelect[cOut];
            m_acLastIRInPortSelect[cOut]= cTemp;
        }
#endif
    }
    for (cInput = 0; cInput < MAX_IN; cInput++)         //查找输入对应的输出通道，然后放到cOutList数组
    {
        cCNT = 0;
        cCNT2 = 0;
        for (cOut = 0; cOut < MAX_OUT; cOut++)
        {
            cInSel = psOutputPortMgrData->acInPortSelect[cOut];
            if (cInput == cInSel)
            {
                cOutList[cCNT] = cOut;
                cCNT++;
            }
#ifdef PRD_IR_OUT
            cIRInSel =  psOutputPortMgrData->acIRInPortSelect[cOut]; // from remote IR input to local IR out
            if (cInput == cIRInSel)
            {
                cIROutList[cCNT2] = cOut;
                cCNT2++;
            }
#endif
	     if (cInSel>=MAX_IN)		//关闭输出通道
	     {
			#if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
	    	       dvM21452SwitcherPortSet(MAX_IN, 1, &cOut);
	    	       #else
	    	       dvADN4604SwitcherPortSet(cInput, cCNT, cOutList);
	    	       #endif
	     }
        }
        if (cCNT>0)
        {
	     #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    	     dvM21452SwitcherPortSet(cInput, cCNT, cOutList);
    	     #else
    	     dvADN4604SwitcherPortSet(cInput, cCNT, cOutList);
    	     #endif
        }
#ifdef PRD_IR_OUT
        if (cCNT2>0)
        {
            //dvADN4604SwitcherPortSet( cInput, cCNT, cOutList);
            dvCPLDIRSwitcherPortSet(cInput,cCNT2, cIROutList);
        }
#endif
    }
}

//恢复关闭前状态,包括IR
void appSystemSwitcherOutportRecoverAll(void)
{
    BYTE  cOut;
    //BYTE cCNT = 0,cInput, cInSel,;
    //BYTE cOutList[MAX_OUT];
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    //memset(m_acLastInPortSelect, 0xff, MAX_OUT); //[kyq Begin: 20150924 16:04:02] // clear first
    //memset(m_acLastIRInPortSelect, 0xff, MAX_OUT); //[kyq Begin: 20150924 16:04:02] // clear first
    for (cOut = 0; cOut < MAX_OUT; cOut++)
    {
        if(psOutputPortMgrData->acInPortSelect[cOut]>=MAX_IN)
        {
            m_acLastInPortSelect[cOut]=psOutputPortMgrData->acInPortSelect[cOut] ;
            psOutputPortMgrData->acInPortSelect[cOut] = psOutputPortMgrData->acCloseInPortSelect[cOut];
        }
#ifdef PRD_IR_OUT
        if(psOutputPortMgrData->acIRInPortSelect[cOut]>=MAX_IN)
        {
            m_acLastIRInPortSelect[cOut]=psOutputPortMgrData->acIRInPortSelect[cOut] ;
            psOutputPortMgrData->acIRInPortSelect[cOut] = psOutputPortMgrData->acIRCloseInPortSelect[cOut];
        }
#endif
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    appSystemSwitcherSetStart();
#ifdef PRD_IR_OUT
    appSystemIRSwitcherSetStart();
#endif
}

//强制恢复之前的状态
void appSystemSwitcherOutportForceRecoverAll(void)
{
    appSystemSwitcherSetStart();
#ifdef PRD_IR_OUT
    appSystemIRSwitcherSetStart();
#endif
}

//保存当前切换状态到指定的场景里
void appSystemSwitcherScenceSave(BYTE cScence)
{
    BYTE  cOut;
    // BYTE cCNT = 0,cInput, cInSel,;
    //BYTE cOutList[MAX_OUT];
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    if(cScence>=SCENCES_MAX)
    {
        return;
    }
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    for (cOut = 0; cOut < MAX_OUT; cOut++)
    {
        psOutputPortMgrData->acScenceInPortSelect[cScence][cOut] = psOutputPortMgrData->acInPortSelect[cOut];
#ifdef PRD_IR_OUT
        psOutputPortMgrData->acScenceIRInPortSelect[cScence][cOut] = psOutputPortMgrData->acIRInPortSelect[cOut];
#endif
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    //appSystemSwitcherSetStart();
}

//调用指定的场景
void appSystemSwitcherScenceCall(BYTE cScence)
{
    BYTE  cOut;//cInput, cInSel,
    //BYTE cCNT = 0;
    //BYTE cOutList[MAX_OUT];
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    if(cScence>=SCENCES_MAX)
    {
        return;
    }
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    for (cOut = 0; cOut < MAX_OUT; cOut++)
    {
        if(psOutputPortMgrData->acScenceInPortSelect[cScence][cOut] !=0xff)
        {
            break;
        }
    }
    if(cOut== MAX_OUT)
    {
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                          TRUE);
        return; // empty data ,no action
    }
    //memset(m_acLastInPortSelect, 0xff, MAX_OUT); //[kyq Begin: 20150924 16:04:02] // clear first
    //memset(m_acLastIRInPortSelect, 0xff, MAX_OUT); //[kyq Begin: 20150924 16:04:02] // clear first
    for (cOut = 0; cOut < MAX_OUT; cOut++)
    {
        m_acLastInPortSelect[cOut]=psOutputPortMgrData->acInPortSelect[cOut] ;
        psOutputPortMgrData->acInPortSelect[cOut] = psOutputPortMgrData->acScenceInPortSelect[cScence][cOut];
#ifdef PRD_IR_OUT
        m_acLastIRInPortSelect[cOut]=psOutputPortMgrData->acIRInPortSelect[cOut] ;
        psOutputPortMgrData->acIRInPortSelect[cOut] = psOutputPortMgrData->acScenceIRInPortSelect[cScence][cOut];
#endif
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    appSystemSwitcherSetStart();
#ifdef PRD_IR_OUT
    appSystemIRSwitcherSetStart();
#endif
}

//删除指定的场景
void appSystemSwitcherScenceDel(BYTE cScence)
{
    BYTE cOut;// cInput, cInSel,
    //BYTE cCNT = 0;
    //BYTE cOutList[MAX_OUT];
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    if(cScence>=SCENCES_MAX)
    {
        return;
    }
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    for (cOut = 0; cOut < MAX_OUT; cOut++)
    {
        psOutputPortMgrData->acScenceInPortSelect[cScence][cOut] = MAX_IN;//0xff;
#ifdef PRD_IR_OUT
        psOutputPortMgrData->acScenceIRInPortSelect[cScence][cOut] = MAX_IN;//0xff;
#endif
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
}

void appSystemSwitchDemoModeEnableSet(BOOL bEnable)
{
    m_bDemoMode = bEnable;
}
BOOL appSystemSwitchDemoModeEnableGet(void)
{
    return m_bDemoMode;
}

void appSystemSwitchDemoModeProcess(void)
{
    static clock_time_t  h_DemoTimer = 0;
    static BYTE h_cInputPort = 0;
    static BYTE h_cOutputPort = 0;
    if (!m_bDemoMode)
    {
        return;
    }
    if(SkTimeDiffMs(h_DemoTimer, SiiPlatformTimerSysTicksGet()) > mS_TO_TICKS(4000))
    {
        h_DemoTimer = SiiPlatformTimerSysTicksGet();
        // This will be save the swticher status
        //appUIStateChange(uiSYSTEM_SWITCH_INFO);
        appSystemSwitcherSet(h_cInputPort,1,h_cOutputPort);
#ifdef PRD_IR_OUT
        appSystemIRSwitcherSet(h_cInputPort,1,h_cOutputPort); // add by kyq 20150929
#endif
        appUIDemoSwitcherInfoSet(h_cOutputPort);
        h_cOutputPort++;
        if (h_cOutputPort == MAX_OUT)
        {
            h_cOutputPort = 0;
            h_cInputPort++;
            if (h_cInputPort == MAX_IN)
            {
                h_cInputPort = 0;
            }
        }
    }
}

bool_t SwitchVideoinit(void)
{
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    if(!dvM21452Init())
    {
       #if INC_UART	 
	if(psSystemSettingData->cPOSTEnable)
	UART1_SendData2PC("M21452 fail!", strlen("M21452 fail!"));
       #endif
    }
    #else
    if(!dvADN4604Init())
    {
       #if INC_UART	 
	if(psSystemSettingData->cPOSTEnable)
	UART1_SendData2PC("ADN4604 fail!", strlen("ADN4604 fail!"));
       #endif
    }
    #endif
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE); 
    return 0;
}

