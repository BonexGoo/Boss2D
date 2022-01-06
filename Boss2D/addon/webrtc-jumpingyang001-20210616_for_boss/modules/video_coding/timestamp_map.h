/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_TIMESTAMP_MAP_H_
#define MODULES_VIDEO_CODING_TIMESTAMP_MAP_H_

#include <memory>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__rtp_packet_infos_h //original-code:"api/rtp_packet_infos.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_api__video__video_content_type_h //original-code:"api/video/video_content_type.h"
#include BOSS_WEBRTC_U_api__video__video_rotation_h //original-code:"api/video/video_rotation.h"
#include BOSS_WEBRTC_U_api__video__video_timing_h //original-code:"api/video/video_timing.h"

namespace webrtc {

struct VCMFrameInformation {
  int64_t renderTimeMs;
  absl::optional<Timestamp> decodeStart;
  void* userData;
  VideoRotation rotation;
  VideoContentType content_type;
  EncodedImage::Timing timing;
  int64_t ntp_time_ms;
  RtpPacketInfos packet_infos;
  // ColorSpace is not stored here, as it might be modified by decoders.
};

class VCMTimestampMap {
 public:
  explicit VCMTimestampMap(size_t capacity);
  ~VCMTimestampMap();

  void Add(uint32_t timestamp, const VCMFrameInformation& data);
  absl::optional<VCMFrameInformation> Pop(uint32_t timestamp);
  size_t Size() const;
  void Clear();

 private:
  struct TimestampDataTuple {
    uint32_t timestamp;
    VCMFrameInformation data;
  };
  bool IsEmpty() const;

  std::unique_ptr<TimestampDataTuple[]> ring_buffer_;
  const size_t capacity_;
  size_t next_add_idx_;
  size_t next_pop_idx_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_TIMESTAMP_MAP_H_
