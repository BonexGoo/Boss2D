/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_call__call_h //original-code:"call/call.h"

#include <list>
#include <map>
#include <memory>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_decoder_factory_h //original-code:"api/audio_codecs/builtin_audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_h //original-code:"api/rtc_event_log/rtc_event_log.h"
#include BOSS_WEBRTC_U_api__task_queue__default_task_queue_factory_h //original-code:"api/task_queue/default_task_queue_factory.h"
#include BOSS_WEBRTC_U_api__test__mock_audio_mixer_h //original-code:"api/test/mock_audio_mixer.h"
#include "api/test/video/function_video_encoder_factory.h"
#include BOSS_WEBRTC_U_api__transport__field_trial_based_config_h //original-code:"api/transport/field_trial_based_config.h"
#include BOSS_WEBRTC_U_api__video__builtin_video_bitrate_allocator_factory_h //original-code:"api/video/builtin_video_bitrate_allocator_factory.h"
#include BOSS_WEBRTC_U_audio__audio_receive_stream_h //original-code:"audio/audio_receive_stream.h"
#include BOSS_WEBRTC_U_audio__audio_send_stream_h //original-code:"audio/audio_send_stream.h"
#include "call/adaptation/test/fake_resource.h"
#include "call/adaptation/test/mock_resource_listener.h"
#include BOSS_WEBRTC_U_call__audio_state_h //original-code:"call/audio_state.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__mock_audio_device_h //original-code:"modules/audio_device/include/mock_audio_device.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__mock_audio_processing_h //original-code:"modules/audio_processing/include/mock_audio_processing.h"
#include BOSS_WEBRTC_U_modules__include__module_h //original-code:"modules/include/module.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_rtcp_interface_h //original-code:"modules/rtp_rtcp/source/rtp_rtcp_interface.h"
#include BOSS_WEBRTC_U_test__fake_encoder_h //original-code:"test/fake_encoder.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_test__mock_audio_decoder_factory_h //original-code:"test/mock_audio_decoder_factory.h"
#include BOSS_WEBRTC_U_test__mock_transport_h //original-code:"test/mock_transport.h"
#include BOSS_WEBRTC_U_test__run_loop_h //original-code:"test/run_loop.h"

namespace {

using ::testing::_;
using ::testing::Contains;
using ::testing::NiceMock;
using ::testing::StrictMock;

struct CallHelper {
  explicit CallHelper(bool use_null_audio_processing) {
    task_queue_factory_ = webrtc::CreateDefaultTaskQueueFactory();
    webrtc::AudioState::Config audio_state_config;
    audio_state_config.audio_mixer =
        rtc::make_ref_counted<webrtc::test::MockAudioMixer>();
    audio_state_config.audio_processing =
        use_null_audio_processing
            ? nullptr
            : rtc::make_ref_counted<
                  NiceMock<webrtc::test::MockAudioProcessing>>();
    audio_state_config.audio_device_module =
        rtc::make_ref_counted<webrtc::test::MockAudioDeviceModule>();
    webrtc::Call::Config config(&event_log_);
    config.audio_state = webrtc::AudioState::Create(audio_state_config);
    config.task_queue_factory = task_queue_factory_.get();
    config.trials = &field_trials_;
    call_.reset(webrtc::Call::Create(config));
  }

  webrtc::Call* operator->() { return call_.get(); }

 private:
  webrtc::test::RunLoop loop_;
  webrtc::RtcEventLogNull event_log_;
  webrtc::FieldTrialBasedConfig field_trials_;
  std::unique_ptr<webrtc::TaskQueueFactory> task_queue_factory_;
  std::unique_ptr<webrtc::Call> call_;
};
}  // namespace

namespace webrtc {

namespace {

rtc::scoped_refptr<Resource> FindResourceWhoseNameContains(
    const std::vector<rtc::scoped_refptr<Resource>>& resources,
    const std::string& name_contains) {
  for (const auto& resource : resources) {
    if (resource->Name().find(name_contains) != std::string::npos)
      return resource;
  }
  return nullptr;
}

}  // namespace

TEST(CallTest, ConstructDestruct) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
  }
}

TEST(CallTest, CreateDestroy_AudioSendStream) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
    MockTransport send_transport;
    AudioSendStream::Config config(&send_transport);
    config.rtp.ssrc = 42;
    AudioSendStream* stream = call->CreateAudioSendStream(config);
    EXPECT_NE(stream, nullptr);
    call->DestroyAudioSendStream(stream);
  }
}

TEST(CallTest, CreateDestroy_AudioReceiveStream) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
    AudioReceiveStream::Config config;
    MockTransport rtcp_send_transport;
    config.rtp.remote_ssrc = 42;
    config.rtcp_send_transport = &rtcp_send_transport;
    config.decoder_factory =
        rtc::make_ref_counted<webrtc::MockAudioDecoderFactory>();
    AudioReceiveStream* stream = call->CreateAudioReceiveStream(config);
    EXPECT_NE(stream, nullptr);
    call->DestroyAudioReceiveStream(stream);
  }
}

TEST(CallTest, CreateDestroy_AudioSendStreams) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
    MockTransport send_transport;
    AudioSendStream::Config config(&send_transport);
    std::list<AudioSendStream*> streams;
    for (int i = 0; i < 2; ++i) {
      for (uint32_t ssrc = 0; ssrc < 1234567; ssrc += 34567) {
        config.rtp.ssrc = ssrc;
        AudioSendStream* stream = call->CreateAudioSendStream(config);
        EXPECT_NE(stream, nullptr);
        if (ssrc & 1) {
          streams.push_back(stream);
        } else {
          streams.push_front(stream);
        }
      }
      for (auto s : streams) {
        call->DestroyAudioSendStream(s);
      }
      streams.clear();
    }
  }
}

TEST(CallTest, CreateDestroy_AudioReceiveStreams) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
    AudioReceiveStream::Config config;
    MockTransport rtcp_send_transport;
    config.rtcp_send_transport = &rtcp_send_transport;
    config.decoder_factory =
        rtc::make_ref_counted<webrtc::MockAudioDecoderFactory>();
    std::list<AudioReceiveStream*> streams;
    for (int i = 0; i < 2; ++i) {
      for (uint32_t ssrc = 0; ssrc < 1234567; ssrc += 34567) {
        config.rtp.remote_ssrc = ssrc;
        AudioReceiveStream* stream = call->CreateAudioReceiveStream(config);
        EXPECT_NE(stream, nullptr);
        if (ssrc & 1) {
          streams.push_back(stream);
        } else {
          streams.push_front(stream);
        }
      }
      for (auto s : streams) {
        call->DestroyAudioReceiveStream(s);
      }
      streams.clear();
    }
  }
}

TEST(CallTest, CreateDestroy_AssociateAudioSendReceiveStreams_RecvFirst) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
    AudioReceiveStream::Config recv_config;
    MockTransport rtcp_send_transport;
    recv_config.rtp.remote_ssrc = 42;
    recv_config.rtp.local_ssrc = 777;
    recv_config.rtcp_send_transport = &rtcp_send_transport;
    recv_config.decoder_factory =
        rtc::make_ref_counted<webrtc::MockAudioDecoderFactory>();
    AudioReceiveStream* recv_stream =
        call->CreateAudioReceiveStream(recv_config);
    EXPECT_NE(recv_stream, nullptr);

    MockTransport send_transport;
    AudioSendStream::Config send_config(&send_transport);
    send_config.rtp.ssrc = 777;
    AudioSendStream* send_stream = call->CreateAudioSendStream(send_config);
    EXPECT_NE(send_stream, nullptr);

    internal::AudioReceiveStream* internal_recv_stream =
        static_cast<internal::AudioReceiveStream*>(recv_stream);
    EXPECT_EQ(send_stream,
              internal_recv_stream->GetAssociatedSendStreamForTesting());

    call->DestroyAudioSendStream(send_stream);
    EXPECT_EQ(nullptr,
              internal_recv_stream->GetAssociatedSendStreamForTesting());

    call->DestroyAudioReceiveStream(recv_stream);
  }
}

TEST(CallTest, CreateDestroy_AssociateAudioSendReceiveStreams_SendFirst) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
    MockTransport send_transport;
    AudioSendStream::Config send_config(&send_transport);
    send_config.rtp.ssrc = 777;
    AudioSendStream* send_stream = call->CreateAudioSendStream(send_config);
    EXPECT_NE(send_stream, nullptr);

    AudioReceiveStream::Config recv_config;
    MockTransport rtcp_send_transport;
    recv_config.rtp.remote_ssrc = 42;
    recv_config.rtp.local_ssrc = 777;
    recv_config.rtcp_send_transport = &rtcp_send_transport;
    recv_config.decoder_factory =
        rtc::make_ref_counted<webrtc::MockAudioDecoderFactory>();
    AudioReceiveStream* recv_stream =
        call->CreateAudioReceiveStream(recv_config);
    EXPECT_NE(recv_stream, nullptr);

    internal::AudioReceiveStream* internal_recv_stream =
        static_cast<internal::AudioReceiveStream*>(recv_stream);
    EXPECT_EQ(send_stream,
              internal_recv_stream->GetAssociatedSendStreamForTesting());

    call->DestroyAudioReceiveStream(recv_stream);

    call->DestroyAudioSendStream(send_stream);
  }
}

TEST(CallTest, CreateDestroy_FlexfecReceiveStream) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
    MockTransport rtcp_send_transport;
    FlexfecReceiveStream::Config config(&rtcp_send_transport);
    config.payload_type = 118;
    config.rtp.remote_ssrc = 38837212;
    config.protected_media_ssrcs = {27273};

    FlexfecReceiveStream* stream = call->CreateFlexfecReceiveStream(config);
    EXPECT_NE(stream, nullptr);
    call->DestroyFlexfecReceiveStream(stream);
  }
}

TEST(CallTest, CreateDestroy_FlexfecReceiveStreams) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
    MockTransport rtcp_send_transport;
    FlexfecReceiveStream::Config config(&rtcp_send_transport);
    config.payload_type = 118;
    std::list<FlexfecReceiveStream*> streams;

    for (int i = 0; i < 2; ++i) {
      for (uint32_t ssrc = 0; ssrc < 1234567; ssrc += 34567) {
        config.rtp.remote_ssrc = ssrc;
        config.protected_media_ssrcs = {ssrc + 1};
        FlexfecReceiveStream* stream = call->CreateFlexfecReceiveStream(config);
        EXPECT_NE(stream, nullptr);
        if (ssrc & 1) {
          streams.push_back(stream);
        } else {
          streams.push_front(stream);
        }
      }
      for (auto s : streams) {
        call->DestroyFlexfecReceiveStream(s);
      }
      streams.clear();
    }
  }
}

TEST(CallTest, MultipleFlexfecReceiveStreamsProtectingSingleVideoStream) {
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);
    MockTransport rtcp_send_transport;
    FlexfecReceiveStream::Config config(&rtcp_send_transport);
    config.payload_type = 118;
    config.protected_media_ssrcs = {1324234};
    FlexfecReceiveStream* stream;
    std::list<FlexfecReceiveStream*> streams;

    config.rtp.remote_ssrc = 838383;
    stream = call->CreateFlexfecReceiveStream(config);
    EXPECT_NE(stream, nullptr);
    streams.push_back(stream);

    config.rtp.remote_ssrc = 424993;
    stream = call->CreateFlexfecReceiveStream(config);
    EXPECT_NE(stream, nullptr);
    streams.push_back(stream);

    config.rtp.remote_ssrc = 99383;
    stream = call->CreateFlexfecReceiveStream(config);
    EXPECT_NE(stream, nullptr);
    streams.push_back(stream);

    config.rtp.remote_ssrc = 5548;
    stream = call->CreateFlexfecReceiveStream(config);
    EXPECT_NE(stream, nullptr);
    streams.push_back(stream);

    for (auto s : streams) {
      call->DestroyFlexfecReceiveStream(s);
    }
  }
}

TEST(CallTest, RecreatingAudioStreamWithSameSsrcReusesRtpState) {
  constexpr uint32_t kSSRC = 12345;
  for (bool use_null_audio_processing : {false, true}) {
    CallHelper call(use_null_audio_processing);

    auto create_stream_and_get_rtp_state = [&](uint32_t ssrc) {
      MockTransport send_transport;
      AudioSendStream::Config config(&send_transport);
      config.rtp.ssrc = ssrc;
      AudioSendStream* stream = call->CreateAudioSendStream(config);
      const RtpState rtp_state =
          static_cast<internal::AudioSendStream*>(stream)->GetRtpState();
      call->DestroyAudioSendStream(stream);
      return rtp_state;
    };

    const RtpState rtp_state1 = create_stream_and_get_rtp_state(kSSRC);
    const RtpState rtp_state2 = create_stream_and_get_rtp_state(kSSRC);

    EXPECT_EQ(rtp_state1.sequence_number, rtp_state2.sequence_number);
    EXPECT_EQ(rtp_state1.start_timestamp, rtp_state2.start_timestamp);
    EXPECT_EQ(rtp_state1.timestamp, rtp_state2.timestamp);
    EXPECT_EQ(rtp_state1.capture_time_ms, rtp_state2.capture_time_ms);
    EXPECT_EQ(rtp_state1.last_timestamp_time_ms,
              rtp_state2.last_timestamp_time_ms);
  }
}

TEST(CallTest, AddAdaptationResourceAfterCreatingVideoSendStream) {
  CallHelper call(true);
  // Create a VideoSendStream.
  test::FunctionVideoEncoderFactory fake_encoder_factory([]() {
    return std::make_unique<test::FakeEncoder>(Clock::GetRealTimeClock());
  });
  auto bitrate_allocator_factory = CreateBuiltinVideoBitrateAllocatorFactory();
  MockTransport send_transport;
  VideoSendStream::Config config(&send_transport);
  config.rtp.payload_type = 110;
  config.rtp.ssrcs = {42};
  config.encoder_settings.encoder_factory = &fake_encoder_factory;
  config.encoder_settings.bitrate_allocator_factory =
      bitrate_allocator_factory.get();
  VideoEncoderConfig encoder_config;
  encoder_config.max_bitrate_bps = 1337;
  VideoSendStream* stream1 =
      call->CreateVideoSendStream(config.Copy(), encoder_config.Copy());
  EXPECT_NE(stream1, nullptr);
  config.rtp.ssrcs = {43};
  VideoSendStream* stream2 =
      call->CreateVideoSendStream(config.Copy(), encoder_config.Copy());
  EXPECT_NE(stream2, nullptr);
  // Add a fake resource.
  auto fake_resource = FakeResource::Create("FakeResource");
  call->AddAdaptationResource(fake_resource);
  // An adapter resource mirroring the |fake_resource| should now be present on
  // both streams.
  auto injected_resource1 = FindResourceWhoseNameContains(
      stream1->GetAdaptationResources(), fake_resource->Name());
  EXPECT_TRUE(injected_resource1);
  auto injected_resource2 = FindResourceWhoseNameContains(
      stream2->GetAdaptationResources(), fake_resource->Name());
  EXPECT_TRUE(injected_resource2);
  // Overwrite the real resource listeners with mock ones to verify the signal
  // gets through.
  injected_resource1->SetResourceListener(nullptr);
  StrictMock<MockResourceListener> resource_listener1;
  EXPECT_CALL(resource_listener1, OnResourceUsageStateMeasured(_, _))
      .Times(1)
      .WillOnce([injected_resource1](rtc::scoped_refptr<Resource> resource,
                                     ResourceUsageState usage_state) {
        EXPECT_EQ(injected_resource1, resource);
        EXPECT_EQ(ResourceUsageState::kOveruse, usage_state);
      });
  injected_resource1->SetResourceListener(&resource_listener1);
  injected_resource2->SetResourceListener(nullptr);
  StrictMock<MockResourceListener> resource_listener2;
  EXPECT_CALL(resource_listener2, OnResourceUsageStateMeasured(_, _))
      .Times(1)
      .WillOnce([injected_resource2](rtc::scoped_refptr<Resource> resource,
                                     ResourceUsageState usage_state) {
        EXPECT_EQ(injected_resource2, resource);
        EXPECT_EQ(ResourceUsageState::kOveruse, usage_state);
      });
  injected_resource2->SetResourceListener(&resource_listener2);
  // The kOveruse signal should get to our resource listeners.
  fake_resource->SetUsageState(ResourceUsageState::kOveruse);
  call->DestroyVideoSendStream(stream1);
  call->DestroyVideoSendStream(stream2);
}

TEST(CallTest, AddAdaptationResourceBeforeCreatingVideoSendStream) {
  CallHelper call(true);
  // Add a fake resource.
  auto fake_resource = FakeResource::Create("FakeResource");
  call->AddAdaptationResource(fake_resource);
  // Create a VideoSendStream.
  test::FunctionVideoEncoderFactory fake_encoder_factory([]() {
    return std::make_unique<test::FakeEncoder>(Clock::GetRealTimeClock());
  });
  auto bitrate_allocator_factory = CreateBuiltinVideoBitrateAllocatorFactory();
  MockTransport send_transport;
  VideoSendStream::Config config(&send_transport);
  config.rtp.payload_type = 110;
  config.rtp.ssrcs = {42};
  config.encoder_settings.encoder_factory = &fake_encoder_factory;
  config.encoder_settings.bitrate_allocator_factory =
      bitrate_allocator_factory.get();
  VideoEncoderConfig encoder_config;
  encoder_config.max_bitrate_bps = 1337;
  VideoSendStream* stream1 =
      call->CreateVideoSendStream(config.Copy(), encoder_config.Copy());
  EXPECT_NE(stream1, nullptr);
  config.rtp.ssrcs = {43};
  VideoSendStream* stream2 =
      call->CreateVideoSendStream(config.Copy(), encoder_config.Copy());
  EXPECT_NE(stream2, nullptr);
  // An adapter resource mirroring the |fake_resource| should be present on both
  // streams.
  auto injected_resource1 = FindResourceWhoseNameContains(
      stream1->GetAdaptationResources(), fake_resource->Name());
  EXPECT_TRUE(injected_resource1);
  auto injected_resource2 = FindResourceWhoseNameContains(
      stream2->GetAdaptationResources(), fake_resource->Name());
  EXPECT_TRUE(injected_resource2);
  // Overwrite the real resource listeners with mock ones to verify the signal
  // gets through.
  injected_resource1->SetResourceListener(nullptr);
  StrictMock<MockResourceListener> resource_listener1;
  EXPECT_CALL(resource_listener1, OnResourceUsageStateMeasured(_, _))
      .Times(1)
      .WillOnce([injected_resource1](rtc::scoped_refptr<Resource> resource,
                                     ResourceUsageState usage_state) {
        EXPECT_EQ(injected_resource1, resource);
        EXPECT_EQ(ResourceUsageState::kUnderuse, usage_state);
      });
  injected_resource1->SetResourceListener(&resource_listener1);
  injected_resource2->SetResourceListener(nullptr);
  StrictMock<MockResourceListener> resource_listener2;
  EXPECT_CALL(resource_listener2, OnResourceUsageStateMeasured(_, _))
      .Times(1)
      .WillOnce([injected_resource2](rtc::scoped_refptr<Resource> resource,
                                     ResourceUsageState usage_state) {
        EXPECT_EQ(injected_resource2, resource);
        EXPECT_EQ(ResourceUsageState::kUnderuse, usage_state);
      });
  injected_resource2->SetResourceListener(&resource_listener2);
  // The kUnderuse signal should get to our resource listeners.
  fake_resource->SetUsageState(ResourceUsageState::kUnderuse);
  call->DestroyVideoSendStream(stream1);
  call->DestroyVideoSendStream(stream2);
}

TEST(CallTest, SharedModuleThread) {
  class SharedModuleThreadUser : public Module {
   public:
    SharedModuleThreadUser(ProcessThread* expected_thread,
                           rtc::scoped_refptr<SharedModuleThread> thread)
        : expected_thread_(expected_thread), thread_(std::move(thread)) {
      thread_->EnsureStarted();
      thread_->process_thread()->RegisterModule(this, RTC_FROM_HERE);
    }

    ~SharedModuleThreadUser() override {
      thread_->process_thread()->DeRegisterModule(this);
      EXPECT_TRUE(thread_was_checked_);
    }

   private:
    int64_t TimeUntilNextProcess() override { return 1000; }
    void Process() override {}
    void ProcessThreadAttached(ProcessThread* process_thread) override {
      if (!process_thread) {
        // Being detached.
        return;
      }
      EXPECT_EQ(process_thread, expected_thread_);
      thread_was_checked_ = true;
    }

    bool thread_was_checked_ = false;
    ProcessThread* const expected_thread_;
    rtc::scoped_refptr<SharedModuleThread> thread_;
  };

  // Create our test instance and pass a lambda to it that gets executed when
  // the reference count goes back to 1 - meaning |shared| again is the only
  // reference, which means we can free the variable and deallocate the thread.
  rtc::scoped_refptr<SharedModuleThread> shared;
  shared =
      SharedModuleThread::Create(ProcessThread::Create("MySharedProcessThread"),
                                 [&shared]() { shared = nullptr; });
  ProcessThread* process_thread = shared->process_thread();

  ASSERT_TRUE(shared.get());

  {
    // Create a couple of users of the thread.
    // These instances are in a separate scope to trigger the callback to our
    // lambda, which will run when these go out of scope.
    SharedModuleThreadUser user1(process_thread, shared);
    SharedModuleThreadUser user2(process_thread, shared);
  }

  // The thread should now have been stopped and freed.
  EXPECT_FALSE(shared);
}

}  // namespace webrtc
