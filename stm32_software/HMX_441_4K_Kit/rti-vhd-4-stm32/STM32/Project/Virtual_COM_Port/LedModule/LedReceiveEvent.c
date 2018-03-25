#include "LedReceiveEvent.h"
#include "dvTm1623.h"

LedInstanceData_t LedGlobal;
static BYTE m_LedIntervalTime;
//static uint8_t registeredLedsTaskID = NO_TASK_ID;
//static DWORD           s_wLedToFlashing;   // 用于记录那些灯要闪烁	
bool_t LedProcessHandler (void);
bool_t LedRxMsgHandler (SendData_t *pMsg );
// ***************************************************************************
// ---------------------------------------------------------------------------
// This API sets up the internal state of component.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
bool_t halLedInitialize(void)
{
    bool_t      success = true;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
     if(LedInitialize())
   {  success = false;
#if INC_UART	 
      if(psSystemSettingData->cPOSTEnable)
	 UART1_SendData2PC("LEDInitialize fail!", strlen("LEDInitialize fail!"));
#endif
   }
     utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
	 return success;
}

bool_t Init_Led_module(BYTE MsgId)
{
    bool_t      success = true;
  //注册LED 灯功能
  if(!halLedInitialize())
  	 success = false;  
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,LedRxMsgHandler ,true))   //注册事件
  	 success = false; 
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,LedProcessHandler ,true))  //注册任务
  	 success = false; 
  return success;
}


bool_t LedRxMsgHandler (SendData_t *pMsg )
{
    bool_t  usedMessage         = false;
    DWORD wLed = 0;
    BYTE  i;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    //bool_t  isDirectAddressed   = !((pMsg->srcDestAddr & mqLED ) == mqLED);
    Event_t sLedEvent;
    //BYTE InPut = 0, OutPut = 0;
          /* ---------------------------- END CUSTOM AREA --------------------------- */
    if (utilExecMsgPreRead(mqLED))
    {
        utilExecMsgDataRead(mqLED,&sLedEvent);
        /* Add code here to perform a post read operation on the user input queue */
        // utilExecMsgClear(mqLED);
        /* ---------------------------- END CUSTOM AREA --------------------------- */
    }
    else 
        return usedMessage;

    usedMessage=true;
    switch ( sLedEvent.Head.opcode )
    {
        case sLEDx_STOP_FLICKER:
            {
                LedGlobal.LedCntTime=0;  //not  Flashing
                dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led	
            }
            break;
        case sLEDx_ChannelState:
            {
                POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
                psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
                // dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led	 
                LedGlobal.LedCntTime=0;  //not  Flashing
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
                wLed |= (1 << (elnOUT1 +sLedEvent.args[0] )) | (1 << (elnIN1 + psOutputPortMgrData->acInPortSelect[sLedEvent.args[0]]));
                utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                FALSE);
                dvLedOnSet(wLed);
            }
            break;
        case sLEDx_FLICKER:
            break;
            
        case  sLEDAll_FLICKER:
		   break;
           
        case sLEDx_OFF:
            {
                dvLedOffSet(1 << (sLedEvent.args[0]));
            }
			break;
        case  sLEDAll_OFF:
            {
                dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
            }
            break;
        case sLEDx_ON:
            {
                LedGlobal.LedCntTime=0;  //not  Flashing
                dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
                dvLedOnSet(1 << (sLedEvent.args[0]));
            }
            break;
        case  sLEDAll_ON:
            {
                dvLedAllOffSet(0xFFFFFFFF); // all led off, except power led
            }
            break;
        case  eLEDAllOpen:
        case  eLEDAllClose:
            {
                for (i = 0; i < MAX_OUT; i++)  
                    wLed |= (1 << (elnOUT1 + i)) ;
                dvLedOnSet(wLed);
                //****************************		
                LedGlobal.LedCntTime=3*2-1;  //three time  Flashing
                LedGlobal.LedIntervalTime=100;  // 1S
                m_LedIntervalTime=LedGlobal.LedIntervalTime;
                LedGlobal.tLedValue= dvLedOnGet();
                /**************************************/
            }
            break;	
        case  eLEDxOpen:
        case  eLEDxClose:
            {
        		pCHANNELSWITCH_EVENT LedSwitchEvent;
        		LedSwitchEvent=(CHANNELSWITCH_EVENT *)&sLedEvent.args;
        		wLed |= (1 << (elnOUT1 + LedSwitchEvent->LedOutputPort)) ;
        		dvLedAllOnSet(wLed);
        		//****************************		
        		LedGlobal.LedCntTime=3*2-1;  //three time  Flashing
        		LedGlobal.LedIntervalTime=100;  // 1S
        		m_LedIntervalTime=LedGlobal.LedIntervalTime;
        		LedGlobal.tLedValue= dvLedOnGet();
        		/**************************************/
        	}
		break;
        case  eLEDAllThrough:
            {
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
                dvLedOffSet(1 << elnENTER);
                for (i = 0; i < MAX_OUT; i++)           //输入输出灯全亮
                {
                    if(psSystemSettingData->acIChannellock[i]&BIT(i))
                        ;
                    else  //output not lock
                        wLed |= (1 << (elnOUT1 + i)) | (1 << (elnIN1 + i));
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);           	
                dvLedOnSet(wLed);
                //****************************		
                LedGlobal.LedCntTime=3*2-1;  //three time  Flashing
                LedGlobal.LedIntervalTime=100;  // 1S
                m_LedIntervalTime=LedGlobal.LedIntervalTime;
                LedGlobal.tLedValue= dvLedOnGet();
                //***************************************
            }
            break;
        case sLEDxLightOut:  //olny  Light x Led of Output
            {
                pCHANNELSWITCH_EVENT LightOut;
                LightOut=(CHANNELSWITCH_EVENT *)&sLedEvent.args;	   
                wLed |= 1 << (elnOUT1 +LightOut->LedOutputPort);
                dvLedOnSet(wLed);
                //***************************************
            }
            break;
        case sLEDAllLightOut:  //olny  Light All Led of Output
            {
                for (i = 0; i <MAX_OUT; i++)
                    wLed |= 1 << (elnOUT1 +i);
                dvLedOnSet(wLed);
        	//***************************************
            }
        	break;
         case eLEDChannelSwitch:
            {
                pCHANNELSWITCH_EVENT SwitchEvent;
                SwitchEvent=(CHANNELSWITCH_EVENT *)&sLedEvent.args;
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
                dvLedOffSet(1 << elnENTER);
                for (i = 0; i < SwitchEvent->LedxNum; i++)
                {
                    if(psSystemSettingData->acIChannellock[SwitchEvent->LedInputPort]&BIT(SwitchEvent->LedOutputPortList[i])) 
                        ;
                    else  //output not lock
                        wLed |= (1 << (elnOUT1 + SwitchEvent->LedOutputPortList[i]));
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);           
                dvLedAllOnSet(1 << (elnIN1 + SwitchEvent->LedInputPort));
                dvLedOnSet(wLed);
                dvLedOnSet(1 << elnENTER);
                //****************************		
                LedGlobal.LedCntTime=3*2-1;  //three time  Flashing
                LedGlobal.LedIntervalTime=100;  // 1S
                m_LedIntervalTime=LedGlobal.LedIntervalTime;
                LedGlobal.tLedValue= dvLedOnGet();
                //***************************************
            }
            break;
        case eLEDSwitchxAll:
            {
                pCHANNELSWITCH_EVENT SwitchxAll;
                SwitchxAll=(CHANNELSWITCH_EVENT *)&sLedEvent.args;
                psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
                dvLedOffSet(1 << elnENTER);
                for (i = 0; i <MAX_OUT; i++)
                {
                    if(psSystemSettingData->acIChannellock[SwitchxAll->LedInputPort]&BIT(i)) 
                        ;
                    else  //output not lock
                        wLed |= (1 << (elnOUT1 + i));
                }
                utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);     
                dvLedAllOnSet(1 << (elnIN1 + SwitchxAll->LedInputPort));
                dvLedOnSet(wLed);
                dvLedOnSet(1 << elnENTER);
                //****************************		
                LedGlobal.LedCntTime=3*2-1;  //three time  Flashing
                LedGlobal.LedIntervalTime=100;  // 1S
                m_LedIntervalTime=LedGlobal.LedIntervalTime;
                LedGlobal.tLedValue= dvLedOnGet();
                //***************************************
            }
		break;
		
        default:
            usedMessage = false;                        // Didn't use the message
            break;
    }

    return( usedMessage );


}


#ifdef MODIFY_IR_SWITCH_2017_2_21
extern bool g_bLEDUpdateMainLoopFlag;//是否刷新主循环LED显示
extern bool g_bSwitcherFlag;
extern bool g_bLEDBlankTimeOKFlag;
extern void LEDFlash(bool bLignt);
#endif

bool_t LedProcessHandler (void)
{
    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    #ifdef MODIFY_IR_SWITCH_2017_2_21
    if(g_bLEDUpdateMainLoopFlag)
    {
    #endif
        LED_Display();
    #ifdef MODIFY_IR_SWITCH_2017_2_21
    }
    else
    {
    	//加上闪烁间隔时间到标志,有时会导致显示不正常，一组亮一组灭
    	//if(g_bLEDBlankTimeOKFlag)
    	{
			LEDFlash(g_bSwitcherFlag);
			//g_bLEDBlankTimeOKFlag = false;
		}
    }
    #endif
    #endif
	return true;

}



//LED time
void TIM4_IRQHandler(void)   //100US
{
    TIM_ClearFlag(TIM4, TIM_FLAG_Update); //一定要清中断
if(m_LedIntervalTime>0)
    m_LedIntervalTime--;

	if(m_LedIntervalTime==0)
	{
		m_LedIntervalTime=LedGlobal.LedIntervalTime;

		if( LedGlobal.LedCntTime>0)
		{
		LedGlobal.LedCntTime--;
		dvLedToggleSet(LedGlobal.tLedValue);
		}
	}
}












