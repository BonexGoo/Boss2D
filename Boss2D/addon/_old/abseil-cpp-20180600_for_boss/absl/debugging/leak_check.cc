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
// Wrappers around lsan_interface functions.
// When lsan is not linked in, these functions are not available,
// therefore Abseil code which depends on these functions is conditioned on the
// definition of LEAK_SANITIZER.
#include BOSS_ABSEILCPP_U_absl__debugging__leak_check_h //original-code:"absl/debugging/leak_check.h"

#ifndef LEAK_SANITIZER

namespace absl {
inline namespace lts_2018_06_20 {
bool HaveLeakSanitizer() { return false; }
void DoIgnoreLeak(const void*) { }
void RegisterLivePointers(const void*, size_t) { }
void UnRegisterLivePointers(const void*, size_t) { }
LeakCheckDisabler::LeakCheckDisabler() { }
LeakCheckDisabler::~LeakCheckDisabler() { }
}  // inline namespace lts_2018_06_20
}  // namespace absl

#else

#include <sanitizer/lsan_interface.h>

namespace absl {
inline namespace lts_2018_06_20 {
bool HaveLeakSanitizer() { return true; }
void DoIgnoreLeak(const void* ptr) { __lsan_ignore_object(ptr); }
void RegisterLivePointers(const void* ptr, size_t size) {
  __lsan_register_root_region(ptr, size);
}
void UnRegisterLivePointers(const void* ptr, size_t size) {
  __lsan_unregister_root_region(ptr, size);
}
LeakCheckDisabler::LeakCheckDisabler() { __lsan_disable(); }
LeakCheckDisabler::~LeakCheckDisabler() { __lsan_enable(); }
}  // inline namespace lts_2018_06_20
}  // namespace absl

#endif  // LEAK_SANITIZER
