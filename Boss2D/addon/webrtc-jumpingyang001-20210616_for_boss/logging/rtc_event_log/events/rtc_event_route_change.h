/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_ROUTE_CHANGE_H_
#define LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_ROUTE_CHANGE_H_

#include <memory>

#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_h //original-code:"api/rtc_event_log/rtc_event.h"

namespace webrtc {

class RtcEventRouteChange final : public RtcEvent {
 public:
  static constexpr Type kType = Type::RouteChangeEvent;

  RtcEventRouteChange(bool connected, uint32_t overhead);
  ~RtcEventRouteChange() override;

  Type GetType() const override { return kType; }
  bool IsConfigEvent() const override { return false; }

  std::unique_ptr<RtcEventRouteChange> Copy() const;

  bool connected() const { return connected_; }
  uint32_t overhead() const { return overhead_; }

 private:
  RtcEventRouteChange(const RtcEventRouteChange& other);

  const bool connected_;
  const uint32_t overhead_;
};

struct LoggedRouteChangeEvent {
  LoggedRouteChangeEvent() = default;
  LoggedRouteChangeEvent(int64_t timestamp_ms,
                         bool connected,
                         uint32_t overhead)
      : timestamp_ms(timestamp_ms), connected(connected), overhead(overhead) {}

  int64_t log_time_us() const { return timestamp_ms * 1000; }
  int64_t log_time_ms() const { return timestamp_ms; }

  int64_t timestamp_ms;
  bool connected;
  uint32_t overhead;
};

}  // namespace webrtc
#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_ROUTE_CHANGE_H_
