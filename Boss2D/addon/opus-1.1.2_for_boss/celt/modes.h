/* Copyright (c) 2007-2008 CSIRO
   Copyright (c) 2007-2009 Xiph.Org Foundation
   Copyright (c) 2008 Gregory Maxwell
   Written by Jean-Marc Valin and Gregory Maxwell */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MODES_H
#define MODES_H

#include BOSS_OPUS_U_opus_types_h //original-code:"opus_types.h"
#include BOSS_OPUS_U_celt_h //original-code:"celt.h"
#include BOSS_OPUS_U_arch_h //original-code:"arch.h"
#include BOSS_OPUS_U_mdct_h //original-code:"mdct.h"
#include BOSS_OPUS_U_entenc_h //original-code:"entenc.h"
#include BOSS_OPUS_U_entdec_h //original-code:"entdec.h"

#define MAX_PERIOD 1024

typedef struct {
   int size;
   const opus_int16 *index;
   const unsigned char *bits;
   const unsigned char *caps;
} PulseCache;

/** Mode definition (opaque)
 @brief Mode definition
 */
struct OpusCustomMode {
   opus_int32 Fs;
   int          overlap;

   int          nbEBands;
   int          effEBands;
   opus_val16    preemph[4];
   const opus_int16   *eBands;   /**< Definition for each "pseudo-critical band" */

   int         maxLM;
   int         nbShortMdcts;
   int         shortMdctSize;

   int          nbAllocVectors; /**< Number of lines in the matrix below */
   const unsigned char   *allocVectors;   /**< Number of bits in each band for several rates */
   const opus_int16 *logN;

   const opus_val16 *window;
   mdct_lookup mdct;
   PulseCache cache;
};


#endif
