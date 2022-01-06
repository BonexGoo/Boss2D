/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "examples/turnserver/read_auth_file.h"
#include BOSS_WEBRTC_U_p2p__base__basic_packet_socket_factory_h //original-code:"p2p/base/basic_packet_socket_factory.h"
#include BOSS_WEBRTC_U_p2p__base__port_interface_h //original-code:"p2p/base/port_interface.h"
#include BOSS_WEBRTC_U_p2p__base__turn_server_h //original-code:"p2p/base/turn_server.h"
#include BOSS_WEBRTC_U_rtc_base__async_udp_socket_h //original-code:"rtc_base/async_udp_socket.h"
#include BOSS_WEBRTC_U_rtc_base__ip_address_h //original-code:"rtc_base/ip_address.h"
#include BOSS_WEBRTC_U_rtc_base__socket_address_h //original-code:"rtc_base/socket_address.h"
#include BOSS_WEBRTC_U_rtc_base__socket_server_h //original-code:"rtc_base/socket_server.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"

namespace {
const char kSoftware[] = "libjingle TurnServer";

class TurnFileAuth : public cricket::TurnAuthInterface {
 public:
  explicit TurnFileAuth(std::map<std::string, std::string> name_to_key)
      : name_to_key_(std::move(name_to_key)) {}

  virtual bool GetKey(const std::string& username,
                      const std::string& realm,
                      std::string* key) {
    // File is stored as lines of <username>=<HA1>.
    // Generate HA1 via "echo -n "<username>:<realm>:<password>" | md5sum"
    auto it = name_to_key_.find(username);
    if (it == name_to_key_.end())
      return false;
    *key = it->second;
    return true;
  }

 private:
  const std::map<std::string, std::string> name_to_key_;
};

}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 5) {
    std::cerr << "usage: turnserver int-addr ext-ip realm auth-file"
              << std::endl;
    return 1;
  }

  rtc::SocketAddress int_addr;
  if (!int_addr.FromString(argv[1])) {
    std::cerr << "Unable to parse IP address: " << argv[1] << std::endl;
    return 1;
  }

  rtc::IPAddress ext_addr;
  if (!IPFromString(argv[2], &ext_addr)) {
    std::cerr << "Unable to parse IP address: " << argv[2] << std::endl;
    return 1;
  }

  rtc::Thread* main = rtc::Thread::Current();
  rtc::AsyncUDPSocket* int_socket =
      rtc::AsyncUDPSocket::Create(main->socketserver(), int_addr);
  if (!int_socket) {
    std::cerr << "Failed to create a UDP socket bound at" << int_addr.ToString()
              << std::endl;
    return 1;
  }

  cricket::TurnServer server(main);
  std::fstream auth_file(argv[4], std::fstream::in);

  TurnFileAuth auth(auth_file.is_open()
                        ? webrtc_examples::ReadAuthFile(&auth_file)
                        : std::map<std::string, std::string>());
  server.set_realm(argv[3]);
  server.set_software(kSoftware);
  server.set_auth_hook(&auth);
  server.AddInternalSocket(int_socket, cricket::PROTO_UDP);
  server.SetExternalSocketFactory(new rtc::BasicPacketSocketFactory(),
                                  rtc::SocketAddress(ext_addr, 0));

  std::cout << "Listening internally at " << int_addr.ToString() << std::endl;

  main->Run();
  return 0;
}
