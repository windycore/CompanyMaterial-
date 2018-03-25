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
 * @file si_drv_adapter_internal.c
 *
 * @brief Adapter Driver private functions.
 *
 *****************************************************************************/
/***** #include statements ***************************************************/
#include "si_drv_adapter_internal.h"
#include "si_virtual_page.h"
#include "main.h"
#include "Mcu.h"
#include "uart.h"
#include "IR.h"
#include "dvCPLD.h"
#include "bsp_i2c.h"
#include "dvPC9539.h"
#define SII9679_IICFLG   FLG_9679I2C
static const BYTE m_acBusID[INSTANCE_9679] =
{
#if ((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431)||(MACHINETYPE==MUH44TPR2_N)||(MACHINETYPE==MUH44F)||(MACHINETYPE==PRD_VHD4))
    eI2CBus9679,
    
#if(INSTANCE_9679>1)
    eI2CBus9679,
#endif

#if(INSTANCE_9679>2)
    eI2CBus9679,
#endif

#if(INSTANCE_9679>3)
    eI2CBus9679,
#endif

#elif ((MACHINETYPE==MUH88E_BYE)||(MACHINETYPE==MUH4K862))
    eI2CBus9679_1,
    
#if (INSTANCE_9679>1)
    eI2CBus9679_1,
#endif

#if (INSTANCE_9679>2)
    eI2CBus9679_1,
#endif

#if (INSTANCE_9679>3)
    eI2CBus9679_1,
#endif

#if (INSTANCE_9679>4)
    eI2CBus9679_2,
#endif

#if (INSTANCE_9679>5)
    eI2CBus9679_2,
#endif

#if (INSTANCE_9679>6)
    eI2CBus9679_2,
#endif

#if (INSTANCE_9679>7)
    eI2CBus9679_2,
#endif

#endif
};

extern void device_slect_9679(BYTE deviceID);//·¶Î§ÊÇ1µ½10
static BYTE hcSii9679BusSelect = 0xff;
void SiiHalI2cBusSelect(BYTE cInstance)
{
    assert_param(cInstance<INSTANCE_9679);
#if ((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431))
       device_slect_9679(cInstance);
#elif(MACHINETYPE==MUHBT44)
    WORD wData= 0x1fff;
    WORD GPIOVaule;
    assert_param(cInstance<INSTANCE_9679);
    /*
    A: BIT0
    B: BIT1
    EN:BIT4
    Enable   B     A     ON Channels
    BIT4     BIT1 BIT0
    L            0   0       Y0  X0
    L            0   1       Y1  X1
    L            1   0       Y2  X2
    L            1   1       Y3  X3
    H            X  X        None
    */
    wData = ((WORD) cInstance) <<15;
    if (hcSii9679BusSelect == cInstance)
    {
        return;
    }
    if (cInstance < INSTANCE_9679)
    {
        dvPC9539Read(epi9539_IN5VDET_OUTHPDDET,outport0,(uint8_t*)&GPIOVaule);
        GPIOVaule&=~(0x03<<15);
        GPIOVaule|=wData;
        dvPC9539Write(epi9539_IN5VDET_OUTHPDDET,outport0,GPIOVaule);
        hcSii9679BusSelect = cInstance;
    }
#elif((MACHINETYPE==MUH88E_BYE)||(MACHINETYPE==MUH4K862))
    device_slect_9679(cInstance);
#endif
}
/***** local macro definitions ***********************************************/
static SiiOsStatus_t I2cAccessStatus = SII_OS_STATUS_ERR_FAILED;
/***** public functions ******************************************************/
/*****************************************************************************/
/**
* @brief Return I2C status, ACK(SII_OS_STATUS_SUCCESS)/NACK(SII_OS_STATUS_ERR_FAILED)
*
* @param[in] N/A
*
* @return     SiiOsStatus_t status of operation (0 == success).
*
*****************************************************************************/
SiiOsStatus_t SiiDrvGetI2cAccessStatus(void)
{
    return I2cAccessStatus;
}
#if 0
//-----------------------------------------------------------------------------
// I2C transfer function
//-----------------------------------------------------------------------------
SiiOsStatus_t SiiHalMasterI2cTransfer(int busId, SiiI2cMsg_t *pMsgs, uint8_t msgNum)
{
    int                 msgIndex;
    SiiOsStatus_t       osStatus = SII_OS_STATUS_SUCCESS;
    // busId
    // It is always 0 in current implementation.
    // It is reserved to potentially be used in in the future systems with multiple adapter chips
    // connected to different physical I2C busses.
    busId = busId; // to avoid compiler warnings about unused variable
    // msgNum is 1 for write operations (as there is only 1 transaction
    // that includes register address writing and the data payload)
    // and 2 for read operations (one transaction to write register address
    // and the other to read the data).
    msgNum = msgNum; // to avoid compiler warnings about unused variable
    // pMsgs points to the I2C transaction details info.
    pMsgs = pMsgs; // to avoid compiler warnings about unused variable
    // SiiHalMasterI2cTransfer() mimics Linux's i2c_transfer() function.
    // Please check Linux documentation on the following API function:
    // int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msg, int num);
    // Here is an example of how the function can be implemented in non-Linux systems
    for ( msgIndex = 0; msgIndex < msgNum; msgIndex++ )
    {
        SiiI2cMsg_t *pMsg = &pMsgs[msgIndex];
        pMsg = pMsg; // to avoid compiler warnings about unused variable
        if (pMsgs[msgIndex].cmdFlags & SII_MI2C_RD)
        {
            // Send Start condition and receive data.
            // Send Stop condition for the last transaction.
            uint16_t i = 0;
            for(i = 0; i < 3000; i++)
            {
                i++;
            }
            I2C_Start(IIC_9187_PI3HDMI412);
            for(i = 0; i < 2000; i++)
            {
                i++;
            }
            if (twi_send_MRslave(pMsg->addr | SII_MI2C_RD))
            {
                I2C_Stop(IIC_9187_PI3HDMI412);
                //printf("I2C Send Error:1\r\n");
                return (SiiOsStatus_t)I2C_NOACK;             /* NoACK */
            }
            for (i=0; i<pMsg->len; i++)
            {
                if (twi_get_data(pMsg->pBuf +i, i==pMsg->len-1) != 0)
                {
                    I2C_Stop(IIC_9187_PI3HDMI412);
                    //printf("I2C Send Error:2\r\n");
                    return (SiiOsStatus_t)I2C_NOACK;
                }
                //delay_nms(1);
                //UART1_SendByte(*(pMsg->pBuf +i));
            }
            if (msgIndex == (msgNum - 1))
            {
                I2C_Stop(IIC_9187_PI3HDMI412);
            }
        }
        else
        {
            // Send Start condition and send data.
            // Send Stop condition for the last transaction.
            uint16_t i = 0;
            I2C_Start(IIC_9187_PI3HDMI412);
            if (twi_send_MTslave(pMsg->addr | SII_MI2C_WR))
            {
                I2C_Stop(IIC_9187_PI3HDMI412);
                //printf("I2C Send Error:3\r\n");
                return (SiiOsStatus_t)I2C_NOACK; /* NoACK */
            }
            if (twi_send_data(pMsg->pBuf[0]))
            {
                I2C_Stop(IIC_9187_PI3HDMI412);
                //printf("I2C Send Error:4\r\n");
                return (SiiOsStatus_t)I2C_NOACK; /* NoACK */
            }
            for (i=1; i<pMsg->len; i++)
            {
                if (twi_send_data(pMsg->pBuf[i]))
                {
                    I2C_Stop(IIC_9187_PI3HDMI412);
                    //printf("I2C Send Error:5\r\n");
                    return (SiiOsStatus_t)I2C_NOACK;   /* NoACK */
                }
                //delay_nms(1);
                //sprintf(USART1_tx_buff,"%02x ",(int)pMsg->pBuf[i]);
                //UART1_SendBytes((uint8_t *)USART1_tx_buff,strlen(USART1_tx_buff));
            }
            if (msgIndex == (msgNum - 1))
            {
                I2C_Stop(IIC_9187_PI3HDMI412);
            }
        }
        if (osStatus != SII_OS_STATUS_SUCCESS)
        {
            break;
        }
    }
    // NOTE:
    // If I2C error checking is needed to be done, insert an error callback here
    // or store the last I2C error result to be read by a separate function.
    // Adapter driver does not propagate the I2C error indication to the application.
    return osStatus; // SII_OS_STATUS_SUCCESS in case of success or SII_OS_STATUS_ERR_FAIL in case of failure
}
#endif
/*****************************************************************************/
/**
* @brief Read a byte or bytes from the specified bus and i2c device.
*
* @param[in] busIndex    - I2C bus selection
* @param[in] deviceId    - I2C device address, eight-bit format (addr + r/w bit)
* @param[in] regAddr     - Eight or sixteen bit starting register address
* @param[in]  pBuffer     - destination buffer
* @param[in]  count       - number of registers (bytes) to read
*
* @return     SiiOsStatus_t status of operation (0 == success).
*
*****************************************************************************/
static eRESULT CraReadBlockI2c(BYTE cInstance, uint8_t deviceId, uint16_t regAddr, uint8_t *pBuffer, uint16_t count)
{
    eRESULT  eResult = rcSUCCESS;
    SiiHalI2cBusSelect(cInstance);
    eResult = dvDeviceBusRead(  m_acBusID[cInstance],deviceId, regAddr, count ,pBuffer, SII9679_IICFLG);
    return eResult;
}
/*****************************************************************************/
/**
* @brief Write a byte or bytes from the specified bus and i2c device.
*
* @param[in] busIndex    - I2C bus selection
* @param[in] deviceId    - I2C device address, eight-bit format (addr + r/w bit)
* @param[in] regAddr     - Eight or sixteen bit starting register address
* @param[in]  pBuffer     - source data buffer.
* @param[in]  count       - number of registers (bytes) to write (maximum 256)
*
* @return     SiiOsStatus_t status of operation (0 == success).
*
*****************************************************************************/
static eRESULT CraWriteBlockI2c(BYTE cInstance, uint8_t deviceId, uint16_t regAddr, const uint8_t *pBuffer, uint16_t count)
{
    eRESULT  eResult = rcSUCCESS;
    SiiHalI2cBusSelect(cInstance);
    eResult = dvDeviceBusWrite(m_acBusID[cInstance],deviceId, regAddr, count ,pBuffer, SII9679_IICFLG);
    return eResult;
}
/*****************************************************************************/
/**
* @brief Read a byte from the virtual page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] virtualAddr    starting register address
*
* @return the got data from the virtual page
*
*****************************************************************************/
uint8_t SiiDrvVirtualPageRead(DrvAdaptInst_t *pDrvAdapt,  uint8_t virtualAddr)
{
    uint8_t             value = 0;
    (void) CraReadBlockI2c(pDrvAdapt->instCra, pDrvAdapt->config.i2cSlaveAddr, virtualAddr, &value, 1 );
    //(void) CraReadBlockI2c(0, 0x70, virtualAddr, &value, 1 );
    return value;
}
/*****************************************************************************/
/**
* @brief Write a byte to the virtual page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] virtualAddr    starting register address
* @param[in] value          source data value.
* @return
*
*****************************************************************************/
void SiiDrvVirtualPageWrite(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint8_t value)
{
    (void) CraWriteBlockI2c(pDrvAdapt->instCra, pDrvAdapt->config.i2cSlaveAddr, virtualAddr, &value, 1);
}
void SiiDrvVirtualPageWriteWord(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint16_t value)
{
    uint8_t buffer[2];
    buffer[0] = value &0xFF;
    buffer[1] = (value>>8) &0xFF;
    (void) CraWriteBlockI2c(pDrvAdapt->instCra, pDrvAdapt->config.i2cSlaveAddr, virtualAddr, buffer, 2);
}
/*****************************************************************************/
/**
* @brief Write bytes to the virtual page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] virtualAddr    starting register address
* @param[in] value          source data buffer
* @param[in] count          the number of bytes to write
* @return
*
*****************************************************************************/
void SiiDrvVirtualPageWriteBlock(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, const uint8_t *pBuffer, uint16_t count)
{
    (void) CraWriteBlockI2c(pDrvAdapt->instCra, pDrvAdapt->config.i2cSlaveAddr, virtualAddr, pBuffer, count);
}
/*****************************************************************************/
/**
* @brief Read bytes from the virtual page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] virtualAddr    starting register address
* @param[out] pBuffer       destination data buffer
*
* @return
*
*****************************************************************************/
void SiiDrvVirtualPageReadBlock(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint8_t *pBuffer, uint16_t count)
{
    (void) CraReadBlockI2c(pDrvAdapt->instCra, pDrvAdapt->config.i2cSlaveAddr, virtualAddr, pBuffer, count );
}
void SiiDrvVirtualPageReadWord(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint16_t *value)
{
    uint8_t buffer[2] = {0,0};
    (void) CraReadBlockI2c(pDrvAdapt->instCra, pDrvAdapt->config.i2cSlaveAddr, virtualAddr, buffer, 2);
    *value = buffer[1];
    *value <<=8;
    *value += buffer[0];
}
/*****************************************************************************/
/**
* @brief modify the value on the virtual page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] virtualAddr    starting register address
* @param[in] mask           mask for the parameter of value
* @param[in] value          source data
*
* @return
*
*****************************************************************************/
void SiiDrvVirtualRegModify(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint8_t mask, uint8_t value)
{
    uint8_t aByte;
    aByte = SiiDrvVirtualPageRead(pDrvAdapt, virtualAddr);
    aByte &= (~mask);                       // first clear all bits in mask
    aByte |= (mask & value);                // then set bits from value
    SiiDrvVirtualPageWrite(pDrvAdapt, virtualAddr, aByte );
}
/*****************************************************************************/
/**
* @brief set the related bits on the virtual page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] virtualAddr    starting register address
* @param[in] bitMask        mask value
* @param[in] setBits        whether to set or clean these bits
*
* @return
*
*****************************************************************************/
void SiiDrvVirtualRegBitsSet(DrvAdaptInst_t *pDrvAdapt, uint8_t virtualAddr, uint8_t bitMask, bool_t setBits)
{
    uint8_t aByte;
    aByte = SiiDrvVirtualPageRead(pDrvAdapt, virtualAddr);
    aByte = (setBits) ? (aByte | bitMask) : (aByte & ~bitMask);
    SiiDrvVirtualPageWrite(pDrvAdapt, virtualAddr, aByte);
}
/*****************************************************************************/
/**
* @brief Read a byte from the chip hardware page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] pageAddr       I2C slave address
* @param[in] pageoffset     starting register address
*
* @return the got data from the virtual page
*
*****************************************************************************/
uint8_t SiiDrvRegReadByte(DrvAdaptInst_t *pDrvAdapt,  uint8_t pageAddr, uint8_t pageoffset)
{
    uint8_t             value = 0;
    (void)CraReadBlockI2c (pDrvAdapt->instCra, pageAddr, pageoffset, &value, 1 );
    return value;
}
/*****************************************************************************/
/**
* @brief Write a byte to the chip hardware page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] pageAddr       I2C slave address
* @param[in] pageoffset     starting register address
* @param[in] value          destination data value.
* @return
*
*****************************************************************************/
void SiiDrvRegWriteByte(DrvAdaptInst_t *pDrvAdapt,  uint8_t pageAddr, uint8_t pageoffset, uint8_t value)
{
    (void)CraWriteBlockI2c(pDrvAdapt->instCra, pageAddr, pageoffset, &value, 1);
}
/*****************************************************************************/
/**
* @brief Read bytes from the chip hardware page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] pageAddr       I2C slave address
* @param[in] pageoffset     starting register address
* @param[in] pBuffer        destination data buffer
* @param[in] count          the number of bytes to write
*
*****************************************************************************/
void SiiDrvRegReadBlock(DrvAdaptInst_t *pDrvAdapt,  uint8_t pageAddr, uint8_t pageoffset, uint8_t *pBuffer, uint16_t count)
{
    (void)CraReadBlockI2c(pDrvAdapt->instCra, pageAddr, pageoffset, pBuffer, count );
}
/*****************************************************************************/
/**
* @brief write bytes from the chip hardware page of i2c device.
*
* @param[in] pDrvAdapt      the pointer to the Adapter Instance
* @param[in] pageAddr       I2C slave address
* @param[in] pageoffset     starting register address
* @param[in] pBuffer        source data buffer
* @param[in] count          the number of bytes to write
*
*****************************************************************************/
void SiiDrvRegWriteBlock(DrvAdaptInst_t *pDrvAdapt,  uint8_t pageAddr, uint8_t pageoffset, uint8_t *pBuffer, uint16_t count)
{
    (void)CraWriteBlockI2c(pDrvAdapt->instCra, pageAddr, pageoffset, pBuffer, count);
}
