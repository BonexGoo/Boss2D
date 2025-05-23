/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_video__frame_encode_metadata_writer_h //original-code:"video/frame_encode_metadata_writer.h"

#include <algorithm>
#include <utility>

#include BOSS_WEBRTC_U_common_video__h264__sps_vui_rewriter_h //original-code:"common_video/h264/sps_vui_rewriter.h"
#include BOSS_WEBRTC_U_modules__include__module_common_types_public_h //original-code:"modules/include/module_common_types_public.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_defines_h //original-code:"modules/video_coding/include/video_coding_defines.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__time_utils_h //original-code:"rtc_base/time_utils.h"

namespace webrtc {
namespace {
const int kMessagesThrottlingThreshold = 2;
const int kThrottleRatio = 100000;

class EncodedImageBufferWrapper : public EncodedImageBufferInterface {
 public:
  explicit EncodedImageBufferWrapper(rtc::Buffer&& buffer)
      : buffer_(std::move(buffer)) {}

  const uint8_t* data() const override { return buffer_.data(); }
  uint8_t* data() override { return buffer_.data(); }
  size_t size() const override { return buffer_.size(); }

 private:
  rtc::Buffer buffer_;
};

}  // namespace

FrameEncodeMetadataWriter::TimingFramesLayerInfo::TimingFramesLayerInfo() =
    default;
FrameEncodeMetadataWriter::TimingFramesLayerInfo::~TimingFramesLayerInfo() =
    default;

FrameEncodeMetadataWriter::FrameEncodeMetadataWriter(
    EncodedImageCallback* frame_drop_callback)
    : frame_drop_callback_(frame_drop_callback),
      internal_source_(false),
      framerate_fps_(0),
      last_timing_frame_time_ms_(-1),
      reordered_frames_logged_messages_(0),
      stalled_encoder_logged_messages_(0) {
  codec_settings_.timing_frame_thresholds = {-1, 0};
}
FrameEncodeMetadataWriter::~FrameEncodeMetadataWriter() {}

void FrameEncodeMetadataWriter::OnEncoderInit(const VideoCodec& codec,
                                              bool internal_source) {
  MutexLock lock(&lock_);
  codec_settings_ = codec;
  internal_source_ = internal_source;
}

void FrameEncodeMetadataWriter::OnSetRates(
    const VideoBitrateAllocation& bitrate_allocation,
    uint32_t framerate_fps) {
  MutexLock lock(&lock_);
  framerate_fps_ = framerate_fps;
  const size_t num_spatial_layers = NumSpatialLayers();
  if (timing_frames_info_.size() < num_spatial_layers) {
    timing_frames_info_.resize(num_spatial_layers);
  }
  for (size_t i = 0; i < num_spatial_layers; ++i) {
    timing_frames_info_[i].target_bitrate_bytes_per_sec =
        bitrate_allocation.GetSpatialLayerSum(i) / 8;
  }
}

void FrameEncodeMetadataWriter::OnEncodeStarted(const VideoFrame& frame) {
  MutexLock lock(&lock_);
  if (internal_source_) {
    return;
  }

  const size_t num_spatial_layers = NumSpatialLayers();
  timing_frames_info_.resize(num_spatial_layers);
  FrameMetadata metadata;
  metadata.rtp_timestamp = frame.timestamp();
  metadata.encode_start_time_ms = rtc::TimeMillis();
  metadata.ntp_time_ms = frame.ntp_time_ms();
  metadata.timestamp_us = frame.timestamp_us();
  metadata.rotation = frame.rotation();
  metadata.color_space = frame.color_space();
  metadata.packet_infos = frame.packet_infos();
  for (size_t si = 0; si < num_spatial_layers; ++si) {
    RTC_DCHECK(timing_frames_info_[si].frames.empty() ||
               rtc::TimeDiff(
                   frame.render_time_ms(),
                   timing_frames_info_[si].frames.back().timestamp_us / 1000) >=
                   0);
    // If stream is disabled due to low bandwidth OnEncodeStarted still will be
    // called and have to be ignored.
    if (timing_frames_info_[si].target_bitrate_bytes_per_sec == 0)
      continue;
    if (timing_frames_info_[si].frames.size() == kMaxEncodeStartTimeListSize) {
      ++stalled_encoder_logged_messages_;
      if (stalled_encoder_logged_messages_ <= kMessagesThrottlingThreshold ||
          stalled_encoder_logged_messages_ % kThrottleRatio == 0) {
        RTC_LOG(LS_WARNING) << "Too many frames in the encode_start_list."
                               " Did encoder stall?";
        if (stalled_encoder_logged_messages_ == kMessagesThrottlingThreshold) {
          RTC_LOG(LS_WARNING)
              << "Too many log messages. Further stalled encoder"
                 "warnings will be throttled.";
        }
      }
      frame_drop_callback_->OnDroppedFrame(
          EncodedImageCallback::DropReason::kDroppedByEncoder);
      timing_frames_info_[si].frames.pop_front();
    }
    timing_frames_info_[si].frames.emplace_back(metadata);
  }
}

void FrameEncodeMetadataWriter::FillTimingInfo(size_t simulcast_svc_idx,
                                               EncodedImage* encoded_image) {
  MutexLock lock(&lock_);
  absl::optional<size_t> outlier_frame_size;
  absl::optional<int64_t> encode_start_ms;
  uint8_t timing_flags = VideoSendTiming::kNotTriggered;

  int64_t encode_done_ms = rtc::TimeMillis();

  // Encoders with internal sources do not call OnEncodeStarted
  // |timing_frames_info_| may be not filled here.
  if (!internal_source_) {
    encode_start_ms =
        ExtractEncodeStartTimeAndFillMetadata(simulcast_svc_idx, encoded_image);
  }

  if (timing_frames_info_.size() > simulcast_svc_idx) {
    size_t target_bitrate =
        timing_frames_info_[simulcast_svc_idx].target_bitrate_bytes_per_sec;
    if (framerate_fps_ > 0 && target_bitrate > 0) {
      // framerate and target bitrate were reported by encoder.
      size_t average_frame_size = target_bitrate / framerate_fps_;
      outlier_frame_size.emplace(
          average_frame_size *
          codec_settings_.timing_frame_thresholds.outlier_ratio_percent / 100);
    }
  }

  // Outliers trigger timing frames, but do not affect scheduled timing
  // frames.
  if (outlier_frame_size && encoded_image->size() >= *outlier_frame_size) {
    timing_flags |= VideoSendTiming::kTriggeredBySize;
  }

  // Check if it's time to send a timing frame.
  int64_t timing_frame_delay_ms =
      encoded_image->capture_time_ms_ - last_timing_frame_time_ms_;
  // Trigger threshold if it's a first frame, too long passed since the last
  // timing frame, or we already sent timing frame on a different simulcast
  // stream with the same capture time.
  if (last_timing_frame_time_ms_ == -1 ||
      timing_frame_delay_ms >=
          codec_settings_.timing_frame_thresholds.delay_ms ||
      timing_frame_delay_ms == 0) {
    timing_flags |= VideoSendTiming::kTriggeredByTimer;
    last_timing_frame_time_ms_ = encoded_image->capture_time_ms_;
  }

  // Workaround for chromoting encoder: it passes encode start and finished
  // timestamps in |timing_| field, but they (together with capture timestamp)
  // are not in the WebRTC clock.
  if (internal_source_ && encoded_image->timing_.encode_finish_ms > 0 &&
      encoded_image->timing_.encode_start_ms > 0) {
    int64_t clock_offset_ms =
        encode_done_ms - encoded_image->timing_.encode_finish_ms;
    // Translate capture timestamp to local WebRTC clock.
    encoded_image->capture_time_ms_ += clock_offset_ms;
    encoded_image->SetTimestamp(
        static_cast<uint32_t>(encoded_image->capture_time_ms_ * 90));
    encode_start_ms.emplace(encoded_image->timing_.encode_start_ms +
                            clock_offset_ms);
  }

  // If encode start is not available that means that encoder uses internal
  // source. In that case capture timestamp may be from a different clock with a
  // drift relative to rtc::TimeMillis(). We can't use it for Timing frames,
  // because to being sent in the network capture time required to be less than
  // all the other timestamps.
  if (encode_start_ms) {
    encoded_image->SetEncodeTime(*encode_start_ms, encode_done_ms);
    encoded_image->timing_.flags = timing_flags;
  } else {
    encoded_image->timing_.flags = VideoSendTiming::kInvalid;
  }
}

void FrameEncodeMetadataWriter::UpdateBitstream(
    const CodecSpecificInfo* codec_specific_info,
    EncodedImage* encoded_image) {
  if (!codec_specific_info ||
      codec_specific_info->codecType != kVideoCodecH264 ||
      encoded_image->_frameType != VideoFrameType::kVideoFrameKey) {
    return;
  }

  // Make sure that the data is not copied if owned by EncodedImage.
  const EncodedImage& buffer = *encoded_image;
  rtc::Buffer modified_buffer =
      SpsVuiRewriter::ParseOutgoingBitstreamAndRewrite(
          buffer, encoded_image->ColorSpace());

  encoded_image->SetEncodedData(
      rtc::make_ref_counted<EncodedImageBufferWrapper>(
          std::move(modified_buffer)));
}

void FrameEncodeMetadataWriter::Reset() {
  MutexLock lock(&lock_);
  for (auto& info : timing_frames_info_) {
    info.frames.clear();
  }
  last_timing_frame_time_ms_ = -1;
  reordered_frames_logged_messages_ = 0;
  stalled_encoder_logged_messages_ = 0;
}

absl::optional<int64_t>
FrameEncodeMetadataWriter::ExtractEncodeStartTimeAndFillMetadata(
    size_t simulcast_svc_idx,
    EncodedImage* encoded_image) {
  absl::optional<int64_t> result;
  size_t num_simulcast_svc_streams = timing_frames_info_.size();
  if (simulcast_svc_idx < num_simulcast_svc_streams) {
    auto metadata_list = &timing_frames_info_[simulcast_svc_idx].frames;
    // Skip frames for which there was OnEncodeStarted but no OnEncodedImage
    // call. These are dropped by encoder internally.
    // Because some hardware encoders don't preserve capture timestamp we
    // use RTP timestamps here.
    while (!metadata_list->empty() &&
           IsNewerTimestamp(encoded_image->Timestamp(),
                            metadata_list->front().rtp_timestamp)) {
      frame_drop_callback_->OnDroppedFrame(
          EncodedImageCallback::DropReason::kDroppedByEncoder);
      metadata_list->pop_front();
    }

    encoded_image->content_type_ =
        (codec_settings_.mode == VideoCodecMode::kScreensharing)
            ? VideoContentType::SCREENSHARE
            : VideoContentType::UNSPECIFIED;

    if (!metadata_list->empty() &&
        metadata_list->front().rtp_timestamp == encoded_image->Timestamp()) {
      result.emplace(metadata_list->front().encode_start_time_ms);
      encoded_image->capture_time_ms_ =
          metadata_list->front().timestamp_us / 1000;
      encoded_image->ntp_time_ms_ = metadata_list->front().ntp_time_ms;
      encoded_image->rotation_ = metadata_list->front().rotation;
      encoded_image->SetColorSpace(metadata_list->front().color_space);
      encoded_image->SetPacketInfos(metadata_list->front().packet_infos);
      metadata_list->pop_front();
    } else {
      ++reordered_frames_logged_messages_;
      if (reordered_frames_logged_messages_ <= kMessagesThrottlingThreshold ||
          reordered_frames_logged_messages_ % kThrottleRatio == 0) {
        RTC_LOG(LS_WARNING) << "Frame with no encode started time recordings. "
                               "Encoder may be reordering frames "
                               "or not preserving RTP timestamps.";
        if (reordered_frames_logged_messages_ == kMessagesThrottlingThreshold) {
          RTC_LOG(LS_WARNING) << "Too many log messages. Further frames "
                                 "reordering warnings will be throttled.";
        }
      }
    }
  }
  return result;
}

size_t FrameEncodeMetadataWriter::NumSpatialLayers() const {
  size_t num_spatial_layers = codec_settings_.numberOfSimulcastStreams;
  if (codec_settings_.codecType == kVideoCodecVP9) {
    num_spatial_layers = std::max(
        num_spatial_layers,
        static_cast<size_t>(codec_settings_.VP9().numberOfSpatialLayers));
  }
  return std::max(num_spatial_layers, size_t{1});
}

}  // namespace webrtc
