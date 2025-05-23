/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_BITRATE_ESTIMATOR_H_
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_BITRATE_ESTIMATOR_H_

#include <stdint.h>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__transport__webrtc_key_value_config_h //original-code:"api/transport/webrtc_key_value_config.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__field_trial_parser_h //original-code:"rtc_base/experiments/field_trial_parser.h"

namespace webrtc {

// Computes a bayesian estimate of the throughput given acks containing
// the arrival time and payload size. Samples which are far from the current
// estimate or are based on few packets are given a smaller weight, as they
// are considered to be more likely to have been caused by, e.g., delay spikes
// unrelated to congestion.
class BitrateEstimator {
 public:
  explicit BitrateEstimator(const WebRtcKeyValueConfig* key_value_config);
  virtual ~BitrateEstimator();
  virtual void Update(Timestamp at_time, DataSize amount, bool in_alr);

  virtual absl::optional<DataRate> bitrate() const;
  absl::optional<DataRate> PeekRate() const;

  virtual void ExpectFastRateChange();

 private:
  float UpdateWindow(int64_t now_ms,
                     int bytes,
                     int rate_window_ms,
                     bool* is_small_sample);
  int sum_;
  FieldTrialConstrained<int> initial_window_ms_;
  FieldTrialConstrained<int> noninitial_window_ms_;
  FieldTrialParameter<double> uncertainty_scale_;
  FieldTrialParameter<double> uncertainty_scale_in_alr_;
  FieldTrialParameter<double> small_sample_uncertainty_scale_;
  FieldTrialParameter<DataSize> small_sample_threshold_;
  FieldTrialParameter<DataRate> uncertainty_symmetry_cap_;
  FieldTrialParameter<DataRate> estimate_floor_;
  int64_t current_window_ms_;
  int64_t prev_time_ms_;
  float bitrate_estimate_kbps_;
  float bitrate_estimate_var_;
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_BITRATE_ESTIMATOR_H_
