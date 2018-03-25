
#ifndef _DVNVRAM_H_
#define _DVNVRAM_H_



//============================================================================
// CONFIGURATION: parameters for each supported NVRAM device type
//============================================================================

#ifdef NV24LC32A
#define NV_2WIRE_DEVICE_ADDR    0xA2
#define NV_ADDRESS_FIELD_SIZE    16
#define NV_BLOCK_SIZE            4096
#define NV_BLOCK_COUNT           1
#define NV_PAGE_COUNT         126
#define NV_PAGE_WRITE_LEN        32
#define NV_WRITE_RETRY		50
#define NV_READ_RETRY		50
#define NV_FLAG				FLG_WADDR 
#define NV_WRITE_DELAY          0
#endif

#ifdef AT24CM01_1024
#define NV_2WIRE_DEVICE_ADDR    0xA0
#define NV_ADDRESS_FIELD_SIZE    16
#define NV_BLOCK_SIZE            131072
#define NV_BLOCK_COUNT           1
#define NV_PAGE_COUNT         512
#define NV_PAGE_WRITE_LEN        256
#define NV_WRITE_RETRY		50
#define NV_READ_RETRY		50
#define NV_FLAG				FLG_WADDR 
#define NV_WRITE_DELAY          0
#endif

#ifdef  AT24C02
#define NV_ADDRESS_FIELD_SIZE    8
#define NV_BLOCK_SIZE            256
#define NV_BLOCK_COUNT           1
#define NV_PAGE_WRITE_LEN        8
#define NV_WRITE_DELAY          0
#endif

#ifdef  AT24C04
#define NV_ADDRESS_FIELD_SIZE    8
#define NV_BLOCK_SIZE            256
#define NV_BLOCK_COUNT           2
#define NV_PAGE_WRITE_LEN        16
#define NV_WRITE_DELAY          0
#endif

#ifdef  AT24C08 
#define NV_ADDRESS_FIELD_SIZE    8
#define NV_BLOCK_SIZE            256
#define NV_BLOCK_COUNT           4
#define NV_PAGE_WRITE_LEN        16
#define NV_WRITE_DELAY          0
#endif

#ifdef  AT24C16  
#define NV_ADDRESS_FIELD_SIZE    8
#define NV_BLOCK_SIZE            256
#define NV_BLOCK_COUNT           8
#define NV_PAGE_WRITE_LEN        16
#define NV_WRITE_DELAY          0
#endif

#define USER_EDID_NUMBER edsEDID_EXTERN1 //edsEDID_EXTERN1
#define USER_EDID_LENGTH 256
#define USER_EDID_TOALSIZE USER_EDID_NUMBER*USER_EDID_LENGTH
#define USER_EDID_ADDR    (NV_BLOCK_SIZE * NV_BLOCK_COUNT-USER_EDID_TOALSIZE)
// ***************************************************************************
// ****************** START OF PUBLIC PROCEDURE DEFINITIONS ******************
// ***************************************************************************

// ---------------------------------------------------------------------------
// This API initializes the NVRAM device driver.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
void dvNVRAMInitialize(void);

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
                          PBYTE pcData);

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
//
// /*@out@*/ is an annotation for lint denoting a pointer to storage that may
//   be undefined.  This routine will perform the definition.
// ---------------------------------------------------------------------------
eRESULT dvNVRAMBytesRead(DWORD  wStart,
                         DWORD  wSize,
/*@out@*/                PBYTE pcData);

// ---------------------------------------------------------------------------
// This API function returns the defined NVRAMSIZE.
//
// Params: none
//
// Return:
//  Size of NVRAM, in bytes
// ---------------------------------------------------------------------------
DWORD dvNVRAMSizeGet(void);

//  读取NVRAM 中的EDID
//  cId:第几个EDID, 从0 开始
eRESULT dvNVRAMUserEdidRead(BYTE cId,PBYTE pcEdid);

// 把数据写入NVARAM 中
//  cId:第几个EDID, 从0 开始
eRESULT dvNVRAMUserEdidWrite(BYTE cId,PBYTE pcEdid);

// 初始化化EDID 部分数据
eRESULT dvNVRAMUserEdidInit(PBYTE pcEdid);


eRESULT dvNVRAMInputPortEdidInit(PBYTE pcEdid);


#endif // 


