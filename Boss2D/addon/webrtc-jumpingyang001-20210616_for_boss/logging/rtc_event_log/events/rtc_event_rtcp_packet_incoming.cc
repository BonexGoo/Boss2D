/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_rtcp_packet_incoming_h //original-code:"logging/rtc_event_log/events/rtc_event_rtcp_packet_incoming.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"

namespace webrtc {

RtcEventRtcpPacketIncoming::RtcEventRtcpPacketIncoming(
    rtc::ArrayView<const uint8_t> packet)
    : packet_(packet.data(), packet.size()) {}

RtcEventRtcpPacketIncoming::RtcEventRtcpPacketIncoming(
    const RtcEventRtcpPacketIncoming& other)
    : RtcEvent(other.timestamp_us_),
      packet_(other.packet_.data(), other.packet_.size()) {}

RtcEventRtcpPacketIncoming::~RtcEventRtcpPacketIncoming() = default;

std::unique_ptr<RtcEventRtcpPacketIncoming> RtcEventRtcpPacketIncoming::Copy()
    const {
  return absl::WrapUnique<RtcEventRtcpPacketIncoming>(
      new RtcEventRtcpPacketIncoming(*this));
}

}  // namespace webrtc