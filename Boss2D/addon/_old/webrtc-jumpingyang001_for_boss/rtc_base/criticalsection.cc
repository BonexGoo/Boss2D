/*
 *  Copyright 2015 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__criticalsection_h //original-code:"rtc_base/criticalsection.h"

#include BOSS_WEBRTC_U_rtc_base__atomicops_h //original-code:"rtc_base/atomicops.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__platform_thread_types_h //original-code:"rtc_base/platform_thread_types.h"
#include BOSS_WEBRTC_U_rtc_base__system__unused_h //original-code:"rtc_base/system/unused.h"

// TODO(tommi): Split this file up to per-platform implementation files.

namespace rtc {

CriticalSection::CriticalSection() {
#if defined(WEBRTC_WIN)
  InitializeCriticalSection(&crit_);
#elif defined(WEBRTC_POSIX)
#if defined(WEBRTC_MAC) && !USE_NATIVE_MUTEX_ON_MAC
  lock_queue_ = 0;
  owning_thread_ = 0;
  recursion_ = 0;
  semaphore_ = dispatch_semaphore_create(0);
#else
  pthread_mutexattr_t mutex_attribute;
  pthread_mutexattr_init(&mutex_attribute);
  pthread_mutexattr_settype(&mutex_attribute, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mutex_, &mutex_attribute);
  pthread_mutexattr_destroy(&mutex_attribute);
#endif
  CS_DEBUG_CODE(thread_ = 0);
  CS_DEBUG_CODE(recursion_count_ = 0);
  RTC_UNUSED(thread_);
  RTC_UNUSED(recursion_count_);
#else
#error Unsupported platform.
#endif
}

CriticalSection::~CriticalSection() {
#if defined(WEBRTC_WIN)
  DeleteCriticalSection(&crit_);
#elif defined(WEBRTC_POSIX)
#if defined(WEBRTC_MAC) && !USE_NATIVE_MUTEX_ON_MAC
  dispatch_release(semaphore_);
#else
  pthread_mutex_destroy(&mutex_);
#endif
#else
#error Unsupported platform.
#endif
}

void CriticalSection::Enter() const RTC_EXCLUSIVE_LOCK_FUNCTION() {
#if defined(WEBRTC_WIN)
  EnterCriticalSection(&crit_);
#elif defined(WEBRTC_POSIX)
#if defined(WEBRTC_MAC) && !USE_NATIVE_MUTEX_ON_MAC
  int spin = 3000;
  PlatformThreadRef self = CurrentThreadRef();
  bool have_lock = false;
  do {
    // Instead of calling TryEnter() in this loop, we do two interlocked
    // operations, first a read-only one in order to avoid affecting the lock
    // cache-line while spinning, in case another thread is using the lock.
    if (!IsThreadRefEqual(owning_thread_, self)) {
      if (AtomicOps::AcquireLoad(&lock_queue_) == 0) {
        if (AtomicOps::CompareAndSwap(&lock_queue_, 0, 1) == 0) {
          have_lock = true;
          break;
        }
      }
    } else {
      AtomicOps::Increment(&lock_queue_);
      have_lock = true;
      break;
    }

    sched_yield();
  } while (--spin);

  if (!have_lock && AtomicOps::Increment(&lock_queue_) > 1) {
    // Owning thread cannot be the current thread since TryEnter() would
    // have succeeded.
    RTC_DCHECK(!IsThreadRefEqual(owning_thread_, self));
    // Wait for the lock to become available.
    dispatch_semaphore_wait(semaphore_, DISPATCH_TIME_FOREVER);
    RTC_DCHECK(owning_thread_ == 0);
    RTC_DCHECK(!recursion_);
  }

  owning_thread_ = self;
  ++recursion_;

#else
  pthread_mutex_lock(&mutex_);
#endif

#if CS_DEBUG_CHECKS
  if (!recursion_count_) {
    RTC_DCHECK(!thread_);
    thread_ = CurrentThreadRef();
  } else {
    RTC_DCHECK(CurrentThreadIsOwner());
  }
  ++recursion_count_;
#endif
#else
#error Unsupported platform.
#endif
}

bool CriticalSection::TryEnter() const RTC_EXCLUSIVE_TRYLOCK_FUNCTION(true) {
#if defined(WEBRTC_WIN)
  return TryEnterCriticalSection(&crit_) != FALSE;
#elif defined(WEBRTC_POSIX)
#if defined(WEBRTC_MAC) && !USE_NATIVE_MUTEX_ON_MAC
  if (!IsThreadRefEqual(owning_thread_, CurrentThreadRef())) {
    if (AtomicOps::CompareAndSwap(&lock_queue_, 0, 1) != 0)
      return false;
    owning_thread_ = CurrentThreadRef();
    RTC_DCHECK(!recursion_);
  } else {
    AtomicOps::Increment(&lock_queue_);
  }
  ++recursion_;
#else
  if (pthread_mutex_trylock(&mutex_) != 0)
    return false;
#endif
#if CS_DEBUG_CHECKS
  if (!recursion_count_) {
    RTC_DCHECK(!thread_);
    thread_ = CurrentThreadRef();
  } else {
    RTC_DCHECK(CurrentThreadIsOwner());
  }
  ++recursion_count_;
#endif
  return true;
#else
#error Unsupported platform.
#endif
}

void CriticalSection::Leave() const RTC_UNLOCK_FUNCTION() {
  RTC_DCHECK(CurrentThreadIsOwner());
#if defined(WEBRTC_WIN)
  LeaveCriticalSection(&crit_);
#elif defined(WEBRTC_POSIX)
#if CS_DEBUG_CHECKS
  --recursion_count_;
  RTC_DCHECK(recursion_count_ >= 0);
  if (!recursion_count_)
    thread_ = 0;
#endif
#if defined(WEBRTC_MAC) && !USE_NATIVE_MUTEX_ON_MAC
  RTC_DCHECK(IsThreadRefEqual(owning_thread_, CurrentThreadRef()));
  RTC_DCHECK_GE(recursion_, 0);
  --recursion_;
  if (!recursion_)
    owning_thread_ = 0;

  if (AtomicOps::Decrement(&lock_queue_) > 0 && !recursion_)
    dispatch_semaphore_signal(semaphore_);
#else
  pthread_mutex_unlock(&mutex_);
#endif
#else
#error Unsupported platform.
#endif
}

bool CriticalSection::CurrentThreadIsOwner() const {
#if defined(WEBRTC_WIN)
  // OwningThread has type HANDLE but actually contains the Thread ID:
  // http://stackoverflow.com/questions/12675301/why-is-the-owningthread-member-of-critical-section-of-type-handle-when-it-is-de
  // Converting through size_t avoids the VS 2015 warning C4312: conversion from
  // 'type1' to 'type2' of greater size
  return crit_.OwningThread ==
         reinterpret_cast<HANDLE>(static_cast<size_t>(GetCurrentThreadId()));
#elif defined(WEBRTC_POSIX)
#if CS_DEBUG_CHECKS
  return IsThreadRefEqual(thread_, CurrentThreadRef());
#else
  return true;
#endif  // CS_DEBUG_CHECKS
#else
#error Unsupported platform.
#endif
}

CritScope::CritScope(const CriticalSection* cs) : cs_(cs) {
  cs_->Enter();
}
CritScope::~CritScope() {
  cs_->Leave();
}

TryCritScope::TryCritScope(const CriticalSection* cs)
    : cs_(cs), locked_(cs->TryEnter()) {
  CS_DEBUG_CODE(lock_was_called_ = false);
  RTC_UNUSED(lock_was_called_);
}

TryCritScope::~TryCritScope() {
  CS_DEBUG_CODE(RTC_DCHECK(lock_was_called_));
  if (locked_)
    cs_->Leave();
}

bool TryCritScope::locked() const {
  CS_DEBUG_CODE(lock_was_called_ = true);
  return locked_;
}

void GlobalLockPod::Lock() {
#if !defined(WEBRTC_WIN) && (!defined(WEBRTC_MAC) || USE_NATIVE_MUTEX_ON_MAC)
  const struct timespec ts_null = {0};
#endif

  while (AtomicOps::CompareAndSwap(&lock_acquired, 0, 1)) {
#if defined(WEBRTC_WIN)
    ::Sleep(0);
#elif defined(WEBRTC_MAC) && !USE_NATIVE_MUTEX_ON_MAC
    sched_yield();
#else
    nanosleep(&ts_null, nullptr);
#endif
  }
}

void GlobalLockPod::Unlock() {
  int old_value = AtomicOps::CompareAndSwap(&lock_acquired, 1, 0);
  RTC_DCHECK_EQ(1, old_value) << "Unlock called without calling Lock first";
}

GlobalLock::GlobalLock() {
  lock_acquired = 0;
}

GlobalLockScope::GlobalLockScope(GlobalLockPod* lock) : lock_(lock) {
  lock_->Lock();
}

GlobalLockScope::~GlobalLockScope() {
  lock_->Unlock();
}

}  // namespace rtc
