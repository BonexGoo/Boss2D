/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_SOCKET_CONTEXT_H_
#define NET_DCSCTP_SOCKET_CONTEXT_H_

#include <cstdint>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__common__internal_types_h //original-code:"net/dcsctp/common/internal_types.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__sctp_packet_h //original-code:"net/dcsctp/packet/sctp_packet.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_socket_h //original-code:"net/dcsctp/public/dcsctp_socket.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__types_h //original-code:"net/dcsctp/public/types.h"

namespace dcsctp {

// A set of helper methods used by handlers to e.g. send packets.
//
// Implemented by the TransmissionControlBlock.
class Context {
 public:
  virtual ~Context() = default;

  // Indicates if a connection has been established.
  virtual bool is_connection_established() const = 0;

  // Returns this side's initial TSN value.
  virtual TSN my_initial_tsn() const = 0;

  // Returns the peer's initial TSN value.
  virtual TSN peer_initial_tsn() const = 0;

  // Returns the socket callbacks.
  virtual DcSctpSocketCallbacks& callbacks() const = 0;

  // Observes a measured RTT value, in milliseconds.
  virtual void ObserveRTT(DurationMs rtt_ms) = 0;

  // Returns the current Retransmission Timeout (rto) value, in milliseconds.
  virtual DurationMs current_rto() const = 0;

  // Increments the transmission error counter, given a human readable reason.
  virtual bool IncrementTxErrorCounter(absl::string_view reason) = 0;

  // Clears the transmission error counter.
  virtual void ClearTxErrorCounter() = 0;

  // Returns true if there have been too many retransmission errors.
  virtual bool HasTooManyTxErrors() const = 0;

  // Returns a PacketBuilder, filled in with the correct verification tag.
  virtual SctpPacket::Builder PacketBuilder() const = 0;

  // Builds the packet from `builder` and sends it.
  virtual void Send(SctpPacket::Builder& builder) = 0;
};

}  // namespace dcsctp

#endif  // NET_DCSCTP_SOCKET_CONTEXT_H_
