/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_TIMER_TASK_QUEUE_TIMEOUT_H_
#define NET_DCSCTP_TIMER_TASK_QUEUE_TIMEOUT_H_

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__timeout_h //original-code:"net/dcsctp/public/timeout.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__pending_task_safety_flag_h //original-code:"rtc_base/task_utils/pending_task_safety_flag.h"

namespace dcsctp {

// The TaskQueueTimeoutFactory creates `Timeout` instances, which schedules
// itself to be triggered on the provided `task_queue`, which may be a thread,
// an actual TaskQueue or something else which supports posting a delayed task.
//
// Note that each `DcSctpSocket` must have its own `TaskQueueTimeoutFactory`,
// as the `TimeoutID` are not unique among sockets.
//
// This class must outlive any created Timeout that it has created. Note that
// the `DcSctpSocket` will ensure that all Timeouts are deleted when the socket
// is destructed, so this means that this class must outlive the `DcSctpSocket`.
//
// This class, and the timeouts created it, are not thread safe.
class TaskQueueTimeoutFactory {
 public:
  // The `get_time` function must return the current time, relative to any
  // epoch. Whenever a timeout expires, the `on_expired` callback will be
  // triggered, and then the client should provided `timeout_id` to
  // `DcSctpSocketInterface::HandleTimeout`.
  TaskQueueTimeoutFactory(webrtc::TaskQueueBase& task_queue,
                          std::function<TimeMs()> get_time,
                          std::function<void(TimeoutID timeout_id)> on_expired)
      : task_queue_(task_queue),
        get_time_(std::move(get_time)),
        on_expired_(std::move(on_expired)) {}

  // Creates an implementation of `Timeout`.
  std::unique_ptr<Timeout> CreateTimeout() {
    return std::make_unique<TaskQueueTimeout>(*this);
  }

 private:
  class TaskQueueTimeout : public Timeout {
   public:
    explicit TaskQueueTimeout(TaskQueueTimeoutFactory& parent);
    ~TaskQueueTimeout();

    void Start(DurationMs duration_ms, TimeoutID timeout_id) override;
    void Stop() override;

   private:
    TaskQueueTimeoutFactory& parent_;
    // A safety flag to ensure that posted tasks to the task queue don't
    // reference these object when they go out of scope. Note that this safety
    // flag will be re-created if the scheduled-but-not-yet-expired task is not
    // to be run. This happens when there is a posted delayed task with an
    // expiration time _further away_ than what is now the expected expiration
    // time. In this scenario, a new delayed task has to be posted with a
    // shorter duration and the old task has to be forgotten.
    rtc::scoped_refptr<webrtc::PendingTaskSafetyFlag> pending_task_safety_flag_;
    // The time when the posted delayed task is set to expire. Will be set to
    // the infinite future if there is no such task running.
    TimeMs posted_task_expiration_ = TimeMs::InfiniteFuture();
    // The time when the timeout expires. It will be set to the infinite future
    // if the timeout is not running/not started.
    TimeMs timeout_expiration_ = TimeMs::InfiniteFuture();
    // The current timeout ID that will be reported when expired.
    TimeoutID timeout_id_ = TimeoutID(0);
  };

  RTC_NO_UNIQUE_ADDRESS webrtc::SequenceChecker thread_checker_;
  webrtc::TaskQueueBase& task_queue_;
  const std::function<TimeMs()> get_time_;
  const std::function<void(TimeoutID)> on_expired_;
};
}  // namespace dcsctp

#endif  // NET_DCSCTP_TIMER_TASK_QUEUE_TIMEOUT_H_
