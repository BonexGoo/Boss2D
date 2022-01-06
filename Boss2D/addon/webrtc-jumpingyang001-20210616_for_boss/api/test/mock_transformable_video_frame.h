/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TEST_MOCK_TRANSFORMABLE_VIDEO_FRAME_H_
#define API_TEST_MOCK_TRANSFORMABLE_VIDEO_FRAME_H_

#include <vector>

#include BOSS_WEBRTC_U_api__frame_transformer_interface_h //original-code:"api/frame_transformer_interface.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockTransformableVideoFrame
    : public webrtc::TransformableVideoFrameInterface {
 public:
  MOCK_METHOD(rtc::ArrayView<const uint8_t>, GetData, (), (const override));
  MOCK_METHOD(void, SetData, (rtc::ArrayView<const uint8_t> data), (override));
  MOCK_METHOD(uint32_t, GetTimestamp, (), (const override));
  MOCK_METHOD(uint32_t, GetSsrc, (), (const, override));
  MOCK_METHOD(bool, IsKeyFrame, (), (const, override));
  MOCK_METHOD(std::vector<uint8_t>, GetAdditionalData, (), (const, override));
  MOCK_METHOD(const webrtc::VideoFrameMetadata&,
              GetMetadata,
              (),
              (const, override));
};

}  // namespace webrtc

#endif  // API_TEST_MOCK_TRANSFORMABLE_VIDEO_FRAME_H_
