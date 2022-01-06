/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_ACKNOWLEDGED_BITRATE_ESTIMATOR_H_
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_ACKNOWLEDGED_BITRATE_ESTIMATOR_H_

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__transport__network_types_h //original-code:"api/transport/network_types.h"
#include BOSS_WEBRTC_U_api__transport__webrtc_key_value_config_h //original-code:"api/transport/webrtc_key_value_config.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__acknowledged_bitrate_estimator_interface_h //original-code:"modules/congestion_controller/goog_cc/acknowledged_bitrate_estimator_interface.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__bitrate_estimator_h //original-code:"modules/congestion_controller/goog_cc/bitrate_estimator.h"

namespace webrtc {

class AcknowledgedBitrateEstimator
    : public AcknowledgedBitrateEstimatorInterface {
 public:
  AcknowledgedBitrateEstimator(
      const WebRtcKeyValueConfig* key_value_config,
      std::unique_ptr<BitrateEstimator> bitrate_estimator);

  explicit AcknowledgedBitrateEstimator(
      const WebRtcKeyValueConfig* key_value_config);
  ~AcknowledgedBitrateEstimator() override;

  void IncomingPacketFeedbackVector(
      const std::vector<PacketResult>& packet_feedback_vector) override;
  absl::optional<DataRate> bitrate() const override;
  absl::optional<DataRate> PeekRate() const override;
  void SetAlr(bool in_alr) override;
  void SetAlrEndedTime(Timestamp alr_ended_time) override;

 private:
  absl::optional<Timestamp> alr_ended_time_;
  bool in_alr_;
  std::unique_ptr<BitrateEstimator> bitrate_estimator_;
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_ACKNOWLEDGED_BITRATE_ESTIMATOR_H_
