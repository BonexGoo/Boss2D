/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_ANDROID_SRC_JNI_PC_TURNCUSTOMIZER_H_
#define SDK_ANDROID_SRC_JNI_PC_TURNCUSTOMIZER_H_

#include BOSS_WEBRTC_U_api__turncustomizer_h //original-code:"api/turncustomizer.h"
#include "sdk/android/src/jni/scoped_java_ref.h"

namespace webrtc {
namespace jni {

TurnCustomizer* GetNativeTurnCustomizer(
    JNIEnv* env,
    const JavaRef<jobject>& j_turn_customizer);

}  // namespace jni
}  // namespace webrtc

#endif  // SDK_ANDROID_SRC_JNI_PC_TURNCUSTOMIZER_H_
