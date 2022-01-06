/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_CALL_FACTORY_H_
#define CALL_CALL_FACTORY_H_

#include BOSS_WEBRTC_U_api__call__call_factory_interface_h //original-code:"api/call/call_factory_interface.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_call__call_h //original-code:"call/call.h"
#include BOSS_WEBRTC_U_call__call_config_h //original-code:"call/call_config.h"
#include BOSS_WEBRTC_U_rtc_base__system__no_unique_address_h //original-code:"rtc_base/system/no_unique_address.h"

namespace webrtc {

class CallFactory : public CallFactoryInterface {
 public:
  CallFactory();

 private:
  ~CallFactory() override {}

  Call* CreateCall(const CallConfig& config) override;

  RTC_NO_UNIQUE_ADDRESS SequenceChecker call_thread_;
  rtc::scoped_refptr<SharedModuleThread> module_thread_
      RTC_GUARDED_BY(call_thread_);
};

}  // namespace webrtc

#endif  // CALL_CALL_FACTORY_H_
