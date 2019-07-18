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

#include BOSS_ABSEILCPP_U_absl__strings__match_h //original-code:"absl/strings/match.h"

#include BOSS_ABSEILCPP_U_absl__strings__internal__memutil_h //original-code:"absl/strings/internal/memutil.h"

namespace absl {
inline namespace lts_2018_06_20 {

namespace {
bool CaseEqual(absl::string_view piece1, absl::string_view piece2) {
  return (piece1.size() == piece2.size() &&
          0 == strings_internal::memcasecmp(piece1.data(), piece2.data(),
                                            piece1.size()));
  // memcasecmp uses ascii_tolower().
}
}  // namespace

bool StartsWithIgnoreCase(absl::string_view text, absl::string_view prefix) {
  return (text.size() >= prefix.size()) &&
         CaseEqual(text.substr(0, prefix.size()), prefix);
}

bool EndsWithIgnoreCase(absl::string_view text, absl::string_view suffix) {
  return (text.size() >= suffix.size()) &&
         CaseEqual(text.substr(text.size() - suffix.size()), suffix);
}

}  // inline namespace lts_2018_06_20
}  // namespace absl
