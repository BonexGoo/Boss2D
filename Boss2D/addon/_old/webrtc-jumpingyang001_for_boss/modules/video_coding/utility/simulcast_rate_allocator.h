/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_UTILITY_SIMULCAST_RATE_ALLOCATOR_H_
#define MODULES_VIDEO_CODING_UTILITY_SIMULCAST_RATE_ALLOCATOR_H_

#include <stdint.h>

#include <map>
#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__video__video_bitrate_allocator_h //original-code:"api/video/video_bitrate_allocator.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

class SimulcastRateAllocator : public VideoBitrateAllocator {
 public:
  explicit SimulcastRateAllocator(const VideoCodec& codec);

  VideoBitrateAllocation GetAllocation(uint32_t total_bitrate_bps,
                                       uint32_t framerate) override;
  const VideoCodec& GetCodec() const;

  static float GetTemporalRateAllocation(int num_layers, int temporal_id);

 private:
  void DistributeAllocationToSimulcastLayers(
      uint32_t total_bitrate_bps,
      VideoBitrateAllocation* allocated_bitrates_bps) const;
  void DistributeAllocationToTemporalLayers(
      uint32_t framerate,
      VideoBitrateAllocation* allocated_bitrates_bps) const;
  std::vector<uint32_t> DefaultTemporalLayerAllocation(int bitrate_kbps,
                                                       int max_bitrate_kbps,
                                                       int framerate,
                                                       int simulcast_id) const;
  std::vector<uint32_t> ScreenshareTemporalLayerAllocation(
      int bitrate_kbps,
      int max_bitrate_kbps,
      int framerate,
      int simulcast_id) const;
  int NumTemporalStreams(size_t simulcast_id) const;

  const VideoCodec codec_;

  RTC_DISALLOW_COPY_AND_ASSIGN(SimulcastRateAllocator);
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_UTILITY_SIMULCAST_RATE_ALLOCATOR_H_
