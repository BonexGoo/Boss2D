/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_api__audio__echo_detector_creator_h //original-code:"api/audio/echo_detector_creator.h"

#include BOSS_WEBRTC_U_modules__audio_processing__residual_echo_detector_h //original-code:"modules/audio_processing/residual_echo_detector.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"

namespace webrtc {

rtc::scoped_refptr<EchoDetector> CreateEchoDetector() {
  return rtc::make_ref_counted<ResidualEchoDetector>();
}

}  // namespace webrtc
