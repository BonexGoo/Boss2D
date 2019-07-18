/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_rtp_packet_outgoing_h //original-code:"logging/rtc_event_log/events/rtc_event_rtp_packet_outgoing.h"

#include "modules/rtp_rtcp/source/rtp_packet_to_send.h"

namespace webrtc {

RtcEventRtpPacketOutgoing::RtcEventRtpPacketOutgoing(
    const RtpPacketToSend& packet,
    int probe_cluster_id)
    : packet_length_(packet.size()), probe_cluster_id_(probe_cluster_id) {
  header_.CopyHeaderFrom(packet);
}

RtcEventRtpPacketOutgoing::~RtcEventRtpPacketOutgoing() = default;

RtcEvent::Type RtcEventRtpPacketOutgoing::GetType() const {
  return RtcEvent::Type::RtpPacketOutgoing;
}

bool RtcEventRtpPacketOutgoing::IsConfigEvent() const {
  return false;
}

}  // namespace webrtc
