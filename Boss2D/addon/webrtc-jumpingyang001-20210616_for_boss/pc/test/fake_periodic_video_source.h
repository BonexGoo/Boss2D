/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_TEST_FAKE_PERIODIC_VIDEO_SOURCE_H_
#define PC_TEST_FAKE_PERIODIC_VIDEO_SOURCE_H_

#include <memory>

#include BOSS_WEBRTC_U_api__video__video_source_interface_h //original-code:"api/video/video_source_interface.h"
#include BOSS_WEBRTC_U_media__base__fake_frame_source_h //original-code:"media/base/fake_frame_source.h"
#include BOSS_WEBRTC_U_media__base__video_broadcaster_h //original-code:"media/base/video_broadcaster.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_for_test_h //original-code:"rtc_base/task_queue_for_test.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__repeating_task_h //original-code:"rtc_base/task_utils/repeating_task.h"

namespace webrtc {

class FakePeriodicVideoSource final
    : public rtc::VideoSourceInterface<VideoFrame> {
 public:
  static constexpr int kDefaultFrameIntervalMs = 33;
  static constexpr int kDefaultWidth = 640;
  static constexpr int kDefaultHeight = 480;

  struct Config {
    int width = kDefaultWidth;
    int height = kDefaultHeight;
    int frame_interval_ms = kDefaultFrameIntervalMs;
    VideoRotation rotation = kVideoRotation_0;
    int64_t timestamp_offset_ms = 0;
  };

  FakePeriodicVideoSource() : FakePeriodicVideoSource(Config()) {}
  explicit FakePeriodicVideoSource(Config config)
      : frame_source_(
            config.width,
            config.height,
            config.frame_interval_ms * rtc::kNumMicrosecsPerMillisec,
            config.timestamp_offset_ms * rtc::kNumMicrosecsPerMillisec),
        task_queue_(std::make_unique<TaskQueueForTest>(
            "FakePeriodicVideoTrackSource")) {
    thread_checker_.Detach();
    frame_source_.SetRotation(config.rotation);

    TimeDelta frame_interval = TimeDelta::Millis(config.frame_interval_ms);
    RepeatingTaskHandle::Start(task_queue_->Get(), [this, frame_interval] {
      if (broadcaster_.wants().rotation_applied) {
        broadcaster_.OnFrame(frame_source_.GetFrameRotationApplied());
      } else {
        broadcaster_.OnFrame(frame_source_.GetFrame());
      }
      return frame_interval;
    });
  }

  rtc::VideoSinkWants wants() const {
    MutexLock lock(&mutex_);
    return wants_;
  }

  void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) override {
    RTC_DCHECK(thread_checker_.IsCurrent());
    broadcaster_.RemoveSink(sink);
  }

  void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
                       const rtc::VideoSinkWants& wants) override {
    RTC_DCHECK(thread_checker_.IsCurrent());
    {
      MutexLock lock(&mutex_);
      wants_ = wants;
    }
    broadcaster_.AddOrUpdateSink(sink, wants);
  }

  void Stop() {
    RTC_DCHECK(task_queue_);
    task_queue_.reset();
  }

 private:
  SequenceChecker thread_checker_;

  rtc::VideoBroadcaster broadcaster_;
  cricket::FakeFrameSource frame_source_;
  mutable Mutex mutex_;
  rtc::VideoSinkWants wants_ RTC_GUARDED_BY(&mutex_);

  std::unique_ptr<TaskQueueForTest> task_queue_;
};

}  // namespace webrtc

#endif  // PC_TEST_FAKE_PERIODIC_VIDEO_SOURCE_H_
