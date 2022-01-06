/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#ifndef MEDIA_ENGINE_ENCODER_SIMULCAST_PROXY_H_
#define MEDIA_ENGINE_ENCODER_SIMULCAST_PROXY_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_factory_h //original-code:"api/video_codecs/video_encoder_factory.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"

namespace webrtc {

// This class provides fallback to SimulcastEncoderAdapter if default VP8Encoder
// doesn't support simulcast for provided settings.
class RTC_EXPORT EncoderSimulcastProxy : public VideoEncoder {
 public:
  EncoderSimulcastProxy(VideoEncoderFactory* factory,
                        const SdpVideoFormat& format);
  // Deprecated. Remove once all clients use constructor with both factory and
  // SdpVideoFormat;
  explicit EncoderSimulcastProxy(VideoEncoderFactory* factory);

  ~EncoderSimulcastProxy() override;

  // Implements VideoEncoder.
  int Release() override;
  void SetFecControllerOverride(
      FecControllerOverride* fec_controller_override) override;
  int InitEncode(const VideoCodec* codec_settings,
                 const VideoEncoder::Settings& settings) override;
  int Encode(const VideoFrame& input_image,
             const std::vector<VideoFrameType>* frame_types) override;
  int RegisterEncodeCompleteCallback(EncodedImageCallback* callback) override;
  void SetRates(const RateControlParameters& parameters) override;
  void OnPacketLossRateUpdate(float packet_loss_rate) override;
  void OnRttUpdate(int64_t rtt_ms) override;
  void OnLossNotification(const LossNotification& loss_notification) override;
  EncoderInfo GetEncoderInfo() const override;

 private:
  VideoEncoderFactory* const factory_;
  SdpVideoFormat video_format_;
  std::unique_ptr<VideoEncoder> encoder_;
  EncodedImageCallback* callback_;
};

}  // namespace webrtc

#endif  // MEDIA_ENGINE_ENCODER_SIMULCAST_PROXY_H_
