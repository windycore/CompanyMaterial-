//
//  file: Uart2.h
//
#ifndef  _UART2_H_
#define  _UART2_H_
#include "stm32f10x.h"
#define  USART2_RX_BUFF_MAX   20
#define  USART2_TX_BUFF_MAX   20
#define UART_BUFFERED

//*****************************************************************************
//
// If built for buffered operation, the following labels define the sizes of
// the transmit and receive buffers respectively.
//
//*****************************************************************************
#ifdef UART_BUFFERED
#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE     128
#endif
#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE     3
#endif
#endif


void UART2_Init(void);
void UART2_SendByte(u16 Data);
void UART2_SendBytes(const unsigned char  *Data, u8 num);


//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
void            SiiPlatformUartInit(unsigned long ulPortNum, unsigned long ulBaud);
//void            SiiPlatformUartExpClkInit(unsigned long ulPort, unsigned long ulBaud);
//int             SiiPlatformUartGets(char *pcBuf, unsigned long ulLen);
void            SiiPlatformUartPutc( char outChar );
//unsigned char   SiiPlatformUartGetc(void);
BOOL SiiPlatformUartGetc(unsigned char *pchar);

//void            SiiPlatformUartPrintf(const char *pcString, ...);
//int             SiiPlatformUartWrite(const char *pcBuf, unsigned long ulLen);
int SiiPlatformUartWrite(const unsigned char  *pcBuf, unsigned long ulLen);

void            SiiPlatformUartCrlfFilterSet ( bool_t enable );
#ifdef UART_BUFFERED
//void            SiiPlatformUartFlushTx(tBoolean bDiscard);
//void            SiiPlatformUartFlushRx(void);
int             SiiPlatformUartRxBytesAvail(void);
int             SiiPlatformUartTxBytesFreeGet(void);
void            SiiPlatformUartPutcUnfiltered ( char outChar );
#endif
extern int Uart_printf(const char *fmt, ...);

//------------------------------------------------------------------------------
//  UART h/w interrupt handler
//------------------------------------------------------------------------------

//void            SiiPlatformUartIntHandler(void);


#endif

