/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__chunk_h //original-code:"net/dcsctp/packet/chunk/chunk.h"

#include <cstdint>
#include <memory>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__common__math_h //original-code:"net/dcsctp/common/math.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__abort_chunk_h //original-code:"net/dcsctp/packet/chunk/abort_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__cookie_ack_chunk_h //original-code:"net/dcsctp/packet/chunk/cookie_ack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__cookie_echo_chunk_h //original-code:"net/dcsctp/packet/chunk/cookie_echo_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__data_chunk_h //original-code:"net/dcsctp/packet/chunk/data_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__error_chunk_h //original-code:"net/dcsctp/packet/chunk/error_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__forward_tsn_chunk_h //original-code:"net/dcsctp/packet/chunk/forward_tsn_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__heartbeat_ack_chunk_h //original-code:"net/dcsctp/packet/chunk/heartbeat_ack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__heartbeat_request_chunk_h //original-code:"net/dcsctp/packet/chunk/heartbeat_request_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__idata_chunk_h //original-code:"net/dcsctp/packet/chunk/idata_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__iforward_tsn_chunk_h //original-code:"net/dcsctp/packet/chunk/iforward_tsn_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__init_ack_chunk_h //original-code:"net/dcsctp/packet/chunk/init_ack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__init_chunk_h //original-code:"net/dcsctp/packet/chunk/init_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__reconfig_chunk_h //original-code:"net/dcsctp/packet/chunk/reconfig_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__sack_chunk_h //original-code:"net/dcsctp/packet/chunk/sack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__shutdown_ack_chunk_h //original-code:"net/dcsctp/packet/chunk/shutdown_ack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__shutdown_chunk_h //original-code:"net/dcsctp/packet/chunk/shutdown_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__shutdown_complete_chunk_h //original-code:"net/dcsctp/packet/chunk/shutdown_complete_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"

namespace dcsctp {

template <class Chunk>
bool ParseAndPrint(uint8_t chunk_type,
                   rtc::ArrayView<const uint8_t> data,
                   rtc::StringBuilder& sb) {
  if (chunk_type == Chunk::kType) {
    absl::optional<Chunk> c = Chunk::Parse(data);
    if (c.has_value()) {
      sb << c->ToString();
    } else {
      sb << "Failed to parse chunk of type " << chunk_type;
    }
    return true;
  }
  return false;
}

std::string DebugConvertChunkToString(rtc::ArrayView<const uint8_t> data) {
  rtc::StringBuilder sb;

  if (data.empty()) {
    sb << "Failed to parse chunk due to empty data";
  } else {
    uint8_t chunk_type = data[0];
    if (!ParseAndPrint<DataChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<InitChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<InitAckChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<SackChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<HeartbeatRequestChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<HeartbeatAckChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<AbortChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<ErrorChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<CookieEchoChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<CookieAckChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<ShutdownChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<ShutdownAckChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<ShutdownCompleteChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<ReConfigChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<ForwardTsnChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<IDataChunk>(chunk_type, data, sb) &&
        !ParseAndPrint<IForwardTsnChunk>(chunk_type, data, sb)) {
      sb << "Unhandled chunk type: " << static_cast<int>(chunk_type);
    }
  }
  return sb.Release();
}
}  // namespace dcsctp
