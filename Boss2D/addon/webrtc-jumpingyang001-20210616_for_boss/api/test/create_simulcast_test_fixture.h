/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TEST_CREATE_SIMULCAST_TEST_FIXTURE_H_
#define API_TEST_CREATE_SIMULCAST_TEST_FIXTURE_H_

#include <memory>

#include BOSS_WEBRTC_U_api__test__simulcast_test_fixture_h //original-code:"api/test/simulcast_test_fixture.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_factory_h //original-code:"api/video_codecs/video_encoder_factory.h"

namespace webrtc {
namespace test {

std::unique_ptr<SimulcastTestFixture> CreateSimulcastTestFixture(
    std::unique_ptr<VideoEncoderFactory> encoder_factory,
    std::unique_ptr<VideoDecoderFactory> decoder_factory,
    SdpVideoFormat video_format);

}  // namespace test
}  // namespace webrtc

#endif  // API_TEST_CREATE_SIMULCAST_TEST_FIXTURE_H_
