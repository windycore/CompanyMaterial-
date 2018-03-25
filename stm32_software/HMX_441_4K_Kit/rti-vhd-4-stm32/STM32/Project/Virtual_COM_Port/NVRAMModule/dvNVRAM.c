// file: dvNVRAM.h
#include "BSP_I2C.h"
#include "dvNVRAM.h"
#include "mcu.h"
#include "uart.h"
#include "Sk_app_edid.h"
#include "util.h"
// Identifier of I2C bus used to communicate with NVRAM device
//--------------------------------CONFIGURATION-------------------------------
#define NVRAM_BUS_ID   eI2CBusAD4604_EEPROM
//--------------------------------CONFIGURATION-------------------------------
// Select NVRAM device hardware type
//--------------------------------CONFIGURATION-------------------------------
//--------------------------------CONFIGURATION-------------------------------
// The NVRAM may require to poll the ACK if the internal programming cycle is
// in process.  This definition allow the checking the status
//--------------------------------CONFIGURATION-------------------------------
#define WRITE_BUSY_CHECK_COUNT 3   // 3 Time
/* ---------------------------- END CUSTOM AREA --------------------------- */
// ***************************************************************************
// ******************* END OF PRIVATE CONFIGURATION SECTION ******************
// ***************************************************************************
// ***************************************************************************
// ******************** START OF PRIVATE DATA DECLARATIONS *******************
// ***************************************************************************
// This file does not contain any private data declarations.
// ***************************************************************************
// **************** START OF PRIVATE PROCEDURE IMPLEMENTATIONS ***************
// ***************************************************************************
#ifdef  WRITE_BUSY_CHECK_COUNT
// ---------------------------------------------------------------------------
// This proecure uses a read function to poll the NVRAM chip if it is busy
// programing.
//
// Params: none
//
// Return: rcSUCCESS if the read is completed without error
// ---------------------------------------------------------------------------
static eRESULT dvNVRAMCheckWriteBusy(void)
{
   eRESULT eResult = rcINVALID;
   BYTE cIndex, cData;
   for (cIndex = 0; cIndex <= WRITE_BUSY_CHECK_COUNT; cIndex++)
   {
        // read one register to see if the read is successfully completed
        eResult = dvDeviceBusRead(NVRAM_BUS_ID,
                                  NV_2WIRE_DEVICE_ADDR,
                                  0,    
                                   1,
                                  &cData,                                 
                                  NV_FLAG);
        // If not sucessful, sleep one tick and check again.
        if (rcSUCCESS != eResult)
        {
            // sleep one tick
            delay_nus(1);
            
        }
        else
        {
            // read success, it means there is no internal programming
            break;
        }
   }
   // if it is not successful, considering increase the WRITE_BUSY_CHECK_COUNT
   ASSERT(rcSUCCESS == eResult);
   return eResult;
}
#endif
// ***************************************************************************
// **************** START OF PUBLIC PROCEDURE IMPLEMENTATIONS ****************
// ***************************************************************************
// ---------------------------------------------------------------------------
// This API initializes the NVRAM device driver.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void dvNVRAMInitialize(void)
{
    return;
}
// ---------------------------------------------------------------------------
// This API function writes a block of sequential bytes to the NVRAM device
// beginning at a specified address.
//
// Params:
//  wStart : Beginning byte address for block write
//  wSize  : Block size to write in bytes; 0 < wNumBytes <= NVRAMSIZE
//  pcData : Pointer to array of data to write
//
// Return:
//  rcSUCCESS if writing was successful else rcERROR if error occurred.
// ---------------------------------------------------------------------------
eRESULT dvNVRAMBytesWrite(DWORD   wStart,
                          DWORD   wSize,
                          PBYTE pcData)
{
    eRESULT             eResult = rcINVALID;
    BYTE                cRetries;
    DWORD                wCurrentAddress, wEnd;
    WORD                wStartPage, wEndPage, wMidPages;
    WORD                wNextPageAddr, wPagePremainder, wPageRemainder;
    DWORD                 i;
    BYTE                DeviceAddr;   
    BYTE                P0; 
    ASSERT(pcData != NULL);
    // Notes:
    // This routine deals with NVRAM on a page basis, which for 32Kb EEPROMs
    // is every 32 bytes starting at address 0.  Up to the page size may be
    // written to the device in one transaction, but only if the starting
    // address is on a page boundary.  If this is not the case, then the
    // internal address counter overflows and the bytes that go beyond the
    // page boundary are written starting from the beginning of the same page,
    // effectively corrupting the data the used to be there.
    //
    // All transfers are broken up into three major groupings:
    //
    // Premainder: the number of bytes, if any, that exist in a transfer
    // before the next page boundary.  Mid Pages: the number of whole pages,
    // if any, that exist in a transfer following the Premainder and
    // preceeding the Remainder.  The Mid Pages are inherently page aligned.
    // Remainder: the number of bytes, if any, that follow the Mid Pages but
    // do not consume a whole page.  The Remainder is inherently page aligned.
    //
    // Since 32Kb EEPROMs use 12-bit addressing, there are no restrictions
    // such as block sizes that make dealing with smaller devices more
    // tedious.
    // Precalculate all our data used in the write loops that follow.
    wCurrentAddress = wStart;
    wEnd = wStart + wSize - 1;         // end address of this transfer in
                                       // NVRAM
    wStartPage = wStart / NV_PAGE_WRITE_LEN;    // starting block number for
                                                // transfer
    wEndPage = wEnd / NV_PAGE_WRITE_LEN;    // ending block number for
                                            // transfer
    // If this operation is entirely within one NVRAM page, then setup for
    // that situation.
    if (wStartPage == wEndPage)
    {
        wPagePremainder = wSize;       // this transfer is handled entirely as
                                       // premainder
        wMidPages = 0;                 // no chunks in the middle
        wPageRemainder = 0;            // no remainder bytes
    }
    else
    {
        // We're not entirely within one page
        // Calculate the address of the next page boundary.
        wNextPageAddr = ((wStart / NV_PAGE_WRITE_LEN) + 1) *
            NV_PAGE_WRITE_LEN;
        // Premainder is the number of bytes before the first page boundary
        wPagePremainder = wNextPageAddr - wStart;
        // Check if any bytes follow the last page boundary in the transfer
        if ((wEnd + 1) % NV_PAGE_WRITE_LEN)
        {
            // Remainder is the number of bytes after the last page boundary
            wPageRemainder = wEnd - ((wEnd /
                                      NV_PAGE_WRITE_LEN) *
                                     NV_PAGE_WRITE_LEN) + 1;
        }
        else
        {
            // Tranfer ends at a page boundary so no remainder
            wPageRemainder = 0;
        }
    }
    // Calculate number of whole pages are in the middle of this transfer
    wMidPages = (wSize - wPagePremainder - wPageRemainder) /
       NV_PAGE_WRITE_LEN;
    // -----------------------------------------------------------------------
    // BEGIN ACCESSING THE NVRAM
    // -----------------------------------------------------------------------
    // Write out the Premainder
    if (wPagePremainder)
    {
        // Attempt to write up to the configured number of read retries
        for (cRetries = 0; (rcSUCCESS != eResult) &&
             (cRetries < NV_WRITE_RETRY); cRetries++)
        {
#ifdef AT24CM01_1024
           P0 = (wCurrentAddress&0x10000) ? 0x02:0x00;        // 计算第17位，即P0
      DeviceAddr=NV_2WIRE_DEVICE_ADDR | P0;     // 带P0发送器件地址
            // Report failure only after max retries have been exeeded
            eResult = dvDeviceBusWrite(NVRAM_BUS_ID,
                                       DeviceAddr,
                                       wCurrentAddress,  
                                        wPagePremainder,
                                       (PBYTE)pcData,                                       
                                       NV_FLAG);
#else

                // Report failure only after max retries have been exeeded
            eResult = dvDeviceBusWrite(NVRAM_BUS_ID,
                                       NV_2WIRE_DEVICE_ADDR,
                                       wCurrentAddress,  
                                        wPagePremainder,
                                       (PBYTE)pcData,                                       
                                       NV_FLAG);
#endif
            if (rcSUCCESS != eResult)
            {
                //utilExecSleep(uS_TO_TICKS(NV_WRITE_DELAY));
                delay_nus(1);
            }
#ifdef  WRITE_BUSY_CHECK_COUNT
            // Check if the internal programming is completed by reading one byte
            dvNVRAMCheckWriteBusy();
#endif
        }
        wCurrentAddress += wPagePremainder;
        pcData += wPagePremainder;
    }
    // Write out the Mid Pages
    for (i = 0; i < wMidPages; i++)
    {
        // Attempt to write up to the configured number of read retries
        eResult = rcINVALID;
        for (cRetries = 0; (rcSUCCESS != eResult) &&
             (cRetries < NV_WRITE_RETRY); cRetries++)
        {
#ifdef AT24CM01_1024
           P0 = (wCurrentAddress&0x10000) ? 0x02:0x00;        // 计算第17位，即P0
      DeviceAddr=NV_2WIRE_DEVICE_ADDR | P0;     // 带P0发送器件地址
            // Report failure only after max retries have been exeeded
            eResult = dvDeviceBusWrite(NVRAM_BUS_ID,
                                       DeviceAddr,
                                       wCurrentAddress,  
                                        NV_PAGE_WRITE_LEN,
                                       (PBYTE)pcData,                                       
                                       NV_FLAG);
#else

                // Report failure only after max retries have been exeeded
            eResult = dvDeviceBusWrite(NVRAM_BUS_ID,
                                       NV_2WIRE_DEVICE_ADDR,
                                       wCurrentAddress,  
                                        NV_PAGE_WRITE_LEN,
                                       (PBYTE)pcData,                                       
                                       NV_FLAG);
#endif
            if (rcSUCCESS != eResult)
            {
                delay_nus(1);//utilExecSleep(uS_TO_TICKS(NV_WRITE_DELAY));
            }
#ifdef  WRITE_BUSY_CHECK_COUNT
            // Check if the internal programming is completed by reading one byte
            dvNVRAMCheckWriteBusy();
#endif
        }
        wCurrentAddress += NV_PAGE_WRITE_LEN;
        // TODO
        pcData += NV_PAGE_WRITE_LEN;
    }
    // Write out the Remainder
    if (wPageRemainder)
    {
        // Attempt to write up to the configured number of read retries
        eResult = rcINVALID;
        for (cRetries = 0; (rcSUCCESS != eResult) &&
             (cRetries < NV_WRITE_RETRY); cRetries++)
        {
#ifdef AT24CM01_1024
           P0 = (wCurrentAddress&0x10000) ? 0x02:0x00;        // 计算第17位，即P0
      DeviceAddr=NV_2WIRE_DEVICE_ADDR | P0;     // 带P0发送器件地址
            // Report failure only after max retries have been exeeded
            eResult = dvDeviceBusWrite(NVRAM_BUS_ID,
                                       DeviceAddr,
                                       wCurrentAddress,  
                                        wPageRemainder,
                                       (PBYTE)pcData,                                       
                                       NV_FLAG);
#else

                // Report failure only after max retries have been exeeded
            eResult = dvDeviceBusWrite(NVRAM_BUS_ID,
                                       NV_2WIRE_DEVICE_ADDR,
                                       wCurrentAddress,  
                                        wPageRemainder,
                                       (PBYTE)pcData,                                       
                                       NV_FLAG);
#endif
            if (rcSUCCESS != eResult)
            {
                delay_nus(1);//utilExecSleep(uS_TO_TICKS(NV_WRITE_DELAY));
            }
#ifdef  WRITE_BUSY_CHECK_COUNT
            // Check if the internal programming is completed by reading one byte
            dvNVRAMCheckWriteBusy();
#endif
        }
    }
    return eResult;
}
// ---------------------------------------------------------------------------
// This API function reads a number of sequential bytes from the NVRAM device
// beginning at the specified address.
//
// Params:
//  wStart : Beginning byte address for block read
//  wSize  : Block size to read in bytes; 0 < wNumBytes <= NVRAMSIZE
//  pcData : Pointer to array for return data
//
// Return:
//  rcSUCCESS if reading was successful else rcERROR if error occurred
// ---------------------------------------------------------------------------
eRESULT dvNVRAMBytesRead(DWORD  wStart,
                         DWORD  wSize,
                         PBYTE pcData)
{
    eRESULT             eResult = rcINVALID;
    BYTE                cRetries;
    BYTE                DeviceAddr;   
    BYTE                P0; 
    ASSERT(pcData != NULL);
    ASSERT((wStart+wSize)<dvNVRAMSizeGet());
    // Attempt to read up to the configured number of read retries
    for (cRetries = 0; (rcSUCCESS != eResult) &&
         (cRetries < NV_READ_RETRY); cRetries++)
    {

#ifdef AT24CM01_1024
      P0 = (wStart&0x10000) ? 0x02:0x00;        // 计算第17位，即P0
      DeviceAddr=NV_2WIRE_DEVICE_ADDR | P0;     // 带P0发送器件地址
        // Report failure only after max retries have been exeeded
        eResult = dvDeviceBusRead(NVRAM_BUS_ID,
                                  DeviceAddr,
                                  wStart,   
                                  wSize,
                                  (PBYTE)pcData,                                  
                                  NV_FLAG);
#else
           // Report failure only after max retries have been exeeded
        eResult = dvDeviceBusRead(NVRAM_BUS_ID,
                                  NV_2WIRE_DEVICE_ADDR,
                                  wStart,   
                                  wSize,
                                  (PBYTE)pcData,                                  
                                  NV_FLAG);
#endif
        if (rcSUCCESS != eResult)
        {
            delay_nus(1);//utilExecSleep(uS_TO_TICKS(NV_WRITE_DELAY));
        }
    }
    ASSERT(rcSUCCESS == eResult);
    return eResult;
}
// ---------------------------------------------------------------------------
// This API function returns the defined NVRAMSIZE.
//
// Params: none
//
// Return:
//  Size of NVRAM, in bytes
// ---------------------------------------------------------------------------
DWORD dvNVRAMSizeGet(void)
{
    return (DWORD)(NV_BLOCK_SIZE * NV_BLOCK_COUNT);
}
// 先复制这些垃圾代码用
unsigned char i2c_EE_read_one_byte(unsigned int suba) 
{
BYTE cReadData;
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesRead(suba,1,&cReadData);
ASSERT(eResult == rcSUCCESS);
return cReadData;
}
void i2c_EE_write_one_byte(unsigned int suba,unsigned char c) 
{
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesWrite(suba,1,&c);
ASSERT(eResult == rcSUCCESS);
}
void Read_EDID(WORD address, unsigned char *p_data)
{
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesRead(address,256,p_data);
assert_param(eResult == rcSUCCESS);
}
// 把EDID 写入 EEPROM 保存
// channel: 从 1 开始算
void write_EDID(unsigned char channel,unsigned char *pBuff)
{
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesWrite(INPUT_EDID_add + 256*(channel-1) ,256,pBuff);
assert_param(eResult == rcSUCCESS);
}  
// THIS IS READ FROM RX's EDID
// FOR BUILDING NOW
void get_EDID(unsigned char port,unsigned char *p_data)
{
}
void write_C32_page(WORD address,unsigned char *pBuff)  //32字节一页
{
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesWrite(address ,32,pBuff);
assert_param(eResult == rcSUCCESS);
}

eRESULT dvNVRAMUserEdidRead(BYTE cId,PBYTE pcEdid)
{
eRESULT eResult = rcSUCCESS;
assert_param(cId<USER_EDID_NUMBER);
if(cId<USER_EDID_NUMBER)
   eResult = dvNVRAMBytesRead((USER_EDID_ADDR+cId*USER_EDID_LENGTH),USER_EDID_LENGTH,pcEdid);
assert_param(eResult == rcSUCCESS);
return eResult;
}
eRESULT dvNVRAMUserEdidWrite(BYTE cId,PBYTE pcEdid)
{
eRESULT eResult = rcSUCCESS;
eResult = dvNVRAMBytesWrite((USER_EDID_ADDR+cId*USER_EDID_LENGTH),USER_EDID_LENGTH,pcEdid);
assert_param(eResult == rcSUCCESS);
return eResult;
}
eRESULT dvNVRAMUserEdidInit(PBYTE pcEdid)
{
   eRESULT eResult = rcSUCCESS;
   BYTE i;
     for(i =0; i< USER_EDID_NUMBER; i++)
     	{
           eResult |= dvNVRAMUserEdidWrite(i,pcEdid);
     	}
   return eResult;
}
// called by reset
eRESULT dvNVRAMInputPortEdidInit(PBYTE pcEdid)
{
   eRESULT eResult = rcSUCCESS;
   BYTE i;
     for(i =edsEDID_Default; i< edsEDID_INTERN1; i++) // edsEDID_IN1  -> edsEDID_Default  [kyq edid: 20151027 16:42:40]
     	{ 
           eResult |= dvNVRAMUserEdidWrite(i,pcEdid);
     	}
   return eResult;
}
