/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_TIME_CONTROLLER_EXTERNAL_TIME_CONTROLLER_H_
#define TEST_TIME_CONTROLLER_EXTERNAL_TIME_CONTROLLER_H_

#include <functional>
#include <memory>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_api__test__time_controller_h //original-code:"api/test/time_controller.h"
#include BOSS_WEBRTC_U_api__units__time_delta_h //original-code:"api/units/time_delta.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"
#include "test/time_controller/simulated_time_controller.h"

namespace webrtc {

// TimeController implementation built on an external controlled alarm.
// This implementation is used to delegate scheduling and execution to an
// external run loop.
class ExternalTimeController : public TimeController, public TaskQueueFactory {
 public:
  explicit ExternalTimeController(ControlledAlarmClock* alarm);

  // Implementation of TimeController.
  Clock* GetClock() override;
  TaskQueueFactory* GetTaskQueueFactory() override;
  std::unique_ptr<ProcessThread> CreateProcessThread(
      const char* thread_name) override;
  void AdvanceTime(TimeDelta duration) override;
  std::unique_ptr<rtc::Thread> CreateThread(
      const std::string& name,
      std::unique_ptr<rtc::SocketServer> socket_server) override;
  rtc::Thread* GetMainThread() override;

  // Implementation of TaskQueueFactory.
  std::unique_ptr<TaskQueueBase, TaskQueueDeleter> CreateTaskQueue(
      absl::string_view name,
      TaskQueueFactory::Priority priority) const override;

 private:
  class ProcessThreadWrapper;
  class TaskQueueWrapper;

  // Executes any tasks scheduled at or before the current time.  May call
  // |ScheduleNext| to schedule the next call to |Run|.
  void Run();

  void UpdateTime();
  void ScheduleNext();

  ControlledAlarmClock* alarm_;
  sim_time_impl::SimulatedTimeControllerImpl impl_;
  rtc::ScopedYieldPolicy yield_policy_;

  // Overrides the global rtc::Clock to ensure that it reports the same times as
  // the time controller.
  rtc::ScopedBaseFakeClock global_clock_;
};

}  // namespace webrtc

#endif  // TEST_TIME_CONTROLLER_EXTERNAL_TIME_CONTROLLER_H_
