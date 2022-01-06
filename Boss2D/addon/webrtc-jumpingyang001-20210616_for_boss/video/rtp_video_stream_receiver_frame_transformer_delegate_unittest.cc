/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_video__rtp_video_stream_receiver_frame_transformer_delegate_h //original-code:"video/rtp_video_stream_receiver_frame_transformer_delegate.h"

#include <cstdio>
#include <memory>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__call__transport_h //original-code:"api/call/transport.h"
#include BOSS_WEBRTC_U_call__video_receive_stream_h //original-code:"call/video_receive_stream.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_descriptor_authentication_h //original-code:"modules/rtp_rtcp/source/rtp_descriptor_authentication.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__to_queued_task_h //original-code:"rtc_base/task_utils/to_queued_task.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_test__mock_frame_transformer_h //original-code:"test/mock_frame_transformer.h"

namespace webrtc {
namespace {

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::NiceMock;
using ::testing::SaveArg;

std::unique_ptr<RtpFrameObject> CreateRtpFrameObject(
    const RTPVideoHeader& video_header) {
  return std::make_unique<RtpFrameObject>(
      0, 0, true, 0, 0, 0, 0, 0, VideoSendTiming(), 0, video_header.codec,
      kVideoRotation_0, VideoContentType::UNSPECIFIED, video_header,
      absl::nullopt, RtpPacketInfos(), EncodedImageBuffer::Create(0));
}

std::unique_ptr<RtpFrameObject> CreateRtpFrameObject() {
  return CreateRtpFrameObject(RTPVideoHeader());
}

class TestRtpVideoFrameReceiver : public RtpVideoFrameReceiver {
 public:
  TestRtpVideoFrameReceiver() {}
  ~TestRtpVideoFrameReceiver() override = default;

  MOCK_METHOD(void,
              ManageFrame,
              (std::unique_ptr<RtpFrameObject> frame),
              (override));
};

TEST(RtpVideoStreamReceiverFrameTransformerDelegateTest,
     RegisterTransformedFrameCallbackSinkOnInit) {
  TestRtpVideoFrameReceiver receiver;
  auto frame_transformer(rtc::make_ref_counted<MockFrameTransformer>());
  auto delegate(
      rtc::make_ref_counted<RtpVideoStreamReceiverFrameTransformerDelegate>(
          &receiver, frame_transformer, rtc::Thread::Current(),
          /*remote_ssrc*/ 1111));
  EXPECT_CALL(*frame_transformer,
              RegisterTransformedFrameSinkCallback(testing::_, 1111));
  delegate->Init();
}

TEST(RtpVideoStreamReceiverFrameTransformerDelegateTest,
     UnregisterTransformedFrameSinkCallbackOnReset) {
  TestRtpVideoFrameReceiver receiver;
  auto frame_transformer(rtc::make_ref_counted<MockFrameTransformer>());
  auto delegate(
      rtc::make_ref_counted<RtpVideoStreamReceiverFrameTransformerDelegate>(
          &receiver, frame_transformer, rtc::Thread::Current(),
          /*remote_ssrc*/ 1111));
  EXPECT_CALL(*frame_transformer, UnregisterTransformedFrameSinkCallback(1111));
  delegate->Reset();
}

TEST(RtpVideoStreamReceiverFrameTransformerDelegateTest, TransformFrame) {
  TestRtpVideoFrameReceiver receiver;
  auto frame_transformer(
      rtc::make_ref_counted<testing::NiceMock<MockFrameTransformer>>());
  auto delegate(
      rtc::make_ref_counted<RtpVideoStreamReceiverFrameTransformerDelegate>(
          &receiver, frame_transformer, rtc::Thread::Current(),
          /*remote_ssrc*/ 1111));
  auto frame = CreateRtpFrameObject();
  EXPECT_CALL(*frame_transformer, Transform);
  delegate->TransformFrame(std::move(frame));
}

TEST(RtpVideoStreamReceiverFrameTransformerDelegateTest,
     ManageFrameOnTransformedFrame) {
  TestRtpVideoFrameReceiver receiver;
  auto mock_frame_transformer(
      rtc::make_ref_counted<NiceMock<MockFrameTransformer>>());
  auto delegate =
      rtc::make_ref_counted<RtpVideoStreamReceiverFrameTransformerDelegate>(
          &receiver, mock_frame_transformer, rtc::Thread::Current(),
          /*remote_ssrc*/ 1111);

  rtc::scoped_refptr<TransformedFrameCallback> callback;
  EXPECT_CALL(*mock_frame_transformer, RegisterTransformedFrameSinkCallback)
      .WillOnce(SaveArg<0>(&callback));
  delegate->Init();
  ASSERT_TRUE(callback);

  EXPECT_CALL(receiver, ManageFrame);
  ON_CALL(*mock_frame_transformer, Transform)
      .WillByDefault(
          [&callback](std::unique_ptr<TransformableFrameInterface> frame) {
            callback->OnTransformedFrame(std::move(frame));
          });
  delegate->TransformFrame(CreateRtpFrameObject());
  rtc::ThreadManager::ProcessAllMessageQueuesForTesting();
}

TEST(RtpVideoStreamReceiverFrameTransformerDelegateTest,
     TransformableFrameMetadataHasCorrectValue) {
  TestRtpVideoFrameReceiver receiver;
  auto mock_frame_transformer =
      rtc::make_ref_counted<NiceMock<MockFrameTransformer>>();
  auto delegate =
      rtc::make_ref_counted<RtpVideoStreamReceiverFrameTransformerDelegate>(
          &receiver, mock_frame_transformer, rtc::Thread::Current(), 1111);
  delegate->Init();
  RTPVideoHeader video_header;
  video_header.width = 1280u;
  video_header.height = 720u;
  RTPVideoHeader::GenericDescriptorInfo& generic =
      video_header.generic.emplace();
  generic.frame_id = 10;
  generic.temporal_index = 3;
  generic.spatial_index = 2;
  generic.decode_target_indications = {DecodeTargetIndication::kSwitch};
  generic.dependencies = {5};

  // Check that the transformable frame passed to the frame transformer has the
  // correct metadata.
  EXPECT_CALL(*mock_frame_transformer, Transform)
      .WillOnce(
          [](std::unique_ptr<TransformableFrameInterface> transformable_frame) {
            auto frame =
                absl::WrapUnique(static_cast<TransformableVideoFrameInterface*>(
                    transformable_frame.release()));
            ASSERT_TRUE(frame);
            auto metadata = frame->GetMetadata();
            EXPECT_EQ(metadata.GetWidth(), 1280u);
            EXPECT_EQ(metadata.GetHeight(), 720u);
            EXPECT_EQ(metadata.GetFrameId(), 10);
            EXPECT_EQ(metadata.GetTemporalIndex(), 3);
            EXPECT_EQ(metadata.GetSpatialIndex(), 2);
            EXPECT_THAT(metadata.GetFrameDependencies(), ElementsAre(5));
            EXPECT_THAT(metadata.GetDecodeTargetIndications(),
                        ElementsAre(DecodeTargetIndication::kSwitch));
          });
  // The delegate creates a transformable frame from the RtpFrameObject.
  delegate->TransformFrame(CreateRtpFrameObject(video_header));
}

}  // namespace
}  // namespace webrtc
