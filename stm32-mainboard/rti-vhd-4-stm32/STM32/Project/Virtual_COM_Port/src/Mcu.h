///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <mcu.h>
//   @author Max.Kao@ite.com.tw
//   @date   2011/12/01
//   @fileversion: IT6623_Sample_1.04
//******************************************/

#ifndef _MCU_H_
#define _MCU_H_
#include <stdio.h>
#include <string.h>

#define 	_MCU_

//#include "typedefs.h"
#include "stm32f10x.h" 

//#define 	BYTE    u8

#define  Uart_Debug    0

#define ADDR_24C32          0xA2
/*************************************/
/*Port Using Define sys             */
/*************************************/ 

#define CH_CORRECT                  	1             //Channel correct,USART,LCD DISPLAY return the switched channel          
#define CH_ERROR                    		2             //channel invalid,USART.LCD DISPLAY THE "OUT OF RANGE"
#define CM_CORRECT                  	3             //command correct,USART,LCD DISPLAY THE "COMMAND CORRECT"
#define CM_ERROR                    		4             //command invalid,USART LCD DISPLAY THE "COMMAND INVALID"
#define OTHERS                      		5             //DON'T NEED TO OPRATON.USART,LCD data filled out at USART_Proecss();

#define sChannelSwitch           		1
#define sAllThrough              		2
#define sAllClose                		3
#define sToAll                   			4
#define sThrough                 		5
#define sClose                   			6
#define sDispSysInfo             		7
#define sInputData               		8
#define sAllKeyAll               		9
#define sThroughKey              		10
#define sDispSave                		11
#define sStandby                 		12
#define sProgramLoad             		13
#define sProgramSave             		14
#define sAllKey                  		15
#define sRecallFromEEPROM        	16
#define sSaveToEEPROM            	17
#define sCloseKey	               	18
#define sInitProgBuff            		19
#define sInitChannelBuff         		20
#define sClearEEPROM             		21
#define sDEMO                    		22
#define sInputPassword           	23

#define YES                      			1
#define NO                       			0

#define MAX_IN               INSTANCE_9396_IN
#define MAX_OUT		     INSTANCE_9396_OUT

#define VSAVE                    		0

#define EDID_TABLE_LEN           	256
//EEPROM�������򻮷�

//ǰ128�ֽڴ��ϵͳ��Ϣ�Լ��ϵ�ͨ��״̬
#define first_use_add            		0
#define sys_run_count_add        	1  		// 2�ֽ�
#define sys_run_time_count_add   3	  	// 4�ֽ�  ��λS
#define auto_hdcp_manage_add    7		// 1�ֽ�
#define IN_hdcp_en_add           	8  		// 8�ֽ�
#define OUT_hdcp_staues_add      16 		// 8�ֽ�
#define sys_lock_flag_add        	24
#define RE_message_add           	25
#define IR_carrier_mode_add      	26
//gxx 131219
#define PWON_OFF_BAUD            	27		// 16�ֽ�

#define sys_type_add             		53		// 17�ֽ�
#define sys_name_add             	70		// 17�ֽ�
#define sys_EDID_add             		87		// 4�ֽ�
#define sys_IP_add               		91		// 4�ֽ�

#define LCD1_add                 		95		// 16�ֽ�
#define LCD2_add                 		111		// 16�ֽ�  

#define V_channel_befoer_PWOFF   127 	//Ԥ��4
#define A_channel_befoer_PWOFF   131 	//Ԥ��4
#define R_channel_befoer_PWOFF   143 	//Ԥ��16    ��ַ�Ѿ�����159
//�ӵ�160�ֽڿ�ʼ����л���Ϣ    F0 ~ F9  ÿ��8�ֽ� ��80�ֽ�(������Ƶ�ͺ���)  ֧�ֵ�88
#define F0_V_add                 		160
#define F0_R_add                 		240 		// 320 //��ַ�Ѿ�����480
#define F0_A_add                 		320		// ��ַ����400 
#define V_channel_befoer_close    480		// 8���ֽ�
#define Digit_audio_addr   		488		// 4���ֽ�
#define IntOrExt_audio_addr   	492		// 4���ֽ�
#define R_channel_befoer_close    496		// Ԥ��16    ��ַ�Ѿ�����512
#define PowerOff_addr   			504		// ռ1���ֽ�
#define Standby_addr   			505		// ռ1���ֽ�
#define first_PowerOff_addr   		506


//����5��EDID���ӵ�512��ʼ�洢  ��256*5 = 1280 �ֽ�
#define Internal_EDID_add        	512
//1792�ֽڿ�ʼ��ŵ�������ͨ����ǰ��EDID���ݣ���256*6 = 1536 �ֽ�
#define INPUT_EDID_add           	1792       //��ַ�Ѿ�����3328
//���512�ֽڴ洢PWON/OFFԤ��������
#define uart_data_add            		3584


#define   PWON_OFF_Lenth    		32

#define  LCD_MAX_CHAR       		17  		// last must be null

#define SCENCES_MAX   			10

#endif
