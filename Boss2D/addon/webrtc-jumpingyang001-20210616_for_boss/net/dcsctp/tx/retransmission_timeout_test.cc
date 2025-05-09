/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__tx__retransmission_timeout_h //original-code:"net/dcsctp/tx/retransmission_timeout.h"

#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_options_h //original-code:"net/dcsctp/public/dcsctp_options.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace dcsctp {
namespace {

constexpr DurationMs kMaxRtt = DurationMs(8'000);
constexpr DurationMs kInitialRto = DurationMs(200);
constexpr DurationMs kMaxRto = DurationMs(800);
constexpr DurationMs kMinRto = DurationMs(120);

DcSctpOptions MakeOptions() {
  DcSctpOptions options;
  options.rtt_max = kMaxRtt;
  options.rto_initial = kInitialRto;
  options.rto_max = kMaxRto;
  options.rto_min = kMinRto;
  return options;
}

TEST(RetransmissionTimeoutTest, HasValidInitialRto) {
  RetransmissionTimeout rto_(MakeOptions());
  EXPECT_EQ(rto_.rto(), kInitialRto);
}

TEST(RetransmissionTimeoutTest, NegativeValuesDoNotAffectRTO) {
  RetransmissionTimeout rto_(MakeOptions());
  // Initial negative value
  rto_.ObserveRTT(DurationMs(-10));
  EXPECT_EQ(rto_.rto(), kInitialRto);
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 372);
  // Subsequent negative value
  rto_.ObserveRTT(DurationMs(-10));
  EXPECT_EQ(*rto_.rto(), 372);
}

TEST(RetransmissionTimeoutTest, TooLargeValuesDoNotAffectRTO) {
  RetransmissionTimeout rto_(MakeOptions());
  // Initial too large value
  rto_.ObserveRTT(kMaxRtt + DurationMs(100));
  EXPECT_EQ(rto_.rto(), kInitialRto);
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 372);
  // Subsequent too large value
  rto_.ObserveRTT(kMaxRtt + DurationMs(100));
  EXPECT_EQ(*rto_.rto(), 372);
}

TEST(RetransmissionTimeoutTest, WillNeverGoBelowMinimumRto) {
  RetransmissionTimeout rto_(MakeOptions());
  for (int i = 0; i < 1000; ++i) {
    rto_.ObserveRTT(DurationMs(1));
  }
  EXPECT_GE(rto_.rto(), kMinRto);
}

TEST(RetransmissionTimeoutTest, WillNeverGoAboveMaximumRto) {
  RetransmissionTimeout rto_(MakeOptions());
  for (int i = 0; i < 1000; ++i) {
    rto_.ObserveRTT(kMaxRtt - DurationMs(1));
    // Adding jitter, which would make it RTO be well above RTT.
    rto_.ObserveRTT(kMaxRtt - DurationMs(100));
  }
  EXPECT_LE(rto_.rto(), kMaxRto);
}

TEST(RetransmissionTimeoutTest, CalculatesRtoForStableRtt) {
  RetransmissionTimeout rto_(MakeOptions());
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 372);
  rto_.ObserveRTT(DurationMs(128));
  EXPECT_EQ(*rto_.rto(), 314);
  rto_.ObserveRTT(DurationMs(123));
  EXPECT_EQ(*rto_.rto(), 268);
  rto_.ObserveRTT(DurationMs(125));
  EXPECT_EQ(*rto_.rto(), 233);
  rto_.ObserveRTT(DurationMs(127));
  EXPECT_EQ(*rto_.rto(), 208);
}

TEST(RetransmissionTimeoutTest, CalculatesRtoForUnstableRtt) {
  RetransmissionTimeout rto_(MakeOptions());
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 372);
  rto_.ObserveRTT(DurationMs(402));
  EXPECT_EQ(*rto_.rto(), 622);
  rto_.ObserveRTT(DurationMs(728));
  EXPECT_EQ(*rto_.rto(), 800);
  rto_.ObserveRTT(DurationMs(89));
  EXPECT_EQ(*rto_.rto(), 800);
  rto_.ObserveRTT(DurationMs(126));
  EXPECT_EQ(*rto_.rto(), 800);
}

TEST(RetransmissionTimeoutTest, WillStabilizeAfterAWhile) {
  RetransmissionTimeout rto_(MakeOptions());
  rto_.ObserveRTT(DurationMs(124));
  rto_.ObserveRTT(DurationMs(402));
  rto_.ObserveRTT(DurationMs(728));
  rto_.ObserveRTT(DurationMs(89));
  rto_.ObserveRTT(DurationMs(126));
  EXPECT_EQ(*rto_.rto(), 800);
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 800);
  rto_.ObserveRTT(DurationMs(122));
  EXPECT_EQ(*rto_.rto(), 709);
  rto_.ObserveRTT(DurationMs(123));
  EXPECT_EQ(*rto_.rto(), 630);
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 561);
  rto_.ObserveRTT(DurationMs(122));
  EXPECT_EQ(*rto_.rto(), 504);
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 453);
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 409);
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 372);
  rto_.ObserveRTT(DurationMs(124));
  EXPECT_EQ(*rto_.rto(), 339);
}

TEST(RetransmissionTimeoutTest, WillAlwaysStayAboveRTT) {
  // In simulations, it's quite common to have a very stable RTT, and having an
  // RTO at the same value will cause issues as expiry timers will be scheduled
  // to be expire exactly when a packet is supposed to arrive. The RTO must be
  // larger than the RTT. In non-simulated environments, this is a non-issue as
  // any jitter will increase the RTO.
  RetransmissionTimeout rto_(MakeOptions());

  for (int i = 0; i < 100; ++i) {
    rto_.ObserveRTT(DurationMs(124));
  }
  EXPECT_GT(*rto_.rto(), 124);
}

}  // namespace
}  // namespace dcsctp
