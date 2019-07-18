/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__video_codecs__builtin_video_encoder_factory_h //original-code:"api/video_codecs/builtin_video_encoder_factory.h"

#include <vector>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_media__base__codec_h //original-code:"media/base/codec.h"
#include BOSS_WEBRTC_U_media__base__mediaconstants_h //original-code:"media/base/mediaconstants.h"
#include BOSS_WEBRTC_U_media__engine__internalencoderfactory_h //original-code:"media/engine/internalencoderfactory.h"
#include BOSS_WEBRTC_U_media__engine__vp8_encoder_simulcast_proxy_h //original-code:"media/engine/vp8_encoder_simulcast_proxy.h"

namespace webrtc {

namespace {

bool IsFormatSupported(const std::vector<SdpVideoFormat>& supported_formats,
                       const SdpVideoFormat& format) {
  for (const SdpVideoFormat& supported_format : supported_formats) {
    if (cricket::IsSameCodec(format.name, format.parameters,
                             supported_format.name,
                             supported_format.parameters)) {
      return true;
    }
  }
  return false;
}

// This class wraps the internal factory and adds simulcast.
class BuiltinVideoEncoderFactory : public VideoEncoderFactory {
 public:
  BuiltinVideoEncoderFactory()
      : internal_encoder_factory_(new InternalEncoderFactory()) {}

  VideoEncoderFactory::CodecInfo QueryVideoEncoder(
      const SdpVideoFormat& format) const override {
    // Format must be one of the internal formats.
    RTC_DCHECK(IsFormatSupported(
        internal_encoder_factory_->GetSupportedFormats(), format));
    VideoEncoderFactory::CodecInfo info;
    info.has_internal_source = false;
    info.is_hardware_accelerated = false;
    return info;
  }

  std::unique_ptr<VideoEncoder> CreateVideoEncoder(
      const SdpVideoFormat& format) override {
    // Try creating internal encoder.
    std::unique_ptr<VideoEncoder> internal_encoder;
    if (IsFormatSupported(internal_encoder_factory_->GetSupportedFormats(),
                          format)) {
      internal_encoder =
          cricket::CodecNamesEq(format.name.c_str(), cricket::kVp8CodecName)
              ? absl::make_unique<VP8EncoderSimulcastProxy>(
                    internal_encoder_factory_.get(), format)
              : internal_encoder_factory_->CreateVideoEncoder(format);
    }

    return internal_encoder;
  }

  std::vector<SdpVideoFormat> GetSupportedFormats() const override {
    return internal_encoder_factory_->GetSupportedFormats();
  }

 private:
  const std::unique_ptr<VideoEncoderFactory> internal_encoder_factory_;
};

}  // namespace

std::unique_ptr<VideoEncoderFactory> CreateBuiltinVideoEncoderFactory() {
  return absl::make_unique<BuiltinVideoEncoderFactory>();
}

}  // namespace webrtc
