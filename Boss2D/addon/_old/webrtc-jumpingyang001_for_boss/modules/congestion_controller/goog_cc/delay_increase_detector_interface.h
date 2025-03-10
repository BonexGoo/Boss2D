/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_DELAY_INCREASE_DETECTOR_INTERFACE_H_
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_DELAY_INCREASE_DETECTOR_INTERFACE_H_

#include <stdint.h>

#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__bwe_defines_h //original-code:"modules/remote_bitrate_estimator/include/bwe_defines.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

class DelayIncreaseDetectorInterface {
 public:
  DelayIncreaseDetectorInterface() {}
  virtual ~DelayIncreaseDetectorInterface() {}

  // Update the detector with a new sample. The deltas should represent deltas
  // between timestamp groups as defined by the InterArrival class.
  virtual void Update(double recv_delta_ms,
                      double send_delta_ms,
                      int64_t arrival_time_ms) = 0;

  virtual BandwidthUsage State() const = 0;

  RTC_DISALLOW_COPY_AND_ASSIGN(DelayIncreaseDetectorInterface);
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_DELAY_INCREASE_DETECTOR_INTERFACE_H_
