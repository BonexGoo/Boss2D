/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef AUDIO_VOIP_TEST_MOCK_TASK_QUEUE_H_
#define AUDIO_VOIP_TEST_MOCK_TASK_QUEUE_H_

#include <memory>

#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

// MockTaskQueue enables immediate task run from global TaskQueueBase.
// It's necessary for some tests depending on TaskQueueBase internally.
class MockTaskQueue : public TaskQueueBase {
 public:
  MockTaskQueue() : current_(this) {}

  // Delete is deliberately defined as no-op as MockTaskQueue is expected to
  // hold onto current global TaskQueueBase throughout the testing.
  void Delete() override {}

  MOCK_METHOD(void, PostTask, (std::unique_ptr<QueuedTask>), (override));
  MOCK_METHOD(void,
              PostDelayedTask,
              (std::unique_ptr<QueuedTask>, uint32_t),
              (override));

 private:
  CurrentTaskQueueSetter current_;
};

class MockTaskQueueFactory : public TaskQueueFactory {
 public:
  explicit MockTaskQueueFactory(MockTaskQueue* task_queue)
      : task_queue_(task_queue) {}

  std::unique_ptr<TaskQueueBase, TaskQueueDeleter> CreateTaskQueue(
      absl::string_view name,
      Priority priority) const override {
    // Default MockTaskQueue::Delete is no-op, therefore it's safe to pass the
    // raw pointer.
    return std::unique_ptr<TaskQueueBase, TaskQueueDeleter>(task_queue_);
  }

 private:
  MockTaskQueue* task_queue_;
};

}  // namespace webrtc

#endif  // AUDIO_VOIP_TEST_MOCK_TASK_QUEUE_H_
