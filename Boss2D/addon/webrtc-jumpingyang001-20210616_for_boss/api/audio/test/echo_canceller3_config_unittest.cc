/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"

#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_json_h //original-code:"api/audio/echo_canceller3_config_json.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

TEST(EchoCanceller3Config, ValidConfigIsNotModified) {
  EchoCanceller3Config config;
  EXPECT_TRUE(EchoCanceller3Config::Validate(&config));
  EchoCanceller3Config default_config;
  EXPECT_EQ(Aec3ConfigToJsonString(config),
            Aec3ConfigToJsonString(default_config));
}

TEST(EchoCanceller3Config, InvalidConfigIsCorrected) {
  // Change a parameter and validate.
  EchoCanceller3Config config;
  config.echo_model.min_noise_floor_power = -1600000.f;
  EXPECT_FALSE(EchoCanceller3Config::Validate(&config));
  EXPECT_GE(config.echo_model.min_noise_floor_power, 0.f);
  // Verify remaining parameters are unchanged.
  EchoCanceller3Config default_config;
  config.echo_model.min_noise_floor_power =
      default_config.echo_model.min_noise_floor_power;
  EXPECT_EQ(Aec3ConfigToJsonString(config),
            Aec3ConfigToJsonString(default_config));
}

TEST(EchoCanceller3Config, ValidatedConfigsAreValid) {
  EchoCanceller3Config config;
  config.delay.down_sampling_factor = 983;
  EXPECT_FALSE(EchoCanceller3Config::Validate(&config));
  EXPECT_TRUE(EchoCanceller3Config::Validate(&config));
}
}  // namespace webrtc
