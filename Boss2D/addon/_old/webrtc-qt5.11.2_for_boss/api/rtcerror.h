/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_RTCERROR_H_
#define API_RTCERROR_H_

#include <ostream>
#include <string>
#include <utility>  // For std::move.

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace webrtc {

// Enumeration to represent distinct classes of errors that an application
// may wish to act upon differently. These roughly map to DOMExceptions or
// RTCError "errorDetailEnum" values in the web API, as described in the
// comments below.
enum class RTCErrorType {
  // No error.
  NONE,

  // An operation is valid, but currently unsupported.
  // Maps to OperationError DOMException.
  UNSUPPORTED_OPERATION,

  // A supplied parameter is valid, but currently unsupported.
  // Maps to OperationError DOMException.
  UNSUPPORTED_PARAMETER,

  // General error indicating that a supplied parameter is invalid.
  // Maps to InvalidAccessError or TypeError DOMException depending on context.
  INVALID_PARAMETER,

  // Slightly more specific than INVALID_PARAMETER; a parameter's value was
  // outside the allowed range.
  // Maps to RangeError DOMException.
  INVALID_RANGE,

  // Slightly more specific than INVALID_PARAMETER; an error occurred while
  // parsing string input.
  // Maps to SyntaxError DOMException.
  SYNTAX_ERROR,

  // The object does not support this operation in its current state.
  // Maps to InvalidStateError DOMException.
  INVALID_STATE,

  // An attempt was made to modify the object in an invalid way.
  // Maps to InvalidModificationError DOMException.
  INVALID_MODIFICATION,

  // An error occurred within an underlying network protocol.
  // Maps to NetworkError DOMException.
  NETWORK_ERROR,

  // Some resource has been exhausted; file handles, hardware resources, ports,
  // etc.
  // Maps to OperationError DOMException.
  RESOURCE_EXHAUSTED,

  // The operation failed due to an internal error.
  // Maps to OperationError DOMException.
  INTERNAL_ERROR,
};

// Roughly corresponds to RTCError in the web api. Holds an error type, a
// message, and possibly additional information specific to that error.
//
// Doesn't contain anything beyond a type and message now, but will in the
// future as more errors are implemented.
class RTCError {
 public:
  // Constructors.

  // Creates a "no error" error.
  RTCError() {}
  explicit RTCError(RTCErrorType type) : type_(type) {}
  // For performance, prefer using the constructor that takes a const char* if
  // the message is a static string.
  RTCError(RTCErrorType type, const char* message)
      : type_(type), static_message_(message), have_string_message_(false) {}
  RTCError(RTCErrorType type, std::string&& message)
      : type_(type), string_message_(message), have_string_message_(true) {}

  // Delete the copy constructor and assignment operator; there aren't any use
  // cases where you should need to copy an RTCError, as opposed to moving it.
  // Can revisit this decision if use cases arise in the future.
  RTCError(const RTCError& other) = delete;
  RTCError& operator=(const RTCError& other) = delete;

  // Move constructor and move-assignment operator.
  RTCError(RTCError&& other);
  RTCError& operator=(RTCError&& other);

  ~RTCError();

  // Identical to default constructed error.
  //
  // Preferred over the default constructor for code readability.
  static RTCError OK();

  // Error type.
  RTCErrorType type() const { return type_; }
  void set_type(RTCErrorType type) { type_ = type; }

  // Human-readable message describing the error. Shouldn't be used for
  // anything but logging/diagnostics, since messages are not guaranteed to be
  // stable.
  const char* message() const;
  // For performance, prefer using the method that takes a const char* if the
  // message is a static string.
  void set_message(const char* message);
  void set_message(std::string&& message);

  // Convenience method for situations where you only care whether or not an
  // error occurred.
  bool ok() const { return type_ == RTCErrorType::NONE; }

 private:
  RTCErrorType type_ = RTCErrorType::NONE;
  // For performance, we use static strings wherever possible. But in some
  // cases the error string may need to be constructed, in which case an
  // std::string is used.
  union {
    const char* static_message_ = "";
    std::string string_message_;
  };
  // Whether or not |static_message_| or |string_message_| is being used in the
  // above union.
  bool have_string_message_ = false;
};

// Outputs the error as a friendly string. Update this method when adding a new
// error type.
//
// Only intended to be used for logging/disagnostics.
std::ostream& operator<<(std::ostream& stream, RTCErrorType error);

// Helper macro that can be used by implementations to create an error with a
// message and log it. |message| should be a string literal or movable
// std::string.
#define LOG_AND_RETURN_ERROR_EX(type, message, severity) \
  {                                                      \
    RTC_DCHECK(type != RTCErrorType::NONE);              \
    RTC_LOG(severity) << message << " (" << type << ")"; \
    return webrtc::RTCError(type, message);              \
  }

#define LOG_AND_RETURN_ERROR(type, message) \
  LOG_AND_RETURN_ERROR_EX(type, message, LS_ERROR)

// RTCErrorOr<T> is the union of an RTCError object and a T object. RTCErrorOr
// models the concept of an object that is either a usable value, or an error
// Status explaining why such a value is not present. To this end RTCErrorOr<T>
// does not allow its RTCErrorType value to be RTCErrorType::NONE. This is
// enforced by a debug check in most cases.
//
// The primary use-case for RTCErrorOr<T> is as the return value of a function
// which may fail. For example, CreateRtpSender will fail if the parameters
// could not be successfully applied at the media engine level, but if
// successful will return a unique_ptr to an RtpSender.
//
// Example client usage for a RTCErrorOr<std::unique_ptr<T>>:
//
//  RTCErrorOr<std::unique_ptr<Foo>> result = FooFactory::MakeNewFoo(arg);
//  if (result.ok()) {
//    std::unique_ptr<Foo> foo = result.ConsumeValue();
//    foo->DoSomethingCool();
//  } else {
//    RTC_LOG(LS_ERROR) << result.error();
//  }
//
// Example factory implementation returning RTCErrorOr<std::unique_ptr<T>>:
//
//  RTCErrorOr<std::unique_ptr<Foo>> FooFactory::MakeNewFoo(int arg) {
//    if (arg <= 0) {
//      return RTCError(RTCErrorType::INVALID_RANGE, "Arg must be positive");
//    } else {
//      return std::unique_ptr<Foo>(new Foo(arg));
//    }
//  }
//
template <typename T>
class RTCErrorOr {
  // Used to convert between RTCErrorOr<Foo>/RtcErrorOr<Bar>, when an implicit
  // conversion from Foo to Bar exists.
  template <typename U>
  friend class RTCErrorOr;

 public:
  typedef T element_type;

  // Constructs a new RTCErrorOr with RTCErrorType::INTERNAL_ERROR error. This
  // is marked 'explicit' to try to catch cases like 'return {};', where people
  // think RTCErrorOr<std::vector<int>> will be initialized with an empty
  // vector, instead of a RTCErrorType::INTERNAL_ERROR error.
  RTCErrorOr() : error_(RTCErrorType::INTERNAL_ERROR) {}

  // Constructs a new RTCErrorOr with the given non-ok error. After calling
  // this constructor, calls to value() will DCHECK-fail.
  //
  // NOTE: Not explicit - we want to use RTCErrorOr<T> as a return
  // value, so it is convenient and sensible to be able to do 'return
  // RTCError(...)' when the return type is RTCErrorOr<T>.
  //
  // REQUIRES: !error.ok(). This requirement is DCHECKed.
  RTCErrorOr(RTCError&& error) : error_(std::move(error)) {  // NOLINT
    RTC_DCHECK(!error.ok());
  }

  // Constructs a new RTCErrorOr with the given value. After calling this
  // constructor, calls to value() will succeed, and calls to error() will
  // return a default-constructed RTCError.
  //
  // NOTE: Not explicit - we want to use RTCErrorOr<T> as a return type
  // so it is convenient and sensible to be able to do 'return T()'
  // when the return type is RTCErrorOr<T>.
  RTCErrorOr(T&& value) : value_(std::move(value)) {}  // NOLINT

  // Delete the copy constructor and assignment operator; there aren't any use
  // cases where you should need to copy an RTCErrorOr, as opposed to moving
  // it. Can revisit this decision if use cases arise in the future.
  RTCErrorOr(const RTCErrorOr& other) = delete;
  RTCErrorOr& operator=(const RTCErrorOr& other) = delete;

  // Move constructor and move-assignment operator.
  //
  // Visual Studio doesn't support "= default" with move constructors or
  // assignment operators (even though they compile, they segfault), so define
  // them explicitly.
  RTCErrorOr(RTCErrorOr&& other)
      : error_(std::move(other.error_)), value_(std::move(other.value_)) {}
  RTCErrorOr& operator=(RTCErrorOr&& other) {
    error_ = std::move(other.error_);
    value_ = std::move(other.value_);
    return *this;
  }

  // Conversion constructor and assignment operator; T must be copy or move
  // constructible from U.
  template <typename U>
  RTCErrorOr(RTCErrorOr<U> other)  // NOLINT
      : error_(std::move(other.error_)), value_(std::move(other.value_)) {}
  template <typename U>
  RTCErrorOr& operator=(RTCErrorOr<U> other) {
    error_ = std::move(other.error_);
    value_ = std::move(other.value_);
    return *this;
  }

  // Returns a reference to our error. If this contains a T, then returns
  // default-constructed RTCError.
  const RTCError& error() const { return error_; }

  // Moves the error. Can be useful if, say "CreateFoo" returns an
  // RTCErrorOr<Foo>, and internally calls "CreateBar" which returns an
  // RTCErrorOr<Bar>, and wants to forward the error up the stack.
  RTCError MoveError() { return std::move(error_); }

  // Returns this->error().ok()
  bool ok() const { return error_.ok(); }

  // Returns a reference to our current value, or DCHECK-fails if !this->ok().
  //
  // Can be convenient for the implementation; for example, a method may want
  // to access the value in some way before returning it to the next method on
  // the stack.
  const T& value() const {
    RTC_DCHECK(ok());
    return value_;
  }
  T& value() {
    RTC_DCHECK(ok());
    return value_;
  }

  // Moves our current value out of this object and returns it, or DCHECK-fails
  // if !this->ok().
  T MoveValue() {
    RTC_DCHECK(ok());
    return std::move(value_);
  }

 private:
  RTCError error_;
  T value_;
};

}  // namespace webrtc

#endif  // API_RTCERROR_H_
