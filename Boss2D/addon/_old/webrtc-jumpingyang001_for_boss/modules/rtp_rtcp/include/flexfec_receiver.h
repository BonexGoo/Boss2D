/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_INCLUDE_FLEXFEC_RECEIVER_H_
#define MODULES_RTP_RTCP_INCLUDE_FLEXFEC_RECEIVER_H_

#include <memory>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__ulpfec_receiver_h //original-code:"modules/rtp_rtcp/include/ulpfec_receiver.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__forward_error_correction_h //original-code:"modules/rtp_rtcp/source/forward_error_correction.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"
#include BOSS_WEBRTC_U_rtc_base__sequenced_task_checker_h //original-code:"rtc_base/sequenced_task_checker.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

class FlexfecReceiver {
 public:
  FlexfecReceiver(uint32_t ssrc,
                  uint32_t protected_media_ssrc,
                  RecoveredPacketReceiver* recovered_packet_receiver);
  ~FlexfecReceiver();

  // Inserts a received packet (can be either media or FlexFEC) into the
  // internal buffer, and sends the received packets to the erasure code.
  // All newly recovered packets are sent back through the callback.
  void OnRtpPacket(const RtpPacketReceived& packet);

  // Returns a counter describing the added and recovered packets.
  FecPacketCounter GetPacketCounter() const;

  // Protected to aid testing.
 protected:
  std::unique_ptr<ForwardErrorCorrection::ReceivedPacket> AddReceivedPacket(
      const RtpPacketReceived& packet);
  void ProcessReceivedPacket(
      const ForwardErrorCorrection::ReceivedPacket& received_packet);

 private:
  // Config.
  const uint32_t ssrc_;
  const uint32_t protected_media_ssrc_;

  // Erasure code interfacing and callback.
  std::unique_ptr<ForwardErrorCorrection> erasure_code_
      RTC_GUARDED_BY(sequence_checker_);
  ForwardErrorCorrection::RecoveredPacketList recovered_packets_
      RTC_GUARDED_BY(sequence_checker_);
  RecoveredPacketReceiver* const recovered_packet_receiver_;

  // Logging and stats.
  Clock* const clock_;
  int64_t last_recovered_packet_ms_ RTC_GUARDED_BY(sequence_checker_);
  FecPacketCounter packet_counter_ RTC_GUARDED_BY(sequence_checker_);

  rtc::SequencedTaskChecker sequence_checker_;
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_INCLUDE_FLEXFEC_RECEIVER_H_
