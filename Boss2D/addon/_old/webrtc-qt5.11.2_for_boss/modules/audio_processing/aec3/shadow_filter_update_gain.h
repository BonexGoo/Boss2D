/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_SHADOW_FILTER_UPDATE_GAIN_H_
#define MODULES_AUDIO_PROCESSING_AEC3_SHADOW_FILTER_UPDATE_GAIN_H_

#include "modules/audio_processing/aec3/aec3_common.h"
#include "modules/audio_processing/aec3/render_buffer.h"
#include "modules/audio_processing/aec3/render_signal_analyzer.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

// Provides functionality for computing the fixed gain for the shadow filter.
class ShadowFilterUpdateGain {
 public:
  explicit ShadowFilterUpdateGain(
      const EchoCanceller3Config::Filter::ShadowConfiguration& config);

  // Takes action in the case of a known echo path change.
  void HandleEchoPathChange();

  // Computes the gain.
  void Compute(const std::array<float, kFftLengthBy2Plus1>& render_power,
               const RenderSignalAnalyzer& render_signal_analyzer,
               const FftData& E_shadow,
               size_t size_partitions,
               bool saturated_capture_signal,
               FftData* G);

  // Sets a new config.
  void SetConfig(
      const EchoCanceller3Config::Filter::ShadowConfiguration& config) {
    config_ = config;
  }

 private:
  EchoCanceller3Config::Filter::ShadowConfiguration config_;
  // TODO(peah): Check whether this counter should instead be initialized to a
  // large value.
  size_t poor_signal_excitation_counter_ = 0;
  size_t call_counter_ = 0;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_SHADOW_FILTER_UPDATE_GAIN_H_
