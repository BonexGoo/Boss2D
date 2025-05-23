/*
 *  Copyright 2016 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <algorithm>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_p2p__base__basicpacketsocketfactory_h //original-code:"p2p/base/basicpacketsocketfactory.h"
#include BOSS_WEBRTC_U_p2p__base__packettransportinternal_h //original-code:"p2p/base/packettransportinternal.h"
#include BOSS_WEBRTC_U_p2p__base__udptransport_h //original-code:"p2p/base/udptransport.h"
#include BOSS_WEBRTC_U_rtc_base__asyncpacketsocket_h //original-code:"rtc_base/asyncpacketsocket.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__ipaddress_h //original-code:"rtc_base/ipaddress.h"
#include BOSS_WEBRTC_U_rtc_base__socketaddress_h //original-code:"rtc_base/socketaddress.h"
#include BOSS_WEBRTC_U_rtc_base__socketserver_h //original-code:"rtc_base/socketserver.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__virtualsocketserver_h //original-code:"rtc_base/virtualsocketserver.h"

namespace cricket {

constexpr int kTimeoutMs = 10000;
static const rtc::IPAddress kIPv4LocalHostAddress =
    rtc::IPAddress(0x7F000001);  // 127.0.0.1

class UdpTransportTest : public testing::Test, public sigslot::has_slots<> {
 public:
  UdpTransportTest()
      : virtual_socket_server_(new rtc::VirtualSocketServer()),
        network_thread_(virtual_socket_server_.get()),
        ep1_("Name1",
             std::unique_ptr<rtc::AsyncPacketSocket>(
                 socket_factory_.CreateUdpSocket(
                     rtc::SocketAddress(rtc::GetAnyIP(AF_INET), 0),
                     0,
                     0))),
        ep2_("Name2",
             std::unique_ptr<rtc::AsyncPacketSocket>(
                 socket_factory_.CreateUdpSocket(
                     rtc::SocketAddress(rtc::GetAnyIP(AF_INET), 0),
                     0,
                     0))) {
    // Setup IP Address for outgoing packets from sockets bound to IPV4
    // INADDR_ANY ("0.0.0.0."), as used above when creating the virtual
    // sockets. The virtual socket server sends these packets only if the
    // default address is explicit set. With a physical socket, the actual
    // network stack / operating system would set the IP address for outgoing
    // packets.
    virtual_socket_server_->SetDefaultRoute(kIPv4LocalHostAddress);
  }

  struct Endpoint : public sigslot::has_slots<> {
    explicit Endpoint(std::string tch_name,
                      std::unique_ptr<rtc::AsyncPacketSocket> socket) {
      ch_.reset(new UdpTransport(std::move(tch_name), std::move(socket)));
      ch_->SignalReadPacket.connect(this, &Endpoint::OnReadPacket);
      ch_->SignalSentPacket.connect(this, &Endpoint::OnSentPacket);
      ch_->SignalReadyToSend.connect(this, &Endpoint::OnReadyToSend);
      ch_->SignalWritableState.connect(this, &Endpoint::OnWritableState);
    }

    bool CheckData(const char* data, int len) {
      bool ret = false;
      if (!ch_packets_.empty()) {
        std::string packet = ch_packets_.front();
        ret = (packet == std::string(data, len));
        ch_packets_.pop_front();
      }
      return ret;
    }

    void OnWritableState(rtc::PacketTransportInternal* transport) {
      num_sig_writable_++;
    }

    void OnReadyToSend(rtc::PacketTransportInternal* transport) {
      num_sig_ready_to_send_++;
    }

    void OnReadPacket(rtc::PacketTransportInternal* transport,
                      const char* data,
                      size_t len,
                      const rtc::PacketTime& packet_time,
                      int flags) {
      num_received_packets_++;
      RTC_LOG(LS_VERBOSE) << "OnReadPacket (unittest)";
      ch_packets_.push_front(std::string(data, len));
    }

    void OnSentPacket(rtc::PacketTransportInternal* transport,
                      const rtc::SentPacket&) {
      num_sig_sent_packets_++;
    }

    int SendData(const char* data, size_t len) {
      rtc::PacketOptions options;
      return ch_->SendPacket(data, len, options, 0);
    }

    void GetLocalPort(uint16_t* local_port) {
      *local_port = ch_->GetLocalAddress().port();
    }

    std::list<std::string> ch_packets_;
    std::unique_ptr<UdpTransport> ch_;
    uint32_t num_received_packets_ = 0;   // Increases on SignalReadPacket.
    uint32_t num_sig_sent_packets_ = 0;   // Increases on SignalSentPacket.
    uint32_t num_sig_writable_ = 0;       // Increases on SignalWritable.
    uint32_t num_sig_ready_to_send_ = 0;  // Increases on SignalReadyToSend.
  };

  std::unique_ptr<rtc::VirtualSocketServer> virtual_socket_server_;
  rtc::AutoSocketServerThread network_thread_;
  // Uses current thread's socket server, which will be set by ss_scope_.
  rtc::BasicPacketSocketFactory socket_factory_;

  Endpoint ep1_;
  Endpoint ep2_;

  void TestSendRecv() {
    for (uint32_t i = 0; i < 5; ++i) {
      static const char* data = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
      int len = static_cast<int>(strlen(data));
      // local_channel <==> remote_channel
      EXPECT_EQ_WAIT(len, ep1_.SendData(data, len), kTimeoutMs);
      EXPECT_TRUE_WAIT(ep2_.CheckData(data, len), kTimeoutMs);
      EXPECT_EQ_WAIT(i + 1u, ep2_.num_received_packets_, kTimeoutMs);
      EXPECT_EQ_WAIT(len, ep2_.SendData(data, len), kTimeoutMs);
      EXPECT_TRUE_WAIT(ep1_.CheckData(data, len), kTimeoutMs);
      EXPECT_EQ_WAIT(i + 1u, ep1_.num_received_packets_, kTimeoutMs);
    }
  }
};

TEST_F(UdpTransportTest, AddressGetters) {
  // Initially, remote address should be nil but local address shouldn't be.
  EXPECT_FALSE(ep1_.ch_->GetLocalAddress().IsNil());
  EXPECT_TRUE(ep1_.ch_->GetRemoteAddress().IsNil());
  rtc::SocketAddress destination("127.0.0.1", 1337);
  ASSERT_TRUE(ep1_.ch_->SetRemoteAddress(destination));
  EXPECT_EQ(destination, ep1_.ch_->GetRemoteAddress());
}

// Setting an invalid address should fail and have no effect.
TEST_F(UdpTransportTest, SettingIncompleteRemoteAddressFails) {
  EXPECT_FALSE(ep1_.ch_->SetRemoteAddress(rtc::SocketAddress("127.0.0.1", 0)));
  EXPECT_TRUE(ep1_.ch_->GetRemoteAddress().IsNil());
}

TEST_F(UdpTransportTest, SendRecvBasic) {
  uint16_t port;
  ep2_.GetLocalPort(&port);
  rtc::SocketAddress addr2 = rtc::SocketAddress("127.0.0.1", port);
  EXPECT_TRUE(ep1_.ch_->SetRemoteAddress(addr2));
  ep1_.GetLocalPort(&port);
  rtc::SocketAddress addr1 = rtc::SocketAddress("127.0.0.1", port);
  EXPECT_TRUE(ep2_.ch_->SetRemoteAddress(addr1));
  TestSendRecv();
}

// Test the signals and state methods used internally by causing a UdpTransport
// to send a packet to itself.
TEST_F(UdpTransportTest, StatusAndSignals) {
  EXPECT_EQ(0u, ep1_.num_sig_writable_);
  EXPECT_EQ(0u, ep1_.num_sig_ready_to_send_);
  // Loopback
  EXPECT_TRUE(!ep1_.ch_->writable());
  rtc::SocketAddress addr = ep1_.ch_->GetLocalAddress();
  // Keep port, but explicitly set IP.
  addr.SetIP("127.0.0.1");
  ep1_.ch_->SetRemoteAddress(addr);
  EXPECT_TRUE(ep1_.ch_->writable());
  EXPECT_EQ(1u, ep1_.num_sig_writable_);
  EXPECT_EQ(1u, ep1_.num_sig_ready_to_send_);
  const char data[] = "abc";
  ep1_.SendData(data, sizeof(data));
  EXPECT_EQ_WAIT(1u, ep1_.ch_packets_.size(), kTimeoutMs);
  EXPECT_EQ_WAIT(1u, ep1_.num_sig_sent_packets_, kTimeoutMs);
}

}  // namespace cricket
