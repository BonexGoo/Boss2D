/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_VIDEOCAPTURERTRACKSOURCE_H_
#define PC_VIDEOCAPTURERTRACKSOURCE_H_

#include <memory>

#include BOSS_WEBRTC_U_api__mediastreaminterface_h //original-code:"api/mediastreaminterface.h"
#include BOSS_WEBRTC_U_media__base__videocapturer_h //original-code:"media/base/videocapturer.h"
#include BOSS_WEBRTC_U_media__base__videocommon_h //original-code:"media/base/videocommon.h"
#include BOSS_WEBRTC_U_pc__videotracksource_h //original-code:"pc/videotracksource.h"
#include BOSS_WEBRTC_U_rtc_base__asyncinvoker_h //original-code:"rtc_base/asyncinvoker.h"
#include BOSS_WEBRTC_U_rtc_base__sigslot_h //original-code:"rtc_base/sigslot.h"

// VideoCapturerTrackSource implements VideoTrackSourceInterface. It owns a
// cricket::VideoCapturer and make sure the camera is started at a resolution
// that honors the constraints.
// The state is set depending on the result of starting the capturer.
// If the constraint can't be met or the capturer fails to start, the state
// transition to kEnded, otherwise it transitions to kLive.
namespace webrtc {

class MediaConstraintsInterface;

class VideoCapturerTrackSource : public VideoTrackSource,
                                 public sigslot::has_slots<> {
 public:
  // Creates an instance of VideoCapturerTrackSource from |capturer|.
  // |constraints| can be NULL and in that case the camera is opened using a
  // default resolution.
  static rtc::scoped_refptr<VideoTrackSourceInterface> Create(
      rtc::Thread* worker_thread,
      std::unique_ptr<cricket::VideoCapturer> capturer,
      const webrtc::MediaConstraintsInterface* constraints,
      bool remote);

  static rtc::scoped_refptr<VideoTrackSourceInterface> Create(
      rtc::Thread* worker_thread,
      std::unique_ptr<cricket::VideoCapturer> capturer,
      bool remote);

  bool is_screencast() const final { return video_capturer_->IsScreencast(); }
  rtc::Optional<bool> needs_denoising() const final { return needs_denoising_; }

  bool GetStats(Stats* stats) final;

 protected:
  VideoCapturerTrackSource(rtc::Thread* worker_thread,
                           std::unique_ptr<cricket::VideoCapturer> capturer,
                           bool remote);
  virtual ~VideoCapturerTrackSource();
  void Initialize(const webrtc::MediaConstraintsInterface* constraints);

 private:
  void Stop();

  void OnStateChange(cricket::VideoCapturer* capturer,
                     cricket::CaptureState capture_state);

  rtc::Thread* signaling_thread_;
  rtc::Thread* worker_thread_;
  rtc::AsyncInvoker invoker_;
  std::unique_ptr<cricket::VideoCapturer> video_capturer_;
  bool started_;
  cricket::VideoFormat format_;
  rtc::Optional<bool> needs_denoising_;
};

}  // namespace webrtc

#endif  // PC_VIDEOCAPTURERTRACKSOURCE_H_
