/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_DIRECT_TRANSPORT_H_
#define TEST_DIRECT_TRANSPORT_H_

#include <assert.h>

#include <memory>

#include BOSS_WEBRTC_U_api__call__transport_h //original-code:"api/call/transport.h"
#include BOSS_WEBRTC_U_call__call_h //original-code:"call/call.h"
#include BOSS_WEBRTC_U_call__fake_network_pipe_h //original-code:"call/fake_network_pipe.h"
#include BOSS_WEBRTC_U_rtc_base__sequenced_task_checker_h //original-code:"rtc_base/sequenced_task_checker.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"
#include "test/single_threaded_task_queue.h"

namespace webrtc {

class Clock;
class PacketReceiver;

namespace test {
class Demuxer {
 public:
  explicit Demuxer(const std::map<uint8_t, MediaType>& payload_type_map);
  ~Demuxer() = default;
  MediaType GetMediaType(const uint8_t* packet_data,
                         const size_t packet_length) const;
  const std::map<uint8_t, MediaType> payload_type_map_;
  RTC_DISALLOW_COPY_AND_ASSIGN(Demuxer);
};

// Objects of this class are expected to be allocated and destroyed  on the
// same task-queue - the one that's passed in via the constructor.
class DirectTransport : public Transport {
 public:
  DirectTransport(SingleThreadedTaskQueueForTesting* task_queue,
                  Call* send_call,
                  const std::map<uint8_t, MediaType>& payload_type_map);

  DirectTransport(SingleThreadedTaskQueueForTesting* task_queue,
                  const FakeNetworkPipe::Config& config,
                  Call* send_call,
                  const std::map<uint8_t, MediaType>& payload_type_map);

  DirectTransport(SingleThreadedTaskQueueForTesting* task_queue,
                  std::unique_ptr<FakeNetworkPipe> pipe,
                  Call* send_call,
                  const std::map<uint8_t, MediaType>& payload_type_map);

  ~DirectTransport() override;

  void SetClockOffset(int64_t offset_ms);

  void SetConfig(const FakeNetworkPipe::Config& config);

  RTC_DEPRECATED void StopSending();

  // TODO(holmer): Look into moving this to the constructor.
  virtual void SetReceiver(PacketReceiver* receiver);

  bool SendRtp(const uint8_t* data,
               size_t length,
               const PacketOptions& options) override;
  bool SendRtcp(const uint8_t* data, size_t length) override;

  int GetAverageDelayMs();

 private:
  void SendPackets();
  void SendPacket(const uint8_t* data, size_t length);
  void Start();

  Call* const send_call_;
  Clock* const clock_;

  SingleThreadedTaskQueueForTesting* const task_queue_;
  SingleThreadedTaskQueueForTesting::TaskId next_scheduled_task_
      RTC_GUARDED_BY(&sequence_checker_);

  const Demuxer demuxer_;
  const std::unique_ptr<FakeNetworkPipe> fake_network_;

  rtc::SequencedTaskChecker sequence_checker_;
};
}  // namespace test
}  // namespace webrtc

#endif  // TEST_DIRECT_TRANSPORT_H_
