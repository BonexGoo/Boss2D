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

#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_h //original-code:"api/rtc_event_log/rtc_event.h"
#include BOSS_WEBRTC_U_logging__rtc_event_log__rtc_stream_config_h //original-code:"logging/rtc_event_log/rtc_stream_config.h"

namespace webrtc {

class RtcEventAudioReceiveStreamConfig final : public RtcEvent {
 public:
  static constexpr Type kType = Type::AudioReceiveStreamConfig;

  explicit RtcEventAudioReceiveStreamConfig(
      std::unique_ptr<rtclog::StreamConfig> config);
  ~RtcEventAudioReceiveStreamConfig() override;

  Type GetType() const override { return kType; }
  bool IsConfigEvent() const override { return true; }

  std::unique_ptr<RtcEventAudioReceiveStreamConfig> Copy() const;

  const rtclog::StreamConfig& config() const { return *config_; }

 private:
  RtcEventAudioReceiveStreamConfig(
      const RtcEventAudioReceiveStreamConfig& other);

  const std::unique_ptr<const rtclog::StreamConfig> config_;
};

struct LoggedAudioRecvConfig {
  LoggedAudioRecvConfig() = default;
  LoggedAudioRecvConfig(int64_t timestamp_us, const rtclog::StreamConfig config)
      : timestamp_us(timestamp_us), config(config) {}

  int64_t log_time_us() const { return timestamp_us; }
  int64_t log_time_ms() const { return timestamp_us / 1000; }

  int64_t timestamp_us;
  rtclog::StreamConfig config;
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_AUDIO_RECEIVE_STREAM_CONFIG_H_
