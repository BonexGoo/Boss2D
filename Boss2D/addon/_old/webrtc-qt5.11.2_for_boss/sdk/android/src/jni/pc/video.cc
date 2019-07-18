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
#include BOSS_WEBRTC_U_media__engine__webrtcvideodecoderfactory_h //original-code:"media/engine/webrtcvideodecoderfactory.h"
#include BOSS_WEBRTC_U_media__engine__webrtcvideoencoderfactory_h //original-code:"media/engine/webrtcvideoencoderfactory.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include "sdk/android/generated_video_jni/jni/EglBase14_jni.h"
#include "sdk/android/src/jni/androidmediadecoder_jni.h"
#include "sdk/android/src/jni/androidmediaencoder_jni.h"
#include "sdk/android/src/jni/androidvideotracksource.h"
#include "sdk/android/src/jni/pc/ownedfactoryandthreads.h"
#include "sdk/android/src/jni/surfacetexturehelper.h"
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

void SetEglContext(JNIEnv* env,
                   cricket::WebRtcVideoEncoderFactory* encoder_factory,
                   const JavaRef<jobject>& egl_context) {
  if (encoder_factory) {
    MediaCodecVideoEncoderFactory* media_codec_factory =
        static_cast<MediaCodecVideoEncoderFactory*>(encoder_factory);
    if (media_codec_factory && Java_Context_isEgl14Context(env, egl_context)) {
      RTC_LOG(LS_INFO) << "Set EGL context for HW encoding.";
      media_codec_factory->SetEGLContext(env, egl_context.obj());
    }
  }
}

void SetEglContext(JNIEnv* env,
                   cricket::WebRtcVideoDecoderFactory* decoder_factory,
                   const JavaRef<jobject>& egl_context) {
  if (decoder_factory) {
    MediaCodecVideoDecoderFactory* media_codec_factory =
        static_cast<MediaCodecVideoDecoderFactory*>(decoder_factory);
    if (media_codec_factory) {
      RTC_LOG(LS_INFO) << "Set EGL context for HW decoding.";
      media_codec_factory->SetEGLContext(env, egl_context.obj());
    }
  }
}

void* CreateVideoSource(JNIEnv* env,
                        rtc::Thread* signaling_thread,
                        rtc::Thread* worker_thread,
                        const JavaParamRef<jobject>& j_surface_texture_helper,
                        jboolean is_screencast) {
  rtc::scoped_refptr<AndroidVideoTrackSource> source(
      new rtc::RefCountedObject<AndroidVideoTrackSource>(
          signaling_thread, env, j_surface_texture_helper, is_screencast));
  return VideoTrackSourceProxy::Create(signaling_thread, worker_thread, source)
      .release();
}

cricket::WebRtcVideoEncoderFactory* CreateLegacyVideoEncoderFactory() {
  return new MediaCodecVideoEncoderFactory();
}

cricket::WebRtcVideoDecoderFactory* CreateLegacyVideoDecoderFactory() {
  return new MediaCodecVideoDecoderFactory();
}

VideoEncoderFactory* WrapLegacyVideoEncoderFactory(
    cricket::WebRtcVideoEncoderFactory* legacy_encoder_factory) {
  return ConvertVideoEncoderFactory(
             std::unique_ptr<cricket::WebRtcVideoEncoderFactory>(
                 legacy_encoder_factory))
      .release();
}

VideoDecoderFactory* WrapLegacyVideoDecoderFactory(
    cricket::WebRtcVideoDecoderFactory* legacy_decoder_factory) {
  return ConvertVideoDecoderFactory(
             std::unique_ptr<cricket::WebRtcVideoDecoderFactory>(
                 legacy_decoder_factory))
      .release();
}

}  // namespace jni
}  // namespace webrtc
