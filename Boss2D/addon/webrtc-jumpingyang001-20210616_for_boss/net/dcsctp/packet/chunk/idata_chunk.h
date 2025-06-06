/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_PACKET_CHUNK_IDATA_CHUNK_H_
#define NET_DCSCTP_PACKET_CHUNK_IDATA_CHUNK_H_
#include <stddef.h>
#include <stdint.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__chunk_h //original-code:"net/dcsctp/packet/chunk/chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__data_common_h //original-code:"net/dcsctp/packet/chunk/data_common.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__data_h //original-code:"net/dcsctp/packet/data.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"

namespace dcsctp {

// https://tools.ietf.org/html/rfc8260#section-2.1
struct IDataChunkConfig : ChunkConfig {
  static constexpr int kType = 64;
  static constexpr size_t kHeaderSize = 20;
  static constexpr size_t kVariableLengthAlignment = 1;
};

class IDataChunk : public AnyDataChunk, public TLVTrait<IDataChunkConfig> {
 public:
  static constexpr int kType = IDataChunkConfig::kType;

  // Exposed to allow the retransmission queue to make room for the correct
  // header size.
  static constexpr size_t kHeaderSize = IDataChunkConfig::kHeaderSize;
  IDataChunk(TSN tsn,
             StreamID stream_id,
             MID message_id,
             PPID ppid,
             FSN fsn,
             std::vector<uint8_t> payload,
             const Options& options)
      : AnyDataChunk(tsn,
                     stream_id,
                     SSN(0),
                     message_id,
                     fsn,
                     ppid,
                     std::move(payload),
                     options) {}

  explicit IDataChunk(TSN tsn, Data&& data, bool immediate_ack)
      : AnyDataChunk(tsn, std::move(data), immediate_ack) {}

  static absl::optional<IDataChunk> Parse(rtc::ArrayView<const uint8_t> data);

  void SerializeTo(std::vector<uint8_t>& out) const override;
  std::string ToString() const override;
};

}  // namespace dcsctp

#endif  // NET_DCSCTP_PACKET_CHUNK_IDATA_CHUNK_H_
