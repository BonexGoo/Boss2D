/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__isac__audio_decoder_isac_float_h //original-code:"api/audio_codecs/isac/audio_decoder_isac_float.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__isac__main__include__audio_decoder_isac_h //original-code:"modules/audio_coding/codecs/isac/main/include/audio_decoder_isac.h"

namespace webrtc {

absl::optional<AudioDecoderIsacFloat::Config>
AudioDecoderIsacFloat::SdpToConfig(const SdpAudioFormat& format) {
  if (STR_CASE_CMP(format.name.c_str(), "ISAC") == 0 &&
      (format.clockrate_hz == 16000 || format.clockrate_hz == 32000) &&
      format.num_channels == 1) {
    Config config;
    config.sample_rate_hz = format.clockrate_hz;
    return config;
  } else {
    return absl::nullopt;
  }
}

void AudioDecoderIsacFloat::AppendSupportedDecoders(
    std::vector<AudioCodecSpec>* specs) {
  specs->push_back({{"ISAC", 16000, 1}, {16000, 1, 32000, 10000, 32000}});
  specs->push_back({{"ISAC", 32000, 1}, {32000, 1, 56000, 10000, 56000}});
}

std::unique_ptr<AudioDecoder> AudioDecoderIsacFloat::MakeAudioDecoder(
    Config config,
    absl::optional<AudioCodecPairId> /*codec_pair_id*/) {
  RTC_DCHECK(config.IsOk());
  return absl::make_unique<AudioDecoderIsacFloatImpl>(config.sample_rate_hz);
}

}  // namespace webrtc
