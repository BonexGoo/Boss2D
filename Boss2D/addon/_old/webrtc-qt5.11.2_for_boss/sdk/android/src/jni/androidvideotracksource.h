/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_ANDROID_JNI_ANDROIDVIDEOTRACKSOURCE_H_
#define API_ANDROID_JNI_ANDROIDVIDEOTRACKSOURCE_H_

#include <jni.h>

#include BOSS_WEBRTC_U_common_video__include__i420_buffer_pool_h //original-code:"common_video/include/i420_buffer_pool.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include BOSS_WEBRTC_U_media__base__adaptedvideotracksource_h //original-code:"media/base/adaptedvideotracksource.h"
#include BOSS_WEBRTC_U_rtc_base__asyncinvoker_h //original-code:"rtc_base/asyncinvoker.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__thread_checker_h //original-code:"rtc_base/thread_checker.h"
#include BOSS_WEBRTC_U_rtc_base__timestampaligner_h //original-code:"rtc_base/timestampaligner.h"
#include "sdk/android/src/jni/surfacetexturehelper.h"
#include "sdk/android/src/jni/videoframe.h"

namespace webrtc {
namespace jni {

class AndroidVideoTrackSource : public rtc::AdaptedVideoTrackSource {
 public:
  AndroidVideoTrackSource(rtc::Thread* signaling_thread,
                          JNIEnv* jni,
                          const JavaRef<jobject>& j_surface_texture_helper,
                          bool is_screencast = false);

  bool is_screencast() const override { return is_screencast_; }

  // Indicates that the encoder should denoise video before encoding it.
  // If it is not set, the default configuration is used which is different
  // depending on video codec.
  rtc::Optional<bool> needs_denoising() const override { return false; }

  // Called by the native capture observer
  void SetState(SourceState state);

  SourceState state() const override { return state_; }

  bool remote() const override { return false; }

  void OnByteBufferFrameCaptured(const void* frame_data,
                                 int length,
                                 int width,
                                 int height,
                                 VideoRotation rotation,
                                 int64_t timestamp_ns);

  void OnTextureFrameCaptured(int width,
                              int height,
                              VideoRotation rotation,
                              int64_t timestamp_ns,
                              const NativeHandleImpl& handle);

  void OnFrameCaptured(JNIEnv* jni,
                       int width,
                       int height,
                       int64_t timestamp_ns,
                       VideoRotation rotation,
                       const JavaRef<jobject>& j_video_frame_buffer);

  void OnOutputFormatRequest(int width, int height, int fps);

  rtc::scoped_refptr<SurfaceTextureHelper> surface_texture_helper() {
    return surface_texture_helper_;
  }

 private:
  rtc::Thread* signaling_thread_;
  rtc::AsyncInvoker invoker_;
  rtc::ThreadChecker camera_thread_checker_;
  SourceState state_;
  rtc::TimestampAligner timestamp_aligner_;
  NV12ToI420Scaler nv12toi420_scaler_;
  I420BufferPool buffer_pool_;
  rtc::scoped_refptr<SurfaceTextureHelper> surface_texture_helper_;
  const bool is_screencast_;
};

}  // namespace jni
}  // namespace webrtc

#endif  // API_ANDROID_JNI_ANDROIDVIDEOTRACKSOURCE_H_
