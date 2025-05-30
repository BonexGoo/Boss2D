/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__g711__audio_decoder_g711_h //original-code:"api/audio_codecs/g711/audio_decoder_g711.h"

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__match_h //original-code:"absl/strings/match.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__g711__audio_decoder_pcm_h //original-code:"modules/audio_coding/codecs/g711/audio_decoder_pcm.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"

namespace webrtc {

absl::optional<AudioDecoderG711::Config> AudioDecoderG711::SdpToConfig(
    const SdpAudioFormat& format) {
  const bool is_pcmu = absl::EqualsIgnoreCase(format.name, "PCMU");
  const bool is_pcma = absl::EqualsIgnoreCase(format.name, "PCMA");
  if (format.clockrate_hz == 8000 && format.num_channels >= 1 &&
      (is_pcmu || is_pcma)) {
    Config config;
    config.type = is_pcmu ? Config::Type::kPcmU : Config::Type::kPcmA;
    config.num_channels = rtc::dchecked_cast<int>(format.num_channels);
    RTC_DCHECK(config.IsOk());
    return config;
  } else {
    return absl::nullopt;
  }
}

void AudioDecoderG711::AppendSupportedDecoders(
    std::vector<AudioCodecSpec>* specs) {
  for (const char* type : {"PCMU", "PCMA"}) {
    specs->push_back({{type, 8000, 1}, {8000, 1, 64000}});
  }
}

std::unique_ptr<AudioDecoder> AudioDecoderG711::MakeAudioDecoder(
    const Config& config,
    absl::optional<AudioCodecPairId> /*codec_pair_id*/) {
  RTC_DCHECK(config.IsOk());
  switch (config.type) {
    case Config::Type::kPcmU:
      return std::make_unique<AudioDecoderPcmU>(config.num_channels);
    case Config::Type::kPcmA:
      return std::make_unique<AudioDecoderPcmA>(config.num_channels);
    default:
      return nullptr;
  }
}

}  // namespace webrtc
