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

#include BOSS_ABSEILCPP_U_absl__time__clock_h //original-code:"absl/time/clock.h"

#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdint>
#include <ctime>
#include <limits>

#include BOSS_ABSEILCPP_U_absl__base__internal__spinlock_h //original-code:"absl/base/internal/spinlock.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__unscaledcycleclock_h //original-code:"absl/base/internal/unscaledcycleclock.h"
#include BOSS_ABSEILCPP_U_absl__base__macros_h //original-code:"absl/base/macros.h"
#include BOSS_ABSEILCPP_U_absl__base__port_h //original-code:"absl/base/port.h"
#include BOSS_ABSEILCPP_U_absl__base__thread_annotations_h //original-code:"absl/base/thread_annotations.h"

namespace absl {
inline namespace lts_2018_06_20 {
Time Now() {
  // TODO(bww): Get a timespec instead so we don't have to divide.
  int64_t n = absl::GetCurrentTimeNanos();
  if (n >= 0) {
    return time_internal::FromUnixDuration(
        time_internal::MakeDuration(n / 1000000000, n % 1000000000 * 4));
  }
  return time_internal::FromUnixDuration(absl::Nanoseconds(n));
}
}  // inline namespace lts_2018_06_20
}  // namespace absl

// Decide if we should use the fast GetCurrentTimeNanos() algorithm
// based on the cyclecounter, otherwise just get the time directly
// from the OS on every call. This can be chosen at compile-time via
// -DABSL_USE_CYCLECLOCK_FOR_GET_CURRENT_TIME_NANOS=[0|1]
#ifndef ABSL_USE_CYCLECLOCK_FOR_GET_CURRENT_TIME_NANOS
#if ABSL_USE_UNSCALED_CYCLECLOCK
#define ABSL_USE_CYCLECLOCK_FOR_GET_CURRENT_TIME_NANOS 1
#else
#define ABSL_USE_CYCLECLOCK_FOR_GET_CURRENT_TIME_NANOS 0
#endif
#endif

#if defined(__APPLE__)
#include BOSS_ABSEILCPP_U_absl__time__internal__get_current_time_ios_inc //original-code:"absl/time/internal/get_current_time_ios.inc"
#elif defined(_WIN32)
#include BOSS_ABSEILCPP_U_absl__time__internal__get_current_time_windows_inc //original-code:"absl/time/internal/get_current_time_windows.inc"
#else
#include BOSS_ABSEILCPP_U_absl__time__internal__get_current_time_posix_inc //original-code:"absl/time/internal/get_current_time_posix.inc"
#endif

// Allows override by test.
#ifndef GET_CURRENT_TIME_NANOS_FROM_SYSTEM
#define GET_CURRENT_TIME_NANOS_FROM_SYSTEM() \
  ::absl::time_internal::GetCurrentTimeNanosFromSystem()
#endif

#if !ABSL_USE_CYCLECLOCK_FOR_GET_CURRENT_TIME_NANOS
namespace absl {
inline namespace lts_2018_06_20 {
int64_t GetCurrentTimeNanos() {
  return GET_CURRENT_TIME_NANOS_FROM_SYSTEM();
}
}  // inline namespace lts_2018_06_20
}  // namespace absl
#else  // Use the cyclecounter-based implementation below.

// Allows override by test.
#ifndef GET_CURRENT_TIME_NANOS_CYCLECLOCK_NOW
#define GET_CURRENT_TIME_NANOS_CYCLECLOCK_NOW() \
  ::absl::time_internal::UnscaledCycleClockWrapperForGetCurrentTime::Now()
#endif

// The following counters are used only by the test code.
static int64_t stats_initializations;
static int64_t stats_reinitializations;
static int64_t stats_calibrations;
static int64_t stats_slow_paths;
static int64_t stats_fast_slow_paths;

namespace absl {
inline namespace lts_2018_06_20 {
namespace time_internal {
// This is a friend wrapper around UnscaledCycleClock::Now()
// (needed to access UnscaledCycleClock).
class UnscaledCycleClockWrapperForGetCurrentTime {
 public:
  static int64_t Now() { return base_internal::UnscaledCycleClock::Now(); }
};
}  // namespace time_internal

// uint64_t is used in this module to provide an extra bit in multiplications

// Return the time in ns as told by the kernel interface.  Place in *cycleclock
// the value of the cycleclock at about the time of the syscall.
// This call represents the time base that this module synchronizes to.
// Ensures that *cycleclock does not step back by up to (1 << 16) from
// last_cycleclock, to discard small backward counter steps.  (Larger steps are
// assumed to be complete resyncs, which shouldn't happen.  If they do, a full
// reinitialization of the outer algorithm should occur.)
static int64_t GetCurrentTimeNanosFromKernel(uint64_t last_cycleclock,
                                             uint64_t *cycleclock) {
  // We try to read clock values at about the same time as the kernel clock.
  // This value gets adjusted up or down as estimate of how long that should
  // take, so we can reject attempts that take unusually long.
  static std::atomic<uint64_t> approx_syscall_time_in_cycles{10 * 1000};

  uint64_t local_approx_syscall_time_in_cycles =  // local copy
      approx_syscall_time_in_cycles.load(std::memory_order_relaxed);

  int64_t current_time_nanos_from_system;
  uint64_t before_cycles;
  uint64_t after_cycles;
  uint64_t elapsed_cycles;
  int loops = 0;
  do {
    before_cycles = GET_CURRENT_TIME_NANOS_CYCLECLOCK_NOW();
    current_time_nanos_from_system = GET_CURRENT_TIME_NANOS_FROM_SYSTEM();
    after_cycles = GET_CURRENT_TIME_NANOS_CYCLECLOCK_NOW();
    // elapsed_cycles is unsigned, so is large on overflow
    elapsed_cycles = after_cycles - before_cycles;
    if (elapsed_cycles >= local_approx_syscall_time_in_cycles &&
        ++loops == 20) {  // clock changed frequencies?  Back off.
      loops = 0;
      if (local_approx_syscall_time_in_cycles < 1000 * 1000) {
        local_approx_syscall_time_in_cycles =
            (local_approx_syscall_time_in_cycles + 1) << 1;
      }
      approx_syscall_time_in_cycles.store(
          local_approx_syscall_time_in_cycles,
          std::memory_order_relaxed);
    }
  } while (elapsed_cycles >= local_approx_syscall_time_in_cycles ||
           last_cycleclock - after_cycles < (static_cast<uint64_t>(1) << 16));

  // Number of times in a row we've seen a kernel time call take substantially
  // less than approx_syscall_time_in_cycles.
  static std::atomic<uint32_t> seen_smaller{ 0 };

  // Adjust approx_syscall_time_in_cycles to be within a factor of 2
  // of the typical time to execute one iteration of the loop above.
  if ((local_approx_syscall_time_in_cycles >> 1) < elapsed_cycles) {
    // measured time is no smaller than half current approximation
    seen_smaller.store(0, std::memory_order_relaxed);
  } else if (seen_smaller.fetch_add(1, std::memory_order_relaxed) >= 3) {
    // smaller delays several times in a row; reduce approximation by 12.5%
    const uint64_t new_approximation =
        local_approx_syscall_time_in_cycles -
        (local_approx_syscall_time_in_cycles >> 3);
    approx_syscall_time_in_cycles.store(new_approximation,
                                        std::memory_order_relaxed);
    seen_smaller.store(0, std::memory_order_relaxed);
  }

  *cycleclock = after_cycles;
  return current_time_nanos_from_system;
}


// ---------------------------------------------------------------------
// An implementation of reader-write locks that use no atomic ops in the read
// case.  This is a generalization of Lamport's method for reading a multiword
// clock.  Increment a word on each write acquisition, using the low-order bit
// as a spinlock; the word is the high word of the "clock".  Readers read the
// high word, then all other data, then the high word again, and repeat the
// read if the reads of the high words yields different answers, or an odd
// value (either case suggests possible interference from a writer).
// Here we use a spinlock to ensure only one writer at a time, rather than
// spinning on the bottom bit of the word to benefit from SpinLock
// spin-delay tuning.

// Acquire seqlock (*seq) and return the value to be written to unlock.
static inline uint64_t SeqAcquire(std::atomic<uint64_t> *seq) {
  uint64_t x = seq->fetch_add(1, std::memory_order_relaxed);

  // We put a release fence between update to *seq and writes to shared data.
  // Thus all stores to shared data are effectively release operations and
  // update to *seq above cannot be re-ordered past any of them.  Note that
  // this barrier is not for the fetch_add above.  A release barrier for the
  // fetch_add would be before it, not after.
  std::atomic_thread_fence(std::memory_order_release);

  return x + 2;   // original word plus 2
}

// Release seqlock (*seq) by writing x to it---a value previously returned by
// SeqAcquire.
static inline void SeqRelease(std::atomic<uint64_t> *seq, uint64_t x) {
  // The unlock store to *seq must have release ordering so that all
  // updates to shared data must finish before this store.
  seq->store(x, std::memory_order_release);  // release lock for readers
}

// ---------------------------------------------------------------------

// "nsscaled" is unit of time equal to a (2**kScale)th of a nanosecond.
enum { kScale = 30 };

// The minimum interval between samples of the time base.
// We pick enough time to amortize the cost of the sample,
// to get a reasonably accurate cycle counter rate reading,
// and not so much that calculations will overflow 64-bits.
static const uint64_t kMinNSBetweenSamples = 2000 << 20;

// We require that kMinNSBetweenSamples shifted by kScale
// have at least a bit left over for 64-bit calculations.
static_assert(((kMinNSBetweenSamples << (kScale + 1)) >> (kScale + 1)) ==
               kMinNSBetweenSamples,
               "cannot represent kMaxBetweenSamplesNSScaled");

// A reader-writer lock protecting the static locations below.
// See SeqAcquire() and SeqRelease() above.
static absl::base_internal::SpinLock lock(
    absl::base_internal::kLinkerInitialized);
static std::atomic<uint64_t> seq(0);

// data from a sample of the kernel's time value
struct TimeSampleAtomic {
  std::atomic<uint64_t> raw_ns;              // raw kernel time
  std::atomic<uint64_t> base_ns;             // our estimate of time
  std::atomic<uint64_t> base_cycles;         // cycle counter reading
  std::atomic<uint64_t> nsscaled_per_cycle;  // cycle period
  // cycles before we'll sample again (a scaled reciprocal of the period,
  // to avoid a division on the fast path).
  std::atomic<uint64_t> min_cycles_per_sample;
};
// Same again, but with non-atomic types
struct TimeSample {
  uint64_t raw_ns;                 // raw kernel time
  uint64_t base_ns;                // our estimate of time
  uint64_t base_cycles;            // cycle counter reading
  uint64_t nsscaled_per_cycle;     // cycle period
  uint64_t min_cycles_per_sample;  // approx cycles before next sample
};

static struct TimeSampleAtomic last_sample;   // the last sample; under seq

static int64_t GetCurrentTimeNanosSlowPath() ABSL_ATTRIBUTE_COLD;

// Read the contents of *atomic into *sample.
// Each field is read atomically, but to maintain atomicity between fields,
// the access must be done under a lock.
static void ReadTimeSampleAtomic(const struct TimeSampleAtomic *atomic,
                                 struct TimeSample *sample) {
  sample->base_ns = atomic->base_ns.load(std::memory_order_relaxed);
  sample->base_cycles = atomic->base_cycles.load(std::memory_order_relaxed);
  sample->nsscaled_per_cycle =
      atomic->nsscaled_per_cycle.load(std::memory_order_relaxed);
  sample->min_cycles_per_sample =
      atomic->min_cycles_per_sample.load(std::memory_order_relaxed);
  sample->raw_ns = atomic->raw_ns.load(std::memory_order_relaxed);
}

// Public routine.
// Algorithm:  We wish to compute real time from a cycle counter.  In normal
// operation, we construct a piecewise linear approximation to the kernel time
// source, using the cycle counter value.  The start of each line segment is at
// the same point as the end of the last, but may have a different slope (that
// is, a different idea of the cycle counter frequency).  Every couple of
// seconds, the kernel time source is sampled and compared with the current
// approximation.  A new slope is chosen that, if followed for another couple
// of seconds, will correct the error at the current position.  The information
// for a sample is in the "last_sample" struct.  The linear approximation is
//   estimated_time = last_sample.base_ns +
//     last_sample.ns_per_cycle * (counter_reading - last_sample.base_cycles)
// (ns_per_cycle is actually stored in different units and scaled, to avoid
// overflow).  The base_ns of the next linear approximation is the
// estimated_time using the last approximation; the base_cycles is the cycle
// counter value at that time; the ns_per_cycle is the number of ns per cycle
// measured since the last sample, but adjusted so that most of the difference
// between the estimated_time and the kernel time will be corrected by the
// estimated time to the next sample.  In normal operation, this algorithm
// relies on:
// - the cycle counter and kernel time rates not changing a lot in a few
//   seconds.
// - the client calling into the code often compared to a couple of seconds, so
//   the time to the next correction can be estimated.
// Any time ns_per_cycle is not known, a major error is detected, or the
// assumption about frequent calls is violated, the implementation returns the
// kernel time.  It records sufficient data that a linear approximation can
// resume a little later.

int64_t GetCurrentTimeNanos() {
  // read the data from the "last_sample" struct (but don't need raw_ns yet)
  // The reads of "seq" and test of the values emulate a reader lock.
  uint64_t base_ns;
  uint64_t base_cycles;
  uint64_t nsscaled_per_cycle;
  uint64_t min_cycles_per_sample;
  uint64_t seq_read0;
  uint64_t seq_read1;

  // If we have enough information to interpolate, the value returned will be
  // derived from this cycleclock-derived time estimate.  On some platforms
  // (POWER) the function to retrieve this value has enough complexity to
  // contribute to register pressure - reading it early before initializing
  // the other pieces of the calculation minimizes spill/restore instructions,
  // minimizing icache cost.
  uint64_t now_cycles = GET_CURRENT_TIME_NANOS_CYCLECLOCK_NOW();

  // Acquire pairs with the barrier in SeqRelease - if this load sees that
  // store, the shared-data reads necessarily see that SeqRelease's updates
  // to the same shared data.
  seq_read0 = seq.load(std::memory_order_acquire);

  base_ns = last_sample.base_ns.load(std::memory_order_relaxed);
  base_cycles = last_sample.base_cycles.load(std::memory_order_relaxed);
  nsscaled_per_cycle =
      last_sample.nsscaled_per_cycle.load(std::memory_order_relaxed);
  min_cycles_per_sample =
      last_sample.min_cycles_per_sample.load(std::memory_order_relaxed);

  // This acquire fence pairs with the release fence in SeqAcquire.  Since it
  // is sequenced between reads of shared data and seq_read1, the reads of
  // shared data are effectively acquiring.
  std::atomic_thread_fence(std::memory_order_acquire);

  // The shared-data reads are effectively acquire ordered, and the
  // shared-data writes are effectively release ordered. Therefore if our
  // shared-data reads see any of a particular update's shared-data writes,
  // seq_read1 is guaranteed to see that update's SeqAcquire.
  seq_read1 = seq.load(std::memory_order_relaxed);

  // Fast path.  Return if min_cycles_per_sample has not yet elapsed since the
  // last sample, and we read a consistent sample.  The fast path activates
  // only when min_cycles_per_sample is non-zero, which happens when we get an
  // estimate for the cycle time.  The predicate will fail if now_cycles <
  // base_cycles, or if some other thread is in the slow path.
  //
  // Since we now read now_cycles before base_ns, it is possible for now_cycles
  // to be less than base_cycles (if we were interrupted between those loads and
  // last_sample was updated). This is harmless, because delta_cycles will wrap
  // and report a time much much bigger than min_cycles_per_sample. In that case
  // we will take the slow path.
  uint64_t delta_cycles = now_cycles - base_cycles;
  if (seq_read0 == seq_read1 && (seq_read0 & 1) == 0 &&
      delta_cycles < min_cycles_per_sample) {
    return base_ns + ((delta_cycles * nsscaled_per_cycle) >> kScale);
  }
  return GetCurrentTimeNanosSlowPath();
}

// Return (a << kScale)/b.
// Zero is returned if b==0.   Scaling is performed internally to
// preserve precision without overflow.
static uint64_t SafeDivideAndScale(uint64_t a, uint64_t b) {
  // Find maximum safe_shift so that
  //  0 <= safe_shift <= kScale  and  (a << safe_shift) does not overflow.
  int safe_shift = kScale;
  while (((a << safe_shift) >> safe_shift) != a) {
    safe_shift--;
  }
  uint64_t scaled_b = b >> (kScale - safe_shift);
  uint64_t quotient = 0;
  if (scaled_b != 0) {
    quotient = (a << safe_shift) / scaled_b;
  }
  return quotient;
}

static uint64_t UpdateLastSample(
    uint64_t now_cycles, uint64_t now_ns, uint64_t delta_cycles,
    const struct TimeSample *sample) ABSL_ATTRIBUTE_COLD;

// The slow path of GetCurrentTimeNanos().  This is taken while gathering
// initial samples, when enough time has elapsed since the last sample, and if
// any other thread is writing to last_sample.
//
// Manually mark this 'noinline' to minimize stack frame size of the fast
// path.  Without this, sometimes a compiler may inline this big block of code
// into the fast past.  That causes lots of register spills and reloads that
// are unnecessary unless the slow path is taken.
//
// TODO(absl-team): Remove this attribute when our compiler is smart enough
// to do the right thing.
ABSL_ATTRIBUTE_NOINLINE
static int64_t GetCurrentTimeNanosSlowPath() LOCKS_EXCLUDED(lock) {
  // Serialize access to slow-path.  Fast-path readers are not blocked yet, and
  // code below must not modify last_sample until the seqlock is acquired.
  lock.Lock();

  // Sample the kernel time base.  This is the definition of
  // "now" if we take the slow path.
  static uint64_t last_now_cycles;  // protected by lock
  uint64_t now_cycles;
  uint64_t now_ns = GetCurrentTimeNanosFromKernel(last_now_cycles, &now_cycles);
  last_now_cycles = now_cycles;

  uint64_t estimated_base_ns;

  // ----------
  // Read the "last_sample" values again; this time holding the write lock.
  struct TimeSample sample;
  ReadTimeSampleAtomic(&last_sample, &sample);

  // ----------
  // Try running the fast path again; another thread may have updated the
  // sample between our run of the fast path and the sample we just read.
  uint64_t delta_cycles = now_cycles - sample.base_cycles;
  if (delta_cycles < sample.min_cycles_per_sample) {
    // Another thread updated the sample.  This path does not take the seqlock
    // so that blocked readers can make progress without blocking new readers.
    estimated_base_ns = sample.base_ns +
        ((delta_cycles * sample.nsscaled_per_cycle) >> kScale);
    stats_fast_slow_paths++;
  } else {
    estimated_base_ns =
        UpdateLastSample(now_cycles, now_ns, delta_cycles, &sample);
  }

  lock.Unlock();

  return estimated_base_ns;
}

// Main part of the algorithm.  Locks out readers, updates the approximation
// using the new sample from the kernel, and stores the result in last_sample
// for readers.  Returns the new estimated time.
static uint64_t UpdateLastSample(uint64_t now_cycles, uint64_t now_ns,
                                 uint64_t delta_cycles,
                                 const struct TimeSample *sample)
    EXCLUSIVE_LOCKS_REQUIRED(lock) {
  uint64_t estimated_base_ns = now_ns;
  uint64_t lock_value = SeqAcquire(&seq);  // acquire seqlock to block readers

  // The 5s in the next if-statement limits the time for which we will trust
  // the cycle counter and our last sample to give a reasonable result.
  // Errors in the rate of the source clock can be multiplied by the ratio
  // between this limit and kMinNSBetweenSamples.
  if (sample->raw_ns == 0 ||  // no recent sample, or clock went backwards
      sample->raw_ns + static_cast<uint64_t>(5) * 1000 * 1000 * 1000 < now_ns ||
      now_ns < sample->raw_ns || now_cycles < sample->base_cycles) {
    // record this sample, and forget any previously known slope.
    last_sample.raw_ns.store(now_ns, std::memory_order_relaxed);
    last_sample.base_ns.store(estimated_base_ns, std::memory_order_relaxed);
    last_sample.base_cycles.store(now_cycles, std::memory_order_relaxed);
    last_sample.nsscaled_per_cycle.store(0, std::memory_order_relaxed);
    last_sample.min_cycles_per_sample.store(0, std::memory_order_relaxed);
    stats_initializations++;
  } else if (sample->raw_ns + 500 * 1000 * 1000 < now_ns &&
             sample->base_cycles + 100 < now_cycles) {
    // Enough time has passed to compute the cycle time.
    if (sample->nsscaled_per_cycle != 0) {  // Have a cycle time estimate.
      // Compute time from counter reading, but avoiding overflow
      // delta_cycles may be larger than on the fast path.
      uint64_t estimated_scaled_ns;
      int s = -1;
      do {
        s++;
        estimated_scaled_ns = (delta_cycles >> s) * sample->nsscaled_per_cycle;
      } while (estimated_scaled_ns / sample->nsscaled_per_cycle !=
               (delta_cycles >> s));
      estimated_base_ns = sample->base_ns +
                          (estimated_scaled_ns >> (kScale - s));
    }

    // Compute the assumed cycle time kMinNSBetweenSamples ns into the future
    // assuming the cycle counter rate stays the same as the last interval.
    uint64_t ns = now_ns - sample->raw_ns;
    uint64_t measured_nsscaled_per_cycle = SafeDivideAndScale(ns, delta_cycles);

    uint64_t assumed_next_sample_delta_cycles =
        SafeDivideAndScale(kMinNSBetweenSamples, measured_nsscaled_per_cycle);

    int64_t diff_ns = now_ns - estimated_base_ns;  // estimate low by this much

    // We want to set nsscaled_per_cycle so that our estimate of the ns time
    // at the assumed cycle time is the assumed ns time.
    // That is, we want to set nsscaled_per_cycle so:
    //  kMinNSBetweenSamples + diff_ns  ==
    //  (assumed_next_sample_delta_cycles * nsscaled_per_cycle) >> kScale
    // But we wish to damp oscillations, so instead correct only most
    // of our current error, by solving:
    //  kMinNSBetweenSamples + diff_ns - (diff_ns / 16) ==
    //  (assumed_next_sample_delta_cycles * nsscaled_per_cycle) >> kScale
    ns = kMinNSBetweenSamples + diff_ns - (diff_ns / 16);
    uint64_t new_nsscaled_per_cycle =
        SafeDivideAndScale(ns, assumed_next_sample_delta_cycles);
    if (new_nsscaled_per_cycle != 0 &&
        diff_ns < 100 * 1000 * 1000 && -diff_ns < 100 * 1000 * 1000) {
      // record the cycle time measurement
      last_sample.nsscaled_per_cycle.store(
          new_nsscaled_per_cycle, std::memory_order_relaxed);
      uint64_t new_min_cycles_per_sample =
          SafeDivideAndScale(kMinNSBetweenSamples, new_nsscaled_per_cycle);
      last_sample.min_cycles_per_sample.store(
          new_min_cycles_per_sample, std::memory_order_relaxed);
      stats_calibrations++;
    } else {  // something went wrong; forget the slope
      last_sample.nsscaled_per_cycle.store(0, std::memory_order_relaxed);
      last_sample.min_cycles_per_sample.store(0, std::memory_order_relaxed);
      estimated_base_ns = now_ns;
      stats_reinitializations++;
    }
    last_sample.raw_ns.store(now_ns, std::memory_order_relaxed);
    last_sample.base_ns.store(estimated_base_ns, std::memory_order_relaxed);
    last_sample.base_cycles.store(now_cycles, std::memory_order_relaxed);
  } else {
    // have a sample, but no slope; waiting for enough time for a calibration
    stats_slow_paths++;
  }

  SeqRelease(&seq, lock_value);  // release the readers

  return estimated_base_ns;
}
}  // inline namespace lts_2018_06_20
}  // namespace absl
#endif  // ABSL_USE_CYCLECLOCK_FOR_GET_CURRENT_TIME_NANOS

namespace absl {
inline namespace lts_2018_06_20 {
namespace {

// Returns the maximum duration that SleepOnce() can sleep for.
constexpr absl::Duration MaxSleep() {
#ifdef _WIN32
  // Windows Sleep() takes unsigned long argument in milliseconds.
  return absl::Milliseconds(
      std::numeric_limits<unsigned long>::max());  // NOLINT(runtime/int)
#else
  return absl::Seconds(std::numeric_limits<time_t>::max());
#endif
}

// Sleeps for the given duration.
// REQUIRES: to_sleep <= MaxSleep().
void SleepOnce(absl::Duration to_sleep) {
#ifdef _WIN32
  Sleep(to_sleep / absl::Milliseconds(1));
#else
  struct timespec sleep_time = absl::ToTimespec(to_sleep);
  while (nanosleep(&sleep_time, &sleep_time) != 0 && errno == EINTR) {
    // Ignore signals and wait for the full interval to elapse.
  }
#endif
}

}  // namespace
}  // inline namespace lts_2018_06_20
}  // namespace absl

extern "C" {

ABSL_ATTRIBUTE_WEAK void AbslInternalSleepFor(absl::Duration duration) {
  while (duration > absl::ZeroDuration()) {
    absl::Duration to_sleep = std::min(duration, absl::MaxSleep());
    absl::SleepOnce(to_sleep);
    duration -= to_sleep;
  }
}

}  // extern "C"
