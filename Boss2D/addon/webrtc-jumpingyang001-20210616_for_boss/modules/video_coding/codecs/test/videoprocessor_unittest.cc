/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/video_coding/codecs/test/videoprocessor.h"

#include <memory>

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__test__mock_video_decoder_h //original-code:"api/test/mock_video_decoder.h"
#include BOSS_WEBRTC_U_api__test__mock_video_encoder_h //original-code:"api/test/mock_video_encoder.h"
#include BOSS_WEBRTC_U_api__test__videocodec_test_fixture_h //original-code:"api/test/videocodec_test_fixture.h"
#include BOSS_WEBRTC_U_api__video__i420_buffer_h //original-code:"api/video/i420_buffer.h"
#include BOSS_WEBRTC_U_media__base__media_constants_h //original-code:"media/base/media_constants.h"
#include "modules/video_coding/codecs/test/videocodec_test_stats_impl.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_for_test_h //original-code:"rtc_base/task_queue_for_test.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include "test/testsupport/mock/mock_frame_reader.h"

using ::testing::_;
using ::testing::AllOf;
using ::testing::Field;
using ::testing::Property;
using ::testing::ResultOf;
using ::testing::Return;

namespace webrtc {
namespace test {

namespace {

const int kWidth = 352;
const int kHeight = 288;
const int kFrameSize = kWidth * kHeight * 3 / 2;  // I420.

}  // namespace

class VideoProcessorTest : public ::testing::Test {
 protected:
  VideoProcessorTest() : q_("VP queue") {
    config_.SetCodecSettings(cricket::kVp8CodecName, 1, 1, 1, false, false,
                             false, kWidth, kHeight);

    decoder_mock_ = new MockVideoDecoder();
    decoders_.push_back(std::unique_ptr<VideoDecoder>(decoder_mock_));

    ExpectInit();
    EXPECT_CALL(frame_reader_mock_, FrameLength())
        .WillRepeatedly(Return(kFrameSize));
    q_.SendTask(
        [this] {
          video_processor_ = std::make_unique<VideoProcessor>(
              &encoder_mock_, &decoders_, &frame_reader_mock_, config_, &stats_,
              &encoded_frame_writers_, /*decoded_frame_writers=*/nullptr);
        },
        RTC_FROM_HERE);
  }

  ~VideoProcessorTest() {
    q_.SendTask([this] { video_processor_.reset(); }, RTC_FROM_HERE);
  }

  void ExpectInit() {
    EXPECT_CALL(encoder_mock_, InitEncode(_, _)).Times(1);
    EXPECT_CALL(encoder_mock_, RegisterEncodeCompleteCallback(_)).Times(1);
    EXPECT_CALL(*decoder_mock_, InitDecode(_, _)).Times(1);
    EXPECT_CALL(*decoder_mock_, RegisterDecodeCompleteCallback(_)).Times(1);
  }

  void ExpectRelease() {
    EXPECT_CALL(encoder_mock_, Release()).Times(1);
    EXPECT_CALL(encoder_mock_, RegisterEncodeCompleteCallback(_)).Times(1);
    EXPECT_CALL(*decoder_mock_, Release()).Times(1);
    EXPECT_CALL(*decoder_mock_, RegisterDecodeCompleteCallback(_)).Times(1);
  }

  TaskQueueForTest q_;

  VideoCodecTestFixture::Config config_;

  MockVideoEncoder encoder_mock_;
  MockVideoDecoder* decoder_mock_;
  std::vector<std::unique_ptr<VideoDecoder>> decoders_;
  MockFrameReader frame_reader_mock_;
  VideoCodecTestStatsImpl stats_;
  VideoProcessor::IvfFileWriterMap encoded_frame_writers_;
  std::unique_ptr<VideoProcessor> video_processor_;
};

TEST_F(VideoProcessorTest, InitRelease) {
  ExpectRelease();
}

TEST_F(VideoProcessorTest, ProcessFrames_FixedFramerate) {
  const int kBitrateKbps = 456;
  const int kFramerateFps = 31;
  EXPECT_CALL(
      encoder_mock_,
      SetRates(Field(&VideoEncoder::RateControlParameters::framerate_fps,
                     static_cast<double>(kFramerateFps))))
      .Times(1);
  q_.SendTask([=] { video_processor_->SetRates(kBitrateKbps, kFramerateFps); },
              RTC_FROM_HERE);

  EXPECT_CALL(frame_reader_mock_, ReadFrame())
      .WillRepeatedly(Return(I420Buffer::Create(kWidth, kHeight)));
  EXPECT_CALL(
      encoder_mock_,
      Encode(Property(&VideoFrame::timestamp, 1 * 90000 / kFramerateFps), _))
      .Times(1);
  q_.SendTask([this] { video_processor_->ProcessFrame(); }, RTC_FROM_HERE);

  EXPECT_CALL(
      encoder_mock_,
      Encode(Property(&VideoFrame::timestamp, 2 * 90000 / kFramerateFps), _))
      .Times(1);
  q_.SendTask([this] { video_processor_->ProcessFrame(); }, RTC_FROM_HERE);

  ExpectRelease();
}

TEST_F(VideoProcessorTest, ProcessFrames_VariableFramerate) {
  const int kBitrateKbps = 456;
  const int kStartFramerateFps = 27;
  const int kStartTimestamp = 90000 / kStartFramerateFps;
  EXPECT_CALL(
      encoder_mock_,
      SetRates(Field(&VideoEncoder::RateControlParameters::framerate_fps,
                     static_cast<double>(kStartFramerateFps))))
      .Times(1);
  q_.SendTask(
      [=] { video_processor_->SetRates(kBitrateKbps, kStartFramerateFps); },
      RTC_FROM_HERE);

  EXPECT_CALL(frame_reader_mock_, ReadFrame())
      .WillRepeatedly(Return(I420Buffer::Create(kWidth, kHeight)));
  EXPECT_CALL(encoder_mock_,
              Encode(Property(&VideoFrame::timestamp, kStartTimestamp), _))
      .Times(1);
  q_.SendTask([this] { video_processor_->ProcessFrame(); }, RTC_FROM_HERE);

  const int kNewFramerateFps = 13;
  EXPECT_CALL(
      encoder_mock_,
      SetRates(Field(&VideoEncoder::RateControlParameters::framerate_fps,
                     static_cast<double>(kNewFramerateFps))))
      .Times(1);
  q_.SendTask(
      [=] { video_processor_->SetRates(kBitrateKbps, kNewFramerateFps); },
      RTC_FROM_HERE);

  EXPECT_CALL(encoder_mock_,
              Encode(Property(&VideoFrame::timestamp,
                              kStartTimestamp + 90000 / kNewFramerateFps),
                     _))
      .Times(1);
  q_.SendTask([this] { video_processor_->ProcessFrame(); }, RTC_FROM_HERE);

  ExpectRelease();
}

TEST_F(VideoProcessorTest, SetRates) {
  const uint32_t kBitrateKbps = 123;
  const int kFramerateFps = 17;

  EXPECT_CALL(
      encoder_mock_,
      SetRates(AllOf(ResultOf(
                         [](const VideoEncoder::RateControlParameters& params) {
                           return params.bitrate.get_sum_kbps();
                         },
                         kBitrateKbps),
                     Field(&VideoEncoder::RateControlParameters::framerate_fps,
                           static_cast<double>(kFramerateFps)))))
      .Times(1);
  q_.SendTask([=] { video_processor_->SetRates(kBitrateKbps, kFramerateFps); },
              RTC_FROM_HERE);

  const uint32_t kNewBitrateKbps = 456;
  const int kNewFramerateFps = 34;
  EXPECT_CALL(
      encoder_mock_,
      SetRates(AllOf(ResultOf(
                         [](const VideoEncoder::RateControlParameters& params) {
                           return params.bitrate.get_sum_kbps();
                         },
                         kNewBitrateKbps),
                     Field(&VideoEncoder::RateControlParameters::framerate_fps,
                           static_cast<double>(kNewFramerateFps)))))
      .Times(1);
  q_.SendTask(
      [=] { video_processor_->SetRates(kNewBitrateKbps, kNewFramerateFps); },
      RTC_FROM_HERE);

  ExpectRelease();
}

}  // namespace test
}  // namespace webrtc
