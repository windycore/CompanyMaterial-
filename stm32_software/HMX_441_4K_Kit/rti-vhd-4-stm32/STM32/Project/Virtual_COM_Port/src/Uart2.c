
#include "uart2.h"
#include "stm32f10x.h"




//*****************************************************************************
//
// uartstdio.c - Utility driver to provide simple UART console functions.
//
// Copyright (c) 2007-2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 5570 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "si_platform.h"
//#include "si_arm.h"
//#include "si_uartstdio.h"
//#include "si_usb_serial.h"
#include "si_datatypes.h"
#include "si_debugger_hdmigear.h"
//*****************************************************************************
//
// USB UART TX buffer empty status will be ignored, if the following macro
// is defined to DISABLE. This resolves a stall situation when USB cable
// is connected but the corresponding virtual COM port isn't open.
// If UART USB sync is ENABLEd, the UART Tx will proceed only if both
// Tx buffers (serial & USB) have free space.
//
//*****************************************************************************

#define SI_USB_UART_SYNC  ENABLE

//*****************************************************************************
//
//! \addtogroup uartstdio_api
//! @{
//
//*****************************************************************************

//static tBoolean crlfFilter  = true;

//*****************************************************************************
//
// If buffered mode is defined, set aside RX and TX buffers and read/write
// pointers to control them.
//
//*****************************************************************************
#ifdef UART_BUFFERED

//*****************************************************************************
//
// This global controls whether or not we are echoing characters back to the
// transmitter.  By default, echo is enabled but if using this module as a
// convenient method of implementing a buffered serial interface over which
// you will be running an application protocol, you are likely to want to
// disable echo by calling UARTEchoSet(false).
//
//*****************************************************************************

//*****************************************************************************
//
// Output ring buffer.  Buffer is full if g_ulUARTTxReadIndex is one ahead of
// g_ulUARTTxWriteIndex.  Buffer is empty if the two indices are the same.
//
//*****************************************************************************
//static unsigned char g_pcUARTTxBuffer[UART_TX_BUFFER_SIZE];
static volatile unsigned long g_ulUARTTxWriteIndex = 0;
static volatile unsigned long g_ulUARTTxReadIndex = 0;

//*****************************************************************************
//
// Input ring buffer.  Buffer is full if g_ulUARTTxReadIndex is one ahead of
// g_ulUARTTxWriteIndex.  Buffer is empty if the two indices are the same.
//
//*****************************************************************************
static unsigned char g_pcUARTRxBuffer[UART_RX_BUFFER_SIZE];

//USART2_IRQHandler() ++  写入BUFFER
static volatile unsigned long g_ulUARTRxWriteIndex = 0;

//SiiPlatformUartGetc() j++ 读出BUFFER
static volatile unsigned long g_ulUARTRxReadIndex = 0;


//*****************************************************************************
//
// Macros to determine number of free and used bytes in the transmit buffer.
//
//*****************************************************************************
#define TX_BUFFER_USED          (GetBufferCount(&g_ulUARTTxReadIndex,  \
                                                &g_ulUARTTxWriteIndex, \
                                                UART_TX_BUFFER_SIZE))
#define TX_BUFFER_FREE          (UART_TX_BUFFER_SIZE - TX_BUFFER_USED)
#define TX_BUFFER_EMPTY         (IsBufferEmpty(&g_ulUARTTxReadIndex,   \
                                               &g_ulUARTTxWriteIndex))
#define TX_BUFFER_FULL          (IsBufferFull(&g_ulUARTTxReadIndex,  \
                                              &g_ulUARTTxWriteIndex, \
                                              UART_TX_BUFFER_SIZE))
#define ADVANCE_TX_BUFFER_INDEX(Index) \
                                (Index) = ((Index) + 1) % UART_TX_BUFFER_SIZE

//*****************************************************************************
//
// Macros to determine number of free and used bytes in the receive buffer.
//
//*****************************************************************************
#define RX_BUFFER_USED          (GetBufferCount(&g_ulUARTRxReadIndex,  \
                                                &g_ulUARTRxWriteIndex, \
                                                UART_RX_BUFFER_SIZE))
#define RX_BUFFER_FREE          (UART_RX_BUFFER_SIZE - RX_BUFFER_USED)
#define RX_BUFFER_EMPTY         (IsBufferEmpty(&g_ulUARTRxReadIndex,   \
                                               &g_ulUARTRxWriteIndex))
#define RX_BUFFER_FULL          (IsBufferFull(&g_ulUARTRxReadIndex,  \
                                              &g_ulUARTRxWriteIndex, \
                                              UART_RX_BUFFER_SIZE))
#define ADVANCE_RX_BUFFER_INDEX(Index) \
                                (Index) = ((Index) + 1) % UART_RX_BUFFER_SIZE
#endif

//*****************************************************************************
//
// The base address of the chosen UART.
//
//*****************************************************************************
//static unsigned long g_ulBase = 0;

//*****************************************************************************
//
// A mapping from an integer between 0 and 15 to its ASCII character
// equivalent.
//
//*****************************************************************************
//static const char * const g_pcHex = "0123456789abcdef";



//*****************************************************************************
//
// External interface. The following external functions must be implemented
// in the module consuming incoming UART data.
//
//*****************************************************************************

//------------------------------------------------------------------------------
// Function:    SkSerialRxHandler
// Description: Handle incoming chars from UART
//------------------------------------------------------------------------------
//extern bool_t SkSerialRxHandler(uint8_t achar);
//------------------------------------------------------------------------------
// Function:    SkSerialRxHandler
// Description: Check if receiving is locked until after finishing of command processing
//------------------------------------------------------------------------------
//extern bool_t SkSerialConsumedChar(void);


//*****************************************************************************
//
//! Determines whether the ring buffer whose pointers and size are provided
//! is full or not.
//!
//! \param pulRead points to the read index for the buffer.
//! \param pulWrite points to the write index for the buffer.
//! \param ulSize is the size of the buffer in bytes.
//!
//! This function is used to determine whether or not a given ring buffer is
//! full.  The structure of the code is specifically to ensure that we do not
//! see warnings from the compiler related to the order of volatile accesses
//! being undefined.
//!
//! \return Returns \b true if the buffer is full or \b false otherwise.
//
//*****************************************************************************
#ifdef UART_BUFFERED
static tBoolean
IsBufferFull(volatile unsigned long *pulRead,
             volatile unsigned long *pulWrite, unsigned long ulSize)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    ulWrite = *pulWrite;
    ulRead = *pulRead;

    return((((ulWrite + 1) % ulSize) == ulRead) ? true : false);
}
#endif

//*****************************************************************************
//
//! Determines whether the ring buffer whose pointers and size are provided
//! is empty or not.
//!
//! \param pulRead points to the read index for the buffer.
//! \param pulWrite points to the write index for the buffer.
//!
//! This function is used to determine whether or not a given ring buffer is
//! empty.  The structure of the code is specifically to ensure that we do not
//! see warnings from the compiler related to the order of volatile accesses
//! being undefined.
//!
//! \return Returns \b true if the buffer is empty or \b false otherwise.
//
//*****************************************************************************
#ifdef UART_BUFFERED
static tBoolean
IsBufferEmpty(volatile unsigned long *pulRead,
              volatile unsigned long *pulWrite)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    ulWrite = *pulWrite;
    ulRead = *pulRead;

    return((ulWrite  == ulRead) ? true : false);
}
#endif

//*****************************************************************************
//
//! Determines the number of bytes of data contained in a ring buffer.
//!
//! \param pulRead points to the read index for the buffer.
//! \param pulWrite points to the write index for the buffer.
//! \param ulSize is the size of the buffer in bytes.
//!
//! This function is used to determine how many bytes of data a given ring
//! buffer currently contains.  The structure of the code is specifically to
//! ensure that we do not see warnings from the compiler related to the order
//! of volatile accesses being undefined.
//!
//! \return Returns the number of bytes of data currently in the buffer.
//
//*****************************************************************************
#ifdef UART_BUFFERED
static unsigned long
GetBufferCount(volatile unsigned long *pulRead,
               volatile unsigned long *pulWrite, unsigned long ulSize)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    ulWrite = *pulWrite;
    ulRead = *pulRead;

    return((ulWrite >= ulRead) ? (ulWrite - ulRead) :
                                 (ulSize - (ulRead - ulWrite)));
}
#endif

//*****************************************************************************
//
// Take as many bytes from the transmit buffer as we have space for and move
// them into the UART transmit FIFO.
//
//*****************************************************************************
#if 0  // def UART_BUFFERED
//发送部分数据
static void
UARTPrimeTransmit(unsigned long ulBase)
{
#if 0
    //
    // Do we have any data to transmit?
    //
    if (!TX_BUFFER_EMPTY)
    {
        //
        // Disable the UART interrupt. If we don't do this there is a race
        // condition which can cause the read index to be corrupted.
        //
        MAP_IntDisable(g_ulUARTInt[g_ulPortNum]);

        //
        // Yes - take some characters out of the transmit buffer and feed
        // them to the UART transmit FIFO.
        //
        while(MAP_UARTSpaceAvail(ulBase) && !TX_BUFFER_EMPTY)
        {
            MAP_UARTCharPutNonBlocking(ulBase,
                                       g_pcUARTTxBuffer[g_ulUARTTxReadIndex]);
            ADVANCE_TX_BUFFER_INDEX(g_ulUARTTxReadIndex);
        }

        //
        // Reenable the UART interrupt.
        //
        MAP_IntEnable(g_ulUARTInt[g_ulPortNum]);

    }
#else
	unsigned char ucChar;
	bool_t isTxSpaceAvailable;
	bool_t isUseUsb = FALSE;//SiiPlatformUsbIsComOpen();

    //
    // Do we have any data to transmit?
    //
    if(!TX_BUFFER_EMPTY)
    {

        //
        // Disable the UART interrupt. If we don't do this there is a race
        // condition which can cause the read index to be corrupted.
        //
        //_MAP_IntDisable(g_ulUARTInt[g_ulPortNum]);
        //_if(isUseUsb)
        //_{
        //_MAP_IntDisable(INT_USB0);
        //_}

        //
        // Yes - take some characters out of the transmit buffer and feed
        // them to the UART transmit FIFO.
        //

        do
        {
        	isTxSpaceAvailable = MAP_UARTSpaceAvail(ulBase) && !TX_BUFFER_EMPTY;

        	// Look to see if USB is connected and has space,
        	// if UART TX can accept input.

        	if (isTxSpaceAvailable)
        	{
        		ucChar = g_pcUARTTxBuffer[g_ulUARTTxReadIndex];
        		MAP_UARTCharPutNonBlocking(ulBase, ucChar);
        		
            	ADVANCE_TX_BUFFER_INDEX(g_ulUARTTxReadIndex);

        	}
        } while (isTxSpaceAvailable);

        //
        // Reenable the UART interrupt.
        //
        //_MAP_IntEnable(g_ulUARTInt[g_ulPortNum]);
       //_ if(isUseUsb)
       //_ {
       //_ MAP_IntEnable(INT_USB0);
        //_}

    }

#endif
}
#endif

//*****************************************************************************
//
//! Initializes the UART console.
//!
//! \param ulPortNum is the number of UART port to use for the serial console
//! (0-2)
//!
//! This function will initialize the specified serial port to be used as a
//! serial console.  The serial parameters will be set to 115200, 8-N-1.
//! An application wishing to use a different baud rate may call
//! SiiPlatformUartExpClkInit() instead of this function.
//!
//! This function or SiiPlatformUartExpClkInit() must be called prior to using any
//! of the other UART console functions: SiiPlatformUartPrintf() or SiiPlatformUartGets().  In order
//! for this function to work correctly, SysCtlClockSet() must be called prior
//! to calling this function.
//!
//! It is assumed that the caller has previously configured the relevant UART
//! pins for operation as a UART rather than as GPIOs.
//!
//! \return None.
//
//*****************************************************************************
void SiiPlatformUartInit(unsigned long ulPortNum, unsigned long ulBaud)
{
    //
    // Initialize the Arm UART 0 peripheral
    //

    //_ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //_ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Pass this call on to the version of the function allowing the baud rate
    // to be specified.
    //
    //_SiiPlatformUartExpClkInit(ulPortNum, ulBaud);
}

//*****************************************************************************
//
//! Initializes the UART console and allows the baud rate to be selected.
//!
//! \param ulPortNum is the number of UART port to use for the serial console
//! (0-2)
//! \param ulBaud is the bit rate that the UART is to be configured to use.
//!
//! This function will initialize the specified serial port to be used as a
//! serial console.  The serial parameters will be set to 8-N-1 and the bit
//! rate set according to the value of the \e ulBaud parameter.
//!
//! This function or SiiPlatformUartInit() must be called prior to using any of the
//! other UART console functions: SiiPlatformUartPrintf() or SiiPlatformUartGets().  In order for
//! this function to work correctly, SysCtlClockSet() must be called prior to
//! calling this function.  An application wishing to use 115,200 baud may call
//! SiiPlatformUartInit() instead of this function but should not call both
//! functions.
//!
//! It is assumed that the caller has previously configured the relevant UART
//! pins for operation as a UART rather than as GPIOs.
//!
//! \return None.
//
//*****************************************************************************
#if 0
void
SiiPlatformUartExpClkInit(unsigned long ulPortNum, unsigned long ulBaud)
{
    //
    // Check the arguments.
    //
    ASSERT((ulPortNum == 0) || (ulPortNum == 1) ||
           (ulPortNum == 2));

#ifdef UART_BUFFERED
    //
    // In buffered mode, we only allow a single instance to be opened.
    //
    ASSERT(g_ulBase == 0);
#endif

    //
    // Check to make sure the UART peripheral is present.
    //
    if (!MAP_SysCtlPeripheralPresent(g_ulUARTPeriph[ulPortNum]))
    {
        return;
    }

    crlfFilter = true;   // default
    //
    // Select the base address of the UART.
    //
    g_ulBase = g_ulUARTBase[ulPortNum];

    //
    // Enable the UART peripheral for use.
    //
    MAP_SysCtlPeripheralEnable(g_ulUARTPeriph[ulPortNum]);

    //
    // Configure the UART for 115200, n, 8, 1
    //
    MAP_UARTConfigSetExpClk(g_ulBase, MAP_SysCtlClockGet(), ulBaud,
                            (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_WLEN_8));

#ifdef UART_BUFFERED
    //
    // Set the UART to interrupt whenever the TX FIFO is almost empty or
    // when any character is received.
    //
    MAP_UARTFIFOLevelSet(g_ulBase, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

    //
    // Flush both the buffers.
    //
    SiiPlatformUartFlushRx();
    SiiPlatformUartFlushTx(true);

    //
    // Remember which interrupt we are dealing with.
    //
    g_ulPortNum = ulPortNum;

    //
    // We are configured for buffered output so enable the master interrupt
    // for this UART and the receive interrupts.  We don't actually enable the
    // transmit interrupt in the UART itself until some data has been placed
    // in the transmit buffer.
    //
    MAP_UARTIntDisable(g_ulBase, 0xFFFFFFFF);
    MAP_UARTIntEnable(g_ulBase, UART_INT_RX | UART_INT_RT);
    MAP_IntEnable(g_ulUARTInt[ulPortNum]);
#endif

    //
    // Enable the UART operation.
    //
    MAP_UARTEnable(g_ulBase);
}
#endif
//*****************************************************************************
//
//! Writes a string of characters to the UART output.
//!
//! \param pcBuf points to a buffer containing the string to transmit.
//! \param ulLen is the length of the string to transmit.
//!
//! This function will transmit the string to the UART output.  The number of
//! characters transmitted is determined by the \e ulLen parameter.  This
//! function does no interpretation or translation of any characters.  Since
//! the output is sent to a UART, any LF (/n) characters encountered will be
//! replaced with a CRLF pair.
//!
//! Besides using the \e ulLen parameter to stop transmitting the string, if a
//! null character (0) is encountered, then no more characters will be
//! transmitted and the function will return.
//!
//! In non-buffered mode, this function is blocking and will not return until
//! all the characters have been written to the output FIFO.  In buffered mode,
//! the characters are written to the UART transmit buffer and the call returns
//! immediately.  If insufficient space remains in the transmit buffer,
//! additional characters are discarded.
//!
//! \return Returns the count of characters written.
//
//*****************************************************************************
#if 1
extern void UART1_SendBytes(const unsigned char  *Data, u8 num);
int SiiPlatformUartWrite(const unsigned char  *pcBuf, unsigned long ulLen)
{
    UART1_SendBytes(pcBuf, ulLen);
    return ulLen;

}

#else
int SiiPlatformUartWrite(const char *pcBuf, unsigned long ulLen)
{
#ifdef UART_BUFFERED
    unsigned int uIdx;

    //
    // Check for valid arguments.
    //
    ASSERT(pcBuf != 0);
    //_ASSERT(g_ulBase != 0);

    //
    // Send the characters
    //
    for (uIdx = 0; uIdx < ulLen; uIdx++)
    {
        //
        // If the character to the UART is \n, then add a \r before it so that
        // \n is translated to \n\r in the output.
        //
        if ( crlfFilter && ( pcBuf[uIdx] == '\n'))
        {
            if (!TX_BUFFER_FULL)
            {
                g_pcUARTTxBuffer[g_ulUARTTxWriteIndex] = '\r';
                ADVANCE_TX_BUFFER_INDEX(g_ulUARTTxWriteIndex);
            }
            else
            {
                //
                // Buffer is full - discard remaining characters and return.
                //
                break;
            }
        }

        //
        // Send the character to the UART output.
        //
        if (!TX_BUFFER_FULL)
        {
            g_pcUARTTxBuffer[g_ulUARTTxWriteIndex] = pcBuf[uIdx];
            ADVANCE_TX_BUFFER_INDEX(g_ulUARTTxWriteIndex);
        }
        else
        {
            //
            // Buffer is full - discard remaining characters and return.
            //
            break;
        }
    }

    //
    // If we have anything in the buffer, make sure that the UART is set
    // up to transmit it.
    //
    if (!TX_BUFFER_EMPTY)
    {
        UARTPrimeTransmit(g_ulBase);
        //_ MAP_UARTIntEnable(g_ulBase, UART_INT_TX);
    }

    //
    // Return the number of characters written.
    //
    return(uIdx);
#else
    unsigned int uIdx;

    //
    // Check for valid UART base address, and valid arguments.
    //
    ASSERT(g_ulBase != 0);
    ASSERT(pcBuf != 0);

    //
    // Send the characters
    //
    for (uIdx = 0; uIdx < ulLen; uIdx++)
    {
        //
        // If the character to the UART is \n, then add a \r before it so that
        // \n is translated to \n\r in the output.
        //
        if (pcBuf[uIdx] == '\n')
        {
            MAP_UARTCharPut(g_ulBase, '\r');
        }

        //
        // Send the character to the UART output.
        //
        MAP_UARTCharPut(g_ulBase, pcBuf[uIdx]);
    }

    //
    // Return the number of characters written.
    //
    return(uIdx);
#endif
}
#endif
//------------------------------------------------------------------------------
// Function:    SiiPlatformUartPutc
// Description: Sends a single byte to the UART
//------------------------------------------------------------------------------

void SiiPlatformUartPutc ( char outChar )
{

    SiiPlatformUartWrite( (const unsigned char  *)&outChar, 1 );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformUartPutcUnfiltered
// Description: Sends a single byte to the UART
//------------------------------------------------------------------------------

void SiiPlatformUartPutcUnfiltered ( char outChar )
{

    //crlfFilter = false;
    SiiPlatformUartWrite( (const unsigned char  *)&outChar, 1 );
}


//*****************************************************************************
//
//! Returns the number of bytes available in the receive buffer.
//!
//! This function, available only when the module is built to operate in
//! buffered mode using \b UART_BUFFERED, may be used to determine the number
//! of bytes of data currently available in the receive buffer.
//!
//! \return Returns the number of available bytes.
//
//*****************************************************************************
#if defined(UART_BUFFERED) || defined(DOXYGEN)
int
SiiPlatformUartRxBytesAvail(void)
{
    return(RX_BUFFER_USED);
}
#endif

#if defined(UART_BUFFERED) || defined(DOXYGEN)
//*****************************************************************************
//
//! Returns the number of bytes free in the transmit buffer.
//!
//! This function, available only when the module is built to operate in
//! buffered mode using \b UART_BUFFERED, may be used to determine the amount
//! of space currently available in the transmit buffer.
//!
//! \return Returns the number of free bytes.
//
//*****************************************************************************
int
SiiPlatformUartTxBytesFreeGet(void)
{
    return(TX_BUFFER_FREE);
}
#endif

//*****************************************************************************
//
//! Flushes the receive buffer.
//!
//! This function, available only when the module is built to operate in
//! buffered mode using \b UART_BUFFERED, may be used to discard any data
//! received from the UART but not yet read using SiiPlatformUartGets().
//!
//! \return None.
//
//*****************************************************************************
#if 0 // defined(UART_BUFFERED) || defined(DOXYGEN)
void
SiiPlatformUartFlushRx(void)
{
    unsigned long ulInt;

    //
    // Temporarily turn off interrupts.
    //
    ulInt = ROM_IntMasterDisable();

    //
    // Flush the receive buffer.
    //
    g_ulUARTRxReadIndex = 0;
    g_ulUARTRxWriteIndex = 0;

    //
    // If interrupts were enabled when we turned them off, turn them
    // back on again.
    //
    if (!ulInt)
    {
        ROM_IntMasterEnable();
    }
}
#endif

//*****************************************************************************
//
//! Flushes the transmit buffer.
//!
//! \param bDiscard indicates whether any remaining data in the buffer should
//! be discarded (\b true) or transmitted (\b false).
//!
//! This function, available only when the module is built to operate in
//! buffered mode using \b UART_BUFFERED, may be used to flush the transmit
//! buffer, either discarding or transmitting any data received via calls to
//! SiiPlatformUartPrintf() that is waiting to be transmitted.  On return, the transmit
//! buffer will be empty.
//!
//! \return None.
//
//*****************************************************************************
#if  0//defined(UART_BUFFERED) || defined(DOXYGEN)
void
SiiPlatformUartFlushTx(tBoolean bDiscard)
{
    unsigned long ulInt;

    //
    // Should the remaining data be discarded or transmitted?
    //
    if (bDiscard)
    {
        //
        // The remaining data should be discarded, so temporarily turn off
        // interrupts.
        //
       //_ ulInt = ROM_IntMasterDisable();

        //
        // Flush the transmit buffer.
        //
       // g_ulUARTTxReadIndex = 0;
       // g_ulUARTTxWriteIndex = 0;

        //
        // If interrupts were enabled when we turned them off, turn them
        // back on again.
        //
        if (!ulInt)
        {
            //_ROM_IntMasterEnable();
        }
    }
    else
    {
        //
        // Wait for all remaining data to be transmitted before returning.
        //
        while(!TX_BUFFER_EMPTY)
        {
        }
    }
}
#endif

//*****************************************************************************
//
//! Read a single character from the UART, blocking if necessary.
//!
//! This function will receive a single character from the UART and store it at
//! the supplied address.
//!
//! In both buffered and unbuffered modes, this function will block until a
//! character is received.  If non-blocking operation is required in buffered
//! mode, a call to UARTRxAvail() may be made to determine whether any
//! characters are currently available for reading.
//!
//! \return Returns the character read.
//
//*****************************************************************************
BOOL SiiPlatformUartGetc(unsigned char *pchar)
{
#ifdef UART_BUFFERED
    //unsigned char cChar;

    //
    // Wait for a character to be received.
    //
    if(RX_BUFFER_EMPTY)
    {
        //
        // Block waiting for a character to be received (if the buffer is
        // currently empty).
        //
      return FALSE; // add by kyq
    }

    //
    // Read a character from the buffer.
    //
    *pchar = g_pcUARTRxBuffer[g_ulUARTRxReadIndex];
    ADVANCE_RX_BUFFER_INDEX(g_ulUARTRxReadIndex);

    //
    // Return the character to the caller.
    //
    return(TRUE);
#else
    //
    // Block until a character is received by the UART then return it to
    // the caller.
    //
    return(MAP_UARTCharGet(g_ulBase));
#endif
}

//*****************************************************************************
//
//! Handles UART interrupts.
//!
//! This function handles interrupts from the UART.  It will copy data from the
//! transmit buffer to the UART transmit FIFO if space is available, and it
//! will copy data from the UART receive FIFO to the receive buffer if data is
//! available.
//!
//! \return None.
//
//*****************************************************************************

#if defined(UART_BUFFERED) || defined(DOXYGEN)
//void SiiPlatformUartIntHandler(void)

#endif

void SiiPlatformUartCrlfFilterSet ( bool_t enable )
{
    //crlfFilter = enable;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************



















void USART2_IRQHandler(void)
{
    BYTE cChar;
    static BYTE cLastChar = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        cChar = USART2->DR;

#if 0

		
        if (USART2_rxpoint < (USART2_RX_BUFF + USART2_RX_BUFF_MAX))
            *USART2_rxpoint++ = cData;

        if (USART2_rxpoint >= (USART2_RX_BUFF + USART2_RX_BUFF_MAX))
        {
            USART2_rxpoint = USART2_RX_BUFF;

        }

#endif

        	//Do not store the char if buffer full
        	if (!RX_BUFFER_FULL)
        	{
		        // Read a character	
		    // 连续两个0xff 丢掉一个
		 if(!((cChar == 0xff)&&(cLastChar == 0xff)))  // it's seem some time will be lost data
		 	{
        	         g_pcUARTRxBuffer[g_ulUARTRxWriteIndex] =cChar;
            	         ADVANCE_RX_BUFFER_INDEX(g_ulUARTRxWriteIndex);	
			  cLastChar = cChar;
		 	}
        	}
#if 0
			//processing the packets.
        	if (!SkSerialConsumedChar()) // isBuffInUse == false
        	{
                SkSerialRxHandler( SiiPlatformUartGetc());
            }
#endif



    }
    //else  UART_temp = USART1->DR;  //串口清中断是当你读走数据后硬件自动完成清中断

    if (USART_GetITStatus(USART2, USART_IT_ORE) != RESET)
    {
        USART_ClearFlag(USART2, USART_IT_ORE);
        cChar = USART2->DR;  //串口清中断是当你读走数据后硬件自动完成清中断
    }

}


void UART2_SendByte(u16 Data)
{
    while (!(USART2->SR & USART_FLAG_TXE));
    USART2->DR = (Data & (uint16_t)0x01FF);
    while (!(USART2->SR & USART_FLAG_TC));
}

void UART2_SendBytes(const unsigned char  *Data, u8 num)
{
    u8 i = 0;
    for (i = 0; i < num; i++)	  UART2_SendByte(*(Data + i));
}

void UART2_Init(void)
{
    //USART2_rxpoint = USART2_RX_BUFF;
    //crlfFilter = true;   // default
    g_ulUARTRxReadIndex = 0;
    g_ulUARTRxWriteIndex = 0; 
    g_ulUARTTxWriteIndex = 0;
    g_ulUARTTxReadIndex = 0;

}

