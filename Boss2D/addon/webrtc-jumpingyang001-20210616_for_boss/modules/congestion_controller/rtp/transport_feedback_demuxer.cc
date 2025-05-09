/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_modules__congestion_controller__rtp__transport_feedback_demuxer_h //original-code:"modules/congestion_controller/rtp/transport_feedback_demuxer.h"
#include BOSS_ABSEILCPP_U_absl__algorithm__container_h //original-code:"absl/algorithm/container.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__transport_feedback_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/transport_feedback.h"

namespace webrtc {
namespace {
static const size_t kMaxPacketsInHistory = 5000;
}
void TransportFeedbackDemuxer::RegisterStreamFeedbackObserver(
    std::vector<uint32_t> ssrcs,
    StreamFeedbackObserver* observer) {
  MutexLock lock(&observers_lock_);
  RTC_DCHECK(observer);
  RTC_DCHECK(absl::c_find_if(observers_, [=](const auto& pair) {
               return pair.second == observer;
             }) == observers_.end());
  observers_.push_back({ssrcs, observer});
}

void TransportFeedbackDemuxer::DeRegisterStreamFeedbackObserver(
    StreamFeedbackObserver* observer) {
  MutexLock lock(&observers_lock_);
  RTC_DCHECK(observer);
  const auto it = absl::c_find_if(
      observers_, [=](const auto& pair) { return pair.second == observer; });
  RTC_DCHECK(it != observers_.end());
  observers_.erase(it);
}

void TransportFeedbackDemuxer::AddPacket(const RtpPacketSendInfo& packet_info) {
  MutexLock lock(&lock_);
  if (packet_info.ssrc != 0) {
    StreamFeedbackObserver::StreamPacketInfo info;
    info.ssrc = packet_info.ssrc;
    info.rtp_sequence_number = packet_info.rtp_sequence_number;
    info.received = false;
    history_.insert(
        {seq_num_unwrapper_.Unwrap(packet_info.transport_sequence_number),
         info});
  }
  while (history_.size() > kMaxPacketsInHistory) {
    history_.erase(history_.begin());
  }
}

void TransportFeedbackDemuxer::OnTransportFeedback(
    const rtcp::TransportFeedback& feedback) {
  std::vector<StreamFeedbackObserver::StreamPacketInfo> stream_feedbacks;
  {
    MutexLock lock(&lock_);
    for (const auto& packet : feedback.GetAllPackets()) {
      int64_t seq_num =
          seq_num_unwrapper_.UnwrapWithoutUpdate(packet.sequence_number());
      auto it = history_.find(seq_num);
      if (it != history_.end()) {
        auto packet_info = it->second;
        packet_info.received = packet.received();
        stream_feedbacks.push_back(packet_info);
        if (packet.received())
          history_.erase(it);
      }
    }
  }

  MutexLock lock(&observers_lock_);
  for (auto& observer : observers_) {
    std::vector<StreamFeedbackObserver::StreamPacketInfo> selected_feedback;
    for (const auto& packet_info : stream_feedbacks) {
      if (absl::c_count(observer.first, packet_info.ssrc) > 0) {
        selected_feedback.push_back(packet_info);
      }
    }
    if (!selected_feedback.empty()) {
      observer.second->OnPacketFeedbackVector(std::move(selected_feedback));
    }
  }
}

}  // namespace webrtc
