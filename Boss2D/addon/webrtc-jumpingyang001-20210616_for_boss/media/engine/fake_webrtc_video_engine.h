/*
 *  Copyright (c) 2010 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MEDIA_ENGINE_FAKE_WEBRTC_VIDEO_ENGINE_H_
#define MEDIA_ENGINE_FAKE_WEBRTC_VIDEO_ENGINE_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__fec_controller_override_h //original-code:"api/fec_controller_override.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_factory_h //original-code:"api/video_codecs/video_encoder_factory.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace cricket {

class FakeWebRtcVideoDecoderFactory;
class FakeWebRtcVideoEncoderFactory;

// Fake class for mocking out webrtc::VideoDecoder
class FakeWebRtcVideoDecoder : public webrtc::VideoDecoder {
 public:
  explicit FakeWebRtcVideoDecoder(FakeWebRtcVideoDecoderFactory* factory);
  ~FakeWebRtcVideoDecoder();

  int32_t InitDecode(const webrtc::VideoCodec*, int32_t) override;
  int32_t Decode(const webrtc::EncodedImage&, bool, int64_t) override;
  int32_t RegisterDecodeCompleteCallback(
      webrtc::DecodedImageCallback*) override;
  int32_t Release() override;

  int GetNumFramesReceived() const;

 private:
  int num_frames_received_;
  FakeWebRtcVideoDecoderFactory* factory_;
};

// Fake class for mocking out webrtc::VideoDecoderFactory.
class FakeWebRtcVideoDecoderFactory : public webrtc::VideoDecoderFactory {
 public:
  FakeWebRtcVideoDecoderFactory();

  std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;
  std::unique_ptr<webrtc::VideoDecoder> CreateVideoDecoder(
      const webrtc::SdpVideoFormat& format) override;

  void DecoderDestroyed(FakeWebRtcVideoDecoder* decoder);
  void AddSupportedVideoCodecType(const std::string& name);
  int GetNumCreatedDecoders();
  const std::vector<FakeWebRtcVideoDecoder*>& decoders();

 private:
  std::vector<webrtc::SdpVideoFormat> supported_codec_formats_;
  std::vector<FakeWebRtcVideoDecoder*> decoders_;
  int num_created_decoders_;
};

// Fake class for mocking out webrtc::VideoEnoder
class FakeWebRtcVideoEncoder : public webrtc::VideoEncoder {
 public:
  explicit FakeWebRtcVideoEncoder(FakeWebRtcVideoEncoderFactory* factory);
  ~FakeWebRtcVideoEncoder();

  void SetFecControllerOverride(
      webrtc::FecControllerOverride* fec_controller_override) override;
  int32_t InitEncode(const webrtc::VideoCodec* codecSettings,
                     const VideoEncoder::Settings& settings) override;
  int32_t Encode(
      const webrtc::VideoFrame& inputImage,
      const std::vector<webrtc::VideoFrameType>* frame_types) override;
  int32_t RegisterEncodeCompleteCallback(
      webrtc::EncodedImageCallback* callback) override;
  int32_t Release() override;
  void SetRates(const RateControlParameters& parameters) override;
  webrtc::VideoEncoder::EncoderInfo GetEncoderInfo() const override;

  bool WaitForInitEncode();
  webrtc::VideoCodec GetCodecSettings();
  int GetNumEncodedFrames();

 private:
  webrtc::Mutex mutex_;
  rtc::Event init_encode_event_;
  int num_frames_encoded_ RTC_GUARDED_BY(mutex_);
  webrtc::VideoCodec codec_settings_ RTC_GUARDED_BY(mutex_);
  FakeWebRtcVideoEncoderFactory* factory_;
};

// Fake class for mocking out webrtc::VideoEncoderFactory.
class FakeWebRtcVideoEncoderFactory : public webrtc::VideoEncoderFactory {
 public:
  FakeWebRtcVideoEncoderFactory();

  std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;
  std::unique_ptr<webrtc::VideoEncoder> CreateVideoEncoder(
      const webrtc::SdpVideoFormat& format) override;
  CodecInfo QueryVideoEncoder(
      const webrtc::SdpVideoFormat& format) const override;

  bool WaitForCreatedVideoEncoders(int num_encoders);
  void EncoderDestroyed(FakeWebRtcVideoEncoder* encoder);
  void set_encoders_have_internal_sources(bool internal_source);
  void AddSupportedVideoCodec(const webrtc::SdpVideoFormat& format);
  void AddSupportedVideoCodecType(const std::string& name);
  int GetNumCreatedEncoders();
  const std::vector<FakeWebRtcVideoEncoder*> encoders();

 private:
  webrtc::Mutex mutex_;
  rtc::Event created_video_encoder_event_;
  std::vector<webrtc::SdpVideoFormat> formats_;
  std::vector<FakeWebRtcVideoEncoder*> encoders_ RTC_GUARDED_BY(mutex_);
  int num_created_encoders_ RTC_GUARDED_BY(mutex_);
  bool encoders_have_internal_sources_;
  bool vp8_factory_mode_;
};

}  // namespace cricket

#endif  // MEDIA_ENGINE_FAKE_WEBRTC_VIDEO_ENGINE_H_
