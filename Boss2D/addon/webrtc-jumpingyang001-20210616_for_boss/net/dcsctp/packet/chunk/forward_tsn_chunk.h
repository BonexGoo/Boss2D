/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_PACKET_CHUNK_FORWARD_TSN_CHUNK_H_
#define NET_DCSCTP_PACKET_CHUNK_FORWARD_TSN_CHUNK_H_
#include <stddef.h>
#include <stdint.h>

#include <string>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__chunk_h //original-code:"net/dcsctp/packet/chunk/chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__forward_tsn_common_h //original-code:"net/dcsctp/packet/chunk/forward_tsn_common.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"

namespace dcsctp {

// https://tools.ietf.org/html/rfc3758#section-3.2
struct ForwardTsnChunkConfig : ChunkConfig {
  static constexpr int kType = 192;
  static constexpr size_t kHeaderSize = 8;
  static constexpr size_t kVariableLengthAlignment = 4;
};

class ForwardTsnChunk : public AnyForwardTsnChunk,
                        public TLVTrait<ForwardTsnChunkConfig> {
 public:
  static constexpr int kType = ForwardTsnChunkConfig::kType;

  ForwardTsnChunk(TSN new_cumulative_tsn,
                  std::vector<SkippedStream> skipped_streams)
      : AnyForwardTsnChunk(new_cumulative_tsn, std::move(skipped_streams)) {}

  static absl::optional<ForwardTsnChunk> Parse(
      rtc::ArrayView<const uint8_t> data);

  void SerializeTo(std::vector<uint8_t>& out) const override;
  std::string ToString() const override;

 private:
  static constexpr size_t kSkippedStreamBufferSize = 4;
};

}  // namespace dcsctp

#endif  // NET_DCSCTP_PACKET_CHUNK_FORWARD_TSN_CHUNK_H_
