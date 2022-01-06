/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_probe_result_failure_h //original-code:"logging/rtc_event_log/events/rtc_event_probe_result_failure.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"

namespace webrtc {

RtcEventProbeResultFailure::RtcEventProbeResultFailure(
    int32_t id,
    ProbeFailureReason failure_reason)
    : id_(id), failure_reason_(failure_reason) {}

RtcEventProbeResultFailure::RtcEventProbeResultFailure(
    const RtcEventProbeResultFailure& other)
    : RtcEvent(other.timestamp_us_),
      id_(other.id_),
      failure_reason_(other.failure_reason_) {}

std::unique_ptr<RtcEventProbeResultFailure> RtcEventProbeResultFailure::Copy()
    const {
  return absl::WrapUnique<RtcEventProbeResultFailure>(
      new RtcEventProbeResultFailure(*this));
}

}  // namespace webrtc
