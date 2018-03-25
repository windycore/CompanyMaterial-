//***************************************************************************/
//!file     si_avr_repeater.c
//!brief    SiI9535 Starter Kit firmware main module.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#include "sk_application.h"
#if INC_CBUS
#include "sk_app_cbus.h"
#endif
#include "si_debugger_hdmigear.h"
#include "si_eeprom.h"
#include "si_gpio.h"
#if INC_RTPI
#include "si_rtpi_component.h"
#endif
#include "bsp_gpio.h"
#include "appSystemMgrAPI.h"
#include "Main.h"
#include "si_cra.h"
#include "si_drv_tpi_system.h"
#include "uart.h"
#include "sk_avr_repeater.h"
#include "util.h"
#include "si_datatypes.h"
//------------------------------------------------------------------------------
//  Application Data
//------------------------------------------------------------------------------
const char lSignonMsg[] = "SK953x Firmware v";
const char lCopyrightMsg[] = "Copyright PTN, 2010-2014";
extern char *buildTime;     // From buildtime.c
extern char *buildVersion;  // From buildtime.c
//AppInstanceData_t app;
AppInstanceData_t app[INSTANCE_9533];
//AppInstanceData_t *pApp = &app;
extern uint16_t SiiDrvDeviceIdGet(BYTE cInstance);
extern uint8_t SiiDrvDeviceRevGet(BYTE cInstance);
extern void SiiDrvTpiDdcBusSet(BYTE cInstance,bool_t  bHDCP);
//------------------------------------------------------------------------------
// Function:    SkAppSignon
// Description: Display signon and configuration information
//-----------------------------------------------------------------------------c
void SkAppSignon(BYTE cInstance)
{
    DEBUG_PRINT2(MSG_ALWAYS, "\n\n%s%s\n%s\n", lSignonMsg, buildVersion, lCopyrightMsg);
    PrintAlways(buildTime);   // From buildtime.c
    DEBUG_PRINT2(MSG_ALWAYS, "\nChipdID:0x%x\tChip Rev:0x%x\n", SiiDrvDeviceIdGet(cInstance), SiiDrvDeviceRevGet(cInstance));
    //PrintAlways(buildTime);   // From buildtime.c
    //PrintAlways(m_acGitHash); // this is generate by git commit command
}
void dvSii9533Init(void)
{
    BYTE cInstance = 0;
    // reset move to dvSii9533HardWareReset
    //_9533_5340_reset(0);       //复位所有的9533
    //BSP_Gpio_High(egpioSii9533_9679Reset);  // 经过74LVC14
    //delay_nms(10);
    //_9533_5340_reset(1);
    //BSP_Gpio_Low(egpioSii9533_9679Reset);
    for (cInstance =0; cInstance < INSTANCE_9533; cInstance++)      //Intialize 9533
    {
        IWDG_ReloadCounter(); //喂狗
        Init_StructInstance(cInstance);
        //devType[cInstance]=SiiRegRead (cInstance, 0x0002 );
        //if(devType[cInstance]==0x33)      //有插卡才进行配置扫描
        {
            SkAppInitialize(cInstance);
            SkAppDeviceInitialize(cInstance);    // Initialize anything that can be done with PWD and/or AON power on
#if INC_CEC
            SkAppCecResume(cInstance, true);      // Resume the CEC component so it can handle wake up messages
#endif
        }
        IWDG_ReloadCounter(); //喂狗
        //disconnect_9187();
    }
}
void dvSii9533Process(BYTE cInstance)
{
    // uint8_t i = 0, devType[14] = { 0 }, a = 0;
    //uint8_t a = 0;
    //device_slect_9187(cInstance+1);
    //IWDG_ReloadCounter(); //喂狗
    Init_StructInstance(cInstance);
    SkAppDevicePower(cInstance); //[kyq add: 20150828 9:05:13]
    SkAppTaskSiiDevice(cInstance);//  中断处理和状态保存
    SkAppTaskSwitch(cInstance);  //[kyq Begin: 20150915 20:54:30]  move to here    //  RX 端的一些中断处?
    SkAppSwitchPortUpdate(cInstance);    // Check for port updates from any of the tasks.
#if INC_CEC
    SkAppTaskCec(cInstance);
#endif
    if ((app[cInstance].powerState != APP_POWERSTATUS_ON)||(app[cInstance].isresolutionVaild == false))
    {
        return;//continue;
    }
#if INC_CBUS
    SkAppTaskCbus(cInstance);
#endif
    //SkAppTaskSwitch(cInstance);  //[kyq Begin: 20150915 20:54:30]  move to up
    //if ((cInstance == 4) || (cInstance == 5) || (cInstance == 6) || (cInstance == 7) || (cInstance == 8) || (cInstance == 9))
    {
        SkAppTaskRepeater(cInstance);
    }
    SkAppTaskTx(cInstance); //  中断消息处理
#if INC_IPV
    SkAppTaskIpv(cInstance);
#endif
#if INC_OSD
    SkAppTaskOsd(cInstance);
#endif
    SkAppTaskAudio(cInstance);
#if INC_RTPI  // 使用simon 的RTPI 功能
    SiiRtpiProcessRtpi(cInstance);
#endif

    //SkAppSwitchPortUpdate(cInstance);    // Check for port updates from any of the tasks.

    if(OutHDCP_Flag[cInstance] == 1)
    {
		if(OutHDCP_TimeDelay[cInstance]==0)
		{
			OutHDCP_Flag[cInstance] =0;
			SiiTxAvUnmute(cInstance);
			SiiDrvTpiTmdsOutputEnable(cInstance,true);
		}
    }

}
// 得到当前9533 TX HDCP 是否打开
BOOL dvSii9533TXHDCPGet(BYTE cInstance)
{
    return app[cInstance].isTxHdcpRequired[0];
}
// 使能9533 的TX HDCP
void dvSii9533TXHDCPSetStart(BYTE cInstance,BOOL bEnable)
{
    // TODO
    BYTE i;
    app[cInstance].isTxHdcpRequired[0] = bEnable;
    DEBUG_PRINT(MSG_ALWAYS, "dvSii9533TXHDCPSetStart[%d] =%d\r\n",cInstance,bEnable);
    for (i = 0; i < SII_NUM_TX; i++)
    {
        //StopTx(cInstance, i);
        //StartTx( cInstance, i);
        //EnableTransmitter(cInstance,TRUE);
        //SiiTxHdcpEnable();//这个可能可以
        //SiiDrvTpiDdcMasterReset( cInstance ); //[kyq add: 20150907 17:30:17]
        SiiDrvTpiDdcBusSet(cInstance,bEnable);  // 修改Valens芯片HDCP灯不闪的问题
        SkAppDeviceInitTx(cInstance);
    }
}
// 得到分辨率
BOOL  dvSii9533RXResolutionGet(BYTE cInstance,INT* pHorizPixel, INT *pVertLine,INT *pTmdsClock,bool_t* pbInterlace)
{
    if(app[cInstance].isresolutionVaild)
    {
        if((app[cInstance].VertLine==2160)&&(app[cInstance].HorizPixel==1920))
        {
            *pHorizPixel = app[cInstance].HorizPixel*2;
            *pVertLine = app[cInstance].VertLine;
            *pTmdsClock= app[cInstance].TmdsClock;
            *pbInterlace = app[cInstance].bInterlace;
        }
        else if((app[cInstance].VertLine==2160)&&(app[cInstance].HorizPixel==2048))
        {
            *pHorizPixel = app[cInstance].HorizPixel*2;
            *pVertLine = app[cInstance].VertLine;
            *pTmdsClock= app[cInstance].TmdsClock;
            *pbInterlace = app[cInstance].bInterlace;
        }
        else
        {
            *pHorizPixel = app[cInstance].HorizPixel;
            *pVertLine = app[cInstance].VertLine;
            *pTmdsClock= app[cInstance].TmdsClock;
            *pbInterlace = app[cInstance].bInterlace;
        }
        return TRUE;
    }
    else
    {
        *pHorizPixel =0;
        *pVertLine = 0;
        *pTmdsClock= 0;
        *pbInterlace = 0;
        return FALSE;
    }
}
void dvSii9533TXAudioMixPassThroughConfig(BYTE cInstance,BOOL isAudioPassEnabled,BOOL isNonAudioPassEnabled)
{
    BYTE cPort;
    POUTPUTPORT_MANAGER_DATA psOutputPortMgrData;
    assert_param(cInstance<INSTANCE_9533);
    cPort =  utilInstanceToOutputPort(cInstance);
    psOutputPortMgrData=(POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    psOutputPortMgrData->abHDMIAudio_EN[cPort ] = isAudioPassEnabled;
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, TRUE);
    app[cInstance].bHDMIAudioEn = isAudioPassEnabled;
    SiiDrvRxAudioMixInstanceSet( cInstance ,0);
    SiiDrvRxAudioMixPassThroughConfig( cInstance ,isAudioPassEnabled, isNonAudioPassEnabled);
}
void dvSii9533TXAudioMixPassThroughConfigStart(BYTE cInstance)
{
    BYTE cPort;
    BOOL isAudioPassEnabled;
    POUTPUTPORT_MANAGER_DATA psOutputPortMgrData;
    assert_param(cInstance<INSTANCE_9533);
    cPort =  utilInstanceToOutputPort(cInstance);
    psOutputPortMgrData=(POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
    isAudioPassEnabled = psOutputPortMgrData->abHDMIAudio_EN[cPort ];
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA, FALSE);
    app[cInstance].bHDMIAudioEn = isAudioPassEnabled;
    SiiDrvRxAudioMixInstanceSet( cInstance ,0);
    SiiDrvRxAudioMixPassThroughConfig( cInstance ,isAudioPassEnabled, isAudioPassEnabled);
}
void dvSii9533PowerControl(  BYTE cInstance,SiiAppPowerStatus_t newState )
{
    /*
        switch ( newState )
        {
            case 0:
                app[cInstance].powerState = APP_POWERSTATUS_ON_TO_STANDBY;
                break;
            case 1:
                app[cInstance].powerState = APP_POWERSTATUS_STANDBY_TO_ON;
                break;
            case 2:
                app[cInstance].powerState = APP_POWERSTATUS_ON_TO_STANDBY;
                SkAppDevicePower(cInstance);
                app[cInstance].powerState = APP_POWERSTATUS_STANDBY_TO_ON;
                break;
        }
        */
    app[cInstance].powerState = newState;
    SkAppDevicePower(cInstance);
    //return( true );
}
//#if(MACHINETYPE==MUH44E_BYE)
void dvSii9533HardWareReset(void)
{
    #if ((MACHINETYPE == MHUB4K431)||(MACHINETYPE == MUH44E_BYE))
    delay_nms(100);  //waiting sii9679  normal of pow on
    BSP_Gpio_High(egpioSii9533Reset);  // 经过74LVC14    
    BSP_Gpio_High(egpioSii9679Reset);  // 经过74LVC14   
    delay_nms(10);
    //_9533_5340_reset(1);
    BSP_Gpio_Low(egpioSii9533Reset);
    BSP_Gpio_Low(egpioSii9679Reset);  // 经过74LVC14   
    #else
    //BSP_Gpio_High(egpioSii9533_9679Reset);  // 经过74LVC14
    //delay_nms(10);
    //_9533_5340_reset(1);
    //BSP_Gpio_Low(egpioSii9533_9679Reset);
    #endif
}
//#endif
