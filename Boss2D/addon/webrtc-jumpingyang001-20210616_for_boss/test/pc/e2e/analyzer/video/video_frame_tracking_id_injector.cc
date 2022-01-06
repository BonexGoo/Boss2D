/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "test/pc/e2e/analyzer/video/video_frame_tracking_id_injector.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {
namespace webrtc_pc_e2e {

EncodedImage VideoFrameTrackingIdInjector::InjectData(
    uint16_t id,
    bool unused_discard,
    const EncodedImage& source) {
  RTC_CHECK(!unused_discard);
  EncodedImage out = source;
  out.SetVideoFrameTrackingId(id);
  return out;
}

EncodedImageExtractionResult VideoFrameTrackingIdInjector::ExtractData(
    const EncodedImage& source) {
  return EncodedImageExtractionResult{source.VideoFrameTrackingId().value_or(0),
                                      source, /*discard=*/false};
}

}  // namespace webrtc_pc_e2e
}  // namespace webrtc
