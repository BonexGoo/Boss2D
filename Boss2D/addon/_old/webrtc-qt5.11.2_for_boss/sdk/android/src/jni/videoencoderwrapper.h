/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_ANDROID_SRC_JNI_VIDEOENCODERWRAPPER_H_
#define SDK_ANDROID_SRC_JNI_VIDEOENCODERWRAPPER_H_

#include <jni.h>
#include <deque>
#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp9__include__vp9_globals_h //original-code:"modules/video_coding/codecs/vp9/include/vp9_globals.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"
#include "sdk/android/src/jni/jni_helpers.h"
#include "sdk/android/src/jni/videoframe.h"

namespace webrtc {
namespace jni {

// Wraps a Java encoder and delegates all calls to it.
class VideoEncoderWrapper : public VideoEncoder {
 public:
  VideoEncoderWrapper(JNIEnv* jni, const JavaRef<jobject>& j_encoder);

  int32_t InitEncode(const VideoCodec* codec_settings,
                     int32_t number_of_cores,
                     size_t max_payload_size) override;

  int32_t RegisterEncodeCompleteCallback(
      EncodedImageCallback* callback) override;

  int32_t Release() override;

  int32_t Encode(const VideoFrame& frame,
                 const CodecSpecificInfo* codec_specific_info,
                 const std::vector<FrameType>* frame_types) override;

  int32_t SetChannelParameters(uint32_t packet_loss, int64_t rtt) override;

  int32_t SetRateAllocation(const BitrateAllocation& allocation,
                            uint32_t framerate) override;

  ScalingSettings GetScalingSettings() const override;

  bool SupportsNativeHandle() const override { return true; }

  // Should only be called by JNI.
  void OnEncodedFrame(JNIEnv* jni,
                      const JavaRef<jobject>& j_caller,
                      const JavaRef<jobject>& j_buffer,
                      jint encoded_width,
                      jint encoded_height,
                      jlong capture_time_ms,
                      jint frame_type,
                      jint rotation,
                      jboolean complete_frame,
                      const JavaRef<jobject>& j_qp);

  const char* ImplementationName() const override;

 private:
  struct FrameExtraInfo {
    uint64_t capture_time_ns;  // Used as an identifier of the frame.

    uint32_t timestamp_rtp;
  };

  int32_t InitEncodeInternal(JNIEnv* jni);

  // Takes Java VideoCodecStatus, handles it and returns WEBRTC_VIDEO_CODEC_*
  // status code.
  int32_t HandleReturnCode(JNIEnv* jni, const JavaRef<jobject>& code);

  RTPFragmentationHeader ParseFragmentationHeader(
      const std::vector<uint8_t>& buffer);
  int ParseQp(const std::vector<uint8_t>& buffer);
  CodecSpecificInfo ParseCodecSpecificInfo(const EncodedImage& frame);
  ScopedJavaLocalRef<jobject> ToJavaBitrateAllocation(
      JNIEnv* jni,
      const BitrateAllocation& allocation);
  std::string GetImplementationName(JNIEnv* jni) const;

  const ScopedJavaGlobalRef<jobject> encoder_;
  const ScopedJavaGlobalRef<jclass> int_array_class_;

  std::string implementation_name_;

  rtc::TaskQueue* encoder_queue_;
  std::deque<FrameExtraInfo> frame_extra_infos_;
  EncodedImageCallback* callback_;
  bool initialized_;
  int num_resets_;
  int number_of_cores_;
  VideoCodec codec_settings_;
  H264BitstreamParser h264_bitstream_parser_;

  // RTP state.
  uint16_t picture_id_;
  uint8_t tl0_pic_idx_;

  // VP9 variables to populate codec specific structure.
  GofInfoVP9 gof_;  // Contains each frame's temporal information for
                    // non-flexible VP9 mode.
  size_t gof_idx_;
};

}  // namespace jni
}  // namespace webrtc

#endif  // SDK_ANDROID_SRC_JNI_VIDEOENCODERWRAPPER_H_
