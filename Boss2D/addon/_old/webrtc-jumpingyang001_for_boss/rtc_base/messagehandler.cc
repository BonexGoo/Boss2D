/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__messagehandler_h //original-code:"rtc_base/messagehandler.h"
#include BOSS_WEBRTC_U_rtc_base__messagequeue_h //original-code:"rtc_base/messagequeue.h"

namespace rtc {

MessageHandler::~MessageHandler() {
  MessageQueueManager::Clear(this);
}

}  // namespace rtc
