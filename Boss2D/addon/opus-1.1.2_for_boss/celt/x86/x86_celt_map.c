/* Copyright (c) 2014, Cisco Systems, INC
   Written by XiangMingZhu WeiZhou MinPeng YanWang

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

#if defined(HAVE_CONFIG_H)
#include BOSS_OPUS_U_config_h //original-code:"config.h"
#endif

#include "x86/x86cpu.h"
#include BOSS_OPUS_U_celt_lpc_h //original-code:"celt_lpc.h"
#include BOSS_OPUS_U_pitch_h //original-code:"pitch.h"
#include "pitch_sse.h"

#if defined(OPUS_HAVE_RTCD)

# if defined(FIXED_POINT)

#if defined(OPUS_X86_MAY_HAVE_SSE4_1) && !defined(OPUS_X86_PRESUME_SSE4_1)

void (*const CELT_FIR_IMPL[OPUS_ARCHMASK + 1])(
         const opus_val16 *x,
         const opus_val16 *num,
         opus_val16       *y,
         int              N,
         int              ord,
         opus_val16       *mem,
         int              arch
) = {
  celt_fir_c,                /* non-sse */
  celt_fir_c,
  celt_fir_c,
  MAY_HAVE_SSE4_1(celt_fir), /* sse4.1  */
  MAY_HAVE_SSE4_1(celt_fir)  /* avx  */
};

void (*const XCORR_KERNEL_IMPL[OPUS_ARCHMASK + 1])(
         const opus_val16 *x,
         const opus_val16 *y,
         opus_val32       sum[4],
         int              len
) = {
  xcorr_kernel_c,                /* non-sse */
  xcorr_kernel_c,
  xcorr_kernel_c,
  MAY_HAVE_SSE4_1(xcorr_kernel), /* sse4.1  */
  MAY_HAVE_SSE4_1(xcorr_kernel)  /* avx  */
};

#endif

#if (defined(OPUS_X86_MAY_HAVE_SSE4_1) && !defined(OPUS_X86_PRESUME_SSE4_1)) ||  \
	(!defined(OPUS_X86_MAY_HAVE_SSE_4_1) && defined(OPUS_X86_MAY_HAVE_SSE2) && !defined(OPUS_X86_PRESUME_SSE2))

opus_val32 (*const CELT_INNER_PROD_IMPL[OPUS_ARCHMASK + 1])(
         const opus_val16 *x,
         const opus_val16 *y,
         int              N
) = {
  celt_inner_prod_c,                /* non-sse */
  celt_inner_prod_c,
  MAY_HAVE_SSE2(celt_inner_prod),
  MAY_HAVE_SSE4_1(celt_inner_prod), /* sse4.1  */
  MAY_HAVE_SSE4_1(celt_inner_prod)  /* avx  */
};

#endif

# else

#if defined(OPUS_X86_MAY_HAVE_SSE) && !defined(OPUS_X86_PRESUME_SSE)

void (*const XCORR_KERNEL_IMPL[OPUS_ARCHMASK + 1])(
         const opus_val16 *x,
         const opus_val16 *y,
         opus_val32       sum[4],
         int              len
) = {
  xcorr_kernel_c,                /* non-sse */
  MAY_HAVE_SSE(xcorr_kernel),
  MAY_HAVE_SSE(xcorr_kernel),
  MAY_HAVE_SSE(xcorr_kernel),
  MAY_HAVE_SSE(xcorr_kernel)
};

opus_val32 (*const CELT_INNER_PROD_IMPL[OPUS_ARCHMASK + 1])(
         const opus_val16 *x,
         const opus_val16 *y,
         int              N
) = {
  celt_inner_prod_c,                /* non-sse */
  MAY_HAVE_SSE(celt_inner_prod),
  MAY_HAVE_SSE(celt_inner_prod),
  MAY_HAVE_SSE(celt_inner_prod),
  MAY_HAVE_SSE(celt_inner_prod)
};

void (*const DUAL_INNER_PROD_IMPL[OPUS_ARCHMASK + 1])(
                    const opus_val16 *x,
                    const opus_val16 *y01,
                    const opus_val16 *y02,
                    int               N,
                    opus_val32       *xy1,
                    opus_val32       *xy2
) = {
  dual_inner_prod_c,                /* non-sse */
  MAY_HAVE_SSE(dual_inner_prod),
  MAY_HAVE_SSE(dual_inner_prod),
  MAY_HAVE_SSE(dual_inner_prod),
  MAY_HAVE_SSE(dual_inner_prod)
};

void (*const COMB_FILTER_CONST_IMPL[OPUS_ARCHMASK + 1])(
              opus_val32 *y,
              opus_val32 *x,
              int         T,
              int         N,
              opus_val16  g10,
              opus_val16  g11,
              opus_val16  g12
) = {
  comb_filter_const_c,                /* non-sse */
  MAY_HAVE_SSE(comb_filter_const),
  MAY_HAVE_SSE(comb_filter_const),
  MAY_HAVE_SSE(comb_filter_const),
  MAY_HAVE_SSE(comb_filter_const)
};


#endif

#endif
#endif
