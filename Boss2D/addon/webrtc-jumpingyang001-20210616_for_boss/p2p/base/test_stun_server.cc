/*
 *  Copyright 2017 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_p2p__base__test_stun_server_h //original-code:"p2p/base/test_stun_server.h"

#include BOSS_WEBRTC_U_rtc_base__async_socket_h //original-code:"rtc_base/async_socket.h"
#include BOSS_WEBRTC_U_rtc_base__socket_server_h //original-code:"rtc_base/socket_server.h"

namespace cricket {

TestStunServer* TestStunServer::Create(rtc::SocketServer* ss,
                                       const rtc::SocketAddress& addr) {
  rtc::AsyncSocket* socket = ss->CreateAsyncSocket(addr.family(), SOCK_DGRAM);
  rtc::AsyncUDPSocket* udp_socket = rtc::AsyncUDPSocket::Create(socket, addr);

  return new TestStunServer(udp_socket);
}

void TestStunServer::OnBindingRequest(StunMessage* msg,
                                      const rtc::SocketAddress& remote_addr) {
  if (fake_stun_addr_.IsNil()) {
    StunServer::OnBindingRequest(msg, remote_addr);
  } else {
    StunMessage response;
    GetStunBindResponse(msg, fake_stun_addr_, &response);
    SendResponse(response, remote_addr);
  }
}

}  // namespace cricket
