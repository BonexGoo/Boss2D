/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__alr_detector_h //original-code:"modules/congestion_controller/goog_cc/alr_detector.h"

#include <algorithm>
#include <cstdio>
#include <string>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_alr_state_h //original-code:"logging/rtc_event_log/events/rtc_event_alr_state.h"
#include BOSS_WEBRTC_U_logging__rtc_event_log__rtc_event_log_h //original-code:"logging/rtc_event_log/rtc_event_log.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__alr_experiment_h //original-code:"rtc_base/experiments/alr_experiment.h"
#include BOSS_WEBRTC_U_rtc_base__format_macros_h //original-code:"rtc_base/format_macros.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__timeutils_h //original-code:"rtc_base/timeutils.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"

namespace webrtc {
AlrDetector::AlrDetector() : AlrDetector(nullptr) {}

AlrDetector::AlrDetector(RtcEventLog* event_log)
    : bandwidth_usage_percent_(kDefaultAlrBandwidthUsagePercent),
      alr_start_budget_level_percent_(kDefaultAlrStartBudgetLevelPercent),
      alr_stop_budget_level_percent_(kDefaultAlrStopBudgetLevelPercent),
      alr_budget_(0, true),
      event_log_(event_log) {
  RTC_CHECK(AlrExperimentSettings::MaxOneFieldTrialEnabled());
  absl::optional<AlrExperimentSettings> experiment_settings =
      AlrExperimentSettings::CreateFromFieldTrial(
          AlrExperimentSettings::kScreenshareProbingBweExperimentName);
  if (!experiment_settings) {
    experiment_settings = AlrExperimentSettings::CreateFromFieldTrial(
        AlrExperimentSettings::kStrictPacingAndProbingExperimentName);
  }
  if (experiment_settings) {
    alr_stop_budget_level_percent_ =
        experiment_settings->alr_stop_budget_level_percent;
    alr_start_budget_level_percent_ =
        experiment_settings->alr_start_budget_level_percent;
    bandwidth_usage_percent_ = experiment_settings->alr_bandwidth_usage_percent;
  }
}

AlrDetector::~AlrDetector() {}

void AlrDetector::OnBytesSent(size_t bytes_sent, int64_t send_time_ms) {
  if (!last_send_time_ms_.has_value()) {
    last_send_time_ms_ = send_time_ms;
    // Since the duration for sending the bytes is unknwon, return without
    // updating alr state.
    return;
  }
  int64_t delta_time_ms = send_time_ms - *last_send_time_ms_;
  last_send_time_ms_ = send_time_ms;

  alr_budget_.UseBudget(bytes_sent);
  alr_budget_.IncreaseBudget(delta_time_ms);
  bool state_changed = false;
  if (alr_budget_.budget_level_percent() > alr_start_budget_level_percent_ &&
      !alr_started_time_ms_) {
    alr_started_time_ms_.emplace(rtc::TimeMillis());
    state_changed = true;
  } else if (alr_budget_.budget_level_percent() <
                 alr_stop_budget_level_percent_ &&
             alr_started_time_ms_) {
    state_changed = true;
    alr_started_time_ms_.reset();
  }
  if (event_log_ && state_changed) {
    event_log_->Log(
        absl::make_unique<RtcEventAlrState>(alr_started_time_ms_.has_value()));
  }
}

void AlrDetector::SetEstimatedBitrate(int bitrate_bps) {
  RTC_DCHECK(bitrate_bps);
  const auto target_rate_kbps = static_cast<int64_t>(bitrate_bps) *
                                bandwidth_usage_percent_ / (1000 * 100);
  alr_budget_.set_target_rate_kbps(rtc::dchecked_cast<int>(target_rate_kbps));
}

absl::optional<int64_t> AlrDetector::GetApplicationLimitedRegionStartTime()
    const {
  return alr_started_time_ms_;
}
}  // namespace webrtc
