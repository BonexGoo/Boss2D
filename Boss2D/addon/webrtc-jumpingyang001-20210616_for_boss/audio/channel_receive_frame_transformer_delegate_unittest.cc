/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_audio__channel_receive_frame_transformer_delegate_h //original-code:"audio/channel_receive_frame_transformer_delegate.h"

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_test__mock_frame_transformer_h //original-code:"test/mock_frame_transformer.h"
#include BOSS_WEBRTC_U_test__mock_transformable_frame_h //original-code:"test/mock_transformable_frame.h"

namespace webrtc {
namespace {

using ::testing::NiceMock;
using ::testing::SaveArg;

class MockChannelReceive {
 public:
  MOCK_METHOD(void,
              ReceiveFrame,
              (rtc::ArrayView<const uint8_t> packet, const RTPHeader& header));

  ChannelReceiveFrameTransformerDelegate::ReceiveFrameCallback callback() {
    return [this](rtc::ArrayView<const uint8_t> packet,
                  const RTPHeader& header) { ReceiveFrame(packet, header); };
  }
};

// Test that the delegate registers itself with the frame transformer on Init().
TEST(ChannelReceiveFrameTransformerDelegateTest,
     RegisterTransformedFrameCallbackOnInit) {
  rtc::scoped_refptr<MockFrameTransformer> mock_frame_transformer =
      rtc::make_ref_counted<MockFrameTransformer>();
  rtc::scoped_refptr<ChannelReceiveFrameTransformerDelegate> delegate =
      rtc::make_ref_counted<ChannelReceiveFrameTransformerDelegate>(
          ChannelReceiveFrameTransformerDelegate::ReceiveFrameCallback(),
          mock_frame_transformer, nullptr);
  EXPECT_CALL(*mock_frame_transformer, RegisterTransformedFrameCallback);
  delegate->Init();
}

// Test that the delegate unregisters itself from the frame transformer on
// Reset().
TEST(ChannelReceiveFrameTransformerDelegateTest,
     UnregisterTransformedFrameCallbackOnReset) {
  rtc::scoped_refptr<MockFrameTransformer> mock_frame_transformer =
      rtc::make_ref_counted<MockFrameTransformer>();
  rtc::scoped_refptr<ChannelReceiveFrameTransformerDelegate> delegate =
      rtc::make_ref_counted<ChannelReceiveFrameTransformerDelegate>(
          ChannelReceiveFrameTransformerDelegate::ReceiveFrameCallback(),
          mock_frame_transformer, nullptr);
  EXPECT_CALL(*mock_frame_transformer, UnregisterTransformedFrameCallback);
  delegate->Reset();
}

// Test that when the delegate receives a transformed frame from the frame
// transformer, it passes it to the channel using the ReceiveFrameCallback.
TEST(ChannelReceiveFrameTransformerDelegateTest,
     TransformRunsChannelReceiveCallback) {
  rtc::scoped_refptr<MockFrameTransformer> mock_frame_transformer =
      rtc::make_ref_counted<NiceMock<MockFrameTransformer>>();
  MockChannelReceive mock_channel;
  rtc::scoped_refptr<ChannelReceiveFrameTransformerDelegate> delegate =
      rtc::make_ref_counted<ChannelReceiveFrameTransformerDelegate>(
          mock_channel.callback(), mock_frame_transformer,
          rtc::Thread::Current());
  rtc::scoped_refptr<TransformedFrameCallback> callback;
  EXPECT_CALL(*mock_frame_transformer, RegisterTransformedFrameCallback)
      .WillOnce(SaveArg<0>(&callback));
  delegate->Init();
  ASSERT_TRUE(callback);

  const uint8_t data[] = {1, 2, 3, 4};
  rtc::ArrayView<const uint8_t> packet(data, sizeof(data));
  RTPHeader header;
  EXPECT_CALL(mock_channel, ReceiveFrame);
  ON_CALL(*mock_frame_transformer, Transform)
      .WillByDefault(
          [&callback](std::unique_ptr<TransformableFrameInterface> frame) {
            callback->OnTransformedFrame(std::move(frame));
          });
  delegate->Transform(packet, header, 1111 /*ssrc*/);
  rtc::ThreadManager::ProcessAllMessageQueuesForTesting();
}

// Test that if the delegate receives a transformed frame after it has been
// reset, it does not run the ReceiveFrameCallback, as the channel is destroyed
// after resetting the delegate.
TEST(ChannelReceiveFrameTransformerDelegateTest,
     OnTransformedDoesNotRunChannelReceiveCallbackAfterReset) {
  rtc::scoped_refptr<MockFrameTransformer> mock_frame_transformer =
      rtc::make_ref_counted<testing::NiceMock<MockFrameTransformer>>();
  MockChannelReceive mock_channel;
  rtc::scoped_refptr<ChannelReceiveFrameTransformerDelegate> delegate =
      rtc::make_ref_counted<ChannelReceiveFrameTransformerDelegate>(
          mock_channel.callback(), mock_frame_transformer,
          rtc::Thread::Current());

  delegate->Reset();
  EXPECT_CALL(mock_channel, ReceiveFrame).Times(0);
  delegate->OnTransformedFrame(std::make_unique<MockTransformableFrame>());
  rtc::ThreadManager::ProcessAllMessageQueuesForTesting();
}

}  // namespace
}  // namespace webrtc
