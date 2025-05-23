/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// TODO(zijiehe): Implement ScreenDrawerMac

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__screen_drawer_h //original-code:"modules/desktop_capture/screen_drawer.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__screen_drawer_lock_posix_h //original-code:"modules/desktop_capture/screen_drawer_lock_posix.h"

namespace webrtc {

// static
std::unique_ptr<ScreenDrawerLock> ScreenDrawerLock::Create() {
  return absl::make_unique<ScreenDrawerLockPosix>();
}

// static
std::unique_ptr<ScreenDrawer> ScreenDrawer::Create() {
  return nullptr;
}

}  // namespace webrtc
