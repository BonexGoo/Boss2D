/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_FRAME_GENERATOR_CAPTURER_H_
#define TEST_FRAME_GENERATOR_CAPTURER_H_

#include <memory>
#include <string>

#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_api__test__frame_generator_interface_h //original-code:"api/test/frame_generator_interface.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__repeating_task_h //original-code:"rtc_base/task_utils/repeating_task.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"
#include BOSS_WEBRTC_U_test__test_video_capturer_h //original-code:"test/test_video_capturer.h"

namespace webrtc {

namespace test {
namespace frame_gen_cap_impl {
template <typename T>
class AutoOpt : public absl::optional<T> {
 public:
  using absl::optional<T>::optional;
  T* operator->() {
    if (!absl::optional<T>::has_value())
      this->emplace(T());
    return absl::optional<T>::operator->();
  }
};
}  // namespace frame_gen_cap_impl
struct FrameGeneratorCapturerConfig {
  struct SquaresVideo {
    int framerate = 30;
    FrameGeneratorInterface::OutputType pixel_format =
        FrameGeneratorInterface::OutputType::kI420;
    int width = 320;
    int height = 180;
    int num_squares = 10;
  };

  struct SquareSlides {
    int framerate = 30;
    TimeDelta change_interval = TimeDelta::Seconds(10);
    int width = 1600;
    int height = 1200;
  };

  struct VideoFile {
    int framerate = 30;
    std::string name;
    // Must be set to width and height of the source video file.
    int width = 0;
    int height = 0;
  };

  struct ImageSlides {
    int framerate = 30;
    TimeDelta change_interval = TimeDelta::Seconds(10);
    struct Crop {
      TimeDelta scroll_duration = TimeDelta::Seconds(0);
      absl::optional<int> width;
      absl::optional<int> height;
    } crop;
    int width = 1850;
    int height = 1110;
    std::vector<std::string> paths = {
        "web_screenshot_1850_1110",
        "presentation_1850_1110",
        "photo_1850_1110",
        "difficult_photo_1850_1110",
    };
  };

  frame_gen_cap_impl::AutoOpt<SquaresVideo> squares_video;
  frame_gen_cap_impl::AutoOpt<SquareSlides> squares_slides;
  frame_gen_cap_impl::AutoOpt<VideoFile> video_file;
  frame_gen_cap_impl::AutoOpt<ImageSlides> image_slides;
};

class FrameGeneratorCapturer : public TestVideoCapturer {
 public:
  class SinkWantsObserver {
   public:
    // OnSinkWantsChanged is called when FrameGeneratorCapturer::AddOrUpdateSink
    // is called.
    virtual void OnSinkWantsChanged(rtc::VideoSinkInterface<VideoFrame>* sink,
                                    const rtc::VideoSinkWants& wants) = 0;

   protected:
    virtual ~SinkWantsObserver() {}
  };

  FrameGeneratorCapturer(
      Clock* clock,
      std::unique_ptr<FrameGeneratorInterface> frame_generator,
      int target_fps,
      TaskQueueFactory& task_queue_factory);
  virtual ~FrameGeneratorCapturer();

  static std::unique_ptr<FrameGeneratorCapturer> Create(
      Clock* clock,
      TaskQueueFactory& task_queue_factory,
      FrameGeneratorCapturerConfig::SquaresVideo config);
  static std::unique_ptr<FrameGeneratorCapturer> Create(
      Clock* clock,
      TaskQueueFactory& task_queue_factory,
      FrameGeneratorCapturerConfig::SquareSlides config);
  static std::unique_ptr<FrameGeneratorCapturer> Create(
      Clock* clock,
      TaskQueueFactory& task_queue_factory,
      FrameGeneratorCapturerConfig::VideoFile config);
  static std::unique_ptr<FrameGeneratorCapturer> Create(
      Clock* clock,
      TaskQueueFactory& task_queue_factory,
      FrameGeneratorCapturerConfig::ImageSlides config);
  static std::unique_ptr<FrameGeneratorCapturer> Create(
      Clock* clock,
      TaskQueueFactory& task_queue_factory,
      const FrameGeneratorCapturerConfig& config);

  void Start();
  void Stop();
  void ChangeResolution(size_t width, size_t height);
  void ChangeFramerate(int target_framerate);

  void SetSinkWantsObserver(SinkWantsObserver* observer);

  void AddOrUpdateSink(rtc::VideoSinkInterface<VideoFrame>* sink,
                       const rtc::VideoSinkWants& wants) override;
  void RemoveSink(rtc::VideoSinkInterface<VideoFrame>* sink) override;

  void ForceFrame();
  void SetFakeRotation(VideoRotation rotation);
  void SetFakeColorSpace(absl::optional<ColorSpace> color_space);

  int64_t first_frame_capture_time() const { return first_frame_capture_time_; }

  bool Init();

 private:
  void InsertFrame();
  static bool Run(void* obj);
  int GetCurrentConfiguredFramerate();
  void UpdateFps(int max_fps) RTC_EXCLUSIVE_LOCKS_REQUIRED(&lock_);

  Clock* const clock_;
  RepeatingTaskHandle frame_task_;
  bool sending_;
  SinkWantsObserver* sink_wants_observer_ RTC_GUARDED_BY(&lock_);

  Mutex lock_;
  std::unique_ptr<FrameGeneratorInterface> frame_generator_;

  int source_fps_ RTC_GUARDED_BY(&lock_);
  int target_capture_fps_ RTC_GUARDED_BY(&lock_);
  absl::optional<int> wanted_fps_ RTC_GUARDED_BY(&lock_);
  VideoRotation fake_rotation_ = kVideoRotation_0;
  absl::optional<ColorSpace> fake_color_space_ RTC_GUARDED_BY(&lock_);

  int64_t first_frame_capture_time_;
  // Must be the last field, so it will be deconstructed first as tasks
  // in the TaskQueue access other fields of the instance of this class.
  rtc::TaskQueue task_queue_;
};
}  // namespace test
}  // namespace webrtc

#endif  // TEST_FRAME_GENERATOR_CAPTURER_H_
