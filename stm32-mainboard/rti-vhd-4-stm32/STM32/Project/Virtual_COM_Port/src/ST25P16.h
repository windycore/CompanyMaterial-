
#ifndef __ST25P16_h
#define __ST25P16_h
#include "stm32f10x.h"
#include "sys.h" 

#define IO_H    1
#define IO_L    0
#define ptrNull 0 
#define ENFLASH 1 
#define DISFLASH 2    
#define  byte  uint8_t
#define dword   uint32_t
#define word	uint16_t
#define FLASH_SIZE (0x0400000)           /* Total device size in Bytes */
#define FLASH_PAGE_COUNT (0x04000)       /* Total device size in Pages */   //页大小 16K
#define FLASH_SECTOR_COUNT (0x40)        /* Total device size in Sectors */ //扇区大小64 Byte
#define FLASH_WRITE_BUFFER_SIZE 0x100    /* Write Buffer = 256 bytes */ //我们每次写256字节
#define SPI_FLASH_INS_DUMMY 0xAA         /* dummy byte*/


 //flash使能信号             
#define FLASH_CE_PORT          GPIOA  
#define FLASH_CE_IO            4

#define FLASH_CLK_PORT         GPIOA  
#define FLASH_CLK_IO           7

#define FLASH_DIN_PORT         GPIOA
#define FLASH_DIN_IO           6

#define FLASH_DOUT_PORT        GPIOA  
#define FLASH_DOUT_IO          5

   //flash片选信号
#define FLASH_CE_H            GPIO_SetBits(FLASH_CE_PORT, (1<<FLASH_CE_IO));
#define FLASH_CE_L            GPIO_ResetBits(FLASH_CE_PORT, (1<<FLASH_CE_IO));
  //flash时钟信号
#define FLASH_CLK_H           GPIO_SetBits(FLASH_CLK_PORT, 1<<FLASH_CLK_IO);
#define FLASH_CLK_L           GPIO_ResetBits(FLASH_CLK_PORT, (1<<FLASH_CLK_IO));

  //flash数据输入信号
#define FLASH_DIN_H           GPIO_SetBits(FLASH_DIN_PORT, (1<<FLASH_DIN_IO));
#define FLASH_DIN_L           GPIO_ResetBits(FLASH_DIN_PORT, (1<<FLASH_DIN_IO));

  //flash数据输出信号
#define FLASH_DOUT_R          GPIO_ReadInputDataBit(FLASH_DOUT_PORT, (1<<FLASH_DOUT_IO))


#define QS3L384_BEA_IO         4
#define QS3L384_BEB_IO         2
//对应管脚PC4  BEA    PD2  BEB
#define QS3L384_BEA    PCout(QS3L384_BEA_IO )
#define QS3L384_BEB    PDout(QS3L384_BEB_IO)

 extern unsigned int g_wTimers;
void ST25P16_init(void);
byte Flash_Rw_Func(byte bRw,dword dwAddr,byte  *bpData,word wLen);
void test_main(void);
void QS3L384_init(void);
byte FlashPageProgram( dword dwAddr, byte *bpData, word wLen);
void TIME_Config(void);
byte Flash_Erase(void);
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)  ;
#endif
