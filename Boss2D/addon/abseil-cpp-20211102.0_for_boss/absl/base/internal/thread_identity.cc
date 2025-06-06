// Copyright 2017 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include BOSS_ABSEILCPP_U_absl__base__internal__thread_identity_h //original-code:"absl/base/internal/thread_identity.h"

#ifndef _WIN32
#include <pthread.h>
#include <signal.h>
#endif

#include <atomic>
#include <cassert>
#include <memory>

#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_ABSEILCPP_U_absl__base__call_once_h //original-code:"absl/base/call_once.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__raw_logging_h //original-code:"absl/base/internal/raw_logging.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__spinlock_h //original-code:"absl/base/internal/spinlock.h"

namespace absl {
ABSL_NAMESPACE_BEGIN
namespace base_internal {

#if ABSL_THREAD_IDENTITY_MODE != ABSL_THREAD_IDENTITY_MODE_USE_CPP11
namespace {
// Used to co-ordinate one-time creation of our pthread_key
absl::once_flag init_thread_identity_key_once;
pthread_key_t thread_identity_pthread_key;
std::atomic<bool> pthread_key_initialized(false);

void AllocateThreadIdentityKey(ThreadIdentityReclaimerFunction reclaimer) {
  pthread_key_create(&thread_identity_pthread_key, reclaimer);
  pthread_key_initialized.store(true, std::memory_order_release);
}
}  // namespace
#endif

#if ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_TLS || \
    ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_CPP11
// The actual TLS storage for a thread's currently associated ThreadIdentity.
// This is referenced by inline accessors in the header.
// "protected" visibility ensures that if multiple instances of Abseil code
// exist within a process (via dlopen() or similar), references to
// thread_identity_ptr from each instance of the code will refer to
// *different* instances of this ptr.
// Apple platforms have the visibility attribute, but issue a compile warning
// that protected visibility is unsupported.
#if ABSL_HAVE_ATTRIBUTE(visibility) && !defined(__APPLE__)
__attribute__((visibility("protected")))
#endif  // ABSL_HAVE_ATTRIBUTE(visibility) && !defined(__APPLE__)
#if ABSL_PER_THREAD_TLS
// Prefer __thread to thread_local as benchmarks indicate it is a bit faster.
ABSL_PER_THREAD_TLS_KEYWORD ThreadIdentity* thread_identity_ptr = nullptr;
#elif defined(ABSL_HAVE_THREAD_LOCAL)
thread_local ThreadIdentity* thread_identity_ptr = nullptr;
#endif  // ABSL_PER_THREAD_TLS
#endif  // TLS or CPP11

void SetCurrentThreadIdentity(
    ThreadIdentity* identity, ThreadIdentityReclaimerFunction reclaimer) {
  assert(CurrentThreadIdentityIfPresent() == nullptr);
  // Associate our destructor.
  // NOTE: This call to pthread_setspecific is currently the only immovable
  // barrier to CurrentThreadIdentity() always being async signal safe.
#if ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_POSIX_SETSPECIFIC
  // NOTE: Not async-safe.  But can be open-coded.
  absl::call_once(init_thread_identity_key_once, AllocateThreadIdentityKey,
                  reclaimer);

#if defined(__EMSCRIPTEN__) || defined(__MINGW32__)
  // Emscripten and MinGW pthread implementations does not support signals.
  // See https://kripken.github.io/emscripten-site/docs/porting/pthreads.html
  // for more information.
  pthread_setspecific(thread_identity_pthread_key,
                      reinterpret_cast<void*>(identity));
#else
  // We must mask signals around the call to setspecific as with current glibc,
  // a concurrent getspecific (needed for GetCurrentThreadIdentityIfPresent())
  // may zero our value.
  //
  // While not officially async-signal safe, getspecific within a signal handler
  // is otherwise OK.
  sigset_t all_signals;
  sigset_t curr_signals;
  sigfillset(&all_signals);
  pthread_sigmask(SIG_SETMASK, &all_signals, &curr_signals);
  pthread_setspecific(thread_identity_pthread_key,
                      reinterpret_cast<void*>(identity));
  pthread_sigmask(SIG_SETMASK, &curr_signals, nullptr);
#endif  // !__EMSCRIPTEN__ && !__MINGW32__

#elif ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_TLS
  // NOTE: Not async-safe.  But can be open-coded.
  absl::call_once(init_thread_identity_key_once, AllocateThreadIdentityKey,
                  reclaimer);
  pthread_setspecific(thread_identity_pthread_key,
                      reinterpret_cast<void*>(identity));
  thread_identity_ptr = identity;
#elif ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_CPP11
  thread_local std::unique_ptr<ThreadIdentity, ThreadIdentityReclaimerFunction>
      holder(identity, reclaimer);
  thread_identity_ptr = identity;
#else
#error Unimplemented ABSL_THREAD_IDENTITY_MODE
#endif
}

#if ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_TLS || \
    ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_CPP11

// Please see the comment on `CurrentThreadIdentityIfPresent` in
// thread_identity.h. When we cannot expose thread_local variables in
// headers, we opt for the correct-but-slower option of not inlining this
// function.
#ifndef ABSL_INTERNAL_INLINE_CURRENT_THREAD_IDENTITY_IF_PRESENT
ThreadIdentity* CurrentThreadIdentityIfPresent() { return thread_identity_ptr; }
#endif
#endif

void ClearCurrentThreadIdentity() {
#if ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_TLS || \
    ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_CPP11
  thread_identity_ptr = nullptr;
#elif ABSL_THREAD_IDENTITY_MODE == \
      ABSL_THREAD_IDENTITY_MODE_USE_POSIX_SETSPECIFIC
  // pthread_setspecific expected to clear value on destruction
  assert(CurrentThreadIdentityIfPresent() == nullptr);
#endif
}

#if ABSL_THREAD_IDENTITY_MODE == ABSL_THREAD_IDENTITY_MODE_USE_POSIX_SETSPECIFIC
ThreadIdentity* CurrentThreadIdentityIfPresent() {
  bool initialized = pthread_key_initialized.load(std::memory_order_acquire);
  if (!initialized) {
    return nullptr;
  }
  return reinterpret_cast<ThreadIdentity*>(
      pthread_getspecific(thread_identity_pthread_key));
}
#endif

}  // namespace base_internal
ABSL_NAMESPACE_END
}  // namespace absl
