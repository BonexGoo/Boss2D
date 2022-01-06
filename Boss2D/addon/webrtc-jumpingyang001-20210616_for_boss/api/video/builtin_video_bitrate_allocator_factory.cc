/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__video__builtin_video_bitrate_allocator_factory_h //original-code:"api/video/builtin_video_bitrate_allocator_factory.h"

#include <memory>

#include BOSS_ABSEILCPP_U_absl__base__macros_h //original-code:"absl/base/macros.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocator_h //original-code:"api/video/video_bitrate_allocator.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_modules__video_coding__svc__svc_rate_allocator_h //original-code:"modules/video_coding/svc/svc_rate_allocator.h"
#include BOSS_WEBRTC_U_modules__video_coding__utility__simulcast_rate_allocator_h //original-code:"modules/video_coding/utility/simulcast_rate_allocator.h"

namespace webrtc {

namespace {

class BuiltinVideoBitrateAllocatorFactory
    : public VideoBitrateAllocatorFactory {
 public:
  BuiltinVideoBitrateAllocatorFactory() = default;
  ~BuiltinVideoBitrateAllocatorFactory() override = default;

  std::unique_ptr<VideoBitrateAllocator> CreateVideoBitrateAllocator(
      const VideoCodec& codec) override {
    switch (codec.codecType) {
      case kVideoCodecAV1:
      case kVideoCodecVP9:
        return std::make_unique<SvcRateAllocator>(codec);
      default:
        return std::make_unique<SimulcastRateAllocator>(codec);
    }
  }
};

}  // namespace

std::unique_ptr<VideoBitrateAllocatorFactory>
CreateBuiltinVideoBitrateAllocatorFactory() {
  return std::make_unique<BuiltinVideoBitrateAllocatorFactory>();
}

}  // namespace webrtc
