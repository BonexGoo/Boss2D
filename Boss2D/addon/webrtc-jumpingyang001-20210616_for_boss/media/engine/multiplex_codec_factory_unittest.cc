/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__engine__multiplex_codec_factory_h //original-code:"media/engine/multiplex_codec_factory.h"

#include <utility>

#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_media__base__media_constants_h //original-code:"media/base/media_constants.h"
#include BOSS_WEBRTC_U_media__engine__internal_decoder_factory_h //original-code:"media/engine/internal_decoder_factory.h"
#include BOSS_WEBRTC_U_media__engine__internal_encoder_factory_h //original-code:"media/engine/internal_encoder_factory.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

TEST(MultiplexDecoderFactory, CreateVideoDecoder) {
  std::unique_ptr<VideoDecoderFactory> internal_factory(
      new InternalDecoderFactory());
  MultiplexDecoderFactory factory(std::move(internal_factory));
  std::unique_ptr<VideoDecoder> decoder =
      factory.CreateVideoDecoder(SdpVideoFormat(
          cricket::kMultiplexCodecName,
          {{cricket::kCodecParamAssociatedCodecName, cricket::kVp9CodecName}}));
  EXPECT_TRUE(decoder);
}

TEST(MultiplexEncoderFactory, CreateVideoEncoder) {
  std::unique_ptr<VideoEncoderFactory> internal_factory(
      new InternalEncoderFactory());
  MultiplexEncoderFactory factory(std::move(internal_factory));
  std::unique_ptr<VideoEncoder> encoder =
      factory.CreateVideoEncoder(SdpVideoFormat(
          cricket::kMultiplexCodecName,
          {{cricket::kCodecParamAssociatedCodecName, cricket::kVp9CodecName}}));
  EXPECT_TRUE(encoder);
}

}  // namespace webrtc
