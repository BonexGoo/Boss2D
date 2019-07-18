/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_RTP_RTCP_TEST_TESTAPI_TEST_API_H_
#define MODULES_RTP_RTCP_TEST_TESTAPI_TEST_API_H_

#include BOSS_WEBRTC_U_api__call__transport_h //original-code:"api/call/transport.h"
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__receive_statistics_h //original-code:"modules/rtp_rtcp/include/receive_statistics.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_header_parser_h //original-code:"modules/rtp_rtcp/include/rtp_header_parser.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_payload_registry_h //original-code:"modules/rtp_rtcp/include/rtp_payload_registry.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_receiver_h //original-code:"modules/rtp_rtcp/include/rtp_receiver.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include "test/gtest.h"

namespace webrtc {

// This class sends all its packet straight to the provided RtpRtcp module.
// with optional packet loss.
class LoopBackTransport : public Transport {
 public:
  LoopBackTransport()
      : count_(0),
        packet_loss_(0),
        rtp_payload_registry_(NULL),
        rtp_receiver_(NULL),
        rtp_rtcp_module_(NULL) {}
  void SetSendModule(RtpRtcp* rtp_rtcp_module,
                     RTPPayloadRegistry* payload_registry,
                     RtpReceiver* receiver,
                     ReceiveStatistics* receive_statistics);
  void DropEveryNthPacket(int n);
  bool SendRtp(const uint8_t* data,
               size_t len,
               const PacketOptions& options) override;
  bool SendRtcp(const uint8_t* data, size_t len) override;

 private:
  int count_;
  int packet_loss_;
  ReceiveStatistics* receive_statistics_;
  RTPPayloadRegistry* rtp_payload_registry_;
  RtpReceiver* rtp_receiver_;
  RtpRtcp* rtp_rtcp_module_;
};

class TestRtpReceiver : public RtpData {
 public:
  int32_t OnReceivedPayloadData(
      const uint8_t* payload_data,
      size_t payload_size,
      const webrtc::WebRtcRTPHeader* rtp_header) override;

  const uint8_t* payload_data() const { return payload_data_; }
  size_t payload_size() const { return payload_size_; }
  webrtc::WebRtcRTPHeader rtp_header() const { return rtp_header_; }

 private:
  uint8_t payload_data_[1500];
  size_t payload_size_;
  webrtc::WebRtcRTPHeader rtp_header_;
};

}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_TEST_TESTAPI_TEST_API_H_
