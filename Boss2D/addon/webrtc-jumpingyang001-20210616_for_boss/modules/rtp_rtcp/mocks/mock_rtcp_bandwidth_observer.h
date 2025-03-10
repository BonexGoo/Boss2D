/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_MOCKS_MOCK_RTCP_BANDWIDTH_OBSERVER_H_
#define MODULES_RTP_RTCP_MOCKS_MOCK_RTCP_BANDWIDTH_OBSERVER_H_

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockRtcpBandwidthObserver : public RtcpBandwidthObserver {
 public:
  MOCK_METHOD(void, OnReceivedEstimatedBitrate, (uint32_t), (override));
  MOCK_METHOD(void,
              OnReceivedRtcpReceiverReport,
              (const ReportBlockList&, int64_t, int64_t),
              (override));
};
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_MOCKS_MOCK_RTCP_BANDWIDTH_OBSERVER_H_
