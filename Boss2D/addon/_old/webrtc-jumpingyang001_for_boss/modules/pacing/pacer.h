/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_PACING_PACER_H_
#define MODULES_PACING_PACER_H_

#include BOSS_WEBRTC_U_modules__include__module_h //original-code:"modules/include/module.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"

namespace webrtc {
class Pacer : public Module, public RtpPacketSender {
 public:
  virtual void SetEstimatedBitrate(uint32_t bitrate_bps) {}
  virtual void SetPacingRates(uint32_t pacing_rate_bps,
                              uint32_t padding_rate_bps) {}
  virtual void SetEstimatedBitrateAndCongestionWindow(
      uint32_t bitrate_bps,
      bool in_probe_rtt,
      uint64_t congestion_window) {}
  virtual void OnBytesAcked(size_t bytes) {}
  void InsertPacket(RtpPacketSender::Priority priority,
                    uint32_t ssrc,
                    uint16_t sequence_number,
                    int64_t capture_time_ms,
                    size_t bytes,
                    bool retransmission) override = 0;
  int64_t TimeUntilNextProcess() override = 0;
  void Process() override = 0;
  ~Pacer() override {}
};
}  // namespace webrtc
#endif  // MODULES_PACING_PACER_H_
