/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__video_coding__event_wrapper_h //original-code:"modules/video_coding/event_wrapper.h"

#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"

namespace webrtc {

class EventWrapperImpl : public EventWrapper {
 public:
  ~EventWrapperImpl() override {}

  bool Set() override {
    event_.Set();
    return true;
  }

  EventTypeWrapper Wait(int max_time_ms) override {
    return event_.Wait(max_time_ms) ? kEventSignaled : kEventTimeout;
  }

 private:
  rtc::Event event_;
};

// static
EventWrapper* EventWrapper::Create() {
  return new EventWrapperImpl();
}

}  // namespace webrtc
