// Copyright 2018 The Abseil Authors.
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
//
// -----------------------------------------------------------------------------
// File: sample_recorder.h
// -----------------------------------------------------------------------------
//
// This header file defines a lock-free linked list for recording samples
// collected from a random/stochastic process.
//
// This utility is internal-only. Use at your own risk.

#ifndef ABSL_PROFILING_INTERNAL_SAMPLE_RECORDER_H_
#define ABSL_PROFILING_INTERNAL_SAMPLE_RECORDER_H_

#include <atomic>
#include <cstddef>
#include <functional>

#include BOSS_ABSEILCPP_U_absl__base__config_h //original-code:"absl/base/config.h"
#include BOSS_ABSEILCPP_U_absl__base__thread_annotations_h //original-code:"absl/base/thread_annotations.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__mutex_h //original-code:"absl/synchronization/mutex.h"
#include BOSS_ABSEILCPP_U_absl__time__time_h //original-code:"absl/time/time.h"

namespace absl {
ABSL_NAMESPACE_BEGIN
namespace profiling_internal {

// Sample<T> that has members required for linking samples in the linked list of
// samples maintained by the SampleRecorder.  Type T defines the sampled data.
template <typename T>
struct Sample {
  // Guards the ability to restore the sample to a pristine state.  This
  // prevents races with sampling and resurrecting an object.
  absl::Mutex init_mu;
  T* next = nullptr;
  T* dead ABSL_GUARDED_BY(init_mu) = nullptr;
};

// Holds samples and their associated stack traces with a soft limit of
// `SetHashtablezMaxSamples()`.
//
// Thread safe.
template <typename T>
class SampleRecorder {
 public:
  SampleRecorder();
  ~SampleRecorder();

  // Registers for sampling.  Returns an opaque registration info.
  T* Register();

  // Unregisters the sample.
  void Unregister(T* sample);

  // The dispose callback will be called on all samples the moment they are
  // being unregistered. Only affects samples that are unregistered after the
  // callback has been set.
  // Returns the previous callback.
  using DisposeCallback = void (*)(const T&);
  DisposeCallback SetDisposeCallback(DisposeCallback f);

  // Iterates over all the registered `StackInfo`s.  Returning the number of
  // samples that have been dropped.
  int64_t Iterate(const std::function<void(const T& stack)>& f);

  void SetMaxSamples(int32_t max);

 private:
  void PushNew(T* sample);
  void PushDead(T* sample);
  T* PopDead();

  std::atomic<size_t> dropped_samples_;
  std::atomic<size_t> size_estimate_;
  std::atomic<int32_t> max_samples_{1 << 20};

  // Intrusive lock free linked lists for tracking samples.
  //
  // `all_` records all samples (they are never removed from this list) and is
  // terminated with a `nullptr`.
  //
  // `graveyard_.dead` is a circular linked list.  When it is empty,
  // `graveyard_.dead == &graveyard`.  The list is circular so that
  // every item on it (even the last) has a non-null dead pointer.  This allows
  // `Iterate` to determine if a given sample is live or dead using only
  // information on the sample itself.
  //
  // For example, nodes [A, B, C, D, E] with [A, C, E] alive and [B, D] dead
  // looks like this (G is the Graveyard):
  //
  //           +---+    +---+    +---+    +---+    +---+
  //    all -->| A |--->| B |--->| C |--->| D |--->| E |
  //           |   |    |   |    |   |    |   |    |   |
  //   +---+   |   | +->|   |-+  |   | +->|   |-+  |   |
  //   | G |   +---+ |  +---+ |  +---+ |  +---+ |  +---+
  //   |   |         |        |        |        |
  //   |   | --------+        +--------+        |
  //   +---+                                    |
  //     ^                                      |
  //     +--------------------------------------+
  //
  std::atomic<T*> all_;
  T graveyard_;

  std::atomic<DisposeCallback> dispose_;
};

template <typename T>
typename SampleRecorder<T>::DisposeCallback
SampleRecorder<T>::SetDisposeCallback(DisposeCallback f) {
  return dispose_.exchange(f, std::memory_order_relaxed);
}

template <typename T>
SampleRecorder<T>::SampleRecorder()
    : dropped_samples_(0), size_estimate_(0), all_(nullptr), dispose_(nullptr) {
  absl::MutexLock l(&graveyard_.init_mu);
  graveyard_.dead = &graveyard_;
}

template <typename T>
SampleRecorder<T>::~SampleRecorder() {
  T* s = all_.load(std::memory_order_acquire);
  while (s != nullptr) {
    T* next = s->next;
    delete s;
    s = next;
  }
}

template <typename T>
void SampleRecorder<T>::PushNew(T* sample) {
  sample->next = all_.load(std::memory_order_relaxed);
  while (!all_.compare_exchange_weak(sample->next, sample,
                                     std::memory_order_release,
                                     std::memory_order_relaxed)) {
  }
}

template <typename T>
void SampleRecorder<T>::PushDead(T* sample) {
  if (auto* dispose = dispose_.load(std::memory_order_relaxed)) {
    dispose(*sample);
  }

  absl::MutexLock graveyard_lock(&graveyard_.init_mu);
  absl::MutexLock sample_lock(&sample->init_mu);
  sample->dead = graveyard_.dead;
  graveyard_.dead = sample;
}

template <typename T>
T* SampleRecorder<T>::PopDead() {
  absl::MutexLock graveyard_lock(&graveyard_.init_mu);

  // The list is circular, so eventually it collapses down to
  //   graveyard_.dead == &graveyard_
  // when it is empty.
  T* sample = graveyard_.dead;
  if (sample == &graveyard_) return nullptr;

  absl::MutexLock sample_lock(&sample->init_mu);
  graveyard_.dead = sample->dead;
  sample->dead = nullptr;
  sample->PrepareForSampling();
  return sample;
}

template <typename T>
T* SampleRecorder<T>::Register() {
  int64_t size = size_estimate_.fetch_add(1, std::memory_order_relaxed);
  if (size > max_samples_.load(std::memory_order_relaxed)) {
    size_estimate_.fetch_sub(1, std::memory_order_relaxed);
    dropped_samples_.fetch_add(1, std::memory_order_relaxed);
    return nullptr;
  }

  T* sample = PopDead();
  if (sample == nullptr) {
    // Resurrection failed.  Hire a new warlock.
    sample = new T();
    PushNew(sample);
  }

  return sample;
}

template <typename T>
void SampleRecorder<T>::Unregister(T* sample) {
  PushDead(sample);
  size_estimate_.fetch_sub(1, std::memory_order_relaxed);
}

template <typename T>
int64_t SampleRecorder<T>::Iterate(
    const std::function<void(const T& stack)>& f) {
  T* s = all_.load(std::memory_order_acquire);
  while (s != nullptr) {
    absl::MutexLock l(&s->init_mu);
    if (s->dead == nullptr) {
      f(*s);
    }
    s = s->next;
  }

  return dropped_samples_.load(std::memory_order_relaxed);
}

template <typename T>
void SampleRecorder<T>::SetMaxSamples(int32_t max) {
  max_samples_.store(max, std::memory_order_release);
}

}  // namespace profiling_internal
ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_PROFILING_INTERNAL_SAMPLE_RECORDER_H_
