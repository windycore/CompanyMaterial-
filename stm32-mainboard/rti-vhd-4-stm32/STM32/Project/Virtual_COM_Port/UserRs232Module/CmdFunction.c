
#define _CMDFUNCITON_C_
#include <string.h>
#include "main.h"
#include "stm32f10x.h"
#include "uart.h"
#include "dvCPLD.h"

#include "dvPC9539.h"
#include <stdarg.h>
#include "dvNVRAM.h"
#include "dvM21452.h"
#include "BSP_I2C.h"
#include "util.h"
#include "appSystemMgrAPI.h"
#include "appUserInterface.h"
#include "si_logger.h"
#include "GitHash.h"
#include "Si_timer.h"
#include "appPowerMgrAPI.h"
#include "LedReceiveEvent.h"
#include "SwitchVideoReceiveEvent.h"
#include "KeyTasks.h"
#include "CmdFunction.h"
#include "InputHandle.h"

#include "sii_api.h"
#include "hal.h"

extern uint8_t  USART1_RX_BUFF[USART1_RX_BUFF_MAX]; //串口接收数据区
extern uint8_t  EDID_updata_port;
extern clock_time_t EdidUpataStart;
extern BOOL  System_UnLock_flag;
extern BYTE OutputEdidData[MAX_OUT][256];

#ifndef SWITCH_CMD_COTINUE
extern void ProessVedioSwitch(BYTE cInPort, BYTE cOutPort, SWITCH_e eFlag);
#else
extern void ProessVedioSwitch(BYTE cInPort, unsigned int i, PBYTE acOutPortList);
#endif
#ifndef SWITCH_CMD_COTINUE
extern void ProessIRSwitch(BYTE cInPort, BYTE cOutPort, SWITCH_e eFlag);
#else
extern void ProessIRSwitch(BYTE cInPort, unsigned int i, PBYTE pOutPortList);
#endif
#ifndef SWITCH_CMD_COTINUE
extern void ProessVedioIRSwitch(BYTE cInPort, BYTE cOutPort, SWITCH_e eFlag);
#else
extern void ProessVedioIRSwitch(BYTE cInPort, unsigned int i, PBYTE acOutPortList);
#endif
extern uint8_t ProCECCommand(uint8_t cPort, uint8_t opCode, uint8_t i, uint8_t ArgCnt, uint8_t* pArg);
extern uint8_t ProCECCommand2(uint8_t cPort, uint8_t cecaddr,uint8_t opCode, uint8_t i, uint8_t ArgCnt, uint8_t* pArg);
#ifdef PRD_POWER_ONOFF_MESSAGE
extern void ProcessSendData2Remote(BYTE cRemote, BYTE cBraud, unsigned char* puart_data_temp, unsigned int count, BYTE cbPowrON);
#endif
#ifdef ENABLE_DB9_BAUDRATE_SWITCH
extern void Function_SetPcUartBaudrate(unsigned char tbaudrate);
#endif
extern void ProRecallFactoryDefault(void);
#ifdef PRD_POWER_ONOFF_MESSAGE
extern uint8_t ProQueryPortPWONSendData(unsigned char HPnum1);
extern uint8_t ProQueryPortPWOFFSendData(unsigned char HPnum1);
#endif
extern void ProQuerySystermStatus(unsigned char HPnum1);
extern void ProcessAllThrough(void);
extern void ProcessAllClose(void);
extern void ProcessAllOpen(BOOL Re_Message);
extern void ProcessOne2All(unsigned char InPut);
extern void ProcessOne2One(unsigned char InPut, unsigned char OutPut);
extern void ProcessOneClose(unsigned char OutPut);
extern void ProcessOneOpen(unsigned char OutPut, BOOL Re_Message);
extern void ProcessSaveCurrentStatus(unsigned char InPut);
extern void ProcessRecallStatus(unsigned char InPut);
extern void ProcessClearStatus(unsigned char InPut);
extern void appUIUartMessageSwitcherPortSet(BYTE cOutPort);
extern uint8_t ProcessLockOneChannel(unsigned char InPut);
extern uint8_t ProcessUnlockOneChannel(unsigned char InPut);
extern void ProcessLockAllChannel(void);
extern void ProcessUnlockAllChannel(void);
extern void ProCheckAllLockStatus(void);
extern void ProSettingDemoStatus(void);
extern void ProChannelHdcp(BOOL bIsInport, BOOL bAllPort, BYTE cPort, BYTE cEnalbe);
extern void ProFactoryDefaultEDID(void);
extern uint8_t ProGetOutportEDIDData(unsigned char HPnum1);
extern void ProGetEmbeddingEDIDData(unsigned char HPnum1);
extern void ProGetInportEDIDData(unsigned char HPnum1);
extern void ProCopyEDIDDataOutport2Inport(unsigned char HPnum1, unsigned char HPnum2);
extern uint8_t ProCopyEDIDDataOutport2Inport_AudioPCM(unsigned char HPnum1, unsigned char HPnum2);
extern void ProModifyEDIDDataInport_AudioPCM(unsigned char HPnum2);
extern void ProConfigHDCPMode(unsigned char HPnum1);
extern void ProConfigIRCarrierMode(unsigned char HPnum1);
extern uint8_t ProDigitAudioON(unsigned char HPnum1);
extern uint8_t ProDigitAudioOFF(unsigned char HPnum1);
extern void ProQueryAudioVedioStatus(unsigned char cNum);
extern void ProQuerySystermStatus(unsigned char HPnum1);
extern void Function_QueryBaudrate(void);

extern void edid_info(char *fmt, ...);
extern void device_slect_9396(BYTE deviceID);

BYTE Function_M21452Read(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char  a1=0,a2=0,i=0,DATA[256]={0};
	uint8_t  *pp=NULL,devnum=0,value=0;
	enum sii_scramble_state state;
    //USART1_rxtailpoint = USART1_rxtailpoint;
    //USART1_rxprocesspoint = USART1_rxprocesspoint;

	pp = StrCmp(USART1_rxtailpoint, "M21452Read", USART1_rxprocesspoint);
	#if 0
    if (pp != NULL)
    {
	    devnum= *pp-'0';
	    if((devnum==0)||(devnum>=MAX_OUT+1))   return (CH_ERROR);
	    pp++;
	    if(*pp!=':')   return (CH_ERROR);
	    pp++;
	    value= *pp-'0';
	    if((value==1)||(value==0))
	    {
	    	    appSystemSwitcherOutportCloseAll(); // video and IR
	    	    device_slect_9396(devnum+3);
		    if(value==1)
		    {
				a1=0x01;
				dvDeviceBusWrite(  eI2CBus9396_OUT, 0xa8, 0x02 , 1,&a1 , 0);
				a1=0x03;
	    			dvDeviceBusWrite(  eI2CBus9396_OUT, 0xa8, 0x20 , 1,&a1 , 0);
				a1=0x01;
				dvDeviceBusWrite(  eI2CBus9396_OUT, 0xa8, 0x30 , 1,&a1 , 0);
		    }
		    else
		    {
				a1=0x01;
				dvDeviceBusWrite(  eI2CBus9396_OUT, 0xa8, 0x02 , 1,&a1 , 0);
				a1=0x00;
	    			dvDeviceBusWrite(  eI2CBus9396_OUT, 0xa8, 0x20 , 1,&a1 , 0);
				a1=0x00;
				dvDeviceBusWrite(  eI2CBus9396_OUT, 0xa8, 0x30 , 1,&a1 , 0);
		    }
		    delay_nms(30);
		    appSystemSwitcherOutportRecoverAll(); // video and IR
		    delay_nms(100);
		    dvDeviceBusRead(  eI2CBus9396_OUT, 0xa8, 0x20 , 1,&a1 , 0);
		    dvDeviceBusRead(  eI2CBus9396_OUT, 0xa8, 0x40 , 1,&a2 , 0);
		    info("TMDS_Bit_Clock_Ratio: %X  Clock_ Detected: %X", a1, a2);
	    }
    }
    #endif

    #if 1
    if (pp != NULL)
    {
	    devnum= *pp-'0';
	    if((devnum==0)||(devnum>=MAX_OUT+1))   return (CH_ERROR);
	    pp++;
	    if(*pp!=':')   return (CH_ERROR);
	    pp++;
	    value= *pp-'0';
	    //if((value==1)||(value==4)||(value==2)||(value==3))
	    if((value>0)&&(value<=MAX_IN))
	    {
	    	    //appSystemSwitcherOutportCloseAll(); // video and IR
	    	    //device_slect_9396(devnum+3);
		    SiiRxScrambleFullGet(value-1, &state);
		    if(state==SII_SCRAMBLE_ON_ABOVE_340M)
		    {
				info("SII_SCRAMBLE_ON_ABOVE_340M");
		    }
		    else if(state==SII_SCRAMBLE_ON_BELOW_340M)
		    {
				info("SII_SCRAMBLE_ON_BELOW_340M");
		    }
		    else if(state==SII_SCRAMBLE_OFF)
		    {
				info("SII_SCRAMBLE_OFF");
		    }
		    else
		    {
				info("other value########");
		    }
    		    SiiTxScrambleFullSet(devnum+MAX_OUT-1, state);
		    //delay_nms(30);
		    //appSystemSwitcherOutportRecoverAll(); // video and IR
		    delay_nms(100);
		    //dvDeviceBusRead(  eI2CBus9396_OUT, 0xa8, 0x20 , 1,&a1 , 0);
		    //dvDeviceBusRead(  eI2CBus9396_OUT, 0xa8, 0x40 , 1,&a2 , 0);
		    //info("TMDS_Bit_Clock_Ratio: %X  Clock_ Detected: %X", a1, a2);
	    }
    }
    #endif


    #if 0
    dvM21452Write(0, 0x03, 0x55);
    dvM21452Write(1, 0x05, 0x15);
    a1=dvM21452Read(0,  0x03 );
    a2=dvM21452Read(1,  0x05 );
    info("U4_M21452: %X  U5_M21452: %X", a1, a2);
    #endif
    //a1=0x03;
    //dvDeviceBusWrite(  eI2CBusreadEDID, 0xa8, 0x20 , 1,&a1 , 0);
	#if 0
	for (i = 0; i < SII_EDID_BLOCK_SIZE; i++)
	{
		if (i % 8 == 0)
			edid_info("\r\n");
		edid_info("%02x ", DATA[i]);
	}
	for (i = 0; i < SII_EDID_BLOCK_SIZE; i++)
	{
		if (i % 8 == 0)
			edid_info("\r\n");
		edid_info("%02x ", DATA[i+128]);
	}
	#endif

    return (CH_CORRECT);
}

BYTE Function_HDMISelect(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char  a1=0,a2=0,i=0,DATA[256]={0};
	uint8_t  *pp=NULL,devnum=0,value=0;
	enum sii_scramble_state state;
    //USART1_rxtailpoint = USART1_rxtailpoint;
    //USART1_rxprocesspoint = USART1_rxprocesspoint;

	pp = StrCmp(USART1_rxtailpoint, "HDMISelect", USART1_rxprocesspoint);

    #if 1
    if (pp != NULL)
    {
	    devnum= *pp-'0';
	    if((devnum==0)||(devnum>=MAX_OUT+1))   return (CH_ERROR);
	    pp++;
	    if(*pp!=':')   return (CH_ERROR);
	    pp++;
	    value= *pp-'0';
	    if((value==0)||(value==1)||(value==2))
	    {
			state = value;
			SiiTxScrambleFullSet(devnum+MAX_OUT-1, state);
			if(state==SII_SCRAMBLE_ON_ABOVE_340M)
		       {
				info("SII_SCRAMBLE_ON_ABOVE_340M");
		       }
		       else if(state==SII_SCRAMBLE_ON_BELOW_340M)
		       {
				info("SII_SCRAMBLE_ON_BELOW_340M");
		       }
		       else if(state==SII_SCRAMBLE_OFF)
		       {
				info("SII_SCRAMBLE_OFF");
		       }
	    		delay_nms(100);
	    }
    }
    #endif

    return (CH_CORRECT);
}

BYTE Function_HDMIDetect(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char  a1=0,a2=0,i=0,DATA[256]={0};
	uint8_t  *pp=NULL,devnum=0,value=0;
	enum sii_scramble_state state;
    //USART1_rxtailpoint = USART1_rxtailpoint;
    //USART1_rxprocesspoint = USART1_rxprocesspoint;

	pp = StrCmp(USART1_rxtailpoint, "HDMIDetect", USART1_rxprocesspoint);

    #if 1
    if (pp != NULL)
    {
	    devnum= *pp-'0';
	    if((devnum==0)||(devnum>=MAX_IN+1))   return (CH_ERROR);
	    SiiRxScrambleFullGet(devnum-1, &state);
	    if(state==SII_SCRAMBLE_ON_ABOVE_340M)
	    {
			info("SII_SCRAMBLE_ON_ABOVE_340M");
	    }
	    else if(state==SII_SCRAMBLE_ON_BELOW_340M)
	    {
			info("SII_SCRAMBLE_ON_BELOW_340M");
	    }
	    else if(state==SII_SCRAMBLE_OFF)
	    {
			info("SII_SCRAMBLE_OFF");
	    }
	    else
	    {
			info("other value########");
	    }
    }
    #endif

    return (CH_CORRECT);
}


BYTE Function_setTx5vOn(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setTx5vOn", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			Output5VConctrol(devnum-1,0);
	        delay_nms(200);
	        Output5VConctrol(devnum-1,1);
            //#if UART1_DEBUG
			sprintf(USART1_tx_buff, "setTx5vOn\r\n");
	        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
            //#endif
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_GetChipID(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    uint16_t chip_id = 0;
    uint8_t  *pp=NULL,devnum=0;

    pp = StrCmp(USART1_rxtailpoint, "GetChipID", USART1_rxprocesspoint);
    if (pp != NULL)
    {
           while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
		    SiiChipIdGet(devnum-1, &chip_id);
		    if (chip_id != CHIP_ID) {
			info("Error: Chip ID not match, require Chip ID: 0x%x\n", CHIP_ID);
		    } else {
			info("Chip ID: SiI%x\n", chip_id);
		    }
	    }
    }

    return (CH_CORRECT);
}

BYTE Function_GetBoot(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	enum sii_boot_status boot_status;
	uint16_t chip_id = 0;
	uint8_t fw_version[32];
	uint8_t  *pp=NULL,devnum=0;
	info("EVT: Boot Done\n");

	pp = StrCmp(USART1_rxtailpoint, "GetBoot", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			/* Add user callback for BOOT_DONE event*/
			sii_get_boot_status(devnum-1,&boot_status);
			if (SII_BOOT_STAT__SUCCESS != boot_status) {
				info("Error: Boot Failed\n");
				return 0;
			}
			else if (SII_BOOT_STAT__SUCCESS == boot_status) {
				info("Boot successful....\n");
			}
			sii_get_chip_id(devnum-1,&chip_id);
			if (chip_id != CHIP_ID) {
				info("Error: Chip ID not match, require Chip ID: 0x%x\n", CHIP_ID);
				return 0;
			} else {
				info("Chip ID: SiI%x\n", chip_id);
			}
			sii_get_firmware_version(devnum-1,fw_version);
			if (strcmp(fw_version, RX_FW_VER)) {
				info("Error: Chip FW Version not match.\n");
				return 0;
			}
	    }
    	}
	return (CH_CORRECT);
}

BYTE Function_GetRxResolution(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	struct sii_video_info video_info;
	uint8_t frame_rate = 0;
	uint8_t  *pp=NULL,devnum=0;
	pp = StrCmp(USART1_rxtailpoint, "GetRxResolution", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			SiiRxVideoInfoGet(devnum-1,&video_info);
			if (video_info.v_res_total != 0 && video_info.h_res_total != 0)
			{
				frame_rate =
					(video_info.tmds_clock * 1000000 / video_info.color_depth * 8) /
					(video_info.h_res_total * video_info.v_res_total);
				frame_rate = (uint8_t)(frame_rate / 5.0 + 0.5f) * 5;
				if (video_info.color_space == SII_COLOR_SPACE__YCBCR420)
					frame_rate *= 2;
			}
			info("\tTotal Resolution: %d x % d", video_info.h_res_total,video_info.v_res_total);

			sprintf(USART1_tx_buff, "Total Resolution: %d x % d\r\n", video_info.h_res_total,video_info.v_res_total);
	        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

			if (video_info.interlace)
			{
				info(" I\n");
				sprintf(USART1_tx_buff, " I\r\n");
            }
			else
			{
				info(" P\n");
				sprintf(USART1_tx_buff, " P\r\n");
			}

			UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

			info("Active Resolution: %d x % d", video_info.h_res_active,video_info.v_res_active);

			sprintf(USART1_tx_buff, "Active Resolution: %d x % d", video_info.h_res_active,video_info.v_res_active);
	        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

			if (video_info.interlace)
			{
				info(" I\n");
				sprintf(USART1_tx_buff, " I\r\n");
            }
			else
			{
				info(" P\n");
				sprintf(USART1_tx_buff, " P\r\n");
			}

			UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

			info("%dHz\t", frame_rate);

			sprintf(USART1_tx_buff, "%dHz\r\n", frame_rate);
            UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

			info("%d bits\t", video_info.color_depth);

			sprintf(USART1_tx_buff, "%d bits\r\n", video_info.color_depth);
            UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

			switch (video_info.color_space) {
				case SII_COLOR_SPACE__RGB:
					info("RGB\n");

					sprintf(USART1_tx_buff, "RGB\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

					break;
				case SII_COLOR_SPACE__YCBCR444:
					info("444\n");

					sprintf(USART1_tx_buff, "444\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

					break;
				case SII_COLOR_SPACE__YCBCR422:
					info("422\n");

					sprintf(USART1_tx_buff, "422\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

					break;
				case SII_COLOR_SPACE__YCBCR420:
					info("420\n");
					sprintf(USART1_tx_buff, "420\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
					break;
				case SII_COLOR_SPACE__UNKNOWN:
				default:
					info("Color Space Unknown\n");

					sprintf(USART1_tx_buff, "Color Space Unknown\r\n");
                    UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

					break;
			}
			info("\tTMDS Clock: %d MHz\t", video_info.tmds_clock);

            sprintf(USART1_tx_buff, "TMDS Clock: %d MHz\r\n", video_info.tmds_clock);
            UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));


			if (video_info.ratio)
			{
				info("ratio: 1/40\n");

				sprintf(USART1_tx_buff, "ratio: 1/40\r\n");
                UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

			}
			else
			{
				info("ratio: 1/10\n");

				sprintf(USART1_tx_buff, "ratio: 1/10\r\n");
                UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

			}
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_GetRxEDID(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t edid_block1_buffer[128]={0};
	uint8_t edid_block2_buffer[128]={0};
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "GetRxEDID", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_get_rx_edid_content(devnum-1,0, edid_block1_buffer);
			sii_get_rx_edid_content(devnum-1,1, edid_block2_buffer);
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_SetRxEDID(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t edid_block1_buffer[128]={0};
	uint8_t edid_block2_buffer[128]={0};
	uint8_t i = 0;
	uint8_t  *pp=NULL,devnum=0;
	for(i=0;i<128;i++)	edid_block1_buffer[i]=0;//gEdidTable[i];
	for(i=0;i<128;i++)	edid_block2_buffer[i]=0;//gEdidTable[i+128];

	pp = StrCmp(USART1_rxtailpoint, "SetRxEDID", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_edid_content(devnum-1,0, edid_block1_buffer);
			sii_set_rx_edid_content(devnum-1,1, edid_block2_buffer);
			//delay_nms(2000);
			//sii_set_rx_hpd(devnum-1,0);
			//delay_nms(200);
			//sii_set_rx_hpd(devnum-1,1);
			info("SetRxEDID");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setTxTmdsOn(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setTxTmdsOn", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			SiiTxTmdsEnableSet(devnum-1, 1);
			info("setTxTmdsOn");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setTxTmdsOff(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setTxTmdsOff", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			SiiTxTmdsEnableSet(devnum-1, 0);
			info("setTxTmdsOff");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_SetRxHPD(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "SetRxHPD", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_hpd(devnum-1,0);
			delay_nms(200);
			sii_set_rx_hpd(devnum-1,1);
			info("SetRxHPD low->high");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getRxHPD(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getRxHPD", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			uint8_t hpd;
			sii_get_rx_hpd(devnum-1,&hpd);
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_SetHPDhigh(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "SetHPDhigh", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_hpd(devnum-1,1);
			info("SetRxHPDhigh");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_SetRxHdcpON(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "SetRxHdcpON", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_hdcp_ddc(devnum-1, 1);
			info("SetRxHdcpON");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_SetRxHdcpOFF(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "SetRxHdcpOFF", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_hdcp_ddc(devnum-1, 0);
			info("SetRxHdcpOFF");
	    }
    	}

	return (CH_CORRECT);
}


BYTE Function_SetHPDlow(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "SetHPDlow", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_hpd(devnum-1,0);
			info("SetRxHPDlow");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getTxHPD(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getTxHPD", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			uint8_t hpd;
			sii_get_tx_hpd(devnum-1,&hpd);

			sprintf(USART1_tx_buff, "tx= %d  hpd= %d\r\n", devnum,hpd);
	        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setTxAvMute(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setTxAvMute", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_tx_av_mute(devnum-1,1);
			info("setTxAvMute");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setTxAvUnMute(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setTxAvUnMute", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_tx_av_mute(devnum-1,0);
			info("setTxAvUnMute");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getRxAvMute(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getRxAvMute", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			uint8_t status;
			SiiRxAvMuteGet(devnum-1,&status);
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getRxHdcp(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getRxHdcp", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			enum sii_hdcp_version hdcp_version;
			enum sii_hdcp_status hdcp_status;
			info("enter SII_EVENT__RX_HDCP_STATUS_CHNG..........\n");
			info("EVT: RX HDCP Status Change\n");
			SiiRxHdcpUsVerGet(devnum-1, &hdcp_version);
			switch(hdcp_version) {
			case SII_HDCP_VERSION__NONE:
				info("RX HDCP Version is None\n");
				break;
			case SII_HDCP_VERSION__1X:
				info("RX HDCP Version is 1.4\n");
				break;
			case SII_HDCP_VERSION__20:
				info("RX HDCP Version is 2.0\n");
				break;
			case SII_HDCP_VERSION__22:
				info("RX HDCP Version is 2.2\n");
				break;
			default:
				break;
			}
			SiiRxHdcpStatusGet(devnum-1, &hdcp_status);
			switch(hdcp_status) {
			case SII_HDCP_STATUS__OFF:
				info("RX HDCP is Off\n");
				break;
			case SII_HDCP_STATUS__SUCCESS_1X:
				info("RX HDCP 1.x is Authenticated\n");
				break;
			case SII_HDCP_STATUS__SUCCESS_22:
				info("RX HDCP 2.2 is Authenticated\n");
				break;
			case SII_HDCP_STATUS__AUTHENTICATING:
				info("RX HDCP is Starting\n");
				break;
			case SII_HDCP_STATUS__FAILED:
				info("RX HDCP is Failed\n");
				break;
			default:
				break;
			}
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getTxEDID(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getTxEDID", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			uint8_t edid_block1_buffer[128]={0};
			uint8_t edid_block2_buffer[128]={0};
			uint8_t status;
			sii_get_tx_ds_edid_status(0,&status);
			delay_nms(200);
			sii_get_tx_ds_edid_content(devnum-1,0, edid_block1_buffer);
			sii_get_tx_ds_edid_content(devnum-1,1, edid_block2_buffer);
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getHdrInfo(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getHdrInfo", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			uint8_t infoframe[SII_RX_INFOFRAME_MAX_LENGTH];
			uint8_t length;
			uint8_t i;
			info("enter SII_EVENT__HDR_REC..........\n");
			info("EVT: HDR Infoframe Received\n");
			SiiRxInfoFrameHdrGet(devnum-1, infoframe, &length);
			for (i = 0; i < length; i++) {
				info("%02x ", infoframe[i]);
			}
			info("\n");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getVsifInfo(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getVsifInfo", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			uint8_t infoframe[SII_RX_INFOFRAME_MAX_LENGTH];
			uint8_t length;
			uint8_t i;
			info("enter SII_EVENT__VSIF_REC..........\n");
			info("EVT: VSIF Infoframe Received\n");
			SiiRxInfoFrameVsifGet(devnum-1, infoframe, &length);
			for (i = 0; i < length; i++) {
				info("%02x ", infoframe[i]);
			}
			info("\n");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getRxplus5v(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getRxplus5v", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			uint8_t status;
			sii_get_rx_plus5v(devnum-1,&status);
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getRxlinktype(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getRxlinktype", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			enum sii_av_link_type type;
			info("enter SII_EVENT__RX_AV_LINK_CHNG..........\n");
			info("EVT: Rx Av Link Change\n");
			SiiRxAvLinkGet(devnum-1, &type);
			switch (type) {
			case SII_AV_LINK__NONE:
				info("Rx Link Type is None\n");
				break;
			case SII_AV_LINK__HDMI1:
				info("Rx Link Type is HDMI 1.4\n");
				break;
			case SII_AV_LINK__HDMI2:
				info("Rx Link Type is HDMI 2.0\n");
				break;
			case SII_AV_LINK__DVI:
				info("Rx Link Type is DVI\n");
				break;
			default:
				break;
			}
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setIIsIn(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setIIsIn", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			//SiiTxAudioInsertionSet(devnum-1, SII_AUDIO__I2S);
			info("AudioInsertionSet is  I2S......");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setSPDIFIn(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setSPDIFIn", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			//SiiTxAudioInsertionSet(devnum-1, SII_AUDIO__SPDIF);
			info("AudioInsertionSet is  SPDIF......");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setNONEIn(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setNONEIn", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			SiiTxAudioInsertionSet(devnum-1, SII_AUDIO__NONE);
			info("AudioInsertionSet is  NONE......");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setSPDIFOut(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setSPDIFOut", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_audio_extraction(devnum-1, SII_AUDIO__SPDIF);
			info("AudioInsertionSet is  SPDIF......");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setI2SOut(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setI2SOut", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_audio_extraction(devnum-1, SII_AUDIO__I2S);
			info("AudioInsertionSet is  I2SOut......");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setNONEOut(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setNONEOut", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_audio_extraction(devnum-1, SII_AUDIO__NONE);
			info("AudioInsertionSet is  NONEOut......");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_setTDMOut(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "setTDMOut", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			sii_set_rx_audio_extraction(devnum-1, SII_AUDIO__TDM);
			info("AudioInsertionSet is  TDMOut......");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_HDCPON(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "HDCPON", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			SiiTxHdcpProtectionSet(devnum-1, 1);
			info("HDCPON");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_HDCPOFF(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "HDCPOFF", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			SiiTxHdcpProtectionSet(devnum-1, 0);
			info("HDCPOFF");
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_getTxHdcp(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
	uint8_t  *pp=NULL,devnum=0;

	pp = StrCmp(USART1_rxtailpoint, "getTxHdcp", USART1_rxprocesspoint);
    	if (pp != NULL)
    	{
	    while (*pp != '.')
           {
                if ((*pp > '/') && (*pp < ':'))  // 0 - 9
                {
                    devnum = devnum * 10 + *pp - 0x30;
                }
		  pp++;
           }
	    if((devnum>0)&&(devnum<SII9396_DevNum+1))
	    {
			enum sii_hdcp_version hdcp_version;
			enum sii_hdcp_status hdcp_status;
			info("enter SII_EVENT__TX_HDCP_STATUS_CHNG..........\n");
			info("EVT: TX HDCP Status Change\n");
			SiiTxHdcpDsVerGet(devnum-1, &hdcp_version);
			switch(hdcp_version) {
			case SII_HDCP_VERSION__NONE:
				info("TX HDCP Version is None\n");
				break;
			case SII_HDCP_VERSION__1X:
				info("TX HDCP Version is 1.4\n");
				break;
			case SII_HDCP_VERSION__20:
				info("TX HDCP Version is 2.0\n");
				break;
			case SII_HDCP_VERSION__22:
				info("TX HDCP Version is 2.2\n");
				break;
			default:
				break;
			}
			SiiTxHdcpStatusGet(devnum-1, &hdcp_status);
			switch(hdcp_status) {
			case SII_HDCP_STATUS__OFF:
				info("TX HDCP is Off\n");
				break;
			case SII_HDCP_STATUS__SUCCESS_1X:
				info("TX HDCP 1.x is Authenticated\n");
				break;
			case SII_HDCP_STATUS__SUCCESS_22:
				info("TX HDCP 2.2 is Authenticated\n");
				break;
			case SII_HDCP_STATUS__AUTHENTICATING:
				info("TX HDCP is Starting\n");
				break;
			case SII_HDCP_STATUS__FAILED:
				info("TX HDCP is Failed\n");
				break;
			default:
				break;
			}
	    }
    	}

	return (CH_CORRECT);
}

BYTE Function_XVX(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char  *pp;
    BYTE cInPort = 0, cOutPort = 0;
    BYTE cCount=0;
    unsigned int  i = 0;

    pp = USART1_rxtailpoint+2;
    if (pp != NULL)
    {
        cInPort = *USART1_rxtailpoint-'0';
        if(cInPort == 0)
            cInPort= MAX_IN + 1;

    #ifndef SWITCH_CMD_COTINUE
        cOutPort = *pp++;
        if ((cOutPort - '0') > 0 &&
            (cOutPort - '0') <= MAX_OUT &&
            *pp == '.')
        {
            if(cInPort>MAX_IN)
                ProessVedioSwitch(cInPort-1, cOutPort, eSWITCH_OFF);
            else
                ProessVedioSwitch(cInPort-1, cOutPort, eSWITCH_ON);
            return (CH_CORRECT);
        }
    #else
        BYTE acOutPortList[MAX_OUT];
        i = 0;
        while (*pp != '.' && i < MAX_OUT)
        {
	     cCount++;
	     cOutPort = *pp;
	     pp++;
	     if(cCount%2)			//奇数
	     {
	            if ((cOutPort - '0') > 0 && (cOutPort - '0') <= MAX_OUT)
	            {
	                acOutPortList[i] = cOutPort - '0' - 1;
	                 i++;
	            }
		     else
		     {
				return (CH_ERROR);
		     }
	     }
	     else
	     {
		     if(cOutPort!='.')
		     {
				if(cOutPort!=',')
					return (CH_ERROR);
		     }

	     }
        }
        if (i > 0)
        {
            // 1V1,2,3,4.   AV:1->1 2 3 4
            ProessVedioSwitch(cInPort-1, i, acOutPortList);
            return (CH_CORRECT);
        }
    #endif
    }
    return CM_ERROR;
}


BYTE Function_XBX(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char  *pp;
    BYTE cInPort = 0, cOutPort = 0;
    unsigned int  i = 0;

    pp = USART1_rxtailpoint+2;//StrCmp(USART1_rxtailpoint, "1B", USART1_rxprocesspoint);
    if ((pp != NULL)&&(pp-USART1_rxtailpoint==2))
    {
        cInPort = *USART1_rxtailpoint-'0';
        if(cInPort==0)
            cInPort = MAX_IN+1;
    #ifndef SWITCH_CMD_COTINUE
        cOutPort = *pp++;
        if ((cOutPort - '0') > 0 &&
            (cOutPort - '0') <= MAX_OUT &&
            *pp == '.')
        {
            if(cInPort>MAX_IN)
                ProessVedioIRSwitch(cInPort-1, cOutPort, eSWITCH_OFF);
            else
                ProessVedioIRSwitch(cInPort-1, cOutPort, eSWITCH_ON);
            return (CH_CORRECT);
        }
    #else
        BYTE acOutPortList[MAX_OUT];
        i = 0;
        while (*pp != '.' && i < MAX_OUT)
        {
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT)
            {
                acOutPortList[i] = cOutPort - '0' - 1;
                i++;
            }
        }
        if (i > 0)
        {
            ProessVedioIRSwitch(cInPort-1, i, acOutPortList);
            return (CH_CORRECT);
        }
    #endif
    }
    return CM_ERROR;
}

BYTE Function_XRX(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char  *pp;
    BYTE cInPort = 0, cOutPort = 0;
    unsigned int  i = 0;

    pp = USART1_rxtailpoint+strlen("1R");//StrCmp(USART1_rxtailpoint, "1R", USART1_rxprocesspoint);
    if (pp != NULL)
    {
        cInPort = *USART1_rxtailpoint-'0';
        if(cInPort==0)
            cInPort = MAX_IN+1;
    #ifndef SWITCH_CMD_COTINUE
        cOutPort = *pp++;
        if ((cOutPort - '0') > 0 &&
            (cOutPort - '0') <= MAX_OUT &&
            *pp == '.')
        {
            if(cInPort>MAX_IN)
                ProessIRSwitch(cInPort-1, cOutPort, eSWITCH_OFF);
            else
                ProessIRSwitch(cInPort-1, cOutPort, eSWITCH_ON);
            return (CH_CORRECT);
        }
    #else
        BYTE acOutPortList[MAX_OUT];
        i = 0;
        while (*pp != '.' && i < MAX_OUT)
        {
            cOutPort = *pp++;
            if ((cOutPort - '0') > 0 &&
                (cOutPort - '0') <= MAX_OUT)
            {
                acOutPortList[i] = cOutPort - '0' - 1;
                i++;
            }
        }
        if (i > 0)
        {
            ProessIRSwitch(cInPort-1, i, acOutPortList);
            return (CH_CORRECT);
        }
    #endif
    }
    return CM_ERROR;
}

BYTE Function_CEC(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *temppoint;
    //unsigned char cmddata;
    unsigned char cPort = 0;
    unsigned char opCode = 0;
    unsigned char i = 0;
    unsigned char Arg[16]={0};
    unsigned char ArgCnt = 0;
    //BYTE   cInstance;
    //BYTE   cLocal;
    //BYTE cAdress = 0;       // wf8421 20160111

    temppoint = USART1_rxtailpoint + strlen("CEC"); //     SiiCecSendUserControlCmd();

    while (*(temppoint + i) != '.')
    {

        //cmddata = *(temppoint + i);
        if (i == 0)
        {
            cPort = *(temppoint + i);
        }
    #if 0
        else if(i == 1)
            cAdress= *(temppoint + i);
        else if(i == 2)
            opCode = *(temppoint + i);
        else
        {
            Arg[i - 3] = *(temppoint + i);
            ArgCnt++;
        }
    #else
        else if (i == 1)
            opCode = *(temppoint + i);
        else
        {
            Arg[i - 2] = *(temppoint + i);
            ArgCnt++;
        }
    #endif
        i++;
        if (temppoint + i >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            break;
    } // end while

    return ProCECCommand(cPort, opCode, i, ArgCnt, Arg);
    //return ProCECCommand2(cPort,cAdress, opCode, i, ArgCnt, Arg);
    //return CM_ERROR;
}

/*
//  CEC X:<AA:XX......>.
BYTE Function_CEC(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *temppoint;
    unsigned char cCount = 0;
    uint8_t cecop=0,Args[16]={0},Addr=0,ArgCnt=0;
    uint8_t temp[4]={0};
    uint8_t cPort = 0;
#define UserDataOffset(x)   (x)
    temppoint = USART1_rxtailpoint + 4;
    cPort = *(temppoint + UserDataOffset(0))-0x30;
    // port
#if (MACHINETYPE == MUH4K862)
    if(cPort<=0||(cPort>MAX_OUT-2)||(*(temppoint + UserDataOffset(1))!=':')||(*(temppoint + UserDataOffset(2))!='<'))
#else
    if(cPort<=0||cPort>MAX_OUT||(*(temppoint + UserDataOffset(1))!=':')||(*(temppoint + UserDataOffset(2))!='<'))
#endif
    {
        return (CM_ERROR);
    }
    // addr and opcode
    temp[0]=*(temppoint + UserDataOffset(3));temp[1]=*(temppoint + UserDataOffset(4));temp[2]=*(temppoint + UserDataOffset(6));temp[3]=*(temppoint + UserDataOffset(7));
    if((HexAndAsciiChange(*(temppoint + UserDataOffset(3)),0)==temp[0])
        ||(HexAndAsciiChange(*(temppoint + UserDataOffset(4)),0)==temp[1])
        ||(HexAndAsciiChange(*(temppoint + UserDataOffset(6)),0)==temp[2])
        ||(HexAndAsciiChange(*(temppoint + UserDataOffset(7)),0)==temp[3]))
    {
        return (CM_ERROR);
    }

    Addr=(HexAndAsciiChange(*(temppoint + UserDataOffset(3)),0)<<4)|(HexAndAsciiChange(*(temppoint + UserDataOffset(4)),0));
    cecop=(HexAndAsciiChange(*(temppoint + UserDataOffset(6)),0)<<4)|(HexAndAsciiChange(*(temppoint + UserDataOffset(7)),0));
    i=0;
    while((*(temppoint + UserDataOffset(8)+i)!='>')&&ArgCnt<16)
    {
        if(*(temppoint + UserDataOffset(8)+i)==':')
        {
            temp[0]=*(temppoint + UserDataOffset(9)+i);temp[1]=*(temppoint + UserDataOffset(10)+i);
            if((HexAndAsciiChange(*(temppoint + UserDataOffset(9)+i),0)==temp[0])
                ||(HexAndAsciiChange(*(temppoint + UserDataOffset(10)+i),0)==temp[1]))
            {
                return (CM_ERROR);
            }
            Args[ArgCnt++]=(HexAndAsciiChange(*(temppoint + UserDataOffset(9)+i),0)<<4)|(HexAndAsciiChange(*(temppoint + UserDataOffset(10)+i),0));
        }
        i++;
    }

    //sprintf(USART1_tx_buff, "CEC %d:<%02d:%02d",cPort);
    //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));

#if 1
    cCount += SiiSprintf((char*)&USART1_tx_buff[cCount], "%s%d:<%02x:%02x","CEC ",cPort,Addr,cecop);
    if(ArgCnt>0)
    {
        cCount +=SiiSprintf((char*)&USART1_tx_buff[cCount], ":");
        for(i=0;i<ArgCnt;i++)
        {
            cCount +=SiiSprintf((char*)&USART1_tx_buff[cCount], "%02x",Args[i]);
            if((i+1)<ArgCnt)
                cCount +=SiiSprintf((char*)&USART1_tx_buff[cCount], ":");
        }
    }
    cCount +=SiiSprintf((char*)&USART1_tx_buff[cCount], ">");

    UART1_SendData2PC((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
#endif

    ProCECCommand2(cPort, Addr, cecop,3, ArgCnt,Args);

#undef UserDataOffset(x)
    return CH_CORRECT;
}

*/

BYTE Function_RemoteControl(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char  *pp;
    //unsigned char cInPort = 0, cOutPort = 0;
    unsigned int  /*i = 0,*/count=0;
    unsigned char uart_data_temp[USART1_RX_BUFF_MAX] = { 0 };

    pp = USART1_rxtailpoint+2;
    if (pp != NULL)
    {
        BYTE cBraud, cRemote;
        cRemote = *pp; // Y
        if ((cRemote > '0') && (cRemote <=( '0'+MAX_OUT+1))) //if  MAX_OUT+1 send All port
        {
            cRemote -= 0x30;
            pp++;
            if (*pp != '/')
                return (CM_ERROR);
            pp++;
            cBraud = *pp - 0x30;  // X
            if ((cBraud > 0) && (cBraud <= 7))  // 1--2400；2--4800；3--9600；4--19200；5--38400；6--57600；7--115200
            {
                pp++;
                if (*pp != ':')
                    return (CM_ERROR);
                pp++;
                count = 0;
                //尾指针减1
                if (USART1_rxprocesspoint > USART1_RX_BUFF)
                    USART1_rxprocesspoint--;
                else
                    USART1_rxprocesspoint = (USART1_RX_BUFF + USART1_RX_BUFF_MAX - 1);
                while (pp != USART1_rxprocesspoint)
                {
                    uart_data_temp[count] = *pp;
                    count++;
                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;
                }

#if(MACHINETYPE!=PRD_VHD4)
                UART1_SendData2PC(uart_data_temp, count);
#endif

                dvCpldSendDataToRemote(cRemote,
                                       cBraud,
                                       uart_data_temp,
                                       count);

                return (CH_CORRECT);
            }
            else
                return (CM_ERROR);
        }
#ifdef PRD_POWER_ONOFF_MESSAGE
        else if ((cRemote >= 'a') && (cRemote <= 'a'+MAX_OUT-1) || // A,B,C,D,时表示设定在PWON时往对应的远端发送的数据；
                 (cRemote >= 'A') && (cRemote <= 'A'+MAX_OUT-1))  //  E,F,G,H  时表示设定在PWOFF时往对应的1、2、3、4路远端发送的数据?
        {
            BYTE cbPowrON;
            if((cRemote >= 'a') && (cRemote <= 'a'+MAX_OUT-1))
            {
                cbPowrON = FALSE;
                cRemote -= 'a';
            }
            else if((cRemote >= 'A') && (cRemote <= 'A'+MAX_OUT-1))
            {
                cbPowrON = TRUE;
                cRemote -= 'A';
            }
            pp++;
            if (*pp != '/')
                return (CM_ERROR);
            pp++;
            cBraud = *pp - 0x30;
            if ((cBraud > 0) && (cBraud <= 7))// 1--2400；2--4800；3--9600；4--19200；5--38400；6--57600；7--115200
            {
                pp++;
                if (*pp != ':')
                    return (CM_ERROR);
                pp++;
                count = 0;
                //尾指针减1
                if (USART1_rxprocesspoint > USART1_RX_BUFF)
                    USART1_rxprocesspoint--;
                else
                    USART1_rxprocesspoint = (USART1_RX_BUFF + USART1_RX_BUFF_MAX - 1);
                while (pp != USART1_rxprocesspoint)
                {
                    uart_data_temp[count] = *pp;
                    count++;
                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;
                }
                ProcessSendData2Remote(cRemote, cBraud, uart_data_temp, count, cbPowrON);
                return (CH_CORRECT);
            }
            else
                return (CM_ERROR);
        }
#endif
        else
            return (CM_ERROR);
    }
    return (CM_ERROR);
}

BYTE Funciton_AllThrogh(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    ProcessAllThrough();

    return (CH_CORRECT);
}

BYTE Funciton_AllClose(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    ProcessAllClose();

    return (CH_CORRECT);
}

BYTE Funciton_AllOpen(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    ProcessAllOpen(TRUE);

    return (CH_CORRECT);
}

BYTE Funciton_XToAll(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *temppoint, *tempptr;
    unsigned char InPut = 0;
    unsigned char returnval = 0;
    unsigned char  jiucuo = 0;

    temppoint = USART1_rxprocesspoint-1;
    while (*temppoint != '.')		//用于判断All后面是不是。结束，如果不是。结束，直接反馈错误
    {
        temppoint++;
        if (temppoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            temppoint = USART1_RX_BUFF;
        jiucuo++;
        if (jiucuo > 1)
            return (CM_ERROR);
    }
    temppoint = USART1_rxtailpoint;
    while (*temppoint != 'A')                          // xAll.  [x]路输入到所有路输出。
    {
        if ((*temppoint >= '0') && (*temppoint <= '9'))  // 0 - 9
        {
            InPut = InPut * 10 + *temppoint - 0x30;
            returnval = 1;
        }
        else
        {
            returnval = 0;
            InPut = 0;
        }
        temppoint++;
        if (temppoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            temppoint = USART1_RX_BUFF;
    } // end of while(*temppoint != 'A')
    if (returnval == 0)
        return (CH_ERROR);
    if ((InPut > 0) && (InPut <= MAX_IN))  // 一路输入到所有路输出
    {
        ProcessOne2All(InPut);
        return (CH_CORRECT);
    }
    else
        return (CH_ERROR);

}

BYTE Funciton_OneThrogh(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *temppoint;
	unsigned char InPut = 0, OutPut = 0;
    unsigned char cmddata = 0;

    OutPut = 0;
    temppoint = USART1_rxtailpoint;
    while (*temppoint != '.')
    {
    cmddata = *temppoint;
    if ((cmddata > '/') && (cmddata < ':'))       // number 0 - 9
    {
        OutPut = OutPut * 10 + cmddata - 0x30;
    }
    if ((cmddata == ',') || (cmddata == '#'))
    {
        if ((OutPut > 0) && (OutPut <= MAX_OUT))
        {
            InPut = OutPut;
            ProcessOne2One(InPut, OutPut);
        }
        else
            return (CH_ERROR);
        OutPut = 0;
    }
    temppoint++;
    if (temppoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
        temppoint = USART1_RX_BUFF;
    } // end of while
    return (CH_CORRECT);
}

BYTE Funciton_OneClose(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *temppoint;
	unsigned char OutPut = 0;
    unsigned char cmddata = 0;

    OutPut = 0;
    temppoint = USART1_rxtailpoint;
    while (*temppoint != '.')
    {
        cmddata = *temppoint;
        if ((cmddata > '/') && (cmddata < ':'))       // number 0 - 9
        {
            OutPut = OutPut * 10 + cmddata - 0x30;
        }
        if ((cmddata == ',') || (cmddata == '$'))
        {
            if ((OutPut > 0) && (OutPut <= MAX_OUT))
            {
                ProcessOneClose(OutPut);
            }
            else
                return (CH_ERROR);
            OutPut = 0;
        }
        temppoint++;
        if (temppoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            temppoint = USART1_RX_BUFF;
    } // end of while
    return (CH_CORRECT);
}


BYTE Funciton_OneOpen(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *temppoint, *pp;
	unsigned char OutPut = 0;
    unsigned char cmddata = 0;
    BOOL Re_Message = TRUE;
    unsigned int  jiucuo = 0;

    pp = USART1_rxtailpoint+2;
    if (pp != NULL)      // '@': open
    {
        while (*pp != '.')
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
            jiucuo++;
            if (jiucuo >= 1)
                return (CM_ERROR);
        }
        OutPut = 0;
        temppoint = USART1_rxtailpoint;
        while (*temppoint != '.')
        {
            cmddata = *temppoint;
            if ((cmddata >= '0') && (cmddata <= '9'))       // number 0 - 9
            {
                OutPut = OutPut * 10 + cmddata - 0x30;
            }
            if ((cmddata == ',') || (cmddata == '@'))
            {
                if ((OutPut > 0) && (OutPut <= MAX_OUT))
                {
                    ProcessOneOpen(OutPut, Re_Message);
                }
                else
                    return (CH_ERROR);
                OutPut = 0;
            }
            temppoint++;
            if (temppoint >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                temppoint = USART1_RX_BUFF;
        } // end of while
        return (CH_CORRECT);
    }

    return (CH_ERROR);
}

BYTE Funciton_SaveScene(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *tempptr;
	unsigned char InPut = 0;
    unsigned int i=0, jiucuo = 0;

    tempptr = USART1_rxtailpoint+4;
    if (tempptr != NULL)
    {
        while (*tempptr != '.')
        {
            if ((*tempptr > '/') && (*tempptr < ':')) // number 0 - 9
            {
                InPut = InPut * 10 + *tempptr - 0x30;
                i = 0x55;
            }

            tempptr++;
            if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                tempptr = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 4)
                return (CM_ERROR); //最多支持3位数据位
        }

        if ((InPut > 0)&&(InPut <= 10) && (i == 0x55))
        {
				ProcessSaveCurrentStatus(InPut);
        }
        else
            return (CM_ERROR); //最多支持3位数据位
        return (CH_CORRECT);
    }
    return (CM_ERROR); //最多支持3位数据位
}

BYTE Funciton_RecallScene(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *tempptr;
	unsigned char InPut = 0;
    unsigned int i=0, jiucuo = 0;

    tempptr = USART1_rxtailpoint+6;
    if (tempptr != NULL)
    {
        while (*tempptr != '.')
        {
            if ((*tempptr > '/') && (*tempptr < ':')) // number 0 - 9
            {
                InPut = InPut * 10 + *tempptr - 0x30;
                i = 0x55;
            }

            tempptr++;
            if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                tempptr = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 4)
                return (CM_ERROR); //最多支持3位数据位
        }

        if ((InPut > 0)&&(InPut <= 10) && (i == 0x55))
        {

            ProcessRecallStatus(InPut);
        }
        else
            return (CM_ERROR); //最多支持3位数据位
        return (CH_CORRECT);
    }
    return (CM_ERROR); //最多支持3位数据位
}

BYTE Funciton_ClearScene(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *tempptr;
	unsigned char InPut = 0;
    unsigned int i=0, jiucuo = 0;

    tempptr = USART1_rxtailpoint+5;
    if (tempptr != NULL)
    {
        while (*tempptr != '.')
        {
            if ((*tempptr > '/') && (*tempptr < ':')) // number 0 - 9
            {
                InPut = InPut * 10 + *tempptr - 0x30;
                i = 0x55;
            }

            tempptr++;
            if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                tempptr = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 4)
                return (CM_ERROR); //最多支持3位数据位
        }

        if ((InPut > 0)&&(InPut <= 10) && (i == 0x55))
        {
			ProcessClearStatus(InPut);
        }
        else
            return (CM_ERROR); //最多支持3位数据位
        return (CH_CORRECT);
    }
    return (CM_ERROR); //最多支持3位数据位
}

BYTE Funciton_Status(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *tempptr;
	unsigned char OutPut = 0;
    //unsigned int i=0;

    tempptr = USART1_rxtailpoint+6;
    if (tempptr != NULL)
    {
        OutPut = *(tempptr);
        if ((OutPut > 0x30) && (OutPut < (MAX_OUT + 0x31)))             // 查看一个通道状态
        {
            OutPut = OutPut - 0x30; // ASCII to Digital //[kyq add: 20150825 13:41:52]
            appUIUartMessageSwitcherPortSet(OutPut - 1);
            return (CH_CORRECT);
        }
        else if (*tempptr == '.')                                 // 查看全部通道状态
        {
            appUIUartMessageSwitcherPortSet(MAX_OUT);
            return (CH_CORRECT);
        }
        else
            return (CH_ERROR);
    }

    return (CM_ERROR); //最多支持3位数据位
}

BYTE Funciton_LockOneChanel(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *tempptr;
	unsigned char InPut = 0;
    unsigned int  jiucuo = 0;

    tempptr = USART1_rxtailpoint+6;
    if (tempptr != NULL)
    {
            while (*tempptr != '.')
            {
                if ((*tempptr > '0') && (*tempptr <= ('0'+MAX_IN))) // number 0 - 9
                {
                    InPut =  *tempptr - 0x30;
                }

                tempptr++;
                if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    tempptr = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 1)
                    return (CM_ERROR); //最多支持1位数据位
            }

            return (ProcessLockOneChannel(InPut));
    }

    return (CM_ERROR); //最多支持3位数据位
}

BYTE Funciton_UnLockOneChanel(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *tempptr;
	unsigned char InPut = 0;
    unsigned int jiucuo = 0;

    tempptr = USART1_rxtailpoint+8;
    if (tempptr != NULL)
    {
        while (*tempptr != '.')
    	{
             if ((*tempptr > '0') && (*tempptr <= ('0'+MAX_IN))) // number 0 - 9
            {
                InPut =  *tempptr - 0x30;
            }

            tempptr++;
            if (tempptr >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                tempptr = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 4)
                return (CM_ERROR); //最多支持3位数据位
        }
		return (ProcessUnlockOneChannel(InPut));
    }

    return (CM_ERROR); //最多支持3位数据位
}

BYTE Funciton_LockAllChanel(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    ProcessLockAllChannel();
    return (CH_CORRECT); //最多支持3位数据位
}

BYTE Funciton_UnLockAllChanel(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    ProcessUnlockAllChannel();
    return (CH_CORRECT); //最多支持3位数据位
}

BYTE Funciton_LockStatus(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    ProCheckAllLockStatus();
    return (CH_CORRECT); //最多支持3位数据位
}

BYTE Funciton_Demo(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    ProSettingDemoStatus();
    return (CH_CORRECT); //最多支持3位数据位
}

BYTE Funciton_Undo(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char cCount=0;
    BYTE acMessage2[LCD_MAX_CHAR] = { 0 };

    if (*USART1_rxtailpoint != 'U')
    {
        return (CH_ERROR);
    }
    //SW_Undo();
    cCount += SiiSprintf((char*)&acMessage2[cCount], "%s","Undo Ok!");
    appUIUartMessageSet(NULL, acMessage2, TRUE);
    appUIStateChange(uiUART_MESSAGE);

    appSystemSwitcherUndo();
    return (CH_CORRECT);
}

BYTE Funciton_Standby(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char cCount=0;
    BYTE acMessage2[LCD_MAX_CHAR] = { 0 };

    cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "STANDBY...");
    appUIUartMessageSet(NULL, acMessage2, FALSE);
    appUIStateChange(uiUART_MESSAGE);
    appPowerMgrPowerStateSet(pwrSTANDBY);
    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    KeyLedSetAllClose();
    #endif
    return (CH_CORRECT);
}

BYTE Funciton_PowerOff(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char cCount=0;
    BYTE acMessage2[LCD_MAX_CHAR] = { 0 };

    cCount += SiiSprintf((char*)&acMessage2[cCount],"%s", "PWOFF");
    appUIUartMessageSet(NULL, acMessage2, FALSE);
    appUIStateChange(uiUART_MESSAGE);
    appPowerMgrPowerStateSet(pwrSOFTOFF);
    #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
    KeyLedSetAllClose();
    #endif
    return (CH_CORRECT);
}

BYTE Funciton_HdcpManage(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    BYTE acMessage2[LCD_MAX_CHAR] = { 0 };
    unsigned int jiucuo=0;

    pp = USART1_rxtailpoint+2;			//StrCmp(USART1_rxtailpoint, "/%", USART1_rxprocesspoint);
    if (pp != NULL)   // 开启HDCP
    {
        BYTE  cPort = 0;
        BOOL bIsInport = 0;
        BYTE cEnalbe = 0;
        BOOL bAllPort = 0;

        //这里开始真正处理
        pp = StrCmp(USART1_rxtailpoint, "/%", USART1_rxprocesspoint);
        if (*pp == 'I')
        {
		return (CM_ERROR);			//兼容输入HDCP直接返回
		bIsInport = 1;
        }
        else if (*pp == 'O')
            	bIsInport = 0;
        else
            	return (CH_CORRECT);

        pp++;
        if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            pp = USART1_RX_BUFF;
        if (StrCmp(USART1_rxtailpoint, "ALL", USART1_rxprocesspoint) == NULL)			//表示对单个通道进行操作
        {
            bAllPort = 0; //表示对单个通道进行操作
                          //得出通道号
            while (*pp != ':')
            {
                // 得到端口号
                if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                {
                    cPort = cPort * 10 + *pp - 0x30;
                }

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 4)
                    return (CM_ERROR); //最多支持3位数据位
            }

            if ((cPort == 0) || (cPort > MAX_IN))
                return (CM_ERROR);
        }
        else
        {
            // 针对所有通道
            pp = StrCmp(USART1_rxtailpoint, "ALL", USART1_rxprocesspoint);
            bAllPort = 1;
        }
        //得出HDCP状态
        pp++;
        jiucuo = 0;
        while (*pp != '.')
        {
            if ((*pp > '/') && (*pp < ':')) // number 0 - 9
            {
                cEnalbe = cEnalbe * 10 + *pp - 0x30;
            }
            else
                return (CM_ERROR);

            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 4)
                return (CM_ERROR);

            if (cEnalbe > 1)
                return (CM_ERROR);
        }
        ProChannelHdcp(bIsInport, bAllPort, cPort, cEnalbe);

        pp++;
        memcpy(acMessage2, USART1_rxtailpoint, (pp - USART1_rxtailpoint));
        appUIUartMessageSet(NULL, acMessage2, TRUE);
        appUIStateChange(uiUART_MESSAGE);

        return (CH_CORRECT);
    }
    return (CH_CORRECT);
}

BYTE Funciton_EdidInit(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    ProFactoryDefaultEDID();
    return (CH_CORRECT); //最多支持3位数据位
}

BYTE Funciton_GetOutPutPortEdid(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;
    unsigned int jiucuo=0;

    pp = USART1_rxtailpoint+5;
    if (pp != NULL)
    {
        while (*pp != '.')
        {
            if ((*pp > '/') && (*pp < ':')) // number 0 - 9
            {
                HPnum1 = HPnum1 * 10 + *pp - 0x30;
            }

            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 3)
                return (CM_ERROR); //最多支持3位数据位
        }
        return (ProGetOutportEDIDData(HPnum1));
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_GetOutPortEdid(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;
    unsigned int jiucuo=0;
    unsigned int j = 0;
    pp = USART1_rxtailpoint+5;
    if (pp != NULL)
    {
        while (*pp != '.')
        {
            if ((*pp > '/') && (*pp < ':')) // number 0 - 9
            {
                HPnum1 = HPnum1 * 10 + *pp - 0x30;
            }

            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 3)
                return (CM_ERROR); //最多支持3位数据位
        }
        if ((HPnum1 > 0) && (HPnum1 <= MAX_OUT))
        {
            for (j = 0; j < EDID_TABLE_LEN; j++) UART1_SendByte(OutputEdidData[HPnum1 - 1][j]);
            UART1_SendByte(0x0D);
            UART1_SendByte(0x0A);
            return (CH_CORRECT);
        }
        else
            return (CH_ERROR);
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_GetInternalEdid(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;

    pp = USART1_rxtailpoint+10;
    if (pp != NULL)
    {
        HPnum1 = *pp - 0x30;
        if ((HPnum1 > 0) && (HPnum1 < 6))
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
            if (*pp == '.')
            {
				ProGetEmbeddingEDIDData(HPnum1);
                return (CH_CORRECT);
            }
            else
                return (CM_ERROR);
        }
        else
            return (CM_ERROR);
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_GetInputPortEdid(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;

    pp = USART1_rxtailpoint+13;
    if (pp != NULL)
    {
        HPnum1 = *pp - 0x30;
        if ((HPnum1 > 0) && (HPnum1 < (MAX_IN + 1)))
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;

            if (*pp == '.')
            {
				ProGetInportEDIDData(HPnum1);
                return (CH_CORRECT);
            }
            else
                return (CM_ERROR);
        }
        else
            return (CM_ERROR);
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_CopyEdidFromOutPutToInput(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0,HPnum2=0;
    unsigned int jiucuo=0;

    pp = USART1_rxtailpoint+5;
    if (pp != NULL)             // EdidM 1B1
    {
        while (*pp != '.')
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
        }
        pp = USART1_rxtailpoint+5;
        if ((*pp >= '0') && (*pp <= '9'))
        {
            while (*pp != 'B')
            {
                if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                {
                    HPnum1 = HPnum1 * 10 + *pp - 0x30; // 输入端口
                }
                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 3)
                    return (CM_ERROR);

            }
            if ((HPnum1 > MAX_OUT) || (HPnum1 == 0))
                return (CM_ERROR);
            jiucuo = 0;
            pp++;
            if (StrCmp(pp, "ALL", USART1_rxprocesspoint) == NULL)
            {
                while (*pp != '.')
                {
                    if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                    {
                        HPnum2 = HPnum2 * 10 + *pp - 0x30;
                    }

                    pp++;
                    if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                        pp = USART1_RX_BUFF;

                    jiucuo++;
                    if (jiucuo > 3)
                        return (CM_ERROR);
                }
            }
            else
            {
                HPnum2 = MAX_IN+1;  //over MAX_IN is All channel
            }
            if ((HPnum2 > (MAX_IN + 1)) || (HPnum2 == 0))
                return (CM_ERROR);
            //    HPnum1:  输出口
            //	 HPnum2: 输入口
			ProCopyEDIDDataOutport2Inport(HPnum1, HPnum2);
            return (CH_CORRECT);
        }
        else
            return (CM_ERROR);
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_CopyEdidFromOutPutToInput_ForceToPcm(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0,HPnum2=0;
    unsigned int jiucuo=0;

    pp = USART1_rxtailpoint+5;
    if (pp != NULL)
    {
        //BYTE cInstance = 0;
        //BYTE acEdid[256];
        while (*pp != '.')
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
        }
        pp = USART1_rxtailpoint+5;
        if ((*pp > '/') && (*pp < ':'))
        {
            while (*pp != 'B')
            {
                if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                {
                    HPnum1 = HPnum1 * 10 + *pp - 0x30;
                }

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 3)
                    return (CM_ERROR);
            }
            if ((HPnum1 > MAX_OUT) || (HPnum1 == 0))
                return (CM_ERROR);
            jiucuo = 0;
            pp++;
            while (*pp != '.')
            {
                if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                {
                    HPnum2 = HPnum2 * 10 + *pp - 0x30;
                }

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 3)
                    return (CM_ERROR);
            }
            if ((HPnum2 > MAX_IN) || (HPnum2 == 0))
                return (CM_ERROR);
			return (ProCopyEDIDDataOutport2Inport_AudioPCM(HPnum1, HPnum2));
        }
        else
            return (CM_ERROR);
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_EdidAudioInfoForceToPcm(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum2=0;
    unsigned int jiucuo=0;

    pp = USART1_rxtailpoint+7;
    if (pp != NULL)
    {
        //BYTE acEdid[256];
        while (*pp != '.')
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
        }
        pp = USART1_rxtailpoint+7;//StrCmp(USART1_rxtailpoint, "EDIDPCM", USART1_rxprocesspoint);
        if ((*pp > '/') && (*pp < ':'))
        {
            jiucuo = 0;
            while (*pp != '.')
            {
                if ((*pp > '/') && (*pp < ':')) // number 0 - 9
                {
                    HPnum2 = HPnum2 * 10 + *pp - 0x30;
                }

                pp++;
                if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                    pp = USART1_RX_BUFF;

                jiucuo++;
                if (jiucuo > 3)
                    return (CM_ERROR);
            }
            if ((HPnum2 > MAX_IN) || (HPnum2 == 0))
                return (CM_ERROR);
			ProModifyEDIDDataInport_AudioPCM(HPnum2);
            return (CH_CORRECT);
        }
        else
            return (CM_ERROR);
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_EdidUpdate(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    //unsigned char HPnum2=0;
    //unsigned int jiucuo=0;

    pp = USART1_rxtailpoint+11;
    if (pp != NULL)
    {
        EDID_updata_port = *pp - 0x30;
        if ((EDID_updata_port > 0) && (EDID_updata_port <=(edsEDID_INTERN1-edsEDID_Default)))
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
            if (*pp == '.')
            {
		  if(eEDID_RS232_GUI_CONTROL !=appGetDialSwitchEdidStatus())
		  {
			sprintf(USART1_tx_buff, "Set RS232 or GUI control first.\r\n");
	        	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
			return (CH_CORRECT);
		  }
		  USART_data_flag = 1; //标志后续要升级EDID
                USART_EDIDdata_count = 0;
                EdidUpataStart = SiiPlatformTimerSysTicksGet();
                UART1_SendBytes("Please send the EDID file\r\n", strlen("Please send the EDID file\r\n"));
                return (CH_CORRECT);
            }
            else
                return (CM_ERROR); //gxx 140310
        }
        else
            return (CM_ERROR); //gxx 140310
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_EdidUpdateInternal(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    //unsigned char HPnum2=0;
    //unsigned int jiucuo=0;

    pp = USART1_rxtailpoint+14;
    if (pp != NULL)             // EdidM 1B1
    {
	 if(eEDID_RS232_GUI_CONTROL !=appGetDialSwitchEdidStatus())
	 {
		sprintf(USART1_tx_buff, "Set RS232 or GUI control first.\r\n");
        	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
		return (CH_CORRECT);
	 }
	 EDID_updata_port = *pp - 0x30;
        if ((EDID_updata_port > 0) && (EDID_updata_port <= (edsEDID_EXTERN1-edsEDID_INTERN1)))
        {
            USART_data_flag = 2; //标志后续要升级EDID
            USART_EDIDdata_count = 0;
            EdidUpataStart = SiiPlatformTimerSysTicksGet();
            UART1_SendBytes("Please send the EDID file\r\n", strlen("Please send the EDID file\r\n"));
            return (CH_CORRECT);
        }
        else
            return (CM_ERROR); //gxx 140310
    }

    return (CH_ERROR); //最多支持3位数据位
}

#if 0
BYTE Funciton_CopyInternalEdidToInput(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0,HPnum2=0;
    unsigned int jiucuo=0;
    unsigned char cCount=0;
    unsigned char acMessage2[LCD_MAX_CHAR]={0};
    BYTE   acEdid[256]={0};

    if(eEDID_RS232_GUI_CONTROL !=appGetDialSwitchEdidStatus())
    {
	sprintf(USART1_tx_buff, "Set RS232 or GUI control first.\r\n");
    	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
	return (CH_CORRECT);
    }

    pp = USART1_rxtailpoint+5;
    if (pp != NULL)
    {
        while (*pp != '.')
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
        }
        pp = USART1_rxtailpoint+5;//StrCmp(USART1_rxtailpoint, "EDID/", USART1_rxprocesspoint);
        while (*pp != '/')
        {
            if ((*pp >= '0') && (*pp <= '9')) // number 0 - 9
            {
                HPnum1 = HPnum1 * 10 + *pp - 0x30;
            }

            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 3)
                return (CM_ERROR);
        }
        if ((HPnum1 > MAX_IN) || (HPnum1 == 0))
            return (CM_ERROR);
        jiucuo = 0;
        pp++;
        while (*pp != '.')
        {
            if ((*pp >= '0') && (*pp <= '9')) // number 0 - 9
            {
                HPnum2 = HPnum2 * 10 + *pp - 0x30;
            }

            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 3)
                return (CM_ERROR);
        }
        if ((HPnum2 > (edsEDID_EXTERN1-edsEDID_INTERN1)) || (HPnum2 == 0))
            return (CM_ERROR);

        if (TRUE)
        {
            appSystemInputPortEdidSet(HPnum1 - 1, (eEDIDSELECT)(edsEDID_INTERN1 + HPnum2 - 1), acEdid,SINGLE_INPORT);
        }

        cCount += SiiSprintf((char*)&acMessage2[cCount], "EDID/%d/%d", HPnum1, HPnum2);
        appUIUartMessageSet(NULL, acMessage2, TRUE);
        appUIStateChange(uiUART_MESSAGE);

        return (CH_CORRECT);
    }
    return (CH_ERROR); //最多支持3位数据位
}
#endif

BYTE Funciton_CopyInternalEdidToInput(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0,HPnum2=0;
    unsigned int jiucuo=0;
    unsigned char cCount=0;
    unsigned char acMessage2[LCD_MAX_CHAR]={0};
    BYTE   acEdid[256]={0};

    if(eEDID_RS232_GUI_CONTROL !=appGetDialSwitchEdidStatus())
    {
	sprintf(USART1_tx_buff, "Set RS232 or GUI control first.\r\n");
    	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
	return (CH_CORRECT);
    }

    pp = USART1_rxtailpoint+5;
    if (pp != NULL)
    {
        while (*pp != '.')
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
        }
        pp = USART1_rxtailpoint+5;//StrCmp(USART1_rxtailpoint, "EDID/", USART1_rxprocesspoint);
        while (*pp != '/')
        {
            if ((*pp >= '0') && (*pp <= '9')) // number 0 - 9
            {
                HPnum1 = HPnum1 * 10 + *pp - 0x30;
            }

            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 3)
                return (CM_ERROR);
        }
        if ((HPnum1 > MAX_IN) || (HPnum1 == 0))
            return (CM_ERROR);
        jiucuo = 0;
        pp++;
        while (*pp != '.')
        {
            if ((*pp >= '0') && (*pp <= '9')) // number 0 - 9
            {
                HPnum2 = HPnum2 * 10 + *pp - 0x30;
            }

            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 3)
                return (CM_ERROR);
        }
        if ((HPnum2 > 11))
            return (CM_ERROR);

        if (TRUE)
        {
		SetUartInEdidValue(HPnum1-1,HPnum2);
		SiiPlatformTimerSet(etINPORTMAG_EDID,mS_TO_TICKS(2000) ,1);
        }

        cCount += SiiSprintf((char*)&acMessage2[cCount], "EDID/%d/%d", HPnum1, HPnum2);
        appUIUartMessageSet(NULL, acMessage2, TRUE);
        appUIStateChange(uiUART_MESSAGE);

        return (CH_CORRECT);
    }
    return (CH_ERROR); //最多支持3位数据位
}


BYTE Funciton_CopyInternalEdidToAllInput(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;
    unsigned int jiucuo=0;
    unsigned char cCount=0;
    unsigned char acMessage2[LCD_MAX_CHAR]={0};

    if(eEDID_RS232_GUI_CONTROL !=appGetDialSwitchEdidStatus())
    {
	sprintf(USART1_tx_buff, "Set RS232 or GUI control first.\r\n");
    	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
	return (CH_CORRECT);
    }

    pp = USART1_rxtailpoint+11;
    if (pp != NULL)
    {
        jiucuo = 0;
        while (*pp != '.')
        {
            if ((*pp >= '0') && (*pp <= '9')) // number 0 - 9
            {
                HPnum1 = HPnum1 * 10 + *pp - 0x30;
            }

            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;

            jiucuo++;
            if (jiucuo > 3)
                return (CM_ERROR);
        }
        if (HPnum1 > 11)
            return (CM_ERROR);

        if (TRUE)
        {
            InternalEdidProcess(HPnum1, 0);
        }

        cCount += SiiSprintf((char*)&acMessage2[cCount], "EDIDIN/ALL/%d", HPnum1);
        appUIUartMessageSet(NULL, acMessage2, FALSE);
        appUIStateChange(uiUART_MESSAGE);

        return (CH_CORRECT);
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_HdcpAutoManage(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;
    //unsigned int jiucuo=0;
    //unsigned char cCount=0;
    //unsigned char acMessage2[LCD_MAX_CHAR]=0;

    pp = USART1_rxtailpoint+4;
    if (pp != NULL)
    {
        if ((*pp >= '0') && (*pp < '2'))
            HPnum1 = *pp;
        else
            return (CM_ERROR);
        pp++;
        if (*pp != '.')
            return (CM_ERROR);
		ProConfigHDCPMode(HPnum1);
        return (CH_CORRECT);
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_IrCarrierMode(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;
    //unsigned int jiucuo=0;
    //unsigned char cCount=0;
    //unsigned char acMessage2[LCD_MAX_CHAR]=0;

    pp = USART1_rxtailpoint+4;
    if (pp != NULL)
    {
        if ((*pp >= '0') && (*pp < '2'))
            HPnum1 = *pp;
        else
            return (CM_ERROR);
        pp++;
        if (*pp != '.')
            return (CM_ERROR);
		ProConfigIRCarrierMode(HPnum1);
        return (CH_CORRECT);
    }
    return (CH_ERROR); //最多支持3位数据位
}

BYTE Funciton_OpenDigitalAudio(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;

    pp = USART1_rxtailpoint+strlen("DigitAudioON");
    if (pp != NULL)
    {
        HPnum1 = *pp - 0x30;
        pp++;
        if (*pp != '.')
            return (CM_ERROR);
		return (ProDigitAudioON(HPnum1));
    }
    return (CM_ERROR);
}

BYTE Funciton_CloseDigitalAudio(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;

    pp = USART1_rxtailpoint+strlen("DigitAudioOFF");
    if (pp != NULL)
    {
        HPnum1 = *pp - 0x30;
        pp++;
        if (*pp != '.')
            return (CM_ERROR);
		return (ProDigitAudioOFF(HPnum1));
    }
    return (CM_ERROR);
}

BYTE Funciton_QueryVideoAudioStatus(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    //unsigned char HPnum1=0;
    unsigned int jiucuo=0;

    pp = USART1_rxtailpoint+strlen("%997");
    if (pp != NULL)
    {
        while (*pp != '.')
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
            jiucuo++;
            if (jiucuo > 1)
                return (CM_ERROR);
        }
        pp = USART1_rxtailpoint+strlen("%997");
        #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
        if ((*pp > '0') && (*pp <= '9'))
        #else
        if ((*pp > '0') && (*pp <= '9'))
        #endif
        {
			if(*pp==8)
			{
				return (CM_ERROR);
			}
			else
			{
				ProQueryAudioVedioStatus(*pp);
			}
        }
        else
            return (CM_ERROR);
        return (CH_CORRECT);
    }
    return (CM_ERROR);
}

BYTE Funciton_QuerySystemStatus(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;

    pp = USART1_rxtailpoint+strlen("%996");
    if (pp != NULL)
    {
        if ((*pp > '0') && (*pp < '5'))
            HPnum1 = *pp;
        else
            return (CM_ERROR);
        pp++;
        if (*pp != '.')
            return (CM_ERROR);
		ProQuerySystermStatus(HPnum1);
        return (CH_CORRECT);
    }
    return (CM_ERROR);
}

#ifdef PRD_POWER_ONOFF_MESSAGE
BYTE Funciton_QueryPortPWOFFSendData(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;

    pp = USART1_rxtailpoint+strlen("%994");
    if (pp != NULL)
    {
        if ((*pp > '0') && (*pp <=( '0'+MAX_OUT)))
            HPnum1 = *pp - 0x30;
        else
  	        return (CM_ERROR);

        pp++;
        if (*pp != '.')
            return (CM_ERROR);
        return (ProQueryPortPWOFFSendData(HPnum1));
    }
    return (CM_ERROR);
}

BYTE Funciton_QueryPortPWONSendData(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    unsigned char HPnum1=0;

    pp = USART1_rxtailpoint+strlen("%995");
    if (pp != NULL)
    {
        if((*pp > '0') && (*pp <=( '0'+MAX_OUT)))
            HPnum1 = *pp - 0x30;
        else
            return (CM_ERROR);

        pp++;
        if (*pp != '.')
            return (CM_ERROR);
		return (ProQueryPortPWONSendData(HPnum1));
    }
    return (CM_ERROR);
}
#endif

BYTE Funciton_FactoryReset(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    unsigned char *pp;
    //unsigned char HPnum1=0;
    unsigned int jiucuo=0;

    pp = USART1_rxtailpoint+strlen("%0911");
    if (pp != NULL)
    {
        while (*pp != '.')
        {
            pp++;
            if (pp >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
                pp = USART1_RX_BUFF;
            jiucuo++;
            if (jiucuo >= 1)
                return (CM_ERROR);
        }
        //_ Factory_initialization();
        sprintf(USART1_tx_buff, "Factory Default\r\n");
        UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
        //sprintf(USART1_tx_buff, "Plesse Reboot System\r\n");
        //UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
        ProRecallFactoryDefault();
        // any other need to process?
        return (CH_CORRECT);
    }
    return (CM_ERROR);
}

BYTE Funciton_SiiMonUartNumberSet(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    BYTE cNumber = 0;
    unsigned char *temppoint;
    unsigned int i=0;

    temppoint = USART1_rxtailpoint + strlen("SIIMON"); //
    while (*(temppoint + i) != '.')
    {
        cNumber = *(temppoint + i) - '0';
        i++;
        if (temppoint + i >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            break;
    }

    //SkSiMonUartNumberSet(cNumber);
    return CH_CORRECT;
}

BYTE Funciton_DebugEn(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    SiiPlatformLoggerPrintEnable(TRUE);
    return CH_CORRECT;
}

BYTE Funciton_DebugColorOn(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    SiiPlatformLoggerColorEnable(TRUE);
    return CH_CORRECT;
}

BYTE Funciton_DebugColorOff(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    SiiPlatformLoggerColorEnable(FALSE);
    return CH_CORRECT;
}

BYTE Funciton_GetGitVersion(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    UART1_SendData2PC(m_acGitHash,strlen(m_acGitHash));
    return CH_CORRECT;
}

BYTE Funciton_DebugLevel(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    BYTE cNumber = 0;
    unsigned char *temppoint;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    unsigned int i = 0;

    temppoint = USART1_rxtailpoint + strlen("DEBUGLEVEL"); //     SiiCecSendUserControlCmd();
    i = 0;
    while (*(temppoint + i) != '.')
    {

        cNumber = cNumber * 10 + (*(temppoint + i) - '0');
        i++;
        if (temppoint + i >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            break;

    } // end while
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
    psSystemSettingData->cDebugLevel =  cNumber;
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
    SiiPlatformLoggerMessageLevelSet(cNumber);
    return CH_CORRECT;

}

BYTE Funciton_DebugPost(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    BYTE cNumber = 0;
    unsigned char *temppoint;
    PSYSTEM_SETTING_DATA   psSystemSettingData;
    unsigned int i = 0;

    temppoint = USART1_rxtailpoint + strlen("DEBUGPOST"); //     SiiCecSendUserControlCmd();
    i = 0;
    while (*(temppoint + i) != '.')
    {

        cNumber = cNumber * 10 + (*(temppoint + i) - '0');
        i++;
        if (temppoint + i >= (USART1_RX_BUFF + USART1_RX_BUFF_MAX))
            break;

    } // end while
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
    psSystemSettingData->cPOSTEnable =  cNumber;
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
    return CH_CORRECT;
}

#ifdef ENABLE_DB9_BAUDRATE_SWITCH
BYTE Funciton_SetDB9Baudrate2400(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    Function_SetPcUartBaudrate(1);
    return (CH_CORRECT);
}

BYTE Funciton_SetDB9Baudrate4800(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    Function_SetPcUartBaudrate(2);
    return (CH_CORRECT);
}

BYTE Funciton_SetDB9Baudrate9600(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    Function_SetPcUartBaudrate(3);
    return (CH_CORRECT);
}

BYTE Funciton_SetDB9Baudrate19200(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    Function_SetPcUartBaudrate(4);
    return (CH_CORRECT);
}

BYTE Funciton_SetDB9Baudrate38400(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    Function_SetPcUartBaudrate(5);
    return (CH_CORRECT);
}

BYTE Funciton_SetDB9Baudrate57600(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    Function_SetPcUartBaudrate(6);
    return (CH_CORRECT);
}

BYTE Funciton_SetDB9Baudrate115200(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    Function_SetPcUartBaudrate(7);
    return (CH_CORRECT);
}
#endif

BYTE Funciton_SetSystemLock(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    PSYSTEM_SETTING_DATA   psSystemSettingData;

    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
    psSystemSettingData->bSystemLock = true;
    System_UnLock_flag = psSystemSettingData->bSystemLock;
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                      TRUE);
    appUIIRKeyEnableSet(FALSE);
    appUIStateChange(uiSYSTEM_LOCK_INFO);
    return (CH_CORRECT);
}

BYTE Funciton_SetSystemUnLock(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    PSYSTEM_SETTING_DATA   psSystemSettingData;

    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);
    psSystemSettingData->bSystemLock = FALSE;
    System_UnLock_flag = psSystemSettingData->bSystemLock;
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                      TRUE);
    appUIIRKeyEnableSet(TRUE);
    appUIStateChange(uiSYSTEM_LOCK_INFO);

    return (CH_CORRECT);
}

BYTE Funciton_SetSystemModelType(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    appUIStateChange(uiUART_SYSTEM_TYPE);
    return (CH_CORRECT);
}

BYTE Funciton_SetSystemVersion(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

    appUIStateChange(uiSYSTEM_VERSION);
    return (CH_CORRECT);
}

BYTE Funciton_SetSystemTestVersion(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

	sprintf(USART1_tx_buff, "testV1.0.0\r\n");
	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
		
    return (CH_CORRECT);
}

BYTE Funciton_SetSystemTestType(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint)
{
    USART1_rxtailpoint = USART1_rxtailpoint;
    USART1_rxprocesspoint = USART1_rxprocesspoint;

	sprintf(USART1_tx_buff, "testTypeMUH44A-H2\r\n");
	UART1_SendBytes((uint8_t *)USART1_tx_buff, strlen(USART1_tx_buff));
	
    return (CH_CORRECT);
}


stCmdListType CmdListToMode1[]=
{

    {Flag_HeadMatch,    "GetChipID",       UseStrlen,  1,  Function_GetChipID},
		{Flag_HeadMatch,    "GetBoot",       UseStrlen,  1,  Function_GetBoot},
		{Flag_HeadMatch,    "GetRxResolution",       UseStrlen,  1,  Function_GetRxResolution},
		{Flag_HeadMatch,    "GetRxEDID",       UseStrlen,  1,  Function_GetRxEDID},
		{Flag_HeadMatch,    "SetRxEDID",       UseStrlen,  1,  Function_SetRxEDID},
		{Flag_HeadMatch,    "setTxTmdsOn",       UseStrlen,  1,  Function_setTxTmdsOn},
		{Flag_HeadMatch,    "setTxTmdsOff",       UseStrlen,  1,  Function_setTxTmdsOff},
		{Flag_HeadMatch,    "SetRxHPD",       UseStrlen,  1,  Function_SetRxHPD},
		{Flag_HeadMatch,    "SetRxHdcpON",       UseStrlen,  1,  Function_SetRxHdcpON},
		{Flag_HeadMatch,    "SetRxHdcpOFF",       UseStrlen,  1,  Function_SetRxHdcpOFF},
		{Flag_HeadMatch,    "getRxHPD",       UseStrlen,  1,  Function_getRxHPD},
		{Flag_HeadMatch,    "SetHPDhigh",       UseStrlen,  1,  Function_SetHPDhigh},
		{Flag_HeadMatch,    "SetHPDlow",       UseStrlen,  1,  Function_SetHPDlow},
		{Flag_HeadMatch,    "getTxHPD",       UseStrlen,  1,  Function_getTxHPD},
		{Flag_HeadMatch,    "setTxAvMute",       UseStrlen,  1,  Function_setTxAvMute},
		{Flag_HeadMatch,    "setTxAvUnMute",       UseStrlen,  1,  Function_setTxAvUnMute},
		{Flag_HeadMatch,    "getRxAvMute",       UseStrlen,  1,  Function_getRxAvMute},
		{Flag_HeadMatch,    "getRxHdcp",       UseStrlen,  1,  Function_getRxHdcp},
		{Flag_HeadMatch,    "getTxEDID",       UseStrlen,  1,  Function_getTxEDID},
		{Flag_HeadMatch,    "getHdrInfo",       UseStrlen,  1,  Function_getHdrInfo},
		{Flag_HeadMatch,    "getVsifInfo",       UseStrlen,  1,  Function_getVsifInfo},
		{Flag_HeadMatch,    "getRxplus5v",       UseStrlen,  1,  Function_getRxplus5v},
		{Flag_HeadMatch,    "getRxlinktype",       UseStrlen,  1,  Function_getRxlinktype},
		{Flag_HeadMatch,    "setIIsIn",       UseStrlen,  1,  Function_setIIsIn},
		{Flag_HeadMatch,    "setSPDIFIn",       UseStrlen,  1,  Function_setSPDIFIn},
		{Flag_HeadMatch,    "setNONEIn",       UseStrlen,  1,  Function_setNONEIn},
		{Flag_HeadMatch,    "setSPDIFOut",       UseStrlen,  1,  Function_setSPDIFOut},
		{Flag_HeadMatch,    "setI2SOut",       UseStrlen,  1,  Function_setI2SOut},
		{Flag_HeadMatch,    "setNONEOut",       UseStrlen,  1,  Function_setNONEOut},
		{Flag_HeadMatch,    "setTDMOut",       UseStrlen,  1,  Function_setTDMOut},

		{Flag_HeadMatch,    "HDCPON",       UseStrlen,  1,  Function_HDCPON},
		{Flag_HeadMatch,    "HDCPOFF",       UseStrlen,  1,  Function_HDCPOFF},
		{Flag_HeadMatch,    "getTxHdcp",       UseStrlen,  1,  Function_getTxHdcp},
    {Flag_HeadMatch,    "M21452Read",       UseStrlen,  1,  Function_M21452Read},
    {Flag_HeadMatch,    "HDMISelect",       UseStrlen,  1,  Function_HDMISelect},
    {Flag_HeadMatch,    "HDMIDetect",       UseStrlen,  1,  Function_HDMIDetect},
    {Flag_HeadMatch,    "setTx5vOn",       UseStrlen,  1,  Function_setTx5vOn},

    {Flag_HeadMatch,    "0V",       UseStrlen,  1,  Function_XVX},
    {Flag_HeadMatch,    "1V",       UseStrlen,  1,  Function_XVX},
    {Flag_HeadMatch,    "2V",       UseStrlen,  1,  Function_XVX},
    {Flag_HeadMatch,    "3V",       UseStrlen,  1,  Function_XVX},
    {Flag_HeadMatch,    "4V",       UseStrlen,  1,  Function_XVX},
    #if (MAX_IN>4)
    {Flag_HeadMatch,    "5V",       UseStrlen,  1,  Function_XVX},
    {Flag_HeadMatch,    "6V",       UseStrlen,  1,  Function_XVX},
    {Flag_HeadMatch,    "7V",       UseStrlen,  1,  Function_XVX},
    {Flag_HeadMatch,    "8V",       UseStrlen,  1,  Function_XVX},
    #endif

    #if((MACHINETYPE!=MUH44A_H2)&&(MACHINETYPE!=MUH88A_H2))
    {Flag_HeadMatch,    "0B",       UseStrlen,  1,  Function_XBX},
    {Flag_HeadMatch,    "1B",       UseStrlen,  1,  Function_XBX},
    {Flag_HeadMatch,    "2B",       UseStrlen,  1,  Function_XBX},
    {Flag_HeadMatch,    "3B",       UseStrlen,  1,  Function_XBX},
    {Flag_HeadMatch,    "4B",       UseStrlen,  1,  Function_XBX},
    #if (MAX_IN>4)
    {Flag_HeadMatch,    "5B",       UseStrlen,  1,  Function_XBX},
    {Flag_HeadMatch,    "6B",       UseStrlen,  1,  Function_XBX},
    {Flag_HeadMatch,    "7B",       UseStrlen,  1,  Function_XBX},
    {Flag_HeadMatch,    "8B",       UseStrlen,  1,  Function_XBX},
    #endif
    {Flag_HeadMatch,    "0R",       UseStrlen,  1,  Function_XRX},
    {Flag_HeadMatch,    "1R",       UseStrlen,  1,  Function_XRX},
    {Flag_HeadMatch,    "2R",       UseStrlen,  1,  Function_XRX},
    {Flag_HeadMatch,    "3R",       UseStrlen,  1,  Function_XRX},
    {Flag_HeadMatch,    "4R",       UseStrlen,  1,  Function_XRX},
    #if (MAX_IN>4)
    {Flag_HeadMatch,    "5R",       UseStrlen,  1,  Function_XRX},
    {Flag_HeadMatch,    "6R",       UseStrlen,  1,  Function_XRX},
    {Flag_HeadMatch,    "7R",       UseStrlen,  1,  Function_XRX},
    {Flag_HeadMatch,    "8R",       UseStrlen,  1,  Function_XRX},
    #endif
    #endif

    {Flag_HeadMatch,    "CEC",      UseStrlen,  1,  Function_CEC},
    {Flag_HeadMatch,    "/+",       UseStrlen,  1,  Function_RemoteControl},
    {Flag_WholeWord,    "All#.",    UseStrlen,  1,  Funciton_AllThrogh},
    {Flag_WholeWord,    "All$.",    UseStrlen,  1,  Funciton_AllClose},
    {Flag_WholeWord,    "All@.",    UseStrlen,  1,  Funciton_AllOpen},
    {Flag_SternMatch,   "All",      UseStrlen,  1,  Funciton_XToAll},
    {Flag_SternMatch,   "#",        UseStrlen,  1,  Funciton_OneThrogh},
    {Flag_SternMatch,   "$",        UseStrlen,  1,  Funciton_OneClose},
    {Flag_SternMatch,   "@",        UseStrlen,  1,  Funciton_OneOpen},
    {Flag_HeadMatch,    "Save",     UseStrlen,  1,  Funciton_SaveScene},
    {Flag_HeadMatch,    "Recall",   UseStrlen,  1,  Funciton_RecallScene},
    {Flag_HeadMatch,    "Clear",    UseStrlen,  1,  Funciton_ClearScene},
    {Flag_WholeWord,    "Status.",  UseStrlen,  1,  Funciton_Status},
    {Flag_HeadMatch,    "Status",   UseStrlen,  1,  Funciton_Status},
    {Flag_HeadMatch,    "I-Lock",   UseStrlen,  1,  Funciton_LockOneChanel},
    {Flag_HeadMatch,    "I-UnLock", UseStrlen,  1,  Funciton_UnLockOneChanel},
    {Flag_HeadMatch,    "A-Lock",   UseStrlen,  1,  Funciton_LockAllChanel},
    {Flag_HeadMatch,    "A-UnLock", UseStrlen,  1,  Funciton_UnLockAllChanel},
    {Flag_WholeWord,    "Lock-Sta.",UseStrlen,  1,  Funciton_LockStatus},
    {Flag_WholeWord,    "Demo.",    UseStrlen,  1,  Funciton_Demo},
    {Flag_WholeWord,    "Undo.",    UseStrlen,  1,  Funciton_Undo},
    {Flag_WholeWord,    "STANDBY.", UseStrlen,  1,  Funciton_Standby},
    {Flag_WholeWord,    "PWOFF.",   UseStrlen,  1,  Funciton_PowerOff},
    {Flag_HeadMatch,    "/%",       UseStrlen,  1,  Funciton_HdcpManage},
    {Flag_WholeWord,    "EDIDMInit.",UseStrlen,  1,  Funciton_EdidInit},
    {Flag_HeadMatch,    "EDIDG",    UseStrlen,  1,  Funciton_GetOutPutPortEdid},
    {Flag_HeadMatch,    "OUTTG",    UseStrlen,  1,  Funciton_GetOutPortEdid},

    {Flag_HeadMatch,    "GetInPortEDID",UseStrlen,  1,  Funciton_GetInputPortEdid},
    {Flag_HeadMatch,    "EDIDM",    UseStrlen,  1,  Funciton_CopyEdidFromOutPutToInput},
    {Flag_HeadMatch,    "EDIDH",    UseStrlen,  1,  Funciton_CopyEdidFromOutPutToInput_ForceToPcm},
    {Flag_HeadMatch,    "EDIDPCM",  UseStrlen,  1,  Funciton_EdidAudioInfoForceToPcm},
    {Flag_HeadMatch,    "EDIDUpgrade",UseStrlen,  1,  Funciton_EdidUpdate},

    #if((MACHINETYPE!=MUH44A_H2)&&(MACHINETYPE!=MUH88A_H2))
    {Flag_HeadMatch,    "GetIntEDID",UseStrlen,  1,  Funciton_GetInternalEdid},
    {Flag_HeadMatch,    "UpgradeIntEDID",UseStrlen,  1,  Funciton_EdidUpdateInternal},
    #endif
    {Flag_HeadMatch,    "EDID/",    UseStrlen,  1,  Funciton_CopyInternalEdidToInput},
    {Flag_HeadMatch,    "EDIDIN/ALL/",    UseStrlen,  1,  Funciton_CopyInternalEdidToAllInput},
    {Flag_HeadMatch,    "%080",     UseStrlen,  1,  Funciton_HdcpAutoManage},
    {Flag_HeadMatch,    "%090",     UseStrlen,  1,  Funciton_IrCarrierMode},
    {Flag_HeadMatch,    "DigitAudioON",UseStrlen,  1,  Funciton_OpenDigitalAudio},
    {Flag_HeadMatch,    "DigitAudioOFF",UseStrlen,  1,  Funciton_CloseDigitalAudio},
    {Flag_HeadMatch,    "%997",     UseStrlen,  1,  Funciton_QueryVideoAudioStatus},
    {Flag_HeadMatch,    "%996",     UseStrlen,  1,  Funciton_QuerySystemStatus},
    #ifdef PRD_POWER_ONOFF_MESSAGE
    {Flag_HeadMatch,    "%994",     UseStrlen,  1,  Funciton_QueryPortPWOFFSendData},
    {Flag_HeadMatch,    "%995",     UseStrlen,  1,  Funciton_QueryPortPWONSendData},
    #endif
    {Flag_WholeWord,    "%0911.",   UseStrlen,  1,  Funciton_FactoryReset},
    {Flag_HeadMatch,    "SIIMON",   UseStrlen,  1,  Funciton_SiiMonUartNumberSet},
    {Flag_WholeWord,    "DEBUGEN.", UseStrlen,  1,  Funciton_DebugEn},
    {Flag_WholeWord,    "DEBUGCOLORON.",UseStrlen,  1,  Funciton_DebugColorOn},
    {Flag_WholeWord,    "DEBUGCOLOROFF.",UseStrlen,  1,  Funciton_DebugColorOff},
    {Flag_WholeWord,    "GITVERSION.",UseStrlen,  1,  Funciton_GetGitVersion},
    {Flag_HeadMatch,    "DEBUGLEVEL",UseStrlen,  1,  Funciton_DebugLevel},
    {Flag_HeadMatch,    "DEBUGPOST",UseStrlen,  1,  Funciton_DebugPost},
    #ifdef ENABLE_DB9_BAUDRATE_SWITCH
    {Flag_WholeWord,    "Baudrate 2400.",UseStrlen,  1,  Funciton_SetDB9Baudrate2400},
    {Flag_WholeWord,    "Baudrate 4800.",UseStrlen,  1,  Funciton_SetDB9Baudrate4800},
    {Flag_WholeWord,    "Baudrate 9600.",UseStrlen,  1,  Funciton_SetDB9Baudrate9600},
    {Flag_WholeWord,    "Baudrate 19200.",UseStrlen,  1,  Funciton_SetDB9Baudrate19200},
    {Flag_WholeWord,    "Baudrate 38400.",UseStrlen,  1,  Funciton_SetDB9Baudrate38400},
    {Flag_WholeWord,    "Baudrate 57600.",UseStrlen,  1,  Funciton_SetDB9Baudrate57600},
    {Flag_WholeWord,    "Baudrate 115200.",UseStrlen,  1,  Funciton_SetDB9Baudrate115200},
    #endif
};


stCmdListType CmdListToMode2[]=
{
    {Flag_WholeWord,    "/%Lock;",  UseStrlen,  1,  Funciton_SetSystemLock},
    {Flag_WholeWord,    "/%Unlock;",UseStrlen,  1,  Funciton_SetSystemUnLock},
    {Flag_WholeWord,    "/*Type;",  UseStrlen,  1,  Funciton_SetSystemModelType},
    {Flag_WholeWord,    "/^Version;",UseStrlen,  1,  Funciton_SetSystemVersion},
    {Flag_WholeWord,    "/^TestVersion;",UseStrlen,  1,  Funciton_SetSystemTestVersion},
    {Flag_WholeWord,    "/^TestType;",UseStrlen,  1,  Funciton_SetSystemTestType},
};

int GetCmdListToMode1_Len(void)
{
    return (sizeof(CmdListToMode1)/sizeof(CmdListToMode1[0]));
}

int GetCmdListToMode2_Len(void)
{
    return (sizeof(CmdListToMode2)/sizeof(CmdListToMode2[0]));
}

