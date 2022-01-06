//
//  Copyright 2019 The Abseil Authors.
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

#include BOSS_ABSEILCPP_U_absl__flags__flag_h //original-code:"absl/flags/flag.h"

#include BOSS_ABSEILCPP_U_absl__base__config_h //original-code:"absl/base/config.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

// This global mutex protects on-demand construction of flag objects in MSVC
// builds.
#if defined(_MSC_VER) && !defined(__clang__)

namespace flags_internal {

ABSL_CONST_INIT static absl::Mutex construction_guard(absl::kConstInit);

absl::Mutex* GetGlobalConstructionGuard() { return &construction_guard; }

}  // namespace flags_internal

#endif

ABSL_NAMESPACE_END
}  // namespace absl
