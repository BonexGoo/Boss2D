// Copyright 2018 The Abseil Authors.
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

#include <cstdint>

#include BOSS_ABSEILCPP_U_absl__base__internal__raw_logging_h //original-code:"absl/base/internal/raw_logging.h"

namespace absl {
inline namespace lts_2018_06_20 {

namespace debugging_internal {

int InstallSymbolDecorator(SymbolDecorator, void*) { return -1; }
bool RemoveSymbolDecorator(int) { return false; }
bool RemoveAllSymbolDecorators(void) { return false; }
bool RegisterFileMappingHint(const void *, const void *, uint64_t, const char *) {
  return false;
}

}  // namespace debugging_internal

void InitializeSymbolizer(const char*) {}
bool Symbolize(const void *, char *, int) { return false; }

}  // inline namespace lts_2018_06_20
}  // namespace absl
