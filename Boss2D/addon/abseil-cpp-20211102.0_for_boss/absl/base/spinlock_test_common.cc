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

// A bunch of threads repeatedly hash an array of ints protected by a
// spinlock.  If the spinlock is working properly, all elements of the
// array should be equal at the end of the test.

#include <cstdint>
#include <limits>
#include <random>
#include <thread>  // NOLINT(build/c++11)
#include <type_traits>
#include <vector>

#include "gtest/gtest.h"
#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_ABSEILCPP_U_absl__base__config_h //original-code:"absl/base/config.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__low_level_scheduling_h //original-code:"absl/base/internal/low_level_scheduling.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__scheduling_mode_h //original-code:"absl/base/internal/scheduling_mode.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__spinlock_h //original-code:"absl/base/internal/spinlock.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__sysinfo_h //original-code:"absl/base/internal/sysinfo.h"
#include BOSS_ABSEILCPP_U_absl__base__macros_h //original-code:"absl/base/macros.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__blocking_counter_h //original-code:"absl/synchronization/blocking_counter.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__notification_h //original-code:"absl/synchronization/notification.h"

constexpr int32_t kNumThreads = 10;
constexpr int32_t kIters = 1000;

namespace absl {
ABSL_NAMESPACE_BEGIN
namespace base_internal {

// This is defined outside of anonymous namespace so that it can be
// a friend of SpinLock to access protected methods for testing.
struct SpinLockTest {
  static uint32_t EncodeWaitCycles(int64_t wait_start_time,
                                   int64_t wait_end_time) {
    return SpinLock::EncodeWaitCycles(wait_start_time, wait_end_time);
  }
  static uint64_t DecodeWaitCycles(uint32_t lock_value) {
    return SpinLock::DecodeWaitCycles(lock_value);
  }
};

namespace {

static constexpr int kArrayLength = 10;
static uint32_t values[kArrayLength];

ABSL_CONST_INIT static SpinLock static_cooperative_spinlock(
    absl::kConstInit, base_internal::SCHEDULE_COOPERATIVE_AND_KERNEL);
ABSL_CONST_INIT static SpinLock static_noncooperative_spinlock(
    absl::kConstInit, base_internal::SCHEDULE_KERNEL_ONLY);

// Simple integer hash function based on the public domain lookup2 hash.
// http://burtleburtle.net/bob/c/lookup2.c
static uint32_t Hash32(uint32_t a, uint32_t c) {
  uint32_t b = 0x9e3779b9UL;  // The golden ratio; an arbitrary value.
  a -= b; a -= c; a ^= (c >> 13);
  b -= c; b -= a; b ^= (a << 8);
  c -= a; c -= b; c ^= (b >> 13);
  a -= b; a -= c; a ^= (c >> 12);
  b -= c; b -= a; b ^= (a << 16);
  c -= a; c -= b; c ^= (b >> 5);
  a -= b; a -= c; a ^= (c >> 3);
  b -= c; b -= a; b ^= (a << 10);
  c -= a; c -= b; c ^= (b >> 15);
  return c;
}

static void TestFunction(int thread_salt, SpinLock* spinlock) {
  for (int i = 0; i < kIters; i++) {
    SpinLockHolder h(spinlock);
    for (int j = 0; j < kArrayLength; j++) {
      const int index = (j + thread_salt) % kArrayLength;
      values[index] = Hash32(values[index], thread_salt);
      std::this_thread::yield();
    }
  }
}

static void ThreadedTest(SpinLock* spinlock) {
  std::vector<std::thread> threads;
  threads.reserve(kNumThreads);
  for (int i = 0; i < kNumThreads; ++i) {
    threads.push_back(std::thread(TestFunction, i, spinlock));
  }
  for (auto& thread : threads) {
    thread.join();
  }

  SpinLockHolder h(spinlock);
  for (int i = 1; i < kArrayLength; i++) {
    EXPECT_EQ(values[0], values[i]);
  }
}

#ifndef ABSL_HAVE_THREAD_SANITIZER
static_assert(std::is_trivially_destructible<SpinLock>(), "");
#endif

TEST(SpinLock, StackNonCooperativeDisablesScheduling) {
  SpinLock spinlock(base_internal::SCHEDULE_KERNEL_ONLY);
  spinlock.Lock();
  EXPECT_FALSE(base_internal::SchedulingGuard::ReschedulingIsAllowed());
  spinlock.Unlock();
}

TEST(SpinLock, StaticNonCooperativeDisablesScheduling) {
  static_noncooperative_spinlock.Lock();
  EXPECT_FALSE(base_internal::SchedulingGuard::ReschedulingIsAllowed());
  static_noncooperative_spinlock.Unlock();
}

TEST(SpinLock, WaitCyclesEncoding) {
  // These are implementation details not exported by SpinLock.
  const int kProfileTimestampShift = 7;
  const int kLockwordReservedShift = 3;
  const uint32_t kSpinLockSleeper = 8;

  // We should be able to encode up to (1^kMaxCycleBits - 1) without clamping
  // but the lower kProfileTimestampShift will be dropped.
  const int kMaxCyclesShift =
    32 - kLockwordReservedShift + kProfileTimestampShift;
  const uint64_t kMaxCycles = (int64_t{1} << kMaxCyclesShift) - 1;

  // These bits should be zero after encoding.
  const uint32_t kLockwordReservedMask = (1 << kLockwordReservedShift) - 1;

  // These bits are dropped when wait cycles are encoded.
  const uint64_t kProfileTimestampMask = (1 << kProfileTimestampShift) - 1;

  // Test a bunch of random values
  std::default_random_engine generator;
  // Shift to avoid overflow below.
  std::uniform_int_distribution<uint64_t> time_distribution(
      0, std::numeric_limits<uint64_t>::max() >> 4);
  std::uniform_int_distribution<uint64_t> cycle_distribution(0, kMaxCycles);

  for (int i = 0; i < 100; i++) {
    int64_t start_time = time_distribution(generator);
    int64_t cycles = cycle_distribution(generator);
    int64_t end_time = start_time + cycles;
    uint32_t lock_value = SpinLockTest::EncodeWaitCycles(start_time, end_time);
    EXPECT_EQ(0, lock_value & kLockwordReservedMask);
    uint64_t decoded = SpinLockTest::DecodeWaitCycles(lock_value);
    EXPECT_EQ(0, decoded & kProfileTimestampMask);
    EXPECT_EQ(cycles & ~kProfileTimestampMask, decoded);
  }

  // Test corner cases
  int64_t start_time = time_distribution(generator);
  EXPECT_EQ(kSpinLockSleeper,
            SpinLockTest::EncodeWaitCycles(start_time, start_time));
  EXPECT_EQ(0, SpinLockTest::DecodeWaitCycles(0));
  EXPECT_EQ(0, SpinLockTest::DecodeWaitCycles(kLockwordReservedMask));
  EXPECT_EQ(kMaxCycles & ~kProfileTimestampMask,
            SpinLockTest::DecodeWaitCycles(~kLockwordReservedMask));

  // Check that we cannot produce kSpinLockSleeper during encoding.
  int64_t sleeper_cycles =
      kSpinLockSleeper << (kProfileTimestampShift - kLockwordReservedShift);
  uint32_t sleeper_value =
      SpinLockTest::EncodeWaitCycles(start_time, start_time + sleeper_cycles);
  EXPECT_NE(sleeper_value, kSpinLockSleeper);

  // Test clamping
  uint32_t max_value =
    SpinLockTest::EncodeWaitCycles(start_time, start_time + kMaxCycles);
  uint64_t max_value_decoded = SpinLockTest::DecodeWaitCycles(max_value);
  uint64_t expected_max_value_decoded = kMaxCycles & ~kProfileTimestampMask;
  EXPECT_EQ(expected_max_value_decoded, max_value_decoded);

  const int64_t step = (1 << kProfileTimestampShift);
  uint32_t after_max_value =
    SpinLockTest::EncodeWaitCycles(start_time, start_time + kMaxCycles + step);
  uint64_t after_max_value_decoded =
      SpinLockTest::DecodeWaitCycles(after_max_value);
  EXPECT_EQ(expected_max_value_decoded, after_max_value_decoded);

  uint32_t before_max_value = SpinLockTest::EncodeWaitCycles(
      start_time, start_time + kMaxCycles - step);
  uint64_t before_max_value_decoded =
    SpinLockTest::DecodeWaitCycles(before_max_value);
  EXPECT_GT(expected_max_value_decoded, before_max_value_decoded);
}

TEST(SpinLockWithThreads, StackSpinLock) {
  SpinLock spinlock;
  ThreadedTest(&spinlock);
}

TEST(SpinLockWithThreads, StackCooperativeSpinLock) {
  SpinLock spinlock(base_internal::SCHEDULE_COOPERATIVE_AND_KERNEL);
  ThreadedTest(&spinlock);
}

TEST(SpinLockWithThreads, StackNonCooperativeSpinLock) {
  SpinLock spinlock(base_internal::SCHEDULE_KERNEL_ONLY);
  ThreadedTest(&spinlock);
}

TEST(SpinLockWithThreads, StaticCooperativeSpinLock) {
  ThreadedTest(&static_cooperative_spinlock);
}

TEST(SpinLockWithThreads, StaticNonCooperativeSpinLock) {
  ThreadedTest(&static_noncooperative_spinlock);
}

TEST(SpinLockWithThreads, DoesNotDeadlock) {
  struct Helper {
    static void NotifyThenLock(Notification* locked, SpinLock* spinlock,
                               BlockingCounter* b) {
      locked->WaitForNotification();  // Wait for LockThenWait() to hold "s".
      b->DecrementCount();
      SpinLockHolder l(spinlock);
    }

    static void LockThenWait(Notification* locked, SpinLock* spinlock,
                             BlockingCounter* b) {
      SpinLockHolder l(spinlock);
      locked->Notify();
      b->Wait();
    }

    static void DeadlockTest(SpinLock* spinlock, int num_spinners) {
      Notification locked;
      BlockingCounter counter(num_spinners);
      std::vector<std::thread> threads;

      threads.push_back(
          std::thread(Helper::LockThenWait, &locked, spinlock, &counter));
      for (int i = 0; i < num_spinners; ++i) {
        threads.push_back(
            std::thread(Helper::NotifyThenLock, &locked, spinlock, &counter));
      }

      for (auto& thread : threads) {
        thread.join();
      }
    }
  };

  SpinLock stack_cooperative_spinlock(
      base_internal::SCHEDULE_COOPERATIVE_AND_KERNEL);
  SpinLock stack_noncooperative_spinlock(base_internal::SCHEDULE_KERNEL_ONLY);
  Helper::DeadlockTest(&stack_cooperative_spinlock,
                       base_internal::NumCPUs() * 2);
  Helper::DeadlockTest(&stack_noncooperative_spinlock,
                       base_internal::NumCPUs() * 2);
  Helper::DeadlockTest(&static_cooperative_spinlock,
                       base_internal::NumCPUs() * 2);
  Helper::DeadlockTest(&static_noncooperative_spinlock,
                       base_internal::NumCPUs() * 2);
}

}  // namespace
}  // namespace base_internal
ABSL_NAMESPACE_END
}  // namespace absl
