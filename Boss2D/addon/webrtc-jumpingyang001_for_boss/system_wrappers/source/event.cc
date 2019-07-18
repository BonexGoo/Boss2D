/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_system_wrappers__include__event_wrapper_h //original-code:"system_wrappers/include/event_wrapper.h"

#if defined(_WIN32)
#include <windows.h>
#include BOSS_WEBRTC_U_system_wrappers__source__event_timer_win_h //original-code:"system_wrappers/source/event_timer_win.h"
#elif defined(WEBRTC_MAC) && !defined(WEBRTC_IOS)
#include <ApplicationServices/ApplicationServices.h>
#include <pthread.h>
#include BOSS_WEBRTC_U_system_wrappers__source__event_timer_posix_h //original-code:"system_wrappers/source/event_timer_posix.h"
#else
#include <pthread.h>
#include BOSS_WEBRTC_U_system_wrappers__source__event_timer_posix_h //original-code:"system_wrappers/source/event_timer_posix.h"
#endif

#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"

namespace webrtc {

class EventWrapperImpl : public EventWrapper {
 public:
  EventWrapperImpl() : event_(false, false) {}
  ~EventWrapperImpl() override {}

  bool Set() override {
    event_.Set();
    return true;
  }

  EventTypeWrapper Wait(unsigned long max_time) override {
    int to_wait = max_time == WEBRTC_EVENT_INFINITE
                      ? rtc::Event::kForever
                      : static_cast<int>(max_time);
    return event_.Wait(to_wait) ? kEventSignaled : kEventTimeout;
  }

 private:
  rtc::Event event_;
};

// static
EventWrapper* EventWrapper::Create() {
  return new EventWrapperImpl();
}

}  // namespace webrtc
