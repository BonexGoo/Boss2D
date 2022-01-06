/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__experiments__normalize_simulcast_size_experiment_h //original-code:"rtc_base/experiments/normalize_simulcast_size_experiment.h"

#include <stdio.h>

#include <string>

#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"

namespace webrtc {
namespace {
constexpr char kFieldTrial[] = "WebRTC-NormalizeSimulcastResolution";
constexpr int kMinSetting = 0;
constexpr int kMaxSetting = 5;
}  // namespace

absl::optional<int> NormalizeSimulcastSizeExperiment::GetBase2Exponent() {
  if (!webrtc::field_trial::IsEnabled(kFieldTrial))
    return absl::nullopt;

  const std::string group = webrtc::field_trial::FindFullName(kFieldTrial);
  if (group.empty())
    return absl::nullopt;

  int exponent;
  if (sscanf(group.c_str(), "Enabled-%d", &exponent) != 1) {
    RTC_LOG(LS_WARNING) << "No parameter provided.";
    return absl::nullopt;
  }

  if (exponent < kMinSetting || exponent > kMaxSetting) {
    RTC_LOG(LS_WARNING) << "Unsupported exp value provided, value ignored.";
    return absl::nullopt;
  }

  return absl::optional<int>(exponent);
}

}  // namespace webrtc
