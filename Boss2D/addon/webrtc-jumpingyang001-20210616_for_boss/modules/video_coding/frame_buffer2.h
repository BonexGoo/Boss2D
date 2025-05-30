/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_FRAME_BUFFER2_H_
#define MODULES_VIDEO_CODING_FRAME_BUFFER2_H_

#include <array>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__container__inlined_vector_h //original-code:"absl/container/inlined_vector.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__video__encoded_frame_h //original-code:"api/video/encoded_frame.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_defines_h //original-code:"modules/video_coding/include/video_coding_defines.h"
#include BOSS_WEBRTC_U_modules__video_coding__inter_frame_delay_h //original-code:"modules/video_coding/inter_frame_delay.h"
#include BOSS_WEBRTC_U_modules__video_coding__jitter_estimator_h //original-code:"modules/video_coding/jitter_estimator.h"
#include BOSS_WEBRTC_U_modules__video_coding__utility__decoded_frames_history_h //original-code:"modules/video_coding/utility/decoded_frames_history.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__rtt_mult_experiment_h //original-code:"rtc_base/experiments/rtt_mult_experiment.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__sequence_number_util_h //original-code:"rtc_base/numerics/sequence_number_util.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__system__no_unique_address_h //original-code:"rtc_base/system/no_unique_address.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__repeating_task_h //original-code:"rtc_base/task_utils/repeating_task.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class Clock;
class VCMReceiveStatisticsCallback;
class VCMJitterEstimator;
class VCMTiming;

namespace video_coding {

class FrameBuffer {
 public:
  enum ReturnReason { kFrameFound, kTimeout, kStopped };

  FrameBuffer(Clock* clock,
              VCMTiming* timing,
              VCMReceiveStatisticsCallback* stats_callback);

  FrameBuffer() = delete;
  FrameBuffer(const FrameBuffer&) = delete;
  FrameBuffer& operator=(const FrameBuffer&) = delete;

  virtual ~FrameBuffer();

  // Insert a frame into the frame buffer. Returns the picture id
  // of the last continuous frame or -1 if there is no continuous frame.
  int64_t InsertFrame(std::unique_ptr<EncodedFrame> frame);

  // Get the next frame for decoding. Will return at latest after
  // |max_wait_time_ms|.
  void NextFrame(
      int64_t max_wait_time_ms,
      bool keyframe_required,
      rtc::TaskQueue* callback_queue,
      std::function<void(std::unique_ptr<EncodedFrame>, ReturnReason)> handler);

  // Tells the FrameBuffer which protection mode that is in use. Affects
  // the frame timing.
  // TODO(philipel): Remove this when new timing calculations has been
  //                 implemented.
  void SetProtectionMode(VCMVideoProtection mode);

  // Stop the frame buffer, causing any sleeping thread in NextFrame to
  // return immediately.
  void Stop();

  // Updates the RTT for jitter buffer estimation.
  void UpdateRtt(int64_t rtt_ms);

  // Clears the FrameBuffer, removing all the buffered frames.
  void Clear();

  int Size();

 private:
  struct FrameInfo {
    FrameInfo();
    FrameInfo(FrameInfo&&);
    ~FrameInfo();

    // Which other frames that have direct unfulfilled dependencies
    // on this frame.
    absl::InlinedVector<int64_t, 8> dependent_frames;

    // A frame is continiuous if it has all its referenced/indirectly
    // referenced frames.
    //
    // How many unfulfilled frames this frame have until it becomes continuous.
    size_t num_missing_continuous = 0;

    // A frame is decodable if all its referenced frames have been decoded.
    //
    // How many unfulfilled frames this frame have until it becomes decodable.
    size_t num_missing_decodable = 0;

    // If this frame is continuous or not.
    bool continuous = false;

    // The actual EncodedFrame.
    std::unique_ptr<EncodedFrame> frame;
  };

  using FrameMap = std::map<int64_t, FrameInfo>;

  // Check that the references of |frame| are valid.
  bool ValidReferences(const EncodedFrame& frame) const;

  int64_t FindNextFrame(int64_t now_ms) RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);
  EncodedFrame* GetNextFrame() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  void StartWaitForNextFrameOnQueue() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);
  void CancelCallback() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Update all directly dependent and indirectly dependent frames and mark
  // them as continuous if all their references has been fulfilled.
  void PropagateContinuity(FrameMap::iterator start)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Marks the frame as decoded and updates all directly dependent frames.
  void PropagateDecodability(const FrameInfo& info)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Update the corresponding FrameInfo of |frame| and all FrameInfos that
  // |frame| references.
  // Return false if |frame| will never be decodable, true otherwise.
  bool UpdateFrameInfoWithIncomingFrame(const EncodedFrame& frame,
                                        FrameMap::iterator info)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  void UpdateJitterDelay() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  void UpdateTimingFrameInfo() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  void ClearFramesAndHistory() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  bool HasBadRenderTiming(const EncodedFrame& frame, int64_t now_ms)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // The cleaner solution would be to have the NextFrame function return a
  // vector of frames, but until the decoding pipeline can support decoding
  // multiple frames at the same time we combine all frames to one frame and
  // return it. See bugs.webrtc.org/10064
  EncodedFrame* CombineAndDeleteFrames(
      const std::vector<EncodedFrame*>& frames) const;

  RTC_NO_UNIQUE_ADDRESS SequenceChecker construction_checker_;
  RTC_NO_UNIQUE_ADDRESS SequenceChecker callback_checker_;

  // Stores only undecoded frames.
  FrameMap frames_ RTC_GUARDED_BY(mutex_);
  DecodedFramesHistory decoded_frames_history_ RTC_GUARDED_BY(mutex_);

  Mutex mutex_;
  Clock* const clock_;

  rtc::TaskQueue* callback_queue_ RTC_GUARDED_BY(mutex_);
  RepeatingTaskHandle callback_task_ RTC_GUARDED_BY(mutex_);
  std::function<void(std::unique_ptr<EncodedFrame>, ReturnReason)>
      frame_handler_ RTC_GUARDED_BY(mutex_);
  int64_t latest_return_time_ms_ RTC_GUARDED_BY(mutex_);
  bool keyframe_required_ RTC_GUARDED_BY(mutex_);

  VCMJitterEstimator jitter_estimator_ RTC_GUARDED_BY(mutex_);
  VCMTiming* const timing_ RTC_GUARDED_BY(mutex_);
  VCMInterFrameDelay inter_frame_delay_ RTC_GUARDED_BY(mutex_);
  absl::optional<int64_t> last_continuous_frame_ RTC_GUARDED_BY(mutex_);
  std::vector<FrameMap::iterator> frames_to_decode_ RTC_GUARDED_BY(mutex_);
  bool stopped_ RTC_GUARDED_BY(mutex_);
  VCMVideoProtection protection_mode_ RTC_GUARDED_BY(mutex_);
  VCMReceiveStatisticsCallback* const stats_callback_;
  int64_t last_log_non_decoded_ms_ RTC_GUARDED_BY(mutex_);

  const bool add_rtt_to_playout_delay_;

  // rtt_mult experiment settings.
  const absl::optional<RttMultExperiment::Settings> rtt_mult_settings_;
};

}  // namespace video_coding
}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_FRAME_BUFFER2_H_
