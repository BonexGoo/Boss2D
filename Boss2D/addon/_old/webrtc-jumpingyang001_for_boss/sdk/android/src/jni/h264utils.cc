/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "sdk/android/src/jni/videocodecinfo.h"

#include BOSS_WEBRTC_U_common_video__h264__profile_level_id_h //original-code:"common_video/h264/profile_level_id.h"
#include "sdk/android/generated_video_jni/jni/H264Utils_jni.h"

namespace webrtc {
namespace jni {

static jboolean JNI_H264Utils_IsSameH264Profile(
    JNIEnv* env,
    const JavaParamRef<jclass>&,
    const JavaParamRef<jobject>& params1,
    const JavaParamRef<jobject>& params2) {
  return H264::IsSameH264Profile(JavaToNativeStringMap(env, params1),
                                 JavaToNativeStringMap(env, params2));
}

}  // namespace jni
}  // namespace webrtc
