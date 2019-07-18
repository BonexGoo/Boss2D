/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__L16__audio_decoder_L16_h //original-code:"api/audio_codecs/L16/audio_decoder_L16.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__pcm16b__audio_decoder_pcm16b_h //original-code:"modules/audio_coding/codecs/pcm16b/audio_decoder_pcm16b.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__pcm16b__pcm16b_common_h //original-code:"modules/audio_coding/codecs/pcm16b/pcm16b_common.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"

namespace webrtc {

absl::optional<AudioDecoderL16::Config> AudioDecoderL16::SdpToConfig(
    const SdpAudioFormat& format) {
  Config config;
  config.sample_rate_hz = format.clockrate_hz;
  config.num_channels = rtc::checked_cast<int>(format.num_channels);
  return STR_CASE_CMP(format.name.c_str(), "L16") == 0 && config.IsOk()
             ? absl::optional<Config>(config)
             : absl::nullopt;
}

void AudioDecoderL16::AppendSupportedDecoders(
    std::vector<AudioCodecSpec>* specs) {
  Pcm16BAppendSupportedCodecSpecs(specs);
}

std::unique_ptr<AudioDecoder> AudioDecoderL16::MakeAudioDecoder(
    const Config& config,
    absl::optional<AudioCodecPairId> /*codec_pair_id*/) {
  return config.IsOk() ? absl::make_unique<AudioDecoderPcm16B>(
                             config.sample_rate_hz, config.num_channels)
                       : nullptr;
}

}  // namespace webrtc
