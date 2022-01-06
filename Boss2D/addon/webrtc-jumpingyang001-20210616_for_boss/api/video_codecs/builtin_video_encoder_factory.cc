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

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__match_h //original-code:"absl/strings/match.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_media__base__codec_h //original-code:"media/base/codec.h"
#include BOSS_WEBRTC_U_media__base__media_constants_h //original-code:"media/base/media_constants.h"
#include BOSS_WEBRTC_U_media__engine__encoder_simulcast_proxy_h //original-code:"media/engine/encoder_simulcast_proxy.h"
#include BOSS_WEBRTC_U_media__engine__internal_encoder_factory_h //original-code:"media/engine/internal_encoder_factory.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

namespace {

// This class wraps the internal factory and adds simulcast.
class BuiltinVideoEncoderFactory : public VideoEncoderFactory {
 public:
  BuiltinVideoEncoderFactory()
      : internal_encoder_factory_(new InternalEncoderFactory()) {}

  VideoEncoderFactory::CodecInfo QueryVideoEncoder(
      const SdpVideoFormat& format) const override {
    // Format must be one of the internal formats.
    RTC_DCHECK(
        format.IsCodecInList(internal_encoder_factory_->GetSupportedFormats()));
    VideoEncoderFactory::CodecInfo info;
    return info;
  }

  std::unique_ptr<VideoEncoder> CreateVideoEncoder(
      const SdpVideoFormat& format) override {
    // Try creating internal encoder.
    std::unique_ptr<VideoEncoder> internal_encoder;
    if (format.IsCodecInList(
            internal_encoder_factory_->GetSupportedFormats())) {
      internal_encoder = std::make_unique<EncoderSimulcastProxy>(
          internal_encoder_factory_.get(), format);
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
  return std::make_unique<BuiltinVideoEncoderFactory>();
}

}  // namespace webrtc
