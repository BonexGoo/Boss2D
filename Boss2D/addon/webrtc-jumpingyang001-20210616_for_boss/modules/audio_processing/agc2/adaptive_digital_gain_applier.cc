/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__adaptive_digital_gain_applier_h //original-code:"modules/audio_processing/agc2/adaptive_digital_gain_applier.h"

#include <algorithm>

#include BOSS_WEBRTC_U_common_audio__include__audio_util_h //original-code:"common_audio/include/audio_util.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__agc2_common_h //original-code:"modules/audio_processing/agc2/agc2_common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__logging__apm_data_dumper_h //original-code:"modules/audio_processing/logging/apm_data_dumper.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_minmax_h //original-code:"rtc_base/numerics/safe_minmax.h"
#include BOSS_WEBRTC_U_system_wrappers__include__metrics_h //original-code:"system_wrappers/include/metrics.h"

namespace webrtc {
namespace {

constexpr int kHeadroomHistogramMin = 0;
constexpr int kHeadroomHistogramMax = 50;

// This function maps input level to desired applied gain. We want to
// boost the signal so that peaks are at -kHeadroomDbfs. We can't
// apply more than kMaxGainDb gain.
float ComputeGainDb(float input_level_dbfs) {
  // If the level is very low, boost it as much as we can.
  if (input_level_dbfs < -(kHeadroomDbfs + kMaxGainDb)) {
    return kMaxGainDb;
  }
  // We expect to end up here most of the time: the level is below
  // -headroom, but we can boost it to -headroom.
  if (input_level_dbfs < -kHeadroomDbfs) {
    return -kHeadroomDbfs - input_level_dbfs;
  }
  // Otherwise, the level is too high and we can't boost.
  RTC_DCHECK_GE(input_level_dbfs, -kHeadroomDbfs);
  return 0.f;
}

// Returns `target_gain` if the output noise level is below
// `max_output_noise_level_dbfs`; otherwise returns a capped gain so that the
// output noise level equals `max_output_noise_level_dbfs`.
float LimitGainByNoise(float target_gain,
                       float input_noise_level_dbfs,
                       float max_output_noise_level_dbfs,
                       ApmDataDumper& apm_data_dumper) {
  const float max_allowed_gain_db =
      max_output_noise_level_dbfs - input_noise_level_dbfs;
  apm_data_dumper.DumpRaw("agc2_adaptive_gain_applier_max_allowed_gain_db",
                          max_allowed_gain_db);
  return std::min(target_gain, std::max(max_allowed_gain_db, 0.f));
}

float LimitGainByLowConfidence(float target_gain,
                               float last_gain,
                               float limiter_audio_level_dbfs,
                               bool estimate_is_confident) {
  if (estimate_is_confident ||
      limiter_audio_level_dbfs <= kLimiterThresholdForAgcGainDbfs) {
    return target_gain;
  }
  const float limiter_level_before_gain = limiter_audio_level_dbfs - last_gain;

  // Compute a new gain so that `limiter_level_before_gain` + `new_target_gain`
  // is not great than `kLimiterThresholdForAgcGainDbfs`.
  const float new_target_gain = std::max(
      kLimiterThresholdForAgcGainDbfs - limiter_level_before_gain, 0.f);
  return std::min(new_target_gain, target_gain);
}

// Computes how the gain should change during this frame.
// Return the gain difference in db to 'last_gain_db'.
float ComputeGainChangeThisFrameDb(float target_gain_db,
                                   float last_gain_db,
                                   bool gain_increase_allowed,
                                   float max_gain_decrease_db,
                                   float max_gain_increase_db) {
  RTC_DCHECK_GT(max_gain_decrease_db, 0);
  RTC_DCHECK_GT(max_gain_increase_db, 0);
  float target_gain_difference_db = target_gain_db - last_gain_db;
  if (!gain_increase_allowed) {
    target_gain_difference_db = std::min(target_gain_difference_db, 0.f);
  }
  return rtc::SafeClamp(target_gain_difference_db, -max_gain_decrease_db,
                        max_gain_increase_db);
}

// Copies the (multichannel) audio samples from `src` into `dst`.
void CopyAudio(AudioFrameView<const float> src,
               std::vector<std::vector<float>>& dst) {
  RTC_DCHECK_GT(src.num_channels(), 0);
  RTC_DCHECK_GT(src.samples_per_channel(), 0);
  RTC_DCHECK_EQ(dst.size(), src.num_channels());
  for (size_t c = 0; c < src.num_channels(); ++c) {
    rtc::ArrayView<const float> channel_view = src.channel(c);
    RTC_DCHECK_EQ(channel_view.size(), src.samples_per_channel());
    RTC_DCHECK_EQ(dst[c].size(), src.samples_per_channel());
    std::copy(channel_view.begin(), channel_view.end(), dst[c].begin());
  }
}

}  // namespace

AdaptiveDigitalGainApplier::AdaptiveDigitalGainApplier(
    ApmDataDumper* apm_data_dumper,
    int adjacent_speech_frames_threshold,
    float max_gain_change_db_per_second,
    float max_output_noise_level_dbfs,
    bool dry_run)
    : apm_data_dumper_(apm_data_dumper),
      gain_applier_(
          /*hard_clip_samples=*/false,
          /*initial_gain_factor=*/DbToRatio(kInitialAdaptiveDigitalGainDb)),
      adjacent_speech_frames_threshold_(adjacent_speech_frames_threshold),
      max_gain_change_db_per_10ms_(max_gain_change_db_per_second *
                                   kFrameDurationMs / 1000.f),
      max_output_noise_level_dbfs_(max_output_noise_level_dbfs),
      dry_run_(dry_run),
      calls_since_last_gain_log_(0),
      frames_to_gain_increase_allowed_(adjacent_speech_frames_threshold_),
      last_gain_db_(kInitialAdaptiveDigitalGainDb) {
  RTC_DCHECK_GT(max_gain_change_db_per_second, 0.0f);
  RTC_DCHECK_GE(frames_to_gain_increase_allowed_, 1);
  RTC_DCHECK_GE(max_output_noise_level_dbfs_, -90.0f);
  RTC_DCHECK_LE(max_output_noise_level_dbfs_, 0.0f);
  Initialize(/*sample_rate_hz=*/48000, /*num_channels=*/1);
}

void AdaptiveDigitalGainApplier::Initialize(int sample_rate_hz,
                                            int num_channels) {
  if (!dry_run_) {
    return;
  }
  RTC_DCHECK_GT(sample_rate_hz, 0);
  RTC_DCHECK_GT(num_channels, 0);
  int frame_size = rtc::CheckedDivExact(sample_rate_hz, 100);
  bool sample_rate_changed =
      dry_run_frame_.empty() ||  // Handle initialization.
      dry_run_frame_[0].size() != static_cast<size_t>(frame_size);
  bool num_channels_changed =
      dry_run_channels_.size() != static_cast<size_t>(num_channels);
  if (sample_rate_changed || num_channels_changed) {
    // Resize the multichannel audio vector and update the channel pointers.
    dry_run_frame_.resize(num_channels);
    dry_run_channels_.resize(num_channels);
    for (int c = 0; c < num_channels; ++c) {
      dry_run_frame_[c].resize(frame_size);
      dry_run_channels_[c] = dry_run_frame_[c].data();
    }
  }
}

void AdaptiveDigitalGainApplier::Process(const FrameInfo& info,
                                         AudioFrameView<float> frame) {
  RTC_DCHECK_GE(info.speech_level_dbfs, -150.f);
  RTC_DCHECK_GE(frame.num_channels(), 1);
  RTC_DCHECK(
      frame.samples_per_channel() == 80 || frame.samples_per_channel() == 160 ||
      frame.samples_per_channel() == 320 || frame.samples_per_channel() == 480)
      << "`frame` does not look like a 10 ms frame for an APM supported sample "
         "rate";

  // Compute the input level used to select the desired gain.
  RTC_DCHECK_GT(info.headroom_db, 0.0f);
  const float input_level_dbfs = info.speech_level_dbfs + info.headroom_db;

  const float target_gain_db = LimitGainByLowConfidence(
      LimitGainByNoise(ComputeGainDb(input_level_dbfs), info.noise_rms_dbfs,
                       max_output_noise_level_dbfs_, *apm_data_dumper_),
      last_gain_db_, info.limiter_envelope_dbfs, info.speech_level_reliable);

  // Forbid increasing the gain until enough adjacent speech frames are
  // observed.
  bool first_confident_speech_frame = false;
  if (info.speech_probability < kVadConfidenceThreshold) {
    frames_to_gain_increase_allowed_ = adjacent_speech_frames_threshold_;
  } else if (frames_to_gain_increase_allowed_ > 0) {
    frames_to_gain_increase_allowed_--;
    first_confident_speech_frame = frames_to_gain_increase_allowed_ == 0;
  }
  apm_data_dumper_->DumpRaw(
      "agc2_adaptive_gain_applier_frames_to_gain_increase_allowed",
      frames_to_gain_increase_allowed_);

  const bool gain_increase_allowed = frames_to_gain_increase_allowed_ == 0;

  float max_gain_increase_db = max_gain_change_db_per_10ms_;
  if (first_confident_speech_frame) {
    // No gain increase happened while waiting for a long enough speech
    // sequence. Therefore, temporarily allow a faster gain increase.
    RTC_DCHECK(gain_increase_allowed);
    max_gain_increase_db *= adjacent_speech_frames_threshold_;
  }

  const float gain_change_this_frame_db = ComputeGainChangeThisFrameDb(
      target_gain_db, last_gain_db_, gain_increase_allowed,
      /*max_gain_decrease_db=*/max_gain_change_db_per_10ms_,
      max_gain_increase_db);

  apm_data_dumper_->DumpRaw("agc2_adaptive_gain_applier_want_to_change_by_db",
                            target_gain_db - last_gain_db_);
  apm_data_dumper_->DumpRaw("agc2_adaptive_gain_applier_will_change_by_db",
                            gain_change_this_frame_db);

  // Optimization: avoid calling math functions if gain does not
  // change.
  if (gain_change_this_frame_db != 0.f) {
    gain_applier_.SetGainFactor(
        DbToRatio(last_gain_db_ + gain_change_this_frame_db));
  }

  // Modify `frame` only if not running in "dry run" mode.
  if (!dry_run_) {
    gain_applier_.ApplyGain(frame);
  } else {
    // Copy `frame` so that `ApplyGain()` is called (on a copy).
    CopyAudio(frame, dry_run_frame_);
    RTC_DCHECK(!dry_run_channels_.empty());
    AudioFrameView<float> frame_copy(&dry_run_channels_[0],
                                     frame.num_channels(),
                                     frame.samples_per_channel());
    gain_applier_.ApplyGain(frame_copy);
  }

  // Remember that the gain has changed for the next iteration.
  last_gain_db_ = last_gain_db_ + gain_change_this_frame_db;
  apm_data_dumper_->DumpRaw("agc2_adaptive_gain_applier_applied_gain_db",
                            last_gain_db_);

  // Log every 10 seconds.
  calls_since_last_gain_log_++;
  if (calls_since_last_gain_log_ == 1000) {
    calls_since_last_gain_log_ = 0;
    RTC_HISTOGRAM_COUNTS_LINEAR("WebRTC.Audio.Agc2.EstimatedSpeechLevel",
                                -info.speech_level_dbfs, 0, 100, 101);
    RTC_HISTOGRAM_COUNTS_LINEAR("WebRTC.Audio.Agc2.EstimatedNoiseLevel",
                                -info.noise_rms_dbfs, 0, 100, 101);
    RTC_HISTOGRAM_COUNTS_LINEAR(
        "WebRTC.Audio.Agc2.Headroom", info.headroom_db, kHeadroomHistogramMin,
        kHeadroomHistogramMax,
        kHeadroomHistogramMax - kHeadroomHistogramMin + 1);
    RTC_HISTOGRAM_COUNTS_LINEAR("WebRTC.Audio.Agc2.DigitalGainApplied",
                                last_gain_db_, 0, kMaxGainDb, kMaxGainDb + 1);
    RTC_LOG(LS_INFO) << "AGC2 adaptive digital"
                     << " | speech_dbfs: " << info.speech_level_dbfs
                     << " | noise_dbfs: " << info.noise_rms_dbfs
                     << " | headroom_db: " << info.headroom_db
                     << " | gain_db: " << last_gain_db_;
  }
}

}  // namespace webrtc
