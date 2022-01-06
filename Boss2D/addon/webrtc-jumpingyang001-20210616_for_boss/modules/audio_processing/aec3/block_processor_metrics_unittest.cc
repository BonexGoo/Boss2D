/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__block_processor_metrics_h //original-code:"modules/audio_processing/aec3/block_processor_metrics.h"

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_common_h //original-code:"modules/audio_processing/aec3/aec3_common.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

// Verify the general functionality of BlockProcessorMetrics.
TEST(BlockProcessorMetrics, NormalUsage) {
  BlockProcessorMetrics metrics;

  for (int j = 0; j < 3; ++j) {
    for (int k = 0; k < kMetricsReportingIntervalBlocks - 1; ++k) {
      metrics.UpdateRender(false);
      metrics.UpdateRender(false);
      metrics.UpdateCapture(false);
      EXPECT_FALSE(metrics.MetricsReported());
    }
    metrics.UpdateCapture(false);
    EXPECT_TRUE(metrics.MetricsReported());
  }
}

}  // namespace webrtc
