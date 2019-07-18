/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__test__create_videocodec_test_fixture_h //original-code:"api/test/create_videocodec_test_fixture.h"

#include <memory>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__test__videocodec_test_fixture_h //original-code:"api/test/videocodec_test_fixture.h"
#include "modules/video_coding/codecs/test/videocodec_test_fixture_impl.h"

namespace webrtc {
namespace test {

using Config = VideoCodecTestFixture::Config;

std::unique_ptr<VideoCodecTestFixture> CreateVideoCodecTestFixture(
    const Config& config) {
  return absl::make_unique<VideoCodecTestFixtureImpl>(config);
}

std::unique_ptr<VideoCodecTestFixture> CreateVideoCodecTestFixture(
    const Config& config,
    std::unique_ptr<VideoDecoderFactory> decoder_factory,
    std::unique_ptr<VideoEncoderFactory> encoder_factory) {
  return absl::make_unique<VideoCodecTestFixtureImpl>(
      config, std::move(decoder_factory), std::move(encoder_factory));
}

}  // namespace test
}  // namespace webrtc
