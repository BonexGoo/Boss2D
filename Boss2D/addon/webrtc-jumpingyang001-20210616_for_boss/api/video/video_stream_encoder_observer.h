/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_VIDEO_VIDEO_STREAM_ENCODER_OBSERVER_H_
#define API_VIDEO_VIDEO_STREAM_ENCODER_OBSERVER_H_

#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video__video_adaptation_counters_h //original-code:"api/video/video_adaptation_counters.h"
#include BOSS_WEBRTC_U_api__video__video_adaptation_reason_h //original-code:"api/video/video_adaptation_reason.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_api__video__video_codec_constants_h //original-code:"api/video/video_codec_constants.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_config_h //original-code:"api/video_codecs/video_encoder_config.h"

namespace webrtc {

// TODO(nisse): Used for the OnSendEncodedImage callback below. The callback
// wants metadata such as size, encode timing, qp, but doesn't need actual
// encoded data. So use some other type to represent that.
class EncodedImage;

// Broken out into a base class, with public inheritance below, only to ease
// unit testing of the internal class OveruseFrameDetector.
class CpuOveruseMetricsObserver {
 public:
  virtual ~CpuOveruseMetricsObserver() = default;
  virtual void OnEncodedFrameTimeMeasured(int encode_duration_ms,
                                          int encode_usage_percent) = 0;
};

class VideoStreamEncoderObserver : public CpuOveruseMetricsObserver {
 public:
  struct AdaptationSettings {
    AdaptationSettings()
        : resolution_scaling_enabled(false), framerate_scaling_enabled(false) {}

    AdaptationSettings(bool resolution_scaling_enabled,
                       bool framerate_scaling_enabled)
        : resolution_scaling_enabled(resolution_scaling_enabled),
          framerate_scaling_enabled(framerate_scaling_enabled) {}

    bool resolution_scaling_enabled;
    bool framerate_scaling_enabled;
  };

  // TODO(nisse): Duplicates enum EncodedImageCallback::DropReason.
  enum class DropReason {
    kSource,
    kEncoderQueue,
    kEncoder,
    kMediaOptimization,
    kCongestionWindow
  };

  ~VideoStreamEncoderObserver() override = default;

  virtual void OnIncomingFrame(int width, int height) = 0;

  // TODO(nisse): Merge into one callback per encoded frame.
  using CpuOveruseMetricsObserver::OnEncodedFrameTimeMeasured;
  virtual void OnSendEncodedImage(const EncodedImage& encoded_image,
                                  const CodecSpecificInfo* codec_info) = 0;

  virtual void OnEncoderImplementationChanged(
      const std::string& implementation_name) = 0;

  virtual void OnFrameDropped(DropReason reason) = 0;

  // Used to indicate change in content type, which may require a change in
  // how stats are collected and set the configured preferred media bitrate.
  virtual void OnEncoderReconfigured(
      const VideoEncoderConfig& encoder_config,
      const std::vector<VideoStream>& streams) = 0;

  virtual void OnAdaptationChanged(
      VideoAdaptationReason reason,
      const VideoAdaptationCounters& cpu_steps,
      const VideoAdaptationCounters& quality_steps) = 0;
  virtual void ClearAdaptationStats() = 0;

  virtual void UpdateAdaptationSettings(
      AdaptationSettings cpu_settings,
      AdaptationSettings quality_settings) = 0;
  virtual void OnMinPixelLimitReached() = 0;
  virtual void OnInitialQualityResolutionAdaptDown() = 0;

  virtual void OnSuspendChange(bool is_suspended) = 0;

  virtual void OnBitrateAllocationUpdated(
      const VideoCodec& codec,
      const VideoBitrateAllocation& allocation) {}

  // Informes observer if an internal encoder scaler has reduced video
  // resolution or not. |is_scaled| is a flag indicating if the video is scaled
  // down.
  virtual void OnEncoderInternalScalerUpdate(bool is_scaled) {}

  // TODO(nisse): VideoStreamEncoder wants to query the stats, which makes this
  // not a pure observer. GetInputFrameRate is needed for the cpu adaptation, so
  // can be deleted if that responsibility is moved out to a VideoStreamAdaptor
  // class.
  virtual int GetInputFrameRate() const = 0;
};

}  // namespace webrtc
#endif  // API_VIDEO_VIDEO_STREAM_ENCODER_OBSERVER_H_
