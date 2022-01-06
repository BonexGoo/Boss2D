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

#include <map>
#include <memory>

#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_h //original-code:"api/rtc_event_log/rtc_event_log.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"

namespace webrtc {

bool FakeRtcEventLog::StartLogging(std::unique_ptr<RtcEventLogOutput> output,
                                   int64_t output_period_ms) {
  return true;
}

void FakeRtcEventLog::StopLogging() {}

void FakeRtcEventLog::Log(std::unique_ptr<RtcEvent> event) {
  MutexLock lock(&mu_);
  ++count_[event->GetType()];
}

int FakeRtcEventLog::GetEventCount(RtcEvent::Type event_type) {
  MutexLock lock(&mu_);
  return count_[event_type];
}

}  // namespace webrtc
