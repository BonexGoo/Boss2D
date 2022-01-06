/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__render_delay_controller_h //original-code:"modules/audio_processing/aec3/render_delay_controller.h"

#include <stddef.h>

#include <algorithm>
#include <memory>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_common_h //original-code:"modules/audio_processing/aec3/aec3_common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__delay_estimate_h //original-code:"modules/audio_processing/aec3/delay_estimate.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__downsampled_render_buffer_h //original-code:"modules/audio_processing/aec3/downsampled_render_buffer.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__echo_path_delay_estimator_h //original-code:"modules/audio_processing/aec3/echo_path_delay_estimator.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__render_delay_controller_metrics_h //original-code:"modules/audio_processing/aec3/render_delay_controller_metrics.h"
#include BOSS_WEBRTC_U_modules__audio_processing__logging__apm_data_dumper_h //original-code:"modules/audio_processing/logging/apm_data_dumper.h"
#include BOSS_WEBRTC_U_rtc_base__atomic_ops_h //original-code:"rtc_base/atomic_ops.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

namespace {

class RenderDelayControllerImpl final : public RenderDelayController {
 public:
  RenderDelayControllerImpl(const EchoCanceller3Config& config,
                            int sample_rate_hz,
                            size_t num_capture_channels);

  RenderDelayControllerImpl() = delete;
  RenderDelayControllerImpl(const RenderDelayControllerImpl&) = delete;
  RenderDelayControllerImpl& operator=(const RenderDelayControllerImpl&) =
      delete;

  ~RenderDelayControllerImpl() override;
  void Reset(bool reset_delay_confidence) override;
  void LogRenderCall() override;
  absl::optional<DelayEstimate> GetDelay(
      const DownsampledRenderBuffer& render_buffer,
      size_t render_delay_buffer_delay,
      const std::vector<std::vector<float>>& capture) override;
  bool HasClockdrift() const override;

 private:
  static int instance_count_;
  std::unique_ptr<ApmDataDumper> data_dumper_;
  const int hysteresis_limit_blocks_;
  const int delay_headroom_samples_;
  absl::optional<DelayEstimate> delay_;
  EchoPathDelayEstimator delay_estimator_;
  RenderDelayControllerMetrics metrics_;
  absl::optional<DelayEstimate> delay_samples_;
  size_t capture_call_counter_ = 0;
  int delay_change_counter_ = 0;
  DelayEstimate::Quality last_delay_estimate_quality_;
};

DelayEstimate ComputeBufferDelay(
    const absl::optional<DelayEstimate>& current_delay,
    int hysteresis_limit_blocks,
    int delay_headroom_samples,
    DelayEstimate estimated_delay) {
  // Subtract delay headroom.
  const int delay_with_headroom_samples = std::max(
      static_cast<int>(estimated_delay.delay) - delay_headroom_samples, 0);

  // Compute the buffer delay increase required to achieve the desired latency.
  size_t new_delay_blocks = delay_with_headroom_samples >> kBlockSizeLog2;

  // Add hysteresis.
  if (current_delay) {
    size_t current_delay_blocks = current_delay->delay;
    if (new_delay_blocks > current_delay_blocks &&
        new_delay_blocks <= current_delay_blocks + hysteresis_limit_blocks) {
      new_delay_blocks = current_delay_blocks;
    }
  }

  DelayEstimate new_delay = estimated_delay;
  new_delay.delay = new_delay_blocks;
  return new_delay;
}

int RenderDelayControllerImpl::instance_count_ = 0;

RenderDelayControllerImpl::RenderDelayControllerImpl(
    const EchoCanceller3Config& config,
    int sample_rate_hz,
    size_t num_capture_channels)
    : data_dumper_(
          new ApmDataDumper(rtc::AtomicOps::Increment(&instance_count_))),
      hysteresis_limit_blocks_(
          static_cast<int>(config.delay.hysteresis_limit_blocks)),
      delay_headroom_samples_(config.delay.delay_headroom_samples),
      delay_estimator_(data_dumper_.get(), config, num_capture_channels),
      last_delay_estimate_quality_(DelayEstimate::Quality::kCoarse) {
  RTC_DCHECK(ValidFullBandRate(sample_rate_hz));
  delay_estimator_.LogDelayEstimationProperties(sample_rate_hz, 0);
}

RenderDelayControllerImpl::~RenderDelayControllerImpl() = default;

void RenderDelayControllerImpl::Reset(bool reset_delay_confidence) {
  delay_ = absl::nullopt;
  delay_samples_ = absl::nullopt;
  delay_estimator_.Reset(reset_delay_confidence);
  delay_change_counter_ = 0;
  if (reset_delay_confidence) {
    last_delay_estimate_quality_ = DelayEstimate::Quality::kCoarse;
  }
}

void RenderDelayControllerImpl::LogRenderCall() {}

absl::optional<DelayEstimate> RenderDelayControllerImpl::GetDelay(
    const DownsampledRenderBuffer& render_buffer,
    size_t render_delay_buffer_delay,
    const std::vector<std::vector<float>>& capture) {
  RTC_DCHECK_EQ(kBlockSize, capture[0].size());
  ++capture_call_counter_;

  auto delay_samples = delay_estimator_.EstimateDelay(render_buffer, capture);

  if (delay_samples) {
    if (!delay_samples_ || delay_samples->delay != delay_samples_->delay) {
      delay_change_counter_ = 0;
    }
    if (delay_samples_) {
      delay_samples_->blocks_since_last_change =
          delay_samples_->delay == delay_samples->delay
              ? delay_samples_->blocks_since_last_change + 1
              : 0;
      delay_samples_->blocks_since_last_update = 0;
      delay_samples_->delay = delay_samples->delay;
      delay_samples_->quality = delay_samples->quality;
    } else {
      delay_samples_ = delay_samples;
    }
  } else {
    if (delay_samples_) {
      ++delay_samples_->blocks_since_last_change;
      ++delay_samples_->blocks_since_last_update;
    }
  }

  if (delay_change_counter_ < 2 * kNumBlocksPerSecond) {
    ++delay_change_counter_;
  }

  if (delay_samples_) {
    // Compute the render delay buffer delay.
    const bool use_hysteresis =
        last_delay_estimate_quality_ == DelayEstimate::Quality::kRefined &&
        delay_samples_->quality == DelayEstimate::Quality::kRefined;
    delay_ = ComputeBufferDelay(delay_,
                                use_hysteresis ? hysteresis_limit_blocks_ : 0,
                                delay_headroom_samples_, *delay_samples_);
    last_delay_estimate_quality_ = delay_samples_->quality;
  }

  metrics_.Update(delay_samples_ ? absl::optional<size_t>(delay_samples_->delay)
                                 : absl::nullopt,
                  delay_ ? delay_->delay : 0, 0, delay_estimator_.Clockdrift());

  data_dumper_->DumpRaw("aec3_render_delay_controller_delay",
                        delay_samples ? delay_samples->delay : 0);
  data_dumper_->DumpRaw("aec3_render_delay_controller_buffer_delay",
                        delay_ ? delay_->delay : 0);

  return delay_;
}

bool RenderDelayControllerImpl::HasClockdrift() const {
  return delay_estimator_.Clockdrift() != ClockdriftDetector::Level::kNone;
}

}  // namespace

RenderDelayController* RenderDelayController::Create(
    const EchoCanceller3Config& config,
    int sample_rate_hz,
    size_t num_capture_channels) {
  return new RenderDelayControllerImpl(config, sample_rate_hz,
                                       num_capture_channels);
}

}  // namespace webrtc
