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

#include <stdint.h>

#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__bounded_byte_reader_h //original-code:"net/dcsctp/packet/bounded_byte_reader.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__bounded_byte_writer_h //original-code:"net/dcsctp/packet/bounded_byte_writer.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__error_cause_h //original-code:"net/dcsctp/packet/error_cause/error_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"

namespace dcsctp {

// https://tools.ietf.org/html/rfc4960#section-3.3.10

//   0                   1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |   Type = 9    | Chunk  Flags  |           Length              |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  \                                                               \
//  /                    one or more Error Causes                   /
//  \                                                               \
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
constexpr int ErrorChunk::kType;

absl::optional<ErrorChunk> ErrorChunk::Parse(
    rtc::ArrayView<const uint8_t> data) {
  absl::optional<BoundedByteReader<kHeaderSize>> reader = ParseTLV(data);
  if (!reader.has_value()) {
    return absl::nullopt;
  }
  absl::optional<Parameters> error_causes =
      Parameters::Parse(reader->variable_data());
  if (!error_causes.has_value()) {
    return absl::nullopt;
  }
  return ErrorChunk(*std::move(error_causes));
}

void ErrorChunk::SerializeTo(std::vector<uint8_t>& out) const {
  rtc::ArrayView<const uint8_t> error_causes = error_causes_.data();
  BoundedByteWriter<kHeaderSize> writer = AllocateTLV(out, error_causes.size());
  writer.CopyToVariableData(error_causes);
}

std::string ErrorChunk::ToString() const {
  return "ERROR";
}
}  // namespace dcsctp
