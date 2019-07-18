/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__ilbc__audio_encoder_ilbc_h //original-code:"api/audio_codecs/ilbc/audio_encoder_ilbc.h"

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include "modules/audio_coding/codecs/ilbc/audio_encoder_ilbc.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_minmax_h //original-code:"rtc_base/numerics/safe_minmax.h"
#include BOSS_WEBRTC_U_rtc_base__ptr_util_h //original-code:"rtc_base/ptr_util.h"
#include BOSS_WEBRTC_U_rtc_base__string_to_number_h //original-code:"rtc_base/string_to_number.h"

namespace webrtc {
namespace {
int GetIlbcBitrate(int ptime) {
  switch (ptime) {
    case 20:
    case 40:
      // 38 bytes per frame of 20 ms => 15200 bits/s.
      return 15200;
    case 30:
    case 60:
      // 50 bytes per frame of 30 ms => (approx) 13333 bits/s.
      return 13333;
    default:
      FATAL();
  }
}
}  // namespace

rtc::Optional<AudioEncoderIlbcConfig> AudioEncoderIlbc::SdpToConfig(
    const SdpAudioFormat& format) {
  if (STR_CASE_CMP(format.name.c_str(), "ILBC") != 0 ||
      format.clockrate_hz != 8000 || format.num_channels != 1) {
    return rtc::nullopt;
  }

  AudioEncoderIlbcConfig config;
  auto ptime_iter = format.parameters.find("ptime");
  if (ptime_iter != format.parameters.end()) {
    auto ptime = rtc::StringToNumber<int>(ptime_iter->second);
    if (ptime && *ptime > 0) {
      const int whole_packets = *ptime / 10;
      config.frame_size_ms = rtc::SafeClamp<int>(whole_packets * 10, 20, 60);
    }
  }
  return config.IsOk() ? rtc::Optional<AudioEncoderIlbcConfig>(config)
                       : rtc::nullopt;
}

void AudioEncoderIlbc::AppendSupportedEncoders(
    std::vector<AudioCodecSpec>* specs) {
  const SdpAudioFormat fmt = {"ILBC", 8000, 1};
  const AudioCodecInfo info = QueryAudioEncoder(*SdpToConfig(fmt));
  specs->push_back({fmt, info});
}

AudioCodecInfo AudioEncoderIlbc::QueryAudioEncoder(
    const AudioEncoderIlbcConfig& config) {
  RTC_DCHECK(config.IsOk());
  return {8000, 1, GetIlbcBitrate(config.frame_size_ms)};
}

std::unique_ptr<AudioEncoder> AudioEncoderIlbc::MakeAudioEncoder(
    const AudioEncoderIlbcConfig& config,
    int payload_type) {
  RTC_DCHECK(config.IsOk());
  return rtc::MakeUnique<AudioEncoderIlbcImpl>(config, payload_type);
}

}  // namespace webrtc
