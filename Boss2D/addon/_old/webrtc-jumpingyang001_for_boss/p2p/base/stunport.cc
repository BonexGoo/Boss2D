/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_p2p__base__stunport_h //original-code:"p2p/base/stunport.h"

#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_p2p__base__p2pconstants_h //original-code:"p2p/base/p2pconstants.h"
#include BOSS_WEBRTC_U_p2p__base__portallocator_h //original-code:"p2p/base/portallocator.h"
#include BOSS_WEBRTC_U_p2p__base__stun_h //original-code:"p2p/base/stun.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__helpers_h //original-code:"rtc_base/helpers.h"
#include BOSS_WEBRTC_U_rtc_base__ipaddress_h //original-code:"rtc_base/ipaddress.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__nethelpers_h //original-code:"rtc_base/nethelpers.h"

namespace cricket {

// TODO(?): Move these to a common place (used in relayport too)
const int RETRY_TIMEOUT = 50 * 1000;  // 50 seconds

// Handles a binding request sent to the STUN server.
class StunBindingRequest : public StunRequest {
 public:
  StunBindingRequest(UDPPort* port,
                     const rtc::SocketAddress& addr,
                     int64_t start_time)
      : port_(port), server_addr_(addr), start_time_(start_time) {}

  const rtc::SocketAddress& server_addr() const { return server_addr_; }

  void Prepare(StunMessage* request) override {
    request->SetType(STUN_BINDING_REQUEST);
  }

  void OnResponse(StunMessage* response) override {
    const StunAddressAttribute* addr_attr =
        response->GetAddress(STUN_ATTR_MAPPED_ADDRESS);
    if (!addr_attr) {
      RTC_LOG(LS_ERROR) << "Binding response missing mapped address.";
    } else if (addr_attr->family() != STUN_ADDRESS_IPV4 &&
               addr_attr->family() != STUN_ADDRESS_IPV6) {
      RTC_LOG(LS_ERROR) << "Binding address has bad family";
    } else {
      rtc::SocketAddress addr(addr_attr->ipaddr(), addr_attr->port());
      port_->OnStunBindingRequestSucceeded(this->Elapsed(), server_addr_, addr);
    }

    // The keep-alive requests will be stopped after its lifetime has passed.
    if (WithinLifetime(rtc::TimeMillis())) {
      port_->requests_.SendDelayed(
          new StunBindingRequest(port_, server_addr_, start_time_),
          port_->stun_keepalive_delay());
    }
  }

  void OnErrorResponse(StunMessage* response) override {
    const StunErrorCodeAttribute* attr = response->GetErrorCode();
    if (!attr) {
      RTC_LOG(LS_ERROR) << "Missing binding response error code.";
    } else {
      RTC_LOG(LS_ERROR) << "Binding error response:"
                           " class="
                        << attr->eclass() << " number=" << attr->number()
                        << " reason=" << attr->reason();
    }

    port_->OnStunBindingOrResolveRequestFailed(server_addr_);

    int64_t now = rtc::TimeMillis();
    if (WithinLifetime(now) &&
        rtc::TimeDiff(now, start_time_) < RETRY_TIMEOUT) {
      port_->requests_.SendDelayed(
          new StunBindingRequest(port_, server_addr_, start_time_),
          port_->stun_keepalive_delay());
    }
  }
  void OnTimeout() override {
    RTC_LOG(LS_ERROR) << "Binding request timed out from "
                      << port_->GetLocalAddress().ToSensitiveString() << " ("
                      << port_->Network()->name() << ")";

    port_->OnStunBindingOrResolveRequestFailed(server_addr_);
  }

 private:
  // Returns true if |now| is within the lifetime of the request (a negative
  // lifetime means infinite).
  bool WithinLifetime(int64_t now) const {
    int lifetime = port_->stun_keepalive_lifetime();
    return lifetime < 0 || rtc::TimeDiff(now, start_time_) <= lifetime;
  }
  UDPPort* port_;
  const rtc::SocketAddress server_addr_;

  int64_t start_time_;
};

UDPPort::AddressResolver::AddressResolver(rtc::PacketSocketFactory* factory)
    : socket_factory_(factory) {}

UDPPort::AddressResolver::~AddressResolver() {
  for (ResolverMap::iterator it = resolvers_.begin(); it != resolvers_.end();
       ++it) {
    // TODO(guoweis): Change to asynchronous DNS resolution to prevent the hang
    // when passing true to the Destroy() which is a safer way to avoid the code
    // unloaded before the thread exits. Please see webrtc bug 5139.
    it->second->Destroy(false);
  }
}

void UDPPort::AddressResolver::Resolve(const rtc::SocketAddress& address) {
  if (resolvers_.find(address) != resolvers_.end())
    return;

  rtc::AsyncResolverInterface* resolver =
      socket_factory_->CreateAsyncResolver();
  resolvers_.insert(std::pair<rtc::SocketAddress, rtc::AsyncResolverInterface*>(
      address, resolver));

  resolver->SignalDone.connect(this,
                               &UDPPort::AddressResolver::OnResolveResult);

  resolver->Start(address);
}

bool UDPPort::AddressResolver::GetResolvedAddress(
    const rtc::SocketAddress& input,
    int family,
    rtc::SocketAddress* output) const {
  ResolverMap::const_iterator it = resolvers_.find(input);
  if (it == resolvers_.end())
    return false;

  return it->second->GetResolvedAddress(family, output);
}

void UDPPort::AddressResolver::OnResolveResult(
    rtc::AsyncResolverInterface* resolver) {
  for (ResolverMap::iterator it = resolvers_.begin(); it != resolvers_.end();
       ++it) {
    if (it->second == resolver) {
      SignalDone(it->first, resolver->GetError());
      return;
    }
  }
}

UDPPort::UDPPort(rtc::Thread* thread,
                 rtc::PacketSocketFactory* factory,
                 rtc::Network* network,
                 rtc::AsyncPacketSocket* socket,
                 const std::string& username,
                 const std::string& password,
                 const std::string& origin,
                 bool emit_local_for_anyaddress)
    : Port(thread, LOCAL_PORT_TYPE, factory, network, username, password),
      requests_(thread),
      socket_(socket),
      error_(0),
      ready_(false),
      stun_keepalive_delay_(STUN_KEEPALIVE_INTERVAL),
      emit_local_for_anyaddress_(emit_local_for_anyaddress) {
  requests_.set_origin(origin);
}

UDPPort::UDPPort(rtc::Thread* thread,
                 rtc::PacketSocketFactory* factory,
                 rtc::Network* network,
                 uint16_t min_port,
                 uint16_t max_port,
                 const std::string& username,
                 const std::string& password,
                 const std::string& origin,
                 bool emit_local_for_anyaddress)
    : Port(thread,
           LOCAL_PORT_TYPE,
           factory,
           network,
           min_port,
           max_port,
           username,
           password),
      requests_(thread),
      socket_(NULL),
      error_(0),
      ready_(false),
      stun_keepalive_delay_(STUN_KEEPALIVE_INTERVAL),
      emit_local_for_anyaddress_(emit_local_for_anyaddress) {
  requests_.set_origin(origin);
}

bool UDPPort::Init() {
  stun_keepalive_lifetime_ = GetStunKeepaliveLifetime();
  if (!SharedSocket()) {
    RTC_DCHECK(socket_ == NULL);
    socket_ = socket_factory()->CreateUdpSocket(
        rtc::SocketAddress(Network()->GetBestIP(), 0), min_port(), max_port());
    if (!socket_) {
      RTC_LOG(LS_WARNING) << ToString() << ": UDP socket creation failed";
      return false;
    }
    socket_->SignalReadPacket.connect(this, &UDPPort::OnReadPacket);
  }
  socket_->SignalSentPacket.connect(this, &UDPPort::OnSentPacket);
  socket_->SignalReadyToSend.connect(this, &UDPPort::OnReadyToSend);
  socket_->SignalAddressReady.connect(this, &UDPPort::OnLocalAddressReady);
  requests_.SignalSendPacket.connect(this, &UDPPort::OnSendPacket);
  return true;
}

UDPPort::~UDPPort() {
  if (!SharedSocket())
    delete socket_;
}

void UDPPort::PrepareAddress() {
  RTC_DCHECK(requests_.empty());
  if (socket_->GetState() == rtc::AsyncPacketSocket::STATE_BOUND) {
    OnLocalAddressReady(socket_, socket_->GetLocalAddress());
  }
}

void UDPPort::MaybePrepareStunCandidate() {
  // Sending binding request to the STUN server if address is available to
  // prepare STUN candidate.
  if (!server_addresses_.empty()) {
    SendStunBindingRequests();
  } else {
    // Port is done allocating candidates.
    MaybeSetPortCompleteOrError();
  }
}

Connection* UDPPort::CreateConnection(const Candidate& address,
                                      CandidateOrigin origin) {
  if (!SupportsProtocol(address.protocol())) {
    return NULL;
  }

  if (!IsCompatibleAddress(address.address())) {
    return NULL;
  }

  if (SharedSocket() && Candidates()[0].type() != LOCAL_PORT_TYPE) {
    RTC_NOTREACHED();
    return NULL;
  }

  Connection* conn = new ProxyConnection(this, 0, address);
  AddOrReplaceConnection(conn);
  return conn;
}

int UDPPort::SendTo(const void* data,
                    size_t size,
                    const rtc::SocketAddress& addr,
                    const rtc::PacketOptions& options,
                    bool payload) {
  rtc::PacketOptions modified_options(options);
  CopyPortInformationToPacketInfo(&modified_options.info_signaled_after_sent);
  int sent = socket_->SendTo(data, size, addr, modified_options);
  if (sent < 0) {
    error_ = socket_->GetError();
    RTC_LOG(LS_ERROR) << ToString() << ": UDP send of " << size
                      << " bytes failed with error " << error_;
  }
  return sent;
}

void UDPPort::UpdateNetworkCost() {
  Port::UpdateNetworkCost();
  stun_keepalive_lifetime_ = GetStunKeepaliveLifetime();
}

int UDPPort::SetOption(rtc::Socket::Option opt, int value) {
  return socket_->SetOption(opt, value);
}

int UDPPort::GetOption(rtc::Socket::Option opt, int* value) {
  return socket_->GetOption(opt, value);
}

int UDPPort::GetError() {
  return error_;
}

bool UDPPort::HandleIncomingPacket(rtc::AsyncPacketSocket* socket,
                                   const char* data,
                                   size_t size,
                                   const rtc::SocketAddress& remote_addr,
                                   const rtc::PacketTime& packet_time) {
  // All packets given to UDP port will be consumed.
  OnReadPacket(socket, data, size, remote_addr, packet_time);
  return true;
}

bool UDPPort::SupportsProtocol(const std::string& protocol) const {
  return protocol == UDP_PROTOCOL_NAME;
}

ProtocolType UDPPort::GetProtocol() const {
  return PROTO_UDP;
}

void UDPPort::GetStunStats(absl::optional<StunStats>* stats) {
  *stats = stats_;
}

void UDPPort::set_stun_keepalive_delay(const absl::optional<int>& delay) {
  stun_keepalive_delay_ = delay.value_or(STUN_KEEPALIVE_INTERVAL);
}

void UDPPort::OnLocalAddressReady(rtc::AsyncPacketSocket* socket,
                                  const rtc::SocketAddress& address) {
  // When adapter enumeration is disabled and binding to the any address, the
  // default local address will be issued as a candidate instead if
  // |emit_local_for_anyaddress| is true. This is to allow connectivity for
  // applications which absolutely requires a HOST candidate.
  rtc::SocketAddress addr = address;

  // If MaybeSetDefaultLocalAddress fails, we keep the "any" IP so that at
  // least the port is listening.
  MaybeSetDefaultLocalAddress(&addr);

  AddAddress(addr, addr, rtc::SocketAddress(), UDP_PROTOCOL_NAME, "", "",
             LOCAL_PORT_TYPE, ICE_TYPE_PREFERENCE_HOST, 0, "", false);
  MaybePrepareStunCandidate();
}

void UDPPort::OnReadPacket(rtc::AsyncPacketSocket* socket,
                           const char* data,
                           size_t size,
                           const rtc::SocketAddress& remote_addr,
                           const rtc::PacketTime& packet_time) {
  RTC_DCHECK(socket == socket_);
  RTC_DCHECK(!remote_addr.IsUnresolvedIP());

  // Look for a response from the STUN server.
  // Even if the response doesn't match one of our outstanding requests, we
  // will eat it because it might be a response to a retransmitted packet, and
  // we already cleared the request when we got the first response.
  if (server_addresses_.find(remote_addr) != server_addresses_.end()) {
    requests_.CheckResponse(data, size);
    return;
  }

  if (Connection* conn = GetConnection(remote_addr)) {
    conn->OnReadPacket(data, size, packet_time);
  } else {
    Port::OnReadPacket(data, size, remote_addr, PROTO_UDP);
  }
}

void UDPPort::OnSentPacket(rtc::AsyncPacketSocket* socket,
                           const rtc::SentPacket& sent_packet) {
  PortInterface::SignalSentPacket(sent_packet);
}

void UDPPort::OnReadyToSend(rtc::AsyncPacketSocket* socket) {
  Port::OnReadyToSend();
}

void UDPPort::SendStunBindingRequests() {
  // We will keep pinging the stun server to make sure our NAT pin-hole stays
  // open until the deadline (specified in SendStunBindingRequest).
  RTC_DCHECK(requests_.empty());

  for (ServerAddresses::const_iterator it = server_addresses_.begin();
       it != server_addresses_.end(); ++it) {
    SendStunBindingRequest(*it);
  }
}

void UDPPort::ResolveStunAddress(const rtc::SocketAddress& stun_addr) {
  if (!resolver_) {
    resolver_.reset(new AddressResolver(socket_factory()));
    resolver_->SignalDone.connect(this, &UDPPort::OnResolveResult);
  }

  RTC_LOG(LS_INFO) << ToString() << ": Starting STUN host lookup for "
                   << stun_addr.ToSensitiveString();
  resolver_->Resolve(stun_addr);
}

void UDPPort::OnResolveResult(const rtc::SocketAddress& input, int error) {
  RTC_DCHECK(resolver_.get() != NULL);

  rtc::SocketAddress resolved;
  if (error != 0 || !resolver_->GetResolvedAddress(
                        input, Network()->GetBestIP().family(), &resolved)) {
    RTC_LOG(LS_WARNING) << ToString()
                        << ": StunPort: stun host lookup received error "
                        << error;
    OnStunBindingOrResolveRequestFailed(input);
    return;
  }

  server_addresses_.erase(input);

  if (server_addresses_.find(resolved) == server_addresses_.end()) {
    server_addresses_.insert(resolved);
    SendStunBindingRequest(resolved);
  }
}

void UDPPort::SendStunBindingRequest(const rtc::SocketAddress& stun_addr) {
  if (stun_addr.IsUnresolvedIP()) {
    ResolveStunAddress(stun_addr);

  } else if (socket_->GetState() == rtc::AsyncPacketSocket::STATE_BOUND) {
    // Check if |server_addr_| is compatible with the port's ip.
    if (IsCompatibleAddress(stun_addr)) {
      requests_.Send(
          new StunBindingRequest(this, stun_addr, rtc::TimeMillis()));
    } else {
      // Since we can't send stun messages to the server, we should mark this
      // port ready.
      RTC_LOG(LS_WARNING) << "STUN server address is incompatible.";
      OnStunBindingOrResolveRequestFailed(stun_addr);
    }
  }
}

bool UDPPort::MaybeSetDefaultLocalAddress(rtc::SocketAddress* addr) const {
  if (!addr->IsAnyIP() || !emit_local_for_anyaddress_ ||
      !Network()->default_local_address_provider()) {
    return true;
  }
  rtc::IPAddress default_address;
  bool result =
      Network()->default_local_address_provider()->GetDefaultLocalAddress(
          addr->family(), &default_address);
  if (!result || default_address.IsNil()) {
    return false;
  }

  addr->SetIP(default_address);
  return true;
}

void UDPPort::OnStunBindingRequestSucceeded(
    int rtt_ms,
    const rtc::SocketAddress& stun_server_addr,
    const rtc::SocketAddress& stun_reflected_addr) {
  RTC_DCHECK(stats_.stun_binding_responses_received <
             stats_.stun_binding_requests_sent);
  stats_.stun_binding_responses_received++;
  stats_.stun_binding_rtt_ms_total += rtt_ms;
  stats_.stun_binding_rtt_ms_squared_total += rtt_ms * rtt_ms;
  if (bind_request_succeeded_servers_.find(stun_server_addr) !=
      bind_request_succeeded_servers_.end()) {
    return;
  }
  bind_request_succeeded_servers_.insert(stun_server_addr);
  // If socket is shared and |stun_reflected_addr| is equal to local socket
  // address, or if the same address has been added by another STUN server,
  // then discarding the stun address.
  // For STUN, related address is the local socket address.
  if ((!SharedSocket() || stun_reflected_addr != socket_->GetLocalAddress()) &&
      !HasCandidateWithAddress(stun_reflected_addr)) {
    rtc::SocketAddress related_address = socket_->GetLocalAddress();
    // If we can't stamp the related address correctly, empty it to avoid leak.
    if (!MaybeSetDefaultLocalAddress(&related_address)) {
      related_address =
          rtc::EmptySocketAddressWithFamily(related_address.family());
    }

    std::ostringstream url;
    url << "stun:" << stun_server_addr.ipaddr().ToString() << ":"
        << stun_server_addr.port();
    AddAddress(stun_reflected_addr, socket_->GetLocalAddress(), related_address,
               UDP_PROTOCOL_NAME, "", "", STUN_PORT_TYPE,
               ICE_TYPE_PREFERENCE_SRFLX, 0, url.str(), false);
  }
  MaybeSetPortCompleteOrError();
}

void UDPPort::OnStunBindingOrResolveRequestFailed(
    const rtc::SocketAddress& stun_server_addr) {
  if (bind_request_failed_servers_.find(stun_server_addr) !=
      bind_request_failed_servers_.end()) {
    return;
  }
  bind_request_failed_servers_.insert(stun_server_addr);
  MaybeSetPortCompleteOrError();
}

void UDPPort::MaybeSetPortCompleteOrError() {
  if (ready_)
    return;

  // Do not set port ready if we are still waiting for bind responses.
  const size_t servers_done_bind_request =
      bind_request_failed_servers_.size() +
      bind_request_succeeded_servers_.size();
  if (server_addresses_.size() != servers_done_bind_request) {
    return;
  }

  // Setting ready status.
  ready_ = true;

  // The port is "completed" if there is no stun server provided, or the bind
  // request succeeded for any stun server, or the socket is shared.
  if (server_addresses_.empty() || bind_request_succeeded_servers_.size() > 0 ||
      SharedSocket()) {
    SignalPortComplete(this);
  } else {
    SignalPortError(this);
  }
}

// TODO(?): merge this with SendTo above.
void UDPPort::OnSendPacket(const void* data, size_t size, StunRequest* req) {
  StunBindingRequest* sreq = static_cast<StunBindingRequest*>(req);
  rtc::PacketOptions options(DefaultDscpValue());
  options.info_signaled_after_sent.packet_type = rtc::PacketType::kStunMessage;
  CopyPortInformationToPacketInfo(&options.info_signaled_after_sent);
  if (socket_->SendTo(data, size, sreq->server_addr(), options) < 0) {
    RTC_LOG_ERR_EX(LERROR, socket_->GetError()) << "sendto";
  }
  stats_.stun_binding_requests_sent++;
}

bool UDPPort::HasCandidateWithAddress(const rtc::SocketAddress& addr) const {
  const std::vector<Candidate>& existing_candidates = Candidates();
  std::vector<Candidate>::const_iterator it = existing_candidates.begin();
  for (; it != existing_candidates.end(); ++it) {
    if (it->address() == addr)
      return true;
  }
  return false;
}

StunPort* StunPort::Create(rtc::Thread* thread,
                           rtc::PacketSocketFactory* factory,
                           rtc::Network* network,
                           uint16_t min_port,
                           uint16_t max_port,
                           const std::string& username,
                           const std::string& password,
                           const ServerAddresses& servers,
                           const std::string& origin,
                           absl::optional<int> stun_keepalive_interval) {
  StunPort* port = new StunPort(thread, factory, network, min_port, max_port,
                                username, password, servers, origin);
  port->set_stun_keepalive_delay(stun_keepalive_interval);
  if (!port->Init()) {
    delete port;
    port = NULL;
  }
  return port;
}

StunPort::StunPort(rtc::Thread* thread,
                   rtc::PacketSocketFactory* factory,
                   rtc::Network* network,
                   uint16_t min_port,
                   uint16_t max_port,
                   const std::string& username,
                   const std::string& password,
                   const ServerAddresses& servers,
                   const std::string& origin)
    : UDPPort(thread,
              factory,
              network,
              min_port,
              max_port,
              username,
              password,
              origin,
              false) {
  // UDPPort will set these to local udp, updating these to STUN.
  set_type(STUN_PORT_TYPE);
  set_server_addresses(servers);
}

void StunPort::PrepareAddress() {
  SendStunBindingRequests();
}

}  // namespace cricket
