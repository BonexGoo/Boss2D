/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__neteq__tick_timer_h //original-code:"api/neteq/tick_timer.h"

namespace webrtc {

TickTimer::Stopwatch::Stopwatch(const TickTimer& ticktimer)
    : ticktimer_(ticktimer), starttick_(ticktimer.ticks()) {}

TickTimer::Countdown::Countdown(const TickTimer& ticktimer,
                                uint64_t ticks_to_count)
    : stopwatch_(ticktimer.GetNewStopwatch()),
      ticks_to_count_(ticks_to_count) {}

TickTimer::Countdown::~Countdown() = default;

}  // namespace webrtc
