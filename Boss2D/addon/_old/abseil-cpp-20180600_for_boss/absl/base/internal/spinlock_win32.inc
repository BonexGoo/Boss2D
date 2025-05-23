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
//
// This file is a Win32-specific part of spinlock_wait.cc

#include <windows.h>
#include <atomic>
#include BOSS_ABSEILCPP_U_absl__base__internal__scheduling_mode_h //original-code:"absl/base/internal/scheduling_mode.h"

extern "C" {

void AbslInternalSpinLockDelay(std::atomic<uint32_t>* /* lock_word */,
                               uint32_t /* value */, int loop,
                               absl::base_internal::SchedulingMode /* mode */) {
  if (loop == 0) {
  } else if (loop == 1) {
    Sleep(0);
  } else {
    Sleep(absl::base_internal::SpinLockSuggestedDelayNS(loop) / 1000000);
  }
}

void AbslInternalSpinLockWake(std::atomic<uint32_t>* /* lock_word */,
                              bool /* all */) {}

}  // extern "C"
