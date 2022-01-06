/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__test__create_simulcast_test_fixture_h //original-code:"api/test/create_simulcast_test_fixture.h"

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_api__test__simulcast_test_fixture_h //original-code:"api/test/simulcast_test_fixture.h"
#include BOSS_WEBRTC_U_modules__video_coding__utility__simulcast_test_fixture_impl_h //original-code:"modules/video_coding/utility/simulcast_test_fixture_impl.h"

namespace webrtc {
namespace test {

std::unique_ptr<SimulcastTestFixture> CreateSimulcastTestFixture(
    std::unique_ptr<VideoEncoderFactory> encoder_factory,
    std::unique_ptr<VideoDecoderFactory> decoder_factory,
    SdpVideoFormat video_format) {
  return std::make_unique<SimulcastTestFixtureImpl>(
      std::move(encoder_factory), std::move(decoder_factory), video_format);
}

}  // namespace test
}  // namespace webrtc
