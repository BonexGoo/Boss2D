/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__L16__audio_encoder_L16_h //original-code:"api/audio_codecs/L16/audio_encoder_L16.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__pcm16b__audio_encoder_pcm16b_h //original-code:"modules/audio_coding/codecs/pcm16b/audio_encoder_pcm16b.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__pcm16b__pcm16b_common_h //original-code:"modules/audio_coding/codecs/pcm16b/pcm16b_common.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_minmax_h //original-code:"rtc_base/numerics/safe_minmax.h"
#include BOSS_WEBRTC_U_rtc_base__string_to_number_h //original-code:"rtc_base/string_to_number.h"

namespace webrtc {

absl::optional<AudioEncoderL16::Config> AudioEncoderL16::SdpToConfig(
    const SdpAudioFormat& format) {
  if (!rtc::IsValueInRangeForNumericType<int>(format.num_channels)) {
    return absl::nullopt;
  }
  Config config;
  config.sample_rate_hz = format.clockrate_hz;
  config.num_channels = rtc::dchecked_cast<int>(format.num_channels);
  auto ptime_iter = format.parameters.find("ptime");
  if (ptime_iter != format.parameters.end()) {
    const auto ptime = rtc::StringToNumber<int>(ptime_iter->second);
    if (ptime && *ptime > 0) {
      config.frame_size_ms = rtc::SafeClamp(10 * (*ptime / 10), 10, 60);
    }
  }
  return STR_CASE_CMP(format.name.c_str(), "L16") == 0 && config.IsOk()
             ? absl::optional<Config>(config)
             : absl::nullopt;
}

void AudioEncoderL16::AppendSupportedEncoders(
    std::vector<AudioCodecSpec>* specs) {
  Pcm16BAppendSupportedCodecSpecs(specs);
}

AudioCodecInfo AudioEncoderL16::QueryAudioEncoder(
    const AudioEncoderL16::Config& config) {
  RTC_DCHECK(config.IsOk());
  return {config.sample_rate_hz,
          rtc::dchecked_cast<size_t>(config.num_channels),
          config.sample_rate_hz * config.num_channels * 16};
}

std::unique_ptr<AudioEncoder> AudioEncoderL16::MakeAudioEncoder(
    const AudioEncoderL16::Config& config,
    int payload_type,
    absl::optional<AudioCodecPairId> /*codec_pair_id*/) {
  RTC_DCHECK(config.IsOk());
  AudioEncoderPcm16B::Config c;
  c.sample_rate_hz = config.sample_rate_hz;
  c.num_channels = config.num_channels;
  c.frame_size_ms = config.frame_size_ms;
  c.payload_type = payload_type;
  return absl::make_unique<AudioEncoderPcm16B>(c);
}

}  // namespace webrtc
