/*
 *  Copyright 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_OBJC_NATIVE_API_NETWORK_MONITOR_FACTORY_H_
#define SDK_OBJC_NATIVE_API_NETWORK_MONITOR_FACTORY_H_

#include <memory>

#include BOSS_WEBRTC_U_rtc_base__network_monitor_factory_h //original-code:"rtc_base/network_monitor_factory.h"

namespace webrtc {

std::unique_ptr<rtc::NetworkMonitorFactory> CreateNetworkMonitorFactory();

}  // namespace webrtc

#endif  // SDK_OBJC_NATIVE_API_NETWORK_MONITOR_FACTORY_H_
