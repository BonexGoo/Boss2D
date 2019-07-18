/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// Unit tests for Merge class.

#include BOSS_WEBRTC_U_modules__audio_coding__neteq__merge_h //original-code:"modules/audio_coding/neteq/merge.h"

#include <vector>

#include BOSS_WEBRTC_U_modules__audio_coding__neteq__background_noise_h //original-code:"modules/audio_coding/neteq/background_noise.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__expand_h //original-code:"modules/audio_coding/neteq/expand.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__random_vector_h //original-code:"modules/audio_coding/neteq/random_vector.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__statistics_calculator_h //original-code:"modules/audio_coding/neteq/statistics_calculator.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__sync_buffer_h //original-code:"modules/audio_coding/neteq/sync_buffer.h"
#include "test/gtest.h"

namespace webrtc {

TEST(Merge, CreateAndDestroy) {
  int fs = 8000;
  size_t channels = 1;
  BackgroundNoise bgn(channels);
  SyncBuffer sync_buffer(1, 1000);
  RandomVector random_vector;
  StatisticsCalculator statistics;
  Expand expand(&bgn, &sync_buffer, &random_vector, &statistics, fs, channels);
  Merge merge(fs, channels, &expand, &sync_buffer);
}

// TODO(hlundin): Write more tests.

}  // namespace webrtc
