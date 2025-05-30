/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__test__create_videocodec_test_fixture_h //original-code:"api/test/create_videocodec_test_fixture.h"
#include "api/test/video/function_video_encoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_media__base__media_constants_h //original-code:"media/base/media_constants.h"
#include BOSS_WEBRTC_U_media__engine__internal_decoder_factory_h //original-code:"media/engine/internal_decoder_factory.h"
#include BOSS_WEBRTC_U_media__engine__internal_encoder_factory_h //original-code:"media/engine/internal_encoder_factory.h"
#include BOSS_WEBRTC_U_media__engine__simulcast_encoder_adapter_h //original-code:"media/engine/simulcast_encoder_adapter.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include "test/testsupport/file_utils.h"

namespace webrtc {
namespace test {
namespace {
// Test clips settings.
constexpr int kCifWidth = 352;
constexpr int kCifHeight = 288;
constexpr int kNumFramesLong = 300;

VideoCodecTestFixture::Config CreateConfig(std::string filename) {
  VideoCodecTestFixture::Config config;
  config.filename = filename;
  config.filepath = ResourcePath(config.filename, "yuv");
  config.num_frames = kNumFramesLong;
  config.use_single_core = true;
  return config;
}

TEST(VideoCodecTestLibaom, HighBitrateAV1) {
  auto config = CreateConfig("foreman_cif");
  config.SetCodecSettings(cricket::kAv1CodecName, 1, 1, 1, false, true, true,
                          kCifWidth, kCifHeight);
  config.codec_settings.SetScalabilityMode("NONE");
  config.num_frames = kNumFramesLong;
  auto fixture = CreateVideoCodecTestFixture(config);

  std::vector<RateProfile> rate_profiles = {{500, 30, 0}};

  std::vector<RateControlThresholds> rc_thresholds = {
      {12, 1, 0, 1, 0.3, 0.1, 0, 1}};

  std::vector<QualityThresholds> quality_thresholds = {{37, 34, 0.94, 0.915}};

  fixture->RunTest(rate_profiles, &rc_thresholds, &quality_thresholds, nullptr);
}

TEST(VideoCodecTestLibaom, VeryLowBitrateAV1) {
  auto config = CreateConfig("foreman_cif");
  config.SetCodecSettings(cricket::kAv1CodecName, 1, 1, 1, false, true, true,
                          kCifWidth, kCifHeight);
  config.codec_settings.SetScalabilityMode("NONE");
  auto fixture = CreateVideoCodecTestFixture(config);

  std::vector<RateProfile> rate_profiles = {{50, 30, 0}};

  std::vector<RateControlThresholds> rc_thresholds = {
      {15, 8, 75, 2, 2, 2, 2, 1}};

  std::vector<QualityThresholds> quality_thresholds = {{28, 25, 0.70, 0.60}};

  fixture->RunTest(rate_profiles, &rc_thresholds, &quality_thresholds, nullptr);
}

#if !defined(WEBRTC_ANDROID)
constexpr int kHdWidth = 1280;
constexpr int kHdHeight = 720;
TEST(VideoCodecTestLibaom, HdAV1) {
  auto config = CreateConfig("ConferenceMotion_1280_720_50");
  config.SetCodecSettings(cricket::kAv1CodecName, 1, 1, 1, false, true, true,
                          kHdWidth, kHdHeight);
  config.codec_settings.SetScalabilityMode("NONE");
  config.num_frames = kNumFramesLong;
  auto fixture = CreateVideoCodecTestFixture(config);

  std::vector<RateProfile> rate_profiles = {{1000, 50, 0}};

  std::vector<RateControlThresholds> rc_thresholds = {
      {13, 3, 0, 1, 0.3, 0.1, 0, 1}};

  std::vector<QualityThresholds> quality_thresholds = {{36, 31.7, 0.93, 0.87}};

  fixture->RunTest(rate_profiles, &rc_thresholds, &quality_thresholds, nullptr);
}
#endif

}  // namespace
}  // namespace test
}  // namespace webrtc
