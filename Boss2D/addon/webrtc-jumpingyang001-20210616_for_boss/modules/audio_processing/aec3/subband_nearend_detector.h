/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_SUBBAND_NEAREND_DETECTOR_H_
#define MODULES_AUDIO_PROCESSING_AEC3_SUBBAND_NEAREND_DETECTOR_H_

#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__moving_average_h //original-code:"modules/audio_processing/aec3/moving_average.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__nearend_detector_h //original-code:"modules/audio_processing/aec3/nearend_detector.h"

namespace webrtc {
// Class for selecting whether the suppressor is in the nearend or echo state.
class SubbandNearendDetector : public NearendDetector {
 public:
  SubbandNearendDetector(
      const EchoCanceller3Config::Suppressor::SubbandNearendDetection& config,
      size_t num_capture_channels);

  // Returns whether the current state is the nearend state.
  bool IsNearendState() const override { return nearend_state_; }

  // Updates the state selection based on latest spectral estimates.
  void Update(rtc::ArrayView<const std::array<float, kFftLengthBy2Plus1>>
                  nearend_spectrum,
              rtc::ArrayView<const std::array<float, kFftLengthBy2Plus1>>
                  residual_echo_spectrum,
              rtc::ArrayView<const std::array<float, kFftLengthBy2Plus1>>
                  comfort_noise_spectrum,
              bool initial_state) override;

 private:
  const EchoCanceller3Config::Suppressor::SubbandNearendDetection config_;
  const size_t num_capture_channels_;
  std::vector<aec3::MovingAverage> nearend_smoothers_;
  const float one_over_subband_length1_;
  const float one_over_subband_length2_;
  bool nearend_state_ = false;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_SUBBAND_NEAREND_DETECTOR_H_
