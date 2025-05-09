/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include <memory>

#include BOSS_WEBRTC_U_api__transport__network_types_h //original-code:"api/transport/network_types.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__probe_controller_h //original-code:"modules/congestion_controller/goog_cc/probe_controller.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"
#include "test/gmock.h"
#include "test/gtest.h"

using testing::_;
using testing::AtLeast;
using testing::Field;
using testing::Matcher;
using testing::NiceMock;
using testing::Return;

namespace webrtc {
namespace test {

namespace {

constexpr int kMinBitrateBps = 100;
constexpr int kStartBitrateBps = 300;
constexpr int kMaxBitrateBps = 10000;

constexpr int kExponentialProbingTimeoutMs = 5000;

constexpr int kAlrProbeInterval = 5000;
constexpr int kAlrEndedTimeoutMs = 3000;
constexpr int kBitrateDropTimeoutMs = 5000;
}  // namespace

class ProbeControllerTest : public ::testing::Test {
 protected:
  ProbeControllerTest() : clock_(100000000L) {
    probe_controller_.reset(new ProbeController());
  }
  ~ProbeControllerTest() override {}

  std::vector<ProbeClusterConfig> SetNetworkAvailable(bool available) {
    NetworkAvailability msg;
    msg.at_time = Timestamp::ms(NowMs());
    msg.network_available = available;
    return probe_controller_->OnNetworkAvailability(msg);
  }

  int64_t NowMs() { return clock_.TimeInMilliseconds(); }

  SimulatedClock clock_;
  std::unique_ptr<ProbeController> probe_controller_;
};

TEST_F(ProbeControllerTest, InitiatesProbingAtStart) {
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  EXPECT_GE(probes.size(), 2u);
}

TEST_F(ProbeControllerTest, ProbeOnlyWhenNetworkIsUp) {
  SetNetworkAvailable(false);
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  EXPECT_EQ(probes.size(), 0u);
  probes = SetNetworkAvailable(true);
  EXPECT_GE(probes.size(), 2u);
}

TEST_F(ProbeControllerTest, InitiatesProbingOnMaxBitrateIncrease) {
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  // Long enough to time out exponential probing.
  clock_.AdvanceTimeMilliseconds(kExponentialProbingTimeoutMs);
  probes = probe_controller_->SetEstimatedBitrate(kStartBitrateBps, NowMs());
  probes = probe_controller_->Process(NowMs());
  probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                          kMaxBitrateBps + 100, NowMs());
  EXPECT_EQ(probes[0].target_data_rate.bps(), kMaxBitrateBps + 100);
}

TEST_F(ProbeControllerTest, InitiatesProbingOnMaxBitrateIncreaseAtMaxBitrate) {
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  // Long enough to time out exponential probing.
  clock_.AdvanceTimeMilliseconds(kExponentialProbingTimeoutMs);
  probes = probe_controller_->SetEstimatedBitrate(kStartBitrateBps, NowMs());
  probes = probe_controller_->Process(NowMs());
  probes = probe_controller_->SetEstimatedBitrate(kMaxBitrateBps, NowMs());
  probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                          kMaxBitrateBps + 100, NowMs());
  EXPECT_EQ(probes[0].target_data_rate.bps(), kMaxBitrateBps + 100);
}

TEST_F(ProbeControllerTest, TestExponentialProbing) {
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());

  // Repeated probe should only be sent when estimated bitrate climbs above
  // 0.7 * 6 * kStartBitrateBps = 1260.
  probes = probe_controller_->SetEstimatedBitrate(1000, NowMs());
  EXPECT_EQ(probes.size(), 0u);

  probes = probe_controller_->SetEstimatedBitrate(1800, NowMs());
  EXPECT_EQ(probes[0].target_data_rate.bps(), 2 * 1800);
}

TEST_F(ProbeControllerTest, TestExponentialProbingTimeout) {
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  // Advance far enough to cause a time out in waiting for probing result.
  clock_.AdvanceTimeMilliseconds(kExponentialProbingTimeoutMs);
  probes = probe_controller_->Process(NowMs());

  probes = probe_controller_->SetEstimatedBitrate(1800, NowMs());
  EXPECT_EQ(probes.size(), 0u);
}

TEST_F(ProbeControllerTest, RequestProbeInAlr) {
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  EXPECT_GE(probes.size(), 2u);
  probes = probe_controller_->SetEstimatedBitrate(500, NowMs());

  probe_controller_->SetAlrStartTimeMs(clock_.TimeInMilliseconds());
  clock_.AdvanceTimeMilliseconds(kAlrProbeInterval + 1);
  probes = probe_controller_->Process(NowMs());
  probes = probe_controller_->SetEstimatedBitrate(250, NowMs());
  probes = probe_controller_->RequestProbe(NowMs());

  EXPECT_EQ(probes.size(), 1u);
  EXPECT_EQ(probes[0].target_data_rate.bps(), 0.85 * 500);
}

TEST_F(ProbeControllerTest, RequestProbeWhenAlrEndedRecently) {
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  EXPECT_EQ(probes.size(), 2u);
  probes = probe_controller_->SetEstimatedBitrate(500, NowMs());

  probe_controller_->SetAlrStartTimeMs(absl::nullopt);
  clock_.AdvanceTimeMilliseconds(kAlrProbeInterval + 1);
  probes = probe_controller_->Process(NowMs());
  probes = probe_controller_->SetEstimatedBitrate(250, NowMs());
  probe_controller_->SetAlrEndedTimeMs(clock_.TimeInMilliseconds());
  clock_.AdvanceTimeMilliseconds(kAlrEndedTimeoutMs - 1);
  probes = probe_controller_->RequestProbe(NowMs());

  EXPECT_EQ(probes.size(), 1u);
  EXPECT_EQ(probes[0].target_data_rate.bps(), 0.85 * 500);
}

TEST_F(ProbeControllerTest, RequestProbeWhenAlrNotEndedRecently) {
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  EXPECT_EQ(probes.size(), 2u);
  probes = probe_controller_->SetEstimatedBitrate(500, NowMs());

  probe_controller_->SetAlrStartTimeMs(absl::nullopt);
  clock_.AdvanceTimeMilliseconds(kAlrProbeInterval + 1);
  probes = probe_controller_->Process(NowMs());
  probes = probe_controller_->SetEstimatedBitrate(250, NowMs());
  probe_controller_->SetAlrEndedTimeMs(clock_.TimeInMilliseconds());
  clock_.AdvanceTimeMilliseconds(kAlrEndedTimeoutMs + 1);
  probes = probe_controller_->RequestProbe(NowMs());
  EXPECT_EQ(probes.size(), 0u);
}

TEST_F(ProbeControllerTest, RequestProbeWhenBweDropNotRecent) {
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  EXPECT_EQ(probes.size(), 2u);
  probes = probe_controller_->SetEstimatedBitrate(500, NowMs());

  probe_controller_->SetAlrStartTimeMs(clock_.TimeInMilliseconds());
  clock_.AdvanceTimeMilliseconds(kAlrProbeInterval + 1);
  probes = probe_controller_->Process(NowMs());
  probes = probe_controller_->SetEstimatedBitrate(250, NowMs());
  clock_.AdvanceTimeMilliseconds(kBitrateDropTimeoutMs + 1);
  probes = probe_controller_->RequestProbe(NowMs());
  EXPECT_EQ(probes.size(), 0u);
}

TEST_F(ProbeControllerTest, PeriodicProbing) {
  probe_controller_->EnablePeriodicAlrProbing(true);
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  EXPECT_EQ(probes.size(), 2u);
  probes = probe_controller_->SetEstimatedBitrate(500, NowMs());

  int64_t start_time = clock_.TimeInMilliseconds();

  // Expect the controller to send a new probe after 5s has passed.
  probe_controller_->SetAlrStartTimeMs(start_time);
  clock_.AdvanceTimeMilliseconds(5000);
  probes = probe_controller_->Process(NowMs());
  EXPECT_EQ(probes.size(), 1u);
  EXPECT_EQ(probes[0].target_data_rate.bps(), 1000);

  probes = probe_controller_->SetEstimatedBitrate(500, NowMs());

  // The following probe should be sent at 10s into ALR.
  probe_controller_->SetAlrStartTimeMs(start_time);
  clock_.AdvanceTimeMilliseconds(4000);
  probes = probe_controller_->Process(NowMs());
  probes = probe_controller_->SetEstimatedBitrate(500, NowMs());
  EXPECT_EQ(probes.size(), 0u);

  probe_controller_->SetAlrStartTimeMs(start_time);
  clock_.AdvanceTimeMilliseconds(1000);
  probes = probe_controller_->Process(NowMs());
  EXPECT_EQ(probes.size(), 1u);
  probes = probe_controller_->SetEstimatedBitrate(500, NowMs());
  EXPECT_EQ(probes.size(), 0u);
}

TEST_F(ProbeControllerTest, PeriodicProbingAfterReset) {
  probe_controller_.reset(new ProbeController());
  int64_t alr_start_time = clock_.TimeInMilliseconds();

  probe_controller_->SetAlrStartTimeMs(alr_start_time);
  probe_controller_->EnablePeriodicAlrProbing(true);
  auto probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                               kMaxBitrateBps, NowMs());
  probe_controller_->Reset(NowMs());

  clock_.AdvanceTimeMilliseconds(10000);
  probes = probe_controller_->Process(NowMs());
  // Since bitrates are not yet set, no probe is sent event though we are in ALR
  // mode.
  EXPECT_EQ(probes.size(), 0u);

  probes = probe_controller_->SetBitrates(kMinBitrateBps, kStartBitrateBps,
                                          kMaxBitrateBps, NowMs());
  EXPECT_EQ(probes.size(), 2u);

  // Make sure we use |kStartBitrateBps| as the estimated bitrate
  // until SetEstimatedBitrate is called with an updated estimate.
  clock_.AdvanceTimeMilliseconds(10000);
  probes = probe_controller_->Process(NowMs());
  EXPECT_EQ(probes[0].target_data_rate.bps(), kStartBitrateBps * 2);
}

TEST_F(ProbeControllerTest, TestExponentialProbingOverflow) {
  const int64_t kMbpsMultiplier = 1000000;
  auto probes = probe_controller_->SetBitrates(
      kMinBitrateBps, 10 * kMbpsMultiplier, 100 * kMbpsMultiplier, NowMs());
  // Verify that probe bitrate is capped at the specified max bitrate.
  probes =
      probe_controller_->SetEstimatedBitrate(60 * kMbpsMultiplier, NowMs());
  EXPECT_EQ(probes[0].target_data_rate.bps(), 100 * kMbpsMultiplier);
  // Verify that repeated probes aren't sent.
  probes =
      probe_controller_->SetEstimatedBitrate(100 * kMbpsMultiplier, NowMs());
  EXPECT_EQ(probes.size(), 0u);
}

}  // namespace test
}  // namespace webrtc
