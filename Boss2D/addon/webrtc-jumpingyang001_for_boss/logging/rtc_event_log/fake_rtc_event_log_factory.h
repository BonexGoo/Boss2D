/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_FAKE_RTC_EVENT_LOG_FACTORY_H_
#define LOGGING_RTC_EVENT_LOG_FAKE_RTC_EVENT_LOG_FACTORY_H_

#include <memory>

#include BOSS_WEBRTC_U_logging__rtc_event_log__fake_rtc_event_log_h //original-code:"logging/rtc_event_log/fake_rtc_event_log.h"
#include BOSS_WEBRTC_U_logging__rtc_event_log__rtc_event_log_factory_interface_h //original-code:"logging/rtc_event_log/rtc_event_log_factory_interface.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"

namespace webrtc {

class FakeRtcEventLogFactory : public RtcEventLogFactoryInterface {
 public:
  explicit FakeRtcEventLogFactory(rtc::Thread* thread) : thread_(thread) {}
  ~FakeRtcEventLogFactory() override {}

  std::unique_ptr<RtcEventLog> CreateRtcEventLog(
      RtcEventLog::EncodingType encoding_type) override;

  std::unique_ptr<RtcEventLog> CreateRtcEventLog(
      RtcEventLog::EncodingType encoding_type,
      std::unique_ptr<rtc::TaskQueue> task_queue) override;

  webrtc::RtcEventLog* last_log_created() { return last_log_created_; }
  rtc::Thread* thread() { return thread_; }

 private:
  webrtc::RtcEventLog* last_log_created_;
  rtc::Thread* thread_;
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_FAKE_RTC_EVENT_LOG_FACTORY_H_
