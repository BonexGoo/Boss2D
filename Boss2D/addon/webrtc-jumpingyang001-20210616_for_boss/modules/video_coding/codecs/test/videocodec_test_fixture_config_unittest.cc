/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stddef.h>

#include BOSS_WEBRTC_U_api__test__videocodec_test_fixture_h //original-code:"api/test/videocodec_test_fixture.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_test__video_codec_settings_h //original-code:"test/video_codec_settings.h"

using ::testing::ElementsAre;

namespace webrtc {
namespace test {

using Config = VideoCodecTestFixture::Config;

namespace {
const size_t kNumTemporalLayers = 2;
}  // namespace

TEST(Config, NumberOfCoresWithUseSingleCore) {
  Config config;
  config.use_single_core = true;
  EXPECT_EQ(1u, config.NumberOfCores());
}

TEST(Config, NumberOfCoresWithoutUseSingleCore) {
  Config config;
  config.use_single_core = false;
  EXPECT_GE(config.NumberOfCores(), 1u);
}

TEST(Config, NumberOfTemporalLayersIsOne) {
  Config config;
  webrtc::test::CodecSettings(kVideoCodecH264, &config.codec_settings);
  EXPECT_EQ(1u, config.NumberOfTemporalLayers());
}

TEST(Config, NumberOfTemporalLayers_Vp8) {
  Config config;
  webrtc::test::CodecSettings(kVideoCodecVP8, &config.codec_settings);
  config.codec_settings.VP8()->numberOfTemporalLayers = kNumTemporalLayers;
  EXPECT_EQ(kNumTemporalLayers, config.NumberOfTemporalLayers());
}

TEST(Config, NumberOfTemporalLayers_Vp9) {
  Config config;
  webrtc::test::CodecSettings(kVideoCodecVP9, &config.codec_settings);
  config.codec_settings.VP9()->numberOfTemporalLayers = kNumTemporalLayers;
  EXPECT_EQ(kNumTemporalLayers, config.NumberOfTemporalLayers());
}

}  // namespace test
}  // namespace webrtc