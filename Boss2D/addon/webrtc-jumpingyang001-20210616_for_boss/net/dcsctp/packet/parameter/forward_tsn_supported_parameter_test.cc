/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__packet__parameter__forward_tsn_supported_parameter_h //original-code:"net/dcsctp/packet/parameter/forward_tsn_supported_parameter.h"

#include <stdint.h>

#include <type_traits>
#include <vector>

#include "net/dcsctp/testing/testing_macros.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"

namespace dcsctp {
namespace {

TEST(ForwardTsnSupportedParameterTest, SerializeAndDeserialize) {
  ForwardTsnSupportedParameter parameter;

  std::vector<uint8_t> serialized;
  parameter.SerializeTo(serialized);

  ASSERT_HAS_VALUE_AND_ASSIGN(ForwardTsnSupportedParameter deserialized,
                              ForwardTsnSupportedParameter::Parse(serialized));
}

}  // namespace
}  // namespace dcsctp
