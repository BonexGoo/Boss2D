/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__congestion_controller__rtp__send_time_history_h //original-code:"modules/congestion_controller/rtp/send_time_history.h"

#include <algorithm>
#include <utility>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

SendTimeHistory::SendTimeHistory(const Clock* clock,
                                 int64_t packet_age_limit_ms)
    : clock_(clock), packet_age_limit_ms_(packet_age_limit_ms) {}

SendTimeHistory::~SendTimeHistory() {}

void SendTimeHistory::AddAndRemoveOld(const PacketFeedback& packet) {
  int64_t now_ms = clock_->TimeInMilliseconds();
  // Remove old.
  while (!history_.empty() &&
         now_ms - history_.begin()->second.creation_time_ms >
             packet_age_limit_ms_) {
    // TODO(sprang): Warn if erasing (too many) old items?
    RemovePacketBytes(history_.begin()->second);
    history_.erase(history_.begin());
  }

  // Add new.
  int64_t unwrapped_seq_num = seq_num_unwrapper_.Unwrap(packet.sequence_number);
  PacketFeedback packet_copy = packet;
  packet_copy.long_sequence_number = unwrapped_seq_num;
  history_.insert(std::make_pair(unwrapped_seq_num, packet_copy));
  if (packet.send_time_ms >= 0)
    AddPacketBytes(packet_copy);
}

bool SendTimeHistory::OnSentPacket(uint16_t sequence_number,
                                   int64_t send_time_ms) {
  int64_t unwrapped_seq_num = seq_num_unwrapper_.Unwrap(sequence_number);
  auto it = history_.find(unwrapped_seq_num);
  if (it == history_.end())
    return false;
  bool packet_retransmit = it->second.send_time_ms >= 0;
  it->second.send_time_ms = send_time_ms;
  if (!packet_retransmit)
    AddPacketBytes(it->second);
  return true;
}

absl::optional<PacketFeedback> SendTimeHistory::GetPacket(
    uint16_t sequence_number) const {
  int64_t unwrapped_seq_num =
      seq_num_unwrapper_.UnwrapWithoutUpdate(sequence_number);
  absl::optional<PacketFeedback> optional_feedback;
  auto it = history_.find(unwrapped_seq_num);
  if (it != history_.end())
    optional_feedback.emplace(it->second);
  return optional_feedback;
}

bool SendTimeHistory::GetFeedback(PacketFeedback* packet_feedback,
                                  bool remove) {
  RTC_DCHECK(packet_feedback);
  int64_t unwrapped_seq_num =
      seq_num_unwrapper_.Unwrap(packet_feedback->sequence_number);
  UpdateAckedSeqNum(unwrapped_seq_num);
  RTC_DCHECK_GE(*last_ack_seq_num_, 0);
  auto it = history_.find(unwrapped_seq_num);
  if (it == history_.end())
    return false;

  // Save arrival_time not to overwrite it.
  int64_t arrival_time_ms = packet_feedback->arrival_time_ms;
  *packet_feedback = it->second;
  packet_feedback->arrival_time_ms = arrival_time_ms;

  if (remove)
    history_.erase(it);
  return true;
}

size_t SendTimeHistory::GetOutstandingBytes(uint16_t local_net_id,
                                            uint16_t remote_net_id) const {
  auto it = in_flight_bytes_.find({local_net_id, remote_net_id});
  if (it != in_flight_bytes_.end()) {
    return it->second;
  } else {
    return 0;
  }
}

void SendTimeHistory::AddPacketBytes(const PacketFeedback& packet) {
  if (packet.send_time_ms < 0 || packet.payload_size == 0 ||
      (last_ack_seq_num_ && *last_ack_seq_num_ >= packet.long_sequence_number))
    return;
  auto it = in_flight_bytes_.find({packet.local_net_id, packet.remote_net_id});
  if (it != in_flight_bytes_.end()) {
    it->second += packet.payload_size;
  } else {
    in_flight_bytes_[{packet.local_net_id, packet.remote_net_id}] =
        packet.payload_size;
  }
}

void SendTimeHistory::RemovePacketBytes(const PacketFeedback& packet) {
  if (packet.send_time_ms < 0 || packet.payload_size == 0 ||
      (last_ack_seq_num_ && *last_ack_seq_num_ >= packet.long_sequence_number))
    return;
  auto it = in_flight_bytes_.find({packet.local_net_id, packet.remote_net_id});
  if (it != in_flight_bytes_.end()) {
    it->second -= packet.payload_size;
    if (it->second == 0)
      in_flight_bytes_.erase(it);
  }
}

void SendTimeHistory::UpdateAckedSeqNum(int64_t acked_seq_num) {
  if (last_ack_seq_num_ && *last_ack_seq_num_ >= acked_seq_num)
    return;

  auto unacked_it = history_.begin();
  if (last_ack_seq_num_)
    unacked_it = history_.lower_bound(*last_ack_seq_num_);

  auto newly_acked_end = history_.upper_bound(acked_seq_num);
  for (; unacked_it != newly_acked_end; ++unacked_it) {
    RemovePacketBytes(unacked_it->second);
  }
  last_ack_seq_num_.emplace(acked_seq_num);
}
}  // namespace webrtc
