/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_PLATFORM_THREAD_TYPES_H_
#define RTC_BASE_PLATFORM_THREAD_TYPES_H_

#if defined(WEBRTC_WIN)
#include BOSS_FAKEWIN_V_winsock2_h //original-code:<winsock2.h>
#include BOSS_FAKEWIN_V_windows_h //original-code:<windows.h>
#elif defined(WEBRTC_FUCHSIA)
#include <zircon/types.h>
#elif defined(WEBRTC_POSIX)
#include <pthread.h>
#include <unistd.h>
#endif

namespace rtc {
#if defined(WEBRTC_WIN)
typedef DWORD PlatformThreadId;
typedef DWORD PlatformThreadRef;
#elif defined(WEBRTC_FUCHSIA)
typedef zx_handle_t PlatformThreadId;
typedef pthread_t PlatformThreadRef;
#elif defined(WEBRTC_POSIX)
typedef pid_t PlatformThreadId;
typedef pthread_t PlatformThreadRef;
#endif
}  // namespace rtc

#endif  // RTC_BASE_PLATFORM_THREAD_TYPES_H_
