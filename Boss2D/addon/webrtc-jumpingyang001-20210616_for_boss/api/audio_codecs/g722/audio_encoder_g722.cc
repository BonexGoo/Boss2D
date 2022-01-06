/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__g722__audio_encoder_g722_h //original-code:"api/audio_codecs/g722/audio_encoder_g722.h"

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__match_h //original-code:"absl/strings/match.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__g722__audio_encoder_g722_h //original-code:"modules/audio_coding/codecs/g722/audio_encoder_g722.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_minmax_h //original-code:"rtc_base/numerics/safe_minmax.h"
#include BOSS_WEBRTC_U_rtc_base__string_to_number_h //original-code:"rtc_base/string_to_number.h"

namespace webrtc {

absl::optional<AudioEncoderG722Config> AudioEncoderG722::SdpToConfig(
    const SdpAudioFormat& format) {
  if (!absl::EqualsIgnoreCase(format.name, "g722") ||
      format.clockrate_hz != 8000) {
    return absl::nullopt;
  }

  AudioEncoderG722Config config;
  config.num_channels = rtc::checked_cast<int>(format.num_channels);
  auto ptime_iter = format.parameters.find("ptime");
  if (ptime_iter != format.parameters.end()) {
    auto ptime = rtc::StringToNumber<int>(ptime_iter->second);
    if (ptime && *ptime > 0) {
      const int whole_packets = *ptime / 10;
      config.frame_size_ms = rtc::SafeClamp<int>(whole_packets * 10, 10, 60);
    }
  }
  return config.IsOk() ? absl::optional<AudioEncoderG722Config>(config)
                       : absl::nullopt;
}

void AudioEncoderG722::AppendSupportedEncoders(
    std::vector<AudioCodecSpec>* specs) {
  const SdpAudioFormat fmt = {"G722", 8000, 1};
  const AudioCodecInfo info = QueryAudioEncoder(*SdpToConfig(fmt));
  specs->push_back({fmt, info});
}

AudioCodecInfo AudioEncoderG722::QueryAudioEncoder(
    const AudioEncoderG722Config& config) {
  RTC_DCHECK(config.IsOk());
  return {16000, rtc::dchecked_cast<size_t>(config.num_channels),
          64000 * config.num_channels};
}

std::unique_ptr<AudioEncoder> AudioEncoderG722::MakeAudioEncoder(
    const AudioEncoderG722Config& config,
    int payload_type,
    absl::optional<AudioCodecPairId> /*codec_pair_id*/) {
  RTC_DCHECK(config.IsOk());
  return std::make_unique<AudioEncoderG722Impl>(config, payload_type);
}

}  // namespace webrtc
