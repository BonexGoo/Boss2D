/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/*
 *  This is a diagram of how TCP reconnect works for the active side. The
 *  passive side just waits for an incoming connection.
 *
 *  - Connected: Indicate whether the TCP socket is connected.
 *
 *  - Writable: Whether the stun binding is completed. Sending a data packet
 *    before stun binding completed will trigger IPC socket layer to shutdown
 *    the connection.
 *
 *  - PendingTCP: |connection_pending_| indicates whether there is an
 *    outstanding TCP connection in progress.
 *
 *  - PretendWri: Tracked by |pretending_to_be_writable_|. Marking connection as
 *    WRITE_TIMEOUT will cause the connection be deleted. Instead, we're
 *    "pretending" we're still writable for a period of time such that reconnect
 *    could work.
 *
 *  Data could only be sent in state 3. Sening data during state 2 & 6 will get
 *  EWOULDBLOCK, 4 & 5 EPIPE.
 *
 *         OS Timeout         7 -------------+
 *   +----------------------->|Connected: N  |
 *   |                        |Writable:  N  |     Timeout
 *   |       Timeout          |Connection is |<----------------+
 *   |   +------------------->|Dead          |                 |
 *   |   |                    +--------------+                 |
 *   |   |                               ^                     |
 *   |   |            OnClose            |                     |
 *   |   |    +-----------------------+  |                     |
 *   |   |    |                       |  |Timeout              |
 *   |   |    v                       |  |                     |
 *   | 4 +----------+          5 -----+--+--+           6 -----+-----+
 *   | |Connected: N|Send() or |Connected: N|           |Connected: Y|
 *   | |Writable:  Y|Ping()    |Writable:  Y|OnConnect  |Writable:  Y|
 *   | |PendingTCP:N+--------> |PendingTCP:Y+---------> |PendingTCP:N|
 *   | |PretendWri:Y|          |PretendWri:Y|           |PretendWri:Y|
 *   | +-----+------+          +------------+           +---+--+-----+
 *   |   ^   ^                                              |  |
 *   |   |   |                     OnClose                  |  |
 *   |   |   +----------------------------------------------+  |
 *   |   |                                                     |
 *   |   |                              Stun Binding Completed |
 *   |   |                                                     |
 *   |   |                    OnClose                          |
 *   |   +------------------------------------------------+    |
 *   |                                                    |    v
 *  1 -----------+           2 -----------+Stun      3 -----------+
 *  |Connected: N|           |Connected: Y|Binding   |Connected: Y|
 *  |Writable:  N|OnConnect  |Writable:  N|Completed |Writable:  Y|
 *  |PendingTCP:Y+---------> |PendingTCP:N+--------> |PendingTCP:N|
 *  |PretendWri:N|           |PretendWri:N|          |PretendWri:N|
 *  +------------+           +------------+          +------------+
 *
 */

#include BOSS_WEBRTC_U_p2p__base__tcp_port_h //original-code:"p2p/base/tcp_port.h"

#include <errno.h>

#include <vector>

#include BOSS_ABSEILCPP_U_absl__algorithm__container_h //original-code:"absl/algorithm/container.h"
#include BOSS_WEBRTC_U_p2p__base__p2p_constants_h //original-code:"p2p/base/p2p_constants.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__ip_address_h //original-code:"rtc_base/ip_address.h"
#include BOSS_WEBRTC_U_rtc_base__location_h //original-code:"rtc_base/location.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__net_helper_h //original-code:"rtc_base/net_helper.h"
#include BOSS_WEBRTC_U_rtc_base__rate_tracker_h //original-code:"rtc_base/rate_tracker.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__sigslot__sigslot_h //original-code:"rtc_base/third_party/sigslot/sigslot.h"

namespace cricket {

TCPPort::TCPPort(rtc::Thread* thread,
                 rtc::PacketSocketFactory* factory,
                 rtc::Network* network,
                 uint16_t min_port,
                 uint16_t max_port,
                 const std::string& username,
                 const std::string& password,
                 bool allow_listen)
    : Port(thread,
           LOCAL_PORT_TYPE,
           factory,
           network,
           min_port,
           max_port,
           username,
           password),
      allow_listen_(allow_listen),
      socket_(NULL),
      error_(0) {
  // TODO(mallinath) - Set preference value as per RFC 6544.
  // http://b/issue?id=7141794
  if (allow_listen_) {
    TryCreateServerSocket();
  }
}

TCPPort::~TCPPort() {
  delete socket_;
  std::list<Incoming>::iterator it;
  for (it = incoming_.begin(); it != incoming_.end(); ++it)
    delete it->socket;
  incoming_.clear();
}

Connection* TCPPort::CreateConnection(const Candidate& address,
                                      CandidateOrigin origin) {
  if (!SupportsProtocol(address.protocol())) {
    return NULL;
  }

  if ((address.tcptype() == TCPTYPE_ACTIVE_STR &&
       address.type() != PRFLX_PORT_TYPE) ||
      (address.tcptype().empty() && address.address().port() == 0)) {
    // It's active only candidate, we should not try to create connections
    // for these candidates.
    return NULL;
  }

  // We can't accept TCP connections incoming on other ports
  if (origin == ORIGIN_OTHER_PORT)
    return NULL;

  // We don't know how to act as an ssl server yet
  if ((address.protocol() == SSLTCP_PROTOCOL_NAME) &&
      (origin == ORIGIN_THIS_PORT)) {
    return NULL;
  }

  if (!IsCompatibleAddress(address.address())) {
    return NULL;
  }

  TCPConnection* conn = NULL;
  if (rtc::AsyncPacketSocket* socket = GetIncoming(address.address(), true)) {
    // Incoming connection; we already created a socket and connected signals,
    // so we need to hand off the "read packet" responsibility to
    // TCPConnection.
    socket->SignalReadPacket.disconnect(this);
    conn = new TCPConnection(this, address, socket);
  } else {
    // Outgoing connection, which will create a new socket for which we still
    // need to connect SignalReadyToSend and SignalSentPacket.
    conn = new TCPConnection(this, address);
    if (conn->socket()) {
      conn->socket()->SignalReadyToSend.connect(this, &TCPPort::OnReadyToSend);
      conn->socket()->SignalSentPacket.connect(this, &TCPPort::OnSentPacket);
    }
  }
  AddOrReplaceConnection(conn);
  return conn;
}

void TCPPort::PrepareAddress() {
  if (socket_) {
    // If socket isn't bound yet the address will be added in
    // OnAddressReady(). Socket may be in the CLOSED state if Listen()
    // failed, we still want to add the socket address.
    RTC_LOG(LS_VERBOSE) << "Preparing TCP address, current state: "
                        << socket_->GetState();
    if (socket_->GetState() == rtc::AsyncPacketSocket::STATE_BOUND ||
        socket_->GetState() == rtc::AsyncPacketSocket::STATE_CLOSED)
      AddAddress(socket_->GetLocalAddress(), socket_->GetLocalAddress(),
                 rtc::SocketAddress(), TCP_PROTOCOL_NAME, "",
                 TCPTYPE_PASSIVE_STR, LOCAL_PORT_TYPE,
                 ICE_TYPE_PREFERENCE_HOST_TCP, 0, "", true);
  } else {
    RTC_LOG(LS_INFO) << ToString()
                     << ": Not listening due to firewall restrictions.";
    // Note: We still add the address, since otherwise the remote side won't
    // recognize our incoming TCP connections. According to
    // https://tools.ietf.org/html/rfc6544#section-4.5, for active candidate,
    // the port must be set to the discard port, i.e. 9. We can't be 100% sure
    // which IP address will actually be used, so GetBestIP is as good as we
    // can do.
    // TODO(deadbeef): We could do something like create a dummy socket just to
    // see what IP we get. But that may be overkill.
    AddAddress(rtc::SocketAddress(Network()->GetBestIP(), DISCARD_PORT),
               rtc::SocketAddress(Network()->GetBestIP(), 0),
               rtc::SocketAddress(), TCP_PROTOCOL_NAME, "", TCPTYPE_ACTIVE_STR,
               LOCAL_PORT_TYPE, ICE_TYPE_PREFERENCE_HOST_TCP, 0, "", true);
  }
}

int TCPPort::SendTo(const void* data,
                    size_t size,
                    const rtc::SocketAddress& addr,
                    const rtc::PacketOptions& options,
                    bool payload) {
  rtc::AsyncPacketSocket* socket = NULL;
  TCPConnection* conn = static_cast<TCPConnection*>(GetConnection(addr));

  // For Connection, this is the code path used by Ping() to establish
  // WRITABLE. It has to send through the socket directly as TCPConnection::Send
  // checks writability.
  if (conn) {
    if (!conn->connected()) {
      conn->MaybeReconnect();
      return SOCKET_ERROR;
    }
    socket = conn->socket();
    if (!socket) {
      // The failure to initialize should have been logged elsewhere,
      // so this log is not important.
      RTC_LOG(LS_INFO) << ToString()
                       << ": Attempted to send to an uninitialized socket: "
                       << addr.ToSensitiveString();
      error_ = EHOSTUNREACH;
      return SOCKET_ERROR;
    }
  } else {
    socket = GetIncoming(addr);
    if (!socket) {
      RTC_LOG(LS_ERROR) << ToString()
                        << ": Attempted to send to an unknown destination: "
                        << addr.ToSensitiveString();
      error_ = EHOSTUNREACH;
      return SOCKET_ERROR;
    }
  }
  rtc::PacketOptions modified_options(options);
  CopyPortInformationToPacketInfo(&modified_options.info_signaled_after_sent);
  int sent = socket->Send(data, size, modified_options);
  if (sent < 0) {
    error_ = socket->GetError();
    // Error from this code path for a Connection (instead of from a bare
    // socket) will not trigger reconnecting. In theory, this shouldn't matter
    // as OnClose should always be called and set connected to false.
    RTC_LOG(LS_ERROR) << ToString() << ": TCP send of " << size
                      << " bytes failed with error " << error_;
  }
  return sent;
}

int TCPPort::GetOption(rtc::Socket::Option opt, int* value) {
  if (socket_) {
    return socket_->GetOption(opt, value);
  } else {
    return SOCKET_ERROR;
  }
}

int TCPPort::SetOption(rtc::Socket::Option opt, int value) {
  if (socket_) {
    return socket_->SetOption(opt, value);
  } else {
    return SOCKET_ERROR;
  }
}

int TCPPort::GetError() {
  return error_;
}

bool TCPPort::SupportsProtocol(const std::string& protocol) const {
  return protocol == TCP_PROTOCOL_NAME || protocol == SSLTCP_PROTOCOL_NAME;
}

ProtocolType TCPPort::GetProtocol() const {
  return PROTO_TCP;
}

void TCPPort::OnNewConnection(rtc::AsyncPacketSocket* socket,
                              rtc::AsyncPacketSocket* new_socket) {
  RTC_DCHECK(socket == socket_);

  Incoming incoming;
  incoming.addr = new_socket->GetRemoteAddress();
  incoming.socket = new_socket;
  incoming.socket->SignalReadPacket.connect(this, &TCPPort::OnReadPacket);
  incoming.socket->SignalReadyToSend.connect(this, &TCPPort::OnReadyToSend);
  incoming.socket->SignalSentPacket.connect(this, &TCPPort::OnSentPacket);

  RTC_LOG(LS_VERBOSE) << ToString() << ": Accepted connection from "
                      << incoming.addr.ToSensitiveString();
  incoming_.push_back(incoming);
}

void TCPPort::TryCreateServerSocket() {
  socket_ = socket_factory()->CreateServerTcpSocket(
      rtc::SocketAddress(Network()->GetBestIP(), 0), min_port(), max_port(),
      false /* ssl */);
  if (!socket_) {
    RTC_LOG(LS_WARNING)
        << ToString()
        << ": TCP server socket creation failed; continuing anyway.";
    return;
  }
  socket_->SignalNewConnection.connect(this, &TCPPort::OnNewConnection);
  socket_->SignalAddressReady.connect(this, &TCPPort::OnAddressReady);
}

rtc::AsyncPacketSocket* TCPPort::GetIncoming(const rtc::SocketAddress& addr,
                                             bool remove) {
  rtc::AsyncPacketSocket* socket = NULL;
  for (std::list<Incoming>::iterator it = incoming_.begin();
       it != incoming_.end(); ++it) {
    if (it->addr == addr) {
      socket = it->socket;
      if (remove)
        incoming_.erase(it);
      break;
    }
  }
  return socket;
}

void TCPPort::OnReadPacket(rtc::AsyncPacketSocket* socket,
                           const char* data,
                           size_t size,
                           const rtc::SocketAddress& remote_addr,
                           const int64_t& packet_time_us) {
  Port::OnReadPacket(data, size, remote_addr, PROTO_TCP);
}

void TCPPort::OnSentPacket(rtc::AsyncPacketSocket* socket,
                           const rtc::SentPacket& sent_packet) {
  PortInterface::SignalSentPacket(sent_packet);
}

void TCPPort::OnReadyToSend(rtc::AsyncPacketSocket* socket) {
  Port::OnReadyToSend();
}

void TCPPort::OnAddressReady(rtc::AsyncPacketSocket* socket,
                             const rtc::SocketAddress& address) {
  AddAddress(address, address, rtc::SocketAddress(), TCP_PROTOCOL_NAME, "",
             TCPTYPE_PASSIVE_STR, LOCAL_PORT_TYPE, ICE_TYPE_PREFERENCE_HOST_TCP,
             0, "", true);
}

// TODO(qingsi): |CONNECTION_WRITE_CONNECT_TIMEOUT| is overriden by
// |ice_unwritable_timeout| in IceConfig when determining the writability state.
// Replace this constant with the config parameter assuming the default value if
// we decide it is also applicable here.
TCPConnection::TCPConnection(TCPPort* port,
                             const Candidate& candidate,
                             rtc::AsyncPacketSocket* socket)
    : Connection(port, 0, candidate),
      socket_(socket),
      error_(0),
      outgoing_(socket == NULL),
      connection_pending_(false),
      pretending_to_be_writable_(false),
      reconnection_timeout_(cricket::CONNECTION_WRITE_CONNECT_TIMEOUT) {
  if (outgoing_) {
    CreateOutgoingTcpSocket();
  } else {
    // Incoming connections should match one of the network addresses. Same as
    // what's being checked in OnConnect, but just DCHECKing here.
    RTC_LOG(LS_VERBOSE) << ToString() << ": socket ipaddr: "
                        << socket_->GetLocalAddress().ToSensitiveString()
                        << ", port() Network:" << port->Network()->ToString();
    RTC_DCHECK(absl::c_any_of(
        port_->Network()->GetIPs(), [this](const rtc::InterfaceAddress& addr) {
          return socket_->GetLocalAddress().ipaddr() == addr;
        }));
    ConnectSocketSignals(socket);
  }
}

TCPConnection::~TCPConnection() {}

int TCPConnection::Send(const void* data,
                        size_t size,
                        const rtc::PacketOptions& options) {
  if (!socket_) {
    error_ = ENOTCONN;
    return SOCKET_ERROR;
  }

  // Sending after OnClose on active side will trigger a reconnect for a
  // outgoing connection. Note that the write state is still WRITABLE as we want
  // to spend a few seconds attempting a reconnect before saying we're
  // unwritable.
  if (!connected()) {
    MaybeReconnect();
    return SOCKET_ERROR;
  }

  // Note that this is important to put this after the previous check to give
  // the connection a chance to reconnect.
  if (pretending_to_be_writable_ || write_state() != STATE_WRITABLE) {
    // TODO(?): Should STATE_WRITE_TIMEOUT return a non-blocking error?
    error_ = ENOTCONN;
    return SOCKET_ERROR;
  }
  stats_.sent_total_packets++;
  rtc::PacketOptions modified_options(options);
  static_cast<TCPPort*>(port_)->CopyPortInformationToPacketInfo(
      &modified_options.info_signaled_after_sent);
  int sent = socket_->Send(data, size, modified_options);
  int64_t now = rtc::TimeMillis();
  if (sent < 0) {
    stats_.sent_discarded_packets++;
    error_ = socket_->GetError();
  } else {
    send_rate_tracker_.AddSamplesAtTime(now, sent);
  }
  last_send_data_ = now;
  return sent;
}

int TCPConnection::GetError() {
  return error_;
}

void TCPConnection::OnConnectionRequestResponse(ConnectionRequest* req,
                                                StunMessage* response) {
  // Process the STUN response before we inform upper layer ready to send.
  Connection::OnConnectionRequestResponse(req, response);

  // If we're in the state of pretending to be writeable, we should inform the
  // upper layer it's ready to send again as previous EWOULDLBLOCK from socket
  // would have stopped the outgoing stream.
  if (pretending_to_be_writable_) {
    Connection::OnReadyToSend();
  }
  pretending_to_be_writable_ = false;
  RTC_DCHECK(write_state() == STATE_WRITABLE);
}

void TCPConnection::OnConnect(rtc::AsyncPacketSocket* socket) {
  RTC_DCHECK(socket == socket_.get());
  // Do not use this port if the socket bound to an address not associated with
  // the desired network interface. This is seen in Chrome, where TCP sockets
  // cannot be given a binding address, and the platform is expected to pick
  // the correct local address.
  //
  // However, there are two situations in which we allow the bound address to
  // not be one of the addresses of the requested interface:
  // 1. The bound address is the loopback address. This happens when a proxy
  // forces TCP to bind to only the localhost address (see issue 3927).
  // 2. The bound address is the "any address". This happens when
  // multiple_routes is disabled (see issue 4780).
  //
  // Note that, aside from minor differences in log statements, this logic is
  // identical to that in TurnPort.
  const rtc::SocketAddress& socket_address = socket->GetLocalAddress();
  if (absl::c_any_of(port_->Network()->GetIPs(),
                     [socket_address](const rtc::InterfaceAddress& addr) {
                       return socket_address.ipaddr() == addr;
                     })) {
    RTC_LOG(LS_VERBOSE) << ToString() << ": Connection established to "
                        << socket->GetRemoteAddress().ToSensitiveString();
  } else {
    if (socket->GetLocalAddress().IsLoopbackIP()) {
      RTC_LOG(LS_WARNING) << "Socket is bound to the address:"
                          << socket_address.ipaddr().ToSensitiveString()
                          << ", rather than an address associated with network:"
                          << port_->Network()->ToString()
                          << ". Still allowing it since it's localhost.";
    } else if (IPIsAny(port_->Network()->GetBestIP())) {
      RTC_LOG(LS_WARNING)
          << "Socket is bound to the address:"
          << socket_address.ipaddr().ToSensitiveString()
          << ", rather than an address associated with network:"
          << port_->Network()->ToString()
          << ". Still allowing it since it's the 'any' address"
             ", possibly caused by multiple_routes being disabled.";
    } else {
      RTC_LOG(LS_WARNING) << "Dropping connection as TCP socket bound to IP "
                          << socket_address.ipaddr().ToSensitiveString()
                          << ", rather than an address associated with network:"
                          << port_->Network()->ToString();
      OnClose(socket, 0);
      return;
    }
  }

  // Connection is established successfully.
  set_connected(true);
  connection_pending_ = false;
}

void TCPConnection::OnClose(rtc::AsyncPacketSocket* socket, int error) {
  RTC_DCHECK(socket == socket_.get());
  RTC_LOG(LS_INFO) << ToString() << ": Connection closed with error " << error;

  // Guard against the condition where IPC socket will call OnClose for every
  // packet it can't send.
  if (connected()) {
    set_connected(false);

    // Prevent the connection from being destroyed by redundant SignalClose
    // events.
    pretending_to_be_writable_ = true;

    // We don't attempt reconnect right here. This is to avoid a case where the
    // shutdown is intentional and reconnect is not necessary. We only reconnect
    // when the connection is used to Send() or Ping().
    port()->thread()->PostDelayed(RTC_FROM_HERE, reconnection_timeout(), this,
                                  MSG_TCPCONNECTION_DELAYED_ONCLOSE);
  } else if (!pretending_to_be_writable_) {
    // OnClose could be called when the underneath socket times out during the
    // initial connect() (i.e. |pretending_to_be_writable_| is false) . We have
    // to manually destroy here as this connection, as never connected, will not
    // be scheduled for ping to trigger destroy.
    Destroy();
  }
}

void TCPConnection::OnMessage(rtc::Message* pmsg) {
  switch (pmsg->message_id) {
    case MSG_TCPCONNECTION_DELAYED_ONCLOSE:
      // If this connection can't become connected and writable again in 5
      // seconds, it's time to tear this down. This is the case for the original
      // TCP connection on passive side during a reconnect.
      if (pretending_to_be_writable_) {
        Destroy();
      }
      break;
    case MSG_TCPCONNECTION_FAILED_CREATE_SOCKET:
      FailAndPrune();
      break;
    default:
      Connection::OnMessage(pmsg);
  }
}

void TCPConnection::MaybeReconnect() {
  // Only reconnect for an outgoing TCPConnection when OnClose was signaled and
  // no outstanding reconnect is pending.
  if (connected() || connection_pending_ || !outgoing_) {
    return;
  }

  RTC_LOG(LS_INFO) << ToString()
                   << ": TCP Connection with remote is closed, "
                      "trying to reconnect";

  CreateOutgoingTcpSocket();
  error_ = EPIPE;
}

void TCPConnection::OnReadPacket(rtc::AsyncPacketSocket* socket,
                                 const char* data,
                                 size_t size,
                                 const rtc::SocketAddress& remote_addr,
                                 const int64_t& packet_time_us) {
  RTC_DCHECK(socket == socket_.get());
  Connection::OnReadPacket(data, size, packet_time_us);
}

void TCPConnection::OnReadyToSend(rtc::AsyncPacketSocket* socket) {
  RTC_DCHECK(socket == socket_.get());
  Connection::OnReadyToSend();
}

void TCPConnection::CreateOutgoingTcpSocket() {
  RTC_DCHECK(outgoing_);
  int opts = (remote_candidate().protocol() == SSLTCP_PROTOCOL_NAME)
                 ? rtc::PacketSocketFactory::OPT_TLS_FAKE
                 : 0;
  rtc::PacketSocketTcpOptions tcp_opts;
  tcp_opts.opts = opts;
  socket_.reset(port()->socket_factory()->CreateClientTcpSocket(
      rtc::SocketAddress(port()->Network()->GetBestIP(), 0),
      remote_candidate().address(), port()->proxy(), port()->user_agent(),
      tcp_opts));
  if (socket_) {
    RTC_LOG(LS_VERBOSE) << ToString() << ": Connecting from "
                        << socket_->GetLocalAddress().ToSensitiveString()
                        << " to "
                        << remote_candidate().address().ToSensitiveString();
    set_connected(false);
    connection_pending_ = true;
    ConnectSocketSignals(socket_.get());
  } else {
    RTC_LOG(LS_WARNING) << ToString() << ": Failed to create connection to "
                        << remote_candidate().address().ToSensitiveString();
    // We can't FailAndPrune directly here. FailAndPrune and deletes all
    // the StunRequests from the request_map_. And if this is in the stack
    // of Connection::Ping(), we are still using the request.
    // Unwind the stack and defer the FailAndPrune.
    set_state(IceCandidatePairState::FAILED);
    port()->thread()->Post(RTC_FROM_HERE, this,
                           MSG_TCPCONNECTION_FAILED_CREATE_SOCKET);
  }
}

void TCPConnection::ConnectSocketSignals(rtc::AsyncPacketSocket* socket) {
  if (outgoing_) {
    socket->SignalConnect.connect(this, &TCPConnection::OnConnect);
  }
  socket->SignalReadPacket.connect(this, &TCPConnection::OnReadPacket);
  socket->SignalReadyToSend.connect(this, &TCPConnection::OnReadyToSend);
  socket->SignalClose.connect(this, &TCPConnection::OnClose);
}

}  // namespace cricket
