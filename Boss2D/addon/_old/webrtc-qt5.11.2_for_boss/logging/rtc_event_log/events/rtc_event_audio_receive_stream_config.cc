/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_audio_receive_stream_config_h //original-code:"logging/rtc_event_log/events/rtc_event_audio_receive_stream_config.h"

#include <utility>

#include BOSS_WEBRTC_U_logging__rtc_event_log__rtc_stream_config_h //original-code:"logging/rtc_event_log/rtc_stream_config.h"

namespace webrtc {

RtcEventAudioReceiveStreamConfig::RtcEventAudioReceiveStreamConfig(
    std::unique_ptr<rtclog::StreamConfig> config)
    : config_(std::move(config)) {}

RtcEventAudioReceiveStreamConfig::~RtcEventAudioReceiveStreamConfig() = default;

RtcEvent::Type RtcEventAudioReceiveStreamConfig::GetType() const {
  return RtcEvent::Type::AudioReceiveStreamConfig;
}

bool RtcEventAudioReceiveStreamConfig::IsConfigEvent() const {
  return true;
}

}  // namespace webrtc
