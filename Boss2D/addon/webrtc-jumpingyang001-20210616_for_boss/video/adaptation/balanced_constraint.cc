/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_video__adaptation__balanced_constraint_h //original-code:"video/adaptation/balanced_constraint.h"

#include <string>
#include <utility>

#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__to_queued_task_h //original-code:"rtc_base/task_utils/to_queued_task.h"

namespace webrtc {

BalancedConstraint::BalancedConstraint(
    DegradationPreferenceProvider* degradation_preference_provider)
    : encoder_target_bitrate_bps_(absl::nullopt),
      degradation_preference_provider_(degradation_preference_provider) {
  RTC_DCHECK(degradation_preference_provider_);
  sequence_checker_.Detach();
}

void BalancedConstraint::OnEncoderTargetBitrateUpdated(
    absl::optional<uint32_t> encoder_target_bitrate_bps) {
  RTC_DCHECK_RUN_ON(&sequence_checker_);
  encoder_target_bitrate_bps_ = std::move(encoder_target_bitrate_bps);
}

bool BalancedConstraint::IsAdaptationUpAllowed(
    const VideoStreamInputState& input_state,
    const VideoSourceRestrictions& restrictions_before,
    const VideoSourceRestrictions& restrictions_after) const {
  RTC_DCHECK_RUN_ON(&sequence_checker_);
  // Don't adapt if BalancedDegradationSettings applies and determines this will
  // exceed bitrate constraints.
  if (degradation_preference_provider_->degradation_preference() ==
      DegradationPreference::BALANCED) {
    int frame_size_pixels = input_state.single_active_stream_pixels().value_or(
        input_state.frame_size_pixels().value());
    if (!balanced_settings_.CanAdaptUp(
            input_state.video_codec_type(), frame_size_pixels,
            encoder_target_bitrate_bps_.value_or(0))) {
      return false;
    }
    if (DidIncreaseResolution(restrictions_before, restrictions_after) &&
        !balanced_settings_.CanAdaptUpResolution(
            input_state.video_codec_type(), frame_size_pixels,
            encoder_target_bitrate_bps_.value_or(0))) {
      return false;
    }
  }
  return true;
}

}  // namespace webrtc
