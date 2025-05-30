/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/rtp_rtcp/source/rtp_packet_history.h"

#include <algorithm>
#include <limits>
#include <utility>

#include "modules/rtp_rtcp/source/rtp_packet_to_send.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {
namespace {
constexpr size_t kMinPacketRequestBytes = 50;
// Don't overwrite a packet within one second, or three RTTs, after transmission
// whichever is larger. Instead try to dynamically expand history.
constexpr int64_t kMinPacketDurationMs = 1000;
constexpr int kMinPacketDurationRtt = 3;
}  // namespace
constexpr size_t RtpPacketHistory::kMaxCapacity;

RtpPacketHistory::RtpPacketHistory(Clock* clock)
    : clock_(clock), store_(false), prev_index_(0), rtt_ms_(-1) {}

RtpPacketHistory::~RtpPacketHistory() {}

void RtpPacketHistory::SetStorePacketsStatus(bool enable,
                                             uint16_t number_to_store) {
  rtc::CritScope cs(&critsect_);
  if (enable) {
    if (store_) {
      RTC_LOG(LS_WARNING)
          << "Purging packet history in order to re-set status.";
      Free();
    }
    RTC_DCHECK(!store_);
    Allocate(number_to_store);
  } else {
    Free();
  }
}

void RtpPacketHistory::Allocate(size_t number_to_store) {
  RTC_DCHECK_GT(number_to_store, 0);
  RTC_DCHECK_LE(number_to_store, kMaxCapacity);
  store_ = true;
  stored_packets_.resize(number_to_store);
}

void RtpPacketHistory::Free() {
  if (!store_) {
    return;
  }

  stored_packets_.clear();

  store_ = false;
  prev_index_ = 0;
}

bool RtpPacketHistory::StorePackets() const {
  rtc::CritScope cs(&critsect_);
  return store_;
}

void RtpPacketHistory::PutRtpPacket(std::unique_ptr<RtpPacketToSend> packet,
                                    StorageType type,
                                    bool sent) {
  RTC_DCHECK(packet);
  rtc::CritScope cs(&critsect_);
  if (!store_) {
    return;
  }

  int64_t now_ms = clock_->TimeInMilliseconds();

  // If index we're about to overwrite contains a packet that has not
  // yet been sent (probably pending in paced sender), or if the send time is
  // less than 3 round trip times ago, expand the buffer to avoid overwriting
  // valid data.
  StoredPacket* stored_packet = &stored_packets_[prev_index_];
  int64_t packet_duration_ms =
      std::max(kMinPacketDurationRtt * rtt_ms_, kMinPacketDurationMs);
  if (stored_packet->packet &&
      (stored_packet->send_time == 0 ||
       (rtt_ms_ >= 0 &&
        now_ms - stored_packet->send_time <= packet_duration_ms))) {
    size_t current_size = stored_packets_.size();
    if (current_size < kMaxCapacity) {
      size_t expanded_size = std::max(current_size * 3 / 2, current_size + 1);
      expanded_size = std::min(expanded_size, kMaxCapacity);
      Allocate(expanded_size);
      // Causes discontinuity, but that's OK-ish. FindSeqNum() will still work,
      // but may be slower - at least until buffer has wrapped around once.
      prev_index_ = current_size;
      stored_packet = &stored_packets_[prev_index_];
    }
  }

  // Store packet.
  if (packet->capture_time_ms() <= 0)
    packet->set_capture_time_ms(now_ms);
  stored_packet->sequence_number = packet->SequenceNumber();
  stored_packet->send_time = sent ? now_ms : 0;
  stored_packet->storage_type = type;
  stored_packet->has_been_retransmitted = false;
  stored_packet->packet = std::move(packet);

  prev_index_ = (prev_index_ + 1) % stored_packets_.size();
}

bool RtpPacketHistory::HasRtpPacket(uint16_t sequence_number) const {
  rtc::CritScope cs(&critsect_);
  if (!store_) {
    return false;
  }

  int unused_index = 0;
  return FindSeqNum(sequence_number, &unused_index);
}

std::unique_ptr<RtpPacketToSend> RtpPacketHistory::GetPacketAndSetSendTime(
    uint16_t sequence_number,
    int64_t min_elapsed_time_ms,
    bool retransmit) {
  rtc::CritScope cs(&critsect_);
  if (!store_) {
    return nullptr;
  }

  int index = 0;
  if (!FindSeqNum(sequence_number, &index)) {
    RTC_LOG(LS_WARNING) << "No match for getting seqNum " << sequence_number;
    return nullptr;
  }
  RTC_DCHECK_EQ(sequence_number,
                stored_packets_[index].packet->SequenceNumber());

  // Verify elapsed time since last retrieve, but only for retransmissions and
  // always send packet upon first retransmission request.
  int64_t now = clock_->TimeInMilliseconds();
  if (min_elapsed_time_ms > 0 && retransmit &&
      stored_packets_[index].has_been_retransmitted &&
      ((now - stored_packets_[index].send_time) < min_elapsed_time_ms)) {
    return nullptr;
  }

  if (retransmit) {
    if (stored_packets_[index].storage_type == kDontRetransmit) {
      // No bytes copied since this packet shouldn't be retransmitted.
      return nullptr;
    }
    stored_packets_[index].has_been_retransmitted = true;
  }
  stored_packets_[index].send_time = clock_->TimeInMilliseconds();
  return GetPacket(index);
}

std::unique_ptr<RtpPacketToSend> RtpPacketHistory::GetPacket(int index) const {
  const RtpPacketToSend& stored = *stored_packets_[index].packet;
  return std::unique_ptr<RtpPacketToSend>(new RtpPacketToSend(stored));
}

std::unique_ptr<RtpPacketToSend> RtpPacketHistory::GetBestFittingPacket(
    size_t packet_length) const {
  rtc::CritScope cs(&critsect_);
  if (!store_)
    return nullptr;
  int index = FindBestFittingPacket(packet_length);
  if (index < 0)
    return nullptr;
  return GetPacket(index);
}

bool RtpPacketHistory::FindSeqNum(uint16_t sequence_number, int* index) const {
  if (prev_index_ > 0) {
    *index = prev_index_ - 1;
  } else {
    *index = stored_packets_.size() - 1;  // Wrap.
  }
  uint16_t temp_sequence_number = stored_packets_[*index].sequence_number;

  int idx = *index - (temp_sequence_number - sequence_number);
  if (idx >= 0 && idx < static_cast<int>(stored_packets_.size())) {
    *index = idx;
    temp_sequence_number = stored_packets_[*index].sequence_number;
  }

  if (temp_sequence_number != sequence_number) {
    // We did not found a match, search all.
    for (uint16_t m = 0; m < stored_packets_.size(); m++) {
      if (stored_packets_[m].sequence_number == sequence_number) {
        *index = m;
        temp_sequence_number = stored_packets_[*index].sequence_number;
        break;
      }
    }
  }
  return temp_sequence_number == sequence_number &&
         stored_packets_[*index].packet;
}

int RtpPacketHistory::FindBestFittingPacket(size_t size) const {
  if (size < kMinPacketRequestBytes || stored_packets_.empty())
    return -1;
  size_t min_diff = std::numeric_limits<size_t>::max();
  int best_index = -1;  // Returned unchanged if we don't find anything.
  for (size_t i = 0; i < stored_packets_.size(); ++i) {
    if (!stored_packets_[i].packet)
      continue;
    size_t stored_size = stored_packets_[i].packet->size();
    size_t diff =
        (stored_size > size) ? (stored_size - size) : (size - stored_size);
    if (diff < min_diff) {
      min_diff = diff;
      best_index = static_cast<int>(i);
    }
  }
  return best_index;
}

void RtpPacketHistory::SetRtt(int64_t rtt_ms) {
  rtc::CritScope cs(&critsect_);
  RTC_DCHECK_GE(rtt_ms, 0);
  rtt_ms_ = rtt_ms;
}

}  // namespace webrtc
