/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__fake_rtc_event_log_h //original-code:"logging/rtc_event_log/fake_rtc_event_log.h"

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_ice_candidate_pair_h //original-code:"logging/rtc_event_log/events/rtc_event_ice_candidate_pair.h"
#include BOSS_WEBRTC_U_rtc_base__bind_h //original-code:"rtc_base/bind.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace webrtc {

FakeRtcEventLog::FakeRtcEventLog(rtc::Thread* thread) : thread_(thread) {
  RTC_DCHECK(thread_);
}
FakeRtcEventLog::~FakeRtcEventLog() = default;

bool FakeRtcEventLog::StartLogging(std::unique_ptr<RtcEventLogOutput> output,
                                   int64_t output_period_ms) {
  return true;
}

void FakeRtcEventLog::StopLogging() {
  invoker_.Flush(thread_);
}

void FakeRtcEventLog::Log(std::unique_ptr<RtcEvent> event) {
  RtcEvent::Type rtc_event_type = event->GetType();
  invoker_.AsyncInvoke<void>(
      RTC_FROM_HERE, thread_,
      rtc::Bind(&FakeRtcEventLog::IncrementEventCount, this, rtc_event_type));
}

}  // namespace webrtc
