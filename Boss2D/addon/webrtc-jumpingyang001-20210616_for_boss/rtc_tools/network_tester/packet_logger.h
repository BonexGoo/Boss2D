/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_TOOLS_NETWORK_TESTER_PACKET_LOGGER_H_
#define RTC_TOOLS_NETWORK_TESTER_PACKET_LOGGER_H_

#include <fstream>
#include <string>

#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__ignore_wundef_h //original-code:"rtc_base/ignore_wundef.h"

#ifdef WEBRTC_NETWORK_TESTER_PROTO
RTC_PUSH_IGNORING_WUNDEF()
#include "rtc_tools/network_tester/network_tester_packet.pb.h"
RTC_POP_IGNORING_WUNDEF()
using webrtc::network_tester::packet::NetworkTesterPacket;
#else
class NetworkTesterPacket;
#endif  // WEBRTC_NETWORK_TESTER_PROTO

namespace webrtc {

class PacketLogger {
 public:
  explicit PacketLogger(const std::string& log_file_path);
  ~PacketLogger();

  void LogPacket(const NetworkTesterPacket& packet);

 private:
  std::ofstream packet_logger_stream_;

  RTC_DISALLOW_COPY_AND_ASSIGN(PacketLogger);
};

}  // namespace webrtc

#endif  // RTC_TOOLS_NETWORK_TESTER_PACKET_LOGGER_H_
