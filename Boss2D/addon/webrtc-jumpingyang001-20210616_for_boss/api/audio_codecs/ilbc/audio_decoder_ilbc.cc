/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__ilbc__audio_decoder_ilbc_h //original-code:"api/audio_codecs/ilbc/audio_decoder_ilbc.h"

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__match_h //original-code:"absl/strings/match.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__ilbc__audio_decoder_ilbc_h //original-code:"modules/audio_coding/codecs/ilbc/audio_decoder_ilbc.h"

namespace webrtc {

absl::optional<AudioDecoderIlbc::Config> AudioDecoderIlbc::SdpToConfig(
    const SdpAudioFormat& format) {
  return absl::EqualsIgnoreCase(format.name, "ILBC") &&
                 format.clockrate_hz == 8000 && format.num_channels == 1
             ? absl::optional<Config>(Config())
             : absl::nullopt;
}

void AudioDecoderIlbc::AppendSupportedDecoders(
    std::vector<AudioCodecSpec>* specs) {
  specs->push_back({{"ILBC", 8000, 1}, {8000, 1, 13300}});
}

std::unique_ptr<AudioDecoder> AudioDecoderIlbc::MakeAudioDecoder(
    Config config,
    absl::optional<AudioCodecPairId> /*codec_pair_id*/) {
  return std::make_unique<AudioDecoderIlbcImpl>();
}

}  // namespace webrtc
