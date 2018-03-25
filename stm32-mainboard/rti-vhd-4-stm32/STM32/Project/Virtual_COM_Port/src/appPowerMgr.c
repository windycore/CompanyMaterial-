//[kyq Begin: 20150805 9:33:46]
//
//
//
#include "appPowerMgrAPI.h"
#include "utilDataMgrAPI.h"
#include "util.h"
#include "appSystemMgrAPI.h"
#include "dvCPLD.h"
#include "misc.h"
#include "appUserInterface.h"
#include "uart.h"
#include "dvM21452.h"
#include "halPowerCtrlAPI.h"
#include "dvPC9539.h"
#include "si_timer.h"
#include "SwitchVideoHandle.h"
#include "dvLcd.h"

#define     POWER_UPDATE_POLL   1000                               // 1s
#define     TICKS_PER_SECOND    (1000L/TICK_TIME_mS)     // 1s 有多少个TICK
#define     TICKS_PER_MSECOND   (1/TICK_TIME_mS)          // 1ms 有多少个TICK
#define     NO_WINDOW           0xff
#define     MAX_COUNTER         0xffff
// ms
#define SUSPEND_DELAY       2000			//9500
#define STANDBY_DELAY       3000//5000			//7500, to short can't display osd
#define SOFTOFF_DELAY       2000
#define POWER_ON_DELAY      2000 //750  kyq 20150929
#define SOFTRESET_DELAY     2000
#define SOURCE_ROUTE_POWERDN 20000
// Scaled the saved data so each tick saved in NVRAM data is 1 minute
#define NVRAM_SECONDS_PER_TICK          60
// Local power state the system is currently running in.
static ePOWERSTATE      m_ePowerState;
// Local power state the system was in before the request to change states
// started the transition to a new power state.
static ePOWERSTATE      m_eLastPowerState;
// Local power state the system will be in after completing the state
// transition
static ePOWERSTATE      m_eNextPowerState;
static WORD             m_wLastUnitOnSeconds;
// Local copy of system on timer.
static WORD             m_wSecondsTimer; // 秒计数, m_wPowerTimer - m_wLastSecondsTimer
// Local copy of system on timer.
static WORD             m_wLastSecondsTimer; // TICK计数，上一次计数变量
// Local timer updated by cylic processing using util counter (in system
// ticks)
static WORD             m_wPowerTimer;   //  单位tick    当前计数变量
// Local counter used to update a local timer
static WORD             m_wLastPowerCounter;
// Local event timer for timing power on transitions.
// - PowerMgrStartStateTransition 开始更新
// - appPowerMgrProcess  判断时间是否到点，到点开始切电操作
static WORD             m_wTransTimer;
// ---------------------------------------------------------------------------
// This API starts a power transition by setting up the next power state. The
// next time through the main loop process routine we will start any delays or
// special processing associated with this state transition.
//
// Params:
//  cRouteId       : Route identifier - for future use.
//  eNewPowerState : The power state we want to transition to
//
// Return: none
// ---------------------------------------------------------------------------
//  处理其他需要较长时间关掉的电源处理
//  比如芯片关电
//
//    powerstate  --写入---> NVRAM
//
//   poweron --> standy/off  先执行这个函数, 不能改变m_ePowerState,  然后改变ProcessPowerOnTransition
//   Standby --> poweron     先执行ProcessStandbyTransition， 再执行 这个函数，执行后需要更改m_ePowerState
//
//
//----> ProcessStandbyTransition()  ---->appPowerMgrProcess()
//----> ProcessSoftOffTransition()   ---->appPowerMgrProcess()
//----> ProcessOffTransition()        ---->appPowerMgrProcess()
static void PowerMgrStartStateTransition(ePOWERSTATE eNewPowerState)
{
    // Record the next power state and keep the route ID for future use.
//[kyq Begin: 20150819 0:36:46]
    PPOWER_MANAGER_DATA         psPowerMgrData;
    PSYSTEM_SETTING_DATA         psSystemSettingData;
    BYTE OutPut, i;
    BYTE  cPowerOnOffMsgBaudRate;         //PWOFF_baud
    BYTE  acPowerOnOffMsg[PWON_OFF_Lenth];
    BYTE  cPowerOnOffMsgLength;
    psPowerMgrData = (PPOWER_MANAGER_DATA)utilDataMgrWriteLockData(nnPOWER_MGR_DATA);
    psPowerMgrData->eLastPwrState = eNewPowerState;
    utilDataMgrUnlock(nnPOWER_MGR_DATA, TRUE);
    m_wTransTimer = m_wPowerTimer;
    m_eNextPowerState = eNewPowerState;
//[kyq End: 20150819 0:36:48]
// Add any special processing such as messages signals here.
    switch (eNewPowerState)
    {
/* ---------------- BEGIN CUSTOM AREA [050] palPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
// 通知其他部件关电
// 发送数据给远端
    case pwrON:
       // if (m_ePowerState != pwrSTANDBY)
            UART1_SendData2PC("PWON", strlen("PWON"));
#ifdef PRD_POWER_ONOFF_MESSAGE
        for (OutPut = 0; OutPut < MAX_OUT; OutPut++)  //+2
        {
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
            cPowerOnOffMsgBaudRate = psSystemSettingData->acPowerOnMsgBaudRate[OutPut];
            cPowerOnOffMsgLength = psSystemSettingData->acPowerOnMsgLength[OutPut];
            memcpy(acPowerOnOffMsg, psSystemSettingData->acPowerOnMsg[OutPut], cPowerOnOffMsgLength);
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
            if (cPowerOnOffMsgLength > 0)
            {
    
                dvCpldSendDataToRemote(OutPut + 1,
                                       cPowerOnOffMsgBaudRate,
                                       acPowerOnOffMsg,
                                       cPowerOnOffMsgLength);
		  delay_nms(70);
                IWDG_ReloadCounter(); //喂狗
            }

        }
#endif
	 #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
	 dvM21452PowerSet(TRUE);
    	 #else
    	 dvADN4604PowerSet(TRUE);
    	 #endif
        appSystemSwitcherSetStart();
#ifdef PRD_IR_OUT
        appSystemIRSwitcherSetStart();
#endif
		
        m_ePowerState = pwrON;
        appUIStateChange(uiSPLASH); // add by kyq 20151210
        break;
    case pwrSTANDBY:
        //appUIStateChange(uiIDLE);// if here to IDLE, the OSD will be not display //[kyq CANCEL: 20150907 20:41:55]
        dvLedOnSet(1 << elnRED);
        dvLedOffSet(1 << elnGREEN);
        UART1_SendData2PC("STANDBY", strlen("STANDBY"));
	 #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
	 dvM21452PowerSet(FALSE);
	 appSystemSwitcherOutportForceCloseAll ();
    	 #else
    	 dvADN4604PowerSet(FALSE);
    	 #endif
#ifdef PRD_POWER_ONOFF_MESSAGE
        for (OutPut = 0; OutPut < MAX_OUT; OutPut++)  //+2
        {
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
            cPowerOnOffMsgBaudRate = psSystemSettingData->acPowerOffMsgBaudRate[OutPut];
            cPowerOnOffMsgLength = psSystemSettingData->acPowerOffMsgLength[OutPut];
            memcpy(acPowerOnOffMsg, psSystemSettingData->acPowerOffMsg[OutPut], cPowerOnOffMsgLength);
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
            if (cPowerOnOffMsgLength > 0)
            {
     
                // note : if use acRemoteBaudRate then must unlock first
                dvCpldSendDataToRemote(OutPut + 1,
                                       cPowerOnOffMsgBaudRate,
                                       acPowerOnOffMsg,
                                       cPowerOnOffMsgLength);
		 delay_nms(70);
		  
            }
        }
#endif
        break;
    case pwrOFF:
    case pwrSOFTOFF:
        //appUIStateChange(uiIDLE); //[kyq CANCEL: 20150907 20:41:55]
        UART1_SendData2PC("PWOFF", strlen("PWOFF"));
	 #if ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
	 dvM21452PowerSet(FALSE);
    	 #else
    	 dvADN4604PowerSet(FALSE);
    	 #endif
#ifdef PRD_POWER_ONOFF_MESSAGE
        for (OutPut = 0; OutPut < MAX_OUT; OutPut++)  //+2
        {
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
            cPowerOnOffMsgBaudRate = psSystemSettingData->acPowerOffMsgBaudRate[OutPut];
            cPowerOnOffMsgLength = psSystemSettingData->acPowerOffMsgLength[OutPut];
            memcpy(acPowerOnOffMsg, psSystemSettingData->acPowerOffMsg[OutPut], cPowerOnOffMsgLength);
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
            if (cPowerOnOffMsgLength > 0)
            {
      
                dvCpldSendDataToRemote(OutPut + 1,
                                       cPowerOnOffMsgBaudRate,
                                       acPowerOnOffMsg,
                                       cPowerOnOffMsgLength);
		 delay_nms(70);
            }
        }
#endif
        break;
    case pwrSOFTRESET:
        appUIStateChange(uiIDLE);
        break;
    default:
        break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
    }
    //utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
}
// ---------------------------------------------------------------------------
// This API sets the internal state of the component. The local state
// transition timers are initialized and the first power transition from
// pwrOFF or pwrSOFTOFF to pwrON is started. Custom areas are provided to
// allow the OEM to automatically enter any of the defined power states if the
// system is first powering up.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void appPowerMgrInitialize(void)
{
    ePOWERSTATE     eLastBootPowerState;
    //BOOL            bSuspendExitOnUserInput;
    //BYTE            i;
     PSYSTEM_SETTING_DATA         psSystemSettingData;
    BYTE OutPut;
    BYTE  cPowerOnOffMsgBaudRate;         //PWOFF_baud
    BYTE  acPowerOnOffMsg[PWON_OFF_Lenth];
    BYTE  cPowerOnOffMsgLength;
    PPOWER_MANAGER_DATA psPowerMgrData;
    psPowerMgrData = (PPOWER_MANAGER_DATA)utilDataMgrReadLockData(nnPOWER_MGR_DATA);
    // Before we let the system wake up see how we got here.
    eLastBootPowerState  = psPowerMgrData->eLastPwrState;
    utilDataMgrUnlock(nnPOWER_MGR_DATA, FALSE);
    // We must keep track of how we exited the low power suspend loop. If the
    // timer timed out then we will look for a signal and go back to low power.
    // If we got user input then we will turn off.
    //bSuspendExitOnUserInput = halPowerCtrlGetLastSuspendExit();
    // Start with all the local timers at 0.
    m_wTransTimer   = 0;
    m_wPowerTimer   = 0;
    m_wSecondsTimer = 0;
    m_wLastSecondsTimer = 0;
    // Until this is well defined for a specific OEM board the state transitions
    // will assume all routes. Another words this is not used at the other end.
    // There is no constant for all routes so use 0.
    //m_cRouteForTransition = 0;
    //utilExecCounterSet(coPOWER_MGR,
    //                   MAX_COUNTER);
    //m_wLastPowerCounter = MAX_COUNTER;
    SiiPlatformTimerSet(etPOWERCNT,
                        MAX_COUNTER,0xFF);
    m_wLastPowerCounter = MAX_COUNTER;
    switch (eLastBootPowerState)
    {
/* ---------------- BEGIN CUSTOM AREA [130] palPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
    case pwrON:
        m_eLastPowerState = pwrON;
        m_ePowerState =        pwrON;
        m_eNextPowerState = pwrON; // let continue to process
        break;
    case pwrOFF:
        // The system just powered up for the first time. */
        m_eLastPowerState = pwrOFF;
        // Do we just want to wake up or Head to low power mode and
        // wait for "on" request. For now wake up until the user
        // hits the off button or we DPMS suspend.
        m_ePowerState =        pwrOFF;
        m_eNextPowerState = pwrOFF;
        break;
    case pwrSOFTOFF:
        m_eLastPowerState = pwrSOFTOFF;
        m_ePowerState =        pwrSOFTOFF;
        m_eNextPowerState = pwrSOFTOFF;
        break;
    case pwrSTANDBY:
        m_eLastPowerState = pwrSTANDBY;
        m_ePowerState =        pwrSTANDBY;
        m_eNextPowerState = pwrSTANDBY;
        break;
    default:
        break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
    }
    //m_eNextPowerState = pwrSTANDBY;
/* ---------------- BEGIN CUSTOM AREA [140] palPowerMgr.c ----------------- */
// See if we need to head straight into soft off to wait for an on button or
// the IR on button or some other type of message.
    if (m_eNextPowerState == pwrSTANDBY)
    {
        UART1_SendData2PC("STANDBY", strlen("STANDBY"));
    }
    else if (m_eNextPowerState == pwrOFF || m_eNextPowerState == pwrSOFTOFF)
    {
        UART1_SendData2PC("PWOFF", strlen("PWOFF"));
    }

  // here need wait Gui to get infor?
   halPowerCtrlPowerSet(m_eNextPowerState);
   if(eLastBootPowerState != pwrON)   //[kyq add: 20151014 16:15:51]
   {
      halPowerCtrlPowerSet(pwrON);
   }

  //  if (m_eNextPowerState == pwrON)
    {
        UART1_SendData2PC("PWON", strlen("PWON"));
    }
	
#ifdef PRD_POWER_ONOFF_MESSAGE
        for (OutPut = 0; OutPut < MAX_OUT; OutPut++)  //+2
        {
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
            cPowerOnOffMsgBaudRate = psSystemSettingData->acPowerOnMsgBaudRate[OutPut];
            cPowerOnOffMsgLength = psSystemSettingData->acPowerOnMsgLength[OutPut];
            memcpy(acPowerOnOffMsg, psSystemSettingData->acPowerOnMsg[OutPut], cPowerOnOffMsgLength);
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
            if (cPowerOnOffMsgLength > 0)
            {
               delay_nms(500);
                /*
                    send_data_to_FPGA(0xEE); //通知CPLD置为标志位，那么下面发送数据给PC时就不会再发给远端402了
                    send_data_to_FPGA(OutPut + 1);
                    send_data_to_FPGA(psSystemSettingData->acPowerOnMsgBaudRate[OutPut] - 1);
                    for (i = 0; i < psSystemSettingData->acPowerOnMsgLength[OutPut]; i++)  
                send_data_to_FPGA(psSystemSettingData->acPowerOnMsg[OutPut][i]);
                    send_data_to_FPGA(0xEF);
                    delay_nms(50);
                   */
                dvCpldSendDataToRemote(OutPut + 1,
                                       cPowerOnOffMsgBaudRate,
                                       acPowerOnOffMsg,
                                       cPowerOnOffMsgLength);
		 
                IWDG_ReloadCounter(); //喂狗
            }
        }
#endif
  psPowerMgrData = (PPOWER_MANAGER_DATA)utilDataMgrWriteLockData(nnPOWER_MGR_DATA);
  // if code run here it's power on
  psPowerMgrData->eLastPwrState = pwrON;
   utilDataMgrUnlock(nnPOWER_MGR_DATA, TRUE);
   m_eLastPowerState = pwrON;  //[kyq Begin: 20151014 16:34:56]
   m_ePowerState =        pwrON;
   m_eNextPowerState = pwrON; // let continue to process
/* ---------------------------- END CUSTOM AREA --------------------------- */
}
// ---------------------------------------------------------------------------
// This API updates the local timer used in the state transitions
//
// params: none.
//
// Returns: nothing
// ---------------------------------------------------------------------------
//
//-----> appPowerMgrProcess()  -------> main()
//----->appPowerMgrPowerStateSet()
static void PowerMgrUpdateLocalTimer(void)
{
    WORD                wTempCounter;
    wTempCounter = SiiPlatformTimerGet(etPOWERCNT); //(etPOWERCNT);
    m_wPowerTimer += (m_wLastPowerCounter - wTempCounter);
    if (m_wPowerTimer - m_wLastSecondsTimer >= TICKS_PER_SECOND)
    {
        m_wSecondsTimer += 1;
        m_wLastSecondsTimer = m_wPowerTimer;
    }
    SiiPlatformTimerSet(etPOWERCNT,
                        MAX_COUNTER,0xFF);
    m_wLastPowerCounter = MAX_COUNTER;
}
static void PowerMgrUpdateLocalTimerReset(void)
{
    //WORD                wTempCounter;
    //wTempCounter = SiiPlatformTimerGet(etPOWERCNT); //(etPOWERCNT);
    SiiPlatformTimerSet(etPOWERCNT,
                        MAX_COUNTER,0xFF);
    m_wLastPowerCounter = MAX_COUNTER;
    m_wPowerTimer =0;
    m_wLastSecondsTimer = 0;
}
// ---------------------------------------------------------------------------
// This API is called by the cyclic processing routine to update any NVRAM
// counter/timers used by DPMS or the system logger.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
static void PowerMgrUpdateParameterData(void)
{
/* ---------------- BEGIN CUSTOM AREA [040] palPowerMgr.c ----------------- */
    POWER_MANAGER_DATA *psPowerMgrData;
    // If we have been on for another second then update the NVRAM node.
    if (m_wSecondsTimer - m_wLastUnitOnSeconds >= NVRAM_SECONDS_PER_TICK)  // 一分钟
    {
        // Lock and read the NVRAM data.
        psPowerMgrData = (PPOWER_MANAGER_DATA)utilDataMgrWriteLockData(nnPOWER_MGR_DATA);
        // If we get here the unit is on.
        psPowerMgrData->wUnitOnTime += 1;
        // The lamp will be off in pwrSTANDBY, pwrSUSPEND, pwrSOFTOFF
        // if (pwrON == m_ePowerState)
        //    psPowerMgrData->wLampOnTime += 1;
        if (psPowerMgrData->wUnitOnTime > 0x5a0)
        {
            psPowerMgrData->wUnitOnDays++;
            psPowerMgrData->wUnitOnTime = 0;
        }
        // Unlock the data node
        utilDataMgrUnlock(nnPOWER_MGR_DATA, TRUE);
        m_wLastUnitOnSeconds = m_wSecondsTimer;
    }
/* ---------------------------- END CUSTOM AREA --------------------------- */
}
// ---------------------------------------------------------------------------
// This API manages transitions from pwrON to pwrSTANDBY, pwrSUSPEND,
// pwrSOFTOFF Any delays or special message processing should be done here in
// advance of calling the hal routins to actually change the power state.
//
// Params:
//  eNextPowerState : The power state to transition to.
//
// Return: none
// ---------------------------------------------------------------------------
//
//---->appPowerMgrProcess()
//
static void ProcessPowerOnTransition(ePOWERSTATE eNextPowerState)
{
    // Valid transitions from pwrON include pwrSTANDBY, pwrSUSPEND, pwrSOFTOFF
    switch (eNextPowerState)
    {
/* ---------------- BEGIN CUSTOM AREA [060] palPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
    case pwrSTANDBY:
        // If we are paused by user input then stop the transition so they
        // can continue to see the display.
        if (m_wPowerTimer - m_wTransTimer > STANDBY_DELAY)
        {
            appUIEnable(FALSE); //[kyq ADD: 20150907 20:40:51]
            utilDataMgrUpdate(UPDATE_ALL_NODEID);
            halPowerCtrlPowerSet(pwrSTANDBY);
            m_ePowerState = pwrSTANDBY; // from power standby to power on
            m_eNextPowerState = pwrON; //[kyq add: 20150819 0:45:47]
            // got to  appPowerMgrProcess() ->ProcessStandbyTransition()  first
            // then go to PowerMgrStartStateTransition(pwrON);
            PowerMgrUpdateLocalTimerReset();
        }
        break;
    case pwrSOFTOFF:
        // The user just hit the power button so we need to force a system
        // shutdown and go into the low power monitor loop in
        // dvPowerCtrl.c - Wait a while for the buttons to settle.
        if (m_wPowerTimer - m_wTransTimer > SOFTOFF_DELAY)
        {
            // update all data node to prevent data lost
            appUIEnable(FALSE); //[kyq ADD: 20150907 20:40:51]
            utilDataMgrUpdate(UPDATE_ALL_NODEID);
            halPowerCtrlPowerSet(pwrSOFTOFF);
	     m_ePowerState = pwrSOFTOFF;
            m_eNextPowerState = pwrON; //[kyq add: 20150819 0:45:47]
            // got to  appPowerMgrProcess() ->ProcessSoftOffTransition() first
            // then go to PowerMgrStartStateTransition(pwrON);
            halPowerCtrlPowerSet(pwrON); // kyq 20150929
            PowerMgrUpdateLocalTimerReset();
            m_wTransTimer = m_wPowerTimer; // kyq 20150929
        }
        break;
    case pwrSOFTRESET:
        // The user just select the factory parameter reset. Allow the
        // NVRAM update to complete and then restart.
        if (m_wPowerTimer - m_wTransTimer > SOFTRESET_DELAY)
        {
            appUIEnable(FALSE); //[kyq ADD: 20150907 20:40:51]
            utilDataMgrUpdate(UPDATE_ALL_NODEID);
            halPowerCtrlPowerSet(pwrSOFTRESET);
            m_eNextPowerState = pwrON; //[kyq add: 20150819 0:45:47]
            PowerMgrUpdateLocalTimerReset();
        }
        break;
    default:
        break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
    }
}
// ---------------------------------------------------------------------------
// This API manages transitions from pwrSTANDBY to pwrON,  pwrSUSPEND,
// pwrSOFTOFF Any delays or special message processing should be done here in
// advance of calling the hal routins to actually change the power state.
//
// Params:
//  eNextPowerState : The power state to transition to.
//
// Return: none
// ---------------------------------------------------------------------------
static void ProcessStandbyTransition(ePOWERSTATE eNextPowerState)
{
    // Valid transitions from standby to pwrON, pwrSUSPEND and pwrSOFTOFF
    switch (eNextPowerState)
    {
/* ---------------- BEGIN CUSTOM AREA [070] palPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
    case pwrON:
        halPowerCtrlPowerSet(pwrON);
       // m_ePowerState = m_eNextPowerState; //// 不改变状态，在PowerMgrStartStateTransition 后改变
       //appPowerMgrPowerStateSet(pwrON);
       delay_nms(1000);
       PowerMgrStartStateTransition(pwrON);
        break;
    case pwrSOFTOFF:
        // The user just hit the power button so we need to force a system
        // shutdown and go into the low power monitor loop in
        // dvPowerCtrl.c - Wait 100 mS for the buttons to settle.
        // Add power down delay if needed
        if (m_wPowerTimer - m_wTransTimer > SOFTOFF_DELAY)
        {
            halPowerCtrlPowerSet(pwrSOFTOFF);
        }
        break;
    default:
        break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
    }
}
// ---------------------------------------------------------------------------
// This API manages transitions from pwrOFF to pwrSOFTOFF and pwrON Any delays
// or special message processing should be done here in advance of calling the
// hal routinEs to actually change the power state.
//
// Params:
//  m_eNextPowerState : The power state to transition to.
//
// Return: none
// ---------------------------------------------------------------------------
static void ProcessOffTransition(ePOWERSTATE m_eNextPowerState)
{
    switch (m_eNextPowerState)
    {
/* ---------------- BEGIN CUSTOM AREA [100] palPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
// Valid transition from pwrSOFTOFF is only pwrON
// For now we are not doing anything special just set the state to on.
    case pwrON:
        if (m_wPowerTimer - m_wTransTimer > POWER_ON_DELAY)
        {
            //m_ePowerState = m_eNextPowerState; // 不改变状态，在PowerMgrStartStateTransition 后改变
            halPowerCtrlPowerSet(pwrON);
	     PowerMgrStartStateTransition(pwrON);
        }
        break;
// if the power key press early? then power off it.
    case pwrSOFTOFF:
        halPowerCtrlPowerSet(pwrSOFTOFF);
        break;
    default:
        break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
    }
}
// ---------------------------------------------------------------------------
// This API manages transitions from pwrSOFTOFF to pwrON
// Any delays or special message processing should be done here in advance of
// calling the hal routinEs to actually change the power state.
//
// params: eNextPowerState - The power state to transition to.
//
// Returns: nothing
// ---------------------------------------------------------------------------
//
//   ---->appPowerMgrProcess()
//   softoff ----> poweron :first
static void ProcessSoftOffTransition(ePOWERSTATE eNextPowerState)
{
    switch (eNextPowerState)
    {
/* ---------------- BEGIN CUSTOM AREA [090] palPowerMgr.c ----------------- */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
// Valid transition from pwrSOFTOFF is only pwrON
// For now we are not doing anything special just set the state to on.
    case pwrON:
        if (m_wPowerTimer - m_wTransTimer > POWER_ON_DELAY)
        {
            //m_ePowerState = eNextPowerState;// 不改变状态，在PowerMgrStartStateTransition 后改变
            //halPowerCtrlPowerSet(pwrON); kyq 20150929 , poweroff -> poweron need more time to wait link stable
	     PowerMgrStartStateTransition(pwrON); // 在PowerMgrStartStateTransition 后改变
        }
        break;
    default:
        break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
    }
}
void appPowerMgrProcess(void)
{
    PowerMgrUpdateLocalTimer();
    // update timer and NVRAM counters.
    PowerMgrUpdateParameterData(); // 经过一分钟更新NVRAM 记录
    // Process any active transitions
    if (m_ePowerState != m_eNextPowerState)
    {
        switch (m_ePowerState)
        {
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
        case pwrON:
            ProcessPowerOnTransition(m_eNextPowerState);
            break;
        case pwrSTANDBY:
            ProcessStandbyTransition(m_eNextPowerState);
            break;
            //case pwrSUSPEND:
            //    ProcessSuspendTransition(m_eNextPowerState);
            // break;
        case pwrOFF:
            ProcessOffTransition(m_eNextPowerState);
            break;
        case pwrSOFTOFF:
            ProcessSoftOffTransition(m_eNextPowerState);
            break;
        default:
            break;
/* ---------------------------- END CUSTOM AREA --------------------------- */
        }
    }
}
// This API forces a change from the current power state to the new state
// provided by the caller. The Route Id is included to provide a mechanism to
// power down all components for just the given route.
//
// Params:
//
//  eNewState : The desired new power state
//
// Return: none
// ---------------------------------------------------------------------------
void appPowerMgrPowerStateSet(ePOWERSTATE eNewState)
{
    PowerMgrUpdateLocalTimer();
    // Kick off the state transition through the common local routine used by
    // the automatic state monitoring logic.
    // Don't allow a transition to the same state while a transition is in
    // progress. This can happen in the case of some slow devices like IR
    // where we are waking up and there are extra ON button key presses
    // present even though we have not officially transitioned to pwrON yet.
    if (m_eNextPowerState != eNewState && m_ePowerState != eNewState)
    {
        PowerMgrStartStateTransition(eNewState);
    }
}
//  Init the power manage node
//
void appPowerMgrNodeCallback(eNODE_NAME eNodeID,
                             eNV_EVENT  eEvent)
{
    //BYTE i;
    switch (eEvent)
    {
    case nveRESETALL:
    case nveINITIALIZE:
        switch (eNodeID)
        {
/* -------------- BEGIN CUSTOM AREA [420] palWindowMgrPop.c ------------------
 * Add parameter nodes used in the system here. */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
        case nnPOWER_MGR_DATA:
            {
                PPOWER_MANAGER_DATA         psPowerMgrData;
                psPowerMgrData = (PPOWER_MANAGER_DATA)utilDataMgrWriteLockData(eNodeID);
                psPowerMgrData->eLastPwrState = pwrON;
                psPowerMgrData->wUnitOnTime = 0;
                psPowerMgrData->wOnTime = 0;
                psPowerMgrData->wUnitOnDays = 0;
                utilDataMgrUnlock(eNodeID,
                                  TRUE);
            }
            break;
        default:    // unsupported data node
            ASSERT_ALWAYS();
/* ---------------------------- END CUSTOM AREA --------------------------- */
        }
        break;
    default:                      // unsupported event
        ASSERT_ALWAYS();
    }
}
//[kyq Begin: 20151014 16:40:13]
ePOWERSTATE  appPowerMgrLastPowerStateGet(void)
{
     return m_eLastPowerState;
}
//[kyq End: 20151014 16:40:15]
