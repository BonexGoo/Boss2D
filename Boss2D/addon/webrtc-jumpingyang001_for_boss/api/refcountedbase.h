/*
 *  Copyright 2017 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef API_REFCOUNTEDBASE_H_
#define API_REFCOUNTEDBASE_H_

#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"
#include BOSS_WEBRTC_U_rtc_base__refcount_h //original-code:"rtc_base/refcount.h"
#include BOSS_WEBRTC_U_rtc_base__refcounter_h //original-code:"rtc_base/refcounter.h"

namespace rtc {

class RefCountedBase {
 public:
  RefCountedBase() = default;

  void AddRef() const { ref_count_.IncRef(); }
  RefCountReleaseStatus Release() const {
    const auto status = ref_count_.DecRef();
    if (status == RefCountReleaseStatus::kDroppedLastRef) {
      delete this;
    }
    return status;
  }

 protected:
  virtual ~RefCountedBase() = default;

 private:
  mutable webrtc::webrtc_impl::RefCounter ref_count_{0};

  RTC_DISALLOW_COPY_AND_ASSIGN(RefCountedBase);
};

}  // namespace rtc

#endif  // API_REFCOUNTEDBASE_H_
