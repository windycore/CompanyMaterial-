#include "si_i2c.h"
#include "si_eeprom.h"
#include "BSP_I2C.h"



static const BYTE m_acBusID[INSTANCE_9533] = {

#if ((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431))
   eI2CBus9533,
   #if (INSTANCE_9533>1)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>2)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>3)
   eI2CBus9533,
   #endif
#elif (MACHINETYPE==MUHBT44)
   eI2CBus9533,
   #if (INSTANCE_9533>1)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>2)
   eI2CBus9533,
   #endif
   #if (INSTANCE_9533>3)
   eI2CBus9533,
   #endif
#elif((MACHINETYPE==MUH88E_BYE)||(MACHINETYPE==MUH4K862))
	eI2CBus9533_1, // 远端1
	#if (INSTANCE_9533>1)
	eI2CBus9533_1, // 远端1
	#endif
	#if (INSTANCE_9533>2)
	eI2CBus9533_1, // 远端1
	#endif
	#if (INSTANCE_9533>3)
	eI2CBus9533_1, // 远端1
	#endif
	#if (INSTANCE_9533>4)
	eI2CBus9533_2, // 远端1
	#endif
	#if (INSTANCE_9533>5)
	eI2CBus9533_2, // 远端1
	#endif
	#if (INSTANCE_9533>6)
	eI2CBus9533_2, // 远端1
	#endif
	#if (INSTANCE_9533>7)
	eI2CBus9533_2, // 远端1
	#endif
#endif
   

};

#if ((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431))
//下面是9539驱动的延生驱动，控制片选，使能等信号
void device_slect_9533(BYTE deviceID)//范围是1到10
{
    switch(deviceID)
    {
    case 0:
        GPIOB->BRR = GPIO_Pin_12;
	 GPIOB->BSRR = GPIO_Pin_13;
	 GPIOC->BSRR = GPIO_Pin_14;
	 
        GPIOC->BRR = GPIO_Pin_0;
        GPIOC->BRR = GPIO_Pin_11 ;
        break;
    case 1:
        GPIOB->BRR = GPIO_Pin_12;
	 GPIOB->BSRR = GPIO_Pin_13;
	 GPIOC->BSRR = GPIO_Pin_14;
	 
        GPIOC->BSRR = GPIO_Pin_0;
        GPIOC->BRR = GPIO_Pin_11 ;
        break;
    case 2:
        GPIOB->BRR = GPIO_Pin_12;
	 GPIOB->BSRR = GPIO_Pin_13;
	 GPIOC->BSRR = GPIO_Pin_14;
	 
        GPIOC->BRR = GPIO_Pin_0;
        GPIOC->BSRR = GPIO_Pin_11 ;
        break;
    case 3:
        GPIOB->BSRR = GPIO_Pin_12;
	 GPIOB->BRR = GPIO_Pin_13;
	 GPIOC->BSRR = GPIO_Pin_14;
	 
        GPIOC->BRR = GPIO_Pin_0;
        GPIOC->BRR = GPIO_Pin_11 ;
        break;
    case 4:
        GPIOB->BSRR = GPIO_Pin_12;
	 GPIOB->BRR = GPIO_Pin_13;
	 GPIOC->BSRR = GPIO_Pin_14;
	 
        GPIOC->BSRR = GPIO_Pin_0;
        GPIOC->BRR = GPIO_Pin_11 ;
        break;
    case 5:
        GPIOB->BSRR = GPIO_Pin_12;
	 GPIOB->BRR = GPIO_Pin_13;
	 GPIOC->BSRR = GPIO_Pin_14;
	 
        GPIOC->BRR = GPIO_Pin_0;
        GPIOC->BSRR = GPIO_Pin_11 ;
        break;
    
    default:
        break;
    }
}
#else
void device_slect_9533(BYTE deviceID)//范围是1到10
{
    switch(deviceID)
    {
    case 0:
        GPIOA->BRR = GPIO_Pin_5 ;
        GPIOA->BRR = GPIO_Pin_4 ;
        break;
    case 1:
        GPIOA->BSRR = GPIO_Pin_5 ;
        GPIOA->BRR = GPIO_Pin_4 ;
        break;
    case 2:
        GPIOA->BRR = GPIO_Pin_5 ;
        GPIOA->BSRR = GPIO_Pin_4 ;
        break;
    case 3:
        GPIOA->BSRR = GPIO_Pin_5 ;
        GPIOA->BSRR = GPIO_Pin_4 ;
        break;
    case 4:
        GPIOA->BRR = GPIO_Pin_7 ;
        GPIOA->BRR = GPIO_Pin_6 ;
        break;
    case 5:
        GPIOA->BSRR = GPIO_Pin_7 ;
        GPIOA->BRR = GPIO_Pin_6 ;
        break;
    case 6:
        GPIOA->BRR = GPIO_Pin_7 ;
        GPIOA->BSRR = GPIO_Pin_6 ;
        break;
    case 7:
        GPIOA->BSRR = GPIO_Pin_7 ;
        GPIOA->BSRR = GPIO_Pin_6 ;
        break;

    default:
        break;
    }
}

#endif



#if ((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431))
void device_slect_9679(BYTE deviceID)//范围是1到10
{
    switch(deviceID)
    {
    case 0:
        GPIOB->BSRR = GPIO_Pin_12;
	 GPIOB->BSRR = GPIO_Pin_13;
	 GPIOC->BRR = GPIO_Pin_14;
	 
        GPIOC->BRR = GPIO_Pin_0;
        GPIOC->BRR = GPIO_Pin_11 ;
        break;
    case 1:
        GPIOB->BSRR = GPIO_Pin_12;
	 GPIOB->BSRR = GPIO_Pin_13;
	 GPIOC->BRR = GPIO_Pin_14;
	 
        GPIOC->BSRR = GPIO_Pin_0;
        GPIOC->BRR = GPIO_Pin_11 ;
        break;
    case 2:
        GPIOB->BSRR = GPIO_Pin_12;
	 GPIOB->BSRR = GPIO_Pin_13;
	 GPIOC->BRR = GPIO_Pin_14;
	 
        GPIOC->BRR = GPIO_Pin_0;
        GPIOC->BSRR = GPIO_Pin_11 ;
        break;
    case 3:
        GPIOB->BSRR = GPIO_Pin_12;
	 GPIOB->BSRR = GPIO_Pin_13;
	 GPIOC->BRR = GPIO_Pin_14;
	 
        GPIOC->BSRR = GPIO_Pin_0;
        GPIOC->BSRR = GPIO_Pin_11 ;
        break;
    
    default:
        break;
    }
}
#else
void device_slect_9679(BYTE deviceID)//范围是1到10
{
    switch(deviceID)
    {
    case 0:
        GPIOC->BRR = GPIO_Pin_1 ;
        GPIOC->BRR = GPIO_Pin_0 ;
        break;
    case 1:
        GPIOC->BSRR = GPIO_Pin_1 ;
        GPIOC->BRR = GPIO_Pin_0 ;
        break;
    case 2:
        GPIOC->BRR = GPIO_Pin_1 ;
        GPIOC->BSRR = GPIO_Pin_0 ;
        break;
    case 3:
        GPIOC->BSRR = GPIO_Pin_1 ;
        GPIOC->BSRR = GPIO_Pin_0 ;
        break;
    case 4:
        GPIOC->BRR = GPIO_Pin_3 ;
        GPIOC->BRR = GPIO_Pin_2 ;
        break;
    case 5:
        GPIOC->BSRR = GPIO_Pin_3 ;
        GPIOC->BRR = GPIO_Pin_2 ;
        break;
    case 6:
        GPIOC->BRR = GPIO_Pin_3 ;
        GPIOC->BSRR = GPIO_Pin_2 ;
        break;
    case 7:
        GPIOC->BSRR = GPIO_Pin_3 ;
        GPIOC->BSRR = GPIO_Pin_2 ;
        break;

    default:
        break;
    }
}
#endif

//------------------------------------------------------------------------------
// Function:    SiiPlatformI2cWriteBlock
// Description:
// Parameters:
// Returns:
//------------------------------------------------------------------------------
// true : OK
// false: NOACK
bool_t SiiPlatformI2cWriteBlock ( BYTE cInstance,  uint8_t devId, uint8_t regAddr,uint8_t *pWrData, uint16_t nBytes )
{
    //return( HalI2cBus0WriteBlock( m_acI2CBus[cInstance], devId, regAddr, pWrData, nBytes ));
   return( rcSUCCESS == dvDeviceBusWrite( m_acBusID[cInstance],devId, regAddr, nBytes,pWrData, 0 ));

}

//------------------------------------------------------------------------------
// Function:    SiiPlatformI2cWriteByte
// Description: Write a single byte to the I2C controller.
// Parameters:
// Returns:     None
// Note:        The SiiPlatformI2cWriteBlock() function updates lastResultCode and i2CError
//------------------------------------------------------------------------------

void SiiPlatformI2cWriteByte (BYTE cInstance,  uint8_t devId, uint8_t regAddr, uint8_t wrData )
{
    //HalI2cBus0WriteBlock( m_acI2CBus[cInstance],devId, regAddr, &wrData, 1 );
    eRESULT  eResult = rcSUCCESS;
     eResult =  dvDeviceBusWrite( m_acBusID[cInstance],devId, regAddr,  1,&wrData,0 );
     assert_param(eResult == rcSUCCESS);
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformI2cReadBlock
// Description:
// Parameters:
// Returns:
//------------------------------------------------------------------------------
// return 1: sucess
//           0: fail
bool_t SiiPlatformI2cReadBlock (BYTE cInstance,  uint8_t devId, uint8_t regAddr, uint8_t *pRdData, uint16_t nBytes )
{
    //return( HalI2cBus0ReadBlock(m_acI2CBus[cInstance], devId, regAddr, pRdData, nBytes ));
    return( rcSUCCESS == dvDeviceBusRead( m_acBusID[cInstance],devId, regAddr,nBytes, pRdData, 0 ));
}



//------------------------------------------------------------------------------
// Function:    SkI2cRead
// Description: Read a single byte from the I2C controller
// Parameters:
// Returns:
// Note:        The SkI2cWriteRead() function updates lastResultCode and i2CError
//------------------------------------------------------------------------------

uint8_t SiiPlatformI2cReadByte (BYTE cInstance,  uint8_t devId, uint8_t regAddr )
{
    uint8_t returnData = 0;
    eRESULT  eResult = rcSUCCESS;
    //HalI2cBus0ReadBlock(m_acI2CBus[cInstance], devId, regAddr, &returnData, 1 );
    eResult = dvDeviceBusRead( m_acBusID[cInstance],devId, regAddr, 1,&returnData, 0 );
    assert_param(eResult == rcSUCCESS);
    return( returnData );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformI2cWriteBlock
// Description:
// Parameters:
// Returns:
//------------------------------------------------------------------------------
// true : OK
// false: NOACK
bool_t SiiPlatformI2cWriteBlock16 (BYTE cInstance,/* int busIndex, */uint8_t devId, uint16_t regAddr, uint8_t *pWrData, uint16_t nBytes )
{
    
    eRESULT  eResult = rcSUCCESS;
    ASSERT_ALWAYS();
    eResult = dvDeviceBusWrite( m_acBusID[cInstance],devId, regAddr, nBytes,pWrData, FLG_WADDR);

    return (eResult ==rcSUCCESS) ;
   // return( i2c_EE_write_Nbytes( devId, regAddr, pWrData, nBytes ));
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformI2cWriteByte
// Description: Write a single byte to the I2C controller.
// Parameters:
// Returns:		None
// Note:		The SiiPlatformI2cWriteBlock() function updates lastResultCode and i2CError
//------------------------------------------------------------------------------

void SiiPlatformI2cWriteByte16 (BYTE cInstance,/* int busIndex,*/ uint8_t devId, uint16_t regAddr, uint8_t wrData )
{
   // i2c_EE_write_Nbytes( devId, regAddr, &wrData, 1 );
    eRESULT  eResult = rcSUCCESS;

       ASSERT_ALWAYS();
       eResult = dvDeviceBusWrite( m_acBusID[cInstance],devId, regAddr, 1,&wrData, FLG_WADDR);

    //return FALSE;
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformI2cReadBlock
// Description:
// Parameters:
// Returns:
//------------------------------------------------------------------------------
// true : OK
// false: NOACK
bool_t SiiPlatformI2cReadBlock16 ( BYTE cInstance,/*int busIndex,*/ uint8_t devId, uint16_t regAddr, uint8_t *pRdData, uint16_t nBytes )
{
    //return( i2c_EE_read_Nbytes( devId, regAddr, pRdData, nBytes ));
   //     ASSERT_ALWAYS();
   // return FALSE;
       eRESULT  eResult = rcSUCCESS;
    //HalI2cBus0ReadBlock(m_acI2CBus[cInstance], devId, regAddr, &returnData, 1 );
    eResult = dvDeviceBusRead( m_acBusID[cInstance],devId, regAddr, nBytes,pRdData, FLG_WADDR );
    assert_param(eResult == rcSUCCESS);
    return( eResult == rcSUCCESS );

}

//------------------------------------------------------------------------------
// Function:    SkI2cRead
// Description: Read a single byte from the I2C controller
// Parameters:
// Returns:
// Note:		The SkI2cWriteRead() function updates lastResultCode and i2CError
//------------------------------------------------------------------------------

uint8_t SiiPlatformI2cReadByte16 (BYTE cInstance,/* int busIndex, */uint8_t devId, uint16_t regAddr )
{
    //uint8_t returnData = 0;
    //i2c_EE_read_Nbytes( devId, regAddr, &returnData, 1 );
    //return( returnData );
     uint8_t returnData = 0;
     ASSERT_ALWAYS();
    //return 0;
       // return FALSE;
       eRESULT  eResult = rcSUCCESS;
    //HalI2cBus0ReadBlock(m_acI2CBus[cInstance], devId, regAddr, &returnData, 1 );
    eResult = dvDeviceBusRead( m_acBusID[cInstance],devId, regAddr, 1,&returnData, FLG_WADDR );
    assert_param(eResult == rcSUCCESS);
    return returnData;

}


//------------------------------------------------------------------------------
// Function:    SiiPlatformI2cInit
// Description: Initialization of I2C hardware
// Parameters:  index        - 0 for I2C0, 1 for I2C1
//              isMasterMode - true for I2C master mode, false for I2C slave mode
//              slaveAddress - I2C slave address (applicable to slave mode only)
//              clockSpeedKHz - I2C clock speed (applicable to master mode only)
//------------------------------------------------------------------------------
void SiiPlatformI2cInit (uint8_t index, bool_t isMasterMode, uint8_t slaveAddress, uint_t clockSpeedKhz )
{
   /* uint_t ulSCLFreq, ulTPR;

    // Enable the ARM peripherals needed.
    ROM_SysCtlPeripheralEnable( l_i2cPeripheralGpio[ index] );
    ROM_SysCtlPeripheralEnable( l_i2cPeripheralI2c[ index] );
    ROM_SysCtlPeripheralReset( l_i2cPeripheralI2c[ index] );

    // Put the pins into HW control mode for the I2C module.
    ROM_GPIODirModeSet( l_i2cPortBase[ index], l_i2cPins[index], GPIO_DIR_MODE_HW);
    ROM_GPIOPinTypeI2C( l_i2cPortBase[ index], l_i2cPins[index] );

    if (isMasterMode)
    {
        ROM_I2CMasterEnable( l_i2cBase[ index] );

        // Clock speed needs to be set only on the master
        ulSCLFreq = clockSpeedKhz * 1000;
        ulTPR = (ROM_SysCtlClockGet() / (2 * 10 * ulSCLFreq)) - 1;
        HWREG( l_i2cBase[ index] + I2C_O_MTPR) = ulTPR;

    }
    else // Slave mode
    {
        unsigned long slaveBase = l_i2cBase[ index] + I2C_SLAVE_BASE_OFFSET;

        ROM_I2CSlaveEnable( slaveBase );
        ROM_I2CSlaveInit( slaveBase, slaveAddress );

        // Using interrupts only on the slave
        ROM_IntEnable( l_i2cInt[ index] );
        ROM_I2CSlaveIntClear( slaveBase );
        ROM_I2CSlaveIntEnable( slaveBase );
    }

    //I2C1 workaround, API could not set this register; so we set directly
    if ( index == 0 )
    {
        //GPIO_PORTB_PCTL_R = 0x11111111;
    }
    else
    {
        GPIO_PORTA_PCTL_R = 0x11111111;
    }		  */

}


//------------------------------------------------------------------------------
// Function:    SiiPlatformI2cReadEdidBlock
// Description: Read an EDID block over DDC link
// Parameters:
// Returns:
//------------------------------------------------------------------------------

bool_t SiiPlatformI2cReadEdidBlock(BYTE cInstance,/*int busIndex,*/ uint8_t segmentDevId, uint8_t offsetDevId, uint8_t segment, uint8_t offset, uint8_t *pRdData, uint16_t nBytes)
{
    bool_t isSuccess = false;

    /*platform.i2c16bitAddr = false;
    platform.i2cNoInterimStops = true;

    // write EDID segment index
    isSuccess = I2cWriteBlock(busIndex, segmentDevId, segment, 0, 0);
    // write EDID offset and read nBytes of data
    isSuccess = isSuccess && I2cReadBlock(busIndex, offsetDevId, offset, pRdData, nBytes);

    platform.i2cNoInterimStops = false;	*/

    return( isSuccess );
}
