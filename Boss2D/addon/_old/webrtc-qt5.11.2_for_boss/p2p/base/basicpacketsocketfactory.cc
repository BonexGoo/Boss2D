/*
 *  Copyright 2011 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_p2p__base__basicpacketsocketfactory_h //original-code:"p2p/base/basicpacketsocketfactory.h"

#include <string>

#include BOSS_WEBRTC_U_p2p__base__asyncstuntcpsocket_h //original-code:"p2p/base/asyncstuntcpsocket.h"
#include BOSS_WEBRTC_U_p2p__base__stun_h //original-code:"p2p/base/stun.h"
#include BOSS_WEBRTC_U_rtc_base__asynctcpsocket_h //original-code:"rtc_base/asynctcpsocket.h"
#include BOSS_WEBRTC_U_rtc_base__asyncudpsocket_h //original-code:"rtc_base/asyncudpsocket.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__nethelpers_h //original-code:"rtc_base/nethelpers.h"
#include BOSS_WEBRTC_U_rtc_base__physicalsocketserver_h //original-code:"rtc_base/physicalsocketserver.h"
#include BOSS_WEBRTC_U_rtc_base__socketadapters_h //original-code:"rtc_base/socketadapters.h"
#include BOSS_WEBRTC_U_rtc_base__ssladapter_h //original-code:"rtc_base/ssladapter.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"

namespace rtc {

BasicPacketSocketFactory::BasicPacketSocketFactory()
    : thread_(Thread::Current()),
      socket_factory_(NULL) {
}

BasicPacketSocketFactory::BasicPacketSocketFactory(Thread* thread)
    : thread_(thread),
      socket_factory_(NULL) {
}

BasicPacketSocketFactory::BasicPacketSocketFactory(
    SocketFactory* socket_factory)
    : thread_(NULL),
      socket_factory_(socket_factory) {
}

BasicPacketSocketFactory::~BasicPacketSocketFactory() {
}

AsyncPacketSocket* BasicPacketSocketFactory::CreateUdpSocket(
    const SocketAddress& address,
    uint16_t min_port,
    uint16_t max_port) {
  // UDP sockets are simple.
  AsyncSocket* socket =
      socket_factory()->CreateAsyncSocket(address.family(), SOCK_DGRAM);
  if (!socket) {
    return NULL;
  }
  if (BindSocket(socket, address, min_port, max_port) < 0) {
    RTC_LOG(LS_ERROR) << "UDP bind failed with error " << socket->GetError();
    delete socket;
    return NULL;
  }
  return new AsyncUDPSocket(socket);
}

AsyncPacketSocket* BasicPacketSocketFactory::CreateServerTcpSocket(
    const SocketAddress& local_address,
    uint16_t min_port,
    uint16_t max_port,
    int opts) {
  // Fail if TLS is required.
  if (opts & PacketSocketFactory::OPT_TLS) {
    RTC_LOG(LS_ERROR) << "TLS support currently is not available.";
    return NULL;
  }

  AsyncSocket* socket =
      socket_factory()->CreateAsyncSocket(local_address.family(), SOCK_STREAM);
  if (!socket) {
    return NULL;
  }

  if (BindSocket(socket, local_address, min_port, max_port) < 0) {
    RTC_LOG(LS_ERROR) << "TCP bind failed with error " << socket->GetError();
    delete socket;
    return NULL;
  }

  // If using fake TLS, wrap the TCP socket in a pseudo-SSL socket.
  if (opts & PacketSocketFactory::OPT_TLS_FAKE) {
    RTC_DCHECK(!(opts & PacketSocketFactory::OPT_TLS));
    socket = new AsyncSSLSocket(socket);
  }

  // Set TCP_NODELAY (via OPT_NODELAY) for improved performance.
  // See http://go/gtalktcpnodelayexperiment
  socket->SetOption(Socket::OPT_NODELAY, 1);

  if (opts & PacketSocketFactory::OPT_STUN)
    return new cricket::AsyncStunTCPSocket(socket, true);

  return new AsyncTCPSocket(socket, true);
}

AsyncPacketSocket* BasicPacketSocketFactory::CreateClientTcpSocket(
    const SocketAddress& local_address,
    const SocketAddress& remote_address,
    const ProxyInfo& proxy_info,
    const std::string& user_agent,
    int opts) {
  PacketSocketTcpOptions tcp_options;
  tcp_options.opts = opts;
  return CreateClientTcpSocket(local_address, remote_address, proxy_info,
                               user_agent, tcp_options);
}

AsyncPacketSocket* BasicPacketSocketFactory::CreateClientTcpSocket(
    const SocketAddress& local_address,
    const SocketAddress& remote_address,
    const ProxyInfo& proxy_info,
    const std::string& user_agent,
    const PacketSocketTcpOptions& tcp_options) {
  AsyncSocket* socket =
      socket_factory()->CreateAsyncSocket(local_address.family(), SOCK_STREAM);
  if (!socket) {
    return NULL;
  }

  if (BindSocket(socket, local_address, 0, 0) < 0) {
    // Allow BindSocket to fail if we're binding to the ANY address, since this
    // is mostly redundant in the first place. The socket will be bound when we
    // call Connect() instead.
    if (local_address.IsAnyIP()) {
      RTC_LOG(LS_WARNING) << "TCP bind failed with error " << socket->GetError()
                          << "; ignoring since socket is using 'any' address.";
    } else {
      RTC_LOG(LS_ERROR) << "TCP bind failed with error " << socket->GetError();
      delete socket;
      return NULL;
    }
  }

  // If using a proxy, wrap the socket in a proxy socket.
  if (proxy_info.type == PROXY_SOCKS5) {
    socket = new AsyncSocksProxySocket(
        socket, proxy_info.address, proxy_info.username, proxy_info.password);
  } else if (proxy_info.type == PROXY_HTTPS) {
    socket =
        new AsyncHttpsProxySocket(socket, user_agent, proxy_info.address,
                                  proxy_info.username, proxy_info.password);
  }

  // Assert that at most one TLS option is used.
  int tlsOpts = tcp_options.opts & (PacketSocketFactory::OPT_TLS |
                                    PacketSocketFactory::OPT_TLS_FAKE |
                                    PacketSocketFactory::OPT_TLS_INSECURE);
  RTC_DCHECK((tlsOpts & (tlsOpts - 1)) == 0);

  if ((tlsOpts & PacketSocketFactory::OPT_TLS) ||
      (tlsOpts & PacketSocketFactory::OPT_TLS_INSECURE)) {
    // Using TLS, wrap the socket in an SSL adapter.
    SSLAdapter* ssl_adapter = SSLAdapter::Create(socket);
    if (!ssl_adapter) {
      return NULL;
    }

    if (tlsOpts & PacketSocketFactory::OPT_TLS_INSECURE) {
      ssl_adapter->SetIgnoreBadCert(true);
    }

    ssl_adapter->SetAlpnProtocols(tcp_options.tls_alpn_protocols);
    ssl_adapter->SetEllipticCurves(tcp_options.tls_elliptic_curves);

    socket = ssl_adapter;

    if (ssl_adapter->StartSSL(remote_address.hostname().c_str(), false) != 0) {
      delete ssl_adapter;
      return NULL;
    }

  } else if (tlsOpts & PacketSocketFactory::OPT_TLS_FAKE) {
    // Using fake TLS, wrap the TCP socket in a pseudo-SSL socket.
    socket = new AsyncSSLSocket(socket);
  }

  if (socket->Connect(remote_address) < 0) {
    RTC_LOG(LS_ERROR) << "TCP connect failed with error " << socket->GetError();
    delete socket;
    return NULL;
  }

  // Finally, wrap that socket in a TCP or STUN TCP packet socket.
  AsyncPacketSocket* tcp_socket;
  if (tcp_options.opts & PacketSocketFactory::OPT_STUN) {
    tcp_socket = new cricket::AsyncStunTCPSocket(socket, false);
  } else {
    tcp_socket = new AsyncTCPSocket(socket, false);
  }

  // Set TCP_NODELAY (via OPT_NODELAY) for improved performance.
  // See http://go/gtalktcpnodelayexperiment
  tcp_socket->SetOption(Socket::OPT_NODELAY, 1);

  return tcp_socket;
}

AsyncResolverInterface* BasicPacketSocketFactory::CreateAsyncResolver() {
  return new AsyncResolver();
}

int BasicPacketSocketFactory::BindSocket(AsyncSocket* socket,
                                         const SocketAddress& local_address,
                                         uint16_t min_port,
                                         uint16_t max_port) {
  int ret = -1;
  if (min_port == 0 && max_port == 0) {
    // If there's no port range, let the OS pick a port for us.
    ret = socket->Bind(local_address);
  } else {
    // Otherwise, try to find a port in the provided range.
    for (int port = min_port; ret < 0 && port <= max_port; ++port) {
      ret = socket->Bind(SocketAddress(local_address.ipaddr(), port));
    }
  }
  return ret;
}

SocketFactory* BasicPacketSocketFactory::socket_factory() {
  if (thread_) {
    RTC_DCHECK(thread_ == Thread::Current());
    return thread_->socketserver();
  } else {
    return socket_factory_;
  }
}

}  // namespace rtc
