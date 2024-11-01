/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_MOCKS_MOCK_RECOVERED_PACKET_RECEIVER_H_
#define MODULES_RTP_RTCP_MOCKS_MOCK_RECOVERED_PACKET_RECEIVER_H_

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__flexfec_receiver_h //original-code:"modules/rtp_rtcp/include/flexfec_receiver.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockRecoveredPacketReceiver : public RecoveredPacketReceiver {
 public:
  MOCK_METHOD(void,
              OnRecoveredPacket,
              (const uint8_t* packet, size_t length),
              (override));
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_MOCKS_MOCK_RECOVERED_PACKET_RECEIVER_H_
