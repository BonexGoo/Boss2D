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

#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__socket_address_h //original-code:"rtc_base/socket_address.h"

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
#undef EMSGSIZE
#define EMSGSIZE WSAEMSGSIZE
#undef EADDRINUSE
#define EADDRINUSE WSAEADDRINUSE
#undef EADDRNOTAVAIL
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#undef ENETDOWN
#define ENETDOWN WSAENETDOWN
#undef ECONNABORTED
#define ECONNABORTED WSAECONNABORTED
#undef ENOBUFS
#define ENOBUFS WSAENOBUFS
#undef EISCONN
#define EISCONN WSAEISCONN
#undef ENOTCONN
#define ENOTCONN WSAENOTCONN
#undef ECONNREFUSED
#define ECONNREFUSED WSAECONNREFUSED
#undef EHOSTUNREACH
#define EHOSTUNREACH WSAEHOSTUNREACH
#undef ENETUNREACH
#define ENETUNREACH WSAENETUNREACH
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
