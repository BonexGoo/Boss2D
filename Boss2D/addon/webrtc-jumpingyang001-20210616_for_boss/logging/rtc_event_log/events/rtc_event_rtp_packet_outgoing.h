/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_RTP_PACKET_OUTGOING_H_
#define LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_RTP_PACKET_OUTGOING_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_h //original-code:"api/rtc_event_log/rtc_event.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_h //original-code:"modules/rtp_rtcp/source/rtp_packet.h"

namespace webrtc {

class RtpPacketToSend;

class RtcEventRtpPacketOutgoing final : public RtcEvent {
 public:
  static constexpr Type kType = Type::RtpPacketOutgoing;

  RtcEventRtpPacketOutgoing(const RtpPacketToSend& packet,
                            int probe_cluster_id);
  ~RtcEventRtpPacketOutgoing() override;

  Type GetType() const override { return kType; }
  bool IsConfigEvent() const override { return false; }

  std::unique_ptr<RtcEventRtpPacketOutgoing> Copy() const;

  size_t packet_length() const { return packet_.size(); }

  rtc::ArrayView<const uint8_t> RawHeader() const {
    return rtc::MakeArrayView(packet_.data(), header_length());
  }
  uint32_t Ssrc() const { return packet_.Ssrc(); }
  uint32_t Timestamp() const { return packet_.Timestamp(); }
  uint16_t SequenceNumber() const { return packet_.SequenceNumber(); }
  uint8_t PayloadType() const { return packet_.PayloadType(); }
  bool Marker() const { return packet_.Marker(); }
  template <typename ExtensionTrait, typename... Args>
  bool GetExtension(Args&&... args) const {
    return packet_.GetExtension<ExtensionTrait>(std::forward<Args>(args)...);
  }
  template <typename ExtensionTrait>
  bool HasExtension() const {
    return packet_.HasExtension<ExtensionTrait>();
  }

  size_t payload_length() const { return packet_.payload_size(); }
  size_t header_length() const { return packet_.headers_size(); }
  size_t padding_length() const { return packet_.padding_size(); }
  int probe_cluster_id() const { return probe_cluster_id_; }

 private:
  RtcEventRtpPacketOutgoing(const RtcEventRtpPacketOutgoing& other);

  const RtpPacket packet_;
  // TODO(eladalon): Delete |probe_cluster_id_| along with legacy encoding.
  const int probe_cluster_id_;
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_RTP_PACKET_OUTGOING_H_
