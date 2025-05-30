/*
 *  Copyright 2019 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_TASK_QUEUE_LIBEVENT_H_
#define RTC_BASE_TASK_QUEUE_LIBEVENT_H_

#include <memory>

#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"

namespace webrtc {

std::unique_ptr<TaskQueueFactory> CreateTaskQueueLibeventFactory();

}  // namespace webrtc

#endif  // RTC_BASE_TASK_QUEUE_LIBEVENT_H_
