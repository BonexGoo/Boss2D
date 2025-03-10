/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_modules__video_coding__codecs__av1__libaom_av1_decoder_h //original-code:"modules/video_coding/codecs/av1/libaom_av1_decoder.h"

#include <stdint.h>

#include <memory>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_api__video__i420_buffer_h //original-code:"api/video/i420_buffer.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_common_video__include__video_frame_buffer_pool_h //original-code:"common_video/include/video_frame_buffer_pool.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_error_codes_h //original-code:"modules/video_coding/include/video_error_codes.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include "third_party/libaom/source/libaom/aom/aom_decoder.h"
#include "third_party/libaom/source/libaom/aom/aomdx.h"
#include BOSS_LIBYUV_U_third_party__libyuv__include__libyuv__convert_h //original-code:"third_party/libyuv/include/libyuv/convert.h"

namespace webrtc {
namespace {

constexpr int kConfigLowBitDepth = 1;  // 8-bits per luma/chroma sample.
constexpr int kDecFlags = 0;           // 0 signals no post processing.

class LibaomAv1Decoder final : public VideoDecoder {
 public:
  LibaomAv1Decoder();
  LibaomAv1Decoder(const LibaomAv1Decoder&) = delete;
  LibaomAv1Decoder& operator=(const LibaomAv1Decoder&) = delete;
  ~LibaomAv1Decoder();

  // Implements VideoDecoder.
  int32_t InitDecode(const VideoCodec* codec_settings,
                     int number_of_cores) override;

  // Decode an encoded video frame.
  int32_t Decode(const EncodedImage& encoded_image,
                 bool missing_frames,
                 int64_t render_time_ms) override;

  int32_t RegisterDecodeCompleteCallback(
      DecodedImageCallback* callback) override;

  int32_t Release() override;

  DecoderInfo GetDecoderInfo() const override;
  const char* ImplementationName() const override;

 private:
  aom_codec_ctx_t context_;
  bool inited_;
  // Pool of memory buffers to store decoded image data for application access.
  VideoFrameBufferPool buffer_pool_;
  DecodedImageCallback* decode_complete_callback_;
};

LibaomAv1Decoder::LibaomAv1Decoder()
    : context_(),  // Force value initialization instead of default one.
      inited_(false),
      buffer_pool_(false, /*max_number_of_buffers=*/150),
      decode_complete_callback_(nullptr) {}

LibaomAv1Decoder::~LibaomAv1Decoder() {
  Release();
}

int32_t LibaomAv1Decoder::InitDecode(const VideoCodec* codec_settings,
                                     int number_of_cores) {
  aom_codec_dec_cfg_t config = {
      static_cast<unsigned int>(number_of_cores),  // Max # of threads.
      0,                    // Frame width set after decode.
      0,                    // Frame height set after decode.
      kConfigLowBitDepth};  // Enable low-bit-depth code path.

  aom_codec_err_t ret =
      aom_codec_dec_init(&context_, aom_codec_av1_dx(), &config, kDecFlags);
  if (ret != AOM_CODEC_OK) {
    RTC_LOG(LS_WARNING) << "LibaomAv1Decoder::InitDecode returned " << ret
                        << " on aom_codec_dec_init.";
    return WEBRTC_VIDEO_CODEC_ERROR;
  }
  inited_ = true;
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t LibaomAv1Decoder::Decode(const EncodedImage& encoded_image,
                                 bool missing_frames,
                                 int64_t /*render_time_ms*/) {
  if (!inited_) {
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }
  if (decode_complete_callback_ == nullptr) {
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }

  // Decode one video frame.
  aom_codec_err_t ret =
      aom_codec_decode(&context_, encoded_image.data(), encoded_image.size(),
                       /*user_priv=*/nullptr);
  if (ret != AOM_CODEC_OK) {
    RTC_LOG(LS_WARNING) << "LibaomAv1Decoder::Decode returned " << ret
                        << " on aom_codec_decode.";
    return WEBRTC_VIDEO_CODEC_ERROR;
  }

  // Get decoded frame data.
  int corrupted_frame = 0;
  aom_codec_iter_t iter = nullptr;
  while (aom_image_t* decoded_image = aom_codec_get_frame(&context_, &iter)) {
    if (aom_codec_control(&context_, AOMD_GET_FRAME_CORRUPTED,
                          &corrupted_frame)) {
      RTC_LOG(LS_WARNING) << "LibaomAv1Decoder::Decode "
                             "AOM_GET_FRAME_CORRUPTED.";
    }
    // Check that decoded image format is I420 and has 8-bit depth.
    if (decoded_image->fmt != AOM_IMG_FMT_I420) {
      RTC_LOG(LS_WARNING) << "LibaomAv1Decoder::Decode invalid image format";
      return WEBRTC_VIDEO_CODEC_ERROR;
    }

    // Return decoded frame data.
    int qp;
    ret = aom_codec_control(&context_, AOMD_GET_LAST_QUANTIZER, &qp);
    if (ret != AOM_CODEC_OK) {
      RTC_LOG(LS_WARNING) << "LibaomAv1Decoder::Decode returned " << ret
                          << " on control AOME_GET_LAST_QUANTIZER.";
      return WEBRTC_VIDEO_CODEC_ERROR;
    }

    // Allocate memory for decoded frame.
    rtc::scoped_refptr<I420Buffer> buffer =
        buffer_pool_.CreateI420Buffer(decoded_image->d_w, decoded_image->d_h);
    if (!buffer.get()) {
      // Pool has too many pending frames.
      RTC_LOG(LS_WARNING) << "LibaomAv1Decoder::Decode returned due to lack of"
                             " space in decoded frame buffer pool.";
      return WEBRTC_VIDEO_CODEC_ERROR;
    }

    // Copy decoded_image to decoded_frame.
    libyuv::I420Copy(
        decoded_image->planes[AOM_PLANE_Y], decoded_image->stride[AOM_PLANE_Y],
        decoded_image->planes[AOM_PLANE_U], decoded_image->stride[AOM_PLANE_U],
        decoded_image->planes[AOM_PLANE_V], decoded_image->stride[AOM_PLANE_V],
        buffer->MutableDataY(), buffer->StrideY(), buffer->MutableDataU(),
        buffer->StrideU(), buffer->MutableDataV(), buffer->StrideV(),
        decoded_image->d_w, decoded_image->d_h);
    VideoFrame decoded_frame = VideoFrame::Builder()
                                   .set_video_frame_buffer(buffer)
                                   .set_timestamp_rtp(encoded_image.Timestamp())
                                   .set_ntp_time_ms(encoded_image.ntp_time_ms_)
                                   .set_color_space(encoded_image.ColorSpace())
                                   .build();

    decode_complete_callback_->Decoded(decoded_frame, absl::nullopt,
                                       absl::nullopt);
  }
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t LibaomAv1Decoder::RegisterDecodeCompleteCallback(
    DecodedImageCallback* decode_complete_callback) {
  decode_complete_callback_ = decode_complete_callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t LibaomAv1Decoder::Release() {
  if (aom_codec_destroy(&context_) != AOM_CODEC_OK) {
    return WEBRTC_VIDEO_CODEC_MEMORY;
  }
  buffer_pool_.Release();
  inited_ = false;
  return WEBRTC_VIDEO_CODEC_OK;
}

VideoDecoder::DecoderInfo LibaomAv1Decoder::GetDecoderInfo() const {
  DecoderInfo info;
  info.implementation_name = "libaom";
  info.is_hardware_accelerated = false;
  return info;
}

const char* LibaomAv1Decoder::ImplementationName() const {
  return "libaom";
}

}  // namespace

const bool kIsLibaomAv1DecoderSupported = true;

std::unique_ptr<VideoDecoder> CreateLibaomAv1Decoder() {
  return std::make_unique<LibaomAv1Decoder>();
}

}  // namespace webrtc
