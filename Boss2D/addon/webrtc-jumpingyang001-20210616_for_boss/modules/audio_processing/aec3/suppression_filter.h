/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_SUPPRESSION_FILTER_H_
#define MODULES_AUDIO_PROCESSING_AEC3_SUPPRESSION_FILTER_H_

#include <array>
#include <vector>

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_common_h //original-code:"modules/audio_processing/aec3/aec3_common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_fft_h //original-code:"modules/audio_processing/aec3/aec3_fft.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__fft_data_h //original-code:"modules/audio_processing/aec3/fft_data.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

class SuppressionFilter {
 public:
  SuppressionFilter(Aec3Optimization optimization,
                    int sample_rate_hz,
                    size_t num_capture_channels_);
  ~SuppressionFilter();
  void ApplyGain(rtc::ArrayView<const FftData> comfort_noise,
                 rtc::ArrayView<const FftData> comfort_noise_high_bands,
                 const std::array<float, kFftLengthBy2Plus1>& suppression_gain,
                 float high_bands_gain,
                 rtc::ArrayView<const FftData> E_lowest_band,
                 std::vector<std::vector<std::vector<float>>>* e);

 private:
  const Aec3Optimization optimization_;
  const int sample_rate_hz_;
  const size_t num_capture_channels_;
  const Aec3Fft fft_;
  std::vector<std::vector<std::array<float, kFftLengthBy2>>> e_output_old_;
  RTC_DISALLOW_COPY_AND_ASSIGN(SuppressionFilter);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_SUPPRESSION_FILTER_H_
