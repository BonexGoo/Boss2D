/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_SYNCHRONIZATION_RW_LOCK_WIN_H_
#define RTC_BASE_SYNCHRONIZATION_RW_LOCK_WIN_H_

#include BOSS_WEBRTC_U_rtc_base__synchronization__rw_lock_wrapper_h //original-code:"rtc_base/synchronization/rw_lock_wrapper.h"

#include BOSS_FAKEWIN_V_windows_h //original-code:<Windows.h>

namespace webrtc {

class RWLockWin : public RWLockWrapper {
 public:
  static RWLockWin* Create();

  void AcquireLockExclusive() override;
  void ReleaseLockExclusive() override;

  void AcquireLockShared() override;
  void ReleaseLockShared() override;

 private:
  RWLockWin();
  static bool LoadModule();

  SRWLOCK lock_;
};

}  // namespace webrtc

#endif  // RTC_BASE_SYNCHRONIZATION_RW_LOCK_WIN_H_
