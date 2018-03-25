/******************************************************************************/
/*                                                                            */
/*          (c) Copyright Explore Semiconductor, Inc. Limited 2005            */
/*                           ALL RIGHTS RESERVED                              */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*  File        :  Edid.h                                                     */
/*                                                                            */
/*  Description :  Head file of Edid IO Interface                             */
/*                                                                            */
/******************************************************************************/

#ifndef EDID_H
#define EDID_H

#include "uart.h"

#define EDID_BLOCK_SIZE  128
#define EDID_HDMI_SIZE  256


#define EDID2B_TOTAL_SHORT_VIDEO_DESC_SIZE 30	// 1-59
#define EDID2B_TOTAL_SHORT_AUDIO_DESC_SIZE 45	// 15*3
#define EDID2B_TOTAL_SHORT_SPEAKER_DESC_SIZE 3	// 3
#define EDID2B_TOTAL_VENDORDESC_DESC_SIZE 31	// 31
#define EDID2B_TOTAL_XVYCC_DESC_SIZE 3
#define EDID2B_TOTAL_VIDEO_CAP_DESC_SIZE 2

#define EDID2B_TOTAL_420_VIDEO_BLOCK_SIZE 8
#define EDID2B_TOTAL_420_CAP_MAP_BLOCK_SIZE 4
#define EDID2B_TOTAL_VIDEO_PREFERENCE_SIZE 4

#define EDID2B_DETAILED_TIMING_DESCRITPOR_SIZE 18

#define MAX_VIDEO_CODE 60


// math
#ifndef min
#define min(a,b) (((a)<(b))? (a):(b))
#endif

#ifndef max
#define max(a,b) (((a)>(b))? (a):(b))
#endif

typedef struct _EDIDB {
	unsigned char Blocks[2][128];

} EDIDB, *PEDIDB;

//
// Mixing Mode
//

typedef enum {
	EDID_MIX_MODE_Normal = 0, // Video = min, Audio = min
	EDID_MIX_MODE_MaxAudio,   // Video = min, Audio = Max
	EDID_MIX_MODE_Copy,       // Video = cpy, Audio = cpy
	EDID_MIX_MODE_MaxAll,     // Video = Max, Audio = Max
	EDID_MIX_MODE_Size
} EDID_MIX_MODE;

typedef enum 
{
	DEGREE_NO_SUPPORT = 0,
	DEGREE_3D_PRESENT,
	DEGREE_3D_STRUCTURE,
	DEGREE_3D_MASK,
	DEGREE_3D_SIZE

} SUPPORT_3D_DEGREE;

typedef enum
{
	VSDB_PARSER_IEEE_1BYTE = 0,	
	VSDB_PARSER_IEEE_2BYTE,
	VSDB_PARSER_IEEE_3BYTE,

	VSDB_PARSER_PA_HBYTE,
	VSDB_PARSER_PA_LBYTE,
	
	VSDB_PARSER_SUPPORT,
	VSDB_PARSER_TMDS,

	VSDB_PARSER_PRESENT,

	VSDB_PARSER_LANTCY_VIDEO,
	VSDB_PARSER_LANTCY_AUDIO,
	VSDB_PARSER_I_LANTCY_VIDEO,
	VSDB_PARSER_I_LANTCY_AUDIO,

	VSDB_PARSER_3D_INFO,
	VSDB_PARSER_LEN_INFO,

	VSDB_PARSER_HDMI_VIC,
	VSDB_PARSER_3D_STRUCT_HBYTE,
	VSDB_PARSER_3D_STRUCT_LBYTE,
	VSDB_PARSER_3D_MASK_HBYTE,
	VSDB_PARSER_3D_MASK_LBYTE,
	VSDB_PARSER_3D_VIC,

	VSDB_PARSER_FINISHING_STATE,
	VSDB_PARSER_UNKONW_STATE

} VSDB_PARSER_STATE;

typedef struct _VSDB_PTR
{
	BYTE ptr_support;
	BYTE ptr_present;
	BYTE save_3d_mp;
	BYTE len_hdmi_vic;
	BYTE len_hdmi_3d;
	BYTE ptr_latency;
	BYTE ptr_i_latency;
	BYTE ptr_info_3d;
	BYTE ptr_hdmi_vic;
	BYTE ptr_3d_struct;
	BYTE ptr_3d_mask;
	BYTE ptr_hdmi_3d;
		 
} VSDB_PTR;

#define VSDB_PTR_INIT			0x20
#define VSDB_3D_MP_INIT			0x04

#define VSDB_VIDEO_PRESENT		0x20
#define VSDB_3D_PRESENT			0x80
#define VSDB_3D_MULTI_PRESENT	0x60


//
// bit0 -> Max Audio, bit1 -> Max Video, bit4 -> Add Audio


//
// Structure Definitions
//

typedef struct _EDID2B {
	BYTE Blocks[2][EDID_BLOCK_SIZE];
	BYTE Flags;
	EDID_MIX_MODE MixMode;
} EDID2B, *PEDID2B;

// EDID2B.Flags
#define EDID2B_Block0_Default  				0x01
#define EDID2B_Block1_Default  				0x02
#define EDID2B_DVI_HDMI_Mixed  				0x04
#define EDID2B_No_Audio_HDMI_Mixed 			0x08
#define EDID2B_Add_Advanced_Audio			0x10


// EDID_MixWith_Flag
#define EDID_MixWith_Flag__Deep_Color 		0x01
#define EDID_MixWith_Flag__PhyAddr_Copy 	0x02
#define EDID_MixWith_Flag__PhyAddr_Default	0x04

extern  BYTE EDID2B_Default[EDID_BLOCK_SIZE*2];
extern EDID2B EDID_Main[1] ; // force the EDID_Main[0].Flags to zero
extern BYTE Sys_Ram[128];
//
// Pubic Functions
//

// Basic EDID pare function
extern BYTE EDID_GetHDMICap(BYTE *pTarget);
extern BYTE EDID_GetPCMFreqCap(BYTE *pTarget);
extern BYTE EDID_GetChannelCap(BYTE *pTarget, BYTE AFormat);
extern BYTE EDID_GetColorDepthCap(BYTE *pTarget);
extern BYTE EDID_GetDataBlockAddr(BYTE *pTarget, BYTE Tag, BYTE Start);

/// The EDID2B function for EDID mixing
extern void EDID_Reset(PEDID2B pCurrent);
extern BYTE EDID_MixWith(PEDID2B pCurrent, BYTE *pTarget, BYTE EDID_MixWith_Flag);
extern void EDID_SetMixingMode(PEDID2B pCurrent, EDID_MIX_MODE Mode);
extern EDID_MIX_MODE EDID_GetMixingMode(PEDID2B pCurrent);

// Force Support 3D
extern BYTE EDID_Force_Support_3D(BYTE *ptr, BYTE *size, SUPPORT_3D_DEGREE degree);
extern BYTE parser_VSDB(BYTE *ptr, BYTE *size, VSDB_PTR *db);

void EDID_BypassWithAudio(ePOWER_STATUS POWER_STATUS, PEDIDB pCurrent);

#endif // EDID_H


