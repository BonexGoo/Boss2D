/*
 *  Copyright 2015 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_KEEP_REF_UNTIL_DONE_H_
#define RTC_BASE_KEEP_REF_UNTIL_DONE_H_

#include BOSS_WEBRTC_U_rtc_base__bind_h //original-code:"rtc_base/bind.h"
#include BOSS_WEBRTC_U_rtc_base__callback_h //original-code:"rtc_base/callback.h"
#include BOSS_WEBRTC_U_rtc_base__refcount_h //original-code:"rtc_base/refcount.h"
#include BOSS_WEBRTC_U_rtc_base__scoped_ref_ptr_h //original-code:"rtc_base/scoped_ref_ptr.h"

namespace rtc {

namespace impl {
template <class T>
static inline void DoNothing(const scoped_refptr<T>& object) {}
}  // namespace impl

// KeepRefUntilDone keeps a reference to |object| until the returned
// callback goes out of scope. If the returned callback is copied, the
// reference will be released when the last callback goes out of scope.
template <class ObjectT>
static inline Callback0<void> KeepRefUntilDone(ObjectT* object) {
  return rtc::Bind(&impl::DoNothing<ObjectT>, scoped_refptr<ObjectT>(object));
}

template <class ObjectT>
static inline Callback0<void> KeepRefUntilDone(
    const scoped_refptr<ObjectT>& object) {
  return rtc::Bind(&impl::DoNothing<ObjectT>, object);
}

}  // namespace rtc

#endif  // RTC_BASE_KEEP_REF_UNTIL_DONE_H_
