/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_receiver_h //original-code:"modules/rtp_rtcp/source/rtcp_receiver.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {
namespace {

class NullModuleRtpRtcp : public RTCPReceiver::ModuleRtpRtcp {
 public:
  void SetTmmbn(std::vector<rtcp::TmmbItem>) override {}
  void OnRequestSendReport() override {}
  void OnReceivedNack(const std::vector<uint16_t>&) override{};
  void OnReceivedRtcpReportBlocks(const ReportBlockList&) override{};
};

}  // namespace

void FuzzOneInput(const uint8_t* data, size_t size) {
  NullModuleRtpRtcp rtp_rtcp_module;
  SimulatedClock clock(1234);

  RTCPReceiver receiver(&clock, false, nullptr, nullptr, nullptr, nullptr,
                        nullptr, &rtp_rtcp_module);

  receiver.IncomingPacket(data, size);
}
}  // namespace webrtc