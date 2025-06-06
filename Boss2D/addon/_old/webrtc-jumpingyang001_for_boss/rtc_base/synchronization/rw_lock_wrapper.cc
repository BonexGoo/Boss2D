/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__synchronization__rw_lock_wrapper_h //original-code:"rtc_base/synchronization/rw_lock_wrapper.h"

#include <assert.h>

#if defined(_WIN32)
#include BOSS_WEBRTC_U_rtc_base__synchronization__rw_lock_win_h //original-code:"rtc_base/synchronization/rw_lock_win.h"
#else
#include BOSS_WEBRTC_U_rtc_base__synchronization__rw_lock_posix_h //original-code:"rtc_base/synchronization/rw_lock_posix.h"
#endif

namespace webrtc {

RWLockWrapper* RWLockWrapper::CreateRWLock() {
#ifdef _WIN32
  return RWLockWin::Create();
#else
  return RWLockPosix::Create();
#endif
}

}  // namespace webrtc
