/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_STRING_ENCODE_H_
#define RTC_BASE_STRING_ENCODE_H_

#include <stddef.h>

#include <string>
#include <type_traits>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__string_to_number_h //original-code:"rtc_base/string_to_number.h"

namespace rtc {

//////////////////////////////////////////////////////////////////////
// String Encoding Utilities
//////////////////////////////////////////////////////////////////////

std::string hex_encode(const std::string& str);
std::string hex_encode(const char* source, size_t srclen);
std::string hex_encode_with_delimiter(const char* source,
                                      size_t srclen,
                                      char delimiter);

// hex_decode converts ascii hex to binary.
size_t hex_decode(char* buffer,
                  size_t buflen,
                  const char* source,
                  size_t srclen);

// hex_decode, assuming that there is a delimiter between every byte
// pair.
// |delimiter| == 0 means no delimiter
// If the buffer is too short or the data is invalid, we return 0.
size_t hex_decode_with_delimiter(char* buffer,
                                 size_t buflen,
                                 const char* source,
                                 size_t srclen,
                                 char delimiter);

// Helper functions for hex_decode.
size_t hex_decode(char* buffer, size_t buflen, const std::string& source);
size_t hex_decode_with_delimiter(char* buffer,
                                 size_t buflen,
                                 const std::string& source,
                                 char delimiter);

// Joins the source vector of strings into a single string, with each
// field in source being separated by delimiter. No trailing delimiter is added.
std::string join(const std::vector<std::string>& source, char delimiter);

// Splits the source string into multiple fields separated by delimiter,
// with duplicates of delimiter creating empty fields.
size_t split(const std::string& source,
             char delimiter,
             std::vector<std::string>* fields);

// Splits the source string into multiple fields separated by delimiter,
// with duplicates of delimiter ignored.  Trailing delimiter ignored.
size_t tokenize(const std::string& source,
                char delimiter,
                std::vector<std::string>* fields);

// Tokenize, including the empty tokens.
size_t tokenize_with_empty_tokens(const std::string& source,
                                  char delimiter,
                                  std::vector<std::string>* fields);

// Tokenize and append the tokens to fields. Return the new size of fields.
size_t tokenize_append(const std::string& source,
                       char delimiter,
                       std::vector<std::string>* fields);

// Splits the source string into multiple fields separated by delimiter, with
// duplicates of delimiter ignored. Trailing delimiter ignored. A substring in
// between the start_mark and the end_mark is treated as a single field. Return
// the size of fields. For example, if source is "filename
// \"/Library/Application Support/media content.txt\"", delimiter is ' ', and
// the start_mark and end_mark are '"', this method returns two fields:
// "filename" and "/Library/Application Support/media content.txt".
size_t tokenize(const std::string& source,
                char delimiter,
                char start_mark,
                char end_mark,
                std::vector<std::string>* fields);

// Extract the first token from source as separated by delimiter, with
// duplicates of delimiter ignored. Return false if the delimiter could not be
// found, otherwise return true.
bool tokenize_first(const std::string& source,
                    const char delimiter,
                    std::string* token,
                    std::string* rest);

// Convert arbitrary values to/from a string.
// TODO(jonasolsson): Remove these when absl::StrCat becomes available.
std::string ToString(bool b);

std::string ToString(const char* s);
std::string ToString(std::string t);

std::string ToString(short s);
std::string ToString(unsigned short s);
std::string ToString(int s);
std::string ToString(unsigned int s);
std::string ToString(long int s);
std::string ToString(unsigned long int s);
std::string ToString(long long int s);
std::string ToString(unsigned long long int s);

std::string ToString(double t);
std::string ToString(long double t);

std::string ToString(const void* p);

template <typename T,
          typename std::enable_if<std::is_arithmetic<T>::value &&
                                      !std::is_same<T, bool>::value,
                                  int>::type = 0>
static bool FromString(const std::string& s, T* t) {
  RTC_DCHECK(t);
  absl::optional<T> result = StringToNumber<T>(s);

  if (result)
    *t = *result;

  return result.has_value();
}

bool FromString(const std::string& s, bool* b);

template <typename T>
static inline T FromString(const std::string& str) {
  T val;
  FromString(str, &val);
  return val;
}

//////////////////////////////////////////////////////////////////////

}  // namespace rtc

#endif  // RTC_BASE_STRING_ENCODE_H__
