/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_ANDROID_NATIVE_API_BASE_NETWORKMONITOR_H_
#define SDK_ANDROID_NATIVE_API_BASE_NETWORKMONITOR_H_

#include <jni.h>

#include <memory>

#include BOSS_WEBRTC_U_rtc_base__networkmonitor_h //original-code:"rtc_base/networkmonitor.h"

namespace webrtc {

// Creates an Android-specific network monitor, which is capable of detecting
// network changes as soon as they occur, requesting a cellular interface
// (dependent on permissions), and binding sockets to network interfaces (more
// reliable than binding to IP addresses on Android).
std::unique_ptr<rtc::NetworkMonitorFactory> CreateAndroidNetworkMonitorFactory(
    JNIEnv* env,
    jobject application_context);

// Deprecated. Pass in application context instead.
std::unique_ptr<rtc::NetworkMonitorFactory>
CreateAndroidNetworkMonitorFactory();

}  // namespace webrtc

#endif  // SDK_ANDROID_NATIVE_API_BASE_NETWORKMONITOR_H_