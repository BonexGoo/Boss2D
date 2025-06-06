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

#include "benchmark/benchmark.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__thread_identity_h //original-code:"absl/base/internal/thread_identity.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__internal__create_thread_identity_h //original-code:"absl/synchronization/internal/create_thread_identity.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__internal__per_thread_sem_h //original-code:"absl/synchronization/internal/per_thread_sem.h"

namespace {

void BM_SafeCurrentThreadIdentity(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(
        absl::synchronization_internal::GetOrCreateCurrentThreadIdentity());
  }
}
BENCHMARK(BM_SafeCurrentThreadIdentity);

void BM_UnsafeCurrentThreadIdentity(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(
        absl::base_internal::CurrentThreadIdentityIfPresent());
  }
}
BENCHMARK(BM_UnsafeCurrentThreadIdentity);

}  // namespace
