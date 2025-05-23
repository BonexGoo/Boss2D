/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_decoder_factory_h //original-code:"api/audio_codecs/builtin_audio_decoder_factory.h"

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__audio_codecs__L16__audio_decoder_L16_h //original-code:"api/audio_codecs/L16/audio_decoder_L16.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_factory_template_h //original-code:"api/audio_codecs/audio_decoder_factory_template.h"
#include BOSS_WEBRTC_U_api__audio_codecs__g711__audio_decoder_g711_h //original-code:"api/audio_codecs/g711/audio_decoder_g711.h"
#include BOSS_WEBRTC_U_api__audio_codecs__g722__audio_decoder_g722_h //original-code:"api/audio_codecs/g722/audio_decoder_g722.h"
#if WEBRTC_USE_BUILTIN_ILBC
#include BOSS_WEBRTC_U_api__audio_codecs__ilbc__audio_decoder_ilbc_h //original-code:"api/audio_codecs/ilbc/audio_decoder_ilbc.h"  // nogncheck
#endif
#include BOSS_WEBRTC_U_api__audio_codecs__isac__audio_decoder_isac_h //original-code:"api/audio_codecs/isac/audio_decoder_isac.h"
#if WEBRTC_USE_BUILTIN_OPUS
#include BOSS_WEBRTC_U_api__audio_codecs__opus__audio_decoder_opus_h //original-code:"api/audio_codecs/opus/audio_decoder_opus.h"  // nogncheck
#endif

namespace webrtc {

namespace {

// Modify an audio decoder to not advertise support for anything.
template <typename T>
struct NotAdvertised {
  using Config = typename T::Config;
  static absl::optional<Config> SdpToConfig(
      const SdpAudioFormat& audio_format) {
    return T::SdpToConfig(audio_format);
  }
  static void AppendSupportedDecoders(std::vector<AudioCodecSpec>* specs) {
    // Don't advertise support for anything.
  }
  static std::unique_ptr<AudioDecoder> MakeAudioDecoder(
      const Config& config,
      absl::optional<AudioCodecPairId> codec_pair_id = absl::nullopt) {
    return T::MakeAudioDecoder(config, codec_pair_id);
  }
};

}  // namespace

rtc::scoped_refptr<AudioDecoderFactory> CreateBuiltinAudioDecoderFactory() {
  return CreateAudioDecoderFactory<

#if WEBRTC_USE_BUILTIN_OPUS
      AudioDecoderOpus,
#endif

      AudioDecoderIsac, AudioDecoderG722,

#if WEBRTC_USE_BUILTIN_ILBC
      AudioDecoderIlbc,
#endif

      AudioDecoderG711, NotAdvertised<AudioDecoderL16>>();
}

}  // namespace webrtc
