/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_TRANSPORT_STATS_H_
#define PC_TRANSPORT_STATS_H_

#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__dtls_transport_interface_h //original-code:"api/dtls_transport_interface.h"
#include BOSS_WEBRTC_U_p2p__base__dtls_transport_internal_h //original-code:"p2p/base/dtls_transport_internal.h"
#include BOSS_WEBRTC_U_p2p__base__ice_transport_internal_h //original-code:"p2p/base/ice_transport_internal.h"
#include BOSS_WEBRTC_U_p2p__base__port_h //original-code:"p2p/base/port.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_stream_adapter_h //original-code:"rtc_base/ssl_stream_adapter.h"

namespace cricket {

struct TransportChannelStats {
  TransportChannelStats();
  TransportChannelStats(const TransportChannelStats&);
  ~TransportChannelStats();

  int component = 0;
  int ssl_version_bytes = 0;
  int srtp_crypto_suite = rtc::SRTP_INVALID_CRYPTO_SUITE;
  int ssl_cipher_suite = rtc::TLS_NULL_WITH_NULL_NULL;
  webrtc::DtlsTransportState dtls_state = webrtc::DtlsTransportState::kNew;
  IceTransportStats ice_transport_stats;
};

// Information about all the channels of a transport.
// TODO(hta): Consider if a simple vector is as good as a map.
typedef std::vector<TransportChannelStats> TransportChannelStatsList;

// Information about the stats of a transport.
struct TransportStats {
  std::string transport_name;
  TransportChannelStatsList channel_stats;
};

}  // namespace cricket

#endif  // PC_TRANSPORT_STATS_H_
