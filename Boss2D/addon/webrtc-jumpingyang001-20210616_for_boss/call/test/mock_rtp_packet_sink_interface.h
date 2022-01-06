/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef CALL_TEST_MOCK_RTP_PACKET_SINK_INTERFACE_H_
#define CALL_TEST_MOCK_RTP_PACKET_SINK_INTERFACE_H_

#include BOSS_WEBRTC_U_call__rtp_packet_sink_interface_h //original-code:"call/rtp_packet_sink_interface.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockRtpPacketSink : public RtpPacketSinkInterface {
 public:
  MOCK_METHOD(void, OnRtpPacket, (const RtpPacketReceived&), (override));
};

}  // namespace webrtc

#endif  // CALL_TEST_MOCK_RTP_PACKET_SINK_INTERFACE_H_
