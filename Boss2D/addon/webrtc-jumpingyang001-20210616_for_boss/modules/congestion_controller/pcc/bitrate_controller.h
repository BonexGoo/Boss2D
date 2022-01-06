/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_CONGESTION_CONTROLLER_PCC_BITRATE_CONTROLLER_H_
#define MODULES_CONGESTION_CONTROLLER_PCC_BITRATE_CONTROLLER_H_

#include <stdint.h>

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__pcc__monitor_interval_h //original-code:"modules/congestion_controller/pcc/monitor_interval.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__pcc__utility_function_h //original-code:"modules/congestion_controller/pcc/utility_function.h"

namespace webrtc {
namespace pcc {

class PccBitrateController {
 public:
  PccBitrateController(double initial_conversion_factor,
                       double initial_dynamic_boundary,
                       double dynamic_boundary_increment,
                       double rtt_gradient_coefficient,
                       double loss_coefficient,
                       double throughput_coefficient,
                       double throughput_power,
                       double rtt_gradient_threshold,
                       double delay_gradient_negative_bound);

  PccBitrateController(
      double initial_conversion_factor,
      double initial_dynamic_boundary,
      double dynamic_boundary_increment,
      std::unique_ptr<PccUtilityFunctionInterface> utility_function);

  absl::optional<DataRate> ComputeRateUpdateForSlowStartMode(
      const PccMonitorInterval& monitor_interval);

  DataRate ComputeRateUpdateForOnlineLearningMode(
      const std::vector<PccMonitorInterval>& block,
      DataRate bandwidth_estimate);

  ~PccBitrateController();

 private:
  double ApplyDynamicBoundary(double rate_change, double bitrate);
  double ComputeStepSize(double utility_gradient);

  // Dynamic boundary variables:
  int64_t consecutive_boundary_adjustments_number_;
  const double initial_dynamic_boundary_;
  const double dynamic_boundary_increment_;

  const std::unique_ptr<PccUtilityFunctionInterface> utility_function_;
  // Step Size variables:
  int64_t step_size_adjustments_number_;
  const double initial_conversion_factor_;

  absl::optional<double> previous_utility_;
};

}  // namespace pcc
}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_PCC_BITRATE_CONTROLLER_H_
