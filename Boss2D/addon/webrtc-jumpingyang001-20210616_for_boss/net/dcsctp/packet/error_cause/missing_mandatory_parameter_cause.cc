/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__missing_mandatory_parameter_cause_h //original-code:"net/dcsctp/packet/error_cause/missing_mandatory_parameter_cause.h"

#include <stddef.h>

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__common__str_join_h //original-code:"net/dcsctp/common/str_join.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__bounded_byte_reader_h //original-code:"net/dcsctp/packet/bounded_byte_reader.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__bounded_byte_writer_h //original-code:"net/dcsctp/packet/bounded_byte_writer.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"

namespace dcsctp {

// https://tools.ietf.org/html/rfc4960#section-3.3.10.2

//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |     Cause Code=2              |      Cause Length=8+N*2       |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                   Number of missing params=N                  |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |   Missing Param Type #1       |   Missing Param Type #2       |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |   Missing Param Type #N-1     |   Missing Param Type #N       |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
constexpr int MissingMandatoryParameterCause::kType;

absl::optional<MissingMandatoryParameterCause>
MissingMandatoryParameterCause::Parse(rtc::ArrayView<const uint8_t> data) {
  absl::optional<BoundedByteReader<kHeaderSize>> reader = ParseTLV(data);
  if (!reader.has_value()) {
    return absl::nullopt;
  }

  uint32_t count = reader->Load32<4>();
  if (reader->variable_data_size() / kMissingParameterSize != count) {
    RTC_DLOG(LS_WARNING) << "Invalid number of missing parameters";
    return absl::nullopt;
  }

  std::vector<uint16_t> missing_parameter_types;
  missing_parameter_types.reserve(count);
  for (uint32_t i = 0; i < count; ++i) {
    BoundedByteReader<kMissingParameterSize> sub_reader =
        reader->sub_reader<kMissingParameterSize>(i * kMissingParameterSize);

    missing_parameter_types.push_back(sub_reader.Load16<0>());
  }
  return MissingMandatoryParameterCause(missing_parameter_types);
}

void MissingMandatoryParameterCause::SerializeTo(
    std::vector<uint8_t>& out) const {
  size_t variable_size =
      missing_parameter_types_.size() * kMissingParameterSize;
  BoundedByteWriter<kHeaderSize> writer = AllocateTLV(out, variable_size);

  writer.Store32<4>(missing_parameter_types_.size());

  for (size_t i = 0; i < missing_parameter_types_.size(); ++i) {
    BoundedByteWriter<kMissingParameterSize> sub_writer =
        writer.sub_writer<kMissingParameterSize>(i * kMissingParameterSize);

    sub_writer.Store16<0>(missing_parameter_types_[i]);
  }
}

std::string MissingMandatoryParameterCause::ToString() const {
  rtc::StringBuilder sb;
  sb << "Missing Mandatory Parameter, missing_parameter_types="
     << StrJoin(missing_parameter_types_, ",");
  return sb.Release();
}

}  // namespace dcsctp
