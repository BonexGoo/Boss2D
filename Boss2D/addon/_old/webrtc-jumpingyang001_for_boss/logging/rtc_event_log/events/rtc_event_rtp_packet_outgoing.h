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

#include <memory>

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_h //original-code:"logging/rtc_event_log/events/rtc_event.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_h //original-code:"modules/rtp_rtcp/source/rtp_packet.h"

namespace webrtc {

class RtpPacketToSend;

class RtcEventRtpPacketOutgoing final : public RtcEvent {
 public:
  RtcEventRtpPacketOutgoing(const RtpPacketToSend& packet,
                            int probe_cluster_id);
  ~RtcEventRtpPacketOutgoing() override;

  Type GetType() const override;

  bool IsConfigEvent() const override;

  std::unique_ptr<RtcEvent> Copy() const override;

  RtpPacket header_;            // Only the packet's header will be stored here.
  const size_t packet_length_;  // Length before stripping away all but header.
  const int probe_cluster_id_;

 private:
  RtcEventRtpPacketOutgoing(const RtcEventRtpPacketOutgoing& other);
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_RTP_PACKET_OUTGOING_H_
