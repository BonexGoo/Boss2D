/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_ANDROID_SRC_JNI_WRAPPEDNATIVECODEC_H_
#define SDK_ANDROID_SRC_JNI_WRAPPEDNATIVECODEC_H_

#include <jni.h>
#include <memory>

#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include "sdk/android/src/jni/scoped_java_ref.h"

namespace webrtc {
namespace jni {

/* If the j_decoder is a wrapped native decoder, unwrap it. If it is not,
 * wrap it in a VideoDecoderWrapper.
 */
std::unique_ptr<VideoDecoder> JavaToNativeVideoDecoder(
    JNIEnv* jni,
    const JavaRef<jobject>& j_decoder);

/* If the j_encoder is a wrapped native encoder, unwrap it. If it is not,
 * wrap it in a VideoEncoderWrapper.
 */
std::unique_ptr<VideoEncoder> JavaToNativeVideoEncoder(
    JNIEnv* jni,
    const JavaRef<jobject>& j_encoder);

bool IsWrappedSoftwareEncoder(JNIEnv* jni, const JavaRef<jobject>& j_encoder);

}  // namespace jni
}  // namespace webrtc

#endif  // SDK_ANDROID_SRC_JNI_WRAPPEDNATIVECODEC_H_
