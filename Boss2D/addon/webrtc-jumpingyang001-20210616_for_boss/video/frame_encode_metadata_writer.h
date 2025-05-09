/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VIDEO_FRAME_ENCODE_METADATA_WRITER_H_
#define VIDEO_FRAME_ENCODE_METADATA_WRITER_H_

#include <list>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"

namespace webrtc {

class FrameEncodeMetadataWriter {
 public:
  explicit FrameEncodeMetadataWriter(EncodedImageCallback* frame_drop_callback);
  ~FrameEncodeMetadataWriter();

  void OnEncoderInit(const VideoCodec& codec, bool internal_source);
  void OnSetRates(const VideoBitrateAllocation& bitrate_allocation,
                  uint32_t framerate_fps);

  void OnEncodeStarted(const VideoFrame& frame);

  void FillTimingInfo(size_t simulcast_svc_idx, EncodedImage* encoded_image);

  void UpdateBitstream(const CodecSpecificInfo* codec_specific_info,
                       EncodedImage* encoded_image);

  void Reset();

 private:
  size_t NumSpatialLayers() const RTC_EXCLUSIVE_LOCKS_REQUIRED(lock_);

  // For non-internal-source encoders, returns encode started time and fixes
  // capture timestamp for the frame, if corrupted by the encoder.
  absl::optional<int64_t> ExtractEncodeStartTimeAndFillMetadata(
      size_t simulcast_svc_idx,
      EncodedImage* encoded_image) RTC_EXCLUSIVE_LOCKS_REQUIRED(lock_);

  struct FrameMetadata {
    uint32_t rtp_timestamp;
    int64_t encode_start_time_ms;
    int64_t ntp_time_ms = 0;
    int64_t timestamp_us = 0;
    VideoRotation rotation = kVideoRotation_0;
    absl::optional<ColorSpace> color_space;
    RtpPacketInfos packet_infos;
  };
  struct TimingFramesLayerInfo {
    TimingFramesLayerInfo();
    ~TimingFramesLayerInfo();
    size_t target_bitrate_bytes_per_sec = 0;
    std::list<FrameMetadata> frames;
  };

  Mutex lock_;
  EncodedImageCallback* const frame_drop_callback_;
  VideoCodec codec_settings_ RTC_GUARDED_BY(&lock_);
  bool internal_source_ RTC_GUARDED_BY(&lock_);
  uint32_t framerate_fps_ RTC_GUARDED_BY(&lock_);

  // Separate instance for each simulcast stream or spatial layer.
  std::vector<TimingFramesLayerInfo> timing_frames_info_ RTC_GUARDED_BY(&lock_);
  int64_t last_timing_frame_time_ms_ RTC_GUARDED_BY(&lock_);
  size_t reordered_frames_logged_messages_ RTC_GUARDED_BY(&lock_);
  size_t stalled_encoder_logged_messages_ RTC_GUARDED_BY(&lock_);
};

}  // namespace webrtc

#endif  // VIDEO_FRAME_ENCODE_METADATA_WRITER_H_
