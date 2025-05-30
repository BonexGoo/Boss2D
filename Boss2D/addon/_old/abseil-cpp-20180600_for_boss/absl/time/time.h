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
// File: time.h
// -----------------------------------------------------------------------------
//
// This header file defines abstractions for computing with absolute points
// in time, durations of time, and formatting and parsing time within a given
// time zone. The following abstractions are defined:
//
//  * `absl::Time` defines an absolute, specific instance in time
//  * `absl::Duration` defines a signed, fixed-length span of time
//  * `absl::TimeZone` defines geopolitical time zone regions (as collected
//     within the IANA Time Zone database (https://www.iana.org/time-zones)).
//
// Example:
//
//   absl::TimeZone nyc;
//
//   // LoadTimeZone may fail so it's always better to check for success.
//   if (!absl::LoadTimeZone("America/New_York", &nyc)) {
//      // handle error case
//   }
//
//   // My flight leaves NYC on Jan 2, 2017 at 03:04:05
//   absl::Time takeoff = absl::FromDateTime(2017, 1, 2, 3, 4, 5, nyc);
//   absl::Duration flight_duration = absl::Hours(21) + absl::Minutes(35);
//   absl::Time landing = takeoff + flight_duration;
//
//   absl::TimeZone syd;
//   if (!absl::LoadTimeZone("Australia/Sydney", &syd)) {
//      // handle error case
//   }
//   std::string s = absl::FormatTime(
//       "My flight will land in Sydney on %Y-%m-%d at %H:%M:%S",
//       landing, syd);
//
#ifndef ABSL_TIME_TIME_H_
#define ABSL_TIME_TIME_H_

#if !defined(_WIN32)
#include <sys/time.h>
#else
#include <winsock2.h>
#endif
#include <chrono>  // NOLINT(build/c++11)
#include <cstdint>
#include <ctime>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__base__port_h //original-code:"absl/base/port.h"  // Needed for string vs std::string
#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__time__internal__cctz__include__cctz__time_zone_h //original-code:"absl/time/internal/cctz/include/cctz/time_zone.h"

namespace absl {
inline namespace lts_2018_06_20 {

class Duration;  // Defined below
class Time;      // Defined below
class TimeZone;  // Defined below

namespace time_internal {
int64_t IDivDuration(bool satq, Duration num, Duration den, Duration* rem);
constexpr Time FromUnixDuration(Duration d);
constexpr Duration ToUnixDuration(Time t);
constexpr int64_t GetRepHi(Duration d);
constexpr uint32_t GetRepLo(Duration d);
constexpr Duration MakeDuration(int64_t hi, uint32_t lo);
constexpr Duration MakeDuration(int64_t hi, int64_t lo);
constexpr int64_t kTicksPerNanosecond = 4;
constexpr int64_t kTicksPerSecond = 1000 * 1000 * 1000 * kTicksPerNanosecond;
template <std::intmax_t N>
constexpr Duration FromInt64(int64_t v, std::ratio<1, N>);
constexpr Duration FromInt64(int64_t v, std::ratio<60>);
constexpr Duration FromInt64(int64_t v, std::ratio<3600>);
template <typename T>
using EnableIfIntegral = typename std::enable_if<
    std::is_integral<T>::value || std::is_enum<T>::value, int>::type;
template <typename T>
using EnableIfFloat =
    typename std::enable_if<std::is_floating_point<T>::value, int>::type;
}  // namespace time_internal

// Duration
//
// The `absl::Duration` class represents a signed, fixed-length span of time.
// A `Duration` is generated using a unit-specific factory function, or is
// the result of subtracting one `absl::Time` from another. Durations behave
// like unit-safe integers and they support all the natural integer-like
// arithmetic operations. Arithmetic overflows and saturates at +/- infinity.
// `Duration` should be passed by value rather than const reference.
//
// Factory functions `Nanoseconds()`, `Microseconds()`, `Milliseconds()`,
// `Seconds()`, `Minutes()`, `Hours()` and `InfiniteDuration()` allow for
// creation of constexpr `Duration` values
//
// Examples:
//
//   constexpr absl::Duration ten_ns = absl::Nanoseconds(10);
//   constexpr absl::Duration min = absl::Minutes(1);
//   constexpr absl::Duration hour = absl::Hours(1);
//   absl::Duration dur = 60 * min;  // dur == hour
//   absl::Duration half_sec = absl::Milliseconds(500);
//   absl::Duration quarter_sec = 0.25 * absl::Seconds(1);
//
// `Duration` values can be easily converted to an integral number of units
// using the division operator.
//
// Example:
//
//   constexpr absl::Duration dur = absl::Milliseconds(1500);
//   int64_t ns = dur / absl::Nanoseconds(1);   // ns == 1500000000
//   int64_t ms = dur / absl::Milliseconds(1);  // ms == 1500
//   int64_t sec = dur / absl::Seconds(1);    // sec == 1 (subseconds truncated)
//   int64_t min = dur / absl::Minutes(1);    // min == 0
//
// See the `IDivDuration()` and `FDivDuration()` functions below for details on
// how to access the fractional parts of the quotient.
//
// Alternatively, conversions can be performed using helpers such as
// `ToInt64Microseconds()` and `ToDoubleSeconds()`.
class Duration {
 public:
  // Value semantics.
  constexpr Duration() : rep_hi_(0), rep_lo_(0) {}  // zero-length duration

  // Compound assignment operators.
  Duration& operator+=(Duration d);
  Duration& operator-=(Duration d);
  Duration& operator*=(int64_t r);
  Duration& operator*=(double r);
  Duration& operator/=(int64_t r);
  Duration& operator/=(double r);
  Duration& operator%=(Duration rhs);

  // Overloads that forward to either the int64_t or double overloads above.
  template <typename T>
  Duration& operator*=(T r) {
    int64_t x = r;
    return *this *= x;
  }
  template <typename T>
  Duration& operator/=(T r) {
    int64_t x = r;
    return *this /= x;
  }
  Duration& operator*=(float r) { return *this *= static_cast<double>(r); }
  Duration& operator/=(float r) { return *this /= static_cast<double>(r); }

 private:
  friend constexpr int64_t time_internal::GetRepHi(Duration d);
  friend constexpr uint32_t time_internal::GetRepLo(Duration d);
  friend constexpr Duration time_internal::MakeDuration(int64_t hi,
                                                        uint32_t lo);
  constexpr Duration(int64_t hi, uint32_t lo) : rep_hi_(hi), rep_lo_(lo) {}
  int64_t rep_hi_;
  uint32_t rep_lo_;
};

// Relational Operators
constexpr bool operator<(Duration lhs, Duration rhs);
constexpr bool operator>(Duration lhs, Duration rhs) { return rhs < lhs; }
constexpr bool operator>=(Duration lhs, Duration rhs) { return !(lhs < rhs); }
constexpr bool operator<=(Duration lhs, Duration rhs) { return !(rhs < lhs); }
constexpr bool operator==(Duration lhs, Duration rhs);
constexpr bool operator!=(Duration lhs, Duration rhs) { return !(lhs == rhs); }

// Additive Operators
constexpr Duration operator-(Duration d);
inline Duration operator+(Duration lhs, Duration rhs) { return lhs += rhs; }
inline Duration operator-(Duration lhs, Duration rhs) { return lhs -= rhs; }

// Multiplicative Operators
template <typename T>
Duration operator*(Duration lhs, T rhs) {
  return lhs *= rhs;
}
template <typename T>
Duration operator*(T lhs, Duration rhs) {
  return rhs *= lhs;
}
template <typename T>
Duration operator/(Duration lhs, T rhs) {
  return lhs /= rhs;
}
inline int64_t operator/(Duration lhs, Duration rhs) {
  return time_internal::IDivDuration(true, lhs, rhs,
                                     &lhs);  // trunc towards zero
}
inline Duration operator%(Duration lhs, Duration rhs) { return lhs %= rhs; }

// IDivDuration()
//
// Divides a numerator `Duration` by a denominator `Duration`, returning the
// quotient and remainder. The remainder always has the same sign as the
// numerator. The returned quotient and remainder respect the identity:
//
//   numerator = denominator * quotient + remainder
//
// Returned quotients are capped to the range of `int64_t`, with the difference
// spilling into the remainder to uphold the above identity. This means that the
// remainder returned could differ from the remainder returned by
// `Duration::operator%` for huge quotients.
//
// See also the notes on `InfiniteDuration()` below regarding the behavior of
// division involving zero and infinite durations.
//
// Example:
//
//   constexpr absl::Duration a =
//       absl::Seconds(std::numeric_limits<int64_t>::max());  // big
//   constexpr absl::Duration b = absl::Nanoseconds(1);       // small
//
//   absl::Duration rem = a % b;
//   // rem == absl::ZeroDuration()
//
//   // Here, q would overflow int64_t, so rem accounts for the difference.
//   int64_t q = absl::IDivDuration(a, b, &rem);
//   // q == std::numeric_limits<int64_t>::max(), rem == a - b * q
inline int64_t IDivDuration(Duration num, Duration den, Duration* rem) {
  return time_internal::IDivDuration(true, num, den,
                                     rem);  // trunc towards zero
}

// FDivDuration()
//
// Divides a `Duration` numerator into a fractional number of units of a
// `Duration` denominator.
//
// See also the notes on `InfiniteDuration()` below regarding the behavior of
// division involving zero and infinite durations.
//
// Example:
//
//   double d = absl::FDivDuration(absl::Milliseconds(1500), absl::Seconds(1));
//   // d == 1.5
double FDivDuration(Duration num, Duration den);

// ZeroDuration()
//
// Returns a zero-length duration. This function behaves just like the default
// constructor, but the name helps make the semantics clear at call sites.
constexpr Duration ZeroDuration() { return Duration(); }

// AbsDuration()
//
// Returns the absolute value of a duration.
inline Duration AbsDuration(Duration d) {
  return (d < ZeroDuration()) ? -d : d;
}

// Trunc()
//
// Truncates a duration (toward zero) to a multiple of a non-zero unit.
//
// Example:
//
//   absl::Duration d = absl::Nanoseconds(123456789);
//   absl::Duration a = absl::Trunc(d, absl::Microseconds(1));  // 123456us
Duration Trunc(Duration d, Duration unit);

// Floor()
//
// Floors a duration using the passed duration unit to its largest value not
// greater than the duration.
//
// Example:
//
//   absl::Duration d = absl::Nanoseconds(123456789);
//   absl::Duration b = absl::Floor(d, absl::Microseconds(1));  // 123456us
Duration Floor(Duration d, Duration unit);

// Ceil()
//
// Returns the ceiling of a duration using the passed duration unit to its
// smallest value not less than the duration.
//
// Example:
//
//   absl::Duration d = absl::Nanoseconds(123456789);
//   absl::Duration c = absl::Ceil(d, absl::Microseconds(1));   // 123457us
Duration Ceil(Duration d, Duration unit);

// Nanoseconds()
// Microseconds()
// Milliseconds()
// Seconds()
// Minutes()
// Hours()
//
// Factory functions for constructing `Duration` values from an integral number
// of the unit indicated by the factory function's name.
//
// Note: no "Days()" factory function exists because "a day" is ambiguous. Civil
// days are not always 24 hours long, and a 24-hour duration often does not
// correspond with a civil day. If a 24-hour duration is needed, use
// `absl::Hours(24)`.
//
//
// Example:
//
//   absl::Duration a = absl::Seconds(60);
//   absl::Duration b = absl::Minutes(1);  // b == a
constexpr Duration Nanoseconds(int64_t n);
constexpr Duration Microseconds(int64_t n);
constexpr Duration Milliseconds(int64_t n);
constexpr Duration Seconds(int64_t n);
constexpr Duration Minutes(int64_t n);
constexpr Duration Hours(int64_t n);

// Factory overloads for constructing `Duration` values from a floating-point
// number of the unit indicated by the factory function's name. These functions
// exist for convenience, but they are not as efficient as the integral
// factories, which should be preferred.
//
// Example:
//   auto a = absl::Seconds(1.5);        // OK
//   auto b = absl::Milliseconds(1500);  // BETTER
template <typename T, time_internal::EnableIfFloat<T> = 0>
Duration Nanoseconds(T n) {
  return n * Nanoseconds(1);
}
template <typename T, time_internal::EnableIfFloat<T> = 0>
Duration Microseconds(T n) {
  return n * Microseconds(1);
}
template <typename T, time_internal::EnableIfFloat<T> = 0>
Duration Milliseconds(T n) {
  return n * Milliseconds(1);
}
template <typename T, time_internal::EnableIfFloat<T> = 0>
Duration Seconds(T n) {
  return n * Seconds(1);
}
template <typename T, time_internal::EnableIfFloat<T> = 0>
Duration Minutes(T n) {
  return n * Minutes(1);
}
template <typename T, time_internal::EnableIfFloat<T> = 0>
Duration Hours(T n) {
  return n * Hours(1);
}

// ToInt64Nanoseconds()
// ToInt64Microseconds()
// ToInt64Milliseconds()
// ToInt64Seconds()
// ToInt64Minutes()
// ToInt64Hours()
//
// Helper functions that convert a Duration to an integral count of the
// indicated unit. These functions are shorthand for the `IDivDuration()`
// function above; see its documentation for details about overflow, etc.
//
// Example:
//
//   absl::Duration d = absl::Milliseconds(1500);
//   int64_t isec = absl::ToInt64Seconds(d);  // isec == 1
int64_t ToInt64Nanoseconds(Duration d);
int64_t ToInt64Microseconds(Duration d);
int64_t ToInt64Milliseconds(Duration d);
int64_t ToInt64Seconds(Duration d);
int64_t ToInt64Minutes(Duration d);
int64_t ToInt64Hours(Duration d);

// ToDoubleNanoSeconds()
// ToDoubleMicroseconds()
// ToDoubleMilliseconds()
// ToDoubleSeconds()
// ToDoubleMinutes()
// ToDoubleHours()
//
// Helper functions that convert a Duration to a floating point count of the
// indicated unit. These functions are shorthand for the `FDivDuration()`
// function above; see its documentation for details about overflow, etc.
//
// Example:
//
//   absl::Duration d = absl::Milliseconds(1500);
//   double dsec = absl::ToDoubleSeconds(d);  // dsec == 1.5
double ToDoubleNanoseconds(Duration d);
double ToDoubleMicroseconds(Duration d);
double ToDoubleMilliseconds(Duration d);
double ToDoubleSeconds(Duration d);
double ToDoubleMinutes(Duration d);
double ToDoubleHours(Duration d);

// FromChrono()
//
// Converts any of the pre-defined std::chrono durations to an absl::Duration.
//
// Example:
//
//   std::chrono::milliseconds ms(123);
//   absl::Duration d = absl::FromChrono(ms);
constexpr Duration FromChrono(const std::chrono::nanoseconds& d);
constexpr Duration FromChrono(const std::chrono::microseconds& d);
constexpr Duration FromChrono(const std::chrono::milliseconds& d);
constexpr Duration FromChrono(const std::chrono::seconds& d);
constexpr Duration FromChrono(const std::chrono::minutes& d);
constexpr Duration FromChrono(const std::chrono::hours& d);

// ToChronoNanoseconds()
// ToChronoMicroseconds()
// ToChronoMilliseconds()
// ToChronoSeconds()
// ToChronoMinutes()
// ToChronoHours()
//
// Converts an absl::Duration to any of the pre-defined std::chrono durations.
// If overflow would occur, the returned value will saturate at the min/max
// chrono duration value instead.
//
// Example:
//
//   absl::Duration d = absl::Microseconds(123);
//   auto x = absl::ToChronoMicroseconds(d);
//   auto y = absl::ToChronoNanoseconds(d);  // x == y
//   auto z = absl::ToChronoSeconds(absl::InfiniteDuration());
//   // z == std::chrono::seconds::max()
std::chrono::nanoseconds ToChronoNanoseconds(Duration d);
std::chrono::microseconds ToChronoMicroseconds(Duration d);
std::chrono::milliseconds ToChronoMilliseconds(Duration d);
std::chrono::seconds ToChronoSeconds(Duration d);
std::chrono::minutes ToChronoMinutes(Duration d);
std::chrono::hours ToChronoHours(Duration d);

// InfiniteDuration()
//
// Returns an infinite `Duration`.  To get a `Duration` representing negative
// infinity, use `-InfiniteDuration()`.
//
// Duration arithmetic overflows to +/- infinity and saturates. In general,
// arithmetic with `Duration` infinities is similar to IEEE 754 infinities
// except where IEEE 754 NaN would be involved, in which case +/-
// `InfiniteDuration()` is used in place of a "nan" Duration.
//
// Examples:
//
//   constexpr absl::Duration inf = absl::InfiniteDuration();
//   const absl::Duration d = ... any finite duration ...
//
//   inf == inf + inf
//   inf == inf + d
//   inf == inf - inf
//   -inf == d - inf
//
//   inf == d * 1e100
//   inf == inf / 2
//   0 == d / inf
//   INT64_MAX == inf / d
//
//   // Division by zero returns infinity, or INT64_MIN/MAX where appropriate.
//   inf == d / 0
//   INT64_MAX == d / absl::ZeroDuration()
//
// The examples involving the `/` operator above also apply to `IDivDuration()`
// and `FDivDuration()`.
constexpr Duration InfiniteDuration();

// FormatDuration()
//
// Returns a std::string representing the duration in the form "72h3m0.5s".
// Returns "inf" or "-inf" for +/- `InfiniteDuration()`.
std::string FormatDuration(Duration d);

// Output stream operator.
inline std::ostream& operator<<(std::ostream& os, Duration d) {
  return os << FormatDuration(d);
}

// ParseDuration()
//
// Parses a duration std::string consisting of a possibly signed sequence of
// decimal numbers, each with an optional fractional part and a unit
// suffix.  The valid suffixes are "ns", "us" "ms", "s", "m", and "h".
// Simple examples include "300ms", "-1.5h", and "2h45m".  Parses "0" as
// `ZeroDuration()`.  Parses "inf" and "-inf" as +/- `InfiniteDuration()`.
bool ParseDuration(const std::string& dur_string, Duration* d);

// ParseFlag()
//
bool ParseFlag(const std::string& text, Duration* dst, std::string* error);

// UnparseFlag()
//
std::string UnparseFlag(Duration d);

// Time
//
// An `absl::Time` represents a specific instant in time. Arithmetic operators
// are provided for naturally expressing time calculations. Instances are
// created using `absl::Now()` and the `absl::From*()` factory functions that
// accept the gamut of other time representations. Formatting and parsing
// functions are provided for conversion to and from strings.  `absl::Time`
// should be passed by value rather than const reference.
//
// `absl::Time` assumes there are 60 seconds in a minute, which means the
// underlying time scales must be "smeared" to eliminate leap seconds.
// See https://developers.google.com/time/smear.
//
// Even though `absl::Time` supports a wide range of timestamps, exercise
// caution when using values in the distant past. `absl::Time` uses the
// Proleptic Gregorian calendar, which extends the Gregorian calendar backward
// to dates before its introduction in 1582.
// See https://en.wikipedia.org/wiki/Proleptic_Gregorian_calendar
// for more information. Use the ICU calendar classes to convert a date in
// some other calendar (http://userguide.icu-project.org/datetime/calendar).
//
// Similarly, standardized time zones are a reasonably recent innovation, with
// the Greenwich prime meridian being established in 1884. The TZ database
// itself does not profess accurate offsets for timestamps prior to 1970. The
// breakdown of future timestamps is subject to the whim of regional
// governments.
//
// The `absl::Time` class represents an instant in time as a count of clock
// ticks of some granularity (resolution) from some starting point (epoch).
//
//
// `absl::Time` uses a resolution that is high enough to avoid loss in
// precision, and a range that is wide enough to avoid overflow, when
// converting between tick counts in most Google time scales (i.e., precision
// of at least one nanosecond, and range +/-100 billion years).  Conversions
// between the time scales are performed by truncating (towards negative
// infinity) to the nearest representable point.
//
// Examples:
//
//   absl::Time t1 = ...;
//   absl::Time t2 = t1 + absl::Minutes(2);
//   absl::Duration d = t2 - t1;  // == absl::Minutes(2)
//   absl::Time::Breakdown bd = t1.In(absl::LocalTimeZone());
//
class Time {
 public:
  // Value semantics.

  // Returns the Unix epoch.  However, those reading your code may not know
  // or expect the Unix epoch as the default value, so make your code more
  // readable by explicitly initializing all instances before use.
  //
  // Example:
  //   absl::Time t = absl::UnixEpoch();
  //   absl::Time t = absl::Now();
  //   absl::Time t = absl::TimeFromTimeval(tv);
  //   absl::Time t = absl::InfinitePast();
  constexpr Time() {}

  // Assignment operators.
  Time& operator+=(Duration d) {
    rep_ += d;
    return *this;
  }
  Time& operator-=(Duration d) {
    rep_ -= d;
    return *this;
  }

  // Time::Breakdown
  //
  // The calendar and wall-clock (aka "civil time") components of an
  // `absl::Time` in a certain `absl::TimeZone`. This struct is not
  // intended to represent an instant in time. So, rather than passing
  // a `Time::Breakdown` to a function, pass an `absl::Time` and an
  // `absl::TimeZone`.
  struct Breakdown {
    int64_t year;          // year (e.g., 2013)
    int month;           // month of year [1:12]
    int day;             // day of month [1:31]
    int hour;            // hour of day [0:23]
    int minute;          // minute of hour [0:59]
    int second;          // second of minute [0:59]
    Duration subsecond;  // [Seconds(0):Seconds(1)) if finite
    int weekday;         // 1==Mon, ..., 7=Sun
    int yearday;         // day of year [1:366]

    // Note: The following fields exist for backward compatibility
    // with older APIs.  Accessing these fields directly is a sign of
    // imprudent logic in the calling code.  Modern time-related code
    // should only access this data indirectly by way of FormatTime().
    // These fields are undefined for InfiniteFuture() and InfinitePast().
    int offset;             // seconds east of UTC
    bool is_dst;            // is offset non-standard?
    const char* zone_abbr;  // time-zone abbreviation (e.g., "PST")
  };

  // Time::In()
  //
  // Returns the breakdown of this instant in the given TimeZone.
  Breakdown In(TimeZone tz) const;

 private:
  friend constexpr Time time_internal::FromUnixDuration(Duration d);
  friend constexpr Duration time_internal::ToUnixDuration(Time t);
  friend constexpr bool operator<(Time lhs, Time rhs);
  friend constexpr bool operator==(Time lhs, Time rhs);
  friend Duration operator-(Time lhs, Time rhs);
  friend constexpr Time UniversalEpoch();
  friend constexpr Time InfiniteFuture();
  friend constexpr Time InfinitePast();
  constexpr explicit Time(Duration rep) : rep_(rep) {}
  Duration rep_;
};

// Relational Operators
constexpr bool operator<(Time lhs, Time rhs) { return lhs.rep_ < rhs.rep_; }
constexpr bool operator>(Time lhs, Time rhs) { return rhs < lhs; }
constexpr bool operator>=(Time lhs, Time rhs) { return !(lhs < rhs); }
constexpr bool operator<=(Time lhs, Time rhs) { return !(rhs < lhs); }
constexpr bool operator==(Time lhs, Time rhs) { return lhs.rep_ == rhs.rep_; }
constexpr bool operator!=(Time lhs, Time rhs) { return !(lhs == rhs); }

// Additive Operators
inline Time operator+(Time lhs, Duration rhs) { return lhs += rhs; }
inline Time operator+(Duration lhs, Time rhs) { return rhs += lhs; }
inline Time operator-(Time lhs, Duration rhs) { return lhs -= rhs; }
inline Duration operator-(Time lhs, Time rhs) { return lhs.rep_ - rhs.rep_; }

// UnixEpoch()
//
// Returns the `absl::Time` representing "1970-01-01 00:00:00.0 +0000".
constexpr Time UnixEpoch() { return Time(); }

// UniversalEpoch()
//
// Returns the `absl::Time` representing "0001-01-01 00:00:00.0 +0000", the
// epoch of the ICU Universal Time Scale.
constexpr Time UniversalEpoch() {
  // 719162 is the number of days from 0001-01-01 to 1970-01-01,
  // assuming the Gregorian calendar.
  return Time(time_internal::MakeDuration(-24 * 719162 * int64_t{3600}, 0U));
}

// InfiniteFuture()
//
// Returns an `absl::Time` that is infinitely far in the future.
constexpr Time InfiniteFuture() {
  return Time(
      time_internal::MakeDuration(std::numeric_limits<int64_t>::max(), ~0U));
}

// InfinitePast()
//
// Returns an `absl::Time` that is infinitely far in the past.
constexpr Time InfinitePast() {
  return Time(
      time_internal::MakeDuration(std::numeric_limits<int64_t>::min(), ~0U));
}

// TimeConversion
//
// An `absl::TimeConversion` represents the conversion of year, month, day,
// hour, minute, and second values (i.e., a civil time), in a particular
// `absl::TimeZone`, to a time instant (an absolute time), as returned by
// `absl::ConvertDateTime()`. (Subseconds must be handled separately.)
//
// It is possible, though, for a caller to try to convert values that
// do not represent an actual or unique instant in time (due to a shift
// in UTC offset in the `absl::TimeZone`, which results in a discontinuity in
// the civil-time components). For example, a daylight-saving-time
// transition skips or repeats civil times---in the United States, March
// 13, 2011 02:15 never occurred, while November 6, 2011 01:15 occurred
// twice---so requests for such times are not well-defined.
//
// To account for these possibilities, `absl::TimeConversion` is richer
// than just a single `absl::Time`. When the civil time is skipped or
// repeated, `absl::ConvertDateTime()` returns times calculated using the
// pre-transition and post-transition UTC offsets, plus the transition
// time itself.
//
// Examples:
//
//   absl::TimeZone lax;
//   if (!absl::LoadTimeZone("America/Los_Angeles", &lax)) { ... }
//
//   // A unique civil time
//   absl::TimeConversion jan01 =
//       absl::ConvertDateTime(2011, 1, 1, 0, 0, 0, lax);
//   // jan01.kind == TimeConversion::UNIQUE
//   // jan01.pre    is 2011/01/01 00:00:00 -0800
//   // jan01.trans  is 2011/01/01 00:00:00 -0800
//   // jan01.post   is 2011/01/01 00:00:00 -0800
//
//   // A Spring DST transition, when there is a gap in civil time
//   absl::TimeConversion mar13 =
//       absl::ConvertDateTime(2011, 3, 13, 2, 15, 0, lax);
//   // mar13.kind == TimeConversion::SKIPPED
//   // mar13.pre   is 2011/03/13 03:15:00 -0700
//   // mar13.trans is 2011/03/13 03:00:00 -0700
//   // mar13.post  is 2011/03/13 01:15:00 -0800
//
//   // A Fall DST transition, when civil times are repeated
//   absl::TimeConversion nov06 =
//       absl::ConvertDateTime(2011, 11, 6, 1, 15, 0, lax);
//   // nov06.kind == TimeConversion::REPEATED
//   // nov06.pre   is 2011/11/06 01:15:00 -0700
//   // nov06.trans is 2011/11/06 01:00:00 -0800
//   // nov06.post  is 2011/11/06 01:15:00 -0800
//
// The input month, day, hour, minute, and second values can also be
// outside of their valid ranges, in which case they will be "normalized"
// during the conversion.
//
// Example:
//
//   // "October 32" normalizes to "November 1".
//   absl::TimeZone tz = absl::LocalTimeZone();
//   absl::TimeConversion tc =
//       absl::ConvertDateTime(2013, 10, 32, 8, 30, 0, tz);
//   // tc.kind == TimeConversion::UNIQUE && tc.normalized == true
//   // tc.pre.In(tz).month == 11 && tc.pre.In(tz).day == 1
struct TimeConversion {
  Time pre;    // time calculated using the pre-transition offset
  Time trans;  // when the civil-time discontinuity occurred
  Time post;   // time calculated using the post-transition offset

  enum Kind {
    UNIQUE,    // the civil time was singular (pre == trans == post)
    SKIPPED,   // the civil time did not exist
    REPEATED,  // the civil time was ambiguous
  };
  Kind kind;

  bool normalized;  // input values were outside their valid ranges
};

// ConvertDateTime()
//
// The full generality of a civil time to absl::Time conversion.
TimeConversion ConvertDateTime(int64_t year, int mon, int day, int hour,
                               int min, int sec, TimeZone tz);

// FromDateTime()
//
// A convenience wrapper for `absl::ConvertDateTime()` that simply returns the
// "pre" `absl::Time`.  That is, the unique result, or the instant that
// is correct using the pre-transition offset (as if the transition
// never happened). This is typically the answer that humans expected when
// faced with non-unique times, such as near daylight-saving time transitions.
//
// Example:
//
//   absl::TimeZone seattle;
//   if (!absl::LoadTimeZone("America/Los_Angeles", &seattle)) { ... }
//   absl::Time t =  absl::FromDateTime(2017, 9, 26, 9, 30, 0, seattle);
Time FromDateTime(int64_t year, int mon, int day, int hour, int min, int sec,
                  TimeZone tz);

// FromTM()
//
// Converts the `tm_year`, `tm_mon`, `tm_mday`, `tm_hour`, `tm_min`, and
// `tm_sec` fields to an `absl::Time` using the given time zone. See ctime(3)
// for a description of the expected values of the tm fields. IFF the indicated
// time instant is not unique (see `absl::ConvertDateTime()` above), the
// `tm_isdst` field is consulted to select the desired instant (`tm_isdst` > 0
// means DST, `tm_isdst` == 0 means no DST, `tm_isdst` < 0 means use the default
// like `absl::FromDateTime()`).
Time FromTM(const struct tm& tm, TimeZone tz);

// ToTM()
//
// Converts the given `absl::Time` to a struct tm using the given time zone.
// See ctime(3) for a description of the values of the tm fields.
struct tm ToTM(Time t, TimeZone tz);

// FromUnixNanos()
// FromUnixMicros()
// FromUnixMillis()
// FromUnixSeconds()
// FromTimeT()
// FromUDate()
// FromUniversal()
//
// Creates an `absl::Time` from a variety of other representations.
constexpr Time FromUnixNanos(int64_t ns);
constexpr Time FromUnixMicros(int64_t us);
constexpr Time FromUnixMillis(int64_t ms);
constexpr Time FromUnixSeconds(int64_t s);
constexpr Time FromTimeT(time_t t);
Time FromUDate(double udate);
Time FromUniversal(int64_t universal);

// ToUnixNanos()
// ToUnixMicros()
// ToUnixMillis()
// ToUnixSeconds()
// ToTimeT()
// ToUDate()
// ToUniversal()
//
// Converts an `absl::Time` to a variety of other representations.  Note that
// these operations round down toward negative infinity where necessary to
// adjust to the resolution of the result type.  Beware of possible time_t
// over/underflow in ToTime{T,val,spec}() on 32-bit platforms.
int64_t ToUnixNanos(Time t);
int64_t ToUnixMicros(Time t);
int64_t ToUnixMillis(Time t);
int64_t ToUnixSeconds(Time t);
time_t ToTimeT(Time t);
double ToUDate(Time t);
int64_t ToUniversal(Time t);

// DurationFromTimespec()
// DurationFromTimeval()
// ToTimespec()
// ToTimeval()
// TimeFromTimespec()
// TimeFromTimeval()
// ToTimespec()
// ToTimeval()
//
// Some APIs use a timespec or a timeval as a Duration (e.g., nanosleep(2)
// and select(2)), while others use them as a Time (e.g. clock_gettime(2)
// and gettimeofday(2)), so conversion functions are provided for both cases.
// The "to timespec/val" direction is easily handled via overloading, but
// for "from timespec/val" the desired type is part of the function name.
Duration DurationFromTimespec(timespec ts);
Duration DurationFromTimeval(timeval tv);
timespec ToTimespec(Duration d);
timeval ToTimeval(Duration d);
Time TimeFromTimespec(timespec ts);
Time TimeFromTimeval(timeval tv);
timespec ToTimespec(Time t);
timeval ToTimeval(Time t);

// FromChrono()
//
// Converts a std::chrono::system_clock::time_point to an absl::Time.
//
// Example:
//
//   auto tp = std::chrono::system_clock::from_time_t(123);
//   absl::Time t = absl::FromChrono(tp);
//   // t == absl::FromTimeT(123)
Time FromChrono(const std::chrono::system_clock::time_point& tp);

// ToChronoTime()
//
// Converts an absl::Time to a std::chrono::system_clock::time_point. If
// overflow would occur, the returned value will saturate at the min/max time
// point value instead.
//
// Example:
//
//   absl::Time t = absl::FromTimeT(123);
//   auto tp = absl::ToChronoTime(t);
//   // tp == std::chrono::system_clock::from_time_t(123);
std::chrono::system_clock::time_point ToChronoTime(Time);

// RFC3339_full
// RFC3339_sec
//
// FormatTime()/ParseTime() format specifiers for RFC3339 date/time strings,
// with trailing zeros trimmed or with fractional seconds omitted altogether.
//
// Note that RFC3339_sec[] matches an ISO 8601 extended format for date
// and time with UTC offset.
extern const char RFC3339_full[];  // %Y-%m-%dT%H:%M:%E*S%Ez
extern const char RFC3339_sec[];   // %Y-%m-%dT%H:%M:%S%Ez

// RFC1123_full
// RFC1123_no_wday
//
// FormatTime()/ParseTime() format specifiers for RFC1123 date/time strings.
extern const char RFC1123_full[];     // %a, %d %b %E4Y %H:%M:%S %z
extern const char RFC1123_no_wday[];  // %d %b %E4Y %H:%M:%S %z

// FormatTime()
//
// Formats the given `absl::Time` in the `absl::TimeZone` according to the
// provided format std::string. Uses strftime()-like formatting options, with
// the following extensions:
//
//   - %Ez  - RFC3339-compatible numeric UTC offset (+hh:mm or -hh:mm)
//   - %E*z - Full-resolution numeric UTC offset (+hh:mm:ss or -hh:mm:ss)
//   - %E#S - Seconds with # digits of fractional precision
//   - %E*S - Seconds with full fractional precision (a literal '*')
//   - %E#f - Fractional seconds with # digits of precision
//   - %E*f - Fractional seconds with full precision (a literal '*')
//   - %E4Y - Four-character years (-999 ... -001, 0000, 0001 ... 9999)
//
// Note that %E0S behaves like %S, and %E0f produces no characters.  In
// contrast %E*f always produces at least one digit, which may be '0'.
//
// Note that %Y produces as many characters as it takes to fully render the
// year.  A year outside of [-999:9999] when formatted with %E4Y will produce
// more than four characters, just like %Y.
//
// We recommend that format strings include the UTC offset (%z, %Ez, or %E*z)
// so that the result uniquely identifies a time instant.
//
// Example:
//
//   absl::TimeZone lax;
//   if (!absl::LoadTimeZone("America/Los_Angeles", &lax)) { ... }
//   absl::Time t = absl::FromDateTime(2013, 1, 2, 3, 4, 5, lax);
//
//   std::string f = absl::FormatTime("%H:%M:%S", t, lax);  // "03:04:05"
//   f = absl::FormatTime("%H:%M:%E3S", t, lax);  // "03:04:05.000"
//
// Note: If the given `absl::Time` is `absl::InfiniteFuture()`, the returned
// std::string will be exactly "infinite-future". If the given `absl::Time` is
// `absl::InfinitePast()`, the returned std::string will be exactly "infinite-past".
// In both cases the given format std::string and `absl::TimeZone` are ignored.
//
std::string FormatTime(const std::string& format, Time t, TimeZone tz);

// Convenience functions that format the given time using the RFC3339_full
// format.  The first overload uses the provided TimeZone, while the second
// uses LocalTimeZone().
std::string FormatTime(Time t, TimeZone tz);
std::string FormatTime(Time t);

// Output stream operator.
inline std::ostream& operator<<(std::ostream& os, Time t) {
  return os << FormatTime(t);
}

// ParseTime()
//
// Parses an input std::string according to the provided format std::string and
// returns the corresponding `absl::Time`. Uses strftime()-like formatting
// options, with the same extensions as FormatTime(), but with the
// exceptions that %E#S is interpreted as %E*S, and %E#f as %E*f.  %Ez
// and %E*z also accept the same inputs.
//
// %Y consumes as many numeric characters as it can, so the matching data
// should always be terminated with a non-numeric.  %E4Y always consumes
// exactly four characters, including any sign.
//
// Unspecified fields are taken from the default date and time of ...
//
//   "1970-01-01 00:00:00.0 +0000"
//
// For example, parsing a std::string of "15:45" (%H:%M) will return an absl::Time
// that represents "1970-01-01 15:45:00.0 +0000".
//
// Note that since ParseTime() returns time instants, it makes the most sense
// to parse fully-specified date/time strings that include a UTC offset (%z,
// %Ez, or %E*z).
//
// Note also that `absl::ParseTime()` only heeds the fields year, month, day,
// hour, minute, (fractional) second, and UTC offset.  Other fields, like
// weekday (%a or %A), while parsed for syntactic validity, are ignored
// in the conversion.
//
// Date and time fields that are out-of-range will be treated as errors
// rather than normalizing them like `absl::FromDateTime()` does.  For example,
// it is an error to parse the date "Oct 32, 2013" because 32 is out of range.
//
// A leap second of ":60" is normalized to ":00" of the following minute
// with fractional seconds discarded.  The following table shows how the
// given seconds and subseconds will be parsed:
//
//   "59.x" -> 59.x  // exact
//   "60.x" -> 00.0  // normalized
//   "00.x" -> 00.x  // exact
//
// Errors are indicated by returning false and assigning an error message
// to the "err" out param if it is non-null.
//
// Note: If the input std::string is exactly "infinite-future", the returned
// `absl::Time` will be `absl::InfiniteFuture()` and `true` will be returned.
// If the input std::string is "infinite-past", the returned `absl::Time` will be
// `absl::InfinitePast()` and `true` will be returned.
//
bool ParseTime(const std::string& format, const std::string& input, Time* time,
               std::string* err);

// Like ParseTime() above, but if the format std::string does not contain a UTC
// offset specification (%z/%Ez/%E*z) then the input is interpreted in the
// given TimeZone.  This means that the input, by itself, does not identify a
// unique instant.  Being time-zone dependent, it also admits the possibility
// of ambiguity or non-existence, in which case the "pre" time (as defined
// for ConvertDateTime()) is returned.  For these reasons we recommend that
// all date/time strings include a UTC offset so they're context independent.
bool ParseTime(const std::string& format, const std::string& input, TimeZone tz,
               Time* time, std::string* err);

// ParseFlag()
// UnparseFlag()
//
// Support for flag values of type Time. Time flags must be specified in a
// format that matches absl::RFC3339_full. For example:
//
//   --start_time=2016-01-02T03:04:05.678+08:00
//
// Note: A UTC offset (or 'Z' indicating a zero-offset from UTC) is required.
//
// Additionally, if you'd like to specify a time as a count of
// seconds/milliseconds/etc from the Unix epoch, use an absl::Duration flag
// and add that duration to absl::UnixEpoch() to get an absl::Time.
bool ParseFlag(const std::string& text, Time* t, std::string* error);
std::string UnparseFlag(Time t);

// TimeZone
//
// The `absl::TimeZone` is an opaque, small, value-type class representing a
// geo-political region within which particular rules are used for converting
// between absolute and civil times (see https://git.io/v59Ly). `absl::TimeZone`
// values are named using the TZ identifiers from the IANA Time Zone Database,
// such as "America/Los_Angeles" or "Australia/Sydney". `absl::TimeZone` values
// are created from factory functions such as `absl::LoadTimeZone()`. Note:
// strings like "PST" and "EDT" are not valid TZ identifiers. Prefer to pass by
// value rather than const reference.
//
// For more on the fundamental concepts of time zones, absolute times, and civil
// times, see https://github.com/google/cctz#fundamental-concepts
//
// Examples:
//
//   absl::TimeZone utc = absl::UTCTimeZone();
//   absl::TimeZone pst = absl::FixedTimeZone(-8 * 60 * 60);
//   absl::TimeZone loc = absl::LocalTimeZone();
//   absl::TimeZone lax;
//   if (!absl::LoadTimeZone("America/Los_Angeles", &lax)) { ... }
//
// See also:
// - https://github.com/google/cctz
// - http://www.iana.org/time-zones
// - http://en.wikipedia.org/wiki/Zoneinfo
class TimeZone {
 public:
  explicit TimeZone(time_internal::cctz::time_zone tz) : cz_(tz) {}
  TimeZone() = default;  // UTC, but prefer UTCTimeZone() to be explicit.
  TimeZone(const TimeZone&) = default;
  TimeZone& operator=(const TimeZone&) = default;

  explicit operator time_internal::cctz::time_zone() const { return cz_; }

  std::string name() const { return cz_.name(); }

 private:
  friend bool operator==(TimeZone a, TimeZone b) { return a.cz_ == b.cz_; }
  friend bool operator!=(TimeZone a, TimeZone b) { return a.cz_ != b.cz_; }
  friend std::ostream& operator<<(std::ostream& os, TimeZone tz) {
    return os << tz.name();
  }

  time_internal::cctz::time_zone cz_;
};

// LoadTimeZone()
//
// Loads the named zone. May perform I/O on the initial load of the named
// zone. If the name is invalid, or some other kind of error occurs, returns
// `false` and `*tz` is set to the UTC time zone.
inline bool LoadTimeZone(const std::string& name, TimeZone* tz) {
  if (name == "localtime") {
    *tz = TimeZone(time_internal::cctz::local_time_zone());
    return true;
  }
  time_internal::cctz::time_zone cz;
  const bool b = time_internal::cctz::load_time_zone(name, &cz);
  *tz = TimeZone(cz);
  return b;
}

// FixedTimeZone()
//
// Returns a TimeZone that is a fixed offset (seconds east) from UTC.
// Note: If the absolute value of the offset is greater than 24 hours
// you'll get UTC (i.e., no offset) instead.
inline TimeZone FixedTimeZone(int seconds) {
  return TimeZone(
      time_internal::cctz::fixed_time_zone(std::chrono::seconds(seconds)));
}

// UTCTimeZone()
//
// Convenience method returning the UTC time zone.
inline TimeZone UTCTimeZone() {
  return TimeZone(time_internal::cctz::utc_time_zone());
}

// LocalTimeZone()
//
// Convenience method returning the local time zone, or UTC if there is
// no configured local zone.  Warning: Be wary of using LocalTimeZone(),
// and particularly so in a server process, as the zone configured for the
// local machine should be irrelevant.  Prefer an explicit zone name.
inline TimeZone LocalTimeZone() {
  return TimeZone(time_internal::cctz::local_time_zone());
}

// ============================================================================
// Implementation Details Follow
// ============================================================================

namespace time_internal {

// Creates a Duration with a given representation.
// REQUIRES: hi,lo is a valid representation of a Duration as specified
// in time/duration.cc.
constexpr Duration MakeDuration(int64_t hi, uint32_t lo = 0) {
  return Duration(hi, lo);
}

constexpr Duration MakeDuration(int64_t hi, int64_t lo) {
  return MakeDuration(hi, static_cast<uint32_t>(lo));
}

// Creates a normalized Duration from an almost-normalized (sec,ticks)
// pair. sec may be positive or negative.  ticks must be in the range
// -kTicksPerSecond < *ticks < kTicksPerSecond.  If ticks is negative it
// will be normalized to a positive value in the resulting Duration.
constexpr Duration MakeNormalizedDuration(int64_t sec, int64_t ticks) {
  return (ticks < 0) ? MakeDuration(sec - 1, ticks + kTicksPerSecond)
                     : MakeDuration(sec, ticks);
}
// Provide access to the Duration representation.
constexpr int64_t GetRepHi(Duration d) { return d.rep_hi_; }
constexpr uint32_t GetRepLo(Duration d) { return d.rep_lo_; }
constexpr bool IsInfiniteDuration(Duration d) { return GetRepLo(d) == ~0U; }

// Returns an infinite Duration with the opposite sign.
// REQUIRES: IsInfiniteDuration(d)
constexpr Duration OppositeInfinity(Duration d) {
  return GetRepHi(d) < 0
             ? MakeDuration(std::numeric_limits<int64_t>::max(), ~0U)
             : MakeDuration(std::numeric_limits<int64_t>::min(), ~0U);
}

// Returns (-n)-1 (equivalently -(n+1)) without avoidable overflow.
constexpr int64_t NegateAndSubtractOne(int64_t n) {
  // Note: Good compilers will optimize this expression to ~n when using
  // a two's-complement representation (which is required for int64_t).
  return (n < 0) ? -(n + 1) : (-n) - 1;
}

// Map between a Time and a Duration since the Unix epoch.  Note that these
// functions depend on the above mentioned choice of the Unix epoch for the
// Time representation (and both need to be Time friends).  Without this
// knowledge, we would need to add-in/subtract-out UnixEpoch() respectively.
constexpr Time FromUnixDuration(Duration d) { return Time(d); }
constexpr Duration ToUnixDuration(Time t) { return t.rep_; }

template <std::intmax_t N>
constexpr Duration FromInt64(int64_t v, std::ratio<1, N>) {
  static_assert(0 < N && N <= 1000 * 1000 * 1000, "Unsupported ratio");
  // Subsecond ratios cannot overflow.
  return MakeNormalizedDuration(
      v / N, v % N * kTicksPerNanosecond * 1000 * 1000 * 1000 / N);
}
constexpr Duration FromInt64(int64_t v, std::ratio<60>) {
  return (v <= std::numeric_limits<int64_t>::max() / 60 &&
          v >= std::numeric_limits<int64_t>::min() / 60)
             ? MakeDuration(v * 60)
             : v > 0 ? InfiniteDuration() : -InfiniteDuration();
}
constexpr Duration FromInt64(int64_t v, std::ratio<3600>) {
  return (v <= std::numeric_limits<int64_t>::max() / 3600 &&
          v >= std::numeric_limits<int64_t>::min() / 3600)
             ? MakeDuration(v * 3600)
             : v > 0 ? InfiniteDuration() : -InfiniteDuration();
}

// IsValidRep64<T>(0) is true if the expression `int64_t{std::declval<T>()}` is
// valid. That is, if a T can be assigned to an int64_t without narrowing.
template <typename T>
constexpr auto IsValidRep64(int)
    -> decltype(int64_t{std::declval<T>()}, bool()) {
  return true;
}
template <typename T>
constexpr auto IsValidRep64(char) -> bool {
  return false;
}

// Converts a std::chrono::duration to an absl::Duration.
template <typename Rep, typename Period>
constexpr Duration FromChrono(const std::chrono::duration<Rep, Period>& d) {
  static_assert(IsValidRep64<Rep>(0), "duration::rep is invalid");
  return FromInt64(int64_t{d.count()}, Period{});
}

template <typename Ratio>
int64_t ToInt64(Duration d, Ratio) {
  // Note: This may be used on MSVC, which may have a system_clock period of
  // std::ratio<1, 10 * 1000 * 1000>
  return ToInt64Seconds(d * Ratio::den / Ratio::num);
}
// Fastpath implementations for the 6 common duration units.
inline int64_t ToInt64(Duration d, std::nano) {
  return ToInt64Nanoseconds(d);
}
inline int64_t ToInt64(Duration d, std::micro) {
  return ToInt64Microseconds(d);
}
inline int64_t ToInt64(Duration d, std::milli) {
  return ToInt64Milliseconds(d);
}
inline int64_t ToInt64(Duration d, std::ratio<1>) {
  return ToInt64Seconds(d);
}
inline int64_t ToInt64(Duration d, std::ratio<60>) {
  return ToInt64Minutes(d);
}
inline int64_t ToInt64(Duration d, std::ratio<3600>) {
  return ToInt64Hours(d);
}

// Converts an absl::Duration to a chrono duration of type T.
template <typename T>
T ToChronoDuration(Duration d) {
  using Rep = typename T::rep;
  using Period = typename T::period;
  static_assert(IsValidRep64<Rep>(0), "duration::rep is invalid");
  if (time_internal::IsInfiniteDuration(d))
    return d < ZeroDuration() ? T::min() : T::max();
  const auto v = ToInt64(d, Period{});
  if (v > std::numeric_limits<Rep>::max()) return T::max();
  if (v < std::numeric_limits<Rep>::min()) return T::min();
  return T{v};
}

}  // namespace time_internal
constexpr Duration Nanoseconds(int64_t n) {
  return time_internal::FromInt64(n, std::nano{});
}
constexpr Duration Microseconds(int64_t n) {
  return time_internal::FromInt64(n, std::micro{});
}
constexpr Duration Milliseconds(int64_t n) {
  return time_internal::FromInt64(n, std::milli{});
}
constexpr Duration Seconds(int64_t n) {
  return time_internal::FromInt64(n, std::ratio<1>{});
}
constexpr Duration Minutes(int64_t n) {
  return time_internal::FromInt64(n, std::ratio<60>{});
}
constexpr Duration Hours(int64_t n) {
  return time_internal::FromInt64(n, std::ratio<3600>{});
}

constexpr bool operator<(Duration lhs, Duration rhs) {
  return time_internal::GetRepHi(lhs) != time_internal::GetRepHi(rhs)
             ? time_internal::GetRepHi(lhs) < time_internal::GetRepHi(rhs)
             : time_internal::GetRepHi(lhs) == std::numeric_limits<int64_t>::min()
                   ? time_internal::GetRepLo(lhs) + 1 <
                         time_internal::GetRepLo(rhs) + 1
                   : time_internal::GetRepLo(lhs) <
                         time_internal::GetRepLo(rhs);
}

constexpr bool operator==(Duration lhs, Duration rhs) {
  return time_internal::GetRepHi(lhs) == time_internal::GetRepHi(rhs) &&
         time_internal::GetRepLo(lhs) == time_internal::GetRepLo(rhs);
}

constexpr Duration operator-(Duration d) {
  // This is a little interesting because of the special cases.
  //
  // If rep_lo_ is zero, we have it easy; it's safe to negate rep_hi_, we're
  // dealing with an integral number of seconds, and the only special case is
  // the maximum negative finite duration, which can't be negated.
  //
  // Infinities stay infinite, and just change direction.
  //
  // Finally we're in the case where rep_lo_ is non-zero, and we can borrow
  // a second's worth of ticks and avoid overflow (as negating int64_t-min + 1
  // is safe).
  return time_internal::GetRepLo(d) == 0
             ? time_internal::GetRepHi(d) == std::numeric_limits<int64_t>::min()
                   ? InfiniteDuration()
                   : time_internal::MakeDuration(-time_internal::GetRepHi(d))
             : time_internal::IsInfiniteDuration(d)
                   ? time_internal::OppositeInfinity(d)
                   : time_internal::MakeDuration(
                         time_internal::NegateAndSubtractOne(
                             time_internal::GetRepHi(d)),
                         time_internal::kTicksPerSecond -
                             time_internal::GetRepLo(d));
}

constexpr Duration InfiniteDuration() {
  return time_internal::MakeDuration(std::numeric_limits<int64_t>::max(), ~0U);
}

constexpr Duration FromChrono(const std::chrono::nanoseconds& d) {
  return time_internal::FromChrono(d);
}
constexpr Duration FromChrono(const std::chrono::microseconds& d) {
  return time_internal::FromChrono(d);
}
constexpr Duration FromChrono(const std::chrono::milliseconds& d) {
  return time_internal::FromChrono(d);
}
constexpr Duration FromChrono(const std::chrono::seconds& d) {
  return time_internal::FromChrono(d);
}
constexpr Duration FromChrono(const std::chrono::minutes& d) {
  return time_internal::FromChrono(d);
}
constexpr Duration FromChrono(const std::chrono::hours& d) {
  return time_internal::FromChrono(d);
}

constexpr Time FromUnixNanos(int64_t ns) {
  return time_internal::FromUnixDuration(Nanoseconds(ns));
}

constexpr Time FromUnixMicros(int64_t us) {
  return time_internal::FromUnixDuration(Microseconds(us));
}

constexpr Time FromUnixMillis(int64_t ms) {
  return time_internal::FromUnixDuration(Milliseconds(ms));
}

constexpr Time FromUnixSeconds(int64_t s) {
  return time_internal::FromUnixDuration(Seconds(s));
}

constexpr Time FromTimeT(time_t t) {
  return time_internal::FromUnixDuration(Seconds(t));
}

}  // inline namespace lts_2018_06_20
}  // namespace absl

#endif  // ABSL_TIME_TIME_H_
