/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_EVENT_LOG_WRITER_H_
#define MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_EVENT_LOG_WRITER_H_

#include BOSS_WEBRTC_U_modules__audio_coding__audio_network_adaptor__include__audio_network_adaptor_config_h //original-code:"modules/audio_coding/audio_network_adaptor/include/audio_network_adaptor_config.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {
class RtcEventLog;

class EventLogWriter final {
 public:
  EventLogWriter(RtcEventLog* event_log,
                 int min_bitrate_change_bps,
                 float min_bitrate_change_fraction,
                 float min_packet_loss_change_fraction);
  ~EventLogWriter();
  void MaybeLogEncoderConfig(const AudioEncoderRuntimeConfig& config);

 private:
  void LogEncoderConfig(const AudioEncoderRuntimeConfig& config);

  RtcEventLog* const event_log_;
  const int min_bitrate_change_bps_;
  const float min_bitrate_change_fraction_;
  const float min_packet_loss_change_fraction_;
  AudioEncoderRuntimeConfig last_logged_config_;
  RTC_DISALLOW_COPY_AND_ASSIGN(EventLogWriter);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_EVENT_LOG_WRITER_H_
