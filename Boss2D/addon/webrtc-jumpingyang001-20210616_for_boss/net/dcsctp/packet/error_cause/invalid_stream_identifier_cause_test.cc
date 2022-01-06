/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__invalid_stream_identifier_cause_h //original-code:"net/dcsctp/packet/error_cause/invalid_stream_identifier_cause.h"

#include <stdint.h>

#include <type_traits>
#include <vector>

#include "net/dcsctp/testing/testing_macros.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"

namespace dcsctp {
namespace {

TEST(InvalidStreamIdentifierCauseTest, SerializeAndDeserialize) {
  InvalidStreamIdentifierCause parameter(StreamID(1));

  std::vector<uint8_t> serialized;
  parameter.SerializeTo(serialized);

  ASSERT_HAS_VALUE_AND_ASSIGN(InvalidStreamIdentifierCause deserialized,
                              InvalidStreamIdentifierCause::Parse(serialized));

  EXPECT_EQ(*deserialized.stream_id(), 1);
}

}  // namespace
}  // namespace dcsctp
