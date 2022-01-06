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

#include BOSS_ABSEILCPP_U_absl__flags__internal__program_name_h //original-code:"absl/flags/internal/program_name.h"

#include <string>

#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_ABSEILCPP_U_absl__base__config_h //original-code:"absl/base/config.h"
#include BOSS_ABSEILCPP_U_absl__base__const_init_h //original-code:"absl/base/const_init.h"
#include BOSS_ABSEILCPP_U_absl__base__thread_annotations_h //original-code:"absl/base/thread_annotations.h"
#include BOSS_ABSEILCPP_U_absl__flags__internal__path_util_h //original-code:"absl/flags/internal/path_util.h"
#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__synchronization__mutex_h //original-code:"absl/synchronization/mutex.h"

namespace absl {
ABSL_NAMESPACE_BEGIN
namespace flags_internal {

ABSL_CONST_INIT static absl::Mutex program_name_guard(absl::kConstInit);
ABSL_CONST_INIT static std::string* program_name
    ABSL_GUARDED_BY(program_name_guard) = nullptr;

std::string ProgramInvocationName() {
  absl::MutexLock l(&program_name_guard);

  return program_name ? *program_name : "UNKNOWN";
}

std::string ShortProgramInvocationName() {
  absl::MutexLock l(&program_name_guard);

  return program_name ? std::string(flags_internal::Basename(*program_name))
                      : "UNKNOWN";
}

void SetProgramInvocationName(absl::string_view prog_name_str) {
  absl::MutexLock l(&program_name_guard);

  if (!program_name)
    program_name = new std::string(prog_name_str);
  else
    program_name->assign(prog_name_str.data(), prog_name_str.size());
}

}  // namespace flags_internal
ABSL_NAMESPACE_END
}  // namespace absl
