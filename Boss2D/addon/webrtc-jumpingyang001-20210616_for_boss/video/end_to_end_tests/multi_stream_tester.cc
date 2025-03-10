/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_video__end_to_end_tests__multi_stream_tester_h //original-code:"video/end_to_end_tests/multi_stream_tester.h"

#include <memory>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_h //original-code:"api/rtc_event_log/rtc_event_log.h"
#include BOSS_WEBRTC_U_api__task_queue__default_task_queue_factory_h //original-code:"api/task_queue/default_task_queue_factory.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"
#include BOSS_WEBRTC_U_api__test__create_frame_generator_h //original-code:"api/test/create_frame_generator.h"
#include BOSS_WEBRTC_U_api__test__simulated_network_h //original-code:"api/test/simulated_network.h"
#include "api/test/video/function_video_encoder_factory.h"
#include BOSS_WEBRTC_U_api__video__builtin_video_bitrate_allocator_factory_h //original-code:"api/video/builtin_video_bitrate_allocator_factory.h"
#include BOSS_WEBRTC_U_call__fake_network_pipe_h //original-code:"call/fake_network_pipe.h"
#include BOSS_WEBRTC_U_call__simulated_network_h //original-code:"call/simulated_network.h"
#include BOSS_WEBRTC_U_media__engine__internal_decoder_factory_h //original-code:"media/engine/internal_decoder_factory.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp8__include__vp8_h //original-code:"modules/video_coding/codecs/vp8/include/vp8.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_for_test_h //original-code:"rtc_base/task_queue_for_test.h"
#include BOSS_WEBRTC_U_test__call_test_h //original-code:"test/call_test.h"
#include BOSS_WEBRTC_U_test__encoder_settings_h //original-code:"test/encoder_settings.h"

namespace webrtc {

MultiStreamTester::MultiStreamTester() {
  // TODO(sprang): Cleanup when msvc supports explicit initializers for array.
  codec_settings[0] = {1, 640, 480};
  codec_settings[1] = {2, 320, 240};
  codec_settings[2] = {3, 240, 160};
}

MultiStreamTester::~MultiStreamTester() = default;

void MultiStreamTester::RunTest() {
  webrtc::RtcEventLogNull event_log;
  auto task_queue_factory = CreateDefaultTaskQueueFactory();
  // Use high prioirity since this task_queue used for fake network delivering
  // at correct time. Those test tasks should be prefered over code under test
  // to make test more stable.
  auto task_queue = task_queue_factory->CreateTaskQueue(
      "TaskQueue", TaskQueueFactory::Priority::HIGH);
  Call::Config config(&event_log);
  FieldTrialBasedConfig field_trials;
  config.trials = &field_trials;
  config.task_queue_factory = task_queue_factory.get();
  std::unique_ptr<Call> sender_call;
  std::unique_ptr<Call> receiver_call;
  std::unique_ptr<test::DirectTransport> sender_transport;
  std::unique_ptr<test::DirectTransport> receiver_transport;

  VideoSendStream* send_streams[kNumStreams];
  VideoReceiveStream* receive_streams[kNumStreams];
  test::FrameGeneratorCapturer* frame_generators[kNumStreams];
  test::FunctionVideoEncoderFactory encoder_factory(
      []() { return VP8Encoder::Create(); });
  std::unique_ptr<VideoBitrateAllocatorFactory> bitrate_allocator_factory =
      CreateBuiltinVideoBitrateAllocatorFactory();
  InternalDecoderFactory decoder_factory;

  SendTask(RTC_FROM_HERE, task_queue.get(), [&]() {
    sender_call = absl::WrapUnique(Call::Create(config));
    receiver_call = absl::WrapUnique(Call::Create(config));
    sender_transport = CreateSendTransport(task_queue.get(), sender_call.get());
    receiver_transport =
        CreateReceiveTransport(task_queue.get(), receiver_call.get());

    sender_transport->SetReceiver(receiver_call->Receiver());
    receiver_transport->SetReceiver(sender_call->Receiver());

    for (size_t i = 0; i < kNumStreams; ++i) {
      uint32_t ssrc = codec_settings[i].ssrc;
      int width = codec_settings[i].width;
      int height = codec_settings[i].height;

      VideoSendStream::Config send_config(sender_transport.get());
      send_config.rtp.ssrcs.push_back(ssrc);
      send_config.encoder_settings.encoder_factory = &encoder_factory;
      send_config.encoder_settings.bitrate_allocator_factory =
          bitrate_allocator_factory.get();
      send_config.rtp.payload_name = "VP8";
      send_config.rtp.payload_type = kVideoPayloadType;
      VideoEncoderConfig encoder_config;
      test::FillEncoderConfiguration(kVideoCodecVP8, 1, &encoder_config);
      encoder_config.max_bitrate_bps = 100000;

      UpdateSendConfig(i, &send_config, &encoder_config, &frame_generators[i]);

      send_streams[i] = sender_call->CreateVideoSendStream(
          send_config.Copy(), encoder_config.Copy());
      send_streams[i]->Start();

      VideoReceiveStream::Config receive_config(receiver_transport.get());
      receive_config.rtp.remote_ssrc = ssrc;
      receive_config.rtp.local_ssrc = test::CallTest::kReceiverLocalVideoSsrc;
      receive_config.decoder_factory = &decoder_factory;
      VideoReceiveStream::Decoder decoder =
          test::CreateMatchingDecoder(send_config);
      receive_config.decoders.push_back(decoder);

      UpdateReceiveConfig(i, &receive_config);

      receive_streams[i] =
          receiver_call->CreateVideoReceiveStream(std::move(receive_config));
      receive_streams[i]->Start();

      auto* frame_generator = new test::FrameGeneratorCapturer(
          Clock::GetRealTimeClock(),
          test::CreateSquareFrameGenerator(width, height, absl::nullopt,
                                           absl::nullopt),
          30, *task_queue_factory);
      frame_generators[i] = frame_generator;
      send_streams[i]->SetSource(frame_generator,
                                 DegradationPreference::MAINTAIN_FRAMERATE);
      frame_generator->Init();
      frame_generator->Start();
    }
  });

  Wait();

  SendTask(RTC_FROM_HERE, task_queue.get(), [&]() {
    for (size_t i = 0; i < kNumStreams; ++i) {
      frame_generators[i]->Stop();
      sender_call->DestroyVideoSendStream(send_streams[i]);
      receiver_call->DestroyVideoReceiveStream(receive_streams[i]);
      delete frame_generators[i];
    }

    sender_transport.reset();
    receiver_transport.reset();

    sender_call.reset();
    receiver_call.reset();
  });
}

void MultiStreamTester::UpdateSendConfig(
    size_t stream_index,
    VideoSendStream::Config* send_config,
    VideoEncoderConfig* encoder_config,
    test::FrameGeneratorCapturer** frame_generator) {}

void MultiStreamTester::UpdateReceiveConfig(
    size_t stream_index,
    VideoReceiveStream::Config* receive_config) {}

std::unique_ptr<test::DirectTransport> MultiStreamTester::CreateSendTransport(
    TaskQueueBase* task_queue,
    Call* sender_call) {
  return std::make_unique<test::DirectTransport>(
      task_queue,
      std::make_unique<FakeNetworkPipe>(
          Clock::GetRealTimeClock(),
          std::make_unique<SimulatedNetwork>(BuiltInNetworkBehaviorConfig())),
      sender_call, payload_type_map_);
}

std::unique_ptr<test::DirectTransport>
MultiStreamTester::CreateReceiveTransport(TaskQueueBase* task_queue,
                                          Call* receiver_call) {
  return std::make_unique<test::DirectTransport>(
      task_queue,
      std::make_unique<FakeNetworkPipe>(
          Clock::GetRealTimeClock(),
          std::make_unique<SimulatedNetwork>(BuiltInNetworkBehaviorConfig())),
      receiver_call, payload_type_map_);
}
}  // namespace webrtc
