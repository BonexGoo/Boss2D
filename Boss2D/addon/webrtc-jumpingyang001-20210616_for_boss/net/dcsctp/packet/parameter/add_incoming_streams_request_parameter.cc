/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__packet__parameter__add_incoming_streams_request_parameter_h //original-code:"net/dcsctp/packet/parameter/add_incoming_streams_request_parameter.h"

#include <stdint.h>

#include <string>
#include <type_traits>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__common__internal_types_h //original-code:"net/dcsctp/common/internal_types.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__bounded_byte_reader_h //original-code:"net/dcsctp/packet/bounded_byte_reader.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__bounded_byte_writer_h //original-code:"net/dcsctp/packet/bounded_byte_writer.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"

namespace dcsctp {

// https://tools.ietf.org/html/rfc6525#section-4.6

//   0                   1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |     Parameter Type = 18       |      Parameter Length = 12    |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |          Re-configuration Request Sequence Number             |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |      Number of new streams    |         Reserved              |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
constexpr int AddIncomingStreamsRequestParameter::kType;

absl::optional<AddIncomingStreamsRequestParameter>
AddIncomingStreamsRequestParameter::Parse(rtc::ArrayView<const uint8_t> data) {
  absl::optional<BoundedByteReader<kHeaderSize>> reader = ParseTLV(data);
  if (!reader.has_value()) {
    return absl::nullopt;
  }
  ReconfigRequestSN request_sequence_number(reader->Load32<4>());
  uint16_t nbr_of_new_streams = reader->Load16<8>();

  return AddIncomingStreamsRequestParameter(request_sequence_number,
                                            nbr_of_new_streams);
}

void AddIncomingStreamsRequestParameter::SerializeTo(
    std::vector<uint8_t>& out) const {
  BoundedByteWriter<kHeaderSize> writer = AllocateTLV(out);
  writer.Store32<4>(*request_sequence_number_);
  writer.Store16<8>(nbr_of_new_streams_);
}

std::string AddIncomingStreamsRequestParameter::ToString() const {
  rtc::StringBuilder sb;
  sb << "Add Incoming Streams Request, req_seq_nbr="
     << *request_sequence_number();
  return sb.Release();
}

}  // namespace dcsctp
