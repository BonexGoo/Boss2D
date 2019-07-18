/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
spect *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_MAIN_FILTER_UPDATE_GAIN_H_
#define MODULES_AUDIO_PROCESSING_AEC3_MAIN_FILTER_UPDATE_GAIN_H_

#include <memory>
#include <vector>

#include "modules/audio_processing/aec3/adaptive_fir_filter.h"
#include "modules/audio_processing/aec3/aec3_common.h"
#include "modules/audio_processing/aec3/echo_path_variability.h"
#include "modules/audio_processing/aec3/render_signal_analyzer.h"
#include "modules/audio_processing/aec3/subtractor_output.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

class ApmDataDumper;

// Provides functionality for computing the adaptive gain for the main filter.
class MainFilterUpdateGain {
 public:
  explicit MainFilterUpdateGain(
      const EchoCanceller3Config::Filter::MainConfiguration& config);
  ~MainFilterUpdateGain();

  // Takes action in the case of a known echo path change.
  void HandleEchoPathChange(const EchoPathVariability& echo_path_variability);

  // Computes the gain.
  void Compute(const std::array<float, kFftLengthBy2Plus1>& render_power,
               const RenderSignalAnalyzer& render_signal_analyzer,
               const SubtractorOutput& subtractor_output,
               const AdaptiveFirFilter& filter,
               bool saturated_capture_signal,
               FftData* gain_fft);

  // Sets a new config.
  void SetConfig(
      const EchoCanceller3Config::Filter::MainConfiguration& config) {
    config_ = config;
  }

 private:
  static int instance_count_;
  std::unique_ptr<ApmDataDumper> data_dumper_;
  EchoCanceller3Config::Filter::MainConfiguration config_;
  std::array<float, kFftLengthBy2Plus1> H_error_;
  size_t poor_excitation_counter_;
  size_t call_counter_ = 0;
  RTC_DISALLOW_COPY_AND_ASSIGN(MainFilterUpdateGain);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_MAIN_FILTER_UPDATE_GAIN_H_
