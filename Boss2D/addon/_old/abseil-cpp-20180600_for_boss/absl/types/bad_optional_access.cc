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

#include BOSS_ABSEILCPP_U_absl__types__bad_optional_access_h //original-code:"absl/types/bad_optional_access.h"

#include <cstdlib>

#include BOSS_ABSEILCPP_U_absl__base__config_h //original-code:"absl/base/config.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__raw_logging_h //original-code:"absl/base/internal/raw_logging.h"

namespace absl {
inline namespace lts_2018_06_20 {

bad_optional_access::~bad_optional_access() = default;

const char* bad_optional_access::what() const noexcept {
  return "optional has no value";
}

namespace optional_internal {

void throw_bad_optional_access() {
#ifdef ABSL_HAVE_EXCEPTIONS
  throw bad_optional_access();
#else
  ABSL_RAW_LOG(FATAL, "Bad optional access");
  abort();
#endif
}

}  // namespace optional_internal
}  // inline namespace lts_2018_06_20
}  // namespace absl
