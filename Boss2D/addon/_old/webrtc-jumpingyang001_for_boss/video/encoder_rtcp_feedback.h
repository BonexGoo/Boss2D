/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef VIDEO_ENCODER_RTCP_FEEDBACK_H_
#define VIDEO_ENCODER_RTCP_FEEDBACK_H_

#include <vector>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_rtc_base__criticalsection_h //original-code:"rtc_base/criticalsection.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

class VideoStreamEncoderInterface;

class EncoderRtcpFeedback : public RtcpIntraFrameObserver {
 public:
  EncoderRtcpFeedback(Clock* clock,
                      const std::vector<uint32_t>& ssrcs,
                      VideoStreamEncoderInterface* encoder);
  void OnReceivedIntraFrameRequest(uint32_t ssrc) override;

 private:
  bool HasSsrc(uint32_t ssrc);
  size_t GetStreamIndex(uint32_t ssrc);

  Clock* const clock_;
  const std::vector<uint32_t> ssrcs_;
  VideoStreamEncoderInterface* const video_stream_encoder_;

  rtc::CriticalSection crit_;
  std::vector<int64_t> time_last_intra_request_ms_ RTC_GUARDED_BY(crit_);
};

}  // namespace webrtc

#endif  // VIDEO_ENCODER_RTCP_FEEDBACK_H_
