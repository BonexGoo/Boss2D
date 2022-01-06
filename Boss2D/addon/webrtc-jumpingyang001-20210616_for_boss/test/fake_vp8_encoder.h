/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_FAKE_VP8_ENCODER_H_
#define TEST_FAKE_VP8_ENCODER_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>

#include BOSS_WEBRTC_U_api__fec_controller_override_h //original-code:"api/fec_controller_override.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__vp8_frame_buffer_controller_h //original-code:"api/video_codecs/vp8_frame_buffer_controller.h"
#include BOSS_WEBRTC_U_api__video_codecs__vp8_temporal_layers_h //original-code:"api/video_codecs/vp8_temporal_layers.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"
#include BOSS_WEBRTC_U_test__fake_encoder_h //original-code:"test/fake_encoder.h"

namespace webrtc {
namespace test {

class FakeVp8Encoder : public FakeEncoder {
 public:
  explicit FakeVp8Encoder(Clock* clock);
  virtual ~FakeVp8Encoder() = default;

  int32_t InitEncode(const VideoCodec* config,
                     const Settings& settings) override;

  int32_t Release() override;

  EncoderInfo GetEncoderInfo() const override;

 private:
  CodecSpecificInfo PopulateCodecSpecific(size_t size_bytes,
                                          VideoFrameType frame_type,
                                          int stream_idx,
                                          uint32_t timestamp);

  CodecSpecificInfo EncodeHook(
      EncodedImage& encoded_image,
      rtc::scoped_refptr<EncodedImageBuffer> buffer) override;

  SequenceChecker sequence_checker_;

  class FakeFecControllerOverride : public FecControllerOverride {
   public:
    ~FakeFecControllerOverride() override = default;

    void SetFecAllowed(bool fec_allowed) override {}
  };

  FakeFecControllerOverride fec_controller_override_
      RTC_GUARDED_BY(sequence_checker_);

  std::unique_ptr<Vp8FrameBufferController> frame_buffer_controller_
      RTC_GUARDED_BY(sequence_checker_);
};

}  // namespace test
}  // namespace webrtc

#endif  // TEST_FAKE_VP8_ENCODER_H_
