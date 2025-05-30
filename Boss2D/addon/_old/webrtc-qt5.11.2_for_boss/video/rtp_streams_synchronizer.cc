/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "video/rtp_streams_synchronizer.h"

#include BOSS_WEBRTC_U_call__syncable_h //original-code:"call/syncable.h"
#include "modules/video_coding/video_coding_impl.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__timeutils_h //original-code:"rtc_base/timeutils.h"
#include BOSS_WEBRTC_U_rtc_base__trace_event_h //original-code:"rtc_base/trace_event.h"

namespace webrtc {
namespace {
bool UpdateMeasurements(StreamSynchronization::Measurements* stream,
                        const Syncable::Info& info) {
  RTC_DCHECK(stream);
  stream->latest_timestamp = info.latest_received_capture_timestamp;
  stream->latest_receive_time_ms = info.latest_receive_time_ms;
  bool new_rtcp_sr = false;
  if (!stream->rtp_to_ntp.UpdateMeasurements(info.capture_time_ntp_secs,
                                             info.capture_time_ntp_frac,
                                             info.capture_time_source_clock,
                                             &new_rtcp_sr)) {
    return false;
  }
  return true;
}
}  // namespace

RtpStreamsSynchronizer::RtpStreamsSynchronizer(Syncable* syncable_video)
    : syncable_video_(syncable_video),
      syncable_audio_(nullptr),
      sync_(),
      last_sync_time_(rtc::TimeNanos()) {
  RTC_DCHECK(syncable_video);
  process_thread_checker_.DetachFromThread();
}

void RtpStreamsSynchronizer::ConfigureSync(Syncable* syncable_audio) {
  rtc::CritScope lock(&crit_);
  if (syncable_audio == syncable_audio_) {
    // This prevents expensive no-ops.
    return;
  }

  syncable_audio_ = syncable_audio;
  sync_.reset(nullptr);
  if (syncable_audio_) {
    sync_.reset(new StreamSynchronization(syncable_video_->id(),
                                          syncable_audio_->id()));
  }
}

int64_t RtpStreamsSynchronizer::TimeUntilNextProcess() {
  RTC_DCHECK_RUN_ON(&process_thread_checker_);
  const int64_t kSyncIntervalMs = 1000;
  return kSyncIntervalMs -
      (rtc::TimeNanos() - last_sync_time_) / rtc::kNumNanosecsPerMillisec;
}

void RtpStreamsSynchronizer::Process() {
  RTC_DCHECK_RUN_ON(&process_thread_checker_);
  last_sync_time_ = rtc::TimeNanos();

  rtc::CritScope lock(&crit_);
  if (!syncable_audio_) {
    return;
  }
  RTC_DCHECK(sync_.get());

  rtc::Optional<Syncable::Info> audio_info = syncable_audio_->GetInfo();
  if (!audio_info || !UpdateMeasurements(&audio_measurement_, *audio_info)) {
    return;
  }

  int64_t last_video_receive_ms = video_measurement_.latest_receive_time_ms;
  rtc::Optional<Syncable::Info> video_info = syncable_video_->GetInfo();
  if (!video_info || !UpdateMeasurements(&video_measurement_, *video_info)) {
    return;
  }

  if (last_video_receive_ms == video_measurement_.latest_receive_time_ms) {
    // No new video packet has been received since last update.
    return;
  }

  int relative_delay_ms;
  // Calculate how much later or earlier the audio stream is compared to video.
  if (!sync_->ComputeRelativeDelay(audio_measurement_, video_measurement_,
                                   &relative_delay_ms)) {
    return;
  }

  TRACE_COUNTER1("webrtc", "SyncCurrentVideoDelay",
      video_info->current_delay_ms);
  TRACE_COUNTER1("webrtc", "SyncCurrentAudioDelay",
      audio_info->current_delay_ms);
  TRACE_COUNTER1("webrtc", "SyncRelativeDelay", relative_delay_ms);
  int target_audio_delay_ms = 0;
  int target_video_delay_ms = video_info->current_delay_ms;
  // Calculate the necessary extra audio delay and desired total video
  // delay to get the streams in sync.
  if (!sync_->ComputeDelays(relative_delay_ms,
                            audio_info->current_delay_ms,
                            &target_audio_delay_ms,
                            &target_video_delay_ms)) {
    return;
  }

  syncable_audio_->SetMinimumPlayoutDelay(target_audio_delay_ms);
  syncable_video_->SetMinimumPlayoutDelay(target_video_delay_ms);
}

bool RtpStreamsSynchronizer::GetStreamSyncOffsetInMs(
    uint32_t timestamp,
    int64_t render_time_ms,
    int64_t* stream_offset_ms,
    double* estimated_freq_khz) const {
  rtc::CritScope lock(&crit_);
  if (!syncable_audio_) {
    return false;
  }

  uint32_t playout_timestamp = syncable_audio_->GetPlayoutTimestamp();

  int64_t latest_audio_ntp;
  if (!audio_measurement_.rtp_to_ntp.Estimate(playout_timestamp,
                                              &latest_audio_ntp)) {
    return false;
  }

  int64_t latest_video_ntp;
  if (!video_measurement_.rtp_to_ntp.Estimate(timestamp, &latest_video_ntp)) {
    return false;
  }

  int64_t time_to_render_ms = render_time_ms - rtc::TimeMillis();
  if (time_to_render_ms > 0)
    latest_video_ntp += time_to_render_ms;

  *stream_offset_ms = latest_audio_ntp - latest_video_ntp;
  *estimated_freq_khz = video_measurement_.rtp_to_ntp.params()->frequency_khz;
  return true;
}

}  // namespace webrtc
