/******************************************************************************
 *
 * Copyright 2011, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * file  si_drv_adapter_internal.h
 *
 * @brief Internal Adapter Driver definitions
 *
 *****************************************************************************/
#ifndef SI_DRV_ADAPT_INTERNAL_H
#define SI_DRV_ADAPT_INTERNAL_H
#ifdef __cplusplus
extern "C"
{
#endif
    /***** #include statements ***************************************************/
#include <stdio.h>
#include "stm32f10x.h"
#include "si_drv_adapter.h"
#include "si_drv_adapter_rx.h"
#include "Si_drv_adapter_tx.h"
    /***** public macro definitions **********************************************/
#define CHIP_ID         0x9679
#define HOST_SW_VER_H   0x01
#define HOST_SW_VER_L   0x02
#define HOST_SW_REV     0x02
#define CHIP_FW_VER_H   0x01
#define CHIP_FW_VER_L   0x02
#define CHIP_FW_REV     0x02
//#define CHIP_FW_CUSTOMER_SUB_VER 0x00
#define ADAPT_DEFAULT_I2C_DEVICE_ID 0x70 // 0x76
#define SPI_ERASE_COMMAND        0xC7
#define SPI_WRITE_ENABLE_COMMAND 0x06
#define SPI_PAGE_WRITE_COMMAND   0x02
#define DRV_ADAPT_INST2PTR(inst) ((DrvAdaptInst_t *) inst)
#define DRV_ADAPT_PTR2INST(inst) ((SiiInst_t) inst)
// Maximum number of adapter driver instances
#define MAX_NMB_OF_ADAPTER_INSTANCES               INSTANCE_9679
#define SII_MI2C_TEN        0x0010  //!< Set for ten bit chip address; cleared otherwise
#define SII_MI2C_RD         0x0001  //!< Set for read data operation; cleared for write operation
#define SII_MI2C_WR         0x0000  //!< Absent of read == write
#define I2C_NOERROR     (0)
#define I2C_NOACK       (1)
//#define I2C_GEN_ERROR   (2) // no used
#define I2C_BUS_TIMEOUT     10250   //set the timeout value to about 100ms
#define TWI_REGSIZE         256     // This is the size of I2C slave
// This can be any values smaller or equal to 256
#define FW_UPDATE_SLAVE_ADDRESS     0xAA
#define NUM_EXT_BLOCKS_INDEX        126
#define MAX_EXT_BLOCKS              1
#define EDID_BLOCK_SIZE             128
#define BOOT_BLOCK_SIZE             64
#define MAX_EDID_SRAM_SIZE          256
#define MAX_BOOT_SRAM_SIZE          64
    extern SiiInst_t     m_apsInstAdapter[];
    /***** public type definitions ***********************************************/
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
//[kyq Begin: 20150804 9:16:08]
        bool_t                   scdtDetStatus; // Sync Detect:
        bool_t                   ckdtDetStatus; // Clock Detect:
        bool_t                   cdSenseStatus; // Cable detect:
        SiiDrvAdaptHdcpVer_t RxHdcpVer;
        SiiDrvAdaptHdcpRxStatus_t HdcpRxStatus;
        SiiDrvAdaptLinkType_t   RxConType;
        SiiDrvAdaptTmdsOut_t   eTxTmdsOutEn;

//[kyq End: 20150804 9:16:09]
    } DrvAdaptInst_t;
    /***** public type definitions ***********************************************/
    /**
    * @brief Platform layer functions error code
    */
    typedef enum
    {
        SII_OS_STATUS_SUCCESS,           //!< No error
        SII_OS_STATUS_ERR_INVALID_PARAM, //!< One of parameters of called OSAL function is invalid, == I2C_NOACK
        SII_OS_STATUS_ERR_NOT_AVAIL,     //!< OSAL function does not support requested operation
        SII_OS_STATUS_ERR_FAILED         //!< Other error types
    } SiiOsStatus_t;
    typedef enum
    {
        SII_OS_DEBUG_FORMAT_SIMPLE    = 0x0000u,
        SII_OS_DEBUG_FORMAT_FILEINFO  = 0x0001u,
        SII_OS_DEBUG_FORMAT_TIMESTAMP = 0x0004u
    } SiiOsDebugFormat_e;
    /**
    * @brief I2C transaction details for @ref SiiHalMasterI2cTransfer() function
    *
    * Similar to Linux's \c struct \c i2c_msg structure.
    */
    typedef struct
    {
        uint16_t addr;      //!< Slave I2C address
        uint16_t cmdFlags;  //!< Flags defining message actions
        uint16_t len;       //!< Buffer length in bytes
        uint8_t *pBuf;      //!< Pointer to input (for write operations) or
        //!< Pointer to output (for read operations) buffer
    } SiiI2cMsg_t;
    /**
    * @brief Input control selection
    *
    * Used in @ref SiiDrvAdaptRxHpdSet(), @ref SiiDrvAdaptRxTermSet(),
    * @ref SiiDrvAdaptEdidDdcSet(), and @ref SiiDrvAdaptHdcpDdcSet()
    * functions.
    *
    typedef enum
    {
        SII_DRV_ADAPT_INP_CTRL__BYPASS,  //!< Copy from downstream
        SII_DRV_ADAPT_INP_CTRL__DISABLE, //!< Disable or set LOW
        SII_DRV_ADAPT_INP_CTRL__ENABLE,  //!< Enable or set HIGH
    } SiiDrvAdaptInpCtrl_t;
    typedef enum
    {
        SII_DRV_ADAPT_TMDS_OUT__DISABLE, //!< TMDS output is disabled
        SII_DRV_ADAPT_TMDS_OUT__ENABLE   //!< TMDS output is enabled
    } SiiDrvAdaptTmdsOut_t;
    */
    /**
    * @brief TMDS output state
    *
    * Used in @ref SiiDrvAdaptDsEdidStatusGet() function
    */
    /*
    typedef enum
    {
        SII_DRV_ADAPT_DS_EDID__NOT_AVAILABLE, //!< Downstream EDID is not available
        SII_DRV_ADAPT_DS_EDID__AVAILABLE      //!< Downstream EDID is available
    } SiiDrvAdaptDsEdidStatus_t;
    */
    /***** public functions ******************************************************/
//SiiOsStatus_t   SiiDrvGetI2cAccessStatus(void);
    void            SiiDrvVirtualPageWriteWord(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint16_t value);
    void            SiiDrvVirtualPageWrite(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint8_t value);
    void            SiiDrvVirtualPageWriteBlock(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, const uint8_t *pBuffer, uint16_t count);
    uint8_t         SiiDrvVirtualPageRead(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr);
    void            SiiDrvVirtualPageReadBlock(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint8_t *pBuffer, uint16_t count);
    void            SiiDrvVirtualRegModify(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint8_t mask, uint8_t value);
    void            SiiDrvVirtualRegBitsSet(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint8_t bitMask, bool_t setBits);
    void            SiiDrvVirtualPageReadWord(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint16_t *value);
    uint8_t         SiiDrvRegReadByte(DrvAdaptInst_t *pDrvAdapt,  uint8_t pageAddr, uint8_t pageoffset);
    void            SiiDrvRegWriteByte(DrvAdaptInst_t *pDrvAdapt,  uint8_t pageAddr, uint8_t pageoffset, uint8_t value);
    void            SiiDrvRegReadBlock(DrvAdaptInst_t *pDrvAdapt,  uint8_t pageAddr, uint8_t pageoffset, uint8_t *pBuffer, uint16_t count);
    void            SiiDrvRegWriteBlock(DrvAdaptInst_t *pDrvAdapt,  uint8_t pageAddr, uint8_t pageoffset, uint8_t *pBuffer, uint16_t count);
    void AdapterBootEventHandler(SiiInst_t inst, bool_t bBootStatus);
    void SiiHalChipReset(void);
#ifdef __cplusplus
}
#endif
#endif // SI_DRV_ADAPT_INTERNAL_H
