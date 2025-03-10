/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__engine__internal_decoder_factory_h //original-code:"media/engine/internal_decoder_factory.h"

#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__vp9_profile_h //original-code:"api/video_codecs/vp9_profile.h"
#include BOSS_WEBRTC_U_media__base__media_constants_h //original-code:"media/base/media_constants.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__av1__libaom_av1_decoder_h //original-code:"modules/video_coding/codecs/av1/libaom_av1_decoder.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

using ::testing::Contains;
using ::testing::Field;
using ::testing::Not;

TEST(InternalDecoderFactory, TestVP8) {
  InternalDecoderFactory factory;
  std::unique_ptr<VideoDecoder> decoder =
      factory.CreateVideoDecoder(SdpVideoFormat(cricket::kVp8CodecName));
  EXPECT_TRUE(decoder);
}

#ifdef RTC_ENABLE_VP9
TEST(InternalDecoderFactory, TestVP9Profile0) {
  InternalDecoderFactory factory;
  std::unique_ptr<VideoDecoder> decoder =
      factory.CreateVideoDecoder(SdpVideoFormat(
          cricket::kVp9CodecName,
          {{kVP9FmtpProfileId, VP9ProfileToString(VP9Profile::kProfile0)}}));
  EXPECT_TRUE(decoder);
}

TEST(InternalDecoderFactory, TestVP9Profile1) {
  InternalDecoderFactory factory;
  std::unique_ptr<VideoDecoder> decoder =
      factory.CreateVideoDecoder(SdpVideoFormat(
          cricket::kVp9CodecName,
          {{kVP9FmtpProfileId, VP9ProfileToString(VP9Profile::kProfile1)}}));
  EXPECT_TRUE(decoder);
}
#endif  // RTC_ENABLE_VP9

TEST(InternalDecoderFactory, Av1) {
  InternalDecoderFactory factory;
  if (kIsLibaomAv1DecoderSupported) {
    EXPECT_THAT(factory.GetSupportedFormats(),
                Contains(Field(&SdpVideoFormat::name, "AV1X")));
    EXPECT_TRUE(factory.CreateVideoDecoder(SdpVideoFormat("AV1X")));
  } else {
    EXPECT_THAT(factory.GetSupportedFormats(),
                Not(Contains(Field(&SdpVideoFormat::name, "AV1X"))));
  }
}

}  // namespace webrtc
