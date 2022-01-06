//
// Copyright 2019 The Abseil Authors.
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
#include BOSS_ABSEILCPP_U_absl__flags__usage_h //original-code:"absl/flags/usage.h"

#include <stdlib.h>

#include <string>

#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_ABSEILCPP_U_absl__base__config_h //original-code:"absl/base/config.h"
#include BOSS_ABSEILCPP_U_absl__base__const_init_h //original-code:"absl/base/const_init.h"
#include BOSS_ABSEILCPP_U_absl__base__thread_annotations_h //original-code:"absl/base/thread_annotations.h"
#include BOSS_ABSEILCPP_U_absl__flags__internal__usage_h //original-code:"absl/flags/internal/usage.h"
#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__mutex_h //original-code:"absl/synchronization/mutex.h"

namespace absl {
ABSL_NAMESPACE_BEGIN
namespace flags_internal {
namespace {
ABSL_CONST_INIT absl::Mutex usage_message_guard(absl::kConstInit);
ABSL_CONST_INIT std::string* program_usage_message
    ABSL_GUARDED_BY(usage_message_guard) = nullptr;
}  // namespace
}  // namespace flags_internal

// --------------------------------------------------------------------
// Sets the "usage" message to be used by help reporting routines.
void SetProgramUsageMessage(absl::string_view new_usage_message) {
  absl::MutexLock l(&flags_internal::usage_message_guard);

  if (flags_internal::program_usage_message != nullptr) {
    ABSL_INTERNAL_LOG(FATAL, "SetProgramUsageMessage() called twice.");
    std::exit(1);
  }

  flags_internal::program_usage_message = new std::string(new_usage_message);
}

// --------------------------------------------------------------------
// Returns the usage message set by SetProgramUsageMessage().
// Note: We able to return string_view here only because calling
// SetProgramUsageMessage twice is prohibited.
absl::string_view ProgramUsageMessage() {
  absl::MutexLock l(&flags_internal::usage_message_guard);

  return flags_internal::program_usage_message != nullptr
             ? absl::string_view(*flags_internal::program_usage_message)
             : "Warning: SetProgramUsageMessage() never called";
}

ABSL_NAMESPACE_END
}  // namespace absl
