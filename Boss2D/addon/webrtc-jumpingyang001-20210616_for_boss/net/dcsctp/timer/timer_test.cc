/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__timer__timer_h //original-code:"net/dcsctp/timer/timer.h"

#include <memory>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__timeout_h //original-code:"net/dcsctp/public/timeout.h"
#include BOSS_WEBRTC_U_net__dcsctp__timer__fake_timeout_h //original-code:"net/dcsctp/timer/fake_timeout.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace dcsctp {
namespace {
using ::testing::Return;

class TimerTest : public testing::Test {
 protected:
  TimerTest()
      : timeout_manager_([this]() { return now_; }),
        manager_([this]() { return timeout_manager_.CreateTimeout(); }) {
    ON_CALL(on_expired_, Call).WillByDefault(Return(absl::nullopt));
  }

  void AdvanceTimeAndRunTimers(DurationMs duration) {
    now_ = now_ + duration;

    for (;;) {
      absl::optional<TimeoutID> timeout_id =
          timeout_manager_.GetNextExpiredTimeout();
      if (!timeout_id.has_value()) {
        break;
      }
      manager_.HandleTimeout(*timeout_id);
    }
  }

  TimeMs now_ = TimeMs(0);
  FakeTimeoutManager timeout_manager_;
  TimerManager manager_;
  testing::MockFunction<absl::optional<DurationMs>()> on_expired_;
};

TEST_F(TimerTest, TimerIsInitiallyStopped) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kFixed));

  EXPECT_FALSE(t1->is_running());
}

TEST_F(TimerTest, TimerExpiresAtGivenTime) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kFixed));

  EXPECT_CALL(on_expired_, Call).Times(0);
  t1->Start();
  EXPECT_TRUE(t1->is_running());

  AdvanceTimeAndRunTimers(DurationMs(4000));

  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
}

TEST_F(TimerTest, TimerReschedulesAfterExpiredWithFixedBackoff) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kFixed));

  EXPECT_CALL(on_expired_, Call).Times(0);
  t1->Start();
  EXPECT_EQ(t1->expiration_count(), 0);

  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Fire first time
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_TRUE(t1->is_running());
  EXPECT_EQ(t1->expiration_count(), 1);

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Second time
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_TRUE(t1->is_running());
  EXPECT_EQ(t1->expiration_count(), 2);

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Third time
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_TRUE(t1->is_running());
  EXPECT_EQ(t1->expiration_count(), 3);
}

TEST_F(TimerTest, TimerWithNoRestarts) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kFixed,
                   /*max_restart=*/0));

  EXPECT_CALL(on_expired_, Call).Times(0);
  t1->Start();
  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Fire first time
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));

  EXPECT_FALSE(t1->is_running());

  // Second time - shouldn't fire
  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(5000));
  EXPECT_FALSE(t1->is_running());
}

TEST_F(TimerTest, TimerWithOneRestart) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kFixed,
                   /*max_restart=*/1));

  EXPECT_CALL(on_expired_, Call).Times(0);
  t1->Start();
  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Fire first time
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_TRUE(t1->is_running());

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Second time - max restart limit reached.
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_FALSE(t1->is_running());

  // Third time - should not fire.
  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(5000));
  EXPECT_FALSE(t1->is_running());
}

TEST_F(TimerTest, TimerWithTwoRestart) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kFixed,
                   /*max_restart=*/2));

  EXPECT_CALL(on_expired_, Call).Times(0);
  t1->Start();
  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Fire first time
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_TRUE(t1->is_running());

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Second time
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_TRUE(t1->is_running());

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Third time
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_FALSE(t1->is_running());
}

TEST_F(TimerTest, TimerWithExponentialBackoff) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kExponential));

  t1->Start();

  // Fire first time at 5 seconds
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(5000));

  // Second time at 5*2^1 = 10 seconds later.
  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(9000));
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));

  // Third time at 5*2^2 = 20 seconds later.
  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(19000));
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));

  // Fourth time at 5*2^3 = 40 seconds later.
  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(39000));
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
}

TEST_F(TimerTest, StartTimerWillStopAndStart) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kExponential));

  t1->Start();

  AdvanceTimeAndRunTimers(DurationMs(3000));

  t1->Start();

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(2000));

  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(3000));
}

TEST_F(TimerTest, ExpirationCounterWillResetIfStopped) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kExponential));

  t1->Start();

  // Fire first time at 5 seconds
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(5000));
  EXPECT_EQ(t1->expiration_count(), 1);

  // Second time at 5*2^1 = 10 seconds later.
  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(9000));
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_EQ(t1->expiration_count(), 2);

  t1->Start();
  EXPECT_EQ(t1->expiration_count(), 0);

  // Third time at 5*2^0 = 5 seconds later.
  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(4000));
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_EQ(t1->expiration_count(), 1);
}

TEST_F(TimerTest, StopTimerWillMakeItNotExpire) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kExponential));

  t1->Start();
  EXPECT_TRUE(t1->is_running());

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(4000));
  t1->Stop();
  EXPECT_FALSE(t1->is_running());

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(1000));
}

TEST_F(TimerTest, ReturningNewDurationWhenExpired) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(5000), TimerBackoffAlgorithm::kFixed));

  EXPECT_CALL(on_expired_, Call).Times(0);
  t1->Start();
  EXPECT_EQ(t1->duration(), DurationMs(5000));

  AdvanceTimeAndRunTimers(DurationMs(4000));

  // Fire first time
  EXPECT_CALL(on_expired_, Call).WillOnce(Return(DurationMs(2000)));
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_EQ(t1->duration(), DurationMs(2000));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(1000));

  // Second time
  EXPECT_CALL(on_expired_, Call).WillOnce(Return(DurationMs(10000)));
  AdvanceTimeAndRunTimers(DurationMs(1000));
  EXPECT_EQ(t1->duration(), DurationMs(10000));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(9000));
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1000));
}

TEST_F(TimerTest, TimersHaveMaximumDuration) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(1000), TimerBackoffAlgorithm::kExponential));

  t1->set_duration(DurationMs(2 * *Timer::kMaxTimerDuration));
  EXPECT_EQ(t1->duration(), Timer::kMaxTimerDuration);
}

TEST_F(TimerTest, TimersHaveMaximumBackoffDuration) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(1000), TimerBackoffAlgorithm::kExponential));

  t1->Start();

  int max_exponent = static_cast<int>(log2(*Timer::kMaxTimerDuration / 1000));
  for (int i = 0; i < max_exponent; ++i) {
    EXPECT_CALL(on_expired_, Call).Times(1);
    AdvanceTimeAndRunTimers(DurationMs(1000 * (1 << i)));
  }

  // Reached the maximum duration.
  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(Timer::kMaxTimerDuration);

  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(Timer::kMaxTimerDuration);

  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(Timer::kMaxTimerDuration);

  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(Timer::kMaxTimerDuration);
}

TEST_F(TimerTest, TimerCanBeStartedFromWithinExpirationHandler) {
  std::unique_ptr<Timer> t1 = manager_.CreateTimer(
      "t1", on_expired_.AsStdFunction(),
      TimerOptions(DurationMs(1000), TimerBackoffAlgorithm::kFixed));

  t1->Start();

  // Start a timer, but don't return any new duration in callback.
  EXPECT_CALL(on_expired_, Call).WillOnce([&]() {
    EXPECT_TRUE(t1->is_running());
    t1->set_duration(DurationMs(5000));
    t1->Start();
    return absl::nullopt;
  });
  AdvanceTimeAndRunTimers(DurationMs(1000));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(4999));

  // Start a timer, and return any new duration in callback.
  EXPECT_CALL(on_expired_, Call).WillOnce([&]() {
    EXPECT_TRUE(t1->is_running());
    t1->set_duration(DurationMs(5000));
    t1->Start();
    return absl::make_optional(DurationMs(8000));
  });
  AdvanceTimeAndRunTimers(DurationMs(1));

  EXPECT_CALL(on_expired_, Call).Times(0);
  AdvanceTimeAndRunTimers(DurationMs(7999));

  EXPECT_CALL(on_expired_, Call).Times(1);
  AdvanceTimeAndRunTimers(DurationMs(1));
}

}  // namespace
}  // namespace dcsctp
