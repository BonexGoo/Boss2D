/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_CONGESTION_CONTROLLER_GOOG_CC_GOOG_CC_NETWORK_CONTROL_H_
#define MODULES_CONGESTION_CONTROLLER_GOOG_CC_GOOG_CC_NETWORK_CONTROL_H_

#include <stdint.h>

#include <deque>
#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__network_state_predictor_h //original-code:"api/network_state_predictor.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_h //original-code:"api/rtc_event_log/rtc_event_log.h"
#include BOSS_WEBRTC_U_api__transport__field_trial_based_config_h //original-code:"api/transport/field_trial_based_config.h"
#include BOSS_WEBRTC_U_api__transport__network_control_h //original-code:"api/transport/network_control.h"
#include BOSS_WEBRTC_U_api__transport__network_types_h //original-code:"api/transport/network_types.h"
#include BOSS_WEBRTC_U_api__transport__webrtc_key_value_config_h //original-code:"api/transport/webrtc_key_value_config.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"
#include BOSS_WEBRTC_U_api__units__data_size_h //original-code:"api/units/data_size.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__acknowledged_bitrate_estimator_interface_h //original-code:"modules/congestion_controller/goog_cc/acknowledged_bitrate_estimator_interface.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__alr_detector_h //original-code:"modules/congestion_controller/goog_cc/alr_detector.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__congestion_window_pushback_controller_h //original-code:"modules/congestion_controller/goog_cc/congestion_window_pushback_controller.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__delay_based_bwe_h //original-code:"modules/congestion_controller/goog_cc/delay_based_bwe.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__probe_controller_h //original-code:"modules/congestion_controller/goog_cc/probe_controller.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__send_side_bandwidth_estimation_h //original-code:"modules/congestion_controller/goog_cc/send_side_bandwidth_estimation.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__field_trial_parser_h //original-code:"rtc_base/experiments/field_trial_parser.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__rate_control_settings_h //original-code:"rtc_base/experiments/rate_control_settings.h"

namespace webrtc {
struct GoogCcConfig {
  std::unique_ptr<NetworkStateEstimator> network_state_estimator = nullptr;
  std::unique_ptr<NetworkStatePredictor> network_state_predictor = nullptr;
  bool feedback_only = false;
};

class GoogCcNetworkController : public NetworkControllerInterface {
 public:
  GoogCcNetworkController(NetworkControllerConfig config,
                          GoogCcConfig goog_cc_config);

  GoogCcNetworkController() = delete;
  GoogCcNetworkController(const GoogCcNetworkController&) = delete;
  GoogCcNetworkController& operator=(const GoogCcNetworkController&) = delete;

  ~GoogCcNetworkController() override;

  // NetworkControllerInterface
  NetworkControlUpdate OnNetworkAvailability(NetworkAvailability msg) override;
  NetworkControlUpdate OnNetworkRouteChange(NetworkRouteChange msg) override;
  NetworkControlUpdate OnProcessInterval(ProcessInterval msg) override;
  NetworkControlUpdate OnRemoteBitrateReport(RemoteBitrateReport msg) override;
  NetworkControlUpdate OnRoundTripTimeUpdate(RoundTripTimeUpdate msg) override;
  NetworkControlUpdate OnSentPacket(SentPacket msg) override;
  NetworkControlUpdate OnReceivedPacket(ReceivedPacket msg) override;
  NetworkControlUpdate OnStreamsConfig(StreamsConfig msg) override;
  NetworkControlUpdate OnTargetRateConstraints(
      TargetRateConstraints msg) override;
  NetworkControlUpdate OnTransportLossReport(TransportLossReport msg) override;
  NetworkControlUpdate OnTransportPacketsFeedback(
      TransportPacketsFeedback msg) override;
  NetworkControlUpdate OnNetworkStateEstimate(
      NetworkStateEstimate msg) override;

  NetworkControlUpdate GetNetworkState(Timestamp at_time) const;

 private:
  friend class GoogCcStatePrinter;
  std::vector<ProbeClusterConfig> ResetConstraints(
      TargetRateConstraints new_constraints);
  void ClampConstraints();
  void MaybeTriggerOnNetworkChanged(NetworkControlUpdate* update,
                                    Timestamp at_time);
  void UpdateCongestionWindowSize();
  PacerConfig GetPacingRates(Timestamp at_time) const;
  const FieldTrialBasedConfig trial_based_config_;

  const WebRtcKeyValueConfig* const key_value_config_;
  RtcEventLog* const event_log_;
  const bool packet_feedback_only_;
  FieldTrialFlag safe_reset_on_route_change_;
  FieldTrialFlag safe_reset_acknowledged_rate_;
  const bool use_min_allocatable_as_lower_bound_;
  const bool ignore_probes_lower_than_network_estimate_;
  const bool limit_probes_lower_than_throughput_estimate_;
  const RateControlSettings rate_control_settings_;
  const bool loss_based_stable_rate_;

  const std::unique_ptr<ProbeController> probe_controller_;
  const std::unique_ptr<CongestionWindowPushbackController>
      congestion_window_pushback_controller_;

  std::unique_ptr<SendSideBandwidthEstimation> bandwidth_estimation_;
  std::unique_ptr<AlrDetector> alr_detector_;
  std::unique_ptr<ProbeBitrateEstimator> probe_bitrate_estimator_;
  std::unique_ptr<NetworkStateEstimator> network_estimator_;
  std::unique_ptr<NetworkStatePredictor> network_state_predictor_;
  std::unique_ptr<DelayBasedBwe> delay_based_bwe_;
  std::unique_ptr<AcknowledgedBitrateEstimatorInterface>
      acknowledged_bitrate_estimator_;

  absl::optional<NetworkControllerConfig> initial_config_;

  DataRate min_target_rate_ = DataRate::Zero();
  DataRate min_data_rate_ = DataRate::Zero();
  DataRate max_data_rate_ = DataRate::PlusInfinity();
  absl::optional<DataRate> starting_rate_;

  bool first_packet_sent_ = false;

  absl::optional<NetworkStateEstimate> estimate_;

  Timestamp next_loss_update_ = Timestamp::MinusInfinity();
  int lost_packets_since_last_loss_update_ = 0;
  int expected_packets_since_last_loss_update_ = 0;

  std::deque<int64_t> feedback_max_rtts_;

  DataRate last_loss_based_target_rate_;
  DataRate last_pushback_target_rate_;
  DataRate last_stable_target_rate_;

  absl::optional<uint8_t> last_estimated_fraction_loss_ = 0;
  TimeDelta last_estimated_round_trip_time_ = TimeDelta::PlusInfinity();
  Timestamp last_packet_received_time_ = Timestamp::MinusInfinity();

  double pacing_factor_;
  DataRate min_total_allocated_bitrate_;
  DataRate max_padding_rate_;
  DataRate max_total_allocated_bitrate_;

  bool previously_in_alr_ = false;

  absl::optional<DataSize> current_data_window_;
};

}  // namespace webrtc

#endif  // MODULES_CONGESTION_CONTROLLER_GOOG_CC_GOOG_CC_NETWORK_CONTROL_H_
