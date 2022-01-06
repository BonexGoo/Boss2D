/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_EXPERIMENTS_KEYFRAME_INTERVAL_SETTINGS_H_
#define RTC_BASE_EXPERIMENTS_KEYFRAME_INTERVAL_SETTINGS_H_

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__transport__webrtc_key_value_config_h //original-code:"api/transport/webrtc_key_value_config.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__field_trial_parser_h //original-code:"rtc_base/experiments/field_trial_parser.h"

namespace webrtc {

// TODO(bugs.webrtc.org/10427): Remove and replace with proper configuration
// parameter, or move to using FIR if intent is to avoid triggering multiple
// times to PLIs corresponding to the same request when RTT is large.
class KeyframeIntervalSettings final {
 public:
  static KeyframeIntervalSettings ParseFromFieldTrials();

  // Sender side.
  // The encoded keyframe send rate is <= 1/MinKeyframeSendIntervalMs().
  absl::optional<int> MinKeyframeSendIntervalMs() const;

 private:
  explicit KeyframeIntervalSettings(
      const WebRtcKeyValueConfig* key_value_config);

  FieldTrialOptional<int> min_keyframe_send_interval_ms_;
};

}  // namespace webrtc

#endif  // RTC_BASE_EXPERIMENTS_KEYFRAME_INTERVAL_SETTINGS_H_
