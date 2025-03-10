/*
 *  Copyright 2017 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_STRING_TO_NUMBER_H_
#define RTC_BASE_STRING_TO_NUMBER_H_

#include <limits>
#include <string>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"

namespace rtc {

// This file declares a family of functions to parse integers from strings.
// The standard C library functions either fail to indicate errors (atoi, etc.)
// or are a hassle to work with (strtol, sscanf, etc.). The standard C++ library
// functions (std::stoi, etc.) indicate errors by throwing exceptions, which
// are disabled in WebRTC.
//
// Integers are parsed using one of the following functions:
//   absl::optional<int-type> StringToNumber(const char* str, int base = 10);
//   absl::optional<int-type> StringToNumber(const std::string& str,
//                                          int base = 10);
//
// These functions parse a value from the beginning of a string into one of the
// fundamental integer types, or returns an empty Optional if parsing
// failed. Values outside of the range supported by the type will be
// rejected. The strings must begin with a digit or a minus sign. No leading
// space nor trailing contents are allowed.
// By setting base to 0, one of octal, decimal or hexadecimal will be
// detected from the string's prefix (0, nothing or 0x, respectively).
// If non-zero, base can be set to a value between 2 and 36 inclusively.
//
// If desired, this interface could be extended with support for floating-point
// types.

namespace string_to_number_internal {
// These must be (unsigned) long long, to match the signature of strto(u)ll.
using unsigned_type = unsigned long long;  // NOLINT(runtime/int)
using signed_type = long long;             // NOLINT(runtime/int)

absl::optional<signed_type> ParseSigned(const char* str, int base);
absl::optional<unsigned_type> ParseUnsigned(const char* str, int base);
}  // namespace string_to_number_internal

template <typename T>
typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value,
                        absl::optional<T>>::type
StringToNumber(const char* str, int base = 10) {
  using string_to_number_internal::signed_type;
  static_assert(
      std::numeric_limits<T>::max() <=
              std::numeric_limits<signed_type>::max() &&
          std::numeric_limits<T>::lowest() >=
              std::numeric_limits<signed_type>::lowest(),
      "StringToNumber only supports signed integers as large as long long int");
  absl::optional<signed_type> value =
      string_to_number_internal::ParseSigned(str, base);
  if (value && *value >= std::numeric_limits<T>::lowest() &&
      *value <= std::numeric_limits<T>::max()) {
    return static_cast<T>(*value);
  }
  return absl::nullopt;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value &&
                            std::is_unsigned<T>::value,
                        absl::optional<T>>::type
StringToNumber(const char* str, int base = 10) {
  using string_to_number_internal::unsigned_type;
  static_assert(std::numeric_limits<T>::max() <=
                    std::numeric_limits<unsigned_type>::max(),
                "StringToNumber only supports unsigned integers as large as "
                "unsigned long long int");
  absl::optional<unsigned_type> value =
      string_to_number_internal::ParseUnsigned(str, base);
  if (value && *value <= std::numeric_limits<T>::max()) {
    return static_cast<T>(*value);
  }
  return absl::nullopt;
}

// The std::string overloads only exists if there is a matching const char*
// version.
template <typename T>
auto StringToNumber(const std::string& str, int base = 10)
    -> decltype(StringToNumber<T>(str.c_str(), base)) {
  return StringToNumber<T>(str.c_str(), base);
}

}  // namespace rtc

#endif  // RTC_BASE_STRING_TO_NUMBER_H_
