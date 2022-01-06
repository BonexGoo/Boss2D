// Copyright 2017 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include BOSS_ABSEILCPP_U_absl__synchronization__notification_h //original-code:"absl/synchronization/notification.h"

#include <atomic>

#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__raw_logging_h //original-code:"absl/base/internal/raw_logging.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__mutex_h //original-code:"absl/synchronization/mutex.h"
#include BOSS_ABSEILCPP_U_absl__time__time_h //original-code:"absl/time/time.h"

namespace absl {
inline namespace lts_2018_06_20 {
void Notification::Notify() {
  MutexLock l(&this->mutex_);

#ifndef NDEBUG
  if (ABSL_PREDICT_FALSE(notified_yet_.load(std::memory_order_relaxed))) {
    ABSL_RAW_LOG(
        FATAL,
        "Notify() method called more than once for Notification object %p",
        static_cast<void *>(this));
  }
#endif

  notified_yet_.store(true, std::memory_order_release);
}

Notification::~Notification() {
  // Make sure that the thread running Notify() exits before the object is
  // destructed.
  MutexLock l(&this->mutex_);
}

static inline bool HasBeenNotifiedInternal(
    const std::atomic<bool> *notified_yet) {
  return notified_yet->load(std::memory_order_acquire);
}

bool Notification::HasBeenNotified() const {
  return HasBeenNotifiedInternal(&this->notified_yet_);
}

void Notification::WaitForNotification() const {
  if (!HasBeenNotifiedInternal(&this->notified_yet_)) {
    this->mutex_.LockWhen(Condition(&HasBeenNotifiedInternal,
                                    &this->notified_yet_));
    this->mutex_.Unlock();
  }
}

bool Notification::WaitForNotificationWithTimeout(
    absl::Duration timeout) const {
  bool notified = HasBeenNotifiedInternal(&this->notified_yet_);
  if (!notified) {
    notified = this->mutex_.LockWhenWithTimeout(
        Condition(&HasBeenNotifiedInternal, &this->notified_yet_), timeout);
    this->mutex_.Unlock();
  }
  return notified;
}

bool Notification::WaitForNotificationWithDeadline(absl::Time deadline) const {
  bool notified = HasBeenNotifiedInternal(&this->notified_yet_);
  if (!notified) {
    notified = this->mutex_.LockWhenWithDeadline(
        Condition(&HasBeenNotifiedInternal, &this->notified_yet_), deadline);
    this->mutex_.Unlock();
  }
  return notified;
}

}  // inline namespace lts_2018_06_20
}  // namespace absl
