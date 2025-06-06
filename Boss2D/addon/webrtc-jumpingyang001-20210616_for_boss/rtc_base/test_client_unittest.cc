/*
 *  Copyright 2006 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__test_client_h //original-code:"rtc_base/test_client.h"

#include <utility>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_rtc_base__async_socket_h //original-code:"rtc_base/async_socket.h"
#include BOSS_WEBRTC_U_rtc_base__async_tcp_socket_h //original-code:"rtc_base/async_tcp_socket.h"
#include BOSS_WEBRTC_U_rtc_base__async_udp_socket_h //original-code:"rtc_base/async_udp_socket.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__net_helpers_h //original-code:"rtc_base/net_helpers.h"
#include BOSS_WEBRTC_U_rtc_base__socket_server_h //original-code:"rtc_base/socket_server.h"
#include BOSS_WEBRTC_U_rtc_base__test_echo_server_h //original-code:"rtc_base/test_echo_server.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace rtc {
namespace {

#define MAYBE_SKIP_IPV4                        \
  if (!HasIPv4Enabled()) {                     \
    RTC_LOG(LS_INFO) << "No IPv4... skipping"; \
    return;                                    \
  }

#define MAYBE_SKIP_IPV6                        \
  if (!HasIPv6Enabled()) {                     \
    RTC_LOG(LS_INFO) << "No IPv6... skipping"; \
    return;                                    \
  }

void TestUdpInternal(const SocketAddress& loopback) {
  Thread* main = Thread::Current();
  AsyncSocket* socket =
      main->socketserver()->CreateAsyncSocket(loopback.family(), SOCK_DGRAM);
  socket->Bind(loopback);

  TestClient client(std::make_unique<AsyncUDPSocket>(socket));
  SocketAddress addr = client.address(), from;
  EXPECT_EQ(3, client.SendTo("foo", 3, addr));
  EXPECT_TRUE(client.CheckNextPacket("foo", 3, &from));
  EXPECT_EQ(from, addr);
  EXPECT_TRUE(client.CheckNoPacket());
}

void TestTcpInternal(const SocketAddress& loopback) {
  Thread* main = Thread::Current();
  TestEchoServer server(main, loopback);

  AsyncSocket* socket =
      main->socketserver()->CreateAsyncSocket(loopback.family(), SOCK_STREAM);
  std::unique_ptr<AsyncTCPSocket> tcp_socket = absl::WrapUnique(
      AsyncTCPSocket::Create(socket, loopback, server.address()));
  ASSERT_TRUE(tcp_socket != nullptr);

  TestClient client(std::move(tcp_socket));
  SocketAddress addr = client.address(), from;
  EXPECT_TRUE(client.CheckConnected());
  EXPECT_EQ(3, client.Send("foo", 3));
  EXPECT_TRUE(client.CheckNextPacket("foo", 3, &from));
  EXPECT_EQ(from, server.address());
  EXPECT_TRUE(client.CheckNoPacket());
}

// Tests whether the TestClient can send UDP to itself.
TEST(TestClientTest, TestUdpIPv4) {
  MAYBE_SKIP_IPV4;
  TestUdpInternal(SocketAddress("127.0.0.1", 0));
}

#if defined(WEBRTC_LINUX)
#define MAYBE_TestUdpIPv6 DISABLED_TestUdpIPv6
#else
#define MAYBE_TestUdpIPv6 TestUdpIPv6
#endif
TEST(TestClientTest, MAYBE_TestUdpIPv6) {
  MAYBE_SKIP_IPV6;
  TestUdpInternal(SocketAddress("::1", 0));
}

// Tests whether the TestClient can connect to a server and exchange data.
TEST(TestClientTest, TestTcpIPv4) {
  MAYBE_SKIP_IPV4;
  TestTcpInternal(SocketAddress("127.0.0.1", 0));
}

#if defined(WEBRTC_LINUX)
#define MAYBE_TestTcpIPv6 DISABLED_TestTcpIPv6
#else
#define MAYBE_TestTcpIPv6 TestTcpIPv6
#endif
TEST(TestClientTest, MAYBE_TestTcpIPv6) {
  MAYBE_SKIP_IPV6;
  TestTcpInternal(SocketAddress("::1", 0));
}

}  // namespace
}  // namespace rtc
