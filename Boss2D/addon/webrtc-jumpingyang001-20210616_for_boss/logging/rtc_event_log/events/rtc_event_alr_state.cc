/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_alr_state_h //original-code:"logging/rtc_event_log/events/rtc_event_alr_state.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"

namespace webrtc {

RtcEventAlrState::RtcEventAlrState(bool in_alr) : in_alr_(in_alr) {}

RtcEventAlrState::RtcEventAlrState(const RtcEventAlrState& other)
    : RtcEvent(other.timestamp_us_), in_alr_(other.in_alr_) {}

RtcEventAlrState::~RtcEventAlrState() = default;

std::unique_ptr<RtcEventAlrState> RtcEventAlrState::Copy() const {
  return absl::WrapUnique<RtcEventAlrState>(new RtcEventAlrState(*this));
}

}  // namespace webrtc
