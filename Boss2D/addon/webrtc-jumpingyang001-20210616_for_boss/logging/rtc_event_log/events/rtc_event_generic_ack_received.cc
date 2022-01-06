/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_generic_ack_received_h //original-code:"logging/rtc_event_log/events/rtc_event_generic_ack_received.h"

#include <vector>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_rtc_base__time_utils_h //original-code:"rtc_base/time_utils.h"

namespace webrtc {

std::vector<std::unique_ptr<RtcEventGenericAckReceived>>
RtcEventGenericAckReceived::CreateLogs(
    int64_t packet_number,
    const std::vector<AckedPacket>& acked_packets) {
  std::vector<std::unique_ptr<RtcEventGenericAckReceived>> result;
  int64_t time_us = rtc::TimeMicros();
  result.reserve(acked_packets.size());
  for (const AckedPacket& packet : acked_packets) {
    result.emplace_back(new RtcEventGenericAckReceived(
        time_us, packet_number, packet.packet_number,
        packet.receive_acked_packet_time_ms));
  }
  return result;
}

RtcEventGenericAckReceived::RtcEventGenericAckReceived(
    int64_t timestamp_us,
    int64_t packet_number,
    int64_t acked_packet_number,
    absl::optional<int64_t> receive_acked_packet_time_ms)
    : RtcEvent(timestamp_us),
      packet_number_(packet_number),
      acked_packet_number_(acked_packet_number),
      receive_acked_packet_time_ms_(receive_acked_packet_time_ms) {}

std::unique_ptr<RtcEventGenericAckReceived> RtcEventGenericAckReceived::Copy()
    const {
  return absl::WrapUnique(new RtcEventGenericAckReceived(*this));
}

RtcEventGenericAckReceived::RtcEventGenericAckReceived(
    const RtcEventGenericAckReceived& packet) = default;

RtcEventGenericAckReceived::~RtcEventGenericAckReceived() = default;

}  // namespace webrtc
