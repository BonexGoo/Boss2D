/* Copyright (c) 2007-2008 CSIRO
   Copyright (c) 2007-2010 Xiph.Org Foundation
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

#ifndef __CELT_MIPSR1_H__
#define __CELT_MIPSR1_H__

#ifdef HAVE_CONFIG_H
#include BOSS_OPUS_U_config_h //original-code:"config.h"
#endif

#define CELT_C

#include BOSS_OPUS_U_os_support_h //original-code:"os_support.h"
#include BOSS_OPUS_U_mdct_h //original-code:"mdct.h"
#include <math.h>
#include BOSS_OPUS_U_celt_h //original-code:"celt.h"
#include BOSS_OPUS_U_pitch_h //original-code:"pitch.h"
#include BOSS_OPUS_U_bands_h //original-code:"bands.h"
#include BOSS_OPUS_U_modes_h //original-code:"modes.h"
#include BOSS_OPUS_U_entcode_h //original-code:"entcode.h"
#include BOSS_OPUS_U_quant_bands_h //original-code:"quant_bands.h"
#include BOSS_OPUS_U_rate_h //original-code:"rate.h"
#include BOSS_OPUS_U_stack_alloc_h //original-code:"stack_alloc.h"
#include BOSS_OPUS_U_mathops_h //original-code:"mathops.h"
#include BOSS_OPUS_U_float_cast_h //original-code:"float_cast.h"
#include <stdarg.h>
#include BOSS_OPUS_U_celt_lpc_h //original-code:"celt_lpc.h"
#include BOSS_OPUS_U_vq_h //original-code:"vq.h"

#define OVERRIDE_comb_filter
void comb_filter(opus_val32 *y, opus_val32 *x, int T0, int T1, int N,
      opus_val16 g0, opus_val16 g1, int tapset0, int tapset1,
      const opus_val16 *window, int overlap, int arch)
{
   int i;
   opus_val32 x0, x1, x2, x3, x4;

   (void)arch;

   /* printf ("%d %d %f %f\n", T0, T1, g0, g1); */
   opus_val16 g00, g01, g02, g10, g11, g12;
   static const opus_val16 gains[3][3] = {
         {QCONST16(0.3066406250f, 15), QCONST16(0.2170410156f, 15), QCONST16(0.1296386719f, 15)},
         {QCONST16(0.4638671875f, 15), QCONST16(0.2680664062f, 15), QCONST16(0.f, 15)},
         {QCONST16(0.7998046875f, 15), QCONST16(0.1000976562f, 15), QCONST16(0.f, 15)}};

   if (g0==0 && g1==0)
   {
      /* OPT: Happens to work without the OPUS_MOVE(), but only because the current encoder already copies x to y */
      if (x!=y)
         OPUS_MOVE(y, x, N);
      return;
   }

   g00 = MULT16_16_P15(g0, gains[tapset0][0]);
   g01 = MULT16_16_P15(g0, gains[tapset0][1]);
   g02 = MULT16_16_P15(g0, gains[tapset0][2]);
   g10 = MULT16_16_P15(g1, gains[tapset1][0]);
   g11 = MULT16_16_P15(g1, gains[tapset1][1]);
   g12 = MULT16_16_P15(g1, gains[tapset1][2]);
   x1 = x[-T1+1];
   x2 = x[-T1  ];
   x3 = x[-T1-1];
   x4 = x[-T1-2];
   /* If the filter didn't change, we don't need the overlap */
   if (g0==g1 && T0==T1 && tapset0==tapset1)
      overlap=0;

   for (i=0;i<overlap;i++)
   {
      opus_val16 f;
      opus_val32 res;
      f = MULT16_16_Q15(window[i],window[i]);
      x0= x[i-T1+2];

      asm volatile("MULT $ac1, %0, %1" : : "r" ((int)MULT16_16_Q15((Q15ONE-f),g00)), "r" ((int)x[i-T0]));

      asm volatile("MADD $ac1, %0, %1" : : "r" ((int)MULT16_16_Q15((Q15ONE-f),g01)), "r" ((int)ADD32(x[i-T0-1],x[i-T0+1])));
      asm volatile("MADD $ac1, %0, %1" : : "r" ((int)MULT16_16_Q15((Q15ONE-f),g02)), "r" ((int)ADD32(x[i-T0-2],x[i-T0+2])));
      asm volatile("MADD $ac1, %0, %1" : : "r" ((int)MULT16_16_Q15(f,g10)), "r" ((int)x2));
      asm volatile("MADD $ac1, %0, %1" : : "r" ((int)MULT16_16_Q15(f,g11)), "r" ((int)ADD32(x3,x1)));
      asm volatile("MADD $ac1, %0, %1" : : "r" ((int)MULT16_16_Q15(f,g12)), "r" ((int)ADD32(x4,x0)));

      asm volatile("EXTR.W %0,$ac1, %1" : "=r" (res): "i" (15));

      y[i] = x[i] + res;

      x4=x3;
      x3=x2;
      x2=x1;
      x1=x0;
   }

   x4 = x[i-T1-2];
   x3 = x[i-T1-1];
   x2 = x[i-T1];
   x1 = x[i-T1+1];

   if (g1==0)
   {
      /* OPT: Happens to work without the OPUS_MOVE(), but only because the current encoder already copies x to y */
      if (x!=y)
         OPUS_MOVE(y+overlap, x+overlap, N-overlap);
      return;
   }

   for (i=overlap;i<N;i++)
   {
      opus_val32 res;
      x0=x[i-T1+2];

      asm volatile("MULT $ac1, %0, %1" : : "r" ((int)g10), "r" ((int)x2));

      asm volatile("MADD $ac1, %0, %1" : : "r" ((int)g11), "r" ((int)ADD32(x3,x1)));
      asm volatile("MADD $ac1, %0, %1" : : "r" ((int)g12), "r" ((int)ADD32(x4,x0)));
      asm volatile("EXTR.W %0,$ac1, %1" : "=r" (res): "i" (15));
      y[i] = x[i] + res;
      x4=x3;
      x3=x2;
      x2=x1;
      x1=x0;
   }
}

#endif /* __CELT_MIPSR1_H__ */
