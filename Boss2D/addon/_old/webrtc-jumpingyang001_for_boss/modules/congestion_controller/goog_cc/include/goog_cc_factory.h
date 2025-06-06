/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_INCLUDE_GOOG_CC_FACTORY_H_
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_INCLUDE_GOOG_CC_FACTORY_H_
#include <memory>

#include BOSS_WEBRTC_U_api__transport__network_control_h //original-code:"api/transport/network_control.h"

namespace webrtc {
class Clock;
class RtcEventLog;

class GoogCcNetworkControllerFactory
    : public NetworkControllerFactoryInterface {
 public:
  explicit GoogCcNetworkControllerFactory(RtcEventLog*);
  std::unique_ptr<NetworkControllerInterface> Create(
      NetworkControllerConfig config) override;
  TimeDelta GetProcessInterval() const override;

 private:
  RtcEventLog* const event_log_;
};

// Factory to create packet feedback only GoogCC, this can be used for
// connections providing packet receive time feedback but no other reports.
class GoogCcFeedbackNetworkControllerFactory
    : public NetworkControllerFactoryInterface {
 public:
  explicit GoogCcFeedbackNetworkControllerFactory(RtcEventLog*);
  std::unique_ptr<NetworkControllerInterface> Create(
      NetworkControllerConfig config) override;
  TimeDelta GetProcessInterval() const override;

 private:
  RtcEventLog* const event_log_;
};
}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_INCLUDE_GOOG_CC_FACTORY_H_
