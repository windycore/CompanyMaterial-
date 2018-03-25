/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file ai_ap_adapter_rx.c
 *
 * @brief HDCP 1.x/2.x to HDCP 2.x/1.x adapter test application
 *
 *****************************************************************************/
/***** #include statements ***************************************************/
#include <stdio.h>
#include "si_drv_adapter_internal.h"
#include "si_drv_adapter_rx.h"
#include "si_virtual_page.h"
#include "main.h"
#include "Mcu.h"
//#include "key_led_lcd.h"
#include "uart.h"
//#include "LCD_scan.h"
//#include "IIC_EE_4604_EDID_other.h"
#include "bsp_i2c.h"
#include "IR.h"
//#include "switch_driver.h"
#include "dvCPLD.h"
//#include "HDCP_Other_process.h"
#include "si_common.h"
#include "si_drv_adapter.h"
#include "si_drv_adapter_rx.h"
#include "si_drv_adapter_tx.h"
#include "appSystemMgrAPI.h"
#include "utilDataMgrAPI.h"
#include "si_app_adapter_rx.h"
#include "dvNVRAM.h"
/***** global variables *******************************************************/
/*
// Internal instance record
typedef struct
{
    uint8_t                   instCra;   // CRA instance
    SiiDrvAdaptConfig_t       config;    // Driver configuration
    SiiDrvAdaptAccessStatus_t accessStatus; // FW update, SRAM and EDID access status
    SiiLibEvtCbFunc_t         pNtfCallback;  // Pointer to the notification callback function
     //bool_t                    isBootingInProgress; // true during booting
    bool_t                    upgradeMode;
    bool_t                    isError;
} DrvAdaptInst_t;
*/
// 指针数组
// 其实记录的是 指针-->  drvAdaptInstRecord[x]
SiiInst_t     m_apsInstAdapter[INSTANCE_9679];   // point to drvAdaptInstRecord[]
/***** local macro definitions ***********************************************/
#define EDID_BLOCK_LEN 128
#define EDID_BL0_LEN__HEADER 8
//extern uint8_t g_tempData[256];
//extern uint8_t init_edidFlashEdidTable[256];
//extern uint8_t g_edidFlashEdidTable[256];
/***** local variables *******************************************************/
/*****************************************************************************/
/**
* @brief Reset the chip by toggling the reset pin
*
*
* @note This function is used during intialization, needs to be called before
*   register reading and SiiDrvAdaptStart().
*
*****************************************************************************/
static bool_t EdidBlock0Check(uint8_t EdidData[EDID_BLOCK_LEN])
{
    uint8_t check_sum = 0;
    uint8_t i;
    const uint8_t aEdidHeader[EDID_BL0_LEN__HEADER] =
    { 0x00,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0x00 };
    //check check_sum
    for (i = 0; i < EDID_BLOCK_LEN; i++)
    {
        check_sum += EdidData[i];
    }
    if (check_sum)
    {
        return false;
    }
    // Check EDID header
    if (0 != memcmp(EdidData, (char *)aEdidHeader, EDID_BL0_LEN__HEADER))
    {
        return false;
    }
    return true;
}
static void EdidBlockDataDisplay(uint8_t EdidData[EDID_BLOCK_LEN])
{
    uint8_t i;
    for (i = 0; i < EDID_BLOCK_LEN; i++)
    {
        //PRINTF(("%02x ",(int)EdidData[i]));
        PRINTF("%02x ", (int)EdidData[i]);
        //UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
        if (0 == ((i + 1) % 16))
        {
            PRINTF("\n");
            //PRINTF(USART1_tx_buff,"\n");
            //UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
        }
    }
}
static void AdapterGeneralEventHandler(SiiInst_t inst, uint32_t EventFlags)//called by AdapterEventHandler
{
    if (EventFlags & SII_DRV_ADAPT_EVENT__BOOT_DONE)
    {
        //do nothing.
    } //if
    if (EventFlags & SII_DRV_ADAPT_EVENT__ACCESS_DONE)
    {
        // Firmware update is done
        // Read the update status
        SiiDrvAdaptAccessStatus_t updateStatus;
        SiiDrvAdaptAccessStatusGet(inst, &updateStatus);
        switch (updateStatus)
        {
        case SII_DRV_ADAPTER_ACCESS__SUCCESS:
            // Uploading is done successfully
            break;
        case SII_DRV_ADAPTER_ACCESS__IN_PROGRESS:
            // Uploading is in progress
            break;
        case SII_DRV_ADAPTER_ACCESS__FAILURE:
            // Uploading failed
            break;
        default:
            // If this happens, there is a bug
            SII_OS_DEBUG_ASSERT(0);
            break;
        } //switch
    } //if
    //-----------------------------------------------------------
    // SYNC DETECT MONITORING
    //-----------------------------------------------------------
    if (EventFlags & SII_DRV_ADAPT_EVENT__SCDT_STATUS_CHNG)
    {
        bool_t scdtDetStatus;
        SiiDrvAdaptScdtStatusGet(inst, &scdtDetStatus);
        PRINTF("SII9679 Sync Detect: %s\n", scdtDetStatus ? "ON" : "OFF");
        PRINTF("\n");
//[kyq Begin: 20151010 0:14:43]
        if(!scdtDetStatus)
        {
            DrvAdaptInst_t *pDrvAdapt = DRV_ADAPT_INST2PTR(inst);
            pDrvAdapt->RxHdcpVer = SII_DRV_ADAPTER_HDCP_VER__NOT_SUPPORTED;
            PRINTF("----[%d]-pDrvAdapt->RxHdcpVer= %d\n",pDrvAdapt->instCra, pDrvAdapt->RxHdcpVer);
        }
//[kyq End: 20151010 0:14:49]
    }
    if (EventFlags & SII_DRV_ADAPT_EVENT__CKDT_STATUS_CHNG)
    {
        bool_t ckdtDetStatus;
        SiiDrvAdaptCkdtStatusGet(inst, &ckdtDetStatus);
        PRINTF("SII9679 Clock Detect: %s\n", ckdtDetStatus ? "ON" : "OFF");
        PRINTF("\n");
    }
    if (EventFlags & SII_DRV_ADAPT_EVENT__RX_CD_SENSE_STATUS_CHG)
    {
        bool_t cdSenseStatus;
        SiiDrvAdaptRxCdSenseGet(inst, &cdSenseStatus);
        PRINTF("SII9679 Cable detect: %s\n", cdSenseStatus ? "ON" : "OFF");
        PRINTF("\n");
    }
}
static void AdapterTxEventHandler(SiiInst_t inst, uint32_t EventFlags)
{
    if (EventFlags & SII_DRV_ADAPT_EVENT__DS_CONNECT_STATUS_CHNG_ON)
    {
        SiiDrvAdaptLinkType_t LinkType;
        PRINTF("Downstream Connect Status Changed:\n");
        SiiDrvAdaptTxLinkTypeGet(inst, &LinkType);
        switch (LinkType)
        {
        case SII_DRV_LINK_DISCONNECTED:
            PRINTF("   Link Type is: None\n");
            break;
        case SII_DRV_LINK_HDMI:
            PRINTF("   Link Type is: HDMI/DVI\n");
            break;
        case SII_DRV_LINK_MHL12:
            PRINTF("   Link Type is: MHL12\n");
            break;
        case SII_DRV_LINK_MHL3:
            PRINTF("   Link Type is: MHL3\n");
            break;
        default:
            SII_OS_DEBUG_ASSERT(0);
            break;
        }
        PRINTF("\n");
    }
    if (EventFlags & SII_DRV_ADAPT_EVENT__DS_CONNECT_STATUS_CHNG_OFF)
    {
        PRINTF("Downstream DisConnect\n");
    }
}
static void AdapterRxEventHandler(SiiInst_t inst, uint32_t EventFlags)
{
    DrvAdaptInst_t *pDrvAdapt = DRV_ADAPT_INST2PTR(inst);
    if (EventFlags & SII_DRV_ADAPT_EVENT__US_CONNECT_STATUS_CHNG)
    {
        SiiDrvAdaptLinkType_t LinkType;
        PRINTF("9679[%d] Upstream Connect Status Changed:\n",pDrvAdapt->instCra);
        SiiDrvAdaptRxLinkTypeGet(inst, &LinkType);
        switch (LinkType)
        {
        case SII_DRV_LINK_DISCONNECTED:
            PRINTF("   Link Type is: None\n");
            break;
        case SII_DRV_LINK_HDMI:
            PRINTF("   Link Type is: HDMI/DVI\n");
            break;
        case SII_DRV_LINK_MHL12:
            PRINTF("   Link Type is: MHL12\n");
            break;
        case SII_DRV_LINK_MHL3:
            PRINTF("   Link Type is: MHL3\n");
            break;
        default:
            SII_OS_DEBUG_ASSERT(0);
            break;
        }
        PRINTF("\n");
    }
    //-----------------------------------------------------------
    // AV MUTE MONITORING
    //-----------------------------------------------------------
    if (EventFlags & SII_DRV_ADAPT_EVENT__RX_AV_MUTE_STATUS_CHNG)
    {
        SiiDrvAdaptMute_t muteStat;
        SiiDrvAdaptRxAvMuteStatusGet(inst, &muteStat);
        PRINTF("9679[%d] AV Mute Status Changed:\n",pDrvAdapt->instCra);
        switch (muteStat)
        {
        case SII_DRV_ADAPTER_MUTE__AV_MUTE_ON:
            PRINTF("   AV Mute ON\n");
            pDrvAdapt->eTxTmdsOutEn = SII_DRV_ADAPT_TMDS_OUT__DISABLE;  //[kyq add: 20151028 10:19:34]
            break;
        case SII_DRV_ADAPTER_MUTE__AV_MUTE_OFF:
            PRINTF("   AV Mute OFF\n");
            pDrvAdapt->eTxTmdsOutEn = SII_DRV_ADAPT_TMDS_OUT__ENABLE; //[kyq add: 20151028 10:19:37]
            break;
        default:
            SII_OS_DEBUG_ASSERT(0);
            break;
        }
        PRINTF("\n");
    }
}
// EventFlags1:   AdaptEvents.HdcpEvents
// EventFlags2 :  AdaptEvents.RxEvents
// called by : AdapterEventHandler()
static void AdapterHdcpEventHandler(SiiInst_t inst, uint32_t EventFlags1, uint32_t EventFlags2)
{
    if (EventFlags1 & SII_DRV_ADAPT_HDCP_EVENT__TX_STATUS_CHG)
    {
        SiiDrvAdaptHdcpVer_t TxHdcpVer;
        SiiDrvAdaptHdcpTxStatus_t HdcpStatus;
        PRINTF("9679 Tx HDCP  Version:\n");
        SiiDrvAdaptTxHdcpDsVerGet(inst, &TxHdcpVer);
        switch (TxHdcpVer)
        {
        case SII_DRV_ADAPTER_HDCP_VER__NOT_SUPPORTED:
            PRINTF("   HDCP version not support\n");
            break;
        case SII_DRV_ADAPTER_HDCP_VER__1x:
            PRINTF("   HDCP version is: 1.x\n");
            break;
        case SII_DRV_ADAPTER_HDCP_VER__20:
            PRINTF("   HDCP version is: 2.0\n");
            break;
        case SII_DRV_ADAPTER_HDCP_VER__22:
            PRINTF("   HDCP version is: 2.2\n");
            break;
        default:
            SII_OS_DEBUG_ASSERT(0);
            break;
        }
        PRINTF("\n");
        PRINTF("Tx HDCP Status Changed:\n");
        SiiDrvAdaptTxHdcpDsStatusGet(inst, &HdcpStatus);
        switch (HdcpStatus)
        {
        case SII_DRV_ADAPTER_HDCP_TX_STATUS__OFF:
            PRINTF("   Tx HDCP Authentication Off.\n");
            break;
        case SII_DRV_ADAPTER_HDCP_TX_STATUS__AUTHENTICATING:
            PRINTF("   Tx HDCP Authenticating...\n");
            break;
        case SII_DRV_ADAPTER_HDCP_TX_STATUS__SUCCESS:
            PRINTF("   Tx HDCP Authentication Done.\n");
            break;
        case SII_DRV_ADAPTER_HDCP_TX_STATUS__FAILED:
            PRINTF("   Tx HDCP Authentication Failed.\n");
            break;
        default:
            SII_OS_DEBUG_ASSERT(0);
            break;
        }
        PRINTF("\n");
    }
    ////[kyq Begin: 20150814 11:43:39]
    // I find this is not happen, so I add EventFlags1 flag for RX
    if ((EventFlags2 & SII_DRV_ADAPT_EVENT__RX_HDCP_STATUS_CHNG)||
            (EventFlags1 & (SII_DRV_ADAPT_HDCP_EVENT__RX_AUTH_FAIL|
                            SII_DRV_ADAPT_HDCP_EVENT__RX_AUTH_START|
                            SII_DRV_ADAPT_HDCP_EVENT__RX_AUTH_DONE)))
    {
        SiiDrvAdaptHdcpRxStatus_t HdcpRxStatus  = SII_DRV_ADAPTER_HDCP_RX_STATUS__OFF;
        SiiDrvAdaptHdcpVer_t RxHdcpVer          = SII_DRV_ADAPTER_HDCP_VER__NOT_SUPPORTED;
        PRINTF("---9679-Rx HDCP  Version:----\n");
        SiiDrvAdaptRxHdcpUsVerGet(inst, &RxHdcpVer);
        switch (RxHdcpVer)
        {
        case SII_DRV_ADAPTER_HDCP_VER__NOT_SUPPORTED:
            PRINTF("   HDCP version not support\n");
            break;
        case SII_DRV_ADAPTER_HDCP_VER__1x:
            PRINTF("   HDCP version is: 1.x\n");
            break;
        case SII_DRV_ADAPTER_HDCP_VER__20:
            PRINTF("   HDCP version is: 2.0\n");
            break;
        case SII_DRV_ADAPTER_HDCP_VER__22:
            PRINTF("   HDCP version is: 2.2\n");
            break;
        default:
            SII_OS_DEBUG_ASSERT(0);
            break;
        }
        PRINTF("\n");
        PRINTF("Rx HDCP Status Changed:\n");
        SiiDrvAdaptRxHdcpUsStatusGet(inst, &HdcpRxStatus);
        switch (HdcpRxStatus)
        {
        case SII_DRV_ADAPTER_HDCP_RX_STATUS__OFF:
            PRINTF("   Rx HDCP Authentication Off.\n");
            break;
        case SII_DRV_ADAPTER_HDCP_RX_STATUS__SUCCESS:
            PRINTF("   Rx HDCP Authentication Done.\n");
            break;
        case SII_DRV_ADAPTER_HDCP_RX_STATUS__AUTHENTICATING:
            PRINTF("   Rx HDCP Authenticating...\n");
            break;
        case SII_DRV_ADAPTER_HDCP_RX_STATUS__FAIL:
            PRINTF("   Rx HDCP Authentication Failed.\n");
            break;
        default:
            SII_OS_DEBUG_ASSERT(0);
            break;
        }
        PRINTF("\n");
    }
}
//called by AdapterEventHandler() ---> DrvAdaptNtfSend() --->SiiDrvAdaptTaskExecute()--> main()
static void AdapterEdidEventHandler(SiiInst_t inst, uint32_t EventFlags)
{
    if (EventFlags & SII_DRV_ADAPT_EVENT__DS_EDID_STATUS_CHNG)
    {
#define EDID_BL0_ADR__EXTENSION_NMB 0x7E
        SiiDrvAdaptDsEdidStatus_t EdidStatus;
        uint8_t EdidData[EDID_BLOCK_LEN];
        uint8_t EdidExtensionCount = 0x00;
        memset(&EdidData[0], 0x00, sizeof(EdidData) / sizeof(EdidData[0]));
        SiiDrvAdaptTxDsEdidStatusGet(inst, &EdidStatus);
        PRINTF("Edid Status: %s\n", EdidStatus ? "Avaliable" : "Not Avaliable");
        if (EdidStatus)
        {
#if 1
            BYTE i = 0;
            BYTE                         acEdid[256];
            BOOL                        bNVRAMEdidValid;
            BYTE                         cInputEdidSelect = 0;
            PSYSTEM_SETTING_DATA         psSystemSettingData;
            PINPUTPORT_MANAGER_DATA    psInputPortMgrData;
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
            bNVRAMEdidValid = psSystemSettingData->bNVARMEDID_Inited;
            utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                              FALSE);
            SiiDrvAdaptRxHpdSet(inst, SII_DRV_ADAPT_INP_CTRL__DISABLE);        //拉低热拔插脚
            DrvAdaptInst_t *pDrvAdapt = DRV_ADAPT_INST2PTR(inst);
            i = pDrvAdapt->instCra;
            PRINTF("Edid Select %d\n",i);
            dvNVRAMUserEdidRead(edsEDID_IN1+i, acEdid);
            SiiDrvAdaptRxEdidSramWrite(inst, 0, acEdid, 256);
            delay_nms(30);
            SiiDrvAdaptRxHpdSet(inst, SII_DRV_ADAPT_INP_CTRL__ENABLE);     //拉高热拔插脚
            delay_nms(10);
#endif
            //EDID Block 0 read
            SiiDrvAdaptRxEdidSramRead(inst, 0, &EdidData[0], sizeof(EdidData) / sizeof(EdidData[0]));
            PRINTF("Edid Block 0 Data:\n");
            EdidBlockDataDisplay(EdidData);
            if (!EdidBlock0Check(EdidData))
            {
                PRINTF("Read Edid Block 0 Data error, should re-read again\n");
                return;
            }
            EdidExtensionCount = EdidData[EDID_BL0_ADR__EXTENSION_NMB];
            if (0 == EdidExtensionCount)
            {
                PRINTF("No Externsion, This is a DVI EDID block.\n");
            }
            if (1 == EdidExtensionCount)
            {
                //read EDID Block 1
                //SiiDrvAdaptTxDsEdidRead(inst, 1, 0, &EdidData[0], 128);
                SiiDrvAdaptRxEdidSramRead(inst, EDID_BLOCK_LEN, &EdidData[0], sizeof(EdidData) / sizeof(EdidData[0]));
                PRINTF("Edid Block 1 Data:\n");
                EdidBlockDataDisplay(EdidData);
            }
            if (EdidExtensionCount > 1)
            {
                //Max support 4 EDID Block read
                SiiDrvAdaptTxDsEdidRead(inst, 1, 0, &EdidData[0], sizeof(EdidData) / sizeof(EdidData[0]));
                PRINTF("Edid Block 1 Data:\n");
                EdidBlockDataDisplay(EdidData);
                SiiDrvAdaptTxDsEdidRead(inst, 2, 0, &EdidData[0], sizeof(EdidData) / sizeof(EdidData[0]));
                PRINTF("Edid Block 2 Data:\n");
                EdidBlockDataDisplay(EdidData);
                SiiDrvAdaptTxDsEdidRead(inst, 3, 0, &EdidData[0], sizeof(EdidData) / sizeof(EdidData[0]));
                PRINTF("Edid Block 3 Data:\n");
                EdidBlockDataDisplay(EdidData);
            }
        }
        PRINTF("\n");
    }
}
/*****************************************************************************/
/**
 *  @brief the events handler function.
 *
 *  @param[in]      inst     The instance.
 *  @param[in]      flags    The events flag.
 *
 *  @return     void.
 *
 * @note This function is called by SIMG driver upon HDMI/HDCP event
 * or error.  Be sure it is thread safe.
 *
 *****************************************************************************/
// called by---> DrvAdaptNtfSend() --->SiiDrvAdaptTaskExecute()--> main()
static void AdapterEventHandler(SiiInst_t inst, uint32_t EventAddr)
{
    SiiDrvAdaptEvents_t AdaptEvents;
    memcpy(&AdaptEvents, (SiiDrvAdaptEvents_t *)EventAddr, sizeof(SiiDrvAdaptEvents_t));
    if (AdaptEvents.GeneralEvents & (SII_DRV_ADAPT_EVENT__BOOT_DONE
                                     | SII_DRV_ADAPT_EVENT__ACCESS_DONE
                                     | SII_DRV_ADAPT_EVENT__SCDT_STATUS_CHNG
                                     | SII_DRV_ADAPT_EVENT__CKDT_STATUS_CHNG
                                     | SII_DRV_ADAPT_EVENT__RX_CD_SENSE_STATUS_CHG))
    {
        AdapterGeneralEventHandler(inst, AdaptEvents.GeneralEvents);
    }
    if (AdaptEvents.TxEvents)
    {
        AdapterTxEventHandler(inst, AdaptEvents.TxEvents);
    }
    if (AdaptEvents.RxEvents & (SII_DRV_ADAPT_EVENT__US_CONNECT_STATUS_CHNG
                                | SII_DRV_ADAPT_EVENT__RX_AV_MUTE_STATUS_CHNG))
    {
        AdapterRxEventHandler(inst, AdaptEvents.RxEvents);
    }
    if (AdaptEvents.HdcpEvents
            || (AdaptEvents.RxEvents & (SII_DRV_ADAPT_EVENT__RX_HDCP_STATUS_CHNG)))
    {
        AdapterHdcpEventHandler(inst, AdaptEvents.HdcpEvents, AdaptEvents.RxEvents);
    }
#if 0
    if ((AdaptEvents.CbusEvents)
            ||(AdaptEvents.GeneralEvents&(SII_DRV_ADAPT_EVENT__VBUS_REQUEST
                                          |SII_DRV_ADAPT_EVENT__CBUS_EVENT
                                          |SII_DRV_ADAPT_EVENT__CBUS_STATE_CHNG)))
    {
        AdapterMhlEventHandler(inst, AdaptEvents.CbusEvents, AdaptEvents.GeneralEvents);
    }
#endif
    if (AdaptEvents.EdidEvents)
    {
        AdapterEdidEventHandler(inst, AdaptEvents.EdidEvents);
    }
} // AdapterEventHandler
static bool_t AdapterVersionGet(SiiInst_t inst)
{
    SiiDrvAdaptVersionInfo_t    s_ChipVersion;
    (void)SiiDrvAdaptVersionGet(inst, &s_ChipVersion);
    delay_nms(500);
    if (s_ChipVersion.chipId != CHIP_ID)
    {
        delay_nms(10);
        (void)SiiDrvAdaptVersionGet(inst, &s_ChipVersion);
    }
    PRINTF("*********************\n");
    PRINTF("Chip ID: 0x%x\n", (int)s_ChipVersion.chipId);
    PRINTF("Firmware Version: %02x.%02x.%02x\n", (int)(s_ChipVersion.fwVersion >> 13), (int)((s_ChipVersion.fwVersion & 0x1FFF) >> 5), (int)(s_ChipVersion.fwVersion & 0x001F));
    PRINTF("CustomerVersion = 0x%02x\n", s_ChipVersion.fwCustomerVersion);
    PRINTF("*********************\n");
    if (((uint8_t)(s_ChipVersion.fwVersion >> 13) == CHIP_FW_VER_H) &&
            ((uint8_t)((s_ChipVersion.fwVersion & 0x1FFF) >> 5) == CHIP_FW_VER_L) &&
            ((uint8_t)(s_ChipVersion.fwVersion & 0x001F) == CHIP_FW_REV))
    {
        return true;  //FW version match
    }
    else
    {
        return false;
    }
}
// return false: boot fail
static bool_t AdapterWaitBootDone(SiiInst_t inst)
{
    uint8_t ucHwResetCount = 3; //Max reset time
    uint8_t ucRetryI2cCount = 2; //max wait times of FW I2C ready.
    uint32_t i = 1;
    uint8_t  result = 0;
    SiiDrvAdaptBootStatus_t BootStat = SII_DRV_ADAPTER_BOOTING__IN_PROGRESS;
    PRINTF("\nWaiting Boot Done\n");
    //_ SiiHalTimerSet((uint8_t)TIMER_TIMEOUT,550); // if load FW failed, the time should less than 550ms
#if 0
    while (FALSE)
    {
        ;    //_ !SiiHalGetIntFlag())  //only boot_done occured, this interrupt flag avaliable
    }
    {
        PRINTF(".");
        while (0 == (i++)%50)
        {
            PRINTF("\n");
        }
        //_ SiiHalTimerWait(50);
        if (0x00 == ucHwResetCount)
        {
            PRINTF("\nHardware boot done interrupt detection failed.\n");
            return false;
        }
        //等待550 ms ，重新RESET,
        if (SiiHalTimerExpired((uint8_t)TIMER_TIMEOUT))
        {
            SiiHalTimerSet((uint8_t)TIMER_TIMEOUT,550);
            //PRINTF(("\nRest Chip again\n"));
            SiiHalChipReset();
            ucHwResetCount--;
        }
    }
#endif
    //PRINTF(("\n")); //until now, SiI9679 HW boot done, waiting FW I2C ready...
    PRINTF("\n###Boot Done \n");
    //  SiiHalTimerWait(200); //wait FW I2C slave ready, from INT pin assertion to I2C ready
    /*
    while(1)
    {
     result = SiiDrvVirtualPageRead((DrvAdaptInst_t *)inst, 0x02);
     printf("Read =0x%bx\n",result);
    }
    */
    SiiDrvAdaptBootStatusGet(inst, &BootStat);
    //_ SiiHalTimerSet((uint8_t)TIMER_TIMEOUT,200); // set timer 100ms to wait FW I2C ready...
    while (SII_DRV_ADAPTER_BOOTING__SUCCESS != BootStat)
    {
        PRINTF(".");
        while (0 == (i++) % 50)
        {
            PRINTF("\n");
        }
        //_SiiHalTimerWait(50);
        SiiDrvAdaptBootStatusGet(inst, &BootStat);
        if (0x00 == ucRetryI2cCount)
        {
            PRINTF("\nFirmware boot failed.\n");
            return false;
        }
        /*
          if (SiiHalTimerExpired((uint8_t)TIMER_TIMEOUT))
          {
              SiiHalTimerSet((uint8_t)TIMER_TIMEOUT,200);
              //PRINTF(("\nWaiting Firmware I2C ready...\n"));
              ucRetryI2cCount--;
          }
          */
        ucRetryI2cCount--;
    }
    PRINTF("\n");
    return true;
}
void AdapterBootEventHandler(SiiInst_t inst, bool_t bBootStatus)
{
    if (bBootStatus)
    {
        //PRINTF(("Booting is done successfully.\n\n"));
        if (false == AdapterVersionGet(inst))
        {
            PRINTF("***Error: Chip FW Version not match.\n");
        }
        SiiDrvAdaptStart(inst);
        //PRINTF(("\n"));
    }
}
void dv9679AdapterSystemInit(void)
{
    static bool_t               bBootStatus = false;
    SiiDrvAdaptConfig_t         adaptCfg;
    uint8_t                       i = 0, chipIdL = 0;
    //SiiHalChipReset();   //toggle reset pin to reset SiI9679 Chip
    //SiiHalRemoteHandlerInit(SiiRemoteRXHandler)
    //-----------------------------------------------------------
    // Adapter driver related portion starts here
    //-----------------------------------------------------------
    // Allocate & initialize memory structures within SIMG API
    for (i = 0; i < INSTANCE_9679; i++)      //Intialize 9679
    {
        // it's drvAdaptInstRecord[]
        m_apsInstAdapter[i] = SiiDrvAdaptCreate((SiiInst_t)0, i, AdapterEventHandler); //drvAdaptInstRecord[i].pNtfCallback  =  AdapterEventHandler
        if (m_apsInstAdapter[i])
        {
            // Setup initial chip configuration
            adaptCfg.platformId             = SII_DRV_ADAPT_PLATFORM__SII9679;  //configure driver for 9679/9678/AVR platform
            adaptCfg.i2cSlaveAddr           = ADAPT_DEFAULT_I2C_DEVICE_ID; // use default I2C address
            adaptCfg.i2cSlaveAddrBase       = SII_DRV_ADAPT_DEFAULT_I2C_DEVICE_ID_BASE; // Notify the driver to use this slave address when in debug mode
            adaptCfg.i2cSlaveAddrF0         = 0;  // 0 to use default 0xF0 I2C address, only used at SPI flash update
            adaptCfg.i2cSlaveAddrAA         = 0;  // 0 to use default 0xAA I2C address, only used at SPI flash update
            adaptCfg.interruptPinType       = SII_DRV_ADAPTER_INT__OPENDRAIN;
            adaptCfg.interruptPolarity      = SII_DRV_ADAPTER_INT__POL_LOW;
            adaptCfg.spiConfig.eraseCmd     = SPI_ERASE_COMMAND;
            adaptCfg.spiConfig.writeEnCmd   = SPI_WRITE_ENABLE_COMMAND;
            adaptCfg.spiConfig.pageWrCmd    = SPI_PAGE_WRITE_COMMAND;
            //copy adaptCfg to m_apsInstAdapter->config
            SiiDrvAdaptConfigure(m_apsInstAdapter[i], &adaptCfg);
            //-----------------------------------------------------
            // Wait Boot done
            //-----------------------------------------------------
            //bBootStatus = true;
            bBootStatus = AdapterWaitBootDone(m_apsInstAdapter[i]);
            //AdapterBootEventHandler(m_apsInstAdapter, bBootStatus);
            if (bBootStatus)
            {
                PRINTF("9679[%d] Booting is done successfully.\n\n",i);
            }
            bBootStatus = true;
            AdapterBootEventHandler(m_apsInstAdapter[i], bBootStatus);
        }
    }
}
// 控制9679 输入的HPD
void dvSiiDrvAdaptRxHpdSet(BYTE cInstance, SiiDrvAdaptInpCtrl_t hpd)
{
    assert_param(m_apsInstAdapter[cInstance] != NULL);
    SiiDrvAdaptRxHpdSet(m_apsInstAdapter[cInstance], hpd);
}
//更新输入端的Edid
void dvSiiDrvAdaptRxEdidSramWrite(BYTE cInstance,
                                  uint16_t offset,
                                  uint8_t  *pData,
                                  uint16_t length)
{
    assert_param(m_apsInstAdapter[cInstance] != NULL);
    PRINTF("RX[%d] Edid Write:\n",cInstance);
    SiiDrvAdaptRxEdidSramWrite(m_apsInstAdapter[cInstance], offset, pData, length);
    EdidBlockDataDisplay(pData);
    if(length>128)
    {
        EdidBlockDataDisplay(pData+128);
    }
}
// 设置输9679 是否输出信号
void  dvSiiDrvAdaptTxTmdsOutSet(BYTE cInstance,  SiiDrvAdaptTmdsOut_t tmdsCtrl)
{
    assert_param(m_apsInstAdapter[cInstance] != NULL);
    SiiDrvAdaptTxTmdsOutSet(m_apsInstAdapter[cInstance], tmdsCtrl);
}
void dvSiiDrvAdaptTaskExecute(BYTE cInstance)
{
    SiiDrvAdaptTaskExecute( cInstance, m_apsInstAdapter[cInstance]);
}
// return : true have HDCP
//             false no HDCP
//SiiDrvAdaptHdcpVer_t preRxHdcpVer[INSTANCE_9679];

BOOL dvSiiDrvAdaptRxHDCPGet(BYTE cInstance)
{
    BOOL  bRetVal = TRUE;
    SiiDrvAdaptHdcpVer_t  RxHdcpVer;
    RxHdcpVer = SiiDrvAdaptRxHdcpVerGet(m_apsInstAdapter[cInstance]);
    //SiiDrvAdaptRxHdcpUsVerGet(m_apsInstAdapter[cInstance], &RxHdcpVer);
    if(SII_DRV_ADAPTER_HDCP_VER__NOT_SUPPORTED == RxHdcpVer)
    {
        bRetVal = FALSE;
    }
    #if 0
    if(preRxHdcpVer[cInstance]!=RxHdcpVer)
    {
        preRxHdcpVer[cInstance]=RxHdcpVer;
        Uart_printf("RxHdcpVer[%d]== %d\n",cInstance,RxHdcpVer);
    }
    #endif
    return bRetVal;
}
// 得到9679  输入端是否有接信号?// return TRUE: connected
BOOL dvSiiDrvAdaptRxConnectGet(BYTE cInstance)
{
    BOOL  bRetVal = TRUE;
    SiiDrvAdaptLinkType_t RxConType = SiiDrvAdaptRxConTypeGet(m_apsInstAdapter[cInstance]);
    if(SII_DRV_LINK_HDMI != RxConType)
    {
        bRetVal = FALSE;
    }
    return bRetVal;
}
// 得到9679  输出端是否使能?// return TRUE: Is Enable
//           Fasle is Disable
BOOL dvSiiDrvAdaptTxTmdsOutEnGet(BYTE cInstance)
{
    BOOL  bRetVal = TRUE;
    SiiDrvAdaptTmdsOut_t TxEn = SiiDrvAdaptTxTmdsOutGet(m_apsInstAdapter[cInstance]);
    if(SII_DRV_ADAPT_TMDS_OUT__DISABLE == TxEn)
    {
        bRetVal = FALSE;
    }
    return bRetVal;
}
