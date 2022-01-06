/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_RTC_EVENT_LOG_RTC_EVENT_LOG_FACTORY_H_
#define API_RTC_EVENT_LOG_RTC_EVENT_LOG_FACTORY_H_

#include <memory>

#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_h //original-code:"api/rtc_event_log/rtc_event_log.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_factory_interface_h //original-code:"api/rtc_event_log/rtc_event_log_factory_interface.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"

namespace webrtc {

class RTC_EXPORT RtcEventLogFactory : public RtcEventLogFactoryInterface {
 public:
  explicit RtcEventLogFactory(TaskQueueFactory* task_queue_factory);
  ~RtcEventLogFactory() override {}

  std::unique_ptr<RtcEventLog> CreateRtcEventLog(
      RtcEventLog::EncodingType encoding_type) override;

 private:
  TaskQueueFactory* const task_queue_factory_;
};

}  // namespace webrtc

#endif  // API_RTC_EVENT_LOG_RTC_EVENT_LOG_FACTORY_H_
