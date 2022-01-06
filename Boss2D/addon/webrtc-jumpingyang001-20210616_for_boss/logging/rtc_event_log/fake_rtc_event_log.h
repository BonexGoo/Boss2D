/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_FAKE_RTC_EVENT_LOG_H_
#define LOGGING_RTC_EVENT_LOG_FAKE_RTC_EVENT_LOG_H_

#include <map>
#include <memory>

#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_h //original-code:"api/rtc_event_log/rtc_event.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_h //original-code:"api/rtc_event_log/rtc_event_log.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class FakeRtcEventLog : public RtcEventLog {
 public:
  FakeRtcEventLog() = default;
  ~FakeRtcEventLog() override = default;

  bool StartLogging(std::unique_ptr<RtcEventLogOutput> output,
                    int64_t output_period_ms) override;
  void StopLogging() override;
  void Log(std::unique_ptr<RtcEvent> event) override;
  int GetEventCount(RtcEvent::Type event_type);

 private:
  Mutex mu_;
  std::map<RtcEvent::Type, int> count_ RTC_GUARDED_BY(mu_);
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_FAKE_RTC_EVENT_LOG_H_
