#include "SwitchVideoReceiveEvent.h"
#include "KeyTasks.h"
#include "dvTM1623.h"


bool_t SwitchVideoRxMsgHandler (SendData_t *pMsg )
{

	bool_t  usedMessage=false     ;
	Event_t sSwVideoEvent;
	//BYTE acOutPortList[MAX_OUT];
	//BYTE acMessage1[LCD_MAX_CHAR] = { 0 };
    // AV:00->01,02,03,04,05,06,07,08.
	BYTE acMessage2[USART1_TX_BUFF_MAX] = {0xA5,0x5B,0x02,0x03, 0x02 ,0x00 ,0x01, 0x00 ,0x00 ,0x00 ,0x00 ,0x00};
	BYTE cCount = 0;
	PSYSTEM_SETTING_DATA   psSystemSettingData;
	BYTE InPut = 0, OutPut = 0,PortNumber=0;
	BYTE eflag;
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
        return usedMessage;

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
            {
                Switch_EVENT  SwitchAllThrough;   // static data
                SwitchAllThrough.xOutNum=0;
    		 
        		psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
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
                        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                        KeyLedSetOne(OutPut,InPut);
                        #endif
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
                        appSystemSwitcherSet(InPut, 1, OutPut);     //ALL#
                    }
                }
                else
                {      
                    for (OutPut = 0; OutPut < SwitchAllThrough.xOutNum; OutPut++)
                    {
                        cCount=0;
                        cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:%2d->%2d", SwitchAllThrough.xOutPort[OutPut]+1,SwitchAllThrough.xOutPort[OutPut]+1);
                        UART1_SendData2PC(acMessage2, strlen(acMessage2));
                        InPut = SwitchAllThrough.xOutPort[OutPut];
                        appSystemSwitcherSet(InPut, 1, OutPut);
                    }
                }
            }
    		break;
    	case  eSwitchUndo:
           	appSystemSwitcherUndo();
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

		        OutPut  =   InPut; //	Through  switch
    		
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
                    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                    KeyLedSetOne(OutPut,InPut);
                    #endif
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
    		        cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:%2d->", InPut+1);
    		    for(OutPut=0;OutPut<MAX_OUT;OutPut++)    
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
                        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                        KeyLedSetOne(OutPut,InPut);
                        #endif
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
    				    cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d To All.", InPut+1);
    				UART1_SendData2PC(acMessage2, cCount); 
    			}
    		   	else if(SwitchxAll.xOutNum==0)// all lock
    				;
    		   	else
    		   		UART1_SendData2PC(acMessage2, strlen(acMessage2)-1);  //remove last  ","	
    		       
    			appSystemSwitcherPortSet(SwitchxAll.xInPort, SwitchxAll.xOutNum, SwitchxAll.xOutPort);
    		}
    		break;
    	
        case  eSwitchxTox:
            {
                pSwitch_EVENT  pSwitchxTox;//msg send data 
                Switch_EVENT  SwitchxTox;   // static data
                BOOL   m_abSwitchOutPortSelect[MAX_OUT];  // 切换 下那些输出口被选
                //memset(m_abSwitchOutPortSelect,0,MAX_OUT);//clear all
                for(OutPut=0;OutPut<MAX_OUT;OutPut++)    
                    m_abSwitchOutPortSelect[OutPut]=FALSE;
                //##############
                pSwitchxTox=(Switch_EVENT *)&sSwVideoEvent.args;
                InPut=pSwitchxTox->xInPort&0x2F;  
		  eflag=pSwitchxTox->xInPort&0xE0;     //Hight 3 bit  ,decide  Feedback information to print
                PortNumber=pSwitchxTox->xOutNum;//must like this ,otherwise  printf error
                SwitchxTox.xOutNum=0;
                SwitchxTox.xInPort=InPut;  
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
            
                /**********************************************************************************************/
                //how many output enable ???  goto printf information 
                for(OutPut=0;OutPut<PortNumber;OutPut++)    
                    m_abSwitchOutPortSelect[pSwitchxTox->xOutPort[OutPut]]=TRUE;
            
                /***********************************************************************************************/   
                // print  information 
                if(InPut+1>MAX_IN)
                {
                    cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:OFF->");
                }
                else
                {
                      if(eflag==eVideoSWITCH_HEX) 	//olny one
                          acMessage2[4]=InPut+1;
			 else
                        cCount += SiiSprintf((char*)&acMessage2[cCount], "AV:%2d->", InPut+1);
                }
                for(OutPut=0;OutPut<MAX_OUT;OutPut++)    
                {
                    if(!m_abSwitchOutPortSelect[OutPut])
                        continue;  
                         
                    if((psSystemSettingData->acIChannellock[InPut]&BIT(OutPut))&&(InPut<MAX_IN))      
                    {
                        sprintf(USART1_tx_buff,"Channel %d->%d Lock!\r\n",InPut+1,OutPut+1);
                        UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
                    }
                    else
                    {
                      if(eflag==eVideoSWITCH_HEX) 	//olny one
			      acMessage2[6]=OutPut+ 1;
			 else
                        cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d,",OutPut+ 1);
			 
                        SwitchxTox.xOutPort[SwitchxTox.xOutNum]=OutPut;
                        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
                        KeyLedSetOne(OutPut,InPut);
                        #endif
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
                        cCount += SiiSprintf((char*)&acMessage2[cCount], "%2d To All.", InPut+1);
                    UART1_SendData2PC(acMessage2, cCount); 
                }
                else if(SwitchxTox.xOutNum==0)// all lock
                    ;
                else
                {
			if(eflag==eVideoSWITCH_HEX) 	//olny one
			UART1_SendHexData2PC(acMessage2, 12); //except checksum
			else
			UART1_SendData2PC(acMessage2, strlen(acMessage2)-1);  //remove last  "," 
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


bool_t halSwitchVideoInitialize(void)
{
   SwitchVideoinit();
 //  appSystemSwitcherSetStart();
//   sFirstRun=0;
   return 1;
}




bool_t Init_SwitchVideo_module(BYTE MsgId)
{
    bool_t      success = true;
  if(!halSwitchVideoInitialize())
  	 success = false;  
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,SwitchVideoRxMsgHandler ,true))   //注册事件
  	 success = false; 
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,SwitchVideoProcessHandler ,true))  //注册任务
  	 success = false; 
  return success;
}








