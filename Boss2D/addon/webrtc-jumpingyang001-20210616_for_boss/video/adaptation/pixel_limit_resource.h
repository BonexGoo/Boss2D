/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VIDEO_ADAPTATION_PIXEL_LIMIT_RESOURCE_H_
#define VIDEO_ADAPTATION_PIXEL_LIMIT_RESOURCE_H_

#include <string>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__adaptation__resource_h //original-code:"api/adaptation/resource.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_call__adaptation__video_stream_input_state_provider_h //original-code:"call/adaptation/video_stream_input_state_provider.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__repeating_task_h //original-code:"rtc_base/task_utils/repeating_task.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

// An adaptation resource designed to be used in the TestBed. Used to simulate
// being CPU limited.
//
// Periodically reports "overuse" or "underuse" (every 5 seconds) until the
// stream is within the bounds specified in terms of a maximum resolution and
// one resolution step lower than that (this avoids toggling when this is the
// only resource in play). When multiple resources come in to play some amount
// of toggling is still possible in edge cases but that is OK for testing
// purposes.
class PixelLimitResource : public Resource {
 public:
  static rtc::scoped_refptr<PixelLimitResource> Create(
      TaskQueueBase* task_queue,
      VideoStreamInputStateProvider* input_state_provider);

  PixelLimitResource(TaskQueueBase* task_queue,
                     VideoStreamInputStateProvider* input_state_provider);
  ~PixelLimitResource() override;

  void SetMaxPixels(int max_pixels);

  // Resource implementation.
  std::string Name() const override { return "PixelLimitResource"; }
  void SetResourceListener(ResourceListener* listener) override;

 private:
  TaskQueueBase* const task_queue_;
  VideoStreamInputStateProvider* const input_state_provider_;
  absl::optional<int> max_pixels_ RTC_GUARDED_BY(task_queue_);
  webrtc::ResourceListener* listener_ RTC_GUARDED_BY(task_queue_);
  RepeatingTaskHandle repeating_task_ RTC_GUARDED_BY(task_queue_);
};

}  // namespace webrtc

#endif  // VIDEO_ADAPTATION_PIXEL_LIMIT_RESOURCE_H_
