/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_video__adaptation__quality_scaler_resource_h //original-code:"video/adaptation/quality_scaler_resource.h"

#include <utility>

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__balanced_degradation_settings_h //original-code:"rtc_base/experiments/balanced_degradation_settings.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__to_queued_task_h //original-code:"rtc_base/task_utils/to_queued_task.h"
#include BOSS_WEBRTC_U_rtc_base__time_utils_h //original-code:"rtc_base/time_utils.h"

namespace webrtc {

// static
rtc::scoped_refptr<QualityScalerResource> QualityScalerResource::Create() {
  return rtc::make_ref_counted<QualityScalerResource>();
}

QualityScalerResource::QualityScalerResource()
    : VideoStreamEncoderResource("QualityScalerResource"),
      quality_scaler_(nullptr) {}

QualityScalerResource::~QualityScalerResource() {
  RTC_DCHECK(!quality_scaler_);
}

bool QualityScalerResource::is_started() const {
  RTC_DCHECK_RUN_ON(encoder_queue());
  return quality_scaler_.get();
}

void QualityScalerResource::StartCheckForOveruse(
    VideoEncoder::QpThresholds qp_thresholds) {
  RTC_DCHECK_RUN_ON(encoder_queue());
  RTC_DCHECK(!is_started());
  quality_scaler_ =
      std::make_unique<QualityScaler>(this, std::move(qp_thresholds));
}

void QualityScalerResource::StopCheckForOveruse() {
  RTC_DCHECK_RUN_ON(encoder_queue());
  RTC_DCHECK(is_started());
  // Ensure we have no pending callbacks. This makes it safe to destroy the
  // QualityScaler and even task queues with tasks in-flight.
  quality_scaler_.reset();
}

void QualityScalerResource::SetQpThresholds(
    VideoEncoder::QpThresholds qp_thresholds) {
  RTC_DCHECK_RUN_ON(encoder_queue());
  RTC_DCHECK(is_started());
  quality_scaler_->SetQpThresholds(std::move(qp_thresholds));
}

bool QualityScalerResource::QpFastFilterLow() {
  RTC_DCHECK_RUN_ON(encoder_queue());
  RTC_DCHECK(is_started());
  return quality_scaler_->QpFastFilterLow();
}

void QualityScalerResource::OnEncodeCompleted(const EncodedImage& encoded_image,
                                              int64_t time_sent_in_us) {
  RTC_DCHECK_RUN_ON(encoder_queue());
  if (quality_scaler_ && encoded_image.qp_ >= 0) {
    quality_scaler_->ReportQp(encoded_image.qp_, time_sent_in_us);
  }
}

void QualityScalerResource::OnFrameDropped(
    EncodedImageCallback::DropReason reason) {
  RTC_DCHECK_RUN_ON(encoder_queue());
  if (!quality_scaler_)
    return;
  switch (reason) {
    case EncodedImageCallback::DropReason::kDroppedByMediaOptimizations:
      quality_scaler_->ReportDroppedFrameByMediaOpt();
      break;
    case EncodedImageCallback::DropReason::kDroppedByEncoder:
      quality_scaler_->ReportDroppedFrameByEncoder();
      break;
  }
}

void QualityScalerResource::OnReportQpUsageHigh() {
  OnResourceUsageStateMeasured(ResourceUsageState::kOveruse);
}

void QualityScalerResource::OnReportQpUsageLow() {
  OnResourceUsageStateMeasured(ResourceUsageState::kUnderuse);
}

}  // namespace webrtc
