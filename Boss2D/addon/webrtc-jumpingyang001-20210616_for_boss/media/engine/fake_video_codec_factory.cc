/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__engine__fake_video_codec_factory_h //original-code:"media/engine/fake_video_codec_factory.h"

#include <memory>

#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_error_codes_h //original-code:"modules/video_coding/include/video_error_codes.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_test__fake_decoder_h //original-code:"test/fake_decoder.h"
#include BOSS_WEBRTC_U_test__fake_encoder_h //original-code:"test/fake_encoder.h"

namespace {

static const char kFakeCodecFactoryCodecName[] = "FakeCodec";

}  // anonymous namespace

namespace webrtc {

FakeVideoEncoderFactory::FakeVideoEncoderFactory() = default;

// static
std::unique_ptr<VideoEncoder> FakeVideoEncoderFactory::CreateVideoEncoder() {
  return std::make_unique<test::FakeEncoder>(Clock::GetRealTimeClock());
}

std::vector<SdpVideoFormat> FakeVideoEncoderFactory::GetSupportedFormats()
    const {
  return std::vector<SdpVideoFormat>(
      1, SdpVideoFormat(kFakeCodecFactoryCodecName));
}

std::unique_ptr<VideoEncoder> FakeVideoEncoderFactory::CreateVideoEncoder(
    const SdpVideoFormat& format) {
  return std::make_unique<test::FakeEncoder>(Clock::GetRealTimeClock());
}

FakeVideoDecoderFactory::FakeVideoDecoderFactory() = default;

// static
std::unique_ptr<VideoDecoder> FakeVideoDecoderFactory::CreateVideoDecoder() {
  return std::make_unique<test::FakeDecoder>();
}

std::vector<SdpVideoFormat> FakeVideoDecoderFactory::GetSupportedFormats()
    const {
  return std::vector<SdpVideoFormat>(
      1, SdpVideoFormat(kFakeCodecFactoryCodecName));
}

std::unique_ptr<VideoDecoder> FakeVideoDecoderFactory::CreateVideoDecoder(
    const SdpVideoFormat& format) {
  return std::make_unique<test::FakeDecoder>();
}

}  // namespace webrtc
