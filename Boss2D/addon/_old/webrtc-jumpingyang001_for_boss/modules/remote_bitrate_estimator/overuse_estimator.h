/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_REMOTE_BITRATE_ESTIMATOR_OVERUSE_ESTIMATOR_H_
#define MODULES_REMOTE_BITRATE_ESTIMATOR_OVERUSE_ESTIMATOR_H_

#include <deque>

#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__bwe_defines_h //original-code:"modules/remote_bitrate_estimator/include/bwe_defines.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

class OveruseEstimator {
 public:
  explicit OveruseEstimator(const OverUseDetectorOptions& options);
  ~OveruseEstimator();

  // Update the estimator with a new sample. The deltas should represent deltas
  // between timestamp groups as defined by the InterArrival class.
  // |current_hypothesis| should be the hypothesis of the over-use detector at
  // this time.
  void Update(int64_t t_delta,
              double ts_delta,
              int size_delta,
              BandwidthUsage current_hypothesis,
              int64_t now_ms);

  // Returns the estimated noise/jitter variance in ms^2.
  double var_noise() const { return var_noise_; }

  // Returns the estimated inter-arrival time delta offset in ms.
  double offset() const { return offset_; }

  // Returns the number of deltas which the current over-use estimator state is
  // based on.
  unsigned int num_of_deltas() const { return num_of_deltas_; }

 private:
  double UpdateMinFramePeriod(double ts_delta);
  void UpdateNoiseEstimate(double residual, double ts_delta, bool stable_state);

  // Must be first member variable. Cannot be const because we need to be
  // copyable.
  OverUseDetectorOptions options_;
  uint16_t num_of_deltas_;
  double slope_;
  double offset_;
  double prev_offset_;
  double E_[2][2];
  double process_noise_[2];
  double avg_noise_;
  double var_noise_;
  std::deque<double> ts_delta_hist_;

  RTC_DISALLOW_COPY_AND_ASSIGN(OveruseEstimator);
};
}  // namespace webrtc

#endif  // MODULES_REMOTE_BITRATE_ESTIMATOR_OVERUSE_ESTIMATOR_H_
