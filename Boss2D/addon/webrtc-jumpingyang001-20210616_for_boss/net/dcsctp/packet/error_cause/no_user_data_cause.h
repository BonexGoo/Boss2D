/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_PACKET_ERROR_CAUSE_NO_USER_DATA_CAUSE_H_
#define NET_DCSCTP_PACKET_ERROR_CAUSE_NO_USER_DATA_CAUSE_H_
#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__common__internal_types_h //original-code:"net/dcsctp/common/internal_types.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__error_cause_h //original-code:"net/dcsctp/packet/error_cause/error_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"
namespace dcsctp {

// https://tools.ietf.org/html/rfc4960#section-3.3.10.9
struct NoUserDataCauseConfig : public ParameterConfig {
  static constexpr int kType = 9;
  static constexpr size_t kHeaderSize = 8;
  static constexpr size_t kVariableLengthAlignment = 0;
};

class NoUserDataCause : public Parameter,
                        public TLVTrait<NoUserDataCauseConfig> {
 public:
  static constexpr int kType = NoUserDataCauseConfig::kType;

  explicit NoUserDataCause(TSN tsn) : tsn_(tsn) {}

  static absl::optional<NoUserDataCause> Parse(
      rtc::ArrayView<const uint8_t> data);

  void SerializeTo(std::vector<uint8_t>& out) const override;
  std::string ToString() const override;

  TSN tsn() const { return tsn_; }

 private:
  TSN tsn_;
};

}  // namespace dcsctp

#endif  // NET_DCSCTP_PACKET_ERROR_CAUSE_NO_USER_DATA_CAUSE_H_
