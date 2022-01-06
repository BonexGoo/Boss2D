/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__isac__audio_decoder_isac_fix_h //original-code:"api/audio_codecs/isac/audio_decoder_isac_fix.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__isac__fix__include__audio_decoder_isacfix_h //original-code:"modules/audio_coding/codecs/isac/fix/include/audio_decoder_isacfix.h"

namespace webrtc {

absl::optional<AudioDecoderIsacFix::Config> AudioDecoderIsacFix::SdpToConfig(
    const SdpAudioFormat& format) {
  return STR_CASE_CMP(format.name.c_str(), "ISAC") == 0 &&
                 format.clockrate_hz == 16000 && format.num_channels == 1
             ? absl::optional<Config>(Config())
             : absl::nullopt;
}

void AudioDecoderIsacFix::AppendSupportedDecoders(
    std::vector<AudioCodecSpec>* specs) {
  specs->push_back({{"ISAC", 16000, 1}, {16000, 1, 32000, 10000, 32000}});
}

std::unique_ptr<AudioDecoder> AudioDecoderIsacFix::MakeAudioDecoder(
    Config config,
    absl::optional<AudioCodecPairId> /*codec_pair_id*/) {
  return absl::make_unique<AudioDecoderIsacFixImpl>(16000);
}

}  // namespace webrtc
