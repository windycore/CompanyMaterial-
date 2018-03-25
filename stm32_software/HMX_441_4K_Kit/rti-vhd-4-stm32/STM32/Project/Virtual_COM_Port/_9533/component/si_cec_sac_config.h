//***************************************************************************
//!file     si_cec_sac_config.h
//!brief    System-specific configuration information for System Audio Control Component
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#if !defined __SI_CONFIG_CEC_SAC_H__
#define __SI_CONFIG_CEC_SAC_H__

//------------------------------------------------------------------------------
//  CEC SAC Component SiI953x-specific
//------------------------------------------------------------------------------

#define SII_NUM_CEC_SAC             1

#define NUMBER_OF_AUDIO_FORMATS     4  //!< number of audio formats supported by this device by default

// Device audio format support configuration list (example)
#define SII_CEC_SAC_DEFAULT_AUDIO_FORMAT_LIST   { {4, CEC_AUD_FMT_LPCM,  0, 0},\
                                                  {6, CEC_AUD_FMT_AC3,   0, 0},\
                                                  {2, CEC_AUD_FMT_MPEG1, 0, 0},\
                                                  {2, CEC_AUD_FMT_MP3,   0, 0} }


#endif  //__SI_CONFIG_CEC_SAC_H__

