/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AGC2_RNN_VAD_PITCH_SEARCH_H_
#define MODULES_AUDIO_PROCESSING_AGC2_RNN_VAD_PITCH_SEARCH_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_common_audio__real_fourier_h //original-code:"common_audio/real_fourier.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__common_h //original-code:"modules/audio_processing/agc2/rnn_vad/common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__pitch_info_h //original-code:"modules/audio_processing/agc2/rnn_vad/pitch_info.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__pitch_search_internal_h //original-code:"modules/audio_processing/agc2/rnn_vad/pitch_search_internal.h"

namespace webrtc {
namespace rnn_vad {

// Pitch estimator.
class PitchEstimator {
 public:
  PitchEstimator();
  PitchEstimator(const PitchEstimator&) = delete;
  PitchEstimator& operator=(const PitchEstimator&) = delete;
  ~PitchEstimator();
  // Estimates the pitch period and gain. Returns the pitch estimation data for
  // 48 kHz.
  PitchInfo Estimate(rtc::ArrayView<const float, kBufSize24kHz> pitch_buf);

 private:
  PitchInfo last_pitch_48kHz_;
  std::unique_ptr<RealFourier> fft_;
  std::vector<float> pitch_buf_decimated_;
  rtc::ArrayView<float, kBufSize12kHz> pitch_buf_decimated_view_;
  std::vector<float> auto_corr_;
  rtc::ArrayView<float, kNumInvertedLags12kHz> auto_corr_view_;
};

}  // namespace rnn_vad
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AGC2_RNN_VAD_PITCH_SEARCH_H_
