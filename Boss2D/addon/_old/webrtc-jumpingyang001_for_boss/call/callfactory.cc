/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_call__callfactory_h //original-code:"call/callfactory.h"

#include <memory>
#include <string>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_call__call_h //original-code:"call/call.h"
#include BOSS_WEBRTC_U_call__degraded_call_h //original-code:"call/degraded_call.h"
#include BOSS_WEBRTC_U_call__fake_network_pipe_h //original-code:"call/fake_network_pipe.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"

namespace webrtc {
namespace {
bool ParseConfigParam(std::string exp_name, int* field) {
  std::string group = field_trial::FindFullName(exp_name);
  if (group == "")
    return false;

  return (sscanf(group.c_str(), "%d", field) == 1);
}

absl::optional<webrtc::FakeNetworkPipe::Config> ParseDegradationConfig(
    bool send) {
  std::string exp_prefix = "WebRTCFakeNetwork";
  if (send) {
    exp_prefix += "Send";
  } else {
    exp_prefix += "Receive";
  }

  webrtc::FakeNetworkPipe::Config config;
  bool configured = false;
  configured |=
      ParseConfigParam(exp_prefix + "DelayMs", &config.queue_delay_ms);
  configured |= ParseConfigParam(exp_prefix + "DelayStdDevMs",
                                 &config.delay_standard_deviation_ms);
  int queue_length = 0;
  if (ParseConfigParam(exp_prefix + "QueueLength", &queue_length)) {
    RTC_CHECK_GE(queue_length, 0);
    config.queue_length_packets = queue_length;
    configured = true;
  }
  configured |=
      ParseConfigParam(exp_prefix + "CapacityKbps", &config.link_capacity_kbps);
  configured |=
      ParseConfigParam(exp_prefix + "LossPercent", &config.loss_percent);
  int allow_reordering = 0;
  if (ParseConfigParam(exp_prefix + "AllowReordering", &allow_reordering)) {
    config.allow_reordering = true;
    configured = true;
  }
  configured |= ParseConfigParam(exp_prefix + "AvgBurstLossLength",
                                 &config.avg_burst_loss_length);
  return configured ? absl::optional<webrtc::FakeNetworkPipe::Config>(config)
                    : absl::nullopt;
}
}  // namespace

Call* CallFactory::CreateCall(const Call::Config& config) {
  absl::optional<webrtc::FakeNetworkPipe::Config> send_degradation_config =
      ParseDegradationConfig(true);
  absl::optional<webrtc::FakeNetworkPipe::Config> receive_degradation_config =
      ParseDegradationConfig(false);

  if (send_degradation_config || receive_degradation_config) {
    return new DegradedCall(std::unique_ptr<Call>(Call::Create(config)),
                            send_degradation_config,
                            receive_degradation_config);
  }

  return Call::Create(config);
}

std::unique_ptr<CallFactoryInterface> CreateCallFactory() {
  return std::unique_ptr<CallFactoryInterface>(new CallFactory());
}

}  // namespace webrtc
