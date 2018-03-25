#include "OutputHandle.h"
#include "SwitchVideoHandle.h"
#include "sii_api.h"
#include "hal.h"
#include "si_timer.h"
#include "KeyTasks.h"
#include "dvPC9539.h"

enum sii_scramble_state cOutHDMIVersion[MAX_OUT] = {0};
enum sii_scramble_state cOutHaveDetectHDMI20[MAX_OUT] = {0};
BYTE cOutHaveDetectHDMI20_ProcCount[MAX_OUT] = {0};
BYTE cOutHaveDetectHDMI14_ProcCount[MAX_OUT] = {0};
BYTE InPortLinkType[MAX_IN]={0};
BYTE OutputSwitchFlag[MAX_OUT]={0};
WORD OutputSwitchTimeCount[MAX_OUT] = 0;

BYTE FirstPwonAudioProc= 0;
extern BYTE FirstPwonFlag;

extern BYTE OutputEdidData[MAX_OUT][256];
extern BYTE CurTxDsEdidStatus[MAX_OUT];
extern BYTE OldTxDsEdidStatus[MAX_OUT];

extern void SwitchHDMIVersionOneClose(unsigned char OutPut);
extern void SwitchHDMIVersionOneOpen(unsigned char OutPut);

#if 0
void appInputHDMI14_20DetectAndOutputProcess(void)
{
    POUTPUTPORT_MANAGER_DATA     psOutputPortMgrData;
    BYTE cInput=0,cInSel=0;
    enum sii_scramble_state state=SII_SCRAMBLE_OFF;
    enum sii_av_link_type type;
    static BYTE cOut=MAX_OUT;

    if(cOut<MAX_OUT)
    {
	    psOutputPortMgrData = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);

	    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
	                      FALSE);

	    //halUICtrlEventHandler();

	    cInSel = psOutputPortMgrData->acInPortSelect[cOut];
	    if(cInSel<MAX_IN)
	    {
			//SiiRxScrambleFullGet(cInSel, &state);		// 获得输出通道对应的输入端的HDMI信号版本
			//SiiRxAvLinkGet(cInSel, &type);
			switch (InPortLinkType[cInSel])
			//switch (type)
			{
			case SII_AV_LINK__NONE:
				info("Rx Link Type is None\n");
				break;
			case SII_AV_LINK__HDMI1:
				state=SII_SCRAMBLE_OFF;
				break;
			case SII_AV_LINK__HDMI2:
				state=SII_SCRAMBLE_ON_ABOVE_340M;
				break;
			case SII_AV_LINK__DVI:
				state=SII_SCRAMBLE_OFF;
				break;
			default:
			    state=SII_SCRAMBLE_OFF;
				break;
			}
			if ((cOutHDMIVersion[cOut] != state)&&(state==SII_SCRAMBLE_ON_ABOVE_340M))			//从HDMI1.4变成HDMI2.0时
		       {
				 if(state==SII_SCRAMBLE_ON_ABOVE_340M)
				 {
						info("SII_SCRAMBLE_ON_ABOVE_340M******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_ABOVE_340M******************= %d\r\n",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_ON_BELOW_340M)
				 {
						info("SII_SCRAMBLE_ON_BELOW_340M******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_BELOW_340M******************= %d\r\n",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_OFF)
				 {
						info("SII_SCRAMBLE_OFF******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_OFF******************= %d\r\n",cOut+1);
				 }
				 else
				 {
						info("other value******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "other value******************= %d\r\n",cOut+1);
				 }
				 #if UART1_DEBUG
				 UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
				 #endif
				 //SwitchHDMIVersionOneClose(cOut+1);					//先关闭发生HDMI版本变化的输出通道
                 		 //delay_nms(50);
				 SiiTxScrambleFullSet(MAX_OUT+cOut, state);
				 cOutHaveDetectHDMI20[cOut] = SII_SCRAMBLE_ON_ABOVE_340M;
				 cOutHaveDetectHDMI20_ProcCount[cOut] =0;
				 cOutHaveDetectHDMI14_ProcCount[cOut] =0;
				 cOutHDMIVersion[cOut] = state;
		         	 //delay_nms(100);
		    	 	 //SwitchHDMIVersionOneOpen(cOut+1);					//恢复发生HDMI版本变化时关闭的输出通道
			 }
			 else if ((cOutHDMIVersion[cOut] != state)&&(cOutHaveDetectHDMI20[cOut]==SII_SCRAMBLE_ON_ABOVE_340M))		//从HDMI2.0变成HDMI1.4时
			 {
				 if(state==SII_SCRAMBLE_ON_ABOVE_340M)
				 {
						info("SII_SCRAMBLE_ON_ABOVE_340M******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_ABOVE_340M******************= %d\r\n",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_ON_BELOW_340M)
				 {
						info("SII_SCRAMBLE_ON_BELOW_340M******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_BELOW_340M******************= %d\r\n",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_OFF)
				 {
						info("SII_SCRAMBLE_OFF******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_OFF******************= %d\r\n",cOut+1);
				 }
				 else
				 {
						info("other value******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "other value******************= %d\r\n",cOut+1);
				 }
				 #if UART1_DEBUG
				 UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
				 #endif
	    		 	 //SwitchHDMIVersionOneClose(cOut+1);
	    		 	 //delay_nms(50);
	    		 	 SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_OFF);
				 cOutHaveDetectHDMI20[cOut] = SII_SCRAMBLE_OFF;
				 cOutHaveDetectHDMI14_ProcCount[cOut] =0;
				 cOutHaveDetectHDMI20_ProcCount[cOut] =0;
				 cOutHDMIVersion[cOut] = state;
				 //delay_nms(100);
		    	 	 //SwitchHDMIVersionOneOpen(cOut+1);
			 }
			 else if ((cOutHaveDetectHDMI20_ProcCount[cOut] > 0)&&(cOutHaveDetectHDMI20[cOut]==SII_SCRAMBLE_ON_ABOVE_340M))
			 {
				 cOutHaveDetectHDMI20_ProcCount[cOut] --;
				 SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_ON_ABOVE_340M);
				 info("SII_SCRAMBLE_ON_ABOVE_340M******************= %d",cOut+1);
				 #if UART1_DEBUG
				 sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_ABOVE_340M******************= %d\r\n",cOut+1);
				 UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
				 #endif
			 }
			 else if ((cOutHaveDetectHDMI14_ProcCount[cOut] > 0)&&(cOutHaveDetectHDMI20[cOut]==SII_SCRAMBLE_OFF))
			 {
				 cOutHaveDetectHDMI14_ProcCount[cOut] --;
				 SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_OFF);
				 info("SII_SCRAMBLE_OFF******************= %d",cOut+1);
				 #if UART1_DEBUG
				 sprintf(USART1_tx_buff, "SII_SCRAMBLE_OFF******************= %d\r\n",cOut+1);
				 UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
				 #endif
			 }
	    }

	    cOut++;
    }

    if(cOut==MAX_OUT)
    {
		cOut++;
		SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(3000) ,0xFF);   			// 3s循环一次，重新设置时间
    }
    else if(cOut==MAX_OUT+1)
    {
		if(!SiiPlatformTimerExpired(etHDMI_VERSION))
	    	{
	  	   		return; 		// time not enougth
	    	}
		cOut=0;				//定时3s时间到重新清0
    }
}

#endif


#if 1
void appInputHDMI14_20DetectAndOutputProcessTemp(BYTE cOut)
{
    POUTPUTPORT_MANAGER_DATA     psOutputPortMgrData;
    BYTE cInput=0,cInSel=0;
    enum sii_scramble_state state=SII_SCRAMBLE_OFF;
    enum sii_av_link_type type;
    //static BYTE cOut=MAX_OUT;

    if(cOut<MAX_OUT)
    {
	    psOutputPortMgrData = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);

	    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
	                      FALSE);

	    //halUICtrlEventHandler();

	    cInSel = psOutputPortMgrData->acInPortSelect[cOut];
	    if(cInSel<MAX_IN)
	    {
			//SiiRxScrambleFullGet(cInSel, &state);		// 获得输出通道对应的输入端的HDMI信号版本
			//SiiRxAvLinkGet(cInSel, &type);
			switch (InPortLinkType[cInSel])
			//switch (type)
			{
			case SII_AV_LINK__NONE:
				info("Rx Link Type is None\n");
				break;
			case SII_AV_LINK__HDMI1:
				state=SII_SCRAMBLE_OFF;
				break;
			case SII_AV_LINK__HDMI2:
				state=SII_SCRAMBLE_ON_ABOVE_340M;
				break;
			case SII_AV_LINK__DVI:
				state=SII_SCRAMBLE_OFF;
				break;
			default:
			    state=SII_SCRAMBLE_OFF;
				break;
			}
			if ((cOutHDMIVersion[cOut] != state)&&(state==SII_SCRAMBLE_ON_ABOVE_340M))			//从HDMI1.4变成HDMI2.0时
		       {
				 if(state==SII_SCRAMBLE_ON_ABOVE_340M)
				 {
						info("SII_SCRAMBLE_ON_ABOVE_340M******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_ABOVE_340M******************= %d\r\n",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_ON_BELOW_340M)
				 {
						info("SII_SCRAMBLE_ON_BELOW_340M******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_BELOW_340M******************= %d\r\n",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_OFF)
				 {
						info("SII_SCRAMBLE_OFF******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_OFF******************= %d\r\n",cOut+1);
				 }
				 else
				 {
						info("other value******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "other value******************= %d\r\n",cOut+1);
				 }
				 #if UART1_DEBUG
				 UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
				 #endif
				 //SwitchHDMIVersionOneClose(cOut+1);					//先关闭发生HDMI版本变化的输出通道
                 		 //delay_nms(50);
				 //SiiTxScrambleFullSet(MAX_OUT+cOut, state);
				 cOutHaveDetectHDMI20[cOut] = SII_SCRAMBLE_ON_ABOVE_340M;
				 cOutHaveDetectHDMI20_ProcCount[cOut] =2;
				 cOutHaveDetectHDMI14_ProcCount[cOut] =0;
				 cOutHDMIVersion[cOut] = state;
				 //Output5VConctrol(cOut,0);
				 SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(300) ,0xFF);   // 3s循环一次，重新设置时间
		         	 //delay_nms(100);
		    	 	 //SwitchHDMIVersionOneOpen(cOut+1);					//恢复发生HDMI版本变化时关闭的输出通道
			 }
			 else if ((cOutHDMIVersion[cOut] != state)&&(cOutHaveDetectHDMI20[cOut]==SII_SCRAMBLE_ON_ABOVE_340M))		//从HDMI2.0变成HDMI1.4时
			 {
				 if(state==SII_SCRAMBLE_ON_ABOVE_340M)
				 {
						info("SII_SCRAMBLE_ON_ABOVE_340M******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_ABOVE_340M******************= %d\r\n",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_ON_BELOW_340M)
				 {
						info("SII_SCRAMBLE_ON_BELOW_340M******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_BELOW_340M******************= %d\r\n",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_OFF)
				 {
						info("SII_SCRAMBLE_OFF******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "SII_SCRAMBLE_OFF******************= %d\r\n",cOut+1);
				 }
				 else
				 {
						info("other value******************= %d",cOut+1);
						sprintf(USART1_tx_buff, "other value******************= %d\r\n",cOut+1);
				 }
				 #if UART1_DEBUG
				 UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
				 #endif
	    		 	 //SwitchHDMIVersionOneClose(cOut+1);
	    		 	 //delay_nms(50);
	    		 	 SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_OFF);
	    		 	 //Output5VConctrol(cOut,0);
				 cOutHaveDetectHDMI20[cOut] = SII_SCRAMBLE_OFF;
				 cOutHaveDetectHDMI14_ProcCount[cOut] =0;
				 cOutHaveDetectHDMI20_ProcCount[cOut] =0;
				 cOutHDMIVersion[cOut] = state;
				 SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(300) ,0xFF);   // 3s循环一次，重新设置时间
				 //delay_nms(100);
		    		 //SwitchHDMIVersionOneOpen(cOut+1);
			 }
			 else if ((cOutHaveDetectHDMI20_ProcCount[cOut] > 0)&&(cOutHaveDetectHDMI20[cOut]==SII_SCRAMBLE_ON_ABOVE_340M))
			 {
				 cOutHaveDetectHDMI20_ProcCount[cOut] --;
				 SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_ON_ABOVE_340M);
				 info("SII_SCRAMBLE_ON_ABOVE_340M******************= %d",cOut+1);
				 #if UART1_DEBUG
				 sprintf(USART1_tx_buff, "SII_SCRAMBLE_ON_ABOVE_340M******************= %d\r\n",cOut+1);
				 UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
				 #endif
			 }
			 else if ((cOutHaveDetectHDMI14_ProcCount[cOut] > 0)&&(cOutHaveDetectHDMI20[cOut]==SII_SCRAMBLE_OFF))
			 {
				 cOutHaveDetectHDMI14_ProcCount[cOut] --;
				 SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_OFF);
				 info("SII_SCRAMBLE_OFF******************= %d",cOut+1);
				 #if UART1_DEBUG
				 sprintf(USART1_tx_buff, "SII_SCRAMBLE_OFF******************= %d\r\n",cOut+1);
				 UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
				 #endif
			 }
	    }
	    //if(FirstPwonFlag==1)			//第一次上电不执行
	    {
	    		//Output5VConctrol(cOut,0);
	    		SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(200) ,0xFF);   // 3s循环一次，重新设置时间
	    		OutputSwitchFlag[cOut] = 1;
	    }
    }

}

#endif


#if 1
void appInputHDMI14_20DetectAndOutputProcess(void)
{
    POUTPUTPORT_MANAGER_DATA     psOutputPortMgrData;
    BYTE cInput=0,cOut=0,cInSel=0;
    enum sii_scramble_state state=SII_SCRAMBLE_OFF;
    enum sii_av_link_type type;

    #if 1
    if(!SiiPlatformTimerExpired(etHDMI_VERSION))
    {
  	   return; 		// time not enougth
    }
    #endif

    psOutputPortMgrData = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);

    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,FALSE);

    for (cOut = 0; cOut < MAX_OUT; cOut++)
    {
		cInSel = psOutputPortMgrData->acInPortSelect[cOut];
		if(cInSel<MAX_IN)
		{
			//SiiRxScrambleFullGet(cInSel, &state);		// 获得输出通道对应的输入端的HDMI信号版本
			//SiiRxAvLinkGet(cInSel, &type);
			//switch (type)
			switch (InPortLinkType[cInSel])
			{
			case SII_AV_LINK__NONE:
				info("Rx Link Type is None\n");
				break;
			case SII_AV_LINK__HDMI1:
				state=SII_SCRAMBLE_OFF;
				break;
			case SII_AV_LINK__HDMI2:
				state=SII_SCRAMBLE_ON_ABOVE_340M;
				break;
			case SII_AV_LINK__DVI:
				state=SII_SCRAMBLE_OFF;
				break;
			default:
				break;
			}
			if ((cOutHDMIVersion[cOut] != state)&&(state==SII_SCRAMBLE_ON_ABOVE_340M))			//从HDMI1.4变成HDMI2.0时
		       {
				 if(state==SII_SCRAMBLE_ON_ABOVE_340M)
				 {
						info("SII_SCRAMBLE_ON_ABOVE_340M******************= %d",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_ON_BELOW_340M)
				 {
						info("SII_SCRAMBLE_ON_BELOW_340M******************= %d",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_OFF)
				 {
						info("SII_SCRAMBLE_OFF******************= %d",cOut+1);
				 }
				 else
				 {
						info("other value******************= %d",cOut+1);
				 }
				 //SwitchHDMIVersionOneClose(cOut+1);					//先关闭发生HDMI版本变化的输出通道
				 //delay_nms(10);
				 SiiTxScrambleFullSet(MAX_OUT+cOut, state);
				 cOutHaveDetectHDMI20[cOut] = SII_SCRAMBLE_ON_ABOVE_340M;
				 cOutHaveDetectHDMI20_ProcCount[cOut] =2;
				 cOutHaveDetectHDMI14_ProcCount[cOut] =0;
				 cOutHDMIVersion[cOut] = state;
				 SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(1000) ,0xFF);   // 1s循环一次，重新设置时间
		    	 	 //delay_nms(10);
		    	 	 //SwitchHDMIVersionOneOpen(cOut+1);					//恢复发生HDMI版本变化时关闭的输出通道
			 }
			 else if ((cOutHDMIVersion[cOut] != state)&&(cOutHaveDetectHDMI20[cOut]==SII_SCRAMBLE_ON_ABOVE_340M))		//从HDMI2.0变成HDMI1.4时
			 {
				 if(state==SII_SCRAMBLE_ON_ABOVE_340M)
				 {
						info("SII_SCRAMBLE_ON_ABOVE_340M******************= %d",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_ON_BELOW_340M)
				 {
						info("SII_SCRAMBLE_ON_BELOW_340M******************= %d",cOut+1);
				 }
				 else if(state==SII_SCRAMBLE_OFF)
				 {
						info("SII_SCRAMBLE_OFF******************= %d",cOut+1);
				 }
				 else
				 {
						info("other value******************= %d",cOut+1);
				 }
				 //SwitchHDMIVersionOneClose(cOut+1);
				 //delay_nms(10);
	    		 	 SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_OFF);
				 cOutHaveDetectHDMI20[cOut] = SII_SCRAMBLE_OFF;
				 cOutHaveDetectHDMI14_ProcCount[cOut] =0;
				 cOutHaveDetectHDMI20_ProcCount[cOut] =0;
				 cOutHDMIVersion[cOut] = state;
				 SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(1000) ,0xFF);
				 //delay_nms(10);
		         	 //SwitchHDMIVersionOneOpen(cOut+1);
			 }
			 else if ((cOutHaveDetectHDMI20_ProcCount[cOut] > 0)&&(cOutHaveDetectHDMI20[cOut]==SII_SCRAMBLE_ON_ABOVE_340M))
			 {
				 cOutHaveDetectHDMI20_ProcCount[cOut] --;
				 SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_ON_ABOVE_340M);
				 #if 0
				 if(cOutHaveDetectHDMI20_ProcCount[cOut]==1)
				 {
				    //Output5VConctrol(cOut,0);
				 }
				 else
				 {
				    //Output5VConctrol(cOut,1);
				    SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_ON_ABOVE_340M);
				 }
				 #endif
				 SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(800) ,0xFF);   // 1s循环一次，重新设置时间
				 info("SII_SCRAMBLE_ON_ABOVE_340M******************= %d",cOut+1);
			 }
			 else if ((cOutHaveDetectHDMI14_ProcCount[cOut] > 0)&&(cOutHaveDetectHDMI20[cOut]==SII_SCRAMBLE_OFF))
			 {
				 cOutHaveDetectHDMI14_ProcCount[cOut] --;
				 SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_OFF);
				 #if 0
				 if(cOutHaveDetectHDMI14_ProcCount[cOut]==1)
				 {
				    Output5VConctrol(cOut,0);
				 }
				 else
				 {
				    Output5VConctrol(cOut,1);
				    SiiTxScrambleFullSet(MAX_OUT+cOut, SII_SCRAMBLE_OFF);
				 }
				 #endif
				 SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(800) ,0xFF);   // 1s循环一次，重新设置时间
				 info("SII_SCRAMBLE_OFF******************= %d",cOut+1);
			 }
			 else
			 {
				SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(1000) ,0xFF);   // 1s循环一次，重新设置时间
			 }
		}
#if 0
		if(OutputSwitchFlag[cOut]==1)
		{
			//Output5VConctrol(cOut,1);
			//delay_nms(10);
			Output5VConctrol(cOut,0);
			delay_nms(10);
			Output5VConctrol(cOut,1);
			SiiPlatformTimerSet(etOUTPUT_HDCP,mS_TO_TICKS(3000) ,0xFF);   // 3s循环一次，重新设置时间
			SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(1000) ,0xFF);   // 1s循环一次，重新设置时间
			OutputSwitchTimeCount[cOut] = 50000;
			OutputSwitchFlag[cOut]=0;
		}
#endif         
                
    }
    //SiiPlatformTimerSet(etHDMI_VERSION,mS_TO_TICKS(2000) ,0xFF);   // 3s循环一次，重新设置时间
}

#endif
void Output5VHigthLow(BYTE cOutput)
{
	Output5VConctrol(cOutput,0);
	delay_nms(10);
	Output5VConctrol(cOutput,1);
}

void appOutputEDIDProcess(void)
{    BYTE i=0;
		    if(FirstPwonFlag==1)
		    {

			for(i=0;i<MAX_OUT;i++)
				{
			    if(CurTxDsEdidStatus[i] != OldTxDsEdidStatus[i])
			    {
				    if (CurTxDsEdidStatus[i])
				    {
					   info("DS EDID Available\n");
			                 if ((OutputEdidData[i][0] == 0) && (OutputEdidData[i][1] == 0xff) && (OutputEdidData[i][3] == 0xff)&& (OutputEdidData[i][7] == 0))
					   {
			                 }
			                 else
			                 {
			                        SiiTxEdidGetEdidData(utilOutputPortToInstance(i),OutputEdidData[i]);
			                 }
				    }
				    else
				    {
					   info("DS EDID Unavailable\n");
					   memset(OutputEdidData[i],0,  256);
				    }
				    OldTxDsEdidStatus[i] = CurTxDsEdidStatus[i];
			    }
				}
		    }
	
}
#if 1
void appOutputHDCPProcess(void)
{
    POUTPUTPORT_MANAGER_DATA     psOutputPortMgrData;
    PINPUTPORT_MANAGER_DATA        psInputPortMgrData;
    BYTE cInPort = 0,cOutPort = 0;
    BOOL bInHDCPEN;
    BOOL bOutHDCPEN;
    BOOL bHDCP_AUTO;
    BOOL bUserOutHDCPEN;
    static BYTE i=0;

    if(i<MAX_OUT)
    {
	  // if(OutputSwitchFlag[i]==0)
	   {
		   psOutputPortMgrData = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
                 psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnINPUTPORT_MAMAGER_DATA);
		    //halUICtrlEventHandler();
		    #if 1

	           bHDCP_AUTO = psOutputPortMgrData->abHDCP_AUTO[i];

	           cInPort =  psOutputPortMgrData->acInPortSelect[i];
	           bInHDCPEN=psInputPortMgrData->abCurrentHDCP[cInPort];
	           bOutHDCPEN = Sii9396TxHDCPGet(utilOutputPortToInstance(i));
	           bUserOutHDCPEN = psOutputPortMgrData->abHDCP_EN[i];
	           if (bHDCP_AUTO) 									// 自动跟随模式
	           {
	            	  if (bInHDCPEN != bOutHDCPEN)
		         {
		                  Sii9396TXHDCPSetStart(utilOutputPortToInstance(i), bInHDCPEN);
		         }
	           }
	           else if (bUserOutHDCPEN != bOutHDCPEN) 			// 手动开关输出端口HDCP模式
	           {
	                Sii9396TXHDCPSetStart(utilOutputPortToInstance(i), bUserOutHDCPEN);
	           }

	           info("OUTHDCP******************= %02x",i+1);
		       info(" ");
		    #endif
		    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,FALSE);
		    utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA,FALSE);
                      
	   }

	    i++;
    }

#if 0   
  // if(i==MAX_OUT)
  //       i=0;	
#else
    if(i==MAX_OUT)
    {  
		i++;
		SiiPlatformTimerSet(etOUTPUT_HDCP,mS_TO_TICKS(1000) ,0xFF);   // 1s循环一次，重新设置时间
    }
    else if(i==MAX_OUT+1)
    {
		if(!SiiPlatformTimerExpired(etOUTPUT_HDCP))
	    	{
	  	   	return; 		// time not enougth
	    	}
		i=0;				//定时3s时间到重新清0
    }
#endif
    appOutputEDIDProcess();	
}
#endif


#if 0
void appOutputHDCPProcess(void)
{
    POUTPUTPORT_MANAGER_DATA     psOutputPortMgrData;
    BYTE cInPort = 0;
    BOOL bInHDCPEN;
    BOOL bOutHDCPEN;
    BOOL bHDCP_AUTO;
    BOOL bUserOutHDCPEN;
    BYTE i;

    if(!SiiPlatformTimerExpired(etOUTPUT_HDCP))
    {
  	   return; 		// time not enougth
    }

    psOutputPortMgrData = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
    for (i = 0; i < MAX_OUT; i++)
    {
	   halUICtrlEventHandler();
	   #if 1
	   // psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
          bHDCP_AUTO = psOutputPortMgrData->abHDCP_AUTO[i];
          //utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
          //                               FALSE);
        cInPort =  psOutputPortMgrData->acInPortSelect[i];
        bInHDCPEN = Sii9396RxHDCPGet(cInPort);
        bOutHDCPEN = Sii9396TxHDCPGet(utilOutputPortToInstance(i));
        bUserOutHDCPEN = psOutputPortMgrData->abHDCP_EN[i];
        if (bHDCP_AUTO) 									// 自动跟随模式
        {
            if (bInHDCPEN != bOutHDCPEN)
            {
            #if(MACHINETYPE==MUH44F)
                if (i == 0)
                {
                    Sii9396TXHDCPSetStart(utilOutputPortToInstance(i), bInHDCPEN);
                    Sii9396TXHDCPSetStart(utilOutputPortToInstance(i) + 1, bInHDCPEN);
                }
                else
	   #endif
                    Sii9396TXHDCPSetStart(utilOutputPortToInstance(i), bInHDCPEN);
            }
        }
        else if (bUserOutHDCPEN != bOutHDCPEN) 			// 手动开关输出端口HDCP模式
        {
           #if(MACHINETYPE==MUH44F)
            if (i == 0)
            {
                Sii9396TXHDCPSetStart(utilOutputPortToInstance(i), bUserOutHDCPEN);
                Sii9396TXHDCPSetStart(utilOutputPortToInstance(i) + 1, bUserOutHDCPEN);
            }
            else
	   #endif
                Sii9396TXHDCPSetStart(utilOutputPortToInstance(i), bUserOutHDCPEN);
        }

	  info("OUTHDCP******************= %02x",i+1);
	  info(" ");
	 #endif
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      FALSE);
    SiiPlatformTimerSet(etOUTPUT_HDCP,mS_TO_TICKS(3000) ,0xFF);   // 3s循环一次，重新设置时间
}
#endif


// HDMI 声音开关，并且保存状态到NVRAM,这个函数不太科学
void appSystemHDMITXAudioOutEnableSet(BYTE cInstance, BOOL bEnable)
{
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    psOutputPortMgrData->abHDMIAudio_EN[cInstance]=bEnable;
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    //dvSii9533TXAudioMixPassThroughConfig(cInstance,bEnable,bEnable);
}





