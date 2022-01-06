// auther BOSS

/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_p2p__base__regatheringcontroller_h //original-code:"p2p/base/regatheringcontroller.h"

namespace webrtc {

using BasicRegatheringController_Config_BOSS = BasicRegatheringController::Config; // modified by BOSS, original-code: Config, replace-code: BasicRegatheringController_Config_BOSS

BasicRegatheringController_Config_BOSS::Config(const absl::optional<rtc::IntervalRange>& // modified by BOSS, original-code: Config, replace-code: BasicRegatheringController_Config_BOSS
                   regather_on_all_networks_interval_range,
               int regather_on_failed_networks_interval)
    : regather_on_all_networks_interval_range(
          regather_on_all_networks_interval_range),
      regather_on_failed_networks_interval(
          regather_on_failed_networks_interval) {}

BasicRegatheringController_Config_BOSS::Config(const BasicRegatheringController_Config_BOSS& other) = default; // modified by BOSS, original-code: Config, replace-code: BasicRegatheringController_Config_BOSS

BasicRegatheringController_Config_BOSS::~BasicRegatheringController_Config_BOSS() = default; // modified by BOSS, original-code: Config, replace-code: BasicRegatheringController_Config_BOSS
BasicRegatheringController_Config_BOSS& BasicRegatheringController_Config_BOSS::operator=(const BasicRegatheringController_Config_BOSS& other) = default; // modified by BOSS, original-code: Config, replace-code: BasicRegatheringController_Config_BOSS

BasicRegatheringController::BasicRegatheringController(
    const BasicRegatheringController_Config_BOSS& config, // modified by BOSS, original-code: Config, replace-code: BasicRegatheringController_Config_BOSS
    cricket::IceTransportInternal* ice_transport,
    rtc::Thread* thread)
    : config_(config),
      ice_transport_(ice_transport),
      thread_(thread),
      rand_(rtc::SystemTimeNanos()) {
  RTC_DCHECK(ice_transport_);
  RTC_DCHECK(thread_);
  ice_transport_->SignalStateChanged.connect(
      this, &BasicRegatheringController::OnIceTransportStateChanged);
  ice_transport->SignalWritableState.connect(
      this, &BasicRegatheringController::OnIceTransportWritableState);
  ice_transport->SignalReceivingState.connect(
      this, &BasicRegatheringController::OnIceTransportReceivingState);
  ice_transport->SignalNetworkRouteChanged.connect(
      this, &BasicRegatheringController::OnIceTransportNetworkRouteChanged);
}

BasicRegatheringController::~BasicRegatheringController() = default;

void BasicRegatheringController::Start() {
  ScheduleRecurringRegatheringOnFailedNetworks();
  if (config_.regather_on_all_networks_interval_range) {
    ScheduleRecurringRegatheringOnAllNetworks();
  }
}

void BasicRegatheringController::SetConfig(const BasicRegatheringController_Config_BOSS& config) { // modified by BOSS, original-code: Config, replace-code: BasicRegatheringController_Config_BOSS
  bool need_cancel_on_all_networks =
      has_recurring_schedule_on_all_networks_ &&
      (config_.regather_on_all_networks_interval_range !=
       config.regather_on_all_networks_interval_range);
  bool need_reschedule_on_all_networks =
      config.regather_on_all_networks_interval_range &&
      (config_.regather_on_all_networks_interval_range !=
       config.regather_on_all_networks_interval_range);
  bool need_cancel_and_reschedule_on_failed_networks =
      has_recurring_schedule_on_failed_networks_ &&
      (config_.regather_on_failed_networks_interval !=
       config.regather_on_failed_networks_interval);
  config_ = config;
  if (need_cancel_on_all_networks) {
    CancelScheduledRecurringRegatheringOnAllNetworks();
  }
  if (need_reschedule_on_all_networks) {
    ScheduleRecurringRegatheringOnAllNetworks();
  }
  if (need_cancel_and_reschedule_on_failed_networks) {
    CancelScheduledRecurringRegatheringOnFailedNetworks();
    ScheduleRecurringRegatheringOnFailedNetworks();
  }
}

void BasicRegatheringController::ScheduleRecurringRegatheringOnAllNetworks() {
  RTC_DCHECK(config_.regather_on_all_networks_interval_range &&
             config_.regather_on_all_networks_interval_range.value().min() >=
                 0);
  int delay_ms = SampleRegatherAllNetworksInterval(
      config_.regather_on_all_networks_interval_range.value());
  CancelScheduledRecurringRegatheringOnAllNetworks();
  has_recurring_schedule_on_all_networks_ = true;
  invoker_for_all_networks_.AsyncInvokeDelayed<void>(
      RTC_FROM_HERE, thread(),
      rtc::Bind(
          &BasicRegatheringController::RegatherOnAllNetworksIfDoneGathering,
          this, true),
      delay_ms);
}

void BasicRegatheringController::RegatherOnAllNetworksIfDoneGathering(
    bool repeated) {
  // Only regather when the current session is in the CLEARED state (i.e., not
  // running or stopped). It is only possible to enter this state when we gather
  // continually, so there is an implicit check on continual gathering here.
  if (allocator_session_ && allocator_session_->IsCleared()) {
    allocator_session_->RegatherOnAllNetworks();
  }
  if (repeated) {
    ScheduleRecurringRegatheringOnAllNetworks();
  }
}

void BasicRegatheringController::
    ScheduleRecurringRegatheringOnFailedNetworks() {
  RTC_DCHECK(config_.regather_on_failed_networks_interval >= 0);
  CancelScheduledRecurringRegatheringOnFailedNetworks();
  has_recurring_schedule_on_failed_networks_ = true;
  invoker_for_failed_networks_.AsyncInvokeDelayed<void>(
      RTC_FROM_HERE, thread(),
      rtc::Bind(
          &BasicRegatheringController::RegatherOnFailedNetworksIfDoneGathering,
          this, true),
      config_.regather_on_failed_networks_interval);
}

void BasicRegatheringController::RegatherOnFailedNetworksIfDoneGathering(
    bool repeated) {
  // Only regather when the current session is in the CLEARED state (i.e., not
  // running or stopped). It is only possible to enter this state when we gather
  // continually, so there is an implicit check on continual gathering here.
  if (allocator_session_ && allocator_session_->IsCleared()) {
    allocator_session_->RegatherOnFailedNetworks();
  }
  if (repeated) {
    ScheduleRecurringRegatheringOnFailedNetworks();
  }
}

void BasicRegatheringController::
    CancelScheduledRecurringRegatheringOnAllNetworks() {
  invoker_for_all_networks_.Clear();
  has_recurring_schedule_on_all_networks_ = false;
}

void BasicRegatheringController::
    CancelScheduledRecurringRegatheringOnFailedNetworks() {
  invoker_for_failed_networks_.Clear();
  has_recurring_schedule_on_failed_networks_ = false;
}

int BasicRegatheringController::SampleRegatherAllNetworksInterval(
    const rtc::IntervalRange& range) {
  return rand_.Rand(range.min(), range.max());
}

}  // namespace webrtc
