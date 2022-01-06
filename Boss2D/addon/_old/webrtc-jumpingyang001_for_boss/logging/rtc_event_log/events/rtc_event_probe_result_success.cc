/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_probe_result_success_h //original-code:"logging/rtc_event_log/events/rtc_event_probe_result_success.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"

namespace webrtc {

RtcEventProbeResultSuccess::RtcEventProbeResultSuccess(int32_t id,
                                                       int32_t bitrate_bps)
    : id_(id), bitrate_bps_(bitrate_bps) {}

RtcEventProbeResultSuccess::RtcEventProbeResultSuccess(
    const RtcEventProbeResultSuccess& other)
    : RtcEvent(other.timestamp_us_),
      id_(other.id_),
      bitrate_bps_(other.bitrate_bps_) {}

RtcEvent::Type RtcEventProbeResultSuccess::GetType() const {
  return RtcEvent::Type::ProbeResultSuccess;
}

bool RtcEventProbeResultSuccess::IsConfigEvent() const {
  return false;
}

std::unique_ptr<RtcEvent> RtcEventProbeResultSuccess::Copy() const {
  return absl::WrapUnique<RtcEvent>(new RtcEventProbeResultSuccess(*this));
}

}  // namespace webrtc
