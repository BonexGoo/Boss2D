/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__capture_clock_offset_updater_h //original-code:"modules/rtp_rtcp/source/capture_clock_offset_updater.h"

namespace webrtc {

absl::optional<int64_t>
CaptureClockOffsetUpdater::AdjustEstimatedCaptureClockOffset(
    absl::optional<int64_t> remote_capture_clock_offset) const {
  if (remote_capture_clock_offset == absl::nullopt ||
      remote_to_local_clock_offset_ == absl::nullopt) {
    return absl::nullopt;
  }

  // Do calculations as "unsigned" to make overflows deterministic.
  return static_cast<uint64_t>(*remote_capture_clock_offset) +
         static_cast<uint64_t>(*remote_to_local_clock_offset_);
}

void CaptureClockOffsetUpdater::SetRemoteToLocalClockOffset(
    absl::optional<int64_t> offset_q32x32) {
  remote_to_local_clock_offset_ = offset_q32x32;
}

}  // namespace webrtc
