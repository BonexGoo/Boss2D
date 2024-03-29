/*
 *  Copyright 2019 The Chromium Authors. All rights reserved.
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_PUBLIC_TYPES_H_
#define NET_DCSCTP_PUBLIC_TYPES_H_

#include <cstdint>
#include <limits>

#include BOSS_WEBRTC_U_net__dcsctp__public__strong_alias_h //original-code:"net/dcsctp/public/strong_alias.h"

namespace dcsctp {

// Stream Identifier
using StreamID = StrongAlias<class StreamIDTag, uint16_t>;

// Payload Protocol Identifier (PPID)
using PPID = StrongAlias<class PPIDTag, uint32_t>;

// Timeout Identifier
using TimeoutID = StrongAlias<class TimeoutTag, uint64_t>;

// Indicates if a message is allowed to be received out-of-order compared to
// other messages on the same stream.
using IsUnordered = StrongAlias<class IsUnorderedTag, bool>;

// Duration, as milliseconds. Overflows after 24 days.
class DurationMs : public StrongAlias<class DurationMsTag, int32_t> {
 public:
  constexpr explicit DurationMs(const UnderlyingType& v)
      : StrongAlias<class DurationMsTag, int32_t>(v) {}

  // Convenience methods for working with time.
  constexpr DurationMs& operator+=(DurationMs d) {
    value_ += d.value_;
    return *this;
  }
  constexpr DurationMs& operator-=(DurationMs d) {
    value_ -= d.value_;
    return *this;
  }
  template <typename T>
  constexpr DurationMs& operator*=(T factor) {
    value_ *= factor;
    return *this;
  }
};

constexpr inline DurationMs operator+(DurationMs lhs, DurationMs rhs) {
  return lhs += rhs;
}
constexpr inline DurationMs operator-(DurationMs lhs, DurationMs rhs) {
  return lhs -= rhs;
}
template <typename T>
constexpr inline DurationMs operator*(DurationMs lhs, T rhs) {
  return lhs *= rhs;
}
template <typename T>
constexpr inline DurationMs operator*(T lhs, DurationMs rhs) {
  return rhs *= lhs;
}
constexpr inline int32_t operator/(DurationMs lhs, DurationMs rhs) {
  return lhs.value() / rhs.value();
}

// Represents time, in milliseconds since a client-defined epoch.
class TimeMs : public StrongAlias<class TimeMsTag, int64_t> {
 public:
  constexpr explicit TimeMs(const UnderlyingType& v)
      : StrongAlias<class TimeMsTag, int64_t>(v) {}

  // Convenience methods for working with time.
  constexpr TimeMs& operator+=(DurationMs d) {
    value_ += *d;
    return *this;
  }
  constexpr TimeMs& operator-=(DurationMs d) {
    value_ -= *d;
    return *this;
  }

  static constexpr TimeMs InfiniteFuture() {
    return TimeMs(std::numeric_limits<int64_t>::max());
  }
};

constexpr inline TimeMs operator+(TimeMs lhs, DurationMs rhs) {
  return lhs += rhs;
}
constexpr inline TimeMs operator+(DurationMs lhs, TimeMs rhs) {
  return rhs += lhs;
}
constexpr inline TimeMs operator-(TimeMs lhs, DurationMs rhs) {
  return lhs -= rhs;
}
constexpr inline DurationMs operator-(TimeMs lhs, TimeMs rhs) {
  return DurationMs(*lhs - *rhs);
}

}  // namespace dcsctp

#endif  // NET_DCSCTP_PUBLIC_TYPES_H_
