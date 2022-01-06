/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AGC2_ADAPTIVE_AGC_H_
#define MODULES_AUDIO_PROCESSING_AGC2_ADAPTIVE_AGC_H_

#include <memory>

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__adaptive_digital_gain_applier_h //original-code:"modules/audio_processing/agc2/adaptive_digital_gain_applier.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__adaptive_mode_level_estimator_h //original-code:"modules/audio_processing/agc2/adaptive_mode_level_estimator.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__noise_level_estimator_h //original-code:"modules/audio_processing/agc2/noise_level_estimator.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__saturation_protector_h //original-code:"modules/audio_processing/agc2/saturation_protector.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__vad_with_level_h //original-code:"modules/audio_processing/agc2/vad_with_level.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_frame_view_h //original-code:"modules/audio_processing/include/audio_frame_view.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"

namespace webrtc {
class ApmDataDumper;

// Adaptive digital gain controller.
// TODO(crbug.com/webrtc/7494): Rename to `AdaptiveDigitalGainController`.
class AdaptiveAgc {
 public:
  AdaptiveAgc(
      ApmDataDumper* apm_data_dumper,
      const AudioProcessing::Config::GainController2::AdaptiveDigital& config);
  ~AdaptiveAgc();

  void Initialize(int sample_rate_hz, int num_channels);

  // TODO(crbug.com/webrtc/7494): Add `SetLimiterEnvelope()`.

  // Analyzes `frame` and applies a digital adaptive gain to it. Takes into
  // account the envelope measured by the limiter.
  // TODO(crbug.com/webrtc/7494): Remove `limiter_envelope`.
  void Process(AudioFrameView<float> frame, float limiter_envelope);

  // Handles a gain change applied to the input signal (e.g., analog gain).
  void HandleInputGainChange();

 private:
  AdaptiveModeLevelEstimator speech_level_estimator_;
  VadLevelAnalyzer vad_;
  AdaptiveDigitalGainApplier gain_controller_;
  ApmDataDumper* const apm_data_dumper_;
  std::unique_ptr<NoiseLevelEstimator> noise_level_estimator_;
  std::unique_ptr<SaturationProtector> saturation_protector_;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AGC2_ADAPTIVE_AGC_H_
