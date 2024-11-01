/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__platform_thread_h //original-code:"rtc_base/platform_thread.h"

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_system_wrappers__include__sleep_h //original-code:"system_wrappers/include/sleep.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace rtc {

TEST(PlatformThreadTest, DefaultConstructedIsEmpty) {
  PlatformThread thread;
  EXPECT_EQ(thread.GetHandle(), absl::nullopt);
  EXPECT_TRUE(thread.empty());
}

TEST(PlatformThreadTest, StartFinalize) {
  PlatformThread thread = PlatformThread::SpawnJoinable([] {}, "1");
  EXPECT_NE(thread.GetHandle(), absl::nullopt);
  EXPECT_FALSE(thread.empty());
  thread.Finalize();
  EXPECT_TRUE(thread.empty());
  thread = PlatformThread::SpawnDetached([] {}, "2");
  EXPECT_FALSE(thread.empty());
  thread.Finalize();
  EXPECT_TRUE(thread.empty());
}

TEST(PlatformThreadTest, MovesEmpty) {
  PlatformThread thread1;
  PlatformThread thread2 = std::move(thread1);
  EXPECT_TRUE(thread1.empty());
  EXPECT_TRUE(thread2.empty());
}

TEST(PlatformThreadTest, MovesHandles) {
  PlatformThread thread1 = PlatformThread::SpawnJoinable([] {}, "1");
  PlatformThread thread2 = std::move(thread1);
  EXPECT_TRUE(thread1.empty());
  EXPECT_FALSE(thread2.empty());
  thread1 = PlatformThread::SpawnDetached([] {}, "2");
  thread2 = std::move(thread1);
  EXPECT_TRUE(thread1.empty());
  EXPECT_FALSE(thread2.empty());
}

TEST(PlatformThreadTest,
     TwoThreadHandlesAreDifferentWhenStartedAndEqualWhenJoined) {
  PlatformThread thread1 = PlatformThread();
  PlatformThread thread2 = PlatformThread();
  EXPECT_EQ(thread1.GetHandle(), thread2.GetHandle());
  thread1 = PlatformThread::SpawnJoinable([] {}, "1");
  thread2 = PlatformThread::SpawnJoinable([] {}, "2");
  EXPECT_NE(thread1.GetHandle(), thread2.GetHandle());
  thread1.Finalize();
  EXPECT_NE(thread1.GetHandle(), thread2.GetHandle());
  thread2.Finalize();
  EXPECT_EQ(thread1.GetHandle(), thread2.GetHandle());
}

TEST(PlatformThreadTest, RunFunctionIsCalled) {
  bool flag = false;
  PlatformThread::SpawnJoinable([&] { flag = true; }, "T");
  EXPECT_TRUE(flag);
}

TEST(PlatformThreadTest, JoinsThread) {
  // This test flakes if there are problems with the join implementation.
  rtc::Event event;
  PlatformThread::SpawnJoinable([&] { event.Set(); }, "T");
  EXPECT_TRUE(event.Wait(/*give_up_after_ms=*/0));
}

TEST(PlatformThreadTest, StopsBeforeDetachedThreadExits) {
  // This test flakes if there are problems with the detached thread
  // implementation.
  bool flag = false;
  rtc::Event thread_started;
  rtc::Event thread_continue;
  rtc::Event thread_exiting;
  PlatformThread::SpawnDetached(
      [&] {
        thread_started.Set();
        thread_continue.Wait(Event::kForever);
        flag = true;
        thread_exiting.Set();
      },
      "T");
  thread_started.Wait(Event::kForever);
  EXPECT_FALSE(flag);
  thread_continue.Set();
  thread_exiting.Wait(Event::kForever);
  EXPECT_TRUE(flag);
}

}  // namespace rtc
