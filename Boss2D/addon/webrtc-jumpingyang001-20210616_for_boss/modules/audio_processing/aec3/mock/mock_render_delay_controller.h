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

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__downsampled_render_buffer_h //original-code:"modules/audio_processing/aec3/downsampled_render_buffer.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__render_delay_controller_h //original-code:"modules/audio_processing/aec3/render_delay_controller.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {
namespace test {

class MockRenderDelayController : public RenderDelayController {
 public:
  MockRenderDelayController();
  virtual ~MockRenderDelayController();

  MOCK_METHOD(void, Reset, (bool reset_delay_statistics), (override));
  MOCK_METHOD(void, LogRenderCall, (), (override));
  MOCK_METHOD(absl::optional<DelayEstimate>,
              GetDelay,
              (const DownsampledRenderBuffer& render_buffer,
               size_t render_delay_buffer_delay,
               const std::vector<std::vector<float>>& capture),
              (override));
  MOCK_METHOD(bool, HasClockdrift, (), (const, override));
};

}  // namespace test
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_RENDER_DELAY_CONTROLLER_H_
