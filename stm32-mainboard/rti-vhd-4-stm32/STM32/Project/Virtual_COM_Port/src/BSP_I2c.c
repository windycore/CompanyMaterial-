#include "main.h"
#include "stm32f10x.h"
#include "BSP_I2C.h"
#include "Mcu.h"
#include "uart.h"
#include "si_debug.h"

#define I2C_WRITE 0x00
#define I2C_READ   0x01

typedef struct
{
    GPIO_TypeDef *SDA_PORT;
    uint16_t       SDA_PIN;
    GPIO_TypeDef *SCL_PORT;
    uint16_t	     SCL_PIN;
}I2CBUS;

static const char *m_acBusName[] =
{
#if (MACHINETYPE==MUH44E_BYE)
    "AD4604",
    "9533_1",
    "9533_2",
    "9533_3",
    "9533_4",
    "9679",
    "9539"
#elif(MACHINETYPE==MUH88E_BYE)
    "AD4604",
    "9533_1",
    "9533_2",
    "9533_3",
    //"9533_4",
    "9533_5",
    "9533_6",
    "9679_1",
    "9679_2",
    "9539"
#elif((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
    "M21452",
    "9396_IN",
    "9533_OUT",
    "9539"
#endif
};

static I2CBUS m_acI2CBus[eI2CBusINVALID] =
{
   //eI2CBusM21452_EEPROM,
    {
        GPIOB,
        GPIO_Pin_15, //SDA
        GPIOB,
        GPIO_Pin_14,  // SCL
    },
    //eI2CBus9396_IN, 		 // 输入端的4个sil9396
    {
        GPIOB,
        GPIO_Pin_13, // SDA
        GPIOB,
        GPIO_Pin_12, // SCL
    },
    //eI2CBus9396_OUT,  	//  输出端的4个sil9396
    {
        GPIOB,
        GPIO_Pin_9, // SDA
        GPIOB,
        GPIO_Pin_8, // SCL
    },
	
    //eI2CBus9539,  		// 1个9539: GPIO 口扩展   
    {
        GPIOB,
        GPIO_Pin_0, // SDA
        GPIOC,
        GPIO_Pin_5, // SCL
    },

    //eI2CBusreadEDID
    {
        GPIOA,
        GPIO_Pin_3, // SDA
        GPIOA,
        GPIO_Pin_2, // SCL
    },
};

static  BYTE m_cI2C_Lock[eI2CBusINVALID];

#define halSetSDAPin(cBusId)  (m_acI2CBus[cBusId].SDA_PORT)->BSRR =m_acI2CBus[cBusId].SDA_PIN
#define halSetSCLPin(cBusId)  (m_acI2CBus[cBusId].SCL_PORT)->BSRR =m_acI2CBus[cBusId].SCL_PIN
#define halClearSDAPin(cBusId)  (m_acI2CBus[cBusId].SDA_PORT)->BRR =m_acI2CBus[cBusId].SDA_PIN
#define halClearSCLPin(cBusId)  (m_acI2CBus[cBusId].SCL_PORT)->BRR =m_acI2CBus[cBusId].SCL_PIN
#define halGetSDAPin(cBusId)    (m_acI2CBus[cBusId].SDA_PORT->IDR &m_acI2CBus[cBusId].SDA_PIN)
#define halGetSCLPin(cBusId)    (m_acI2CBus[cBusId].SCL_PORT->IDR &m_acI2CBus[cBusId].SCL_PIN)

void dvI2CBusInit(void)
{
    BYTE cBusId = 0;
    for (cBusId = 0; cBusId < eI2CBusINVALID; cBusId++)
    {
        GPIO_InitTypeDef  GPIO_InitStructure;
        /* Configure I2C1 pins: SCL and SDA */
        GPIO_InitStructure.GPIO_Pin =  m_acI2CBus[cBusId].SDA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_Init(m_acI2CBus[cBusId].SDA_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin =   m_acI2CBus[cBusId].SCL_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_Init(m_acI2CBus[cBusId].SCL_PORT, &GPIO_InitStructure);
	 halSetSDAPin(cBusId);
	 halSetSCLPin(cBusId);
	 m_cI2C_Lock[cBusId]= 0;
    }
}

static void I2C_delay(void) {
    u16 i = 80; //这里可以优化速度	，经测试最低到5还能写入
    while (i)
    {
        i--;
    }
}

//-----------------------------------------------------------------------------------
static void _I2CSCLHigh(BYTE cBusId) {       // set SCL high, and wait for it to go high
    register int err;

    halSetSCLPin(cBusId);
    #if 1
    while (!halGetSCLPin(cBusId))
    {
        err++;
        if (!err)
        {
            //_i2c_error &= 0x02;     // SCL stuck, something's holding it down
            return;
        }
    }
    #endif
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
// return 0 : no error,
//           1:  error
static BYTE I2CSendByte(BYTE cBusId, BYTE bt) {
    register unsigned char i;
    unsigned char error = 0; // if error =1 then there was an error in getting ACK

    for (i = 0; i < 8; i++) {
        if (bt & 0x80)
            halSetSDAPin(cBusId);      // send each bit, MSB first
        else
            halClearSDAPin(cBusId);
        I2C_delay(); // kyq
        _I2CSCLHigh(cBusId);
        I2C_delay(); // kyq
        halClearSCLPin(cBusId);
        bt = bt << 1;
    }
    I2C_delay(); // kyq
    halSetSDAPin(cBusId);                       // listen for ACK
    I2C_delay(); // kyq
    _I2CSCLHigh(cBusId);
    I2C_delay(); // kyq
    if (halGetSDAPin(cBusId))
        error = 1;
    halClearSCLPin(cBusId);
    I2C_delay(); // kyq
    if (error)
    {
        //DEBUG_PRINT(MSG_ERR, "I2C Send Data Error:%2x,%2x\r\n", cBusId, bt);
    }

    return error; // return 0 if no error, else return 1
}

// return 0 : no error,
//           1:  error
//------------------------------------------------------------------------------
static BYTE I2CSendAddr(BYTE cBusId, BYTE addr, BYTE rd) {
    unsigned char error = 0;

    
    halSetSDAPin(cBusId);               // generate start
    I2C_delay(); // kyq
    halSetSCLPin(cBusId);
    I2C_delay(); // kyq
    halClearSDAPin(cBusId);            
    I2C_delay(); // kyq
    halClearSCLPin(cBusId);		// generate start
    I2C_delay(); // kyq
    error = I2CSendByte(cBusId, addr + rd);  // send address byte
    if (error)
    {
       //I2C_delay(); // kyq;
       // DEBUG_PRINT(MSG_ERR, "***I2C WriteAddress Error: Bus[%2x],Addr[%2x]\r\n", cBusId, addr + rd);
    }
    I2C_delay(); // kyq

    return error;
}


//----------------------------------------------------------------------------
static BYTE _I2CGetByte(BYTE cBusId, BYTE lastone) { // lastone == 1 for last byte
    register unsigned char i, res;

    res = 0;
    for (i = 0; i < 8; i++) {          // each bit at a time, MSB first
        _I2CSCLHigh(cBusId);
        I2C_delay(); // kyq
        //res *= 2;
        res<<=1;
        if (halGetSDAPin(cBusId))
            res++;
        halClearSCLPin(cBusId);
        I2C_delay(); // kyq
    }
    if (lastone)
        halSetSDAPin(cBusId);           // send ACK according to 'lastone'
    else
        halClearSDAPin(cBusId);
    I2C_delay(); // kyq
    _I2CSCLHigh(cBusId);
    I2C_delay(); // kyq
    halClearSCLPin(cBusId);
    I2C_delay(); // kyq
    halSetSDAPin(cBusId);
    I2C_delay(); // kyq
    return  res;
}


static void I2CSendStop(BYTE cBusId) {
    halClearSDAPin(cBusId);
    I2C_delay(); // kyq
    _I2CSCLHigh(cBusId);
    I2C_delay(); // kyq
    halSetSDAPin(cBusId);
    I2C_delay(); // kyq
}

//return 0: ok
//         else: NG
static BYTE GetI2CState(BYTE cBusId) {
    BYTE i, j;
    halSetSCLPin(cBusId);
    for (i = 0; i < 10; i++) {
        if (halGetSCLPin(cBusId))
        {
            halSetSDAPin(cBusId);
            for (j = 0; j < 10; j++) {
                if (halGetSDAPin(cBusId))
                    return 0;   // OK
            }
            return 1;      // SCL OK, SDA someone pull down
        }
    }
    return 2;          // error, someone holds the SCL bus
}


#if 0
void I2C_delay(void)
{
    u8 i = 70; //这里可以优化速度	，经测试最低到5还能写入 // 100
    while (i)
    {
        i--;
    }
}

//-----------------------------------------------------------------------------------
static void _I2CSCLHigh(BYTE cBusId)
{       // set SCL high, and wait for it to go high
    register int err;
    halSetSCLPin(cBusId);
    while (!halGetSCLPin(cBusId))
    {
        err++;
        if (!err)
        {
            //_i2c_error &= 0x02;     // SCL stuck, something's holding it down
            return;
        }
    }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
// return 0 : no error,
//           1:  error
static BYTE I2CSendByte(BYTE cBusId, BYTE bt)
{
    register unsigned char i;
    unsigned char error = 0; // if error =1 then there was an error in getting ACK
    for (i = 0; i < 8; i++)
    {
        if (bt & 0x80)
            halSetSDAPin(cBusId);      // send each bit, MSB first
        else
            halClearSDAPin(cBusId);
        I2C_delay(); // kyq
        _I2CSCLHigh(cBusId);
        I2C_delay(); // kyq
        halClearSCLPin(cBusId);
        bt = bt << 1;
        I2C_delay(); // kyq
    }
    halSetSDAPin(cBusId);                       // listen for ACK
    _I2CSCLHigh(cBusId);
    I2C_delay(); // kyq
    I2C_delay(); // kyq
    if (halGetSDAPin(cBusId))
        error = 1;
    halClearSCLPin(cBusId);
    I2C_delay(); // kyq
//  if (error)
//  {
//      //DEBUG_PRINT(MSG_ERR, "I2C Send Data Error:%2x,%2x\r\n", cBusId, bt);
//      DEBUG_PRINT(MSG_ERR, "%s I2C Send Data Error:%2x\r\n", m_acBusName[cBusId], bt);
//  }
    return error; // return 0 if no error, else return 1
}

// return 0 : no error,
//           1:  error
//------------------------------------------------------------------------------
static BYTE I2CSendAddr(BYTE cBusId, BYTE addr, BYTE rd)
{
    unsigned char error = 0;
    halSetSCLPin(cBusId);
    halSetSCLPin(cBusId);                   // Restart was too short
    I2C_delay(); // kyq
    I2C_delay(); // kyq
    halClearSDAPin(cBusId);               // generate start
    halClearSDAPin(cBusId);               // Restart was too short
    I2C_delay(); // kyq
    I2C_delay(); // kyq
    halClearSCLPin(cBusId);
    error = I2CSendByte(cBusId, addr + rd);  // send address byte
    /*if (error)
    {
        DEBUG_PRINT(MSG_ERR, "%s I2C WriteAddress Error: Addr[%2x]\r\n",  m_acBusName[cBusId], addr + rd);
    }
    */
    return error;
}

//----------------------------------------------------------------------------
static BYTE _I2CGetByte(BYTE cBusId, BYTE lastone)
{ // lastone == 1 for last byte
    register unsigned char i, res;
    res = 0;
    for (i = 0; i < 8; i++)          // each bit at a time, MSB first
    {
        _I2CSCLHigh(cBusId);
        I2C_delay(); // kyq
        res *= 2;
        if (halGetSDAPin(cBusId))
            res++;
        halClearSCLPin(cBusId);
        I2C_delay(); // kyq
    }
    if (lastone)
        halSetSDAPin(cBusId);           // send ACK according to 'lastone'
    else
        halClearSDAPin(cBusId);
    I2C_delay(); // kyq
    _I2CSCLHigh(cBusId);
    I2C_delay(); // kyq
    halClearSCLPin(cBusId);
    I2C_delay(); // kyq
    halSetSDAPin(cBusId);
    return  res;
}

static void I2CSendStop(BYTE cBusId)
{
    halClearSDAPin(cBusId);
    _I2CSCLHigh(cBusId);
    I2C_delay(); // kyq
    halSetSDAPin(cBusId);
    I2C_delay(); // kyq
}

//return 0: ok
//         else: NG
static BYTE GetI2CState(BYTE cBusId)
{
    BYTE i, j;
    halSetSCLPin(cBusId);
    for (i = 0; i < 10; i++)
    {
        if (halGetSCLPin(cBusId))
        {
            halSetSDAPin(cBusId);
            for (j = 0; j < 10; j++)
            {
                if (halGetSDAPin(cBusId))
                    return 0;   // OK
            }
            return 1;      // SCL OK, SDA someone pull down
        }
    }
    return 2;          // error, someone holds the SCL bus
}
#endif

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
                        WORD  wFlags)
{
    //DEVICE_BUS          sBus;
    //eRESULT             eResult = rcERROR;
    DWORD cTryCnt = 0;
    PBYTE pacRdBuffer = pacBuffer;
    DWORD i;
    BYTE bState;
    // validate arguments in debug mode only
    ASSERT(cBusId < eI2CBusINVALID);
    ASSERT(wCount > 0);
    ASSERT(NULL != pacBuffer);
    if(m_cI2C_Lock[cBusId])
    	{
          /*
        DEBUG_PRINT(MSG_ERR, "%s I2C BUS Rd Locked Error: Addr=[%2x],Reg=[%2x]\r\n", 
                     m_acBusName[cBusId],
                     cDeviceAddress,
                     wRegisterAddress);
         */
	  return rcERROR;
    	}
     m_cI2C_Lock[cBusId] = TRUE;
    do
    {
        pacRdBuffer = pacBuffer;
        bState = 0;
        if (!(wFlags & FLG_CONTD))
        {
            bState = GetI2CState(cBusId);
//          if (bState)
//          {
//              return rcERROR;
//          }
            bState |= I2CSendAddr(cBusId, cDeviceAddress, I2C_WRITE);  // START ,  Device Address Write
            if (wFlags & FLG_WADDR)
            {
                bState |= I2CSendByte(cBusId, (wRegisterAddress >> 8) & 0xff);
            }
            bState |= I2CSendByte(cBusId, wRegisterAddress & 0xff);
            if (wFlags & FLG_9679I2C)
            {
                uint16_t i = 0;
                for (i = 0; i < 3000; i++)
                {i++;}
            }
            bState |= I2CSendAddr(cBusId, cDeviceAddress, I2C_READ);         // START ,  Device Address Read
        }
        for (i = 0; i < wCount; i++)
        {
            *pacRdBuffer++ = _I2CGetByte(cBusId, (i == wCount - 1));
        }
        if (!(wFlags & FLG_NOSTOP))
        {
            I2CSendStop(cBusId);
        }
        cTryCnt++;
    } while (cTryCnt < 3 && bState);
    if (bState)
    {
        DEBUG_PRINT2(MSG_ERR, "%s I2C dvDeviceBusRead Error: Addr=[%2x],Reg=[%2x]\r\n", 
                     m_acBusName[cBusId],
                     cDeviceAddress,
                     wRegisterAddress);
    }
    m_cI2C_Lock[cBusId] = FALSE;
    return bState ? rcERROR : rcSUCCESS;
}
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
                         WORD   wFlags)
{
    //DEVICE_BUS          sBus;
    //eRESULT             eResult = rcSUCCESS;
    //BYTE cReadVal = 0;
    // validate arguments in debug mode only
    ASSERT(cBusId < eI2CBusINVALID);
//    ASSERT(wCount > 0);// cancel it by Huang 2016-Aug-08, 15:57:41
    ASSERT(NULL != pacBuffer);
    DWORD i;
    BYTE bState = 0;
    if(m_cI2C_Lock[cBusId])
    	{
          /*
        DEBUG_PRINT(MSG_ERR, "%s I2C BUS Wrt Locked Error: Addr=[%2x],Reg=[%2x]\r\n", 
                     m_acBusName[cBusId],
                     cDeviceAddress,
                     wRegisterAddress);
          */
	return rcERROR;
    	}
     m_cI2C_Lock[cBusId] = TRUE;
    //BSP_OS_SemWait(&g_sgI2C[cBusId], 0);
    if (!(wFlags & FLG_CONTD))
    {
        bState = GetI2CState(cBusId);
        bState |= I2CSendAddr(cBusId, cDeviceAddress, I2C_WRITE);
        //if (bState)
        //{
        //    I2CSendStop(cBusId);
        //    return rcERROR;
        //}
        if (wFlags & FLG_WADDR)
        {
            bState |= I2CSendByte(cBusId, (wRegisterAddress >> 8) & 0xFF);
//          if (bState)
//          {
//              I2CSendStop(cBusId);
//              return rcERROR;
//          }
        }
        bState |= I2CSendByte(cBusId, wRegisterAddress & 0xFF);
//      if (bState)
//      {
//          I2CSendStop(cBusId);
//          return rcERROR;
//      }
//
    }
    for (i = 0; i < wCount; i++)
    {
        if (I2CSendByte(cBusId, *pacBuffer++))
        {
            bState |=1;
            break;
        }
    }
    if (!(wFlags & FLG_NOSTOP))
        I2CSendStop(cBusId);
    if (bState)
    {
        DEBUG_PRINT2(MSG_ERR, "%s I2C dvDeviceBusWrite Error: Addr=[%2x],Reg=[%2x]\r\n",
                    m_acBusName[cBusId],
                    cDeviceAddress,
                    wRegisterAddress);
    }
    m_cI2C_Lock[cBusId] = FALSE;
    return bState ? rcERROR : rcSUCCESS;
    
}
