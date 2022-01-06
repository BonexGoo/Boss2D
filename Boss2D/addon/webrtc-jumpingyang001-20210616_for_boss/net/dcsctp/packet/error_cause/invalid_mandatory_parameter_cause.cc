/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__invalid_mandatory_parameter_cause_h //original-code:"net/dcsctp/packet/error_cause/invalid_mandatory_parameter_cause.h"

#include <stdint.h>

#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"

namespace dcsctp {

// https://tools.ietf.org/html/rfc4960#section-3.3.10.7

//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |     Cause Code=7              |      Cause Length=4           |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
constexpr int InvalidMandatoryParameterCause::kType;

absl::optional<InvalidMandatoryParameterCause>
InvalidMandatoryParameterCause::Parse(rtc::ArrayView<const uint8_t> data) {
  if (!ParseTLV(data).has_value()) {
    return absl::nullopt;
  }
  return InvalidMandatoryParameterCause();
}

void InvalidMandatoryParameterCause::SerializeTo(
    std::vector<uint8_t>& out) const {
  AllocateTLV(out);
}

std::string InvalidMandatoryParameterCause::ToString() const {
  return "Invalid Mandatory Parameter";
}

}  // namespace dcsctp
