/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"

namespace rtc {

SimpleStringBuilder::SimpleStringBuilder(rtc::ArrayView<char> buffer)
    : buffer_(buffer) {
  buffer_[0] = '\0';
  RTC_DCHECK(IsConsistent());
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(const char* str) {
  return Append(str);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(char ch) {
  return Append(&ch, 1);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(const std::string& str) {
  return Append(str.c_str(), str.length());
}

// Numeric conversion routines.
//
// We use std::[v]snprintf instead of std::to_string because:
// * std::to_string relies on the current locale for formatting purposes,
//   and therefore concurrent calls to std::to_string from multiple threads
//   may result in partial serialization of calls
// * snprintf allows us to print the number directly into our buffer.
// * avoid allocating a std::string (potential heap alloc).
// TODO(tommi): Switch to std::to_chars in C++17.

SimpleStringBuilder& SimpleStringBuilder::operator<<(int i) {
  return AppendFormat("%d", i);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(unsigned i) {
  return AppendFormat("%u", i);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(long i) {  // NOLINT
  return AppendFormat("%ld", i);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(long long i) {  // NOLINT
  return AppendFormat("%lld", i);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(
    unsigned long i) {  // NOLINT
  return AppendFormat("%lu", i);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(
    unsigned long long i) {  // NOLINT
  return AppendFormat("%llu", i);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(float f) {
  return AppendFormat("%g", f);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(double f) {
  return AppendFormat("%g", f);
}

SimpleStringBuilder& SimpleStringBuilder::operator<<(long double f) {
  return AppendFormat("%Lg", f);
}

SimpleStringBuilder& SimpleStringBuilder::AppendFormat(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  const int len =
      std::vsnprintf(&buffer_[size_], buffer_.size() - size_, fmt, args);
  if (len >= 0) {
    const size_t chars_added = rtc::SafeMin(len, buffer_.size() - 1 - size_);
    size_ += chars_added;
    RTC_DCHECK_EQ(len, chars_added) << "Buffer size was insufficient";
  } else {
    // This should never happen, but we're paranoid, so re-write the
    // terminator in case vsnprintf() overwrote it.
    RTC_NOTREACHED();
    buffer_[size_] = '\0';
  }
  va_end(args);
  RTC_DCHECK(IsConsistent());
  return *this;
}

SimpleStringBuilder& SimpleStringBuilder::Append(const char* str,
                                                 size_t length) {
  const size_t chars_added =
      rtc::strcpyn(&buffer_[size_], buffer_.size() - size_, str, length);
  size_ += chars_added;
  RTC_DCHECK_EQ(chars_added, length == SIZE_UNKNOWN ? std::strlen(str) : length)
      << "Buffer size was insufficient";
  RTC_DCHECK(IsConsistent());
  return *this;
}

}  // namespace rtc
