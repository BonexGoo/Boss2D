/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_MESSAGE_HANDLER_H_
#define RTC_BASE_MESSAGE_HANDLER_H_

#include <utility>

#include BOSS_WEBRTC_U_api__function_view_h //original-code:"api/function_view.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"

namespace rtc {

struct Message;

// MessageQueue/Thread Messages get dispatched via the MessageHandler interface.
class RTC_EXPORT MessageHandler {
 public:
  virtual ~MessageHandler() {}
  virtual void OnMessage(Message* msg) = 0;
};

// Warning: Provided for backwards compatibility.
//
// This class performs expensive cleanup in the dtor that will affect all
// instances of Thread (and their pending message queues) and will block the
// current thread as well as all other threads.
class RTC_EXPORT MessageHandlerAutoCleanup : public MessageHandler {
 public:
  ~MessageHandlerAutoCleanup() override;

 protected:
  MessageHandlerAutoCleanup();

 private:
  RTC_DISALLOW_COPY_AND_ASSIGN(MessageHandlerAutoCleanup);
};

}  // namespace rtc

#endif  // RTC_BASE_MESSAGE_HANDLER_H_
