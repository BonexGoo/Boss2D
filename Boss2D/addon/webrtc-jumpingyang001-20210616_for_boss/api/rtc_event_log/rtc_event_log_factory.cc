/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_factory_h //original-code:"api/rtc_event_log/rtc_event_log_factory.h"

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"

#ifdef WEBRTC_ENABLE_RTC_EVENT_LOG
#include BOSS_WEBRTC_U_logging__rtc_event_log__rtc_event_log_impl_h //original-code:"logging/rtc_event_log/rtc_event_log_impl.h"
#endif

namespace webrtc {

RtcEventLogFactory::RtcEventLogFactory(TaskQueueFactory* task_queue_factory)
    : task_queue_factory_(task_queue_factory) {
  RTC_DCHECK(task_queue_factory_);
}

std::unique_ptr<RtcEventLog> RtcEventLogFactory::CreateRtcEventLog(
    RtcEventLog::EncodingType encoding_type) {
#ifdef WEBRTC_ENABLE_RTC_EVENT_LOG
  if (field_trial::IsEnabled("WebRTC-RtcEventLogKillSwitch")) {
    return std::make_unique<RtcEventLogNull>();
  }
  return std::make_unique<RtcEventLogImpl>(encoding_type, task_queue_factory_);
#else
  return std::make_unique<RtcEventLogNull>();
#endif
}

}  // namespace webrtc
