/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__opus_audio_decoder_factory_h //original-code:"api/audio_codecs/opus_audio_decoder_factory.h"

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_factory_template_h //original-code:"api/audio_codecs/audio_decoder_factory_template.h"
#include BOSS_WEBRTC_U_api__audio_codecs__opus__audio_decoder_multi_channel_opus_h //original-code:"api/audio_codecs/opus/audio_decoder_multi_channel_opus.h"
#include BOSS_WEBRTC_U_api__audio_codecs__opus__audio_decoder_opus_h //original-code:"api/audio_codecs/opus/audio_decoder_opus.h"

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

rtc::scoped_refptr<AudioDecoderFactory> CreateOpusAudioDecoderFactory() {
  return CreateAudioDecoderFactory<
      AudioDecoderOpus, NotAdvertised<AudioDecoderMultiChannelOpus>>();
}

}  // namespace webrtc
