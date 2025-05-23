/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VIDEO_VIDEO_STREAM_DECODER_H_
#define VIDEO_VIDEO_STREAM_DECODER_H_

#include <list>
#include <map>
#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"
#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__remote_bitrate_estimator_h //original-code:"modules/remote_bitrate_estimator/include/remote_bitrate_estimator.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_defines_h //original-code:"modules/video_coding/include/video_coding_defines.h"
#include BOSS_WEBRTC_U_rtc_base__platform_thread_h //original-code:"rtc_base/platform_thread.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"

namespace webrtc {

class ReceiveStatisticsProxy;
class VideoReceiver2;

class VideoStreamDecoder : public VCMReceiveCallback {
 public:
  VideoStreamDecoder(
      VideoReceiver2* video_receiver,
      ReceiveStatisticsProxy* receive_statistics_proxy,
      rtc::VideoSinkInterface<VideoFrame>* incoming_video_stream);
  ~VideoStreamDecoder() override;

  // Implements VCMReceiveCallback.
  int32_t FrameToRender(VideoFrame& video_frame,
                        absl::optional<uint8_t> qp,
                        int32_t decode_time_ms,
                        VideoContentType content_type) override;
  void OnDroppedFrames(uint32_t frames_dropped) override;
  void OnIncomingPayloadType(int payload_type) override;
  void OnDecoderImplementationName(const char* implementation_name) override;

  void RegisterReceiveStatisticsProxy(
      ReceiveStatisticsProxy* receive_statistics_proxy);

 private:
  // Used for all registered callbacks except rendering.
  Mutex mutex_;

  VideoReceiver2* const video_receiver_;

  ReceiveStatisticsProxy* const receive_stats_callback_;
  rtc::VideoSinkInterface<VideoFrame>* const incoming_video_stream_;
};

}  // namespace webrtc

#endif  // VIDEO_VIDEO_STREAM_DECODER_H_
