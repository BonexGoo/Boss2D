/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__packet__parameter__ssn_tsn_reset_request_parameter_h //original-code:"net/dcsctp/packet/parameter/ssn_tsn_reset_request_parameter.h"

#include <stdint.h>

#include <string>
#include <type_traits>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__bounded_byte_reader_h //original-code:"net/dcsctp/packet/bounded_byte_reader.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__bounded_byte_writer_h //original-code:"net/dcsctp/packet/bounded_byte_writer.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"

namespace dcsctp {

// https://tools.ietf.org/html/rfc6525#section-4.3

//   0                   1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |     Parameter Type = 15       |      Parameter Length = 8     |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |         Re-configuration Request Sequence Number              |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
constexpr int SSNTSNResetRequestParameter::kType;

absl::optional<SSNTSNResetRequestParameter> SSNTSNResetRequestParameter::Parse(
    rtc::ArrayView<const uint8_t> data) {
  absl::optional<BoundedByteReader<kHeaderSize>> reader = ParseTLV(data);
  if (!reader.has_value()) {
    return absl::nullopt;
  }
  ReconfigRequestSN request_sequence_number(reader->Load32<4>());

  return SSNTSNResetRequestParameter(request_sequence_number);
}

void SSNTSNResetRequestParameter::SerializeTo(std::vector<uint8_t>& out) const {
  BoundedByteWriter<kHeaderSize> writer = AllocateTLV(out);
  writer.Store32<4>(*request_sequence_number_);
}

std::string SSNTSNResetRequestParameter::ToString() const {
  rtc::StringBuilder sb;
  sb << "SSN/TSN Reset Request, req_seq_nbr=" << *request_sequence_number();
  return sb.Release();
}

}  // namespace dcsctp
