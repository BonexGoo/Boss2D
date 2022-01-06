/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <jni.h>

#include BOSS_WEBRTC_U_modules__utility__include__jvm_android_h //original-code:"modules/utility/include/jvm_android.h"
#include BOSS_WEBRTC_U_rtc_base__ssladapter_h //original-code:"rtc_base/ssladapter.h"
#include "sdk/android/native_api/base/init.h"

namespace webrtc_examples {

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved) {
  webrtc::InitAndroid(jvm);
  webrtc::JVM::Initialize(jvm);
  RTC_CHECK(rtc::InitializeSSL()) << "Failed to InitializeSSL()";
  return JNI_VERSION_1_6;
}

extern "C" void JNIEXPORT JNICALL JNI_OnUnLoad(JavaVM* jvm, void* reserved) {
  RTC_CHECK(rtc::CleanupSSL()) << "Failed to CleanupSSL()";
}

}  // namespace webrtc_examples
