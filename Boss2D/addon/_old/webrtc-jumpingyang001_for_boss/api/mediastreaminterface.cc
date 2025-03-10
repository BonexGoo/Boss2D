/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__mediastreaminterface_h //original-code:"api/mediastreaminterface.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace webrtc {

const char MediaStreamTrackInterface::kVideoKind[] = "video";
const char MediaStreamTrackInterface::kAudioKind[] = "audio";

void AudioProcessorInterface::GetStats(AudioProcessorStats* /*stats*/) {
  RTC_NOTREACHED() << "Old-style GetStats() is called but it has no "
                   << "implementation.";
  RTC_LOG(LS_ERROR) << "Old-style GetStats() is called but it has no "
                    << "implementation.";
}

// TODO(ivoc): Remove this when the function becomes pure virtual.
AudioProcessorInterface::AudioProcessorStatistics
AudioProcessorInterface::GetStats(bool /*has_remote_tracks*/) {
  AudioProcessorStats stats;
  GetStats(&stats);
  AudioProcessorStatistics new_stats;
  new_stats.apm_statistics.divergent_filter_fraction =
      stats.aec_divergent_filter_fraction;
  new_stats.apm_statistics.delay_median_ms = stats.echo_delay_median_ms;
  new_stats.apm_statistics.delay_standard_deviation_ms =
      stats.echo_delay_std_ms;
  new_stats.apm_statistics.echo_return_loss = stats.echo_return_loss;
  new_stats.apm_statistics.echo_return_loss_enhancement =
      stats.echo_return_loss_enhancement;
  new_stats.apm_statistics.residual_echo_likelihood =
      stats.residual_echo_likelihood;
  new_stats.apm_statistics.residual_echo_likelihood_recent_max =
      stats.residual_echo_likelihood_recent_max;
  return new_stats;
}

VideoTrackInterface::ContentHint VideoTrackInterface::content_hint() const {
  return ContentHint::kNone;
}

bool AudioTrackInterface::GetSignalLevel(int* level) {
  return false;
}

rtc::scoped_refptr<AudioProcessorInterface>
AudioTrackInterface::GetAudioProcessor() {
  return nullptr;
}

}  // namespace webrtc
