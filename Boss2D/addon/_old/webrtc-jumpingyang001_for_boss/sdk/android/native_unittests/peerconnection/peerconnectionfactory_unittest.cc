/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "sdk/android/native_api/peerconnection/peerconnectionfactory.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_decoder_factory_h //original-code:"api/audio_codecs/builtin_audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_encoder_factory_h //original-code:"api/audio_codecs/builtin_audio_encoder_factory.h"
#include BOSS_WEBRTC_U_media__base__mediaengine_h //original-code:"media/base/mediaengine.h"
#include BOSS_WEBRTC_U_media__engine__internaldecoderfactory_h //original-code:"media/engine/internaldecoderfactory.h"
#include BOSS_WEBRTC_U_media__engine__internalencoderfactory_h //original-code:"media/engine/internalencoderfactory.h"
#include BOSS_WEBRTC_U_media__engine__webrtcmediaengine_h //original-code:"media/engine/webrtcmediaengine.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include "sdk/android/generated_native_unittests_jni/jni/PeerConnectionFactoryInitializationHelper_jni.h"
#include "sdk/android/native_api/jni/jvm.h"
#include "test/gtest.h"

namespace webrtc {
namespace test {
namespace {

// Create native peer connection factory, that will be wrapped by java one
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> CreateTestPCF(
    rtc::Thread* network_thread,
    rtc::Thread* worker_thread,
    rtc::Thread* signaling_thread) {
  // talk/ assumes pretty widely that the current Thread is ThreadManager'd, but
  // ThreadManager only WrapCurrentThread()s the thread where it is first
  // created.  Since the semantics around when auto-wrapping happens in
  // webrtc/rtc_base/ are convoluted, we simply wrap here to avoid having to
  // think about ramifications of auto-wrapping there.
  rtc::ThreadManager::Instance()->WrapCurrentThread();

  std::unique_ptr<cricket::MediaEngineInterface> media_engine =
      cricket::WebRtcMediaEngineFactory::Create(
          nullptr /* adm */, webrtc::CreateBuiltinAudioEncoderFactory(),
          webrtc::CreateBuiltinAudioDecoderFactory(),
          absl::make_unique<webrtc::InternalEncoderFactory>(),
          absl::make_unique<webrtc::InternalDecoderFactory>(),
          nullptr /* audio_mixer */, webrtc::AudioProcessingBuilder().Create());
  RTC_LOG(LS_INFO) << "Media engine created: " << media_engine.get();

  auto factory = CreateModularPeerConnectionFactory(
      network_thread, worker_thread, signaling_thread, std::move(media_engine),
      webrtc::CreateCallFactory(), webrtc::CreateRtcEventLogFactory());
  RTC_LOG(LS_INFO) << "PeerConnectionFactory created: " << factory;
  RTC_CHECK(factory) << "Failed to create the peer connection factory; "
                     << "WebRTC/libjingle init likely failed on this device";

  return factory;
}

TEST(PeerConnectionFactoryTest, NativeToJavaPeerConnectionFactory) {
  JNIEnv* jni = AttachCurrentThreadIfNeeded();

  RTC_LOG(INFO) << "Initializing java peer connection factory.";
  jni::Java_PeerConnectionFactoryInitializationHelper_initializeFactoryForTests(
      jni);
  RTC_LOG(INFO) << "Java peer connection factory initialized.";

  // Create threads.
  std::unique_ptr<rtc::Thread> network_thread =
      rtc::Thread::CreateWithSocketServer();
  network_thread->SetName("network_thread", nullptr);
  RTC_CHECK(network_thread->Start()) << "Failed to start thread";

  std::unique_ptr<rtc::Thread> worker_thread = rtc::Thread::Create();
  worker_thread->SetName("worker_thread", nullptr);
  RTC_CHECK(worker_thread->Start()) << "Failed to start thread";

  std::unique_ptr<rtc::Thread> signaling_thread = rtc::Thread::Create();
  signaling_thread->SetName("signaling_thread", NULL);
  RTC_CHECK(signaling_thread->Start()) << "Failed to start thread";

  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory =
      CreateTestPCF(network_thread.get(), worker_thread.get(),
                    signaling_thread.get());

  jobject java_factory = NativeToJavaPeerConnectionFactory(
      jni, factory, std::move(network_thread), std::move(worker_thread),
      std::move(signaling_thread), nullptr /* network_monitor_factory */);

  RTC_LOG(INFO) << java_factory;

  EXPECT_NE(java_factory, nullptr);
}

}  // namespace
}  // namespace test
}  // namespace webrtc
