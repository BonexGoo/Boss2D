/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__overuse_detector_h //original-code:"modules/remote_bitrate_estimator/overuse_detector.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <string>

#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__bwe_defines_h //original-code:"modules/remote_bitrate_estimator/include/bwe_defines.h"
#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__test__bwe_test_logging_h //original-code:"modules/remote_bitrate_estimator/test/bwe_test_logging.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_minmax_h //original-code:"rtc_base/numerics/safe_minmax.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"

namespace webrtc {

const char kAdaptiveThresholdExperiment[] = "WebRTC-AdaptiveBweThreshold";
const char kEnabledPrefix[] = "Enabled";
const size_t kEnabledPrefixLength = sizeof(kEnabledPrefix) - 1;
const char kDisabledPrefix[] = "Disabled";
const size_t kDisabledPrefixLength = sizeof(kDisabledPrefix) - 1;

const double kMaxAdaptOffsetMs = 15.0;
const double kOverUsingTimeThreshold = 10;
const int kMinNumDeltas = 60;

bool AdaptiveThresholdExperimentIsDisabled() {
  std::string experiment_string =
      webrtc::field_trial::FindFullName(kAdaptiveThresholdExperiment);
  const size_t kMinExperimentLength = kDisabledPrefixLength;
  if (experiment_string.length() < kMinExperimentLength)
    return false;
  return experiment_string.substr(0, kDisabledPrefixLength) == kDisabledPrefix;
}

// Gets thresholds from the experiment name following the format
// "WebRTC-AdaptiveBweThreshold/Enabled-0.5,0.002/".
bool ReadExperimentConstants(double* k_up, double* k_down) {
  std::string experiment_string =
      webrtc::field_trial::FindFullName(kAdaptiveThresholdExperiment);
  const size_t kMinExperimentLength = kEnabledPrefixLength + 3;
  if (experiment_string.length() < kMinExperimentLength ||
      experiment_string.substr(0, kEnabledPrefixLength) != kEnabledPrefix)
    return false;
  return sscanf(experiment_string.substr(kEnabledPrefixLength + 1).c_str(),
                "%lf,%lf", k_up, k_down) == 2;
}

OveruseDetector::OveruseDetector()
    // Experiment is on by default, but can be disabled with finch by setting
    // the field trial string to "WebRTC-AdaptiveBweThreshold/Disabled/".
    : in_experiment_(!AdaptiveThresholdExperimentIsDisabled()),
      k_up_(0.0087),
      k_down_(0.039),
      overusing_time_threshold_(100),
      threshold_(12.5),
      last_update_ms_(-1),
      prev_offset_(0.0),
      time_over_using_(-1),
      overuse_counter_(0),
      hypothesis_(BandwidthUsage::kBwNormal) {
  if (!AdaptiveThresholdExperimentIsDisabled())
    InitializeExperiment();
}

OveruseDetector::~OveruseDetector() {}

BandwidthUsage OveruseDetector::State() const {
  return hypothesis_;
}

BandwidthUsage OveruseDetector::Detect(double offset,
                                       double ts_delta,
                                       int num_of_deltas,
                                       int64_t now_ms) {
  if (num_of_deltas < 2) {
    return BandwidthUsage::kBwNormal;
  }
  const double T = std::min(num_of_deltas, kMinNumDeltas) * offset;
  BWE_TEST_LOGGING_PLOT(1, "T", now_ms, T);
  BWE_TEST_LOGGING_PLOT(1, "threshold", now_ms, threshold_);
  if (T > threshold_) {
    if (time_over_using_ == -1) {
      // Initialize the timer. Assume that we've been
      // over-using half of the time since the previous
      // sample.
      time_over_using_ = ts_delta / 2;
    } else {
      // Increment timer
      time_over_using_ += ts_delta;
    }
    overuse_counter_++;
    if (time_over_using_ > overusing_time_threshold_ && overuse_counter_ > 1) {
      if (offset >= prev_offset_) {
        time_over_using_ = 0;
        overuse_counter_ = 0;
        hypothesis_ = BandwidthUsage::kBwOverusing;
      }
    }
  } else if (T < -threshold_) {
    time_over_using_ = -1;
    overuse_counter_ = 0;
    hypothesis_ = BandwidthUsage::kBwUnderusing;
  } else {
    time_over_using_ = -1;
    overuse_counter_ = 0;
    hypothesis_ = BandwidthUsage::kBwNormal;
  }
  prev_offset_ = offset;

  UpdateThreshold(T, now_ms);

  return hypothesis_;
}

void OveruseDetector::UpdateThreshold(double modified_offset, int64_t now_ms) {
  if (!in_experiment_)
    return;

  if (last_update_ms_ == -1)
    last_update_ms_ = now_ms;

  if (fabs(modified_offset) > threshold_ + kMaxAdaptOffsetMs) {
    // Avoid adapting the threshold to big latency spikes, caused e.g.,
    // by a sudden capacity drop.
    last_update_ms_ = now_ms;
    return;
  }

  const double k = fabs(modified_offset) < threshold_ ? k_down_ : k_up_;
  const int64_t kMaxTimeDeltaMs = 100;
  int64_t time_delta_ms = std::min(now_ms - last_update_ms_, kMaxTimeDeltaMs);
  threshold_ += k * (fabs(modified_offset) - threshold_) * time_delta_ms;
  threshold_ = rtc::SafeClamp(threshold_, 6.f, 600.f);
  last_update_ms_ = now_ms;
}

void OveruseDetector::InitializeExperiment() {
  RTC_DCHECK(in_experiment_);
  double k_up = 0.0;
  double k_down = 0.0;
  overusing_time_threshold_ = kOverUsingTimeThreshold;
  if (ReadExperimentConstants(&k_up, &k_down)) {
    k_up_ = k_up;
    k_down_ = k_down;
  }
}
}  // namespace webrtc
