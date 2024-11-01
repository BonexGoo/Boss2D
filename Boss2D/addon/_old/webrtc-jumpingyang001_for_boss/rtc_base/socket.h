/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_SOCKET_H_
#define RTC_BASE_SOCKET_H_

#include <errno.h>

#if defined(WEBRTC_POSIX)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#define SOCKET_EACCES EACCES
#endif

#if defined(WEBRTC_WIN)
#include BOSS_WEBRTC_U_rtc_base__win32_h //original-code:"rtc_base/win32.h"
#endif

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"
#include BOSS_WEBRTC_U_rtc_base__socketaddress_h //original-code:"rtc_base/socketaddress.h"

// Rather than converting errors into a private namespace,
// Reuse the POSIX socket api errors. Note this depends on
// Win32 compatibility.

#if defined(WEBRTC_WIN)
#undef EWOULDBLOCK  // Remove errno.h's definition for each macro below.
#define EWOULDBLOCK WSAEWOULDBLOCK
#undef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#undef EALREADY
#define EALREADY WSAEALREADY
#undef ENOTSOCK
#define ENOTSOCK WSAENOTSOCK
#undef EDESTADDRREQ
#define EDESTADDRREQ WSAEDESTADDRREQ
#undef EMSGSIZE
#define EMSGSIZE WSAEMSGSIZE
#undef EPROTOTYPE
#define EPROTOTYPE WSAEPROTOTYPE
#undef ENOPROTOOPT
#define ENOPROTOOPT WSAENOPROTOOPT
#undef EPROTONOSUPPORT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#undef ESOCKTNOSUPPORT
#define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#undef EOPNOTSUPP
#define EOPNOTSUPP WSAEOPNOTSUPP
#undef EPFNOSUPPORT
#define EPFNOSUPPORT WSAEPFNOSUPPORT
#undef EAFNOSUPPORT
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#undef EADDRINUSE
#define EADDRINUSE WSAEADDRINUSE
#undef EADDRNOTAVAIL
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#undef ENETDOWN
#define ENETDOWN WSAENETDOWN
#undef ENETUNREACH
#define ENETUNREACH WSAENETUNREACH
#undef ENETRESET
#define ENETRESET WSAENETRESET
#undef ECONNABORTED
#define ECONNABORTED WSAECONNABORTED
#undef ECONNRESET
#define ECONNRESET WSAECONNRESET
#undef ENOBUFS
#define ENOBUFS WSAENOBUFS
#undef EISCONN
#define EISCONN WSAEISCONN
#undef ENOTCONN
#define ENOTCONN WSAENOTCONN
#undef ESHUTDOWN
#define ESHUTDOWN WSAESHUTDOWN
#undef ETOOMANYREFS
#define ETOOMANYREFS WSAETOOMANYREFS
#undef ETIMEDOUT
#define ETIMEDOUT WSAETIMEDOUT
#undef ECONNREFUSED
#define ECONNREFUSED WSAECONNREFUSED
#undef ELOOP
#define ELOOP WSAELOOP
#undef ENAMETOOLONG
#define ENAMETOOLONG WSAENAMETOOLONG
#undef EHOSTDOWN
#define EHOSTDOWN WSAEHOSTDOWN
#undef EHOSTUNREACH
#define EHOSTUNREACH WSAEHOSTUNREACH
#undef ENOTEMPTY
#define ENOTEMPTY WSAENOTEMPTY
#undef EPROCLIM
#define EPROCLIM WSAEPROCLIM
#undef EUSERS
#define EUSERS WSAEUSERS
#undef EDQUOT
#define EDQUOT WSAEDQUOT
#undef ESTALE
#define ESTALE WSAESTALE
#undef EREMOTE
#define EREMOTE WSAEREMOTE
#define SOCKET_EACCES WSAEACCES
#endif  // WEBRTC_WIN

#if defined(WEBRTC_POSIX)
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define closesocket(s) close(s)
#endif  // WEBRTC_POSIX

namespace rtc {

inline bool IsBlockingError(int e) {
  return (e == EWOULDBLOCK) || (e == EAGAIN) || (e == EINPROGRESS);
}

enum class PacketType {
  kUnknown,
  kData,
  kIceConnectivityCheck,
  kIceConnectivityCheckResponse,
  kStunMessage,
  kTurnMessage,
};

enum class PacketInfoProtocolType {
  kUnknown,
  kUdp,
  kTcp,
  kSsltcp,
  kTls,
};

struct PacketInfo {
  PacketInfo();
  PacketInfo(const PacketInfo& info);
  ~PacketInfo();

  PacketType packet_type = PacketType::kUnknown;
  PacketInfoProtocolType protocol = PacketInfoProtocolType::kUnknown;
  // A unique id assigned by the network manager, and absl::nullopt if not set.
  absl::optional<uint16_t> network_id;
  size_t packet_size_bytes = 0;
  size_t turn_overhead_bytes = 0;
  SocketAddress local_socket_address;
  SocketAddress remote_socket_address;
};

struct SentPacket {
  SentPacket();
  SentPacket(int64_t packet_id, int64_t send_time_ms);
  SentPacket(int64_t packet_id,
             int64_t send_time_ms,
             const rtc::PacketInfo& info);

  int64_t packet_id = -1;
  int64_t send_time_ms = -1;
  rtc::PacketInfo info;
};

// General interface for the socket implementations of various networks.  The
// methods match those of normal UNIX sockets very closely.
class Socket {
 public:
  virtual ~Socket() {}

  // Returns the address to which the socket is bound.  If the socket is not
  // bound, then the any-address is returned.
  virtual SocketAddress GetLocalAddress() const = 0;

  // Returns the address to which the socket is connected.  If the socket is
  // not connected, then the any-address is returned.
  virtual SocketAddress GetRemoteAddress() const = 0;

  virtual int Bind(const SocketAddress& addr) = 0;
  virtual int Connect(const SocketAddress& addr) = 0;
  virtual int Send(const void* pv, size_t cb) = 0;
  virtual int SendTo(const void* pv, size_t cb, const SocketAddress& addr) = 0;
  // |timestamp| is in units of microseconds.
  virtual int Recv(void* pv, size_t cb, int64_t* timestamp) = 0;
  virtual int RecvFrom(void* pv,
                       size_t cb,
                       SocketAddress* paddr,
                       int64_t* timestamp) = 0;
  virtual int Listen(int backlog) = 0;
  virtual Socket* Accept(SocketAddress* paddr) = 0;
  virtual int Close() = 0;
  virtual int GetError() const = 0;
  virtual void SetError(int error) = 0;
  inline bool IsBlocking() const { return IsBlockingError(GetError()); }

  enum ConnState { CS_CLOSED, CS_CONNECTING, CS_CONNECTED };
  virtual ConnState GetState() const = 0;

  enum Option {
    OPT_DONTFRAGMENT,
    OPT_RCVBUF,                // receive buffer size
    OPT_SNDBUF,                // send buffer size
    OPT_NODELAY,               // whether Nagle algorithm is enabled
    OPT_IPV6_V6ONLY,           // Whether the socket is IPv6 only.
    OPT_DSCP,                  // DSCP code
    OPT_RTP_SENDTIME_EXTN_ID,  // This is a non-traditional socket option param.
                               // This is specific to libjingle and will be used
                               // if SendTime option is needed at socket level.
  };
  virtual int GetOption(Option opt, int* value) = 0;
  virtual int SetOption(Option opt, int value) = 0;

 protected:
  Socket() {}

 private:
  RTC_DISALLOW_COPY_AND_ASSIGN(Socket);
};

}  // namespace rtc

#endif  // RTC_BASE_SOCKET_H_
