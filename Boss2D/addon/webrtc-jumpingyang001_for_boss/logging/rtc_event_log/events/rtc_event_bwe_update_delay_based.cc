/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_bwe_update_delay_based_h //original-code:"logging/rtc_event_log/events/rtc_event_bwe_update_delay_based.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__bwe_defines_h //original-code:"modules/remote_bitrate_estimator/include/bwe_defines.h"

namespace webrtc {

RtcEventBweUpdateDelayBased::RtcEventBweUpdateDelayBased(
    int32_t bitrate_bps,
    BandwidthUsage detector_state)
    : bitrate_bps_(bitrate_bps), detector_state_(detector_state) {}

RtcEventBweUpdateDelayBased::RtcEventBweUpdateDelayBased(
    const RtcEventBweUpdateDelayBased& other)
    : RtcEvent(other.timestamp_us_),
      bitrate_bps_(other.bitrate_bps_),
      detector_state_(other.detector_state_) {}

RtcEventBweUpdateDelayBased::~RtcEventBweUpdateDelayBased() = default;

RtcEvent::Type RtcEventBweUpdateDelayBased::GetType() const {
  return RtcEvent::Type::BweUpdateDelayBased;
}

bool RtcEventBweUpdateDelayBased::IsConfigEvent() const {
  return false;
}

std::unique_ptr<RtcEvent> RtcEventBweUpdateDelayBased::Copy() const {
  return absl::WrapUnique<RtcEvent>(new RtcEventBweUpdateDelayBased(*this));
}

}  // namespace webrtc
