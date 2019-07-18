/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_CALL_TEST_H_
#define TEST_CALL_TEST_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_call__call_h //original-code:"call/call.h"
#include BOSS_WEBRTC_U_call__rtp_transport_controller_send_h //original-code:"call/rtp_transport_controller_send.h"
#include BOSS_WEBRTC_U_logging__rtc_event_log__rtc_event_log_h //original-code:"logging/rtc_event_log/rtc_event_log.h"
#include "test/encoder_settings.h"
#include "test/fake_audio_device.h"
#include "test/fake_decoder.h"
#include "test/fake_encoder.h"
#include "test/fake_videorenderer.h"
#include "test/frame_generator_capturer.h"
#include "test/rtp_rtcp_observer.h"
#include "test/single_threaded_task_queue.h"

namespace webrtc {
namespace test {

class BaseTest;

class CallTest : public ::testing::Test {
 public:
  CallTest();
  virtual ~CallTest();

  static constexpr size_t kNumSsrcs = 6;
  static const int kNumSimulcastStreams = 3;
  static const int kDefaultWidth = 320;
  static const int kDefaultHeight = 180;
  static const int kDefaultFramerate = 30;
  static const int kDefaultTimeoutMs;
  static const int kLongTimeoutMs;
  static const uint8_t kVideoSendPayloadType;
  static const uint8_t kSendRtxPayloadType;
  static const uint8_t kFakeVideoSendPayloadType;
  static const uint8_t kRedPayloadType;
  static const uint8_t kRtxRedPayloadType;
  static const uint8_t kUlpfecPayloadType;
  static const uint8_t kFlexfecPayloadType;
  static const uint8_t kAudioSendPayloadType;
  static const uint8_t kPayloadTypeH264;
  static const uint8_t kPayloadTypeVP8;
  static const uint8_t kPayloadTypeVP9;
  static const uint32_t kSendRtxSsrcs[kNumSsrcs];
  static const uint32_t kVideoSendSsrcs[kNumSsrcs];
  static const uint32_t kAudioSendSsrc;
  static const uint32_t kFlexfecSendSsrc;
  static const uint32_t kReceiverLocalVideoSsrc;
  static const uint32_t kReceiverLocalAudioSsrc;
  static const int kNackRtpHistoryMs;
  static const uint8_t kDefaultKeepalivePayloadType;
  static const std::map<uint8_t, MediaType> payload_type_map_;

 protected:
  // RunBaseTest overwrites the audio_state of the send and receive Call configs
  // to simplify test code.
  void RunBaseTest(BaseTest* test);

  void CreateCalls(const Call::Config& sender_config,
                   const Call::Config& receiver_config);
  void CreateSenderCall(const Call::Config& config);
  void CreateReceiverCall(const Call::Config& config);
  void DestroyCalls();

  void CreateVideoSendConfig(VideoSendStream::Config* video_config,
                             size_t num_video_streams,
                             size_t num_used_ssrcs,
                             Transport* send_transport);
  void CreateAudioAndFecSendConfigs(size_t num_audio_streams,
                                    size_t num_flexfec_streams,
                                    Transport* send_transport);
  void CreateSendConfig(size_t num_video_streams,
                        size_t num_audio_streams,
                        size_t num_flexfec_streams,
                        Transport* send_transport);

  std::vector<VideoReceiveStream::Config> CreateMatchingVideoReceiveConfigs(
      const VideoSendStream::Config& video_send_config,
      Transport* rtcp_send_transport);
  void CreateMatchingAudioAndFecConfigs(Transport* rtcp_send_transport);
  void CreateMatchingReceiveConfigs(Transport* rtcp_send_transport);

  void CreateFrameGeneratorCapturerWithDrift(Clock* drift_clock,
                                             float speed,
                                             int framerate,
                                             int width,
                                             int height);
  void CreateFrameGeneratorCapturer(int framerate, int width, int height);
  void CreateFakeAudioDevices(
      std::unique_ptr<FakeAudioDevice::Capturer> capturer,
      std::unique_ptr<FakeAudioDevice::Renderer> renderer);

  void CreateVideoStreams();
  void CreateAudioStreams();
  void CreateFlexfecStreams();

  void AssociateFlexfecStreamsWithVideoStreams();
  void DissociateFlexfecStreamsFromVideoStreams();

  void Start();
  void Stop();
  void DestroyStreams();
  void SetFakeVideoCaptureRotation(VideoRotation rotation);

  Clock* const clock_;

  std::unique_ptr<webrtc::RtcEventLog> event_log_;
  std::unique_ptr<Call> sender_call_;
  RtpTransportControllerSend* sender_call_transport_controller_;
  std::unique_ptr<PacketTransport> send_transport_;
  VideoSendStream::Config video_send_config_;
  VideoEncoderConfig video_encoder_config_;
  VideoSendStream* video_send_stream_;
  AudioSendStream::Config audio_send_config_;
  AudioSendStream* audio_send_stream_;

  std::unique_ptr<Call> receiver_call_;
  std::unique_ptr<PacketTransport> receive_transport_;
  std::vector<VideoReceiveStream::Config> video_receive_configs_;
  std::vector<VideoReceiveStream*> video_receive_streams_;
  std::vector<AudioReceiveStream::Config> audio_receive_configs_;
  std::vector<AudioReceiveStream*> audio_receive_streams_;
  std::vector<FlexfecReceiveStream::Config> flexfec_receive_configs_;
  std::vector<FlexfecReceiveStream*> flexfec_receive_streams_;

  std::unique_ptr<test::FrameGeneratorCapturer> frame_generator_capturer_;
  test::FakeEncoder fake_encoder_;
  std::vector<std::unique_ptr<VideoDecoder>> allocated_decoders_;
  size_t num_video_streams_;
  size_t num_audio_streams_;
  size_t num_flexfec_streams_;
  rtc::scoped_refptr<AudioDecoderFactory> decoder_factory_;
  rtc::scoped_refptr<AudioEncoderFactory> encoder_factory_;
  test::FakeVideoRenderer fake_renderer_;

  SingleThreadedTaskQueueForTesting task_queue_;

 private:
  rtc::scoped_refptr<AudioProcessing> apm_send_;
  rtc::scoped_refptr<AudioProcessing> apm_recv_;
  rtc::scoped_refptr<test::FakeAudioDevice> fake_send_audio_device_;
  rtc::scoped_refptr<test::FakeAudioDevice> fake_recv_audio_device_;
};

class BaseTest : public RtpRtcpObserver {
 public:
  BaseTest();
  explicit BaseTest(unsigned int timeout_ms);
  virtual ~BaseTest();

  virtual void PerformTest() = 0;
  virtual bool ShouldCreateReceivers() const = 0;

  virtual size_t GetNumVideoStreams() const;
  virtual size_t GetNumAudioStreams() const;
  virtual size_t GetNumFlexfecStreams() const;

  virtual std::unique_ptr<FakeAudioDevice::Capturer> CreateCapturer();
  virtual std::unique_ptr<FakeAudioDevice::Renderer> CreateRenderer();
  virtual void OnFakeAudioDevicesCreated(FakeAudioDevice* send_audio_device,
                                         FakeAudioDevice* recv_audio_device);

  virtual Call::Config GetSenderCallConfig();
  virtual Call::Config GetReceiverCallConfig();
  virtual void OnRtpTransportControllerSendCreated(
      RtpTransportControllerSend* controller);
  virtual void OnCallsCreated(Call* sender_call, Call* receiver_call);

  virtual test::PacketTransport* CreateSendTransport(
      SingleThreadedTaskQueueForTesting* task_queue,
      Call* sender_call);
  virtual test::PacketTransport* CreateReceiveTransport(
      SingleThreadedTaskQueueForTesting* task_queue);

  virtual void ModifyVideoConfigs(
      VideoSendStream::Config* send_config,
      std::vector<VideoReceiveStream::Config>* receive_configs,
      VideoEncoderConfig* encoder_config);
  virtual void ModifyVideoCaptureStartResolution(int* width,
                                                 int* heigt,
                                                 int* frame_rate);
  virtual void OnVideoStreamsCreated(
      VideoSendStream* send_stream,
      const std::vector<VideoReceiveStream*>& receive_streams);

  virtual void ModifyAudioConfigs(
      AudioSendStream::Config* send_config,
      std::vector<AudioReceiveStream::Config>* receive_configs);
  virtual void OnAudioStreamsCreated(
      AudioSendStream* send_stream,
      const std::vector<AudioReceiveStream*>& receive_streams);

  virtual void ModifyFlexfecConfigs(
      std::vector<FlexfecReceiveStream::Config>* receive_configs);
  virtual void OnFlexfecStreamsCreated(
      const std::vector<FlexfecReceiveStream*>& receive_streams);

  virtual void OnFrameGeneratorCapturerCreated(
      FrameGeneratorCapturer* frame_generator_capturer);

  virtual void OnStreamsStopped();

  std::unique_ptr<webrtc::RtcEventLog> event_log_;
};

class SendTest : public BaseTest {
 public:
  explicit SendTest(unsigned int timeout_ms);

  bool ShouldCreateReceivers() const override;
};

class EndToEndTest : public BaseTest {
 public:
  EndToEndTest();
  explicit EndToEndTest(unsigned int timeout_ms);

  bool ShouldCreateReceivers() const override;
};

}  // namespace test
}  // namespace webrtc

#endif  // TEST_CALL_TEST_H_
