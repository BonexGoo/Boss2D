/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_ANDROID_SRC_JNI_PC_MEDIASTREAMTRACK_H_
#define SDK_ANDROID_SRC_JNI_PC_MEDIASTREAMTRACK_H_

#include <jni.h>

#include BOSS_WEBRTC_U_api__mediatypes_h //original-code:"api/mediatypes.h"
#include "sdk/android/src/jni/scoped_java_ref.h"

namespace webrtc {
namespace jni {

ScopedJavaLocalRef<jobject> NativeToJavaMediaType(
    JNIEnv* jni,
    cricket::MediaType media_type);
cricket::MediaType JavaToNativeMediaType(JNIEnv* jni,
                                         const JavaRef<jobject>& j_media_type);

}  // namespace jni
}  // namespace webrtc

#endif  // SDK_ANDROID_SRC_JNI_PC_MEDIASTREAMTRACK_H_
