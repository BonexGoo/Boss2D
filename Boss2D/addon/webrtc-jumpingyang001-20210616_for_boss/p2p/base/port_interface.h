/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef P2P_BASE_PORT_INTERFACE_H_
#define P2P_BASE_PORT_INTERFACE_H_

#include <string>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__candidate_h //original-code:"api/candidate.h"
#include BOSS_WEBRTC_U_p2p__base__transport_description_h //original-code:"p2p/base/transport_description.h"
#include BOSS_WEBRTC_U_rtc_base__async_packet_socket_h //original-code:"rtc_base/async_packet_socket.h"
#include BOSS_WEBRTC_U_rtc_base__callback_list_h //original-code:"rtc_base/callback_list.h"
#include BOSS_WEBRTC_U_rtc_base__socket_address_h //original-code:"rtc_base/socket_address.h"

namespace rtc {
class Network;
struct PacketOptions;
}  // namespace rtc

namespace cricket {
class Connection;
class IceMessage;
class StunMessage;
class StunStats;

enum ProtocolType {
  PROTO_UDP,
  PROTO_TCP,
  PROTO_SSLTCP,  // Pseudo-TLS.
  PROTO_TLS,
  PROTO_LAST = PROTO_TLS
};

// Defines the interface for a port, which represents a local communication
// mechanism that can be used to create connections to similar mechanisms of
// the other client. Various types of ports will implement this interface.
class PortInterface {
 public:
  virtual ~PortInterface();

  virtual const std::string& Type() const = 0;
  virtual rtc::Network* Network() const = 0;

  // Methods to set/get ICE role and tiebreaker values.
  virtual void SetIceRole(IceRole role) = 0;
  virtual IceRole GetIceRole() const = 0;

  virtual void SetIceTiebreaker(uint64_t tiebreaker) = 0;
  virtual uint64_t IceTiebreaker() const = 0;

  virtual bool SharedSocket() const = 0;

  virtual bool SupportsProtocol(const std::string& protocol) const = 0;

  // PrepareAddress will attempt to get an address for this port that other
  // clients can send to.  It may take some time before the address is ready.
  // Once it is ready, we will send SignalAddressReady.  If errors are
  // preventing the port from getting an address, it may send
  // SignalAddressError.
  virtual void PrepareAddress() = 0;

  // Returns the connection to the given address or NULL if none exists.
  virtual Connection* GetConnection(const rtc::SocketAddress& remote_addr) = 0;

  // Creates a new connection to the given address.
  enum CandidateOrigin { ORIGIN_THIS_PORT, ORIGIN_OTHER_PORT, ORIGIN_MESSAGE };
  virtual Connection* CreateConnection(const Candidate& remote_candidate,
                                       CandidateOrigin origin) = 0;

  // Functions on the underlying socket(s).
  virtual int SetOption(rtc::Socket::Option opt, int value) = 0;
  virtual int GetOption(rtc::Socket::Option opt, int* value) = 0;
  virtual int GetError() = 0;

  virtual ProtocolType GetProtocol() const = 0;

  virtual const std::vector<Candidate>& Candidates() const = 0;

  // Sends the given packet to the given address, provided that the address is
  // that of a connection or an address that has sent to us already.
  virtual int SendTo(const void* data,
                     size_t size,
                     const rtc::SocketAddress& addr,
                     const rtc::PacketOptions& options,
                     bool payload) = 0;

  // Indicates that we received a successful STUN binding request from an
  // address that doesn't correspond to any current connection.  To turn this
  // into a real connection, call CreateConnection.
  sigslot::signal6<PortInterface*,
                   const rtc::SocketAddress&,
                   ProtocolType,
                   IceMessage*,
                   const std::string&,
                   bool>
      SignalUnknownAddress;

  // Sends a response message (normal or error) to the given request.  One of
  // these methods should be called as a response to SignalUnknownAddress.
  virtual void SendBindingErrorResponse(StunMessage* request,
                                        const rtc::SocketAddress& addr,
                                        int error_code,
                                        const std::string& reason) = 0;

  // Signaled when this port decides to delete itself because it no longer has
  // any usefulness.
  virtual void SubscribePortDestroyed(
      std::function<void(PortInterface*)> callback) = 0;

  // Signaled when Port discovers ice role conflict with the peer.
  sigslot::signal1<PortInterface*> SignalRoleConflict;

  // Normally, packets arrive through a connection (or they result signaling of
  // unknown address).  Calling this method turns off delivery of packets
  // through their respective connection and instead delivers every packet
  // through this port.
  virtual void EnablePortPackets() = 0;
  sigslot::
      signal4<PortInterface*, const char*, size_t, const rtc::SocketAddress&>
          SignalReadPacket;

  // Emitted each time a packet is sent on this port.
  sigslot::signal1<const rtc::SentPacket&> SignalSentPacket;

  virtual std::string ToString() const = 0;

  virtual void GetStunStats(absl::optional<StunStats>* stats) = 0;

 protected:
  PortInterface();
};

}  // namespace cricket

#endif  // P2P_BASE_PORT_INTERFACE_H_
