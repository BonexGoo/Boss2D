/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// Unit tests for ComfortNoise class.

#include BOSS_WEBRTC_U_modules__audio_coding__neteq__comfort_noise_h //original-code:"modules/audio_coding/neteq/comfort_noise.h"

#include "modules/audio_coding/neteq/mock/mock_decoder_database.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__sync_buffer_h //original-code:"modules/audio_coding/neteq/sync_buffer.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

TEST(ComfortNoise, CreateAndDestroy) {
  int fs = 8000;
  MockDecoderDatabase db;
  SyncBuffer sync_buffer(1, 1000);
  ComfortNoise cn(fs, &db, &sync_buffer);
  EXPECT_CALL(db, Die());  // Called when |db| goes out of scope.
}

// TODO(hlundin): Write more tests.

}  // namespace webrtc