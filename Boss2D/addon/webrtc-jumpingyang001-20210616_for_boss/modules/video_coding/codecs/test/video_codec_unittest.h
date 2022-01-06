/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_TEST_VIDEO_CODEC_UNITTEST_H_
#define MODULES_VIDEO_CODING_CODECS_TEST_VIDEO_CODEC_UNITTEST_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__test__frame_generator_interface_h //original-code:"api/test/frame_generator_interface.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include BOSS_WEBRTC_U_modules__video_coding__utility__vp8_header_parser_h //original-code:"modules/video_coding/utility/vp8_header_parser.h"
#include BOSS_WEBRTC_U_modules__video_coding__utility__vp9_uncompressed_header_parser_h //original-code:"modules/video_coding/utility/vp9_uncompressed_header_parser.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

class VideoCodecUnitTest : public ::testing::Test {
 public:
  VideoCodecUnitTest()
      : encode_complete_callback_(this),
        decode_complete_callback_(this),
        wait_for_encoded_frames_threshold_(1),
        last_input_frame_timestamp_(0) {}

 protected:
  class FakeEncodeCompleteCallback : public webrtc::EncodedImageCallback {
   public:
    explicit FakeEncodeCompleteCallback(VideoCodecUnitTest* test)
        : test_(test) {}

    Result OnEncodedImage(const EncodedImage& frame,
                          const CodecSpecificInfo* codec_specific_info);

   private:
    VideoCodecUnitTest* const test_;
  };

  class FakeDecodeCompleteCallback : public webrtc::DecodedImageCallback {
   public:
    explicit FakeDecodeCompleteCallback(VideoCodecUnitTest* test)
        : test_(test) {}

    int32_t Decoded(VideoFrame& frame) override {
      RTC_NOTREACHED();
      return -1;
    }
    int32_t Decoded(VideoFrame& frame, int64_t decode_time_ms) override {
      RTC_NOTREACHED();
      return -1;
    }
    void Decoded(VideoFrame& frame,
                 absl::optional<int32_t> decode_time_ms,
                 absl::optional<uint8_t> qp) override;

   private:
    VideoCodecUnitTest* const test_;
  };

  virtual std::unique_ptr<VideoEncoder> CreateEncoder() = 0;
  virtual std::unique_ptr<VideoDecoder> CreateDecoder() = 0;

  void SetUp() override;

  virtual void ModifyCodecSettings(VideoCodec* codec_settings);

  VideoFrame NextInputFrame();

  // Helper method for waiting a single encoded frame.
  bool WaitForEncodedFrame(EncodedImage* frame,
                           CodecSpecificInfo* codec_specific_info);

  // Helper methods for waiting for multiple encoded frames. Caller must
  // define how many frames are to be waited for via |num_frames| before calling
  // Encode(). Then, they can expect to retrive them via WaitForEncodedFrames().
  void SetWaitForEncodedFramesThreshold(size_t num_frames);
  bool WaitForEncodedFrames(
      std::vector<EncodedImage>* frames,
      std::vector<CodecSpecificInfo>* codec_specific_info);

  // Helper method for waiting a single decoded frame.
  bool WaitForDecodedFrame(std::unique_ptr<VideoFrame>* frame,
                           absl::optional<uint8_t>* qp);

  size_t GetNumEncodedFrames();

  VideoCodec codec_settings_;

  std::unique_ptr<VideoEncoder> encoder_;
  std::unique_ptr<VideoDecoder> decoder_;
  std::unique_ptr<test::FrameGeneratorInterface> input_frame_generator_;

 private:
  FakeEncodeCompleteCallback encode_complete_callback_;
  FakeDecodeCompleteCallback decode_complete_callback_;

  rtc::Event encoded_frame_event_;
  Mutex encoded_frame_section_;
  size_t wait_for_encoded_frames_threshold_;
  std::vector<EncodedImage> encoded_frames_
      RTC_GUARDED_BY(encoded_frame_section_);
  std::vector<CodecSpecificInfo> codec_specific_infos_
      RTC_GUARDED_BY(encoded_frame_section_);

  rtc::Event decoded_frame_event_;
  Mutex decoded_frame_section_;
  absl::optional<VideoFrame> decoded_frame_
      RTC_GUARDED_BY(decoded_frame_section_);
  absl::optional<uint8_t> decoded_qp_ RTC_GUARDED_BY(decoded_frame_section_);

  uint32_t last_input_frame_timestamp_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_TEST_VIDEO_CODEC_UNITTEST_H_
