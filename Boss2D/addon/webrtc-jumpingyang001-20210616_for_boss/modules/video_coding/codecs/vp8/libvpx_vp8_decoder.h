/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_VP8_LIBVPX_VP8_DECODER_H_
#define MODULES_VIDEO_CODING_CODECS_VP8_LIBVPX_VP8_DECODER_H_

#include <memory>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_common_video__include__video_frame_buffer_pool_h //original-code:"common_video/include/video_frame_buffer_pool.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp8__include__vp8_h //original-code:"modules/video_coding/codecs/vp8/include/vp8.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include "vpx/vp8dx.h"
#include "vpx/vpx_decoder.h"

namespace webrtc {

class LibvpxVp8Decoder : public VideoDecoder {
 public:
  LibvpxVp8Decoder();
  ~LibvpxVp8Decoder() override;

  int InitDecode(const VideoCodec* inst, int number_of_cores) override;

  int Decode(const EncodedImage& input_image,
             bool missing_frames,
             int64_t /*render_time_ms*/) override;

  int RegisterDecodeCompleteCallback(DecodedImageCallback* callback) override;
  int Release() override;

  DecoderInfo GetDecoderInfo() const override;
  const char* ImplementationName() const override;

  struct DeblockParams {
    DeblockParams() : max_level(6), degrade_qp(1), min_qp(0) {}
    DeblockParams(int max_level, int degrade_qp, int min_qp)
        : max_level(max_level), degrade_qp(degrade_qp), min_qp(min_qp) {}
    int max_level;   // Deblocking strength: [0, 16].
    int degrade_qp;  // If QP value is below, start lowering |max_level|.
    int min_qp;      // If QP value is below, turn off deblocking.
  };

 private:
  class QpSmoother;
  int ReturnFrame(const vpx_image_t* img,
                  uint32_t timeStamp,
                  int qp,
                  const webrtc::ColorSpace* explicit_color_space);
  const bool use_postproc_;

  VideoFrameBufferPool buffer_pool_;
  DecodedImageCallback* decode_complete_callback_;
  bool inited_;
  vpx_codec_ctx_t* decoder_;
  int propagation_cnt_;
  int last_frame_width_;
  int last_frame_height_;
  bool key_frame_required_;
  const absl::optional<DeblockParams> deblock_params_;
  const std::unique_ptr<QpSmoother> qp_smoother_;

  // Decoder should produce this format if possible.
  const VideoFrameBuffer::Type preferred_output_format_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_VP8_LIBVPX_VP8_DECODER_H_
