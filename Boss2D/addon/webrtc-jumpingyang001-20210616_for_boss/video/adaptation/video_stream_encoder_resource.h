/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VIDEO_ADAPTATION_VIDEO_STREAM_ENCODER_RESOURCE_H_
#define VIDEO_ADAPTATION_VIDEO_STREAM_ENCODER_RESOURCE_H_

#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__adaptation__resource_h //original-code:"api/adaptation/resource.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"
#include BOSS_WEBRTC_U_call__adaptation__adaptation_constraint_h //original-code:"call/adaptation/adaptation_constraint.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"

namespace webrtc {

class VideoStreamEncoderResource : public Resource {
 public:
  ~VideoStreamEncoderResource() override;

  // Registering task queues must be performed as part of initialization.
  void RegisterEncoderTaskQueue(TaskQueueBase* encoder_queue);

  // Resource implementation.
  std::string Name() const override;
  void SetResourceListener(ResourceListener* listener) override;

 protected:
  explicit VideoStreamEncoderResource(std::string name);

  void OnResourceUsageStateMeasured(ResourceUsageState usage_state);

  // The caller is responsible for ensuring the task queue is still valid.
  TaskQueueBase* encoder_queue() const;

 private:
  mutable Mutex lock_;
  const std::string name_;
  // Treated as const after initialization.
  TaskQueueBase* encoder_queue_;
  ResourceListener* listener_ RTC_GUARDED_BY(lock_);
};

}  // namespace webrtc

#endif  // VIDEO_ADAPTATION_VIDEO_STREAM_ENCODER_RESOURCE_H_
