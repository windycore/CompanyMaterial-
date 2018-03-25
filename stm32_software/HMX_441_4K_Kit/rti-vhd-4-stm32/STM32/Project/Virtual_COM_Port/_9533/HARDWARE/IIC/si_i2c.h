#ifndef __SI_I2C_H
#define __SI_I2C_H
#include "stm32f10x.h"
#include "si_common.h"
#include "si_platform.h"

//#define SII9533_NUM_INSTANCES   8
//void HalI2cBus0WriteByte( uint8_t deviceID, uint8_t offset, uint8_t value );
//uint8_t HalI2cBus0ReadByte( uint8_t deviceID, uint8_t addr );
//uint8_t HalI2cBus0ReadBlock( uint8_t deviceID, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
//uint8_t HalI2cBus0WriteBlock( uint8_t deviceID, uint8_t addr, uint8_t *p_data, uint16_t nbytes );


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void delay_nms(uint16_t count);
void device_slect_9533(BYTE deviceID);//范围是1到10
void device_slect_9679(BYTE deviceID);//范围是1到10
void    SiiPlatformI2cInit( uint8_t index, bool_t isMasterMode, uint8_t slaveAddress, uint_t clockSpeedKhz );

bool_t  SiiPlatformI2cWriteBlock(BYTE cInstance, /*int busIndex,*/ uint8_t devId, uint8_t regAddr, uint8_t *pWrData, uint16_t nBytes );
void    SiiPlatformI2cWriteByte( BYTE cInstance,/* int busIndex, */uint8_t devId, uint8_t regAddr, uint8_t wrData );
bool_t  SiiPlatformI2cReadBlock(BYTE cInstance, /*int busIndex, */uint8_t devId, uint8_t regAddr, uint8_t *pRdData, uint16_t nBytes );
uint8_t SiiPlatformI2cReadByte( BYTE cInstance,  uint8_t devId, uint8_t);

bool_t  SiiPlatformI2cWriteBlock16(BYTE cInstance,/*int busIndex,*/ uint8_t devId, uint16_t regAddr, uint8_t *pWrData, uint16_t nBytes );
void    SiiPlatformI2cWriteByte16(BYTE cInstance, /* int busIndex,*/ uint8_t devId, uint16_t regAddr, uint8_t wrData );
bool_t  SiiPlatformI2cReadBlock16(BYTE cInstance,/* int busIndex,*/ uint8_t devId, uint16_t regAddr, uint8_t *pRdData, uint16_t nBytes );
uint8_t SiiPlatformI2cReadByte16(BYTE cInstance,/* int busIndex,*/ uint8_t devId, uint16_t regAddr );


bool_t  SiiPlatformI2cReadEdidBlock(BYTE cInstance,/*int busIndex, */uint8_t segmentDevId, uint8_t offsetDevId, uint8_t segment, uint8_t offset, uint8_t *pRdData, uint16_t nBytes);

// Callback functions for I2C Slave port read/write
uint8_t SiiPlatformI2cCbSlaveReadByte( BYTE cInstance, /*int busIndex,*/ uint8_t regAddr );
void    SiiPlatformI2cCbSlaveWriteByte(  BYTE cInstance,/*int busIndex,*/ uint8_t regAddr, uint8_t regData );





#endif
















