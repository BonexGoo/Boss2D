//
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
// -----------------------------------------------------------------------------
// File: escaping.h
// -----------------------------------------------------------------------------
//
// This header file contains std::string utilities involved in escaping and
// unescaping strings in various ways.
//

#ifndef ABSL_STRINGS_ESCAPING_H_
#define ABSL_STRINGS_ESCAPING_H_

#include <cstddef>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__base__macros_h //original-code:"absl/base/macros.h"
#include BOSS_ABSEILCPP_U_absl__strings__ascii_h //original-code:"absl/strings/ascii.h"
#include BOSS_ABSEILCPP_U_absl__strings__str_join_h //original-code:"absl/strings/str_join.h"
#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"

namespace absl {
inline namespace lts_2018_06_20 {

// CUnescape()
//
// Unescapes a `source` std::string and copies it into `dest`, rewriting C-style
// escape sequences (http://en.cppreference.com/w/cpp/language/escape) into
// their proper code point equivalents, returning `true` if successful.
//
// The following unescape sequences can be handled:
//
//   * ASCII escape sequences ('\n','\r','\\', etc.) to their ASCII equivalents
//   * Octal escape sequences ('\nnn') to byte nnn. The unescaped value must
//     resolve to a single byte or an error will occur. E.g. values greater than
//     0xff will produce an error.
//   * Hexadecimal escape sequences ('\xnn') to byte nn. While an arbitrary
//     number of following digits are allowed, the unescaped value must resolve
//     to a single byte or an error will occur. E.g. '\x0045' is equivalent to
//     '\x45', but '\x1234' will produce an error.
//   * Unicode escape sequences ('\unnnn' for exactly four hex digits or
//     '\Unnnnnnnn' for exactly eight hex digits, which will be encoded in
//     UTF-8. (E.g., `\u2019` unescapes to the three bytes 0xE2, 0x80, and
//     0x99).
//
//
// If any errors are encountered, this function returns `false` and stores the
// first encountered error in `error`. To disable error reporting, set `error`
// to `nullptr` or use the overload with no error reporting below.
//
// Example:
//
//   std::string s = "foo\\rbar\\nbaz\\t";
//   std::string unescaped_s;
//   if (!absl::CUnescape(s, &unescaped_s) {
//     ...
//   }
//   EXPECT_EQ(unescaped_s, "foo\rbar\nbaz\t");
bool CUnescape(absl::string_view source, std::string* dest, std::string* error);

// Overload of `CUnescape()` with no error reporting.
inline bool CUnescape(absl::string_view source, std::string* dest) {
  return CUnescape(source, dest, nullptr);
}

// CEscape()
//
// Escapes a 'src' std::string using C-style escapes sequences
// (http://en.cppreference.com/w/cpp/language/escape), escaping other
// non-printable/non-whitespace bytes as octal sequences (e.g. "\377").
//
// Example:
//
//   std::string s = "foo\rbar\tbaz\010\011\012\013\014\x0d\n";
//   std::string escaped_s = absl::CEscape(s);
//   EXPECT_EQ(escaped_s, "foo\\rbar\\tbaz\\010\\t\\n\\013\\014\\r\\n");
std::string CEscape(absl::string_view src);

// CHexEscape()
//
// Escapes a 'src' std::string using C-style escape sequences, escaping
// other non-printable/non-whitespace bytes as hexadecimal sequences (e.g.
// "\xFF").
//
// Example:
//
//   std::string s = "foo\rbar\tbaz\010\011\012\013\014\x0d\n";
//   std::string escaped_s = absl::CHexEscape(s);
//   EXPECT_EQ(escaped_s, "foo\\rbar\\tbaz\\x08\\t\\n\\x0b\\x0c\\r\\n");
std::string CHexEscape(absl::string_view src);

// Utf8SafeCEscape()
//
// Escapes a 'src' std::string using C-style escape sequences, escaping bytes as
// octal sequences, and passing through UTF-8 characters without conversion.
// I.e., when encountering any bytes with their high bit set, this function
// will not escape those values, whether or not they are valid UTF-8.
std::string Utf8SafeCEscape(absl::string_view src);

// Utf8SafeCHexEscape()
//
// Escapes a 'src' std::string using C-style escape sequences, escaping bytes as
// hexadecimal sequences, and passing through UTF-8 characters without
// conversion.
std::string Utf8SafeCHexEscape(absl::string_view src);

// Base64Unescape()
//
// Converts a `src` std::string encoded in Base64 to its binary equivalent, writing
// it to a `dest` buffer, returning `true` on success. If `src` contains invalid
// characters, `dest` is cleared and returns `false`.
bool Base64Unescape(absl::string_view src, std::string* dest);

// WebSafeBase64Unescape(absl::string_view, std::string*)
//
// Converts a `src` std::string encoded in Base64 to its binary equivalent, writing
// it to a `dest` buffer, but using '-' instead of '+', and '_' instead of '/'.
// If `src` contains invalid characters, `dest` is cleared and returns `false`.
bool WebSafeBase64Unescape(absl::string_view src, std::string* dest);

// Base64Escape()
//
// Encodes a `src` std::string into a `dest` buffer using base64 encoding, with
// padding characters. This function conforms with RFC 4648 section 4 (base64).
void Base64Escape(absl::string_view src, std::string* dest);

// WebSafeBase64Escape()
//
// Encodes a `src` std::string into a `dest` buffer using uses '-' instead of '+' and
// '_' instead of '/', and without padding. This function conforms with RFC 4648
// section 5 (base64url).
void WebSafeBase64Escape(absl::string_view src, std::string* dest);

// HexStringToBytes()
//
// Converts an ASCII hex std::string into bytes, returning binary data of length
// `from.size()/2`.
std::string HexStringToBytes(absl::string_view from);

// BytesToHexString()
//
// Converts binary data into an ASCII text std::string, returning a std::string of size
// `2*from.size()`.
std::string BytesToHexString(absl::string_view from);

}  // inline namespace lts_2018_06_20
}  // namespace absl

#endif  // ABSL_STRINGS_ESCAPING_H_
