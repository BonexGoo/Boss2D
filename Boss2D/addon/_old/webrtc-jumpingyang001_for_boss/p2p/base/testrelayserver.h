/*
 *  Copyright 2008 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef P2P_BASE_TESTRELAYSERVER_H_
#define P2P_BASE_TESTRELAYSERVER_H_

#include <memory>

#include BOSS_WEBRTC_U_p2p__base__relayserver_h //original-code:"p2p/base/relayserver.h"
#include BOSS_WEBRTC_U_rtc_base__asynctcpsocket_h //original-code:"rtc_base/asynctcpsocket.h"
#include BOSS_WEBRTC_U_rtc_base__socketadapters_h //original-code:"rtc_base/socketadapters.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__sigslot__sigslot_h //original-code:"rtc_base/third_party/sigslot/sigslot.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"

namespace cricket {

// A test relay server. Useful for unit tests.
class TestRelayServer : public sigslot::has_slots<> {
 public:
  TestRelayServer(rtc::Thread* thread,
                  const rtc::SocketAddress& udp_int_addr,
                  const rtc::SocketAddress& udp_ext_addr,
                  const rtc::SocketAddress& tcp_int_addr,
                  const rtc::SocketAddress& tcp_ext_addr,
                  const rtc::SocketAddress& ssl_int_addr,
                  const rtc::SocketAddress& ssl_ext_addr)
      : server_(thread) {
    server_.AddInternalSocket(
        rtc::AsyncUDPSocket::Create(thread->socketserver(), udp_int_addr));
    server_.AddExternalSocket(
        rtc::AsyncUDPSocket::Create(thread->socketserver(), udp_ext_addr));

    tcp_int_socket_.reset(CreateListenSocket(thread, tcp_int_addr));
    tcp_ext_socket_.reset(CreateListenSocket(thread, tcp_ext_addr));
    ssl_int_socket_.reset(CreateListenSocket(thread, ssl_int_addr));
    ssl_ext_socket_.reset(CreateListenSocket(thread, ssl_ext_addr));
  }
  int GetConnectionCount() const { return server_.GetConnectionCount(); }
  rtc::SocketAddressPair GetConnection(int connection) const {
    return server_.GetConnection(connection);
  }
  bool HasConnection(const rtc::SocketAddress& address) const {
    return server_.HasConnection(address);
  }

 private:
  rtc::AsyncSocket* CreateListenSocket(rtc::Thread* thread,
                                       const rtc::SocketAddress& addr) {
    rtc::AsyncSocket* socket =
        thread->socketserver()->CreateAsyncSocket(addr.family(), SOCK_STREAM);
    socket->Bind(addr);
    socket->Listen(5);
    socket->SignalReadEvent.connect(this, &TestRelayServer::OnAccept);
    return socket;
  }
  void OnAccept(rtc::AsyncSocket* socket) {
    bool external =
        (socket == tcp_ext_socket_.get() || socket == ssl_ext_socket_.get());
    bool ssl =
        (socket == ssl_int_socket_.get() || socket == ssl_ext_socket_.get());
    rtc::AsyncSocket* raw_socket = socket->Accept(NULL);
    if (raw_socket) {
      rtc::AsyncTCPSocket* packet_socket = new rtc::AsyncTCPSocket(
          (!ssl) ? raw_socket : new rtc::AsyncSSLServerSocket(raw_socket),
          false);
      if (!external) {
        packet_socket->SignalClose.connect(this,
                                           &TestRelayServer::OnInternalClose);
        server_.AddInternalSocket(packet_socket);
      } else {
        packet_socket->SignalClose.connect(this,
                                           &TestRelayServer::OnExternalClose);
        server_.AddExternalSocket(packet_socket);
      }
    }
  }
  void OnInternalClose(rtc::AsyncPacketSocket* socket, int error) {
    server_.RemoveInternalSocket(socket);
  }
  void OnExternalClose(rtc::AsyncPacketSocket* socket, int error) {
    server_.RemoveExternalSocket(socket);
  }

 private:
  cricket::RelayServer server_;
  std::unique_ptr<rtc::AsyncSocket> tcp_int_socket_;
  std::unique_ptr<rtc::AsyncSocket> tcp_ext_socket_;
  std::unique_ptr<rtc::AsyncSocket> ssl_int_socket_;
  std::unique_ptr<rtc::AsyncSocket> ssl_ext_socket_;
};

}  // namespace cricket

#endif  // P2P_BASE_TESTRELAYSERVER_H_
