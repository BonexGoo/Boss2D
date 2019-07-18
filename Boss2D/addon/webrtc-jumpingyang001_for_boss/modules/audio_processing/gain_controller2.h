/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_GAIN_CONTROLLER2_H_
#define MODULES_AUDIO_PROCESSING_GAIN_CONTROLLER2_H_

#include <memory>
#include <string>

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__adaptive_agc_h //original-code:"modules/audio_processing/agc2/adaptive_agc.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__fixed_gain_controller_h //original-code:"modules/audio_processing/agc2/fixed_gain_controller.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

class ApmDataDumper;
class AudioBuffer;

// Gain Controller 2 aims to automatically adjust levels by acting on the
// microphone gain and/or applying digital gain.
class GainController2 {
 public:
  GainController2();
  ~GainController2();

  void Initialize(int sample_rate_hz);
  void Process(AudioBuffer* audio);
  void NotifyAnalogLevel(int level);

  void ApplyConfig(const AudioProcessing::Config::GainController2& config);
  static bool Validate(const AudioProcessing::Config::GainController2& config);
  static std::string ToString(
      const AudioProcessing::Config::GainController2& config);

 private:
  static int instance_count_;
  std::unique_ptr<ApmDataDumper> data_dumper_;
  FixedGainController fixed_gain_controller_;
  AudioProcessing::Config::GainController2 config_;
  AdaptiveAgc adaptive_agc_;
  int analog_level_ = -1;

  RTC_DISALLOW_COPY_AND_ASSIGN(GainController2);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_GAIN_CONTROLLER2_H_
