// Copyright 2020 The Abseil Authors.
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

#ifndef ABSL_STRINGS_INTERNAL_ESCAPING_H_
#define ABSL_STRINGS_INTERNAL_ESCAPING_H_

#include <cassert>

#include BOSS_ABSEILCPP_U_absl__strings__internal__resize_uninitialized_h //original-code:"absl/strings/internal/resize_uninitialized.h"

namespace absl {
ABSL_NAMESPACE_BEGIN
namespace strings_internal {

ABSL_CONST_INIT extern const char kBase64Chars[];

// Calculates how long a string will be when it is base64 encoded given its
// length and whether or not the result should be padded.
size_t CalculateBase64EscapedLenInternal(size_t input_len, bool do_padding);

// Base64-encodes `src` using the alphabet provided in `base64` and writes the
// result to `dest`. If `do_padding` is true, `dest` is padded with '=' chars
// until its length is a multiple of 3. Returns the length of `dest`.
size_t Base64EscapeInternal(const unsigned char* src, size_t szsrc, char* dest,
                            size_t szdest, const char* base64, bool do_padding);

// Base64-encodes `src` using the alphabet provided in `base64` and writes the
// result to `dest`. If `do_padding` is true, `dest` is padded with '=' chars
// until its length is a multiple of 3.
template <typename String>
void Base64EscapeInternal(const unsigned char* src, size_t szsrc, String* dest,
                          bool do_padding, const char* base64_chars) {
  const size_t calc_escaped_size =
      CalculateBase64EscapedLenInternal(szsrc, do_padding);
  STLStringResizeUninitialized(dest, calc_escaped_size);

  const size_t escaped_len = Base64EscapeInternal(
      src, szsrc, &(*dest)[0], dest->size(), base64_chars, do_padding);
  assert(calc_escaped_size == escaped_len);
  dest->erase(escaped_len);
}

}  // namespace strings_internal
ABSL_NAMESPACE_END
}  // namespace absl

#endif  // ABSL_STRINGS_INTERNAL_ESCAPING_H_
