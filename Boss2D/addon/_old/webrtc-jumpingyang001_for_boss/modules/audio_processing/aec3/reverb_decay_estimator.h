/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_REVERB_DECAY_ESTIMATOR_H_
#define MODULES_AUDIO_PROCESSING_AEC3_REVERB_DECAY_ESTIMATOR_H_

#include <array>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include "modules/audio_processing/aec3/aec3_common.h"

namespace webrtc {

class ApmDataDumper;

// Class for estimating the decay of the late reverb.
class ReverbDecayEstimator {
 public:
  explicit ReverbDecayEstimator(const EchoCanceller3Config& config);
  ~ReverbDecayEstimator();
  // Updates the decay estimate.
  void Update(rtc::ArrayView<const float> filter,
              const absl::optional<float>& filter_quality,
              int filter_delay_blocks,
              bool usable_linear_filter,
              bool stationary_signal);
  // Returns the decay for the exponential model.
  float Decay() const { return decay_; }
  // Dumps debug data.
  void Dump(ApmDataDumper* data_dumper) const;

 private:
  void EstimateDecay(rtc::ArrayView<const float> filter);
  void AnalyzeFilter(rtc::ArrayView<const float> filter);

  void ResetDecayEstimation();

  // Class for estimating the decay of the late reverb from the linear filter.
  class LateReverbLinearRegressor {
   public:
    // Resets the estimator to receive a specified number of data points.
    void Reset(int num_data_points);
    // Accumulates estimation data.
    void Accumulate(float z);
    // Estimates the decay.
    float Estimate();
    // Returns whether an estimate is available.
    bool EstimateAvailable() const { return n_ == N_ && N_ != 0; }

   public:
    float nz_ = 0.f;
    float nn_ = 0.f;
    float count_ = 0.f;
    int N_ = 0;
    int n_ = 0;
  };

  // Class for identifying the length of the early reverb from the linear
  // filter.
  class EarlyReverbLengthEstimator {
   public:
    explicit EarlyReverbLengthEstimator(int max_blocks);
    ~EarlyReverbLengthEstimator();

    // Resets the estimator.
    void Reset();
    // Accumulates estimation data.
    void Accumulate(float value, float smoothing);
    // Estimates the size in blocks of the early reverb.
    int Estimate();
    // Dumps debug data.
    void Dump(ApmDataDumper* data_dumper) const;

   private:
    std::vector<float> numerators_;
    std::vector<float> nz_;
    std::vector<float> count_;
    int section_ = 0;
    int section_update_counter_ = 0;
  };

  const int filter_length_blocks_;
  const int filter_length_coefficients_;
  const bool use_adaptive_echo_decay_;
  LateReverbLinearRegressor late_reverb_decay_estimator_;
  EarlyReverbLengthEstimator early_reverb_estimator_;
  int late_reverb_start_;
  int late_reverb_end_;
  int block_to_analyze_ = 0;
  int estimation_region_candidate_size_ = 0;
  bool estimation_region_identified_ = false;
  std::array<float, kMaxAdaptiveFilterLength> previous_gains_;
  float decay_;
  float tail_gain_ = 0.f;
  float smoothing_constant_ = 0.f;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_REVERB_DECAY_ESTIMATOR_H_
