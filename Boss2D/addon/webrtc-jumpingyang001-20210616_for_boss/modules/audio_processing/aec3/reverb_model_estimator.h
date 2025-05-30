/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_REVERB_MODEL_ESTIMATOR_H_
#define MODULES_AUDIO_PROCESSING_AEC3_REVERB_MODEL_ESTIMATOR_H_

#include <array>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_common_h //original-code:"modules/audio_processing/aec3/aec3_common.h"  // kFftLengthBy2Plus1
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__reverb_decay_estimator_h //original-code:"modules/audio_processing/aec3/reverb_decay_estimator.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__reverb_frequency_response_h //original-code:"modules/audio_processing/aec3/reverb_frequency_response.h"

namespace webrtc {

class ApmDataDumper;

// Class for estimating the model parameters for the reverberant echo.
class ReverbModelEstimator {
 public:
  ReverbModelEstimator(const EchoCanceller3Config& config,
                       size_t num_capture_channels);
  ~ReverbModelEstimator();

  // Updates the estimates based on new data.
  void Update(
      rtc::ArrayView<const std::vector<float>> impulse_responses,
      rtc::ArrayView<const std::vector<std::array<float, kFftLengthBy2Plus1>>>
          frequency_responses,
      rtc::ArrayView<const absl::optional<float>> linear_filter_qualities,
      rtc::ArrayView<const int> filter_delays_blocks,
      const std::vector<bool>& usable_linear_estimates,
      bool stationary_block);

  // Returns the exponential decay of the reverberant echo.
  // TODO(peah): Correct to properly support multiple channels.
  float ReverbDecay() const { return reverb_decay_estimators_[0]->Decay(); }

  // Return the frequency response of the reverberant echo.
  // TODO(peah): Correct to properly support multiple channels.
  rtc::ArrayView<const float> GetReverbFrequencyResponse() const {
    return reverb_frequency_responses_[0].FrequencyResponse();
  }

  // Dumps debug data.
  void Dump(ApmDataDumper* data_dumper) const {
    reverb_decay_estimators_[0]->Dump(data_dumper);
  }

 private:
  std::vector<std::unique_ptr<ReverbDecayEstimator>> reverb_decay_estimators_;
  std::vector<ReverbFrequencyResponse> reverb_frequency_responses_;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_REVERB_MODEL_ESTIMATOR_H_
