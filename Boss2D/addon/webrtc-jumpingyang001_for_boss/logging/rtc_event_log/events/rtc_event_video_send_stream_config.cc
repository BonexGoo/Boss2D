/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_video_send_stream_config_h //original-code:"logging/rtc_event_log/events/rtc_event_video_send_stream_config.h"

#include <utility>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"

namespace webrtc {

RtcEventVideoSendStreamConfig::RtcEventVideoSendStreamConfig(
    std::unique_ptr<rtclog::StreamConfig> config)
    : config_(std::move(config)) {}

RtcEventVideoSendStreamConfig::RtcEventVideoSendStreamConfig(
    const RtcEventVideoSendStreamConfig& other)
    : RtcEvent(other.timestamp_us_),
      config_(absl::make_unique<rtclog::StreamConfig>(*other.config_)) {}

RtcEventVideoSendStreamConfig::~RtcEventVideoSendStreamConfig() = default;

RtcEvent::Type RtcEventVideoSendStreamConfig::GetType() const {
  return RtcEvent::Type::VideoSendStreamConfig;
}

bool RtcEventVideoSendStreamConfig::IsConfigEvent() const {
  return true;
}

std::unique_ptr<RtcEvent> RtcEventVideoSendStreamConfig::Copy() const {
  return absl::WrapUnique<RtcEvent>(new RtcEventVideoSendStreamConfig(*this));
}

}  // namespace webrtc
