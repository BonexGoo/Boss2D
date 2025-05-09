/* Copyright (c) 2014-2015 Xiph.Org Foundation
   Written by Viswanath Puttagunta */
/**
   @file celt_neon_intr.c
   @brief ARM Neon Intrinsic optimizations for celt
 */

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

#ifdef HAVE_CONFIG_H
#include BOSS_OPUS_U_config_h //original-code:"config.h"
#endif

#include <arm_neon.h>
#include "../pitch.h"

#if !defined(FIXED_POINT)
/*
 * Function: xcorr_kernel_neon_float
 * ---------------------------------
 * Computes 4 correlation values and stores them in sum[4]
 */
static void xcorr_kernel_neon_float(const float32_t *x, const float32_t *y,
      float32_t sum[4], int len) {
   float32x4_t YY[3];
   float32x4_t YEXT[3];
   float32x4_t XX[2];
   float32x2_t XX_2;
   float32x4_t SUMM;
   const float32_t *xi = x;
   const float32_t *yi = y;

   celt_assert(len>0);

   YY[0] = vld1q_f32(yi);
   SUMM = vdupq_n_f32(0);

   /* Consume 8 elements in x vector and 12 elements in y
    * vector. However, the 12'th element never really gets
    * touched in this loop. So, if len == 8, then we only
    * must access y[0] to y[10]. y[11] must not be accessed
    * hence make sure len > 8 and not len >= 8
    */
   while (len > 8) {
      yi += 4;
      YY[1] = vld1q_f32(yi);
      yi += 4;
      YY[2] = vld1q_f32(yi);

      XX[0] = vld1q_f32(xi);
      xi += 4;
      XX[1] = vld1q_f32(xi);
      xi += 4;

      SUMM = vmlaq_lane_f32(SUMM, YY[0], vget_low_f32(XX[0]), 0);
      YEXT[0] = vextq_f32(YY[0], YY[1], 1);
      SUMM = vmlaq_lane_f32(SUMM, YEXT[0], vget_low_f32(XX[0]), 1);
      YEXT[1] = vextq_f32(YY[0], YY[1], 2);
      SUMM = vmlaq_lane_f32(SUMM, YEXT[1], vget_high_f32(XX[0]), 0);
      YEXT[2] = vextq_f32(YY[0], YY[1], 3);
      SUMM = vmlaq_lane_f32(SUMM, YEXT[2], vget_high_f32(XX[0]), 1);

      SUMM = vmlaq_lane_f32(SUMM, YY[1], vget_low_f32(XX[1]), 0);
      YEXT[0] = vextq_f32(YY[1], YY[2], 1);
      SUMM = vmlaq_lane_f32(SUMM, YEXT[0], vget_low_f32(XX[1]), 1);
      YEXT[1] = vextq_f32(YY[1], YY[2], 2);
      SUMM = vmlaq_lane_f32(SUMM, YEXT[1], vget_high_f32(XX[1]), 0);
      YEXT[2] = vextq_f32(YY[1], YY[2], 3);
      SUMM = vmlaq_lane_f32(SUMM, YEXT[2], vget_high_f32(XX[1]), 1);

      YY[0] = YY[2];
      len -= 8;
   }

   /* Consume 4 elements in x vector and 8 elements in y
    * vector. However, the 8'th element in y never really gets
    * touched in this loop. So, if len == 4, then we only
    * must access y[0] to y[6]. y[7] must not be accessed
    * hence make sure len>4 and not len>=4
    */
   if (len > 4) {
      yi += 4;
      YY[1] = vld1q_f32(yi);

      XX[0] = vld1q_f32(xi);
      xi += 4;

      SUMM = vmlaq_lane_f32(SUMM, YY[0], vget_low_f32(XX[0]), 0);
      YEXT[0] = vextq_f32(YY[0], YY[1], 1);
      SUMM = vmlaq_lane_f32(SUMM, YEXT[0], vget_low_f32(XX[0]), 1);
      YEXT[1] = vextq_f32(YY[0], YY[1], 2);
      SUMM = vmlaq_lane_f32(SUMM, YEXT[1], vget_high_f32(XX[0]), 0);
      YEXT[2] = vextq_f32(YY[0], YY[1], 3);
      SUMM = vmlaq_lane_f32(SUMM, YEXT[2], vget_high_f32(XX[0]), 1);

      YY[0] = YY[1];
      len -= 4;
   }

   while (--len > 0) {
      XX_2 = vld1_dup_f32(xi++);
      SUMM = vmlaq_lane_f32(SUMM, YY[0], XX_2, 0);
      YY[0]= vld1q_f32(++yi);
   }

   XX_2 = vld1_dup_f32(xi);
   SUMM = vmlaq_lane_f32(SUMM, YY[0], XX_2, 0);

   vst1q_f32(sum, SUMM);
}

/*
 * Function: xcorr_kernel_neon_float_process1
 * ---------------------------------
 * Computes single correlation values and stores in *sum
 */
static void xcorr_kernel_neon_float_process1(const float32_t *x,
      const float32_t *y, float32_t *sum, int len) {
   float32x4_t XX[4];
   float32x4_t YY[4];
   float32x2_t XX_2;
   float32x2_t YY_2;
   float32x4_t SUMM;
   float32x2_t SUMM_2[2];
   const float32_t *xi = x;
   const float32_t *yi = y;

   SUMM = vdupq_n_f32(0);

   /* Work on 16 values per iteration */
   while (len >= 16) {
      XX[0] = vld1q_f32(xi);
      xi += 4;
      XX[1] = vld1q_f32(xi);
      xi += 4;
      XX[2] = vld1q_f32(xi);
      xi += 4;
      XX[3] = vld1q_f32(xi);
      xi += 4;

      YY[0] = vld1q_f32(yi);
      yi += 4;
      YY[1] = vld1q_f32(yi);
      yi += 4;
      YY[2] = vld1q_f32(yi);
      yi += 4;
      YY[3] = vld1q_f32(yi);
      yi += 4;

      SUMM = vmlaq_f32(SUMM, YY[0], XX[0]);
      SUMM = vmlaq_f32(SUMM, YY[1], XX[1]);
      SUMM = vmlaq_f32(SUMM, YY[2], XX[2]);
      SUMM = vmlaq_f32(SUMM, YY[3], XX[3]);
      len -= 16;
   }

   /* Work on 8 values */
   if (len >= 8) {
      XX[0] = vld1q_f32(xi);
      xi += 4;
      XX[1] = vld1q_f32(xi);
      xi += 4;

      YY[0] = vld1q_f32(yi);
      yi += 4;
      YY[1] = vld1q_f32(yi);
      yi += 4;

      SUMM = vmlaq_f32(SUMM, YY[0], XX[0]);
      SUMM = vmlaq_f32(SUMM, YY[1], XX[1]);
      len -= 8;
   }

   /* Work on 4 values */
   if (len >= 4) {
      XX[0] = vld1q_f32(xi);
      xi += 4;
      YY[0] = vld1q_f32(yi);
      yi += 4;
      SUMM = vmlaq_f32(SUMM, YY[0], XX[0]);
      len -= 4;
   }

   /* Start accumulating results */
   SUMM_2[0] = vget_low_f32(SUMM);
   if (len >= 2) {
      /* While at it, consume 2 more values if available */
      XX_2 = vld1_f32(xi);
      xi += 2;
      YY_2 = vld1_f32(yi);
      yi += 2;
      SUMM_2[0] = vmla_f32(SUMM_2[0], YY_2, XX_2);
      len -= 2;
   }
   SUMM_2[1] = vget_high_f32(SUMM);
   SUMM_2[0] = vadd_f32(SUMM_2[0], SUMM_2[1]);
   SUMM_2[0] = vpadd_f32(SUMM_2[0], SUMM_2[0]);
   /* Ok, now we have result accumulated in SUMM_2[0].0 */

   if (len > 0) {
      /* Case when you have one value left */
      XX_2 = vld1_dup_f32(xi);
      YY_2 = vld1_dup_f32(yi);
      SUMM_2[0] = vmla_f32(SUMM_2[0], XX_2, YY_2);
   }

   vst1_lane_f32(sum, SUMM_2[0], 0);
}

void celt_pitch_xcorr_float_neon(const opus_val16 *_x, const opus_val16 *_y,
                        opus_val32 *xcorr, int len, int max_pitch) {
   int i;
   celt_assert(max_pitch > 0);
   celt_assert((((unsigned char *)_x-(unsigned char *)NULL)&3)==0);

   for (i = 0; i < (max_pitch-3); i += 4) {
      xcorr_kernel_neon_float((const float32_t *)_x, (const float32_t *)_y+i,
            (float32_t *)xcorr+i, len);
   }

   /* In case max_pitch isn't multiple of 4
    * compute single correlation value per iteration
    */
   for (; i < max_pitch; i++) {
      xcorr_kernel_neon_float_process1((const float32_t *)_x,
            (const float32_t *)_y+i, (float32_t *)xcorr+i, len);
   }
}
#endif
