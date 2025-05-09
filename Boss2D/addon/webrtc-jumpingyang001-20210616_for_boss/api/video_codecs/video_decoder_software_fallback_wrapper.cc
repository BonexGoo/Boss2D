/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_software_fallback_wrapper_h //original-code:"api/video_codecs/video_decoder_software_fallback_wrapper.h"

#include <stdint.h>

#include <memory>
#include <string>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__base__macros_h //original-code:"absl/base/macros.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_error_codes_h //original-code:"modules/video_coding/include/video_error_codes.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__trace_event_h //original-code:"rtc_base/trace_event.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"
#include BOSS_WEBRTC_U_system_wrappers__include__metrics_h //original-code:"system_wrappers/include/metrics.h"

namespace webrtc {

namespace {

constexpr size_t kMaxConsequtiveHwErrors = 4;

class VideoDecoderSoftwareFallbackWrapper final : public VideoDecoder {
 public:
  VideoDecoderSoftwareFallbackWrapper(
      std::unique_ptr<VideoDecoder> sw_fallback_decoder,
      std::unique_ptr<VideoDecoder> hw_decoder);
  ~VideoDecoderSoftwareFallbackWrapper() override;

  int32_t InitDecode(const VideoCodec* codec_settings,
                     int32_t number_of_cores) override;

  int32_t Decode(const EncodedImage& input_image,
                 bool missing_frames,
                 int64_t render_time_ms) override;

  int32_t RegisterDecodeCompleteCallback(
      DecodedImageCallback* callback) override;

  int32_t Release() override;

  DecoderInfo GetDecoderInfo() const override;
  const char* ImplementationName() const override;

 private:
  bool InitFallbackDecoder();
  void UpdateFallbackDecoderHistograms();

  int32_t InitHwDecoder();

  VideoDecoder& active_decoder() const;

  // Determines if we are trying to use the HW or SW decoder.
  enum class DecoderType {
    kNone,
    kHardware,
    kFallback,
  } decoder_type_;
  std::unique_ptr<VideoDecoder> hw_decoder_;

  VideoCodec codec_settings_;
  int32_t number_of_cores_;
  const std::unique_ptr<VideoDecoder> fallback_decoder_;
  const std::string fallback_implementation_name_;
  DecodedImageCallback* callback_;
  int32_t hw_decoded_frames_since_last_fallback_;
  size_t hw_consequtive_generic_errors_;
};

VideoDecoderSoftwareFallbackWrapper::VideoDecoderSoftwareFallbackWrapper(
    std::unique_ptr<VideoDecoder> sw_fallback_decoder,
    std::unique_ptr<VideoDecoder> hw_decoder)
    : decoder_type_(DecoderType::kNone),
      hw_decoder_(std::move(hw_decoder)),
      fallback_decoder_(std::move(sw_fallback_decoder)),
      fallback_implementation_name_(
          std::string(fallback_decoder_->ImplementationName()) +
          " (fallback from: " + hw_decoder_->ImplementationName() + ")"),
      callback_(nullptr),
      hw_decoded_frames_since_last_fallback_(0),
      hw_consequtive_generic_errors_(0) {}
VideoDecoderSoftwareFallbackWrapper::~VideoDecoderSoftwareFallbackWrapper() =
    default;

int32_t VideoDecoderSoftwareFallbackWrapper::InitDecode(
    const VideoCodec* codec_settings,
    int32_t number_of_cores) {
  codec_settings_ = *codec_settings;
  number_of_cores_ = number_of_cores;

  if (webrtc::field_trial::IsEnabled("WebRTC-Video-ForcedSwDecoderFallback")) {
    RTC_LOG(LS_INFO) << "Forced software decoder fallback enabled.";
    RTC_DCHECK(decoder_type_ == DecoderType::kNone);
    return InitFallbackDecoder() ? WEBRTC_VIDEO_CODEC_OK
                                 : WEBRTC_VIDEO_CODEC_ERROR;
  }
  int32_t status = InitHwDecoder();
  if (status == WEBRTC_VIDEO_CODEC_OK) {
    return WEBRTC_VIDEO_CODEC_OK;
  }

  RTC_DCHECK(decoder_type_ == DecoderType::kNone);
  if (InitFallbackDecoder()) {
    return WEBRTC_VIDEO_CODEC_OK;
  }

  return status;
}

int32_t VideoDecoderSoftwareFallbackWrapper::InitHwDecoder() {
  RTC_DCHECK(decoder_type_ == DecoderType::kNone);
  int32_t status = hw_decoder_->InitDecode(&codec_settings_, number_of_cores_);
  if (status != WEBRTC_VIDEO_CODEC_OK) {
    return status;
  }

  decoder_type_ = DecoderType::kHardware;
  if (callback_)
    hw_decoder_->RegisterDecodeCompleteCallback(callback_);
  return status;
}

bool VideoDecoderSoftwareFallbackWrapper::InitFallbackDecoder() {
  RTC_DCHECK(decoder_type_ == DecoderType::kNone ||
             decoder_type_ == DecoderType::kHardware);
  RTC_LOG(LS_WARNING) << "Decoder falling back to software decoding.";
  int32_t status =
      fallback_decoder_->InitDecode(&codec_settings_, number_of_cores_);
  if (status != WEBRTC_VIDEO_CODEC_OK) {
    RTC_LOG(LS_ERROR) << "Failed to initialize software-decoder fallback.";
    return false;
  }

  UpdateFallbackDecoderHistograms();

  if (decoder_type_ == DecoderType::kHardware) {
    hw_decoder_->Release();
  }
  decoder_type_ = DecoderType::kFallback;

  if (callback_)
    fallback_decoder_->RegisterDecodeCompleteCallback(callback_);
  return true;
}

void VideoDecoderSoftwareFallbackWrapper::UpdateFallbackDecoderHistograms() {
  const std::string kFallbackHistogramsUmaPrefix =
      "WebRTC.Video.HardwareDecodedFramesBetweenSoftwareFallbacks.";
  // Each histogram needs its own code path for this to work otherwise the
  // histogram names will be mixed up by the optimization that takes place.
  switch (codec_settings_.codecType) {
    case kVideoCodecGeneric:
      RTC_HISTOGRAM_COUNTS_100000(kFallbackHistogramsUmaPrefix + "Generic",
                                  hw_decoded_frames_since_last_fallback_);
      break;
    case kVideoCodecVP8:
      RTC_HISTOGRAM_COUNTS_100000(kFallbackHistogramsUmaPrefix + "Vp8",
                                  hw_decoded_frames_since_last_fallback_);
      break;
    case kVideoCodecVP9:
      RTC_HISTOGRAM_COUNTS_100000(kFallbackHistogramsUmaPrefix + "Vp9",
                                  hw_decoded_frames_since_last_fallback_);
      break;
    case kVideoCodecAV1:
      RTC_HISTOGRAM_COUNTS_100000(kFallbackHistogramsUmaPrefix + "Av1",
                                  hw_decoded_frames_since_last_fallback_);
      break;
    case kVideoCodecH264:
      RTC_HISTOGRAM_COUNTS_100000(kFallbackHistogramsUmaPrefix + "H264",
                                  hw_decoded_frames_since_last_fallback_);
      break;
    case kVideoCodecMultiplex:
      RTC_HISTOGRAM_COUNTS_100000(kFallbackHistogramsUmaPrefix + "Multiplex",
                                  hw_decoded_frames_since_last_fallback_);
      break;
  }
}

int32_t VideoDecoderSoftwareFallbackWrapper::Decode(
    const EncodedImage& input_image,
    bool missing_frames,
    int64_t render_time_ms) {
  TRACE_EVENT0("webrtc", "VideoDecoderSoftwareFallbackWrapper::Decode");
  switch (decoder_type_) {
    case DecoderType::kNone:
      return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
    case DecoderType::kHardware: {
      int32_t ret = WEBRTC_VIDEO_CODEC_FALLBACK_SOFTWARE;
      ret = hw_decoder_->Decode(input_image, missing_frames, render_time_ms);
      if (ret != WEBRTC_VIDEO_CODEC_FALLBACK_SOFTWARE) {
        if (ret != WEBRTC_VIDEO_CODEC_ERROR) {
          ++hw_decoded_frames_since_last_fallback_;
          hw_consequtive_generic_errors_ = 0;
          return ret;
        }
        if (input_image._frameType == VideoFrameType::kVideoFrameKey) {
          // Only count errors on key-frames, since generic errors can happen
          // with hw decoder due to many arbitrary reasons.
          // However, requesting a key-frame is supposed to fix the issue.
          ++hw_consequtive_generic_errors_;
        }
        if (hw_consequtive_generic_errors_ < kMaxConsequtiveHwErrors) {
          return ret;
        }
      }

      // HW decoder returned WEBRTC_VIDEO_CODEC_FALLBACK_SOFTWARE or
      // too many generic errors on key-frames encountered.
      if (!InitFallbackDecoder()) {
        return ret;
      }

      // Fallback decoder initialized, fall-through.
      ABSL_FALLTHROUGH_INTENDED;
    }
    case DecoderType::kFallback:
      return fallback_decoder_->Decode(input_image, missing_frames,
                                       render_time_ms);
    default:
      RTC_NOTREACHED();
      return WEBRTC_VIDEO_CODEC_ERROR;
  }
}

int32_t VideoDecoderSoftwareFallbackWrapper::RegisterDecodeCompleteCallback(
    DecodedImageCallback* callback) {
  callback_ = callback;
  return active_decoder().RegisterDecodeCompleteCallback(callback);
}

int32_t VideoDecoderSoftwareFallbackWrapper::Release() {
  int32_t status;
  switch (decoder_type_) {
    case DecoderType::kHardware:
      status = hw_decoder_->Release();
      break;
    case DecoderType::kFallback:
      RTC_LOG(LS_INFO) << "Releasing software fallback decoder.";
      status = fallback_decoder_->Release();
      break;
    case DecoderType::kNone:
      status = WEBRTC_VIDEO_CODEC_OK;
      break;
    default:
      RTC_NOTREACHED();
      status = WEBRTC_VIDEO_CODEC_ERROR;
  }

  decoder_type_ = DecoderType::kNone;
  return status;
}

VideoDecoder::DecoderInfo VideoDecoderSoftwareFallbackWrapper::GetDecoderInfo()
    const {
  DecoderInfo info = active_decoder().GetDecoderInfo();
  if (decoder_type_ == DecoderType::kFallback) {
    // Cached "A (fallback from B)" string.
    info.implementation_name = fallback_implementation_name_;
  }
  return info;
}

const char* VideoDecoderSoftwareFallbackWrapper::ImplementationName() const {
  if (decoder_type_ == DecoderType::kFallback) {
    // Cached "A (fallback from B)" string.
    return fallback_implementation_name_.c_str();
  } else {
    return hw_decoder_->ImplementationName();
  }
}

VideoDecoder& VideoDecoderSoftwareFallbackWrapper::active_decoder() const {
  return decoder_type_ == DecoderType::kFallback ? *fallback_decoder_
                                                 : *hw_decoder_;
}

}  // namespace

std::unique_ptr<VideoDecoder> CreateVideoDecoderSoftwareFallbackWrapper(
    std::unique_ptr<VideoDecoder> sw_fallback_decoder,
    std::unique_ptr<VideoDecoder> hw_decoder) {
  return std::make_unique<VideoDecoderSoftwareFallbackWrapper>(
      std::move(sw_fallback_decoder), std::move(hw_decoder));
}

}  // namespace webrtc
