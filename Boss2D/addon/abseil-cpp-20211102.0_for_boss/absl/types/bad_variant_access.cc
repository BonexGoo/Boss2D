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

#include BOSS_ABSEILCPP_U_absl__types__bad_variant_access_h //original-code:"absl/types/bad_variant_access.h"

#ifndef ABSL_USES_STD_VARIANT

#include <cstdlib>
#include <stdexcept>

#include BOSS_ABSEILCPP_U_absl__base__config_h //original-code:"absl/base/config.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__raw_logging_h //original-code:"absl/base/internal/raw_logging.h"

namespace absl {
ABSL_NAMESPACE_BEGIN

//////////////////////////
// [variant.bad.access] //
//////////////////////////

bad_variant_access::~bad_variant_access() = default;

const char* bad_variant_access::what() const noexcept {
  return "Bad variant access";
}

namespace variant_internal {

void ThrowBadVariantAccess() {
#ifdef ABSL_HAVE_EXCEPTIONS
  throw bad_variant_access();
#else
  ABSL_RAW_LOG(FATAL, "Bad variant access");
  abort();  // TODO(calabrese) Remove once RAW_LOG FATAL is noreturn.
#endif
}

void Rethrow() {
#ifdef ABSL_HAVE_EXCEPTIONS
  throw;
#else
  ABSL_RAW_LOG(FATAL,
               "Internal error in absl::variant implementation. Attempted to "
               "rethrow an exception when building with exceptions disabled.");
  abort();  // TODO(calabrese) Remove once RAW_LOG FATAL is noreturn.
#endif
}

}  // namespace variant_internal
ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_USES_STD_VARIANT
