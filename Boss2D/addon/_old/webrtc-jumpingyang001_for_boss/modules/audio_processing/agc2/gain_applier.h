/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AGC2_GAIN_APPLIER_H_
#define MODULES_AUDIO_PROCESSING_AGC2_GAIN_APPLIER_H_

#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_frame_view_h //original-code:"modules/audio_processing/include/audio_frame_view.h"

namespace webrtc {
class GainApplier {
 public:
  GainApplier(bool hard_clip_samples, float initial_gain_factor);

  void ApplyGain(AudioFrameView<float> signal);
  void SetGainFactor(float gain_factor);

 private:
  void Initialize(size_t samples_per_channel);

  // Whether to clip samples after gain is applied. If 'true', result
  // will fit in FloatS16 range.
  const bool hard_clip_samples_;
  float last_gain_factor_;

  // If this value is not equal to 'last_gain_factor', gain will be
  // ramped from 'last_gain_factor_' to this value during the next
  // 'ApplyGain'.
  float current_gain_factor_;
  int samples_per_channel_ = -1;
  float inverse_samples_per_channel_ = -1.f;
};
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AGC2_GAIN_APPLIER_H_
