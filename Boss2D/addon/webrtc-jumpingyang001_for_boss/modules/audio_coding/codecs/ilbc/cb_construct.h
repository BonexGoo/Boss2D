/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/******************************************************************

 iLBC Speech Coder ANSI-C Source Code

 WebRtcIlbcfix_CbConstruct.h

******************************************************************/

#ifndef MODULES_AUDIO_CODING_CODECS_ILBC_MAIN_SOURCE_CB_CONSTRUCT_H_
#define MODULES_AUDIO_CODING_CODECS_ILBC_MAIN_SOURCE_CB_CONSTRUCT_H_

#include <stdbool.h>
#include BOSS_FAKEWIN_V_stdint_h //original-code:<stdint.h>

#include BOSS_WEBRTC_U_modules__audio_coding__codecs__ilbc__defines_h //original-code:"modules/audio_coding/codecs/ilbc/defines.h"
#include BOSS_WEBRTC_U_rtc_base__system__unused_h //original-code:"rtc_base/system/unused.h"

/*----------------------------------------------------------------*
 *  Construct decoded vector from codebook and gains.
 *---------------------------------------------------------------*/

// Returns true on success, false on failure.
bool WebRtcIlbcfix_CbConstruct(
    int16_t* decvector,        /* (o) Decoded vector */
    const int16_t* index,      /* (i) Codebook indices */
    const int16_t* gain_index, /* (i) Gain quantization indices */
    int16_t* mem,              /* (i) Buffer for codevector construction */
    size_t lMem,               /* (i) Length of buffer */
    size_t veclen              /* (i) Length of vector */
    ) RTC_WARN_UNUSED_RESULT;

#endif
