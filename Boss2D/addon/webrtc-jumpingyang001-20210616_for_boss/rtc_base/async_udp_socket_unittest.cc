/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__async_udp_socket_h //original-code:"rtc_base/async_udp_socket.h"

#include <memory>
#include <string>

#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__physical_socket_server_h //original-code:"rtc_base/physical_socket_server.h"
#include BOSS_WEBRTC_U_rtc_base__virtual_socket_server_h //original-code:"rtc_base/virtual_socket_server.h"

namespace rtc {

class AsyncUdpSocketTest : public ::testing::Test, public sigslot::has_slots<> {
 public:
  AsyncUdpSocketTest()
      : pss_(new rtc::PhysicalSocketServer),
        vss_(new rtc::VirtualSocketServer(pss_.get())),
        socket_(vss_->CreateAsyncSocket(SOCK_DGRAM)),
        udp_socket_(new AsyncUDPSocket(socket_)),
        ready_to_send_(false) {
    udp_socket_->SignalReadyToSend.connect(this,
                                           &AsyncUdpSocketTest::OnReadyToSend);
  }

  void OnReadyToSend(rtc::AsyncPacketSocket* socket) { ready_to_send_ = true; }

 protected:
  std::unique_ptr<PhysicalSocketServer> pss_;
  std::unique_ptr<VirtualSocketServer> vss_;
  AsyncSocket* socket_;
  std::unique_ptr<AsyncUDPSocket> udp_socket_;
  bool ready_to_send_;
};

TEST_F(AsyncUdpSocketTest, OnWriteEvent) {
  EXPECT_FALSE(ready_to_send_);
  socket_->SignalWriteEvent(socket_);
  EXPECT_TRUE(ready_to_send_);
}

}  // namespace rtc
