/*
 *  Copyright 2014 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_ASYNCINVOKER_INL_H_
#define RTC_BASE_ASYNCINVOKER_INL_H_

#include BOSS_WEBRTC_U_rtc_base__bind_h //original-code:"rtc_base/bind.h"
#include BOSS_WEBRTC_U_rtc_base__criticalsection_h //original-code:"rtc_base/criticalsection.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_rtc_base__messagehandler_h //original-code:"rtc_base/messagehandler.h"
#include BOSS_WEBRTC_U_rtc_base__refcountedobject_h //original-code:"rtc_base/refcountedobject.h"
#include BOSS_WEBRTC_U_rtc_base__scoped_ref_ptr_h //original-code:"rtc_base/scoped_ref_ptr.h"
#include BOSS_WEBRTC_U_rtc_base__sigslot_h //original-code:"rtc_base/sigslot.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace rtc {

class AsyncInvoker;

// Helper class for AsyncInvoker. Runs a task and triggers a callback
// on the calling thread if necessary.
class AsyncClosure {
 public:
  explicit AsyncClosure(AsyncInvoker* invoker);
  virtual ~AsyncClosure();
  // Runs the asynchronous task, and triggers a callback to the calling
  // thread if needed. Should be called from the target thread.
  virtual void Execute() = 0;

 protected:
  AsyncInvoker* invoker_;
  // Reference counted so that if the AsyncInvoker destructor finishes before
  // an AsyncClosure's destructor that's about to call
  // "invocation_complete_->Set()", it's not dereferenced after being
  // destroyed.
  scoped_refptr<RefCountedObject<Event>> invocation_complete_;
};

// Simple closure that doesn't trigger a callback for the calling thread.
template <class FunctorT>
class FireAndForgetAsyncClosure : public AsyncClosure {
 public:
  explicit FireAndForgetAsyncClosure(AsyncInvoker* invoker,
                                     const FunctorT& functor)
      : AsyncClosure(invoker), functor_(functor) {}
  virtual void Execute() {
    functor_();
  }
 private:
  FunctorT functor_;
};

}  // namespace rtc

#endif  // RTC_BASE_ASYNCINVOKER_INL_H_
