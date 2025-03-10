/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VIDEO_ENCODER_BITRATE_ADJUSTER_H_
#define VIDEO_ENCODER_BITRATE_ADJUSTER_H_

#include <memory>

#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_video__encoder_overshoot_detector_h //original-code:"video/encoder_overshoot_detector.h"

namespace webrtc {

class EncoderBitrateAdjuster {
 public:
  // Size of sliding window used to track overshoot rate.
  static constexpr int64_t kWindowSizeMs = 3000;
  // Minimum number of frames since last layout change required to trust the
  // overshoot statistics. Otherwise falls back to default utilization.
  // By layout change, we mean any spatial/temporal layer being either enabled
  // or disabled.
  static constexpr size_t kMinFramesSinceLayoutChange = 30;
  // Default utilization, before reliable metrics are available, is set to 20%
  // overshoot. This is conservative so that badly misbehaving encoders don't
  // build too much queue at the very start.
  static constexpr double kDefaultUtilizationFactor = 1.2;

  explicit EncoderBitrateAdjuster(const VideoCodec& codec_settings);
  ~EncoderBitrateAdjuster();

  // Adjusts the given rate allocation to make it paceable within the target
  // rates.
  VideoBitrateAllocation AdjustRateAllocation(
      const VideoEncoder::RateControlParameters& rates);

  // Updated overuse detectors with data about the encoder, specifically about
  // the temporal layer frame rate allocation.
  void OnEncoderInfo(const VideoEncoder::EncoderInfo& encoder_info);

  // Updates the overuse detectors according to the encoded image size.
  void OnEncodedFrame(DataSize size, int spatial_index, int temporal_index);

  void Reset();

 private:
  const bool utilize_bandwidth_headroom_;

  VideoEncoder::RateControlParameters current_rate_control_parameters_;
  // FPS allocation of temporal layers, per spatial layer. Represented as a Q8
  // fraction; 0 = 0%, 255 = 100%. See VideoEncoder::EncoderInfo.fps_allocation.
  absl::InlinedVector<uint8_t, kMaxTemporalStreams>
      current_fps_allocation_[kMaxSpatialLayers];

  // Frames since layout was changed, mean that any spatial or temporal layer
  // was either disabled or enabled.
  size_t frames_since_layout_change_;
  std::unique_ptr<EncoderOvershootDetector>
      overshoot_detectors_[kMaxSpatialLayers][kMaxTemporalStreams];

  // Minimum bitrates allowed, per spatial layer.
  uint32_t min_bitrates_bps_[kMaxSpatialLayers];
};

}  // namespace webrtc

#endif  // VIDEO_ENCODER_BITRATE_ADJUSTER_H_
