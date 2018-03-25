
#include "SwitchVideoReceiveEvent.h"
#include "si_logger.h"

bool_t SwitchVideoRxMsgHandler (SendData_t *pMsg )
{
    bool_t  usedMessage=false     ;
    Event_t sSwVideoEvent;
    BYTE acOutPortList[MAX_OUT];
    //BYTE acMessage1[LCD_MAX_CHAR] = { 0 };
    // AV:00->01,02,03,04,05,06,07,08.
    BYTE acMessage2[USART1_TX_BUFF_MAX] = { 0 };
    BYTE cCount = 0;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    BYTE InPut = 0, OutPut = 0,PortNumber=0;;
    Switch_EVENT  SwitchAllThrough;   // static data
    pSwitch_EVENT  pSwitchxClose;   //msg send data ,but only  Output
    pSwitch_EVENT  pSwitchxOpen;   //msg send data ,but only  Output
    /* ---------------------------- END CUSTOM AREA --------------------------- */
    if (utilExecMsgPreRead(mqSWITCH))
    {
        utilExecMsgDataRead(mqSWITCH,
                            &sSwVideoEvent);
        /* Add code here to perform a post read operation on the user input queue */
        // utilExecMsgClear(mqSWITCH);
        /* ---------------------------- END CUSTOM AREA --------------------------- */
    }
    else
    {
        return usedMessage;
    }
// ASSERT((pMsg->srcDestAddr & mqSWITCH ) == mqSWITCH);  // no check address of the module
    usedMessage         = true;
    switch(sSwVideoEvent.Head.opcode)
    {
    case  eSwitchDemoMode:
    {
        PDemo_EVENT  DemoEvent;
        DemoEvent=(Demo_EVENT *)&sSwVideoEvent.args;
        appSystemSwitchDemoModeEnableSet(DemoEvent->Enable);
    }
    break;
    case  eSwitchAllThrough:
        SwitchAllThrough.xOutNum=0;
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
//get actual  switch channel
        for (OutPut = 0; OutPut < MAX_OUT; OutPut++)
        {
            InPut = OutPut;
            if(psSystemSettingData->acIChannellock[InPut]&BIT(OutPut))
            {
                sprintf(USART1_tx_buff,"Channel %d->%d Lock!\r\n",InPut+1,OutPut+1);
                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
            }
            else
            {
                //  appSystemSwitcherSet(InPut, 1, OutPut); //ALL#  so long time , move this Line  back
                SwitchAllThrough.xOutPort[SwitchAllThrough.xOutNum]=OutPut;
                SwitchAllThrough.xOutNum++;
            }
#ifdef PRD_IR_OUT
            appSystemIRSwitcherSet(InPut, 1, OutPut); //ALL#.
#endif
        }
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
//printf  information
        if( SwitchAllThrough.xOutNum==MAX_OUT)
        {
            cCount += SiiSprintf(&acMessage2[cCount],"%s", "All Through.");
            UART1_SendData2PC(acMessage2, strlen(acMessage2));
            for (OutPut = 0; OutPut < MAX_OUT; OutPut++)
            {
                 InPut = OutPut;
                appSystemSwitcherSet(InPut, 1, OutPut);    //ALL#
            }
        }
        else
        {
            for (OutPut = 0; OutPut < SwitchAllThrough.xOutNum; OutPut++)
            {
                cCount=0;
                cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:%2d->%2d", SwitchAllThrough.xOutPort[OutPut]+1,SwitchAllThrough.xOutPort[OutPut]+1);
                UART1_SendData2PC(acMessage2, strlen(acMessage2));
		  InPut =SwitchAllThrough.xOutPort[OutPut];
                appSystemSwitcherSet(InPut, 1, SwitchAllThrough.xOutPort[OutPut]);
            }
        }
        break;
    case  eSwitchUndo:
        appSystemSwitcherUndo();
        break;
    case   eSwitchxClose:
        pSwitchxClose=(Switch_EVENT *)&sSwVideoEvent.args;
        OutPut=pSwitchxClose->xOutputPort;
	cCount += SiiSprintf(&acMessage2[cCount], "%2d Closed.", OutPut+1);
        UART1_SendData2PC(acMessage2, strlen(acMessage2));
        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
        psOutputPortMgrData->acCloseInPortSelect[OutPut ] = psOutputPortMgrData->acInPortSelect[OutPut ];
        utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,TRUE);
        appSystemSwitcherSet(MAX_IN, 1, OutPut);
   
        break;
    case  eSwitchxOpen:
        pSwitchxOpen=(Switch_EVENT *)&sSwVideoEvent.args;
        OutPut=pSwitchxOpen->xOutputPort;
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        if (psSystemSettingData->bUartReturnMessage_EN  == ON)
        {
            cCount += SiiSprintf(&acMessage2[cCount], "%02d Open.", OutPut+1);
            UART1_SendData2PC(acMessage2, strlen(acMessage2));
        }
        else
        {
            cCount += SiiSprintf(&acMessage2[cCount], "Switch Ok!");
            UART1_SendData2PC(acMessage2, strlen(acMessage2));
        }
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
        psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
        if ((psOutputPortMgrData->acInPortSelect[OutPut ]) >= MAX_IN)
        {
            utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,TRUE);
            appSystemSwitcherSet(psOutputPortMgrData->acCloseInPortSelect[OutPut ], 1, OutPut );
        }
        else
        {
            utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,TRUE);
        }
        break;
    case eSwitchAllOpen:
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        if (psSystemSettingData->bUartReturnMessage_EN == ON)
        {
    
            cCount += SiiSprintf(&acMessage2[cCount],"%s", "All Open.");
            UART1_SendData2PC(acMessage2, strlen(acMessage2));
        }
        else
        {
            cCount += SiiSprintf(&acMessage2[cCount],"%s", "Switch Ok!");
            UART1_SendData2PC(acMessage2, strlen(acMessage2));
        }
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
        appSystemSwitcherOutportRecoverAll();// Video and IR
        break;
    case eSwitchAllClose:
        cCount += SiiSprintf(&acMessage2[cCount], "%s","All Closed.");
        UART1_SendData2PC(acMessage2, strlen(acMessage2));
        appSystemSwitcherOutportCloseAll(); // video and IR
        break;
    case  eSwitchxThrough:
    {
        pSwitch_EVENT  pSwitchxThrough;   //msg send data ,but only  InPut
        Switch_EVENT  SwitchxThrough;   // static data
        pSwitchxThrough=(Switch_EVENT *)&sSwVideoEvent.args;
        InPut=pSwitchxThrough->xInPort;
        SwitchxThrough.xInPort=pSwitchxThrough->xInPort;
        SwitchxThrough.xOutNum=0;
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        OutPut=   InPut; //   Through  switch
        if((psSystemSettingData->acIChannellock[InPut]&BIT(OutPut))&&(InPut<MAX_IN))
        {
            sprintf(USART1_tx_buff,"Channel %d->%d Lock!\r\n",InPut+1,OutPut+1);
            UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
        }
        else
        {
            cCount += SiiSprintf(&acMessage2[cCount], "%2d Through.", InPut+1);
            UART1_SendData2PC(acMessage2, strlen(acMessage2));
            SwitchxThrough.xOutPort[SwitchxThrough.xOutNum]=OutPut;
            SwitchxThrough.xOutNum++;
        }
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
        appSystemSwitcherPortSet(SwitchxThrough.xInPort, SwitchxThrough.xOutNum, SwitchxThrough.xOutPort);
    }
    break;
    case  eSwitchxAll:
    {
        pSwitch_EVENT  pSwitchxAll;   //msg send data ,but only  InPut
        Switch_EVENT  SwitchxAll;   // static data
        pSwitchxAll=(Switch_EVENT *)&sSwVideoEvent.args;
        InPut=pSwitchxAll->xInPort;
        SwitchxAll.xInPort=pSwitchxAll->xInPort;
        SwitchxAll.xOutNum=0;
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        if(InPut+1>MAX_IN)
        {
            cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:OFF->");
        }
        else
        {
            cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:%2d->", InPut+1);
        }
        for(OutPut=0; OutPut<MAX_OUT; OutPut++)
        {
            if((psSystemSettingData->acIChannellock[InPut]&BIT(OutPut))&&(InPut<MAX_IN))
            {
                sprintf(USART1_tx_buff,"Channel %d->%d Lock!\r\n",InPut+1,OutPut+1);
                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
            }
            else
            {
                cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d,",OutPut + 1);
                SwitchxAll.xOutPort[SwitchxAll.xOutNum]=OutPut;
                SwitchxAll.xOutNum++;
            }
        }
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
        if(SwitchxAll.xOutNum==MAX_OUT)
        {
            cCount=0;
            if(InPut+1>MAX_IN)
            {
                cCount += SiiSprintf((char*)&acMessage2[cCount], "OFF To All.");
            }
            else
            {
                cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d To All.", InPut+1);
            }
            UART1_SendData2PC(acMessage2, cCount);
        }
        else if(SwitchxAll.xOutNum==0)// all lock
        {
            ;
        }
        else
        {
            UART1_SendData2PC(acMessage2, strlen(acMessage2)-1);    //remove last  ","
        }
        appSystemSwitcherPortSet(SwitchxAll.xInPort, SwitchxAll.xOutNum, SwitchxAll.xOutPort);
    }
    break;
    case  eSwitchxTox:
    {
        pSwitch_EVENT  pSwitchxTox;//msg send data
        Switch_EVENT  SwitchxTox;   // static data
        BOOL   m_abSwitchOutPortSelect[MAX_OUT];  // 切换 下那些输出口被选
        memset(m_abSwitchOutPortSelect,0,MAX_OUT);//clear all
        for(OutPut=0; OutPut<MAX_OUT; OutPut++)
        {
            m_abSwitchOutPortSelect[OutPut]=FALSE;
        }
        //##############
        pSwitchxTox=(Switch_EVENT *)&sSwVideoEvent.args;
        InPut=pSwitchxTox->xInPort;
        PortNumber=pSwitchxTox->xOutNum;//must like this ,otherwise  printf error
        SwitchxTox.xOutNum=0;
        SwitchxTox.xInPort=pSwitchxTox->xInPort;
        psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
        /**********************************************************************************************/
        //how many output enable ???  goto printf information
        for(OutPut=0; OutPut<PortNumber; OutPut++)
        {
            m_abSwitchOutPortSelect[pSwitchxTox->xOutPort[OutPut]]=TRUE;
        }
        /***********************************************************************************************/
        // print  information
        if(InPut+1>MAX_IN)
        {
            cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:OFF->");
        }
        else
        {
            cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:%2d->", InPut+1);
        }
        for(OutPut=0; OutPut<MAX_OUT; OutPut++)
        {
            if(!m_abSwitchOutPortSelect[OutPut])
            {
                continue;
            }
            if((psSystemSettingData->acIChannellock[InPut]&BIT(OutPut))&&(InPut<MAX_IN))
            {
                sprintf(USART1_tx_buff,"Channel %d->%d Lock!\r\n",InPut+1,OutPut+1);
                UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
            }
            else
            {
                cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d,",OutPut+ 1);
                SwitchxTox.xOutPort[SwitchxTox.xOutNum]=OutPut;
                SwitchxTox.xOutNum++;
            }
        }
        utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
        if(SwitchxTox.xOutNum==MAX_OUT)
        {
            cCount=0;
            if(InPut+1>MAX_IN)
            {
                cCount += SiiSprintf((char*)&acMessage2[cCount], "OFF To All.");
            }
            else
            {
                cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d To All.", InPut+1);
            }
            UART1_SendData2PC(acMessage2, cCount);
        }
        else if(SwitchxTox.xOutNum==0)// all lock
        {
            ;
        }
        else
        {
            UART1_SendData2PC(acMessage2, strlen(acMessage2)-1);    //remove last  ","
        }
        appSystemSwitcherPortSet(SwitchxTox.xInPort, SwitchxTox.xOutNum, SwitchxTox.xOutPort);
    }
    break;
    default:
        break;
    }
    return( usedMessage );
}
bool_t SwitchVideoProcessHandler (void)
{
    appSystemSwitchDemoModeProcess();
    return true;
}

static bool_t halSwitchVideoInitialize(void)
{       bool_t  error=0;
   PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    if(!SwitchVideoinit())
    {
	#if INC_UART	 
      if(psSystemSettingData->cPOSTEnable)
	 UART1_SendData2PC("SwitchVideoInitialize fail!", strlen("SwitchVideoInitialize fail!"));
       #endif

    }
 utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);   
//  appSystemSwitcherSetStart();
    return 1;
}
bool_t Init_SwitchVideo_module(BYTE MsgId)
{
    bool_t      success = true;

    if(!halSwitchVideoInitialize())
    {
        success = false;

    }
    if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,SwitchVideoRxMsgHandler ,true))   //注册事件
    {
        success = false;
    }
    if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,SwitchVideoProcessHandler ,true))  //注册任务
    {
        success = false;
    }
    return success;
}
