/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_BWE_UPDATE_DELAY_BASED_H_
#define LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_BWE_UPDATE_DELAY_BASED_H_

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_h //original-code:"logging/rtc_event_log/events/rtc_event.h"

namespace webrtc {

enum class BandwidthUsage;

class RtcEventBweUpdateDelayBased final : public RtcEvent {
 public:
  RtcEventBweUpdateDelayBased(int32_t bitrate_bps,
                              BandwidthUsage detector_state);
  ~RtcEventBweUpdateDelayBased() override;

  Type GetType() const override;

  bool IsConfigEvent() const override;

  const int32_t bitrate_bps_;
  const BandwidthUsage detector_state_;
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_BWE_UPDATE_DELAY_BASED_H_
