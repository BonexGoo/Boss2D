/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_PC_E2E_TEST_ACTIVITIES_EXECUTOR_H_
#define TEST_PC_E2E_TEST_ACTIVITIES_EXECUTOR_H_

#include <queue>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__units__time_delta_h //original-code:"api/units/time_delta.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_for_test_h //original-code:"rtc_base/task_queue_for_test.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__repeating_task_h //original-code:"rtc_base/task_utils/repeating_task.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {
namespace webrtc_pc_e2e {

class TestActivitiesExecutor {
 public:
  explicit TestActivitiesExecutor(Clock* clock) : clock_(clock) {}
  ~TestActivitiesExecutor() { Stop(); }

  // Starts scheduled activities according to their schedule. All activities
  // that will be scheduled after Start(...) was invoked will be executed
  // immediately according to their schedule.
  void Start(TaskQueueForTest* task_queue);
  void Stop();

  // Schedule activity to be executed. If test isn't started yet, then activity
  // will be executed according to its schedule after Start() will be invoked.
  // If test is started, then it will be executed immediately according to its
  // schedule.
  void ScheduleActivity(TimeDelta initial_delay_since_start,
                        absl::optional<TimeDelta> interval,
                        std::function<void(TimeDelta)> func);

 private:
  struct ScheduledActivity {
    ScheduledActivity(TimeDelta initial_delay_since_start,
                      absl::optional<TimeDelta> interval,
                      std::function<void(TimeDelta)> func);

    TimeDelta initial_delay_since_start;
    absl::optional<TimeDelta> interval;
    std::function<void(TimeDelta)> func;
  };

  void PostActivity(ScheduledActivity activity)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(lock_);
  Timestamp Now() const;

  Clock* const clock_;

  TaskQueueForTest* task_queue_;

  Mutex lock_;
  // Time when test was started. Minus infinity means that it wasn't started
  // yet.
  Timestamp start_time_ RTC_GUARDED_BY(lock_) = Timestamp::MinusInfinity();
  // Queue of activities that were added before test was started.
  // Activities from this queue will be posted on the |task_queue_| after test
  // will be set up and then this queue will be unused.
  std::queue<ScheduledActivity> scheduled_activities_ RTC_GUARDED_BY(lock_);
  // List of task handles for activities, that are posted on |task_queue_| as
  // repeated during the call.
  std::vector<RepeatingTaskHandle> repeating_task_handles_
      RTC_GUARDED_BY(lock_);
};

}  // namespace webrtc_pc_e2e
}  // namespace webrtc

#endif  // TEST_PC_E2E_TEST_ACTIVITIES_EXECUTOR_H_
