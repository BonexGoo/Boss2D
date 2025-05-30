/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_DEGRADED_CALL_H_
#define CALL_DEGRADED_CALL_H_

#include <stddef.h>
#include <stdint.h>

#include <map>
#include <memory>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__call__transport_h //original-code:"api/call/transport.h"
#include BOSS_WEBRTC_U_api__fec_controller_h //original-code:"api/fec_controller.h"
#include BOSS_WEBRTC_U_api__media_types_h //original-code:"api/media_types.h"
#include BOSS_WEBRTC_U_api__rtp_headers_h //original-code:"api/rtp_headers.h"
#include BOSS_WEBRTC_U_api__test__simulated_network_h //original-code:"api/test/simulated_network.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_config_h //original-code:"api/video_codecs/video_encoder_config.h"
#include BOSS_WEBRTC_U_call__audio_receive_stream_h //original-code:"call/audio_receive_stream.h"
#include BOSS_WEBRTC_U_call__audio_send_stream_h //original-code:"call/audio_send_stream.h"
#include BOSS_WEBRTC_U_call__call_h //original-code:"call/call.h"
#include BOSS_WEBRTC_U_call__fake_network_pipe_h //original-code:"call/fake_network_pipe.h"
#include BOSS_WEBRTC_U_call__flexfec_receive_stream_h //original-code:"call/flexfec_receive_stream.h"
#include BOSS_WEBRTC_U_call__packet_receiver_h //original-code:"call/packet_receiver.h"
#include BOSS_WEBRTC_U_call__rtp_transport_controller_send_interface_h //original-code:"call/rtp_transport_controller_send_interface.h"
#include BOSS_WEBRTC_U_call__simulated_network_h //original-code:"call/simulated_network.h"
#include BOSS_WEBRTC_U_call__video_receive_stream_h //original-code:"call/video_receive_stream.h"
#include BOSS_WEBRTC_U_call__video_send_stream_h //original-code:"call/video_send_stream.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"
#include BOSS_WEBRTC_U_rtc_base__network__sent_packet_h //original-code:"rtc_base/network/sent_packet.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {
class DegradedCall : public Call, private PacketReceiver {
 public:
  explicit DegradedCall(
      std::unique_ptr<Call> call,
      absl::optional<BuiltInNetworkBehaviorConfig> send_config,
      absl::optional<BuiltInNetworkBehaviorConfig> receive_config,
      TaskQueueFactory* task_queue_factory);
  ~DegradedCall() override;

  // Implements Call.
  AudioSendStream* CreateAudioSendStream(
      const AudioSendStream::Config& config) override;
  void DestroyAudioSendStream(AudioSendStream* send_stream) override;

  AudioReceiveStream* CreateAudioReceiveStream(
      const AudioReceiveStream::Config& config) override;
  void DestroyAudioReceiveStream(AudioReceiveStream* receive_stream) override;

  VideoSendStream* CreateVideoSendStream(
      VideoSendStream::Config config,
      VideoEncoderConfig encoder_config) override;
  VideoSendStream* CreateVideoSendStream(
      VideoSendStream::Config config,
      VideoEncoderConfig encoder_config,
      std::unique_ptr<FecController> fec_controller) override;
  void DestroyVideoSendStream(VideoSendStream* send_stream) override;

  VideoReceiveStream* CreateVideoReceiveStream(
      VideoReceiveStream::Config configuration) override;
  void DestroyVideoReceiveStream(VideoReceiveStream* receive_stream) override;

  FlexfecReceiveStream* CreateFlexfecReceiveStream(
      const FlexfecReceiveStream::Config& config) override;
  void DestroyFlexfecReceiveStream(
      FlexfecReceiveStream* receive_stream) override;

  void AddAdaptationResource(rtc::scoped_refptr<Resource> resource) override;

  PacketReceiver* Receiver() override;

  RtpTransportControllerSendInterface* GetTransportControllerSend() override;

  Stats GetStats() const override;

  const WebRtcKeyValueConfig& trials() const override;

  TaskQueueBase* network_thread() const override;
  TaskQueueBase* worker_thread() const override;

  void SignalChannelNetworkState(MediaType media, NetworkState state) override;
  void OnAudioTransportOverheadChanged(
      int transport_overhead_per_packet) override;
  void OnSentPacket(const rtc::SentPacket& sent_packet) override;

 protected:
  // Implements PacketReceiver.
  DeliveryStatus DeliverPacket(MediaType media_type,
                               rtc::CopyOnWriteBuffer packet,
                               int64_t packet_time_us) override;

 private:
  class FakeNetworkPipeOnTaskQueue {
   public:
    FakeNetworkPipeOnTaskQueue(
        TaskQueueFactory* task_queue_factory,
        Clock* clock,
        std::unique_ptr<NetworkBehaviorInterface> network_behavior);

    void SendRtp(const uint8_t* packet,
                 size_t length,
                 const PacketOptions& options,
                 Transport* transport);
    void SendRtcp(const uint8_t* packet, size_t length, Transport* transport);

    void AddActiveTransport(Transport* transport);
    void RemoveActiveTransport(Transport* transport);

   private:
    // Try to process packets on the fake network queue.
    // Returns true if call resulted in a delayed process, false if queue empty.
    bool Process();

    Clock* const clock_;
    rtc::TaskQueue task_queue_;
    FakeNetworkPipe pipe_;
    absl::optional<int64_t> next_process_ms_ RTC_GUARDED_BY(&task_queue_);
  };

  // For audio/video send stream, a TransportAdapter instance is used to
  // intercept packets to be sent, and put them into a common FakeNetworkPipe
  // in such as way that they will eventually (unless dropped) be forwarded to
  // the correct Transport for that stream.
  class FakeNetworkPipeTransportAdapter : public Transport {
   public:
    FakeNetworkPipeTransportAdapter(FakeNetworkPipeOnTaskQueue* fake_network,
                                    Call* call,
                                    Clock* clock,
                                    Transport* real_transport);
    ~FakeNetworkPipeTransportAdapter();

    bool SendRtp(const uint8_t* packet,
                 size_t length,
                 const PacketOptions& options) override;
    bool SendRtcp(const uint8_t* packet, size_t length) override;

   private:
    FakeNetworkPipeOnTaskQueue* const network_pipe_;
    Call* const call_;
    Clock* const clock_;
    Transport* const real_transport_;
  };

  Clock* const clock_;
  const std::unique_ptr<Call> call_;
  TaskQueueFactory* const task_queue_factory_;

  void SetClientBitratePreferences(
      const webrtc::BitrateSettings& preferences) override {}

  const absl::optional<BuiltInNetworkBehaviorConfig> send_config_;
  SimulatedNetwork* send_simulated_network_;
  std::unique_ptr<FakeNetworkPipeOnTaskQueue> send_pipe_;
  std::map<AudioSendStream*, std::unique_ptr<FakeNetworkPipeTransportAdapter>>
      audio_send_transport_adapters_;
  std::map<VideoSendStream*, std::unique_ptr<FakeNetworkPipeTransportAdapter>>
      video_send_transport_adapters_;

  const absl::optional<BuiltInNetworkBehaviorConfig> receive_config_;
  SimulatedNetwork* receive_simulated_network_;
  std::unique_ptr<FakeNetworkPipe> receive_pipe_;
};

}  // namespace webrtc

#endif  // CALL_DEGRADED_CALL_H_
