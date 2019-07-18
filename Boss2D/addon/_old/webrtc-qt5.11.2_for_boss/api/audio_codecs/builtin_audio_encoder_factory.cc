/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_encoder_factory_h //original-code:"api/audio_codecs/builtin_audio_encoder_factory.h"

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__audio_codecs__L16__audio_encoder_L16_h //original-code:"api/audio_codecs/L16/audio_encoder_L16.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_factory_template_h //original-code:"api/audio_codecs/audio_encoder_factory_template.h"
#include BOSS_WEBRTC_U_api__audio_codecs__g711__audio_encoder_g711_h //original-code:"api/audio_codecs/g711/audio_encoder_g711.h"
#include BOSS_WEBRTC_U_api__audio_codecs__g722__audio_encoder_g722_h //original-code:"api/audio_codecs/g722/audio_encoder_g722.h"
#if WEBRTC_USE_BUILTIN_ILBC
#include BOSS_WEBRTC_U_api__audio_codecs__ilbc__audio_encoder_ilbc_h //original-code:"api/audio_codecs/ilbc/audio_encoder_ilbc.h"  // nogncheck
#endif
#include BOSS_WEBRTC_U_api__audio_codecs__isac__audio_encoder_isac_h //original-code:"api/audio_codecs/isac/audio_encoder_isac.h"
#if WEBRTC_USE_BUILTIN_OPUS
#include BOSS_WEBRTC_U_api__audio_codecs__opus__audio_encoder_opus_h //original-code:"api/audio_codecs/opus/audio_encoder_opus.h"  // nogncheck
#endif

namespace webrtc {

namespace {

// Modify an audio encoder to not advertise support for anything.
template <typename T>
struct NotAdvertised {
  using Config = typename T::Config;
  static rtc::Optional<Config> SdpToConfig(const SdpAudioFormat& audio_format) {
    return T::SdpToConfig(audio_format);
  }
  static void AppendSupportedEncoders(std::vector<AudioCodecSpec>* specs) {
    // Don't advertise support for anything.
  }
  static AudioCodecInfo QueryAudioEncoder(const Config& config) {
    return T::QueryAudioEncoder(config);
  }
  static std::unique_ptr<AudioEncoder> MakeAudioEncoder(const Config& config,
                                                        int payload_type) {
    return T::MakeAudioEncoder(config, payload_type);
  }
};

}  // namespace

rtc::scoped_refptr<AudioEncoderFactory> CreateBuiltinAudioEncoderFactory() {
  return CreateAudioEncoderFactory<

#if WEBRTC_USE_BUILTIN_OPUS
      AudioEncoderOpus,
#endif

      AudioEncoderIsac, AudioEncoderG722,

#if WEBRTC_USE_BUILTIN_ILBC
      AudioEncoderIlbc,
#endif

      AudioEncoderG711, NotAdvertised<AudioEncoderL16>>();
}

}  // namespace webrtc
