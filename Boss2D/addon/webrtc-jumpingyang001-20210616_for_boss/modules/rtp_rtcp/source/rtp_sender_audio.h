/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTP_SENDER_AUDIO_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_SENDER_AUDIO_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__transport__field_trial_based_config_h //original-code:"api/transport/field_trial_based_config.h"
#include BOSS_WEBRTC_U_modules__audio_coding__include__audio_coding_module_typedefs_h //original-code:"modules/audio_coding/include/audio_coding_module_typedefs.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__absolute_capture_time_sender_h //original-code:"modules/rtp_rtcp/source/absolute_capture_time_sender.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__dtmf_queue_h //original-code:"modules/rtp_rtcp/source/dtmf_queue.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_sender_h //original-code:"modules/rtp_rtcp/source/rtp_sender.h"
#include BOSS_WEBRTC_U_rtc_base__one_time_event_h //original-code:"rtc_base/one_time_event.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

class RTPSenderAudio {
 public:
  RTPSenderAudio(Clock* clock, RTPSender* rtp_sender);

  RTPSenderAudio() = delete;
  RTPSenderAudio(const RTPSenderAudio&) = delete;
  RTPSenderAudio& operator=(const RTPSenderAudio&) = delete;

  ~RTPSenderAudio();

  int32_t RegisterAudioPayload(absl::string_view payload_name,
                               int8_t payload_type,
                               uint32_t frequency,
                               size_t channels,
                               uint32_t rate);

  bool SendAudio(AudioFrameType frame_type,
                 int8_t payload_type,
                 uint32_t rtp_timestamp,
                 const uint8_t* payload_data,
                 size_t payload_size);

  // `absolute_capture_timestamp_ms` and `Clock::CurrentTime`
  // should be using the same epoch.
  bool SendAudio(AudioFrameType frame_type,
                 int8_t payload_type,
                 uint32_t rtp_timestamp,
                 const uint8_t* payload_data,
                 size_t payload_size,
                 int64_t absolute_capture_timestamp_ms);

  // Store the audio level in dBov for
  // header-extension-for-audio-level-indication.
  // Valid range is [0,100]. Actual value is negative.
  int32_t SetAudioLevel(uint8_t level_dbov);

  // Send a DTMF tone using RFC 2833 (4733)
  int32_t SendTelephoneEvent(uint8_t key, uint16_t time_ms, uint8_t level);

 protected:
  bool SendTelephoneEventPacket(
      bool ended,
      uint32_t dtmf_timestamp,
      uint16_t duration,
      bool marker_bit);  // set on first packet in talk burst

  bool MarkerBit(AudioFrameType frame_type, int8_t payload_type);

 private:
  Clock* const clock_ = nullptr;
  RTPSender* const rtp_sender_ = nullptr;

  Mutex send_audio_mutex_;

  // DTMF.
  bool dtmf_event_is_on_ = false;
  bool dtmf_event_first_packet_sent_ = false;
  int8_t dtmf_payload_type_ RTC_GUARDED_BY(send_audio_mutex_) = -1;
  uint32_t dtmf_payload_freq_ RTC_GUARDED_BY(send_audio_mutex_) = 8000;
  uint32_t dtmf_timestamp_ = 0;
  uint32_t dtmf_length_samples_ = 0;
  int64_t dtmf_time_last_sent_ = 0;
  uint32_t dtmf_timestamp_last_sent_ = 0;
  DtmfQueue::Event dtmf_current_event_;
  DtmfQueue dtmf_queue_;

  // VAD detection, used for marker bit.
  bool inband_vad_active_ RTC_GUARDED_BY(send_audio_mutex_) = false;
  int8_t cngnb_payload_type_ RTC_GUARDED_BY(send_audio_mutex_) = -1;
  int8_t cngwb_payload_type_ RTC_GUARDED_BY(send_audio_mutex_) = -1;
  int8_t cngswb_payload_type_ RTC_GUARDED_BY(send_audio_mutex_) = -1;
  int8_t cngfb_payload_type_ RTC_GUARDED_BY(send_audio_mutex_) = -1;
  int8_t last_payload_type_ RTC_GUARDED_BY(send_audio_mutex_) = -1;

  // Audio level indication.
  // (https://datatracker.ietf.org/doc/draft-lennox-avt-rtp-audio-level-exthdr/)
  uint8_t audio_level_dbov_ RTC_GUARDED_BY(send_audio_mutex_) = 0;
  OneTimeEvent first_packet_sent_;

  absl::optional<uint32_t> encoder_rtp_timestamp_frequency_
      RTC_GUARDED_BY(send_audio_mutex_);

  AbsoluteCaptureTimeSender absolute_capture_time_sender_;

  const FieldTrialBasedConfig field_trials_;
  const bool include_capture_clock_offset_;
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_RTP_SENDER_AUDIO_H_
