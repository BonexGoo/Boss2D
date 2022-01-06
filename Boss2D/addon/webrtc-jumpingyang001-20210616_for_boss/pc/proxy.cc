/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__proxy_h //original-code:"pc/proxy.h"

#include BOSS_WEBRTC_U_rtc_base__trace_event_h //original-code:"rtc_base/trace_event.h"

namespace webrtc {
namespace proxy_internal {
ScopedTrace::ScopedTrace(const char* class_and_method_name)
    : class_and_method_name_(class_and_method_name) {
  TRACE_EVENT_BEGIN0("webrtc", class_and_method_name_);
}
ScopedTrace::~ScopedTrace() {
  TRACE_EVENT_END0("webrtc", class_and_method_name_);
}
}  // namespace proxy_internal
}  // namespace webrtc
