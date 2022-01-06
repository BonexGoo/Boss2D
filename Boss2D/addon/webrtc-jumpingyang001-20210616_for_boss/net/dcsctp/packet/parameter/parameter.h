/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_PACKET_PARAMETER_PARAMETER_H_
#define NET_DCSCTP_PACKET_PARAMETER_PARAMETER_H_

#include <stddef.h>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__algorithm__container_h //original-code:"absl/algorithm/container.h"
#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"

namespace dcsctp {

class Parameter {
 public:
  Parameter() {}
  virtual ~Parameter() = default;

  Parameter(const Parameter& other) = default;
  Parameter& operator=(const Parameter& other) = default;

  virtual void SerializeTo(std::vector<uint8_t>& out) const = 0;
  virtual std::string ToString() const = 0;
};

struct ParameterDescriptor {
  ParameterDescriptor(uint16_t type, rtc::ArrayView<const uint8_t> data)
      : type(type), data(data) {}
  uint16_t type;
  rtc::ArrayView<const uint8_t> data;
};

class Parameters {
 public:
  class Builder {
   public:
    Builder() {}
    Builder& Add(const Parameter& p);
    Parameters Build() { return Parameters(std::move(data_)); }

   private:
    std::vector<uint8_t> data_;
  };

  static absl::optional<Parameters> Parse(rtc::ArrayView<const uint8_t> data);

  Parameters() {}
  Parameters(Parameters&& other) = default;
  Parameters& operator=(Parameters&& other) = default;

  rtc::ArrayView<const uint8_t> data() const { return data_; }
  std::vector<ParameterDescriptor> descriptors() const;

  template <typename P>
  absl::optional<P> get() const {
    static_assert(std::is_base_of<Parameter, P>::value,
                  "Template parameter not derived from Parameter");
    for (const auto& p : descriptors()) {
      if (p.type == P::kType) {
        return P::Parse(p.data);
      }
    }
    return absl::nullopt;
  }

 private:
  explicit Parameters(std::vector<uint8_t> data) : data_(std::move(data)) {}
  std::vector<uint8_t> data_;
};

struct ParameterConfig {
  static constexpr int kTypeSizeInBytes = 2;
};

}  // namespace dcsctp

#endif  // NET_DCSCTP_PACKET_PARAMETER_PARAMETER_H_
