/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_ULPFEC_RECEIVER_IMPL_H_
#define MODULES_RTP_RTCP_SOURCE_ULPFEC_RECEIVER_IMPL_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__ulpfec_receiver_h //original-code:"modules/rtp_rtcp/include/ulpfec_receiver.h"
#include "modules/rtp_rtcp/source/forward_error_correction.h"
#include BOSS_WEBRTC_U_rtc_base__criticalsection_h //original-code:"rtc_base/criticalsection.h"
#include BOSS_WEBRTC_U_typedefs_h //original-code:"typedefs.h"  // NOLINT(build/include)

namespace webrtc {

class UlpfecReceiverImpl : public UlpfecReceiver {
 public:
  explicit UlpfecReceiverImpl(uint32_t ssrc, RecoveredPacketReceiver* callback);
  virtual ~UlpfecReceiverImpl();

  int32_t AddReceivedRedPacket(const RTPHeader& rtp_header,
                               const uint8_t* incoming_rtp_packet,
                               size_t packet_length,
                               uint8_t ulpfec_payload_type) override;

  int32_t ProcessReceivedFec() override;

  FecPacketCounter GetPacketCounter() const override;

 private:
  const uint32_t ssrc_;

  rtc::CriticalSection crit_sect_;
  RecoveredPacketReceiver* recovered_packet_callback_;
  std::unique_ptr<ForwardErrorCorrection> fec_;
  // TODO(nisse): The AddReceivedRedPacket method adds one or two packets to
  // this list at a time, after which it is emptied by ProcessReceivedFec. It
  // will make things simpler to merge AddReceivedRedPacket and
  // ProcessReceivedFec into a single method, and we can then delete this list.
  std::vector<std::unique_ptr<ForwardErrorCorrection::ReceivedPacket>>
      received_packets_;
  ForwardErrorCorrection::RecoveredPacketList recovered_packets_;
  FecPacketCounter packet_counter_;
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_ULPFEC_RECEIVER_IMPL_H_
