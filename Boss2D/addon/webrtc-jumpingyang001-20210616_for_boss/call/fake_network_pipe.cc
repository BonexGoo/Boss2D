/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_call__fake_network_pipe_h //original-code:"call/fake_network_pipe.h"

#include <string.h>

#include <algorithm>
#include <queue>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_api__media_types_h //original-code:"api/media_types.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

namespace {
constexpr int64_t kLogIntervalMs = 5000;
}  // namespace

NetworkPacket::NetworkPacket(rtc::CopyOnWriteBuffer packet,
                             int64_t send_time,
                             int64_t arrival_time,
                             absl::optional<PacketOptions> packet_options,
                             bool is_rtcp,
                             MediaType media_type,
                             absl::optional<int64_t> packet_time_us,
                             Transport* transport)
    : packet_(std::move(packet)),
      send_time_(send_time),
      arrival_time_(arrival_time),
      packet_options_(packet_options),
      is_rtcp_(is_rtcp),
      media_type_(media_type),
      packet_time_us_(packet_time_us),
      transport_(transport) {}

NetworkPacket::NetworkPacket(NetworkPacket&& o)
    : packet_(std::move(o.packet_)),
      send_time_(o.send_time_),
      arrival_time_(o.arrival_time_),
      packet_options_(o.packet_options_),
      is_rtcp_(o.is_rtcp_),
      media_type_(o.media_type_),
      packet_time_us_(o.packet_time_us_),
      transport_(o.transport_) {}

NetworkPacket::~NetworkPacket() = default;

NetworkPacket& NetworkPacket::operator=(NetworkPacket&& o) {
  packet_ = std::move(o.packet_);
  send_time_ = o.send_time_;
  arrival_time_ = o.arrival_time_;
  packet_options_ = o.packet_options_;
  is_rtcp_ = o.is_rtcp_;
  media_type_ = o.media_type_;
  packet_time_us_ = o.packet_time_us_;
  transport_ = o.transport_;

  return *this;
}

FakeNetworkPipe::FakeNetworkPipe(
    Clock* clock,
    std::unique_ptr<NetworkBehaviorInterface> network_behavior)
    : FakeNetworkPipe(clock, std::move(network_behavior), nullptr, 1) {}

FakeNetworkPipe::FakeNetworkPipe(
    Clock* clock,
    std::unique_ptr<NetworkBehaviorInterface> network_behavior,
    PacketReceiver* receiver)
    : FakeNetworkPipe(clock, std::move(network_behavior), receiver, 1) {}

FakeNetworkPipe::FakeNetworkPipe(
    Clock* clock,
    std::unique_ptr<NetworkBehaviorInterface> network_behavior,
    PacketReceiver* receiver,
    uint64_t seed)
    : clock_(clock),
      network_behavior_(std::move(network_behavior)),
      receiver_(receiver),
      global_transport_(nullptr),
      clock_offset_ms_(0),
      dropped_packets_(0),
      sent_packets_(0),
      total_packet_delay_us_(0),
      last_log_time_us_(clock_->TimeInMicroseconds()) {}

FakeNetworkPipe::FakeNetworkPipe(
    Clock* clock,
    std::unique_ptr<NetworkBehaviorInterface> network_behavior,
    Transport* transport)
    : clock_(clock),
      network_behavior_(std::move(network_behavior)),
      receiver_(nullptr),
      global_transport_(transport),
      clock_offset_ms_(0),
      dropped_packets_(0),
      sent_packets_(0),
      total_packet_delay_us_(0),
      last_log_time_us_(clock_->TimeInMicroseconds()) {
  RTC_DCHECK(global_transport_);
  AddActiveTransport(global_transport_);
}

FakeNetworkPipe::~FakeNetworkPipe() {
  if (global_transport_) {
    RemoveActiveTransport(global_transport_);
  }
  RTC_DCHECK(active_transports_.empty());
}

void FakeNetworkPipe::SetReceiver(PacketReceiver* receiver) {
  MutexLock lock(&config_lock_);
  receiver_ = receiver;
}

void FakeNetworkPipe::AddActiveTransport(Transport* transport) {
  MutexLock lock(&config_lock_);
  active_transports_[transport]++;
}

void FakeNetworkPipe::RemoveActiveTransport(Transport* transport) {
  MutexLock lock(&config_lock_);
  auto it = active_transports_.find(transport);
  RTC_CHECK(it != active_transports_.end());
  if (--(it->second) == 0) {
    active_transports_.erase(it);
  }
}

bool FakeNetworkPipe::SendRtp(const uint8_t* packet,
                              size_t length,
                              const PacketOptions& options) {
  RTC_DCHECK(global_transport_);
  EnqueuePacket(rtc::CopyOnWriteBuffer(packet, length), options, false,
                global_transport_);
  return true;
}

bool FakeNetworkPipe::SendRtcp(const uint8_t* packet, size_t length) {
  RTC_DCHECK(global_transport_);
  EnqueuePacket(rtc::CopyOnWriteBuffer(packet, length), absl::nullopt, true,
                global_transport_);
  return true;
}

bool FakeNetworkPipe::SendRtp(const uint8_t* packet,
                              size_t length,
                              const PacketOptions& options,
                              Transport* transport) {
  RTC_DCHECK(transport);
  EnqueuePacket(rtc::CopyOnWriteBuffer(packet, length), options, false,
                transport);
  return true;
}

bool FakeNetworkPipe::SendRtcp(const uint8_t* packet,
                               size_t length,
                               Transport* transport) {
  RTC_DCHECK(transport);
  EnqueuePacket(rtc::CopyOnWriteBuffer(packet, length), absl::nullopt, true,
                transport);
  return true;
}

PacketReceiver::DeliveryStatus FakeNetworkPipe::DeliverPacket(
    MediaType media_type,
    rtc::CopyOnWriteBuffer packet,
    int64_t packet_time_us) {
  return EnqueuePacket(std::move(packet), absl::nullopt, false, media_type,
                       packet_time_us)
             ? PacketReceiver::DELIVERY_OK
             : PacketReceiver::DELIVERY_PACKET_ERROR;
}

void FakeNetworkPipe::SetClockOffset(int64_t offset_ms) {
  MutexLock lock(&config_lock_);
  clock_offset_ms_ = offset_ms;
}

FakeNetworkPipe::StoredPacket::StoredPacket(NetworkPacket&& packet)
    : packet(std::move(packet)) {}

bool FakeNetworkPipe::EnqueuePacket(rtc::CopyOnWriteBuffer packet,
                                    absl::optional<PacketOptions> options,
                                    bool is_rtcp,
                                    MediaType media_type,
                                    absl::optional<int64_t> packet_time_us) {
  MutexLock lock(&process_lock_);
  int64_t time_now_us = clock_->TimeInMicroseconds();
  return EnqueuePacket(NetworkPacket(std::move(packet), time_now_us,
                                     time_now_us, options, is_rtcp, media_type,
                                     packet_time_us, nullptr));
}

bool FakeNetworkPipe::EnqueuePacket(rtc::CopyOnWriteBuffer packet,
                                    absl::optional<PacketOptions> options,
                                    bool is_rtcp,
                                    Transport* transport) {
  MutexLock lock(&process_lock_);
  int64_t time_now_us = clock_->TimeInMicroseconds();
  return EnqueuePacket(NetworkPacket(std::move(packet), time_now_us,
                                     time_now_us, options, is_rtcp,
                                     MediaType::ANY, absl::nullopt, transport));
}

bool FakeNetworkPipe::EnqueuePacket(NetworkPacket&& net_packet) {
  int64_t send_time_us = net_packet.send_time();
  size_t packet_size = net_packet.data_length();

  packets_in_flight_.emplace_back(StoredPacket(std::move(net_packet)));
  int64_t packet_id = reinterpret_cast<uint64_t>(&packets_in_flight_.back());
  bool sent = network_behavior_->EnqueuePacket(
      PacketInFlightInfo(packet_size, send_time_us, packet_id));

  if (!sent) {
    packets_in_flight_.pop_back();
    ++dropped_packets_;
  }
  return sent;
}

float FakeNetworkPipe::PercentageLoss() {
  MutexLock lock(&process_lock_);
  if (sent_packets_ == 0)
    return 0;

  return static_cast<float>(dropped_packets_) /
         (sent_packets_ + dropped_packets_);
}

int FakeNetworkPipe::AverageDelay() {
  MutexLock lock(&process_lock_);
  if (sent_packets_ == 0)
    return 0;

  return static_cast<int>(total_packet_delay_us_ /
                          (1000 * static_cast<int64_t>(sent_packets_)));
}

size_t FakeNetworkPipe::DroppedPackets() {
  MutexLock lock(&process_lock_);
  return dropped_packets_;
}

size_t FakeNetworkPipe::SentPackets() {
  MutexLock lock(&process_lock_);
  return sent_packets_;
}

void FakeNetworkPipe::Process() {
  int64_t time_now_us;
  std::queue<NetworkPacket> packets_to_deliver;
  {
    MutexLock lock(&process_lock_);
    time_now_us = clock_->TimeInMicroseconds();
    if (time_now_us - last_log_time_us_ > kLogIntervalMs * 1000) {
      int64_t queueing_delay_us = 0;
      if (!packets_in_flight_.empty())
        queueing_delay_us =
            time_now_us - packets_in_flight_.front().packet.send_time();

      RTC_LOG(LS_INFO) << "Network queue: " << queueing_delay_us / 1000
                       << " ms.";
      last_log_time_us_ = time_now_us;
    }

    std::vector<PacketDeliveryInfo> delivery_infos =
        network_behavior_->DequeueDeliverablePackets(time_now_us);
    for (auto& delivery_info : delivery_infos) {
      // In the common case where no reordering happens, find will return early
      // as the first packet will be a match.
      auto packet_it =
          std::find_if(packets_in_flight_.begin(), packets_in_flight_.end(),
                       [&delivery_info](StoredPacket& packet_ref) {
                         return reinterpret_cast<uint64_t>(&packet_ref) ==
                                delivery_info.packet_id;
                       });
      // Check that the packet is in the deque of packets in flight.
      RTC_CHECK(packet_it != packets_in_flight_.end());
      // Check that the packet is not already removed.
      RTC_DCHECK(!packet_it->removed);

      NetworkPacket packet = std::move(packet_it->packet);
      packet_it->removed = true;

      // Cleanup of removed packets at the beginning of the deque.
      while (!packets_in_flight_.empty() &&
             packets_in_flight_.front().removed) {
        packets_in_flight_.pop_front();
      }

      if (delivery_info.receive_time_us != PacketDeliveryInfo::kNotReceived) {
        int64_t added_delay_us =
            delivery_info.receive_time_us - packet.send_time();
        packet.IncrementArrivalTime(added_delay_us);
        packets_to_deliver.emplace(std::move(packet));
        // |time_now_us| might be later than when the packet should have
        // arrived, due to NetworkProcess being called too late. For stats, use
        // the time it should have been on the link.
        total_packet_delay_us_ += added_delay_us;
        ++sent_packets_;
      } else {
        ++dropped_packets_;
      }
    }
  }

  MutexLock lock(&config_lock_);
  while (!packets_to_deliver.empty()) {
    NetworkPacket packet = std::move(packets_to_deliver.front());
    packets_to_deliver.pop();
    DeliverNetworkPacket(&packet);
  }
}

void FakeNetworkPipe::DeliverNetworkPacket(NetworkPacket* packet) {
  Transport* transport = packet->transport();
  if (transport) {
    RTC_DCHECK(!receiver_);
    if (active_transports_.find(transport) == active_transports_.end()) {
      // Transport has been destroyed, ignore this packet.
      return;
    }
    if (packet->is_rtcp()) {
      transport->SendRtcp(packet->data(), packet->data_length());
    } else {
      transport->SendRtp(packet->data(), packet->data_length(),
                         packet->packet_options());
    }
  } else if (receiver_) {
    int64_t packet_time_us = packet->packet_time_us().value_or(-1);
    if (packet_time_us != -1) {
      int64_t queue_time_us = packet->arrival_time() - packet->send_time();
      RTC_CHECK(queue_time_us >= 0);
      packet_time_us += queue_time_us;
      packet_time_us += (clock_offset_ms_ * 1000);
    }
    receiver_->DeliverPacket(packet->media_type(),
                             std::move(*packet->raw_packet()), packet_time_us);
  }
}

absl::optional<int64_t> FakeNetworkPipe::TimeUntilNextProcess() {
  MutexLock lock(&process_lock_);
  absl::optional<int64_t> delivery_us = network_behavior_->NextDeliveryTimeUs();
  if (delivery_us) {
    int64_t delay_us = *delivery_us - clock_->TimeInMicroseconds();
    return std::max<int64_t>((delay_us + 500) / 1000, 0);
  }
  return absl::nullopt;
}

bool FakeNetworkPipe::HasReceiver() const {
  MutexLock lock(&config_lock_);
  return receiver_ != nullptr;
}

void FakeNetworkPipe::DeliverPacketWithLock(NetworkPacket* packet) {
  MutexLock lock(&config_lock_);
  DeliverNetworkPacket(packet);
}

void FakeNetworkPipe::ResetStats() {
  MutexLock lock(&process_lock_);
  dropped_packets_ = 0;
  sent_packets_ = 0;
  total_packet_delay_us_ = 0;
}

int64_t FakeNetworkPipe::GetTimeInMicroseconds() const {
  return clock_->TimeInMicroseconds();
}

}  // namespace webrtc
