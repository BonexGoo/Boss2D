/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AGC2_NOISE_LEVEL_ESTIMATOR_H_
#define MODULES_AUDIO_PROCESSING_AGC2_NOISE_LEVEL_ESTIMATOR_H_

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__signal_classifier_h //original-code:"modules/audio_processing/agc2/signal_classifier.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_frame_view_h //original-code:"modules/audio_processing/include/audio_frame_view.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {
class ApmDataDumper;

class NoiseLevelEstimator {
 public:
  NoiseLevelEstimator(ApmDataDumper* data_dumper);
  ~NoiseLevelEstimator();
  // Returns the estimated noise level in dBFS.
  float Analyze(const AudioFrameView<const float>& frame);

 private:
  void Initialize(int sample_rate_hz);

  int sample_rate_hz_;
  float min_noise_energy_;
  bool first_update_;
  float noise_energy_;
  int noise_energy_hold_counter_;
  SignalClassifier signal_classifier_;

  RTC_DISALLOW_COPY_AND_ASSIGN(NoiseLevelEstimator);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AGC2_NOISE_LEVEL_ESTIMATOR_H_
