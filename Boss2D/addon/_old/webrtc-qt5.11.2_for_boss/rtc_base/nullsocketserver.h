/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_NULLSOCKETSERVER_H_
#define RTC_BASE_NULLSOCKETSERVER_H_

#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_rtc_base__socketserver_h //original-code:"rtc_base/socketserver.h"

namespace rtc {

class NullSocketServer : public SocketServer {
 public:
  NullSocketServer();
  ~NullSocketServer() override;

  bool Wait(int cms, bool process_io) override;
  void WakeUp() override;

  Socket* CreateSocket(int type) override;
  Socket* CreateSocket(int family, int type) override;
  AsyncSocket* CreateAsyncSocket(int type) override;
  AsyncSocket* CreateAsyncSocket(int family, int type) override;

 private:
  Event event_;
};

}  // namespace rtc

#endif  // RTC_BASE_NULLSOCKETSERVER_H_
