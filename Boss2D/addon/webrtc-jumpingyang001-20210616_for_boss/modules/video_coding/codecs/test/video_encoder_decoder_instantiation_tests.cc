/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_factory_h //original-code:"api/video_codecs/video_encoder_factory.h"
#if defined(WEBRTC_ANDROID)
#include "modules/video_coding/codecs/test/android_codec_factory_helper.h"
#elif defined(WEBRTC_IOS)
#include "modules/video_coding/codecs/test/objc_codec_factory_helper.h"
#endif
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_test__video_codec_settings_h //original-code:"test/video_codec_settings.h"

namespace webrtc {
namespace test {

namespace {
const VideoEncoder::Capabilities kCapabilities(false);

int32_t InitEncoder(VideoCodecType codec_type, VideoEncoder* encoder) {
  VideoCodec codec;
  CodecSettings(codec_type, &codec);
  codec.width = 640;
  codec.height = 480;
  codec.maxFramerate = 30;
  RTC_CHECK(encoder);
  return encoder->InitEncode(
      &codec, VideoEncoder::Settings(kCapabilities, 1 /* number_of_cores */,
                                     1200 /* max_payload_size */));
}

int32_t InitDecoder(VideoCodecType codec_type, VideoDecoder* decoder) {
  VideoCodec codec;
  CodecSettings(codec_type, &codec);
  codec.width = 640;
  codec.height = 480;
  codec.maxFramerate = 30;
  RTC_CHECK(decoder);
  return decoder->InitDecode(&codec, 1 /* number_of_cores */);
}

}  // namespace

class VideoEncoderDecoderInstantiationTest
    : public ::testing::Test,
      public ::testing::WithParamInterface<::testing::tuple<int, int>> {
 protected:
  VideoEncoderDecoderInstantiationTest()
      : vp8_format_("VP8"),
        vp9_format_("VP9"),
        h264cbp_format_("H264"),
        num_encoders_(::testing::get<0>(GetParam())),
        num_decoders_(::testing::get<1>(GetParam())) {
#if defined(WEBRTC_ANDROID)
    InitializeAndroidObjects();
    encoder_factory_ = CreateAndroidEncoderFactory();
    decoder_factory_ = CreateAndroidDecoderFactory();
#elif defined(WEBRTC_IOS)
    encoder_factory_ = CreateObjCEncoderFactory();
    decoder_factory_ = CreateObjCDecoderFactory();
#else
    RTC_NOTREACHED() << "Only support Android and iOS.";
#endif
  }

  ~VideoEncoderDecoderInstantiationTest() {
    for (auto& encoder : encoders_) {
      encoder->Release();
    }
    for (auto& decoder : decoders_) {
      decoder->Release();
    }
  }

  const SdpVideoFormat vp8_format_;
  const SdpVideoFormat vp9_format_;
  const SdpVideoFormat h264cbp_format_;
  std::unique_ptr<VideoEncoderFactory> encoder_factory_;
  std::unique_ptr<VideoDecoderFactory> decoder_factory_;

  const int num_encoders_;
  const int num_decoders_;
  std::vector<std::unique_ptr<VideoEncoder>> encoders_;
  std::vector<std::unique_ptr<VideoDecoder>> decoders_;
};

INSTANTIATE_TEST_SUITE_P(MultipleEncoders,
                         VideoEncoderDecoderInstantiationTest,
                         ::testing::Combine(::testing::Range(1, 4),
                                            ::testing::Range(1, 2)));

INSTANTIATE_TEST_SUITE_P(MultipleDecoders,
                         VideoEncoderDecoderInstantiationTest,
                         ::testing::Combine(::testing::Range(1, 2),
                                            ::testing::Range(1, 9)));

INSTANTIATE_TEST_SUITE_P(MultipleEncodersDecoders,
                         VideoEncoderDecoderInstantiationTest,
                         ::testing::Combine(::testing::Range(1, 4),
                                            ::testing::Range(1, 9)));

// TODO(brandtr): Check that the factories actually support the codecs before
// trying to instantiate. Currently, we will just crash with a Java exception
// if the factory does not support the codec.
TEST_P(VideoEncoderDecoderInstantiationTest, DISABLED_InstantiateVp8Codecs) {
  for (int i = 0; i < num_encoders_; ++i) {
    std::unique_ptr<VideoEncoder> encoder =
        encoder_factory_->CreateVideoEncoder(vp8_format_);
    EXPECT_EQ(0, InitEncoder(kVideoCodecVP8, encoder.get()));
    encoders_.emplace_back(std::move(encoder));
  }

  for (int i = 0; i < num_decoders_; ++i) {
    std::unique_ptr<VideoDecoder> decoder =
        decoder_factory_->CreateVideoDecoder(vp8_format_);
    EXPECT_EQ(0, InitDecoder(kVideoCodecVP8, decoder.get()));
    decoders_.emplace_back(std::move(decoder));
  }
}

TEST_P(VideoEncoderDecoderInstantiationTest,
       DISABLED_InstantiateH264CBPCodecs) {
  for (int i = 0; i < num_encoders_; ++i) {
    std::unique_ptr<VideoEncoder> encoder =
        encoder_factory_->CreateVideoEncoder(h264cbp_format_);
    EXPECT_EQ(0, InitEncoder(kVideoCodecH264, encoder.get()));
    encoders_.emplace_back(std::move(encoder));
  }

  for (int i = 0; i < num_decoders_; ++i) {
    std::unique_ptr<VideoDecoder> decoder =
        decoder_factory_->CreateVideoDecoder(h264cbp_format_);
    EXPECT_EQ(0, InitDecoder(kVideoCodecH264, decoder.get()));
    decoders_.emplace_back(std::move(decoder));
  }
}

}  // namespace test
}  // namespace webrtc
