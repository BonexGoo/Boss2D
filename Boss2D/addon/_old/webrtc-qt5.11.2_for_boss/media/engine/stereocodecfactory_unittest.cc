/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__engine__stereocodecfactory_h //original-code:"media/engine/stereocodecfactory.h"

#include <utility>

#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_media__base__mediaconstants_h //original-code:"media/base/mediaconstants.h"
#include BOSS_WEBRTC_U_media__engine__internaldecoderfactory_h //original-code:"media/engine/internaldecoderfactory.h"
#include BOSS_WEBRTC_U_media__engine__internalencoderfactory_h //original-code:"media/engine/internalencoderfactory.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

TEST(StereoDecoderFactory, CreateVideoDecoder) {
  std::unique_ptr<VideoDecoderFactory> internal_factory(
      new InternalDecoderFactory());
  StereoDecoderFactory factory(std::move(internal_factory));
  std::unique_ptr<VideoDecoder> decoder =
      factory.CreateVideoDecoder(SdpVideoFormat(
          cricket::kStereoCodecName,
          {{cricket::kCodecParamAssociatedCodecName, cricket::kVp9CodecName}}));
  EXPECT_TRUE(decoder);
}

TEST(StereoEncoderFactory, CreateVideoEncoder) {
  std::unique_ptr<VideoEncoderFactory> internal_factory(
      new InternalEncoderFactory());
  StereoEncoderFactory factory(std::move(internal_factory));
  std::unique_ptr<VideoEncoder> encoder =
      factory.CreateVideoEncoder(SdpVideoFormat(
          cricket::kStereoCodecName,
          {{cricket::kCodecParamAssociatedCodecName, cricket::kVp9CodecName}}));
  EXPECT_TRUE(encoder);
}

}  // namespace webrtc
