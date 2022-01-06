/*
 *  Copyright 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef P2P_BASE_UDPTRANSPORT_H_
#define P2P_BASE_UDPTRANSPORT_H_

#include <memory>
#include <string>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__ortc__udptransportinterface_h //original-code:"api/ortc/udptransportinterface.h"
#include BOSS_WEBRTC_U_p2p__base__packettransportinternal_h //original-code:"p2p/base/packettransportinternal.h"
#include BOSS_WEBRTC_U_rtc_base__asyncpacketsocket_h //original-code:"rtc_base/asyncpacketsocket.h"  // For PacketOptions.
#include BOSS_WEBRTC_U_rtc_base__thread_checker_h //original-code:"rtc_base/thread_checker.h"

namespace rtc {
class AsyncPacketSocket;
struct PacketTime;
struct SentPacket;
class SocketAddress;
}  // namespace rtc

namespace cricket {

// Implementation of UdpTransportInterface.
// Used by OrtcFactory.
class UdpTransport : public rtc::PacketTransportInternal,
                     public webrtc::UdpTransportInterface {
 public:
  // |transport_name| is only used for identification/logging.
  // |socket| must be non-null.
  UdpTransport(const std::string& transport_name,
               std::unique_ptr<rtc::AsyncPacketSocket> socket);
  ~UdpTransport() override;

  // Overrides of UdpTransportInterface, used by the API consumer.
  rtc::SocketAddress GetLocalAddress() const override;
  bool SetRemoteAddress(const rtc::SocketAddress& addr) override;
  rtc::SocketAddress GetRemoteAddress() const override;

  // Overrides of PacketTransportInternal, used by webrtc internally.
  const std::string& transport_name() const override;

  bool receiving() const override;

  bool writable() const override;

  int SendPacket(const char* data,
                 size_t len,
                 const rtc::PacketOptions& options,
                 int flags) override;

  int SetOption(rtc::Socket::Option opt, int value) override;

  int GetError() override;

  absl::optional<rtc::NetworkRoute> network_route() const override;

 protected:
  PacketTransportInternal* GetInternal() override;

 private:
  void OnSocketReadPacket(rtc::AsyncPacketSocket* socket,
                          const char* data,
                          size_t len,
                          const rtc::SocketAddress& remote_addr,
                          const rtc::PacketTime& packet_time);
  void OnSocketSentPacket(rtc::AsyncPacketSocket* socket,
                          const rtc::SentPacket& packet);
  bool IsLocalConsistent();

  std::string transport_name_;
  int send_error_ = 0;
  std::unique_ptr<rtc::AsyncPacketSocket> socket_;
  // If not set, will be an "nil" address ("IsNil" returns true).
  rtc::SocketAddress remote_address_;
  rtc::ThreadChecker network_thread_checker_;
};

}  // namespace cricket

#endif  // P2P_BASE_UDPTRANSPORT_H_
