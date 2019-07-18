/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_RENDER_DELAY_CONTROLLER_H_
#define MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_RENDER_DELAY_CONTROLLER_H_

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__optional_h //original-code:"api/optional.h"
#include "modules/audio_processing/aec3/downsampled_render_buffer.h"
#include "modules/audio_processing/aec3/render_delay_controller.h"
#include "test/gmock.h"

namespace webrtc {
namespace test {

class MockRenderDelayController : public RenderDelayController {
 public:
  virtual ~MockRenderDelayController() = default;

  MOCK_METHOD0(Reset, void());
  MOCK_METHOD0(LogRenderCall, void());
  MOCK_METHOD2(
      GetDelay,
      rtc::Optional<DelayEstimate>(const DownsampledRenderBuffer& render_buffer,
                                   rtc::ArrayView<const float> capture));
};

}  // namespace test
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_RENDER_DELAY_CONTROLLER_H_
