/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__timer__task_queue_timeout_h //original-code:"net/dcsctp/timer/task_queue_timeout.h"

#include <memory>

#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include "test/time_controller/simulated_time_controller.h"

namespace dcsctp {
namespace {
using ::testing::MockFunction;

class TaskQueueTimeoutTest : public testing::Test {
 protected:
  TaskQueueTimeoutTest()
      : time_controller_(webrtc::Timestamp::Millis(1234)),
        task_queue_(time_controller_.GetMainThread()),
        factory_(
            *task_queue_,
            [this]() {
              return TimeMs(time_controller_.GetClock()->CurrentTime().ms());
            },
            on_expired_.AsStdFunction()) {}

  void AdvanceTime(DurationMs duration) {
    time_controller_.AdvanceTime(webrtc::TimeDelta::Millis(*duration));
  }

  MockFunction<void(TimeoutID)> on_expired_;
  webrtc::GlobalSimulatedTimeController time_controller_;

  rtc::Thread* task_queue_;
  TaskQueueTimeoutFactory factory_;
};

TEST_F(TaskQueueTimeoutTest, StartPostsDelayedTask) {
  std::unique_ptr<Timeout> timeout = factory_.CreateTimeout();
  timeout->Start(DurationMs(1000), TimeoutID(1));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTime(DurationMs(999));

  EXPECT_CALL(on_expired_, Call(TimeoutID(1)));
  AdvanceTime(DurationMs(1));
}

TEST_F(TaskQueueTimeoutTest, StopBeforeExpiringDoesntTrigger) {
  std::unique_ptr<Timeout> timeout = factory_.CreateTimeout();
  timeout->Start(DurationMs(1000), TimeoutID(1));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTime(DurationMs(999));

  timeout->Stop();

  AdvanceTime(DurationMs(1));
  AdvanceTime(DurationMs(1000));
}

TEST_F(TaskQueueTimeoutTest, RestartPrologingTimeoutDuration) {
  std::unique_ptr<Timeout> timeout = factory_.CreateTimeout();
  timeout->Start(DurationMs(1000), TimeoutID(1));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTime(DurationMs(500));

  timeout->Restart(DurationMs(1000), TimeoutID(2));

  AdvanceTime(DurationMs(999));

  EXPECT_CALL(on_expired_, Call(TimeoutID(2)));
  AdvanceTime(DurationMs(1));
}

TEST_F(TaskQueueTimeoutTest, RestartWithShorterDurationExpiresWhenExpected) {
  std::unique_ptr<Timeout> timeout = factory_.CreateTimeout();
  timeout->Start(DurationMs(1000), TimeoutID(1));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTime(DurationMs(500));

  timeout->Restart(DurationMs(200), TimeoutID(2));

  AdvanceTime(DurationMs(199));

  EXPECT_CALL(on_expired_, Call(TimeoutID(2)));
  AdvanceTime(DurationMs(1));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTime(DurationMs(1000));
}

TEST_F(TaskQueueTimeoutTest, KilledBeforeExpired) {
  std::unique_ptr<Timeout> timeout = factory_.CreateTimeout();
  timeout->Start(DurationMs(1000), TimeoutID(1));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTime(DurationMs(500));

  timeout = nullptr;

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTime(DurationMs(1000));
}
}  // namespace
}  // namespace dcsctp
