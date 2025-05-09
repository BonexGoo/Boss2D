/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__g711__audio_encoder_g711_h //original-code:"api/audio_codecs/g711/audio_encoder_g711.h"

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__match_h //original-code:"absl/strings/match.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__g711__audio_encoder_pcm_h //original-code:"modules/audio_coding/codecs/g711/audio_encoder_pcm.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_minmax_h //original-code:"rtc_base/numerics/safe_minmax.h"
#include BOSS_WEBRTC_U_rtc_base__string_to_number_h //original-code:"rtc_base/string_to_number.h"

namespace webrtc {

absl::optional<AudioEncoderG711::Config> AudioEncoderG711::SdpToConfig(
    const SdpAudioFormat& format) {
  const bool is_pcmu = absl::EqualsIgnoreCase(format.name, "PCMU");
  const bool is_pcma = absl::EqualsIgnoreCase(format.name, "PCMA");
  if (format.clockrate_hz == 8000 && format.num_channels >= 1 &&
      (is_pcmu || is_pcma)) {
    Config config;
    config.type = is_pcmu ? Config::Type::kPcmU : Config::Type::kPcmA;
    config.num_channels = rtc::dchecked_cast<int>(format.num_channels);
    config.frame_size_ms = 20;
    auto ptime_iter = format.parameters.find("ptime");
    if (ptime_iter != format.parameters.end()) {
      const auto ptime = rtc::StringToNumber<int>(ptime_iter->second);
      if (ptime && *ptime > 0) {
        config.frame_size_ms = rtc::SafeClamp(10 * (*ptime / 10), 10, 60);
      }
    }
    RTC_DCHECK(config.IsOk());
    return config;
  } else {
    return absl::nullopt;
  }
}

void AudioEncoderG711::AppendSupportedEncoders(
    std::vector<AudioCodecSpec>* specs) {
  for (const char* type : {"PCMU", "PCMA"}) {
    specs->push_back({{type, 8000, 1}, {8000, 1, 64000}});
  }
}

AudioCodecInfo AudioEncoderG711::QueryAudioEncoder(const Config& config) {
  RTC_DCHECK(config.IsOk());
  return {8000, rtc::dchecked_cast<size_t>(config.num_channels),
          64000 * config.num_channels};
}

std::unique_ptr<AudioEncoder> AudioEncoderG711::MakeAudioEncoder(
    const Config& config,
    int payload_type,
    absl::optional<AudioCodecPairId> /*codec_pair_id*/) {
  RTC_DCHECK(config.IsOk());
  switch (config.type) {
    case Config::Type::kPcmU: {
      AudioEncoderPcmU::Config impl_config;
      impl_config.num_channels = config.num_channels;
      impl_config.frame_size_ms = config.frame_size_ms;
      impl_config.payload_type = payload_type;
      return std::make_unique<AudioEncoderPcmU>(impl_config);
    }
    case Config::Type::kPcmA: {
      AudioEncoderPcmA::Config impl_config;
      impl_config.num_channels = config.num_channels;
      impl_config.frame_size_ms = config.frame_size_ms;
      impl_config.payload_type = payload_type;
      return std::make_unique<AudioEncoderPcmA>(impl_config);
    }
    default: {
      return nullptr;
    }
  }
}

}  // namespace webrtc
