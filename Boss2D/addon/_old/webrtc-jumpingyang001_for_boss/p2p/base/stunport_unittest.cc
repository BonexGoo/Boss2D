/*
 *  Copyright 2009 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>

#include BOSS_WEBRTC_U_p2p__base__basicpacketsocketfactory_h //original-code:"p2p/base/basicpacketsocketfactory.h"
#include BOSS_WEBRTC_U_p2p__base__stunport_h //original-code:"p2p/base/stunport.h"
#include BOSS_WEBRTC_U_p2p__base__teststunserver_h //original-code:"p2p/base/teststunserver.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__helpers_h //original-code:"rtc_base/helpers.h"
#include BOSS_WEBRTC_U_rtc_base__socketaddress_h //original-code:"rtc_base/socketaddress.h"
#include BOSS_WEBRTC_U_rtc_base__ssladapter_h //original-code:"rtc_base/ssladapter.h"
#include BOSS_WEBRTC_U_rtc_base__virtualsocketserver_h //original-code:"rtc_base/virtualsocketserver.h"

using cricket::ServerAddresses;
using rtc::SocketAddress;

static const SocketAddress kLocalAddr("127.0.0.1", 0);
static const SocketAddress kStunAddr1("127.0.0.1", 5000);
static const SocketAddress kStunAddr2("127.0.0.1", 4000);
static const SocketAddress kStunAddr3("127.0.0.1", 3000);
static const SocketAddress kBadAddr("0.0.0.1", 5000);
static const SocketAddress kStunHostnameAddr("localhost", 5000);
static const SocketAddress kBadHostnameAddr("not-a-real-hostname", 5000);
// STUN timeout (with all retries) is cricket::STUN_TOTAL_TIMEOUT.
// Add some margin of error for slow bots.
static const int kTimeoutMs = cricket::STUN_TOTAL_TIMEOUT;
// stun prio = 100 << 24 | 30 (IPV4) << 8 | 256 - 0
static const uint32_t kStunCandidatePriority = 1677729535;
static const int kInfiniteLifetime = -1;
static const int kHighCostPortKeepaliveLifetimeMs = 2 * 60 * 1000;

// Tests connecting a StunPort to a fake STUN server (cricket::StunServer)
class StunPortTestBase : public testing::Test, public sigslot::has_slots<> {
 public:
  StunPortTestBase()
      : ss_(new rtc::VirtualSocketServer()),
        thread_(ss_.get()),
        network_("unittest", "unittest", kLocalAddr.ipaddr(), 32),
        socket_factory_(rtc::Thread::Current()),
        stun_server_1_(cricket::TestStunServer::Create(rtc::Thread::Current(),
                                                       kStunAddr1)),
        stun_server_2_(cricket::TestStunServer::Create(rtc::Thread::Current(),
                                                       kStunAddr2)),
        done_(false),
        error_(false),
        stun_keepalive_delay_(1),
        stun_keepalive_lifetime_(-1) {
    network_.AddIP(kLocalAddr.ipaddr());
  }

  cricket::UDPPort* port() const { return stun_port_.get(); }
  bool done() const { return done_; }
  bool error() const { return error_; }

  void SetNetworkType(rtc::AdapterType adapter_type) {
    network_.set_type(adapter_type);
  }

  void CreateStunPort(const rtc::SocketAddress& server_addr) {
    ServerAddresses stun_servers;
    stun_servers.insert(server_addr);
    CreateStunPort(stun_servers);
  }

  void CreateStunPort(const ServerAddresses& stun_servers) {
    stun_port_.reset(cricket::StunPort::Create(
        rtc::Thread::Current(), &socket_factory_, &network_, 0, 0,
        rtc::CreateRandomString(16), rtc::CreateRandomString(22), stun_servers,
        std::string(), absl::nullopt));
    stun_port_->set_stun_keepalive_delay(stun_keepalive_delay_);
    // If |stun_keepalive_lifetime_| is negative, let the stun port
    // choose its lifetime from the network type.
    if (stun_keepalive_lifetime_ >= 0) {
      stun_port_->set_stun_keepalive_lifetime(stun_keepalive_lifetime_);
    }
    stun_port_->SignalPortComplete.connect(this,
                                           &StunPortTestBase::OnPortComplete);
    stun_port_->SignalPortError.connect(this, &StunPortTestBase::OnPortError);
  }

  void CreateSharedUdpPort(const rtc::SocketAddress& server_addr) {
    socket_.reset(socket_factory_.CreateUdpSocket(
        rtc::SocketAddress(kLocalAddr.ipaddr(), 0), 0, 0));
    ASSERT_TRUE(socket_ != NULL);
    socket_->SignalReadPacket.connect(this, &StunPortTestBase::OnReadPacket);
    stun_port_.reset(cricket::UDPPort::Create(
        rtc::Thread::Current(), &socket_factory_, &network_, socket_.get(),
        rtc::CreateRandomString(16), rtc::CreateRandomString(22), std::string(),
        false, absl::nullopt));
    ASSERT_TRUE(stun_port_ != NULL);
    ServerAddresses stun_servers;
    stun_servers.insert(server_addr);
    stun_port_->set_server_addresses(stun_servers);
    stun_port_->SignalPortComplete.connect(this,
                                           &StunPortTestBase::OnPortComplete);
    stun_port_->SignalPortError.connect(this, &StunPortTestBase::OnPortError);
  }

  void PrepareAddress() { stun_port_->PrepareAddress(); }

  void OnReadPacket(rtc::AsyncPacketSocket* socket,
                    const char* data,
                    size_t size,
                    const rtc::SocketAddress& remote_addr,
                    const rtc::PacketTime& packet_time) {
    stun_port_->HandleIncomingPacket(socket, data, size, remote_addr,
                                     rtc::PacketTime());
  }

  void SendData(const char* data, size_t len) {
    stun_port_->HandleIncomingPacket(socket_.get(), data, len,
                                     rtc::SocketAddress("22.22.22.22", 0),
                                     rtc::PacketTime());
  }

 protected:
  static void SetUpTestCase() {
    // Ensure the RNG is inited.
    rtc::InitRandom(NULL, 0);
  }

  void OnPortComplete(cricket::Port* port) {
    ASSERT_FALSE(done_);
    done_ = true;
    error_ = false;
  }
  void OnPortError(cricket::Port* port) {
    done_ = true;
    error_ = true;
  }
  void SetKeepaliveDelay(int delay) { stun_keepalive_delay_ = delay; }

  void SetKeepaliveLifetime(int lifetime) {
    stun_keepalive_lifetime_ = lifetime;
  }

  cricket::TestStunServer* stun_server_1() { return stun_server_1_.get(); }
  cricket::TestStunServer* stun_server_2() { return stun_server_2_.get(); }

 private:
  std::unique_ptr<rtc::VirtualSocketServer> ss_;
  rtc::AutoSocketServerThread thread_;
  rtc::Network network_;
  rtc::BasicPacketSocketFactory socket_factory_;
  std::unique_ptr<cricket::UDPPort> stun_port_;
  std::unique_ptr<cricket::TestStunServer> stun_server_1_;
  std::unique_ptr<cricket::TestStunServer> stun_server_2_;
  std::unique_ptr<rtc::AsyncPacketSocket> socket_;
  bool done_;
  bool error_;
  int stun_keepalive_delay_;
  int stun_keepalive_lifetime_;
};

class StunPortTestWithRealClock : public StunPortTestBase {};

class FakeClockBase {
 public:
  rtc::ScopedFakeClock fake_clock;
};

class StunPortTest : public FakeClockBase, public StunPortTestBase {};

// Test that we can create a STUN port.
TEST_F(StunPortTest, TestCreateStunPort) {
  CreateStunPort(kStunAddr1);
  EXPECT_EQ("stun", port()->Type());
  EXPECT_EQ(0U, port()->Candidates().size());
}

// Test that we can create a UDP port.
TEST_F(StunPortTest, TestCreateUdpPort) {
  CreateSharedUdpPort(kStunAddr1);
  EXPECT_EQ("local", port()->Type());
  EXPECT_EQ(0U, port()->Candidates().size());
}

// Test that we can get an address from a STUN server.
TEST_F(StunPortTest, TestPrepareAddress) {
  CreateStunPort(kStunAddr1);
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  ASSERT_EQ(1U, port()->Candidates().size());
  EXPECT_TRUE(kLocalAddr.EqualIPs(port()->Candidates()[0].address()));
  std::string expected_server_url = "stun:127.0.0.1:5000";
  EXPECT_EQ(port()->Candidates()[0].url(), expected_server_url);

  // TODO(deadbeef): Add IPv6 tests here.
}

// Test that we fail properly if we can't get an address.
TEST_F(StunPortTest, TestPrepareAddressFail) {
  CreateStunPort(kBadAddr);
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  EXPECT_TRUE(error());
  EXPECT_EQ(0U, port()->Candidates().size());
}

// Test that we can get an address from a STUN server specified by a hostname.
// Crashes on Linux, see webrtc:7416
#if defined(WEBRTC_LINUX)
#define MAYBE_TestPrepareAddressHostname DISABLED_TestPrepareAddressHostname
#else
#define MAYBE_TestPrepareAddressHostname TestPrepareAddressHostname
#endif
TEST_F(StunPortTest, MAYBE_TestPrepareAddressHostname) {
  CreateStunPort(kStunHostnameAddr);
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  ASSERT_EQ(1U, port()->Candidates().size());
  EXPECT_TRUE(kLocalAddr.EqualIPs(port()->Candidates()[0].address()));
  EXPECT_EQ(kStunCandidatePriority, port()->Candidates()[0].priority());
}

// Test that we handle hostname lookup failures properly.
TEST_F(StunPortTestWithRealClock, TestPrepareAddressHostnameFail) {
  CreateStunPort(kBadHostnameAddr);
  PrepareAddress();
  EXPECT_TRUE_WAIT(done(), kTimeoutMs);
  EXPECT_TRUE(error());
  EXPECT_EQ(0U, port()->Candidates().size());
}

// This test verifies keepalive response messages don't result in
// additional candidate generation.
TEST_F(StunPortTest, TestKeepAliveResponse) {
  SetKeepaliveDelay(500);  // 500ms of keepalive delay.
  CreateStunPort(kStunAddr1);
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  ASSERT_EQ(1U, port()->Candidates().size());
  EXPECT_TRUE(kLocalAddr.EqualIPs(port()->Candidates()[0].address()));
  SIMULATED_WAIT(false, 1000, fake_clock);
  EXPECT_EQ(1U, port()->Candidates().size());
}

// Test that a local candidate can be generated using a shared socket.
TEST_F(StunPortTest, TestSharedSocketPrepareAddress) {
  CreateSharedUdpPort(kStunAddr1);
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  ASSERT_EQ(1U, port()->Candidates().size());
  EXPECT_TRUE(kLocalAddr.EqualIPs(port()->Candidates()[0].address()));
}

// Test that we still a get a local candidate with invalid stun server hostname.
// Also verifing that UDPPort can receive packets when stun address can't be
// resolved.
TEST_F(StunPortTestWithRealClock,
       TestSharedSocketPrepareAddressInvalidHostname) {
  CreateSharedUdpPort(kBadHostnameAddr);
  PrepareAddress();
  EXPECT_TRUE_WAIT(done(), kTimeoutMs);
  ASSERT_EQ(1U, port()->Candidates().size());
  EXPECT_TRUE(kLocalAddr.EqualIPs(port()->Candidates()[0].address()));

  // Send data to port after it's ready. This is to make sure, UDP port can
  // handle data with unresolved stun server address.
  std::string data = "some random data, sending to cricket::Port.";
  SendData(data.c_str(), data.length());
  // No crash is success.
}

// Test that the same address is added only once if two STUN servers are in use.
TEST_F(StunPortTest, TestNoDuplicatedAddressWithTwoStunServers) {
  ServerAddresses stun_servers;
  stun_servers.insert(kStunAddr1);
  stun_servers.insert(kStunAddr2);
  CreateStunPort(stun_servers);
  EXPECT_EQ("stun", port()->Type());
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  EXPECT_EQ(1U, port()->Candidates().size());
  EXPECT_EQ(port()->Candidates()[0].relay_protocol(), "");
}

// Test that candidates can be allocated for multiple STUN servers, one of which
// is not reachable.
TEST_F(StunPortTest, TestMultipleStunServersWithBadServer) {
  ServerAddresses stun_servers;
  stun_servers.insert(kStunAddr1);
  stun_servers.insert(kBadAddr);
  CreateStunPort(stun_servers);
  EXPECT_EQ("stun", port()->Type());
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  EXPECT_EQ(1U, port()->Candidates().size());
}

// Test that two candidates are allocated if the two STUN servers return
// different mapped addresses.
TEST_F(StunPortTest, TestTwoCandidatesWithTwoStunServersAcrossNat) {
  const SocketAddress kStunMappedAddr1("77.77.77.77", 0);
  const SocketAddress kStunMappedAddr2("88.77.77.77", 0);
  stun_server_1()->set_fake_stun_addr(kStunMappedAddr1);
  stun_server_2()->set_fake_stun_addr(kStunMappedAddr2);

  ServerAddresses stun_servers;
  stun_servers.insert(kStunAddr1);
  stun_servers.insert(kStunAddr2);
  CreateStunPort(stun_servers);
  EXPECT_EQ("stun", port()->Type());
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  EXPECT_EQ(2U, port()->Candidates().size());
  EXPECT_EQ(port()->Candidates()[0].relay_protocol(), "");
  EXPECT_EQ(port()->Candidates()[1].relay_protocol(), "");
}

// Test that the stun_keepalive_lifetime is set correctly based on the network
// type on a STUN port. Also test that it will be updated if the network type
// changes.
TEST_F(StunPortTest, TestStunPortGetStunKeepaliveLifetime) {
  // Lifetime for the default (unknown) network type is |kInfiniteLifetime|.
  CreateStunPort(kStunAddr1);
  EXPECT_EQ(kInfiniteLifetime, port()->stun_keepalive_lifetime());
  // Lifetime for the cellular network is |kHighCostPortKeepaliveLifetimeMs|
  SetNetworkType(rtc::ADAPTER_TYPE_CELLULAR);
  EXPECT_EQ(kHighCostPortKeepaliveLifetimeMs,
            port()->stun_keepalive_lifetime());

  // Lifetime for the wifi network is |kInfiniteLifetime|.
  SetNetworkType(rtc::ADAPTER_TYPE_WIFI);
  CreateStunPort(kStunAddr2);
  EXPECT_EQ(kInfiniteLifetime, port()->stun_keepalive_lifetime());
}

// Test that the stun_keepalive_lifetime is set correctly based on the network
// type on a shared STUN port (UDPPort). Also test that it will be updated
// if the network type changes.
TEST_F(StunPortTest, TestUdpPortGetStunKeepaliveLifetime) {
  // Lifetime for the default (unknown) network type is |kInfiniteLifetime|.
  CreateSharedUdpPort(kStunAddr1);
  EXPECT_EQ(kInfiniteLifetime, port()->stun_keepalive_lifetime());
  // Lifetime for the cellular network is |kHighCostPortKeepaliveLifetimeMs|.
  SetNetworkType(rtc::ADAPTER_TYPE_CELLULAR);
  EXPECT_EQ(kHighCostPortKeepaliveLifetimeMs,
            port()->stun_keepalive_lifetime());

  // Lifetime for the wifi network type is |kInfiniteLifetime|.
  SetNetworkType(rtc::ADAPTER_TYPE_WIFI);
  CreateSharedUdpPort(kStunAddr2);
  EXPECT_EQ(kInfiniteLifetime, port()->stun_keepalive_lifetime());
}

// Test that STUN binding requests will be stopped shortly if the keep-alive
// lifetime is short.
TEST_F(StunPortTest, TestStunBindingRequestShortLifetime) {
  SetKeepaliveDelay(101);
  SetKeepaliveLifetime(100);
  CreateStunPort(kStunAddr1);
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  EXPECT_TRUE_SIMULATED_WAIT(
      !port()->HasPendingRequest(cricket::STUN_BINDING_REQUEST), 2000,
      fake_clock);
}

// Test that by default, the STUN binding requests will last for a long time.
TEST_F(StunPortTest, TestStunBindingRequestLongLifetime) {
  SetKeepaliveDelay(101);
  CreateStunPort(kStunAddr1);
  PrepareAddress();
  EXPECT_TRUE_SIMULATED_WAIT(done(), kTimeoutMs, fake_clock);
  EXPECT_TRUE_SIMULATED_WAIT(
      port()->HasPendingRequest(cricket::STUN_BINDING_REQUEST), 1000,
      fake_clock);
}
