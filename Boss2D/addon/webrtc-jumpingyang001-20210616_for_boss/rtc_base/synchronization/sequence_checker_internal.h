/*
 *  Copyright 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef RTC_BASE_SYNCHRONIZATION_SEQUENCE_CHECKER_INTERNAL_H_
#define RTC_BASE_SYNCHRONIZATION_SEQUENCE_CHECKER_INTERNAL_H_

#include <string>
#include <type_traits>

#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"
#include BOSS_WEBRTC_U_rtc_base__platform_thread_types_h //original-code:"rtc_base/platform_thread_types.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {
namespace webrtc_sequence_checker_internal {

// Real implementation of SequenceChecker, for use in debug mode, or
// for temporary use in release mode (e.g. to RTC_CHECK on a threading issue
// seen only in the wild).
//
// Note: You should almost always use the SequenceChecker class to get the
// right version for your build configuration.
class RTC_EXPORT SequenceCheckerImpl {
 public:
  SequenceCheckerImpl();
  ~SequenceCheckerImpl() = default;

  bool IsCurrent() const;
  // Changes the task queue or thread that is checked for in IsCurrent. This can
  // be useful when an object may be created on one task queue / thread and then
  // used exclusively on another thread.
  void Detach();

  // Returns a string that is formatted to match with the error string printed
  // by RTC_CHECK() when a condition is not met.
  // This is used in conjunction with the RTC_DCHECK_RUN_ON() macro.
  std::string ExpectationToString() const;

 private:
  mutable Mutex lock_;
  // These are mutable so that IsCurrent can set them.
  mutable bool attached_ RTC_GUARDED_BY(lock_);
  mutable rtc::PlatformThreadRef valid_thread_ RTC_GUARDED_BY(lock_);
  mutable const TaskQueueBase* valid_queue_ RTC_GUARDED_BY(lock_);
  mutable const void* valid_system_queue_ RTC_GUARDED_BY(lock_);
};

// Do nothing implementation, for use in release mode.
//
// Note: You should almost always use the SequenceChecker class to get the
// right version for your build configuration.
class SequenceCheckerDoNothing {
 public:
  bool IsCurrent() const { return true; }
  void Detach() {}
};

// Helper class used by RTC_DCHECK_RUN_ON (see example usage below).
class RTC_SCOPED_LOCKABLE SequenceCheckerScope {
 public:
  template <typename ThreadLikeObject>
  explicit SequenceCheckerScope(const ThreadLikeObject* thread_like_object)
      RTC_EXCLUSIVE_LOCK_FUNCTION(thread_like_object) {}
  SequenceCheckerScope(const SequenceCheckerScope&) = delete;
  SequenceCheckerScope& operator=(const SequenceCheckerScope&) = delete;
  ~SequenceCheckerScope() RTC_UNLOCK_FUNCTION() {}

  template <typename ThreadLikeObject>
  static bool IsCurrent(const ThreadLikeObject* thread_like_object) {
    return thread_like_object->IsCurrent();
  }
};

std::string ExpectationToString(const SequenceCheckerImpl* checker);

// Catch-all implementation for types other than explicitly supported above.
template <typename ThreadLikeObject>
std::string ExpectationToString(const ThreadLikeObject*) {
  return std::string();
}

}  // namespace webrtc_sequence_checker_internal
}  // namespace webrtc

#endif  // RTC_BASE_SYNCHRONIZATION_SEQUENCE_CHECKER_INTERNAL_H_
