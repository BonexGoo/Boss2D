/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_VP8_SIMULCAST_RATE_ALLOCATOR_H_
#define MODULES_VIDEO_CODING_CODECS_VP8_SIMULCAST_RATE_ALLOCATOR_H_

#include BOSS_FAKEWIN_V_stdint_h //original-code:<stdint.h>

#include <map>
#include <memory>

#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_common_video__include__video_bitrate_allocator_h //original-code:"common_video/include/video_bitrate_allocator.h"
#include "modules/video_coding/codecs/vp8/temporal_layers.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

class SimulcastRateAllocator : public VideoBitrateAllocator,
                               public TemporalLayersListener {
 public:
  explicit SimulcastRateAllocator(
      const VideoCodec& codec,
      std::unique_ptr<TemporalLayersFactory> tl_factory);

  void OnTemporalLayersCreated(int simulcast_id,
                               TemporalLayers* layers) override;

  BitrateAllocation GetAllocation(uint32_t total_bitrate_bps,
                                  uint32_t framerate) override;
  uint32_t GetPreferredBitrateBps(uint32_t framerate) override;
  const VideoCodec& GetCodec() const;

 private:
  const VideoCodec codec_;
  std::map<uint32_t, TemporalLayers*> temporal_layers_;
  std::unique_ptr<TemporalLayersFactory> tl_factory_;

  RTC_DISALLOW_COPY_AND_ASSIGN(SimulcastRateAllocator);
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_VP8_SIMULCAST_RATE_ALLOCATOR_H_
