/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_rtp_packet_incoming_h //original-code:"logging/rtc_event_log/events/rtc_event_rtp_packet_incoming.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"

namespace webrtc {

RtcEventRtpPacketIncoming::RtcEventRtpPacketIncoming(
    const RtpPacketReceived& packet)
    : packet_length_(packet.size()) {
  header_.CopyHeaderFrom(packet);
}

RtcEventRtpPacketIncoming::RtcEventRtpPacketIncoming(
    const RtcEventRtpPacketIncoming& other)
    : RtcEvent(other.timestamp_us_), packet_length_(other.packet_length_) {
  header_.CopyHeaderFrom(other.header_);
}

RtcEventRtpPacketIncoming::~RtcEventRtpPacketIncoming() = default;

RtcEvent::Type RtcEventRtpPacketIncoming::GetType() const {
  return RtcEvent::Type::RtpPacketIncoming;
}

bool RtcEventRtpPacketIncoming::IsConfigEvent() const {
  return false;
}

std::unique_ptr<RtcEvent> RtcEventRtpPacketIncoming::Copy() const {
  return absl::WrapUnique<RtcEvent>(new RtcEventRtpPacketIncoming(*this));
}

}  // namespace webrtc
