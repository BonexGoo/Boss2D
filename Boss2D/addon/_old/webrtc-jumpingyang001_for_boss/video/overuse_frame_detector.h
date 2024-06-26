/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VIDEO_OVERUSE_FRAME_DETECTOR_H_
#define VIDEO_OVERUSE_FRAME_DETECTOR_H_

#include <list>
#include <memory>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video__video_stream_encoder_observer_h //original-code:"api/video/video_stream_encoder_observer.h"
#include BOSS_WEBRTC_U_modules__video_coding__utility__quality_scaler_h //original-code:"modules/video_coding/utility/quality_scaler.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__exp_filter_h //original-code:"rtc_base/numerics/exp_filter.h"
#include BOSS_WEBRTC_U_rtc_base__sequenced_task_checker_h //original-code:"rtc_base/sequenced_task_checker.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class VideoFrame;

struct CpuOveruseOptions {
  CpuOveruseOptions();

  int low_encode_usage_threshold_percent;  // Threshold for triggering underuse.
  int high_encode_usage_threshold_percent;  // Threshold for triggering overuse.
  // General settings.
  int frame_timeout_interval_ms;  // The maximum allowed interval between two
                                  // frames before resetting estimations.
  int min_frame_samples;          // The minimum number of frames required.
  int min_process_count;  // The number of initial process times required before
                          // triggering an overuse/underuse.
  int high_threshold_consecutive_count;  // The number of consecutive checks
                                         // above the high threshold before
                                         // triggering an overuse.
  // New estimator enabled if this is set non-zero.
  int filter_time_ms;  // Time constant for averaging
};

// Use to detect system overuse based on the send-side processing time of
// incoming frames. All methods must be called on a single task queue but it can
// be created and destroyed on an arbitrary thread.
// OveruseFrameDetector::StartCheckForOveruse  must be called to periodically
// check for overuse.
class OveruseFrameDetector {
 public:
  explicit OveruseFrameDetector(CpuOveruseMetricsObserver* metrics_observer);
  virtual ~OveruseFrameDetector();

  // Start to periodically check for overuse.
  void StartCheckForOveruse(const CpuOveruseOptions& options,
                            AdaptationObserverInterface* overuse_observer);

  // StopCheckForOveruse must be called before destruction if
  // StartCheckForOveruse has been called.
  void StopCheckForOveruse();

  // Defines the current maximum framerate targeted by the capturer. This is
  // used to make sure the encode usage percent doesn't drop unduly if the
  // capturer has quiet periods (for instance caused by screen capturers with
  // variable capture rate depending on content updates), otherwise we might
  // experience adaptation toggling.
  virtual void OnTargetFramerateUpdated(int framerate_fps);

  // Called for each captured frame.
  void FrameCaptured(const VideoFrame& frame, int64_t time_when_first_seen_us);

  // Called for each sent frame.
  void FrameSent(uint32_t timestamp,
                 int64_t time_sent_in_us,
                 int64_t capture_time_us,
                 absl::optional<int> encode_duration_us);

  // Interface for cpu load estimation. Intended for internal use only.
  class ProcessingUsage {
   public:
    virtual void Reset() = 0;
    virtual void SetMaxSampleDiffMs(float diff_ms) = 0;
    virtual void FrameCaptured(const VideoFrame& frame,
                               int64_t time_when_first_seen_us,
                               int64_t last_capture_time_us) = 0;
    // Returns encode_time in us, if there's a new measurement.
    virtual absl::optional<int> FrameSent(
        // These two argument used by old estimator.
        uint32_t timestamp,
        int64_t time_sent_in_us,
        // And these two by the new estimator.
        int64_t capture_time_us,
        absl::optional<int> encode_duration_us) = 0;

    virtual int Value() = 0;
    virtual ~ProcessingUsage() = default;
  };

 protected:
  // Protected for test purposes.
  void CheckForOveruse(AdaptationObserverInterface* overuse_observer);
  void SetOptions(const CpuOveruseOptions& options);

  CpuOveruseOptions options_;

 private:
  class CheckOveruseTask;

  void EncodedFrameTimeMeasured(int encode_duration_ms);
  bool IsOverusing(int encode_usage_percent);
  bool IsUnderusing(int encode_usage_percent, int64_t time_now);

  bool FrameTimeoutDetected(int64_t now) const;
  bool FrameSizeChanged(int num_pixels) const;

  void ResetAll(int num_pixels);

  static std::unique_ptr<ProcessingUsage> CreateProcessingUsage(
      const CpuOveruseOptions& options);

  rtc::SequencedTaskChecker task_checker_;
  // Owned by the task queue from where StartCheckForOveruse is called.
  CheckOveruseTask* check_overuse_task_;

  // Stats metrics.
  CpuOveruseMetricsObserver* const metrics_observer_;
  absl::optional<int> encode_usage_percent_ RTC_GUARDED_BY(task_checker_);

  int64_t num_process_times_ RTC_GUARDED_BY(task_checker_);

  int64_t last_capture_time_us_ RTC_GUARDED_BY(task_checker_);

  // Number of pixels of last captured frame.
  int num_pixels_ RTC_GUARDED_BY(task_checker_);
  int max_framerate_ RTC_GUARDED_BY(task_checker_);
  int64_t last_overuse_time_ms_ RTC_GUARDED_BY(task_checker_);
  int checks_above_threshold_ RTC_GUARDED_BY(task_checker_);
  int num_overuse_detections_ RTC_GUARDED_BY(task_checker_);
  int64_t last_rampup_time_ms_ RTC_GUARDED_BY(task_checker_);
  bool in_quick_rampup_ RTC_GUARDED_BY(task_checker_);
  int current_rampup_delay_ms_ RTC_GUARDED_BY(task_checker_);

  std::unique_ptr<ProcessingUsage> usage_ RTC_PT_GUARDED_BY(task_checker_);

  RTC_DISALLOW_COPY_AND_ASSIGN(OveruseFrameDetector);
};

}  // namespace webrtc

#endif  // VIDEO_OVERUSE_FRAME_DETECTOR_H_
