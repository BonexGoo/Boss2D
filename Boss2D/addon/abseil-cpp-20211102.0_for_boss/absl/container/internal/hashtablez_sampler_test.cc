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

#include BOSS_ABSEILCPP_U_absl__container__internal__hashtablez_sampler_h //original-code:"absl/container/internal/hashtablez_sampler.h"

#include <atomic>
#include <limits>
#include <random>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_ABSEILCPP_U_absl__container__internal__have_sse_h //original-code:"absl/container/internal/have_sse.h"
#include "absl/profiling/internal/sample_recorder.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__blocking_counter_h //original-code:"absl/synchronization/blocking_counter.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__internal__thread_pool_h //original-code:"absl/synchronization/internal/thread_pool.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__mutex_h //original-code:"absl/synchronization/mutex.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__notification_h //original-code:"absl/synchronization/notification.h"
#include BOSS_ABSEILCPP_U_absl__time__clock_h //original-code:"absl/time/clock.h"
#include BOSS_ABSEILCPP_U_absl__time__time_h //original-code:"absl/time/time.h"

#if ABSL_INTERNAL_RAW_HASH_SET_HAVE_SSE2
constexpr int kProbeLength = 16;
#else
constexpr int kProbeLength = 8;
#endif

namespace absl {
ABSL_NAMESPACE_BEGIN
namespace container_internal {
#if defined(ABSL_INTERNAL_HASHTABLEZ_SAMPLE)
class HashtablezInfoHandlePeer {
 public:
  static bool IsSampled(const HashtablezInfoHandle& h) {
    return h.info_ != nullptr;
  }

  static HashtablezInfo* GetInfo(HashtablezInfoHandle* h) { return h->info_; }
};
#else
class HashtablezInfoHandlePeer {
 public:
  static bool IsSampled(const HashtablezInfoHandle&) { return false; }
  static HashtablezInfo* GetInfo(HashtablezInfoHandle*) { return nullptr; }
};
#endif  // defined(ABSL_INTERNAL_HASHTABLEZ_SAMPLE)

namespace {
using ::absl::synchronization_internal::ThreadPool;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAre;

std::vector<size_t> GetSizes(HashtablezSampler* s) {
  std::vector<size_t> res;
  s->Iterate([&](const HashtablezInfo& info) {
    res.push_back(info.size.load(std::memory_order_acquire));
  });
  return res;
}

HashtablezInfo* Register(HashtablezSampler* s, size_t size) {
  auto* info = s->Register();
  assert(info != nullptr);
  info->size.store(size);
  return info;
}

TEST(HashtablezInfoTest, PrepareForSampling) {
  absl::Time test_start = absl::Now();
  const size_t test_element_size = 17;
  HashtablezInfo info;
  absl::MutexLock l(&info.init_mu);
  info.PrepareForSampling();

  info.inline_element_size = test_element_size;
  EXPECT_EQ(info.capacity.load(), 0);
  EXPECT_EQ(info.size.load(), 0);
  EXPECT_EQ(info.num_erases.load(), 0);
  EXPECT_EQ(info.num_rehashes.load(), 0);
  EXPECT_EQ(info.max_probe_length.load(), 0);
  EXPECT_EQ(info.total_probe_length.load(), 0);
  EXPECT_EQ(info.hashes_bitwise_or.load(), 0);
  EXPECT_EQ(info.hashes_bitwise_and.load(), ~size_t{});
  EXPECT_EQ(info.hashes_bitwise_xor.load(), 0);
  EXPECT_EQ(info.max_reserve.load(), 0);
  EXPECT_GE(info.create_time, test_start);
  EXPECT_EQ(info.inline_element_size, test_element_size);

  info.capacity.store(1, std::memory_order_relaxed);
  info.size.store(1, std::memory_order_relaxed);
  info.num_erases.store(1, std::memory_order_relaxed);
  info.max_probe_length.store(1, std::memory_order_relaxed);
  info.total_probe_length.store(1, std::memory_order_relaxed);
  info.hashes_bitwise_or.store(1, std::memory_order_relaxed);
  info.hashes_bitwise_and.store(1, std::memory_order_relaxed);
  info.hashes_bitwise_xor.store(1, std::memory_order_relaxed);
  info.max_reserve.store(1, std::memory_order_relaxed);
  info.create_time = test_start - absl::Hours(20);

  info.PrepareForSampling();
  EXPECT_EQ(info.capacity.load(), 0);
  EXPECT_EQ(info.size.load(), 0);
  EXPECT_EQ(info.num_erases.load(), 0);
  EXPECT_EQ(info.num_rehashes.load(), 0);
  EXPECT_EQ(info.max_probe_length.load(), 0);
  EXPECT_EQ(info.total_probe_length.load(), 0);
  EXPECT_EQ(info.hashes_bitwise_or.load(), 0);
  EXPECT_EQ(info.hashes_bitwise_and.load(), ~size_t{});
  EXPECT_EQ(info.hashes_bitwise_xor.load(), 0);
  EXPECT_EQ(info.max_reserve.load(), 0);
  EXPECT_EQ(info.inline_element_size, test_element_size);
  EXPECT_GE(info.create_time, test_start);
}

TEST(HashtablezInfoTest, RecordStorageChanged) {
  HashtablezInfo info;
  absl::MutexLock l(&info.init_mu);
  info.PrepareForSampling();
  RecordStorageChangedSlow(&info, 17, 47);
  EXPECT_EQ(info.size.load(), 17);
  EXPECT_EQ(info.capacity.load(), 47);
  RecordStorageChangedSlow(&info, 20, 20);
  EXPECT_EQ(info.size.load(), 20);
  EXPECT_EQ(info.capacity.load(), 20);
}

TEST(HashtablezInfoTest, RecordInsert) {
  HashtablezInfo info;
  absl::MutexLock l(&info.init_mu);
  info.PrepareForSampling();
  EXPECT_EQ(info.max_probe_length.load(), 0);
  RecordInsertSlow(&info, 0x0000FF00, 6 * kProbeLength);
  EXPECT_EQ(info.max_probe_length.load(), 6);
  EXPECT_EQ(info.hashes_bitwise_and.load(), 0x0000FF00);
  EXPECT_EQ(info.hashes_bitwise_or.load(), 0x0000FF00);
  EXPECT_EQ(info.hashes_bitwise_xor.load(), 0x0000FF00);
  RecordInsertSlow(&info, 0x000FF000, 4 * kProbeLength);
  EXPECT_EQ(info.max_probe_length.load(), 6);
  EXPECT_EQ(info.hashes_bitwise_and.load(), 0x0000F000);
  EXPECT_EQ(info.hashes_bitwise_or.load(), 0x000FFF00);
  EXPECT_EQ(info.hashes_bitwise_xor.load(), 0x000F0F00);
  RecordInsertSlow(&info, 0x00FF0000, 12 * kProbeLength);
  EXPECT_EQ(info.max_probe_length.load(), 12);
  EXPECT_EQ(info.hashes_bitwise_and.load(), 0x00000000);
  EXPECT_EQ(info.hashes_bitwise_or.load(), 0x00FFFF00);
  EXPECT_EQ(info.hashes_bitwise_xor.load(), 0x00F00F00);
}

TEST(HashtablezInfoTest, RecordErase) {
  const size_t test_element_size = 29;
  HashtablezInfo info;
  absl::MutexLock l(&info.init_mu);
  info.PrepareForSampling();
  info.inline_element_size = test_element_size;
  EXPECT_EQ(info.num_erases.load(), 0);
  EXPECT_EQ(info.size.load(), 0);
  RecordInsertSlow(&info, 0x0000FF00, 6 * kProbeLength);
  EXPECT_EQ(info.size.load(), 1);
  RecordEraseSlow(&info);
  EXPECT_EQ(info.size.load(), 0);
  EXPECT_EQ(info.num_erases.load(), 1);
  EXPECT_EQ(info.inline_element_size, test_element_size);
}

TEST(HashtablezInfoTest, RecordRehash) {
  const size_t test_element_size = 31;
  HashtablezInfo info;
  absl::MutexLock l(&info.init_mu);
  info.PrepareForSampling();
  info.inline_element_size = test_element_size;
  RecordInsertSlow(&info, 0x1, 0);
  RecordInsertSlow(&info, 0x2, kProbeLength);
  RecordInsertSlow(&info, 0x4, kProbeLength);
  RecordInsertSlow(&info, 0x8, 2 * kProbeLength);
  EXPECT_EQ(info.size.load(), 4);
  EXPECT_EQ(info.total_probe_length.load(), 4);

  RecordEraseSlow(&info);
  RecordEraseSlow(&info);
  EXPECT_EQ(info.size.load(), 2);
  EXPECT_EQ(info.total_probe_length.load(), 4);
  EXPECT_EQ(info.num_erases.load(), 2);

  RecordRehashSlow(&info, 3 * kProbeLength);
  EXPECT_EQ(info.size.load(), 2);
  EXPECT_EQ(info.total_probe_length.load(), 3);
  EXPECT_EQ(info.num_erases.load(), 0);
  EXPECT_EQ(info.num_rehashes.load(), 1);
  EXPECT_EQ(info.inline_element_size, test_element_size);
}

TEST(HashtablezInfoTest, RecordReservation) {
  HashtablezInfo info;
  absl::MutexLock l(&info.init_mu);
  info.PrepareForSampling();
  RecordReservationSlow(&info, 3);
  EXPECT_EQ(info.max_reserve.load(), 3);

  RecordReservationSlow(&info, 2);
  // High watermark does not change
  EXPECT_EQ(info.max_reserve.load(), 3);

  RecordReservationSlow(&info, 10);
  // High watermark does change
  EXPECT_EQ(info.max_reserve.load(), 10);
}

#if defined(ABSL_INTERNAL_HASHTABLEZ_SAMPLE)
TEST(HashtablezSamplerTest, SmallSampleParameter) {
  const size_t test_element_size = 31;
  SetHashtablezEnabled(true);
  SetHashtablezSampleParameter(100);

  for (int i = 0; i < 1000; ++i) {
    int64_t next_sample = 0;
    HashtablezInfo* sample = SampleSlow(&next_sample, test_element_size);
    EXPECT_GT(next_sample, 0);
    EXPECT_NE(sample, nullptr);
    UnsampleSlow(sample);
  }
}

TEST(HashtablezSamplerTest, LargeSampleParameter) {
  const size_t test_element_size = 31;
  SetHashtablezEnabled(true);
  SetHashtablezSampleParameter(std::numeric_limits<int32_t>::max());

  for (int i = 0; i < 1000; ++i) {
    int64_t next_sample = 0;
    HashtablezInfo* sample = SampleSlow(&next_sample, test_element_size);
    EXPECT_GT(next_sample, 0);
    EXPECT_NE(sample, nullptr);
    UnsampleSlow(sample);
  }
}

TEST(HashtablezSamplerTest, Sample) {
  const size_t test_element_size = 31;
  SetHashtablezEnabled(true);
  SetHashtablezSampleParameter(100);
  int64_t num_sampled = 0;
  int64_t total = 0;
  double sample_rate = 0.0;
  for (int i = 0; i < 1000000; ++i) {
    HashtablezInfoHandle h = Sample(test_element_size);
    ++total;
    if (HashtablezInfoHandlePeer::IsSampled(h)) {
      ++num_sampled;
    }
    sample_rate = static_cast<double>(num_sampled) / total;
    if (0.005 < sample_rate && sample_rate < 0.015) break;
  }
  EXPECT_NEAR(sample_rate, 0.01, 0.005);
}

TEST(HashtablezSamplerTest, Handle) {
  auto& sampler = GlobalHashtablezSampler();
  HashtablezInfoHandle h(sampler.Register());
  auto* info = HashtablezInfoHandlePeer::GetInfo(&h);
  info->hashes_bitwise_and.store(0x12345678, std::memory_order_relaxed);

  bool found = false;
  sampler.Iterate([&](const HashtablezInfo& h) {
    if (&h == info) {
      EXPECT_EQ(h.hashes_bitwise_and.load(), 0x12345678);
      found = true;
    }
  });
  EXPECT_TRUE(found);

  h = HashtablezInfoHandle();
  found = false;
  sampler.Iterate([&](const HashtablezInfo& h) {
    if (&h == info) {
      // this will only happen if some other thread has resurrected the info
      // the old handle was using.
      if (h.hashes_bitwise_and.load() == 0x12345678) {
        found = true;
      }
    }
  });
  EXPECT_FALSE(found);
}
#endif


TEST(HashtablezSamplerTest, Registration) {
  HashtablezSampler sampler;
  auto* info1 = Register(&sampler, 1);
  EXPECT_THAT(GetSizes(&sampler), UnorderedElementsAre(1));

  auto* info2 = Register(&sampler, 2);
  EXPECT_THAT(GetSizes(&sampler), UnorderedElementsAre(1, 2));
  info1->size.store(3);
  EXPECT_THAT(GetSizes(&sampler), UnorderedElementsAre(3, 2));

  sampler.Unregister(info1);
  sampler.Unregister(info2);
}

TEST(HashtablezSamplerTest, Unregistration) {
  HashtablezSampler sampler;
  std::vector<HashtablezInfo*> infos;
  for (size_t i = 0; i < 3; ++i) {
    infos.push_back(Register(&sampler, i));
  }
  EXPECT_THAT(GetSizes(&sampler), UnorderedElementsAre(0, 1, 2));

  sampler.Unregister(infos[1]);
  EXPECT_THAT(GetSizes(&sampler), UnorderedElementsAre(0, 2));

  infos.push_back(Register(&sampler, 3));
  infos.push_back(Register(&sampler, 4));
  EXPECT_THAT(GetSizes(&sampler), UnorderedElementsAre(0, 2, 3, 4));
  sampler.Unregister(infos[3]);
  EXPECT_THAT(GetSizes(&sampler), UnorderedElementsAre(0, 2, 4));

  sampler.Unregister(infos[0]);
  sampler.Unregister(infos[2]);
  sampler.Unregister(infos[4]);
  EXPECT_THAT(GetSizes(&sampler), IsEmpty());
}

TEST(HashtablezSamplerTest, MultiThreaded) {
  HashtablezSampler sampler;
  Notification stop;
  ThreadPool pool(10);

  for (int i = 0; i < 10; ++i) {
    pool.Schedule([&sampler, &stop]() {
      std::random_device rd;
      std::mt19937 gen(rd());

      std::vector<HashtablezInfo*> infoz;
      while (!stop.HasBeenNotified()) {
        if (infoz.empty()) {
          infoz.push_back(sampler.Register());
        }
        switch (std::uniform_int_distribution<>(0, 2)(gen)) {
          case 0: {
            infoz.push_back(sampler.Register());
            break;
          }
          case 1: {
            size_t p =
                std::uniform_int_distribution<>(0, infoz.size() - 1)(gen);
            HashtablezInfo* info = infoz[p];
            infoz[p] = infoz.back();
            infoz.pop_back();
            sampler.Unregister(info);
            break;
          }
          case 2: {
            absl::Duration oldest = absl::ZeroDuration();
            sampler.Iterate([&](const HashtablezInfo& info) {
              oldest = std::max(oldest, absl::Now() - info.create_time);
            });
            ASSERT_GE(oldest, absl::ZeroDuration());
            break;
          }
        }
      }
    });
  }
  // The threads will hammer away.  Give it a little bit of time for tsan to
  // spot errors.
  absl::SleepFor(absl::Seconds(3));
  stop.Notify();
}

TEST(HashtablezSamplerTest, Callback) {
  HashtablezSampler sampler;

  auto* info1 = Register(&sampler, 1);
  auto* info2 = Register(&sampler, 2);

  static const HashtablezInfo* expected;

  auto callback = [](const HashtablezInfo& info) {
    // We can't use `info` outside of this callback because the object will be
    // disposed as soon as we return from here.
    EXPECT_EQ(&info, expected);
  };

  // Set the callback.
  EXPECT_EQ(sampler.SetDisposeCallback(callback), nullptr);
  expected = info1;
  sampler.Unregister(info1);

  // Unset the callback.
  EXPECT_EQ(callback, sampler.SetDisposeCallback(nullptr));
  expected = nullptr;  // no more calls.
  sampler.Unregister(info2);
}

}  // namespace
}  // namespace container_internal
ABSL_NAMESPACE_END
}  // namespace absl
