/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__error_chunk_h //original-code:"net/dcsctp/packet/chunk/error_chunk.h"

#include <cstdint>
#include <type_traits>
#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__error_cause_h //original-code:"net/dcsctp/packet/error_cause/error_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__unrecognized_chunk_type_cause_h //original-code:"net/dcsctp/packet/error_cause/unrecognized_chunk_type_cause.h"
#include "net/dcsctp/testing/testing_macros.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace dcsctp {
namespace {
using ::testing::ElementsAre;

TEST(ErrorChunkTest, FromCapture) {
  /*
   ERROR chunk
      Chunk type: ERROR (9)
      Chunk flags: 0x00
      Chunk length: 12
      Unrecognized chunk type cause (Type: 73 (unknown))
  */

  uint8_t data[] = {0x09, 0x00, 0x00, 0x0c, 0x00, 0x06,
                    0x00, 0x08, 0x49, 0x00, 0x00, 0x04};

  ASSERT_HAS_VALUE_AND_ASSIGN(ErrorChunk chunk, ErrorChunk::Parse(data));

  ASSERT_HAS_VALUE_AND_ASSIGN(
      UnrecognizedChunkTypeCause cause,
      chunk.error_causes().get<UnrecognizedChunkTypeCause>());

  EXPECT_THAT(cause.unrecognized_chunk(), ElementsAre(0x49, 0x00, 0x00, 0x04));
}

TEST(ErrorChunkTest, SerializeAndDeserialize) {
  ErrorChunk chunk(Parameters::Builder()
                       .Add(UnrecognizedChunkTypeCause({1, 2, 3, 4}))
                       .Build());

  std::vector<uint8_t> serialized;
  chunk.SerializeTo(serialized);

  ASSERT_HAS_VALUE_AND_ASSIGN(ErrorChunk deserialized,
                              ErrorChunk::Parse(serialized));
  ASSERT_HAS_VALUE_AND_ASSIGN(
      UnrecognizedChunkTypeCause cause,
      deserialized.error_causes().get<UnrecognizedChunkTypeCause>());

  EXPECT_THAT(cause.unrecognized_chunk(), ElementsAre(1, 2, 3, 4));
}

}  // namespace
}  // namespace dcsctp
