/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_call__call_config_h //original-code:"call/call_config.h"

namespace webrtc {

CallConfig::CallConfig(RtcEventLog* event_log) : event_log(event_log) {
  RTC_DCHECK(event_log);
}
CallConfig::~CallConfig() = default;

}  // namespace webrtc
