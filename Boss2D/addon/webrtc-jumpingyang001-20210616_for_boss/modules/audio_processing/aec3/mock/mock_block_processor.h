/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_BLOCK_PROCESSOR_H_
#define MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_BLOCK_PROCESSOR_H_

#include <vector>

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__block_processor_h //original-code:"modules/audio_processing/aec3/block_processor.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {
namespace test {

class MockBlockProcessor : public BlockProcessor {
 public:
  MockBlockProcessor();
  virtual ~MockBlockProcessor();

  MOCK_METHOD(void,
              ProcessCapture,
              (bool level_change,
               bool saturated_microphone_signal,
               std::vector<std::vector<std::vector<float>>>* linear_output,
               std::vector<std::vector<std::vector<float>>>* capture_block),
              (override));
  MOCK_METHOD(void,
              BufferRender,
              (const std::vector<std::vector<std::vector<float>>>& block),
              (override));
  MOCK_METHOD(void,
              UpdateEchoLeakageStatus,
              (bool leakage_detected),
              (override));
  MOCK_METHOD(void,
              GetMetrics,
              (EchoControl::Metrics * metrics),
              (const, override));
  MOCK_METHOD(void, SetAudioBufferDelay, (int delay_ms), (override));
  MOCK_METHOD(void,
              SetCaptureOutputUsage,
              (bool capture_output_used),
              (override));
};

}  // namespace test
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_MOCK_MOCK_BLOCK_PROCESSOR_H_
