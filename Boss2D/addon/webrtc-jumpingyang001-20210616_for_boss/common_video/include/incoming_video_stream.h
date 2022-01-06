/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef COMMON_VIDEO_INCLUDE_INCOMING_VIDEO_STREAM_H_
#define COMMON_VIDEO_INCLUDE_INCOMING_VIDEO_STREAM_H_

#include <stdint.h>

#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"
#include BOSS_WEBRTC_U_common_video__video_render_frames_h //original-code:"common_video/video_render_frames.h"
#include BOSS_WEBRTC_U_rtc_base__race_checker_h //original-code:"rtc_base/race_checker.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class IncomingVideoStream : public rtc::VideoSinkInterface<VideoFrame> {
 public:
  IncomingVideoStream(TaskQueueFactory* task_queue_factory,
                      int32_t delay_ms,
                      rtc::VideoSinkInterface<VideoFrame>* callback);
  ~IncomingVideoStream() override;

 private:
  void OnFrame(const VideoFrame& video_frame) override;
  void Dequeue();

  SequenceChecker main_thread_checker_;
  rtc::RaceChecker decoder_race_checker_;

  VideoRenderFrames render_buffers_ RTC_GUARDED_BY(&incoming_render_queue_);
  rtc::VideoSinkInterface<VideoFrame>* const callback_;
  rtc::TaskQueue incoming_render_queue_;
};

}  // namespace webrtc

#endif  // COMMON_VIDEO_INCLUDE_INCOMING_VIDEO_STREAM_H_
