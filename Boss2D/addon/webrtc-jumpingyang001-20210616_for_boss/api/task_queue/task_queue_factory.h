/*
 *  Copyright 2019 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef API_TASK_QUEUE_TASK_QUEUE_FACTORY_H_
#define API_TASK_QUEUE_TASK_QUEUE_FACTORY_H_

#include <memory>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"

namespace webrtc {

// The implementation of this interface must be thread-safe.
class TaskQueueFactory {
 public:
  // TaskQueue priority levels. On some platforms these will map to thread
  // priorities, on others such as Mac and iOS, GCD queue priorities.
  enum class Priority { NORMAL = 0, HIGH, LOW };

  virtual ~TaskQueueFactory() = default;
  virtual std::unique_ptr<TaskQueueBase, TaskQueueDeleter> CreateTaskQueue(
      absl::string_view name,
      Priority priority) const = 0;
};

}  // namespace webrtc

#endif  // API_TASK_QUEUE_TASK_QUEUE_FACTORY_H_
