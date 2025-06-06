/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "sdk/android/src/jni/pc/video.h"

#include <jni.h>
#include <memory>

#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_factory_h //original-code:"api/video_codecs/video_encoder_factory.h"
#include BOSS_WEBRTC_U_api__videosourceproxy_h //original-code:"api/videosourceproxy.h"
#include BOSS_WEBRTC_U_media__engine__convert_legacy_video_factory_h //original-code:"media/engine/convert_legacy_video_factory.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include "sdk/android/src/jni/androidvideotracksource.h"
#include "sdk/android/src/jni/videodecoderfactorywrapper.h"
#include "sdk/android/src/jni/videoencoderfactorywrapper.h"

namespace webrtc {
namespace jni {

VideoEncoderFactory* CreateVideoEncoderFactory(
    JNIEnv* jni,
    const JavaRef<jobject>& j_encoder_factory) {
  return new VideoEncoderFactoryWrapper(jni, j_encoder_factory);
}

VideoDecoderFactory* CreateVideoDecoderFactory(
    JNIEnv* jni,
    const JavaRef<jobject>& j_decoder_factory) {
  return new VideoDecoderFactoryWrapper(jni, j_decoder_factory);
}

void* CreateVideoSource(JNIEnv* env,
                        rtc::Thread* signaling_thread,
                        rtc::Thread* worker_thread,
                        jboolean is_screencast) {
  rtc::scoped_refptr<AndroidVideoTrackSource> source(
      new rtc::RefCountedObject<AndroidVideoTrackSource>(signaling_thread, env,
                                                         is_screencast));
  return VideoTrackSourceProxy::Create(signaling_thread, worker_thread, source)
      .release();
}

}  // namespace jni
}  // namespace webrtc
