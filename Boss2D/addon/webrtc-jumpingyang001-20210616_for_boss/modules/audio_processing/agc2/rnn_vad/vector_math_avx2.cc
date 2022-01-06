/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__vector_math_h //original-code:"modules/audio_processing/agc2/rnn_vad/vector_math.h"

#include <immintrin.h>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"

namespace webrtc {
namespace rnn_vad {

float VectorMath::DotProductAvx2(rtc::ArrayView<const float> x,
                                 rtc::ArrayView<const float> y) const {
  RTC_DCHECK(cpu_features_.avx2);
  RTC_DCHECK_EQ(x.size(), y.size());
  __m256 accumulator = _mm256_setzero_ps();
  constexpr int kBlockSizeLog2 = 3;
  constexpr int kBlockSize = 1 << kBlockSizeLog2;
  const int incomplete_block_index = (x.size() >> kBlockSizeLog2)
                                     << kBlockSizeLog2;
  for (int i = 0; i < incomplete_block_index; i += kBlockSize) {
    RTC_DCHECK_LE(i + kBlockSize, x.size());
    const __m256 x_i = _mm256_loadu_ps(&x[i]);
    const __m256 y_i = _mm256_loadu_ps(&y[i]);
    accumulator = _mm256_fmadd_ps(x_i, y_i, accumulator);
  }
  // Reduce `accumulator` by addition.
  __m128 high = _mm256_extractf128_ps(accumulator, 1);
  __m128 low = _mm256_extractf128_ps(accumulator, 0);
  low = _mm_add_ps(high, low);
  high = _mm_movehl_ps(high, low);
  low = _mm_add_ps(high, low);
  high = _mm_shuffle_ps(low, low, 1);
  low = _mm_add_ss(high, low);
  float dot_product = _mm_cvtss_f32(low);
  // Add the result for the last block if incomplete.
  for (int i = incomplete_block_index; i < rtc::dchecked_cast<int>(x.size());
       ++i) {
    dot_product += x[i] * y[i];
  }
  return dot_product;
}

}  // namespace rnn_vad
}  // namespace webrtc
