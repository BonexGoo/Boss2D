/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_MOCK_FRAME_TRANSFORMER_H_
#define TEST_MOCK_FRAME_TRANSFORMER_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__frame_transformer_interface_h //original-code:"api/frame_transformer_interface.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockFrameTransformer : public FrameTransformerInterface {
 public:
  MOCK_METHOD(void,
              Transform,
              (std::unique_ptr<TransformableFrameInterface>),
              (override));
  MOCK_METHOD(void,
              RegisterTransformedFrameCallback,
              (rtc::scoped_refptr<TransformedFrameCallback>),
              (override));
  MOCK_METHOD(void,
              RegisterTransformedFrameSinkCallback,
              (rtc::scoped_refptr<TransformedFrameCallback>, uint32_t),
              (override));
  MOCK_METHOD(void, UnregisterTransformedFrameCallback, (), (override));
  MOCK_METHOD(void,
              UnregisterTransformedFrameSinkCallback,
              (uint32_t),
              (override));
};

}  // namespace webrtc

#endif  // TEST_MOCK_FRAME_TRANSFORMER_H_
