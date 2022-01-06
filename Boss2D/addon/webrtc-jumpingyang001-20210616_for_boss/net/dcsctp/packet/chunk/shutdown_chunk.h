/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_PACKET_CHUNK_SHUTDOWN_CHUNK_H_
#define NET_DCSCTP_PACKET_CHUNK_SHUTDOWN_CHUNK_H_
#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__chunk_h //original-code:"net/dcsctp/packet/chunk/chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"

namespace dcsctp {

// https://tools.ietf.org/html/rfc4960#section-3.3.8
struct ShutdownChunkConfig : ChunkConfig {
  static constexpr int kType = 7;
  static constexpr size_t kHeaderSize = 8;
  static constexpr size_t kVariableLengthAlignment = 0;
};

class ShutdownChunk : public Chunk, public TLVTrait<ShutdownChunkConfig> {
 public:
  static constexpr int kType = ShutdownChunkConfig::kType;

  explicit ShutdownChunk(TSN cumulative_tsn_ack)
      : cumulative_tsn_ack_(cumulative_tsn_ack) {}

  static absl::optional<ShutdownChunk> Parse(
      rtc::ArrayView<const uint8_t> data);

  void SerializeTo(std::vector<uint8_t>& out) const override;
  std::string ToString() const override;

  TSN cumulative_tsn_ack() const { return cumulative_tsn_ack_; }

 private:
  TSN cumulative_tsn_ack_;
};

}  // namespace dcsctp

#endif  // NET_DCSCTP_PACKET_CHUNK_SHUTDOWN_CHUNK_H_
