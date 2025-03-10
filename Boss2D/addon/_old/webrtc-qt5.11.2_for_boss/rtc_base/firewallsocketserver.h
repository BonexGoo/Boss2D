/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_FIREWALLSOCKETSERVER_H_
#define RTC_BASE_FIREWALLSOCKETSERVER_H_

#include <vector>
#include BOSS_WEBRTC_U_rtc_base__criticalsection_h //original-code:"rtc_base/criticalsection.h"
#include BOSS_WEBRTC_U_rtc_base__socketserver_h //original-code:"rtc_base/socketserver.h"

namespace rtc {

class FirewallManager;

// This SocketServer shim simulates a rule-based firewall server.

enum FirewallProtocol { FP_UDP, FP_TCP, FP_ANY };
enum FirewallDirection { FD_IN, FD_OUT, FD_ANY };

class FirewallSocketServer : public SocketServer {
 public:
  FirewallSocketServer(SocketServer* server,
                       FirewallManager* manager = nullptr,
                       bool should_delete_server = false);
  ~FirewallSocketServer() override;

  SocketServer* socketserver() const { return server_; }
  void set_socketserver(SocketServer* server) {
    if (server_ && should_delete_server_) {
      delete server_;
      server_ = nullptr;
      should_delete_server_ = false;
    }
    server_ = server;
  }

  // Settings to control whether CreateSocket or Socket::Listen succeed.
  void set_udp_sockets_enabled(bool enabled) { udp_sockets_enabled_ = enabled; }
  void set_tcp_sockets_enabled(bool enabled) { tcp_sockets_enabled_ = enabled; }
  bool tcp_listen_enabled() const { return tcp_listen_enabled_; }
  void set_tcp_listen_enabled(bool enabled) { tcp_listen_enabled_ = enabled; }

  // Rules govern the behavior of Connect/Accept/Send/Recv attempts.
  void AddRule(bool allow, FirewallProtocol p = FP_ANY,
               FirewallDirection d = FD_ANY,
               const SocketAddress& addr = SocketAddress());
  void AddRule(bool allow, FirewallProtocol p,
               const SocketAddress& src, const SocketAddress& dst);
  void ClearRules();

  bool Check(FirewallProtocol p,
             const SocketAddress& src, const SocketAddress& dst);

  // Set the IP addresses for which Bind will fail. By default this list is
  // empty. This can be used to simulate a real OS that refuses to bind to
  // addresses under various circumstances.
  //
  // No matter how many addresses are added (including INADDR_ANY), the server
  // will still allow creating outgoing TCP connections, since they don't
  // require explicitly binding a socket.
  void SetUnbindableIps(const std::vector<rtc::IPAddress>& unbindable_ips);
  bool IsBindableIp(const rtc::IPAddress& ip);

  Socket* CreateSocket(int type) override;
  Socket* CreateSocket(int family, int type) override;

  AsyncSocket* CreateAsyncSocket(int type) override;
  AsyncSocket* CreateAsyncSocket(int family, int type) override;

  void SetMessageQueue(MessageQueue* queue) override;
  bool Wait(int cms, bool process_io) override;
  void WakeUp() override;

  Socket * WrapSocket(Socket * sock, int type);
  AsyncSocket * WrapSocket(AsyncSocket * sock, int type);

 private:
  SocketServer * server_;
  FirewallManager * manager_;
  CriticalSection crit_;
  struct Rule {
    bool allow;
    FirewallProtocol p;
    FirewallDirection d;
    SocketAddress src;
    SocketAddress dst;
  };
  std::vector<Rule> rules_;
  std::vector<rtc::IPAddress> unbindable_ips_;
  bool should_delete_server_;
  bool udp_sockets_enabled_;
  bool tcp_sockets_enabled_;
  bool tcp_listen_enabled_;
};

// FirewallManager allows you to manage firewalls in multiple threads together

class FirewallManager {
 public:
  FirewallManager();
  ~FirewallManager();

  void AddServer(FirewallSocketServer * server);
  void RemoveServer(FirewallSocketServer * server);

  void AddRule(bool allow, FirewallProtocol p = FP_ANY,
               FirewallDirection d = FD_ANY,
               const SocketAddress& addr = SocketAddress());
  void ClearRules();

 private:
  CriticalSection crit_;
  std::vector<FirewallSocketServer *> servers_;
};

}  // namespace rtc

#endif  // RTC_BASE_FIREWALLSOCKETSERVER_H_
