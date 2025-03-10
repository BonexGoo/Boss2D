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

#include <map>
#include <string>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__strings__match_h //original-code:"absl/strings/match.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_media__base__codec_h //original-code:"media/base/codec.h"
#include BOSS_WEBRTC_U_media__base__media_constants_h //original-code:"media/base/media_constants.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__multiplex__include__multiplex_decoder_adapter_h //original-code:"modules/video_coding/codecs/multiplex/include/multiplex_decoder_adapter.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__multiplex__include__multiplex_encoder_adapter_h //original-code:"modules/video_coding/codecs/multiplex/include/multiplex_encoder_adapter.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace {

bool IsMultiplexCodec(const cricket::VideoCodec& codec) {
  return absl::EqualsIgnoreCase(codec.name.c_str(),
                                cricket::kMultiplexCodecName);
}

}  // anonymous namespace

namespace webrtc {

constexpr const char* kMultiplexAssociatedCodecName = cricket::kVp9CodecName;

MultiplexEncoderFactory::MultiplexEncoderFactory(
    std::unique_ptr<VideoEncoderFactory> factory,
    bool supports_augmenting_data)
    : factory_(std::move(factory)),
      supports_augmenting_data_(supports_augmenting_data) {}

std::vector<SdpVideoFormat> MultiplexEncoderFactory::GetSupportedFormats()
    const {
  std::vector<SdpVideoFormat> formats = factory_->GetSupportedFormats();
  for (const auto& format : formats) {
    if (absl::EqualsIgnoreCase(format.name, kMultiplexAssociatedCodecName)) {
      SdpVideoFormat multiplex_format = format;
      multiplex_format.parameters[cricket::kCodecParamAssociatedCodecName] =
          format.name;
      multiplex_format.name = cricket::kMultiplexCodecName;
      formats.push_back(multiplex_format);
      break;
    }
  }
  return formats;
}

std::unique_ptr<VideoEncoder> MultiplexEncoderFactory::CreateVideoEncoder(
    const SdpVideoFormat& format) {
  if (!IsMultiplexCodec(cricket::VideoCodec(format)))
    return factory_->CreateVideoEncoder(format);
  const auto& it =
      format.parameters.find(cricket::kCodecParamAssociatedCodecName);
  if (it == format.parameters.end()) {
    RTC_LOG(LS_ERROR) << "No assicated codec for multiplex.";
    return nullptr;
  }
  SdpVideoFormat associated_format = format;
  associated_format.name = it->second;
  return std::unique_ptr<VideoEncoder>(new MultiplexEncoderAdapter(
      factory_.get(), associated_format, supports_augmenting_data_));
}

MultiplexDecoderFactory::MultiplexDecoderFactory(
    std::unique_ptr<VideoDecoderFactory> factory,
    bool supports_augmenting_data)
    : factory_(std::move(factory)),
      supports_augmenting_data_(supports_augmenting_data) {}

std::vector<SdpVideoFormat> MultiplexDecoderFactory::GetSupportedFormats()
    const {
  std::vector<SdpVideoFormat> formats = factory_->GetSupportedFormats();
  for (const auto& format : formats) {
    if (absl::EqualsIgnoreCase(format.name, kMultiplexAssociatedCodecName)) {
      SdpVideoFormat multiplex_format = format;
      multiplex_format.parameters[cricket::kCodecParamAssociatedCodecName] =
          format.name;
      multiplex_format.name = cricket::kMultiplexCodecName;
      formats.push_back(multiplex_format);
    }
  }
  return formats;
}

std::unique_ptr<VideoDecoder> MultiplexDecoderFactory::CreateVideoDecoder(
    const SdpVideoFormat& format) {
  if (!IsMultiplexCodec(cricket::VideoCodec(format)))
    return factory_->CreateVideoDecoder(format);
  const auto& it =
      format.parameters.find(cricket::kCodecParamAssociatedCodecName);
  if (it == format.parameters.end()) {
    RTC_LOG(LS_ERROR) << "No assicated codec for multiplex.";
    return nullptr;
  }
  SdpVideoFormat associated_format = format;
  associated_format.name = it->second;
  return std::unique_ptr<VideoDecoder>(new MultiplexDecoderAdapter(
      factory_.get(), associated_format, supports_augmenting_data_));
}

}  // namespace webrtc
