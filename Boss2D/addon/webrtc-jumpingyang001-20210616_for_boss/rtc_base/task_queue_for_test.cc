/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__task_queue_for_test_h //original-code:"rtc_base/task_queue_for_test.h"

#include BOSS_WEBRTC_U_api__task_queue__default_task_queue_factory_h //original-code:"api/task_queue/default_task_queue_factory.h"

namespace webrtc {

TaskQueueForTest::TaskQueueForTest(absl::string_view name, Priority priority)
    : TaskQueue(
          CreateDefaultTaskQueueFactory()->CreateTaskQueue(name, priority)) {}

}  // namespace webrtc
