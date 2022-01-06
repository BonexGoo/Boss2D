/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_RECEIVER_H_
#define MODULES_VIDEO_CODING_RECEIVER_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_modules__video_coding__event_wrapper_h //original-code:"modules/video_coding/event_wrapper.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_h //original-code:"modules/video_coding/include/video_coding.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_defines_h //original-code:"modules/video_coding/include/video_coding_defines.h"
#include BOSS_WEBRTC_U_modules__video_coding__jitter_buffer_h //original-code:"modules/video_coding/jitter_buffer.h"
#include BOSS_WEBRTC_U_modules__video_coding__packet_h //original-code:"modules/video_coding/packet.h"
#include BOSS_WEBRTC_U_modules__video_coding__timing_h //original-code:"modules/video_coding/timing.h"

namespace webrtc {

class Clock;
class VCMEncodedFrame;

class VCMReceiver {
 public:
  VCMReceiver(VCMTiming* timing, Clock* clock);

  // Using this constructor, you can specify a different event implemetation for
  // the jitter buffer. Useful for unit tests when you want to simulate incoming
  // packets, in which case the jitter buffer's wait event is different from
  // that of VCMReceiver itself.
  VCMReceiver(VCMTiming* timing,
              Clock* clock,
              std::unique_ptr<EventWrapper> receiver_event,
              std::unique_ptr<EventWrapper> jitter_buffer_event);

  ~VCMReceiver();

  int32_t InsertPacket(const VCMPacket& packet);
  VCMEncodedFrame* FrameForDecoding(uint16_t max_wait_time_ms,
                                    bool prefer_late_decoding);
  void ReleaseFrame(VCMEncodedFrame* frame);

  // NACK.
  void SetNackSettings(size_t max_nack_list_size,
                       int max_packet_age_to_nack,
                       int max_incomplete_time_ms);
  std::vector<uint16_t> NackList(bool* request_key_frame);

 private:
  Clock* const clock_;
  VCMJitterBuffer jitter_buffer_;
  VCMTiming* timing_;
  std::unique_ptr<EventWrapper> render_wait_event_;
  int max_video_delay_ms_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_RECEIVER_H_
