/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#ifndef MODULES_VIDEO_CODING_CODECS_VP9_LIBVPX_VP9_DECODER_H_
#define MODULES_VIDEO_CODING_CODECS_VP9_LIBVPX_VP9_DECODER_H_

#ifdef RTC_ENABLE_VP9

#include BOSS_WEBRTC_U_api__transport__webrtc_key_value_config_h //original-code:"api/transport/webrtc_key_value_config.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_common_video__include__video_frame_buffer_pool_h //original-code:"common_video/include/video_frame_buffer_pool.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp9__include__vp9_h //original-code:"modules/video_coding/codecs/vp9/include/vp9.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp9__vp9_frame_buffer_pool_h //original-code:"modules/video_coding/codecs/vp9/vp9_frame_buffer_pool.h"
#include "vpx/vp8cx.h"

namespace webrtc {

class LibvpxVp9Decoder : public VP9Decoder {
 public:
  LibvpxVp9Decoder();
  explicit LibvpxVp9Decoder(const WebRtcKeyValueConfig& trials);

  virtual ~LibvpxVp9Decoder();

  int InitDecode(const VideoCodec* inst, int number_of_cores) override;

  int Decode(const EncodedImage& input_image,
             bool missing_frames,
             int64_t /*render_time_ms*/) override;

  int RegisterDecodeCompleteCallback(DecodedImageCallback* callback) override;

  int Release() override;

  DecoderInfo GetDecoderInfo() const override;
  const char* ImplementationName() const override;

 private:
  int ReturnFrame(const vpx_image_t* img,
                  uint32_t timestamp,
                  int qp,
                  const webrtc::ColorSpace* explicit_color_space);

  // Memory pool used to share buffers between libvpx and webrtc.
  Vp9FrameBufferPool libvpx_buffer_pool_;
  // Buffer pool used to allocate additionally needed NV12 buffers.
  VideoFrameBufferPool output_buffer_pool_;
  DecodedImageCallback* decode_complete_callback_;
  bool inited_;
  vpx_codec_ctx_t* decoder_;
  bool key_frame_required_;
  VideoCodec current_codec_;
  int num_cores_;

  // Decoder should produce this format if possible.
  const VideoFrameBuffer::Type preferred_output_format_;
};
}  // namespace webrtc

#endif  // RTC_ENABLE_VP9

#endif  // MODULES_VIDEO_CODING_CODECS_VP9_LIBVPX_VP9_DECODER_H_
