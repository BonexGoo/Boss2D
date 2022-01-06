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

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_header_extension_map_h //original-code:"modules/rtp_rtcp/include/rtp_header_extension_map.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__ulpfec_receiver_h //original-code:"modules/rtp_rtcp/include/ulpfec_receiver.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__forward_error_correction_h //original-code:"modules/rtp_rtcp/source/forward_error_correction.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"
#include BOSS_WEBRTC_U_rtc_base__system__no_unique_address_h //original-code:"rtc_base/system/no_unique_address.h"

namespace webrtc {

class UlpfecReceiverImpl : public UlpfecReceiver {
 public:
  explicit UlpfecReceiverImpl(uint32_t ssrc,
                              RecoveredPacketReceiver* callback,
                              rtc::ArrayView<const RtpExtension> extensions);
  ~UlpfecReceiverImpl() override;

  bool AddReceivedRedPacket(const RtpPacketReceived& rtp_packet,
                            uint8_t ulpfec_payload_type) override;

  int32_t ProcessReceivedFec() override;

  FecPacketCounter GetPacketCounter() const override;

 private:
  const uint32_t ssrc_;
  const RtpHeaderExtensionMap extensions_;

  RTC_NO_UNIQUE_ADDRESS SequenceChecker sequence_checker_;
  RecoveredPacketReceiver* const recovered_packet_callback_;
  const std::unique_ptr<ForwardErrorCorrection> fec_;
  // TODO(nisse): The AddReceivedRedPacket method adds one or two packets to
  // this list at a time, after which it is emptied by ProcessReceivedFec. It
  // will make things simpler to merge AddReceivedRedPacket and
  // ProcessReceivedFec into a single method, and we can then delete this list.
  std::vector<std::unique_ptr<ForwardErrorCorrection::ReceivedPacket>>
      received_packets_ RTC_GUARDED_BY(&sequence_checker_);
  ForwardErrorCorrection::RecoveredPacketList recovered_packets_
      RTC_GUARDED_BY(&sequence_checker_);
  FecPacketCounter packet_counter_ RTC_GUARDED_BY(&sequence_checker_);
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_ULPFEC_RECEIVER_IMPL_H_
