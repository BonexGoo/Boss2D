/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_AUDIO_RECEIVE_STREAM_CONFIG_H_
#define LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_AUDIO_RECEIVE_STREAM_CONFIG_H_

#include <memory>

#include BOSS_WEBRTC_U_logging__rtc_event_log__events__rtc_event_h //original-code:"logging/rtc_event_log/events/rtc_event.h"

namespace webrtc {

namespace rtclog {
struct StreamConfig;
}  // namespace rtclog

class RtcEventAudioReceiveStreamConfig final : public RtcEvent {
 public:
  explicit RtcEventAudioReceiveStreamConfig(
      std::unique_ptr<rtclog::StreamConfig> config);
  ~RtcEventAudioReceiveStreamConfig() override;

  Type GetType() const override;

  bool IsConfigEvent() const override;

  std::unique_ptr<RtcEvent> Copy() const override;

  const std::unique_ptr<const rtclog::StreamConfig> config_;

 private:
  RtcEventAudioReceiveStreamConfig(
      const RtcEventAudioReceiveStreamConfig& other);
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_AUDIO_RECEIVE_STREAM_CONFIG_H_
