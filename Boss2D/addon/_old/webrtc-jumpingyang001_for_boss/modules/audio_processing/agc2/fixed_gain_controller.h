/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AGC2_FIXED_GAIN_CONTROLLER_H_
#define MODULES_AUDIO_PROCESSING_AGC2_FIXED_GAIN_CONTROLLER_H_

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__gain_curve_applier_h //original-code:"modules/audio_processing/agc2/gain_curve_applier.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_frame_view_h //original-code:"modules/audio_processing/include/audio_frame_view.h"

namespace webrtc {
class ApmDataDumper;

class FixedGainController {
 public:
  explicit FixedGainController(ApmDataDumper* apm_data_dumper);

  void Process(AudioFrameView<float> signal);

  // Rate and gain may be changed at any time (but not concurrently
  // with any other method call).
  void SetGain(float gain_to_apply_db);
  void SetSampleRate(size_t sample_rate_hz);

 private:
  float gain_to_apply_ = 1.f;
  ApmDataDumper* apm_data_dumper_ = nullptr;
  GainCurveApplier gain_curve_applier_;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AGC2_FIXED_GAIN_CONTROLLER_H_
