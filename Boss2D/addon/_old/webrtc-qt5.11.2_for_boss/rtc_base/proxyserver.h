/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_PROXYSERVER_H_
#define RTC_BASE_PROXYSERVER_H_

#include <list>
#include <memory>
#include BOSS_WEBRTC_U_rtc_base__asyncsocket_h //original-code:"rtc_base/asyncsocket.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"
#include BOSS_WEBRTC_U_rtc_base__socketadapters_h //original-code:"rtc_base/socketadapters.h"
#include BOSS_WEBRTC_U_rtc_base__socketaddress_h //original-code:"rtc_base/socketaddress.h"
#include BOSS_WEBRTC_U_rtc_base__stream_h //original-code:"rtc_base/stream.h"

namespace rtc {

class SocketFactory;

// ProxyServer is a base class that allows for easy construction of proxy
// servers. With its helper class ProxyBinding, it contains all the necessary
// logic for receiving and bridging connections. The specific client-server
// proxy protocol is implemented by an instance of the AsyncProxyServerSocket
// class; children of ProxyServer implement WrapSocket appropriately to return
// the correct protocol handler.

class ProxyBinding : public sigslot::has_slots<> {
 public:
  ProxyBinding(AsyncProxyServerSocket* in_socket, AsyncSocket* out_socket);
  ~ProxyBinding() override;
  sigslot::signal1<ProxyBinding*> SignalDestroyed;

 private:
  void OnConnectRequest(AsyncProxyServerSocket* socket,
                        const SocketAddress& addr);
  void OnInternalRead(AsyncSocket* socket);
  void OnInternalWrite(AsyncSocket* socket);
  void OnInternalClose(AsyncSocket* socket, int err);
  void OnExternalConnect(AsyncSocket* socket);
  void OnExternalRead(AsyncSocket* socket);
  void OnExternalWrite(AsyncSocket* socket);
  void OnExternalClose(AsyncSocket* socket, int err);

  static void Read(AsyncSocket* socket, FifoBuffer* buffer);
  static void Write(AsyncSocket* socket, FifoBuffer* buffer);
  void Destroy();

  static const int kBufferSize = 4096;
  std::unique_ptr<AsyncProxyServerSocket> int_socket_;
  std::unique_ptr<AsyncSocket> ext_socket_;
  bool connected_;
  FifoBuffer out_buffer_;
  FifoBuffer in_buffer_;
  RTC_DISALLOW_COPY_AND_ASSIGN(ProxyBinding);
};

class ProxyServer : public sigslot::has_slots<> {
 public:
  ProxyServer(SocketFactory* int_factory, const SocketAddress& int_addr,
              SocketFactory* ext_factory, const SocketAddress& ext_ip);
  ~ProxyServer() override;

  // Returns the address to which the proxy server is bound
  SocketAddress GetServerAddress();

 protected:
  void OnAcceptEvent(AsyncSocket* socket);
  virtual AsyncProxyServerSocket* WrapSocket(AsyncSocket* socket) = 0;
  void OnBindingDestroyed(ProxyBinding* binding);

 private:
  typedef std::list<ProxyBinding*> BindingList;
  SocketFactory* ext_factory_;
  SocketAddress ext_ip_;
  std::unique_ptr<AsyncSocket> server_socket_;
  BindingList bindings_;
  RTC_DISALLOW_COPY_AND_ASSIGN(ProxyServer);
};

// SocksProxyServer is a simple extension of ProxyServer to implement SOCKS.
class SocksProxyServer : public ProxyServer {
 public:
  SocksProxyServer(SocketFactory* int_factory, const SocketAddress& int_addr,
                   SocketFactory* ext_factory, const SocketAddress& ext_ip)
      : ProxyServer(int_factory, int_addr, ext_factory, ext_ip) {
  }
 protected:
  AsyncProxyServerSocket* WrapSocket(AsyncSocket* socket) override;
  RTC_DISALLOW_COPY_AND_ASSIGN(SocksProxyServer);
};

}  // namespace rtc

#endif  // RTC_BASE_PROXYSERVER_H_
