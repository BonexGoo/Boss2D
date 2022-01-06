/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__user_initiated_abort_cause_h //original-code:"net/dcsctp/packet/error_cause/user_initiated_abort_cause.h"

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

// https://tools.ietf.org/html/rfc4960#section-3.3.10.12

//   0                   1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |         Cause Code=12         |      Cause Length=Variable    |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  /                    Upper Layer Abort Reason                   /
//  \                                                               \
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
constexpr int UserInitiatedAbortCause::kType;

absl::optional<UserInitiatedAbortCause> UserInitiatedAbortCause::Parse(
    rtc::ArrayView<const uint8_t> data) {
  absl::optional<BoundedByteReader<kHeaderSize>> reader = ParseTLV(data);
  if (!reader.has_value()) {
    return absl::nullopt;
  }
  if (reader->variable_data().empty()) {
    return UserInitiatedAbortCause("");
  }
  return UserInitiatedAbortCause(
      std::string(reinterpret_cast<const char*>(reader->variable_data().data()),
                  reader->variable_data().size()));
}

void UserInitiatedAbortCause::SerializeTo(std::vector<uint8_t>& out) const {
  BoundedByteWriter<kHeaderSize> writer =
      AllocateTLV(out, upper_layer_abort_reason_.size());
  writer.CopyToVariableData(rtc::MakeArrayView(
      reinterpret_cast<const uint8_t*>(upper_layer_abort_reason_.data()),
      upper_layer_abort_reason_.size()));
}

std::string UserInitiatedAbortCause::ToString() const {
  rtc::StringBuilder sb;
  sb << "User-Initiated Abort, reason=" << upper_layer_abort_reason_;
  return sb.Release();
}

}  // namespace dcsctp
