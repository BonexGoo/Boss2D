/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_RTCP_PACKET_OUTGOING_H_
#define LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_RTCP_PACKET_OUTGOING_H_

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_h //original-code:"logging/rtc_event_log/events/rtc_event.h"
#include BOSS_WEBRTC_U_rtc_base__buffer_h //original-code:"rtc_base/buffer.h"

namespace webrtc {

class RtcEventRtcpPacketOutgoing final : public RtcEvent {
 public:
  explicit RtcEventRtcpPacketOutgoing(rtc::ArrayView<const uint8_t> packet);
  ~RtcEventRtcpPacketOutgoing() override;

  Type GetType() const override;

  bool IsConfigEvent() const override;

  rtc::Buffer packet_;
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_RTCP_PACKET_OUTGOING_H_
