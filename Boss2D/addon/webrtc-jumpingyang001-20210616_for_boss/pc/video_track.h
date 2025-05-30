/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_VIDEO_TRACK_H_
#define PC_VIDEO_TRACK_H_

#include <string>

#include BOSS_WEBRTC_U_api__media_stream_interface_h //original-code:"api/media_stream_interface.h"
#include BOSS_WEBRTC_U_api__media_stream_track_h //original-code:"api/media_stream_track.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"
#include BOSS_WEBRTC_U_api__video__video_source_interface_h //original-code:"api/video/video_source_interface.h"
#include BOSS_WEBRTC_U_media__base__video_source_base_h //original-code:"media/base/video_source_base.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class VideoTrack : public MediaStreamTrack<VideoTrackInterface>,
                   public rtc::VideoSourceBaseGuarded,
                   public ObserverInterface {
 public:
  static rtc::scoped_refptr<VideoTrack> Create(
      const std::string& label,
      VideoTrackSourceInterface* source,
      rtc::Thread* worker_thread);

  void AddOrUpdateSink(rtc::VideoSinkInterface<VideoFrame>* sink,
                       const rtc::VideoSinkWants& wants) override;
  void RemoveSink(rtc::VideoSinkInterface<VideoFrame>* sink) override;
  VideoTrackSourceInterface* GetSource() const override;

  ContentHint content_hint() const override;
  void set_content_hint(ContentHint hint) override;
  bool set_enabled(bool enable) override;
  bool enabled() const override;
  MediaStreamTrackInterface::TrackState state() const override;
  std::string kind() const override;

 protected:
  VideoTrack(const std::string& id,
             VideoTrackSourceInterface* video_source,
             rtc::Thread* worker_thread);
  ~VideoTrack();

 private:
  // Implements ObserverInterface. Observes |video_source_| state.
  void OnChanged() override;

  RTC_NO_UNIQUE_ADDRESS webrtc::SequenceChecker signaling_thread_;
  rtc::Thread* const worker_thread_;
  const rtc::scoped_refptr<VideoTrackSourceInterface> video_source_;
  ContentHint content_hint_ RTC_GUARDED_BY(worker_thread_);
};

}  // namespace webrtc

#endif  // PC_VIDEO_TRACK_H_
