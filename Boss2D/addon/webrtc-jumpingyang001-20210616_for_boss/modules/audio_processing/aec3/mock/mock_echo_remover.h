/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_ECHO_REMOVER_H_
#define MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_ECHO_REMOVER_H_

#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__echo_path_variability_h //original-code:"modules/audio_processing/aec3/echo_path_variability.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__echo_remover_h //original-code:"modules/audio_processing/aec3/echo_remover.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__render_buffer_h //original-code:"modules/audio_processing/aec3/render_buffer.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {
namespace test {

class MockEchoRemover : public EchoRemover {
 public:
  MockEchoRemover();
  virtual ~MockEchoRemover();

  MOCK_METHOD(void,
              ProcessCapture,
              (EchoPathVariability echo_path_variability,
               bool capture_signal_saturation,
               const absl::optional<DelayEstimate>& delay_estimate,
               RenderBuffer* render_buffer,
               std::vector<std::vector<std::vector<float>>>* linear_output,
               std::vector<std::vector<std::vector<float>>>* capture),
              (override));
  MOCK_METHOD(void,
              UpdateEchoLeakageStatus,
              (bool leakage_detected),
              (override));
  MOCK_METHOD(void,
              GetMetrics,
              (EchoControl::Metrics * metrics),
              (const, override));
  MOCK_METHOD(void,
              SetCaptureOutputUsage,
              (bool capture_output_used),
              (override));
};

}  // namespace test
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_ECHO_REMOVER_H_
