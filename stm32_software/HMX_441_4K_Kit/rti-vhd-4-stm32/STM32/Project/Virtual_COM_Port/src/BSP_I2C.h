//Power by kyq : 20150729 10:00:41]//
//FILE: BSP_I2C.h
#ifndef _IIC_BSP_I2C_H_
#define _IIC_BSP_I2C_H_
#include "si_datatypes.h"
typedef enum
{
#if((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431))
   eI2CBusAD4604_EEPROM = 0,
   eI2CBus9533,   // 4 个9533
   eI2CBus9679,  // 4 个9679
   eI2CBus9539,  // 4个9539: GPIO 口扩展
#elif(MACHINETYPE==MUHBT44)
   eI2CBusAD4604_EEPROM = 0,
   eI2CBus9533,   // 4 个9533
   eI2CBus9679,  // 4 个9679
   eI2CBus9539,  // 4个9539: GPIO 口扩展
#elif((MACHINETYPE==MUH88E_BYE)||(MACHINETYPE==MUH4K862))
   eI2CBusAD4604_EEPROM = 0,
   eI2CBus9533_1,   // 远端0
   eI2CBus9533_2,   //  本地0
   eI2CBus9533_3,  //  远端 1
   //eI2CBus9533_4,  //  本地1
   eI2CBus9533_5,  //  远端 2
   eI2CBus9533_6,  //  远端 3
   eI2CBus9679_1,  // 4 个9679
      eI2CBus9679_2,  // 4 个9679
   eI2CBus9539,  // 4个9539: GPIO 口扩展
   
#endif
eI2CBusINVALID,
} eI2CBUS_ID; 
void dvI2CBusInit(void) ;
// ---------------------------------------------------------------------------
// Reads or writes data from a device connected to an external 2-Wire device
// bus.
//
// Params:
//  cBusId           : Bus ID to execute the read / write on
//  cDeviceAddress   : Device address to read / write
//  wRegisterAddress : Internal device register address to read / write.
//                     Optional parameter, unused if the transaction is not an
//                     addressed transaction.  Only the lower 8 bits are used
//                     for addressed transactions unless word address is
//                     specified.
//  wCount           : Number of bytes to read / write from / to the external
//                     device
//  pacBuffer        : Pointer to an array of bytes for storing read data or
//                     data to be written
//  wFlags           : Flags for the 2 wire transaction (see Definitions)
//
// Return:
//  rcSUCCESS if operation succeeds, rcERROR otherwise
// ---------------------------------------------------------------------------
eRESULT dvDeviceBusRead(BYTE  cBusId,
                        BYTE  cDeviceAddress,
                        DWORD  wRegisterAddress,
                        DWORD  wCount,
                        PBYTE pacBuffer,                                                
                        WORD  wFlags);
// ---------------------------------------------------------------------------
// Reads or writes data from a device connected to an external 2-Wire device
// bus.
//
// Params:
//  cBusId           : Bus ID to execute the read / write on
//  cDeviceAddress   : Device address to read / write
//  wRegisterAddress : Internal device register address to read / write.
//                     Optional parameter, unused if the transaction is not an
//                     addressed transaction.  Only the lower 8 bits are used
//                     for addressed transactions unless word address is
//                     specified.
//  wCount           : Number of bytes to read / write from / to the external
//                     device
//  pacBuffer        : Pointer to an array of bytes for storing read data or
//                     data to be written
//  wFlags           : Flags for the 2 wire transaction (see Definitions)
//
// Return:
//  rcSUCCESS if operation succeeds, rcERROR otherwise
// ---------------------------------------------------------------------------
eRESULT dvDeviceBusWrite(BYTE   cBusId,
                         BYTE   cDeviceAddress,
                         DWORD   wRegisterAddress,
                         DWORD   wCount, 
                         PCBYTE pacBuffer,                                                 
                         WORD   wFlags);
//bool_t HalI2cBus0ReadBlock(BYTE cBusId, uint8_t deviceID, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
//bool_t HalI2cBus0WriteBlock(BYTE cBusId, uint8_t deviceID, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
#endif
