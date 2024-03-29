/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_options_h //original-code:"api/audio_options.h"

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"

namespace cricket {
namespace {

template <class T>
void ToStringIfSet(rtc::SimpleStringBuilder* result,
                   const char* key,
                   const absl::optional<T>& val) {
  if (val) {
    (*result) << key << ": " << *val << ", ";
  }
}

template <typename T>
void SetFrom(absl::optional<T>* s, const absl::optional<T>& o) {
  if (o) {
    *s = o;
  }
}

}  // namespace

AudioOptions::AudioOptions() = default;
AudioOptions::~AudioOptions() = default;

void AudioOptions::SetAll(const AudioOptions& change) {
  SetFrom(&echo_cancellation, change.echo_cancellation);
#if defined(WEBRTC_IOS)
  SetFrom(&ios_force_software_aec_HACK, change.ios_force_software_aec_HACK);
#endif
  SetFrom(&auto_gain_control, change.auto_gain_control);
  SetFrom(&noise_suppression, change.noise_suppression);
  SetFrom(&highpass_filter, change.highpass_filter);
  SetFrom(&stereo_swapping, change.stereo_swapping);
  SetFrom(&audio_jitter_buffer_max_packets,
          change.audio_jitter_buffer_max_packets);
  SetFrom(&audio_jitter_buffer_fast_accelerate,
          change.audio_jitter_buffer_fast_accelerate);
  SetFrom(&audio_jitter_buffer_min_delay_ms,
          change.audio_jitter_buffer_min_delay_ms);
  SetFrom(&audio_jitter_buffer_enable_rtx_handling,
          change.audio_jitter_buffer_enable_rtx_handling);
  SetFrom(&typing_detection, change.typing_detection);
  SetFrom(&experimental_agc, change.experimental_agc);
  SetFrom(&experimental_ns, change.experimental_ns);
  SetFrom(&residual_echo_detector, change.residual_echo_detector);
  SetFrom(&tx_agc_target_dbov, change.tx_agc_target_dbov);
  SetFrom(&tx_agc_digital_compression_gain,
          change.tx_agc_digital_compression_gain);
  SetFrom(&tx_agc_limiter, change.tx_agc_limiter);
  SetFrom(&combined_audio_video_bwe, change.combined_audio_video_bwe);
  SetFrom(&audio_network_adaptor, change.audio_network_adaptor);
  SetFrom(&audio_network_adaptor_config, change.audio_network_adaptor_config);
}

bool AudioOptions::operator==(const AudioOptions& o) const {
  return echo_cancellation == o.echo_cancellation &&
#if defined(WEBRTC_IOS)
         ios_force_software_aec_HACK == o.ios_force_software_aec_HACK &&
#endif
         auto_gain_control == o.auto_gain_control &&
         noise_suppression == o.noise_suppression &&
         highpass_filter == o.highpass_filter &&
         stereo_swapping == o.stereo_swapping &&
         audio_jitter_buffer_max_packets == o.audio_jitter_buffer_max_packets &&
         audio_jitter_buffer_fast_accelerate ==
             o.audio_jitter_buffer_fast_accelerate &&
         audio_jitter_buffer_min_delay_ms ==
             o.audio_jitter_buffer_min_delay_ms &&
         audio_jitter_buffer_enable_rtx_handling ==
             o.audio_jitter_buffer_enable_rtx_handling &&
         typing_detection == o.typing_detection &&
         experimental_agc == o.experimental_agc &&
         experimental_ns == o.experimental_ns &&
         residual_echo_detector == o.residual_echo_detector &&
         tx_agc_target_dbov == o.tx_agc_target_dbov &&
         tx_agc_digital_compression_gain == o.tx_agc_digital_compression_gain &&
         tx_agc_limiter == o.tx_agc_limiter &&
         combined_audio_video_bwe == o.combined_audio_video_bwe &&
         audio_network_adaptor == o.audio_network_adaptor &&
         audio_network_adaptor_config == o.audio_network_adaptor_config;
}

std::string AudioOptions::ToString() const {
  char buffer[1024];
  rtc::SimpleStringBuilder result(buffer);
  result << "AudioOptions {";
  ToStringIfSet(&result, "aec", echo_cancellation);
#if defined(WEBRTC_IOS)
  ToStringIfSet(&result, "ios_force_software_aec_HACK",
                ios_force_software_aec_HACK);
#endif
  ToStringIfSet(&result, "agc", auto_gain_control);
  ToStringIfSet(&result, "ns", noise_suppression);
  ToStringIfSet(&result, "hf", highpass_filter);
  ToStringIfSet(&result, "swap", stereo_swapping);
  ToStringIfSet(&result, "audio_jitter_buffer_max_packets",
                audio_jitter_buffer_max_packets);
  ToStringIfSet(&result, "audio_jitter_buffer_fast_accelerate",
                audio_jitter_buffer_fast_accelerate);
  ToStringIfSet(&result, "audio_jitter_buffer_min_delay_ms",
                audio_jitter_buffer_min_delay_ms);
  ToStringIfSet(&result, "audio_jitter_buffer_enable_rtx_handling",
                audio_jitter_buffer_enable_rtx_handling);
  ToStringIfSet(&result, "typing", typing_detection);
  ToStringIfSet(&result, "experimental_agc", experimental_agc);
  ToStringIfSet(&result, "experimental_ns", experimental_ns);
  ToStringIfSet(&result, "residual_echo_detector", residual_echo_detector);
  ToStringIfSet(&result, "tx_agc_target_dbov", tx_agc_target_dbov);
  ToStringIfSet(&result, "tx_agc_digital_compression_gain",
                tx_agc_digital_compression_gain);
  ToStringIfSet(&result, "tx_agc_limiter", tx_agc_limiter);
  ToStringIfSet(&result, "combined_audio_video_bwe", combined_audio_video_bwe);
  ToStringIfSet(&result, "audio_network_adaptor", audio_network_adaptor);
  result << "}";
  return result.str();
}

}  // namespace cricket
