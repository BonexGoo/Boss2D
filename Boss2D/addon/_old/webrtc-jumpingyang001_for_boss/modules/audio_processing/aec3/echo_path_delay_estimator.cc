/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "modules/audio_processing/aec3/echo_path_delay_estimator.h"

#include <algorithm>
#include <array>

#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include "modules/audio_processing/aec3/aec3_common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__logging__apm_data_dumper_h //original-code:"modules/audio_processing/logging/apm_data_dumper.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"

namespace webrtc {
namespace {
size_t GetDownSamplingFactor(const EchoCanceller3Config& config) {
  // Do not use down sampling factor 8 if kill switch is triggered.
  return (config.delay.down_sampling_factor == 8 &&
          field_trial::IsEnabled("WebRTC-Aec3DownSamplingFactor8KillSwitch"))
             ? 4
             : config.delay.down_sampling_factor;
}
}  // namespace

EchoPathDelayEstimator::EchoPathDelayEstimator(
    ApmDataDumper* data_dumper,
    const EchoCanceller3Config& config)
    : data_dumper_(data_dumper),
      down_sampling_factor_(GetDownSamplingFactor(config)),
      sub_block_size_(down_sampling_factor_ != 0
                          ? kBlockSize / down_sampling_factor_
                          : kBlockSize),
      capture_decimator_(down_sampling_factor_),
      matched_filter_(
          data_dumper_,
          DetectOptimization(),
          sub_block_size_,
          kMatchedFilterWindowSizeSubBlocks,
          config.delay.num_filters,
          kMatchedFilterAlignmentShiftSizeSubBlocks,
          GetDownSamplingFactor(config) == 8
              ? config.render_levels.poor_excitation_render_limit_ds8
              : config.render_levels.poor_excitation_render_limit),
      matched_filter_lag_aggregator_(data_dumper_,
                                     matched_filter_.GetMaxFilterLag()) {
  RTC_DCHECK(data_dumper);
  RTC_DCHECK(down_sampling_factor_ > 0);
}

EchoPathDelayEstimator::~EchoPathDelayEstimator() = default;

void EchoPathDelayEstimator::Reset(bool soft_reset) {
  if (!soft_reset) {
    matched_filter_lag_aggregator_.Reset();
  }
  matched_filter_.Reset();
  old_aggregated_lag_ = absl::nullopt;
  consistent_estimate_counter_ = 0;
}

absl::optional<DelayEstimate> EchoPathDelayEstimator::EstimateDelay(
    const DownsampledRenderBuffer& render_buffer,
    rtc::ArrayView<const float> capture) {
  RTC_DCHECK_EQ(kBlockSize, capture.size());

  std::array<float, kBlockSize> downsampled_capture_data;
  rtc::ArrayView<float> downsampled_capture(downsampled_capture_data.data(),
                                            sub_block_size_);
  data_dumper_->DumpWav("aec3_capture_decimator_input", capture.size(),
                        capture.data(), 16000, 1);
  capture_decimator_.Decimate(capture, downsampled_capture);
  data_dumper_->DumpWav("aec3_capture_decimator_output",
                        downsampled_capture.size(), downsampled_capture.data(),
                        16000 / down_sampling_factor_, 1);
  matched_filter_.Update(render_buffer, downsampled_capture);

  absl::optional<DelayEstimate> aggregated_matched_filter_lag =
      matched_filter_lag_aggregator_.Aggregate(
          matched_filter_.GetLagEstimates());

  // TODO(peah): Move this logging outside of this class once EchoCanceller3
  // development is done.
  data_dumper_->DumpRaw(
      "aec3_echo_path_delay_estimator_delay",
      aggregated_matched_filter_lag
          ? static_cast<int>(aggregated_matched_filter_lag->delay *
                             down_sampling_factor_)
          : -1);

  // Return the detected delay in samples as the aggregated matched filter lag
  // compensated by the down sampling factor for the signal being correlated.
  if (aggregated_matched_filter_lag) {
    aggregated_matched_filter_lag->delay *= down_sampling_factor_;
  }

  if (old_aggregated_lag_ && aggregated_matched_filter_lag &&
      old_aggregated_lag_->delay == aggregated_matched_filter_lag->delay) {
    ++consistent_estimate_counter_;
  } else {
    consistent_estimate_counter_ = 0;
  }
  old_aggregated_lag_ = aggregated_matched_filter_lag;
  constexpr size_t kNumBlocksPerSecondBy2 = kNumBlocksPerSecond / 2;
  if (consistent_estimate_counter_ > kNumBlocksPerSecondBy2) {
    Reset(true);
  }

  return aggregated_matched_filter_lag;
}

}  // namespace webrtc
