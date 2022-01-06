/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_VIDEO_CODING_SVC_SCALABLE_VIDEO_CONTROLLER_NO_LAYERING_H_
#define MODULES_VIDEO_CODING_SVC_SCALABLE_VIDEO_CONTROLLER_NO_LAYERING_H_

#include <vector>

#include BOSS_WEBRTC_U_api__transport__rtp__dependency_descriptor_h //original-code:"api/transport/rtp/dependency_descriptor.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_common_video__generic_frame_descriptor__generic_frame_info_h //original-code:"common_video/generic_frame_descriptor/generic_frame_info.h"
#include BOSS_WEBRTC_U_modules__video_coding__svc__scalable_video_controller_h //original-code:"modules/video_coding/svc/scalable_video_controller.h"

namespace webrtc {

class ScalableVideoControllerNoLayering : public ScalableVideoController {
 public:
  ~ScalableVideoControllerNoLayering() override;

  StreamLayersConfig StreamConfig() const override;
  FrameDependencyStructure DependencyStructure() const override;

  std::vector<LayerFrameConfig> NextFrameConfig(bool restart) override;
  GenericFrameInfo OnEncodeDone(const LayerFrameConfig& config) override;
  void OnRatesUpdated(const VideoBitrateAllocation& bitrates) override;

 private:
  bool start_ = true;
  bool enabled_ = true;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_SVC_SCALABLE_VIDEO_CONTROLLER_NO_LAYERING_H_
