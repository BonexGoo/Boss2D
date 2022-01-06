/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VIDEO_ADAPTATION_QUALITY_SCALER_RESOURCE_H_
#define VIDEO_ADAPTATION_QUALITY_SCALER_RESOURCE_H_

#include <memory>
#include <queue>
#include <string>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__video__video_adaptation_reason_h //original-code:"api/video/video_adaptation_reason.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_call__adaptation__degradation_preference_provider_h //original-code:"call/adaptation/degradation_preference_provider.h"
#include BOSS_WEBRTC_U_call__adaptation__resource_adaptation_processor_interface_h //original-code:"call/adaptation/resource_adaptation_processor_interface.h"
#include BOSS_WEBRTC_U_modules__video_coding__utility__quality_scaler_h //original-code:"modules/video_coding/utility/quality_scaler.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"
#include BOSS_WEBRTC_U_video__adaptation__video_stream_encoder_resource_h //original-code:"video/adaptation/video_stream_encoder_resource.h"

namespace webrtc {

// Handles interaction with the QualityScaler.
class QualityScalerResource : public VideoStreamEncoderResource,
                              public QualityScalerQpUsageHandlerInterface {
 public:
  static rtc::scoped_refptr<QualityScalerResource> Create();

  QualityScalerResource();
  ~QualityScalerResource() override;

  bool is_started() const;

  void StartCheckForOveruse(VideoEncoder::QpThresholds qp_thresholds);
  void StopCheckForOveruse();
  void SetQpThresholds(VideoEncoder::QpThresholds qp_thresholds);
  bool QpFastFilterLow();
  void OnEncodeCompleted(const EncodedImage& encoded_image,
                         int64_t time_sent_in_us);
  void OnFrameDropped(EncodedImageCallback::DropReason reason);

  // QualityScalerQpUsageHandlerInterface implementation.
  void OnReportQpUsageHigh() override;
  void OnReportQpUsageLow() override;

 private:
  std::unique_ptr<QualityScaler> quality_scaler_
      RTC_GUARDED_BY(encoder_queue());
};

}  // namespace webrtc

#endif  // VIDEO_ADAPTATION_QUALITY_SCALER_RESOURCE_H_
