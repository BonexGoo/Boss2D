/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VIDEO_ADAPTATION_BITRATE_CONSTRAINT_H_
#define VIDEO_ADAPTATION_BITRATE_CONSTRAINT_H_

#include <string>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_call__adaptation__adaptation_constraint_h //original-code:"call/adaptation/adaptation_constraint.h"
#include BOSS_WEBRTC_U_call__adaptation__encoder_settings_h //original-code:"call/adaptation/encoder_settings.h"
#include BOSS_WEBRTC_U_call__adaptation__video_source_restrictions_h //original-code:"call/adaptation/video_source_restrictions.h"
#include BOSS_WEBRTC_U_call__adaptation__video_stream_input_state_h //original-code:"call/adaptation/video_stream_input_state.h"
#include BOSS_WEBRTC_U_rtc_base__system__no_unique_address_h //original-code:"rtc_base/system/no_unique_address.h"

namespace webrtc {

class BitrateConstraint : public AdaptationConstraint {
 public:
  BitrateConstraint();
  ~BitrateConstraint() override = default;

  void OnEncoderSettingsUpdated(
      absl::optional<EncoderSettings> encoder_settings);
  void OnEncoderTargetBitrateUpdated(
      absl::optional<uint32_t> encoder_target_bitrate_bps);

  // AdaptationConstraint implementation.
  std::string Name() const override { return "BitrateConstraint"; }
  bool IsAdaptationUpAllowed(
      const VideoStreamInputState& input_state,
      const VideoSourceRestrictions& restrictions_before,
      const VideoSourceRestrictions& restrictions_after) const override;

 private:
  RTC_NO_UNIQUE_ADDRESS SequenceChecker sequence_checker_;
  absl::optional<EncoderSettings> encoder_settings_
      RTC_GUARDED_BY(&sequence_checker_);
  absl::optional<uint32_t> encoder_target_bitrate_bps_
      RTC_GUARDED_BY(&sequence_checker_);
};

}  // namespace webrtc

#endif  // VIDEO_ADAPTATION_BITRATE_CONSTRAINT_H_
