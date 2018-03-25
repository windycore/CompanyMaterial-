//***************************************************************************
//!file     si_eeprom.h
//!brief    Silicon Image Starter Kit EEPROM interface
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_EEPROM_H__
#define __SI_EEPROM_H__

#include "si_common.h"


#define EEPROM_I2C_ADDRESS      0xA0 // 0xA8  //[kyq EDID: 20150824 12:43:04]

#define EE_ADDR_SYSTEM_SETTING  m_asNodeInfo[eNodeID].wNodeOffset

#define EE_CONFIG_VALID        EE_ADDR_SYSTEM_SETTING+offsetof(SYSTEM_SETTING,cCONFIG_VALID) //0x0000
             #define CONFIG_VALID            0x55        // Contents of configuration block have been initialized.

#define EE_FW_CONTROL           EE_CONFIG_VALID+sizeof(BYTE)  //0x0001
            #define EEFLAG_HOST_ENABLE      0x01        // 1 - Host Control, 0 – Internal Control
            #define EEFLAG_IR_ENABLE        0x02        // 1 – Enable IR receiver
            #define EEFLAG_ROTARY_ENABLE    0x04        // 1 – Enable on-board rotary switch
            #define EEFLAG_CEC_ENABLE       0x08        // 1 – Enable CEC parsing
            #define EEFLAG_CEC_PRINT        0x10        // 1 - Enable CEC DEBUG_PRINT messages
            #define EEFLAG_CEC_ENUM         0x20        // 1 - Enable CEC enumeration every 20 seconds
                                            // 0 - Enable CEC enumeration only at startup
            #define EEFLAG_NVRAM_IGNORE     0x40        // 1 - Don't read or write NVRAM in normal operation
            #define EEFLAG_DBG_MSG_COLORING 0x80        // 1 - Enable DEBUG_PRINT message coloring
                                            // 0 - Disable DEBUG_PRINT message coloring


#define EEPROM_EDID_LOADSEL    EE_FW_CONTROL+sizeof(BYTE)// 0x0002      // Select which EDID rams to load from EEPROM
            #define EE_EDID0_LOAD           0x01        // Load EDID ram 0 from EEPROM EDID 0
            #define EE_EDID1_LOAD           0x02        // Load EDID ram 1 from EEPROM EDID 1
            #define EE_EDID2_LOAD           0x04        // Load EDID ram 2 from EEPROM EDID 2
            #define EE_EDID3_LOAD           0x08        // Load EDID ram 3 from EEPROM EDID 3
            #define EE_EDID4_LOAD           0x10        // Load EDID ram 4 from EEPROM EDID VGA
            #define EE_EDID5_LOAD           0x20        // Load EDID ram 4 from EEPROM EDID VGA

#define EE_DB_VERSION           EEPROM_EDID_LOADSEL+ sizeof(BYTE)  //0x0003
           #define EEPROM_DB_VERSION       0x10        // Database version 1.0

#define EE_UART_MSG_LEVEL      EE_DB_VERSION+sizeof(BYTE)// 0x0004
           #define MSG_ALWAYS          0x00
           #define MSG_ERR                 0x01
           #define MSG_STAT                0x02
           #define MSG_DBG                 0x03
           #define MSG_PRINT_ALL      0xFF

#define EE_EDID_VALID           0x0010
#define EE_EDIDVGA_VALID        0x0011
#define EE_DEVBOOTDATA_VALID    0x0012

#define EE_EDID0_VALID          EE_EDID_VALID
#define EE_EDID1_VALID          0x0013
#define EE_EDID2_VALID          0x0014
#define EE_EDID3_VALID          0x0015
#define EE_EDID4_VALID          EE_EDIDVGA_VALID
#define EE_EDID5_VALID          0x0016


#define EE_EDID_OFFSET          0x0100
#define EE_EDIDVGA_OFFSET       (EE_EDID_OFFSET + EDID_TABLE_LEN)
#define EE_DEVBOOT_OFFSET       (EE_EDIDVGA_OFFSET + EDID_TABLE_LEN)
#define CBUS_SEL_OFFSET         0x24    // Offset of CBUS port select byte in NVRAM boot data

#define EE_EDID0_OFFSET         EE_EDID_OFFSET
#define EE_EDID1_OFFSET         0x400
#define EE_EDID2_OFFSET         (EE_EDID1_OFFSET + EDID_TABLE_LEN)
#define EE_EDID3_OFFSET         (EE_EDID2_OFFSET + EDID_TABLE_LEN)
#define EE_EDID5_OFFSET         (EE_EDID3_OFFSET + EDID_TABLE_LEN)
#define EE_EDID4_OFFSET         EE_EDIDVGA_OFFSET



//------------------
// Repeater configuration
// (Registers 0x0020...0x003F are reserved for repeater support)

#define EE_REPEATER_BASE        0x0020

#define EE_REPEATER_SIGNATURE_OFFSET (EE_REPEATER_BASE + 0x0000)
#define EE_REPEATER_SIGNATURE_LENGTH 5

#define EE_REPEATER_MODE_OFFSET (EE_REPEATER_BASE + 0x0005)

#define EE_KSV_COUNT_OFFSET     (EE_REPEATER_BASE + 0x0006)

#define EE_KSV_DATA             (EE_REPEATER_BASE + 0x0010)





#define first_use_add            0
#define sys_run_count_add        1  //2×Ö½Ú
#define sys_run_time_count_add   3  //4×Ö½Ú  µ¥Î»S
#define auto_hdcp_manage_add     7  //1×Ö½Ú
#define IN_hdcp_en_add           8  //8×Ö½Ú
#define OUT_hdcp_staues_add      16 //8×Ö½Ú
#define sys_lock_flag_add        24
#define RE_message_add           25
#define IR_carrier_mode_add      26
//gxx 131219
#define PWON_OFF_BAUD            27//16×Ö½Ú

#define sys_type_add             53//17×Ö½Ú
#define sys_name_add             70//17×Ö½Ú
#define sys_EDID_add             87//4×Ö½Ú
#define sys_IP_add               91//4×Ö½Ú

#define LCD1_add                 95//16×Ö½Ú
#define LCD2_add                 111//16×Ö½Ú  

#define V_channel_befoer_PWOFF   127 //Ô¤Áô4
#define A_channel_befoer_PWOFF   131 //Ô¤Áô4
#define R_channel_befoer_PWOFF   143 //Ô¤Áô16    µØÖ·ÒÑ¾­µ½ÁË159
//´ÓµÚ160×Ö½Ú¿ªÊ¼´æ·ÅÇÐ»»ÐÅÏ¢    F0 ~ F9  Ã¿¿é8×Ö½Ú ¹²80×Ö½Ú(°üÀ¨ÊÓÆµºÍºìÍâ)  Ö§³Öµ½88
#define F0_V_add                 160
#define F0_R_add                 240 //320 //µØÖ·ÒÑ¾­µ½ÁË480
#define F0_A_add                 320 //µØÖ·µ½ÁË400 
#define V_channel_befoer_close   480	//8¸ö×Ö½Ú
#define Digit_audio_addr   488	//4¸ö×Ö½Ú
#define IntOrExt_audio_addr   492	//4¸ö×Ö½Ú
#define R_channel_befoer_close   496//Ô¤Áô16    µØÖ·ÒÑ¾­µ½ÁË512
#define PowerOff_addr   504	//Õ¼1¸ö×Ö½Ú
#define Standby_addr   505//Õ¼1¸ö×Ö½Ú
#define first_PowerOff_addr   506


//ÄÚÖÃ5ÖÖEDID£¬´ÓµÚ512¿ªÊ¼´æ´¢  ¹²256*5 = 1280 ×Ö½Ú
#define Internal_EDID_add        512
//1792×Ö½Ú¿ªÊ¼´æ·ÅµÄÊÇÊäÈëÍ¨µÀµ±Ç°µÄEDIDÊý¾Ý£¬¹²256*6 = 1536 ×Ö½Ú
#define INPUT_EDID_add           1792       //µØÖ·ÒÑ¾­µ½ÁË3328
//×îºó512×Ö½Ú´æ´¢PWON/OFFÔ¤·¢ËÍÊý¾Ý
#define uart_data_add            3584

//gxx 131023
#define   PON    1
#define   POFF   0

//#define   PWON_OFF_Lenth    48




//-------------------------------------------------------------------------------
//
//  EEPROM Functions
//
//-------------------------------------------------------------------------------

BYTE       SiiPlatformEepromReadByte( WORD addr );
bool_t     SiiPlatformEepromWriteByte(WORD addr, BYTE value );
bool_t	SiiPlatformEepromReadBlock( WORD addr, BYTE *pRdData, WORD nBytes );
bool_t	SiiPlatformEepromWriteBlock( WORD addr, BYTE *pWrData, WORD nBytes );

#endif  // __SI_EEPROM_H__

