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

#include BOSS_ABSEILCPP_U_absl__base__internal__sysinfo_h //original-code:"absl/base/internal/sysinfo.h"

#ifndef _WIN32
#include <sys/types.h>
#include <unistd.h>
#endif

#include <thread>  // NOLINT(build/c++11)
#include <unordered_set>
#include <vector>

#include "gtest/gtest.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__barrier_h //original-code:"absl/synchronization/barrier.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__mutex_h //original-code:"absl/synchronization/mutex.h"

namespace absl {
inline namespace lts_2018_06_20 {
namespace base_internal {
namespace {

TEST(SysinfoTest, NumCPUs) {
  EXPECT_NE(NumCPUs(), 0)
      << "NumCPUs() should not have the default value of 0";
}

TEST(SysinfoTest, NominalCPUFrequency) {
#if !(defined(__aarch64__) && defined(__linux__))
  EXPECT_GE(NominalCPUFrequency(), 1000.0)
      << "NominalCPUFrequency() did not return a reasonable value";
#else
  // TODO(absl-team): Aarch64 cannot read the CPU frequency from sysfs, so we
  // get back 1.0. Fix once the value is available.
  EXPECT_EQ(NominalCPUFrequency(), 1.0)
      << "CPU frequency detection was fixed! Please update unittest.";
#endif
}

TEST(SysinfoTest, GetTID) {
  EXPECT_EQ(GetTID(), GetTID());  // Basic compile and equality test.
#ifdef __native_client__
  // Native Client has a race condition bug that leads to memory
  // exaustion when repeatedly creating and joining threads.
  // https://bugs.chromium.org/p/nativeclient/issues/detail?id=1027
  return;
#endif
  // Test that TIDs are unique to each thread.
  // Uses a few loops to exercise implementations that reallocate IDs.
  for (int i = 0; i < 32; ++i) {
    constexpr int kNumThreads = 64;
    Barrier all_threads_done(kNumThreads);
    std::vector<std::thread> threads;

    Mutex mutex;
    std::unordered_set<pid_t> tids;

    for (int j = 0; j < kNumThreads; ++j) {
      threads.push_back(std::thread([&]() {
        pid_t id = GetTID();
        {
          MutexLock lock(&mutex);
          ASSERT_TRUE(tids.find(id) == tids.end());
          tids.insert(id);
        }
        // We can't simply join the threads here. The threads need to
        // be alive otherwise the TID might have been reallocated to
        // another live thread.
        all_threads_done.Block();
      }));
    }
    for (auto& thread : threads) {
      thread.join();
    }
  }
}

#ifdef __linux__
TEST(SysinfoTest, LinuxGetTID) {
  // On Linux, for the main thread, GetTID()==getpid() is guaranteed by the API.
  EXPECT_EQ(GetTID(), getpid());
}
#endif

}  // namespace
}  // namespace base_internal
}  // inline namespace lts_2018_06_20
}  // namespace absl
