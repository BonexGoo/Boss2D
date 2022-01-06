/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__agc2_testing_common_h //original-code:"modules/audio_processing/agc2/agc2_testing_common.h"

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

namespace test {

std::vector<double> LinSpace(const double l,
                             const double r,
                             size_t num_points) {
  RTC_CHECK(num_points >= 2);
  std::vector<double> points(num_points);
  const double step = (r - l) / (num_points - 1.0);
  points[0] = l;
  for (size_t i = 1; i < num_points - 1; i++) {
    points[i] = static_cast<double>(l) + i * step;
  }
  points[num_points - 1] = r;
  return points;
}
}  // namespace test
}  // namespace webrtc
