/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_audio_network_adaptation_h //original-code:"logging/rtc_event_log/events/rtc_event_audio_network_adaptation.h"

#include <utility>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_modules__audio_coding__audio_network_adaptor__include__audio_network_adaptor_config_h //original-code:"modules/audio_coding/audio_network_adaptor/include/audio_network_adaptor_config.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

RtcEventAudioNetworkAdaptation::RtcEventAudioNetworkAdaptation(
    std::unique_ptr<AudioEncoderRuntimeConfig> config)
    : config_(std::move(config)) {
  RTC_DCHECK(config_);
}

RtcEventAudioNetworkAdaptation::RtcEventAudioNetworkAdaptation(
    const RtcEventAudioNetworkAdaptation& other)
    : RtcEvent(other.timestamp_us_),
      config_(std::make_unique<AudioEncoderRuntimeConfig>(*other.config_)) {}

RtcEventAudioNetworkAdaptation::~RtcEventAudioNetworkAdaptation() = default;

std::unique_ptr<RtcEventAudioNetworkAdaptation>
RtcEventAudioNetworkAdaptation::Copy() const {
  return absl::WrapUnique(new RtcEventAudioNetworkAdaptation(*this));
}

}  // namespace webrtc
