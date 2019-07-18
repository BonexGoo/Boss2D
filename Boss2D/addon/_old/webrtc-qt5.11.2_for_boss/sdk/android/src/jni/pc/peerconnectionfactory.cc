/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "sdk/android/src/jni/pc/peerconnectionfactory.h"

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_api__peerconnectioninterface_h //original-code:"api/peerconnectioninterface.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_factory_h //original-code:"api/video_codecs/video_encoder_factory.h"
#include BOSS_WEBRTC_U_media__base__mediaengine_h //original-code:"media/base/mediaengine.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__audio_device_h //original-code:"modules/audio_device/include/audio_device.h"
#include "modules/utility/include/jvm_android.h"
// We don't depend on the audio processing module implementation.
// The user may pass in a nullptr.
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"  // nogncheck
#include BOSS_WEBRTC_U_rtc_base__event_tracer_h //original-code:"rtc_base/event_tracer.h"
#include BOSS_WEBRTC_U_rtc_base__stringutils_h //original-code:"rtc_base/stringutils.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include "sdk/android/generated_peerconnection_jni/jni/PeerConnectionFactory_jni.h"
#include "sdk/android/src/jni/jni_helpers.h"
#include "sdk/android/src/jni/pc/androidnetworkmonitor.h"
#include "sdk/android/src/jni/pc/audio.h"
#include "sdk/android/src/jni/pc/icecandidate.h"
#include "sdk/android/src/jni/pc/media.h"
#include "sdk/android/src/jni/pc/ownedfactoryandthreads.h"
#include "sdk/android/src/jni/pc/peerconnection.h"
#include "sdk/android/src/jni/pc/video.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"
// Adding 'nogncheck' to disable the gn include headers check.
// We don't want to depend on 'system_wrappers:field_trial_default' because
// clients should be able to provide their own implementation.
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_default_h //original-code:"system_wrappers/include/field_trial_default.h"  // nogncheck

namespace webrtc {
namespace jni {

namespace {
PeerConnectionFactoryInterface::Options
JavaToNativePeerConnectionFactoryOptions(JNIEnv* jni,
                                         const JavaRef<jobject>& options) {
  int network_ignore_mask = Java_Options_getNetworkIgnoreMask(jni, options);
  bool disable_encryption = Java_Options_getDisableEncryption(jni, options);
  bool disable_network_monitor =
      Java_Options_getDisableNetworkMonitor(jni, options);

  PeerConnectionFactoryInterface::Options native_options;

  // This doesn't necessarily match the c++ version of this struct; feel free
  // to add more parameters as necessary.
  native_options.network_ignore_mask = network_ignore_mask;
  native_options.disable_encryption = disable_encryption;
  native_options.disable_network_monitor = disable_network_monitor;
  return native_options;
}
}  // namespace

// Note: Some of the video-specific PeerConnectionFactory methods are
// implemented in "video.cc". This is done so that if an application
// doesn't need video support, it can just link with "null_video.cc"
// instead of "video.cc", which doesn't bring in the video-specific
// dependencies.

// Field trials initialization string
static char* field_trials_init_string = nullptr;

// Set in PeerConnectionFactory_initializeAndroidGlobals().
static bool factory_static_initialized = false;
static bool video_hw_acceleration_enabled = true;

void PeerConnectionFactoryNetworkThreadReady() {
  RTC_LOG(LS_INFO) << "Network thread JavaCallback";
  JNIEnv* env = AttachCurrentThreadIfNeeded();
  Java_PeerConnectionFactory_onNetworkThreadReady(env);
}

void PeerConnectionFactoryWorkerThreadReady() {
  RTC_LOG(LS_INFO) << "Worker thread JavaCallback";
  JNIEnv* env = AttachCurrentThreadIfNeeded();
  Java_PeerConnectionFactory_onWorkerThreadReady(env);
}

void PeerConnectionFactorySignalingThreadReady() {
  RTC_LOG(LS_INFO) << "Signaling thread JavaCallback";
  JNIEnv* env = AttachCurrentThreadIfNeeded();
  Java_PeerConnectionFactory_onSignalingThreadReady(env);
}

static void JNI_PeerConnectionFactory_InitializeAndroidGlobals(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    const JavaParamRef<jobject>& context,
    jboolean video_hw_acceleration) {
  video_hw_acceleration_enabled = video_hw_acceleration;
  if (!factory_static_initialized) {
    JVM::Initialize(GetJVM());
    factory_static_initialized = true;
  }
}

static void JNI_PeerConnectionFactory_InitializeFieldTrials(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    const JavaParamRef<jstring>& j_trials_init_string) {
  field_trials_init_string = NULL;
  if (!j_trials_init_string.is_null()) {
    const char* init_string =
        jni->GetStringUTFChars(j_trials_init_string.obj(), NULL);
    int init_string_length =
        jni->GetStringUTFLength(j_trials_init_string.obj());
    field_trials_init_string = new char[init_string_length + 1];
    rtc::strcpyn(field_trials_init_string, init_string_length + 1, init_string);
    jni->ReleaseStringUTFChars(j_trials_init_string.obj(), init_string);
    RTC_LOG(LS_INFO) << "initializeFieldTrials: " << field_trials_init_string;
  }
  field_trial::InitFieldTrialsFromString(field_trials_init_string);
}

static void JNI_PeerConnectionFactory_InitializeInternalTracer(
    JNIEnv* jni,
    const JavaParamRef<jclass>&) {
  rtc::tracing::SetupInternalTracer();
}

static ScopedJavaLocalRef<jstring>
JNI_PeerConnectionFactory_FindFieldTrialsFullName(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    const JavaParamRef<jstring>& j_name) {
  return NativeToJavaString(
      jni, field_trial::FindFullName(JavaToStdString(jni, j_name)));
}

static jboolean JNI_PeerConnectionFactory_StartInternalTracingCapture(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    const JavaParamRef<jstring>& j_event_tracing_filename) {
  if (j_event_tracing_filename.is_null())
    return false;

  const char* init_string =
      jni->GetStringUTFChars(j_event_tracing_filename.obj(), NULL);
  RTC_LOG(LS_INFO) << "Starting internal tracing to: " << init_string;
  bool ret = rtc::tracing::StartInternalCapture(init_string);
  jni->ReleaseStringUTFChars(j_event_tracing_filename.obj(), init_string);
  return ret;
}

static void JNI_PeerConnectionFactory_StopInternalTracingCapture(
    JNIEnv* jni,
    const JavaParamRef<jclass>&) {
  rtc::tracing::StopInternalCapture();
}

static void JNI_PeerConnectionFactory_ShutdownInternalTracer(
    JNIEnv* jni,
    const JavaParamRef<jclass>&) {
  rtc::tracing::ShutdownInternalTracer();
}

jlong CreatePeerConnectionFactoryForJava(
    JNIEnv* jni,
    const JavaParamRef<jobject>& joptions,
    const JavaParamRef<jobject>& jencoder_factory,
    const JavaParamRef<jobject>& jdecoder_factory,
    rtc::scoped_refptr<AudioProcessing> audio_processor) {
  // talk/ assumes pretty widely that the current Thread is ThreadManager'd, but
  // ThreadManager only WrapCurrentThread()s the thread where it is first
  // created.  Since the semantics around when auto-wrapping happens in
  // webrtc/rtc_base/ are convoluted, we simply wrap here to avoid having to
  // think about ramifications of auto-wrapping there.
  rtc::ThreadManager::Instance()->WrapCurrentThread();

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

  rtc::NetworkMonitorFactory* network_monitor_factory = nullptr;
  auto audio_encoder_factory = CreateAudioEncoderFactory();
  auto audio_decoder_factory = CreateAudioDecoderFactory();

  PeerConnectionFactoryInterface::Options options;
  bool has_options = !joptions.is_null();
  if (has_options) {
    options = JavaToNativePeerConnectionFactoryOptions(jni, joptions);
  }

  // Do not create network_monitor_factory only if the options are
  // provided and disable_network_monitor therein is set to true.
  if (!(has_options && options.disable_network_monitor)) {
    network_monitor_factory = new AndroidNetworkMonitorFactory();
    rtc::NetworkMonitorFactory::SetFactory(network_monitor_factory);
  }

  AudioDeviceModule* adm = nullptr;
  rtc::scoped_refptr<AudioMixer> audio_mixer = nullptr;
  std::unique_ptr<CallFactoryInterface> call_factory(CreateCallFactory());
  std::unique_ptr<RtcEventLogFactoryInterface> rtc_event_log_factory(
      CreateRtcEventLogFactory());

  cricket::WebRtcVideoEncoderFactory* legacy_video_encoder_factory = nullptr;
  cricket::WebRtcVideoDecoderFactory* legacy_video_decoder_factory = nullptr;
  std::unique_ptr<cricket::MediaEngineInterface> media_engine;
  if (jencoder_factory.is_null() && jdecoder_factory.is_null()) {
    // This uses the legacy API, which automatically uses the internal SW
    // codecs in WebRTC.
    if (video_hw_acceleration_enabled) {
      legacy_video_encoder_factory = CreateLegacyVideoEncoderFactory();
      legacy_video_decoder_factory = CreateLegacyVideoDecoderFactory();
    }
    media_engine.reset(CreateMediaEngine(
        adm, audio_encoder_factory, audio_decoder_factory,
        legacy_video_encoder_factory, legacy_video_decoder_factory, audio_mixer,
        audio_processor));
  } else {
    // This uses the new API, does not automatically include software codecs.
    std::unique_ptr<VideoEncoderFactory> video_encoder_factory = nullptr;
    if (jencoder_factory.is_null()) {
      legacy_video_encoder_factory = CreateLegacyVideoEncoderFactory();
      video_encoder_factory = std::unique_ptr<VideoEncoderFactory>(
          WrapLegacyVideoEncoderFactory(legacy_video_encoder_factory));
    } else {
      video_encoder_factory = std::unique_ptr<VideoEncoderFactory>(
          CreateVideoEncoderFactory(jni, jencoder_factory));
    }

    std::unique_ptr<VideoDecoderFactory> video_decoder_factory = nullptr;
    if (jdecoder_factory.is_null()) {
      legacy_video_decoder_factory = CreateLegacyVideoDecoderFactory();
      video_decoder_factory = std::unique_ptr<VideoDecoderFactory>(
          WrapLegacyVideoDecoderFactory(legacy_video_decoder_factory));
    } else {
      video_decoder_factory = std::unique_ptr<VideoDecoderFactory>(
          CreateVideoDecoderFactory(jni, jdecoder_factory));
    }

    rtc::scoped_refptr<AudioDeviceModule> adm_scoped = nullptr;
    media_engine.reset(CreateMediaEngine(
        adm_scoped, audio_encoder_factory, audio_decoder_factory,
        std::move(video_encoder_factory), std::move(video_decoder_factory),
        audio_mixer, audio_processor));
  }

  rtc::scoped_refptr<PeerConnectionFactoryInterface> factory(
      CreateModularPeerConnectionFactory(
          network_thread.get(), worker_thread.get(), signaling_thread.get(),
          std::move(media_engine), std::move(call_factory),
          std::move(rtc_event_log_factory)));
  RTC_CHECK(factory) << "Failed to create the peer connection factory; "
                     << "WebRTC/libjingle init likely failed on this device";
  // TODO(honghaiz): Maybe put the options as the argument of
  // CreatePeerConnectionFactory.
  if (has_options) {
    factory->SetOptions(options);
  }
  OwnedFactoryAndThreads* owned_factory = new OwnedFactoryAndThreads(
      std::move(network_thread), std::move(worker_thread),
      std::move(signaling_thread), legacy_video_encoder_factory,
      legacy_video_decoder_factory, network_monitor_factory, factory.release());
  owned_factory->InvokeJavaCallbacksOnFactoryThreads();
  return jlongFromPointer(owned_factory);
}

static jlong JNI_PeerConnectionFactory_CreatePeerConnectionFactory(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    const JavaParamRef<jobject>& joptions,
    const JavaParamRef<jobject>& jencoder_factory,
    const JavaParamRef<jobject>& jdecoder_factory) {
  return CreatePeerConnectionFactoryForJava(jni, joptions, jencoder_factory,
                                            jdecoder_factory,
                                            CreateAudioProcessing());
}

static jlong
JNI_PeerConnectionFactory_CreatePeerConnectionFactoryWithAudioProcessing(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    const JavaParamRef<jobject>& joptions,
    const JavaParamRef<jobject>& jencoder_factory,
    const JavaParamRef<jobject>& jdecoder_factory,
    jlong native_audio_processor) {
  rtc::scoped_refptr<AudioProcessing> audio_processor =
      reinterpret_cast<AudioProcessing*>(native_audio_processor);
  RTC_DCHECK(audio_processor);
  return CreatePeerConnectionFactoryForJava(jni, joptions, jencoder_factory,
                                            jdecoder_factory, audio_processor);
}

static void JNI_PeerConnectionFactory_FreeFactory(JNIEnv*,
                                                  const JavaParamRef<jclass>&,
                                                  jlong j_p) {
  delete reinterpret_cast<OwnedFactoryAndThreads*>(j_p);
  if (field_trials_init_string) {
    field_trial::InitFieldTrialsFromString(NULL);
    delete field_trials_init_string;
    field_trials_init_string = NULL;
  }
}

static void JNI_PeerConnectionFactory_InvokeThreadsCallbacks(
    JNIEnv*,
    const JavaParamRef<jclass>&,
    jlong j_p) {
  OwnedFactoryAndThreads* factory =
      reinterpret_cast<OwnedFactoryAndThreads*>(j_p);
  factory->InvokeJavaCallbacksOnFactoryThreads();
}

static jlong JNI_PeerConnectionFactory_CreateLocalMediaStream(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    jlong native_factory,
    const JavaParamRef<jstring>& label) {
  rtc::scoped_refptr<PeerConnectionFactoryInterface> factory(
      factoryFromJava(native_factory));
  rtc::scoped_refptr<MediaStreamInterface> stream(
      factory->CreateLocalMediaStream(JavaToStdString(jni, label)));
  return jlongFromPointer(stream.release());
}

static jlong JNI_PeerConnectionFactory_CreateAudioSource(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    jlong native_factory,
    const JavaParamRef<jobject>& j_constraints) {
  std::unique_ptr<MediaConstraintsInterface> constraints =
      JavaToNativeMediaConstraints(jni, j_constraints);
  rtc::scoped_refptr<PeerConnectionFactoryInterface> factory(
      factoryFromJava(native_factory));
  cricket::AudioOptions options;
  CopyConstraintsIntoAudioOptions(constraints.get(), &options);
  rtc::scoped_refptr<AudioSourceInterface> source(
      factory->CreateAudioSource(options));
  return jlongFromPointer(source.release());
}

jlong JNI_PeerConnectionFactory_CreateAudioTrack(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    jlong native_factory,
    const JavaParamRef<jstring>& id,
    jlong native_source) {
  rtc::scoped_refptr<PeerConnectionFactoryInterface> factory(
      factoryFromJava(native_factory));
  rtc::scoped_refptr<AudioTrackInterface> track(factory->CreateAudioTrack(
      JavaToStdString(jni, id),
      reinterpret_cast<AudioSourceInterface*>(native_source)));
  return jlongFromPointer(track.release());
}

static jboolean JNI_PeerConnectionFactory_StartAecDump(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    jlong native_factory,
    jint file,
    jint filesize_limit_bytes) {
  rtc::scoped_refptr<PeerConnectionFactoryInterface> factory(
      factoryFromJava(native_factory));
  return factory->StartAecDump(file, filesize_limit_bytes);
}

static void JNI_PeerConnectionFactory_StopAecDump(JNIEnv* jni,
                                                  const JavaParamRef<jclass>&,
                                                  jlong native_factory) {
  rtc::scoped_refptr<PeerConnectionFactoryInterface> factory(
      factoryFromJava(native_factory));
  factory->StopAecDump();
}

static void JNI_PeerConnectionFactory_SetOptions(
    JNIEnv* jni,
    const JavaParamRef<jobject>&,
    jlong native_factory,
    const JavaParamRef<jobject>& options) {
  rtc::scoped_refptr<PeerConnectionFactoryInterface> factory(
      factoryFromJava(native_factory));
  PeerConnectionFactoryInterface::Options options_to_set =
      JavaToNativePeerConnectionFactoryOptions(jni, options);
  factory->SetOptions(options_to_set);

  if (options_to_set.disable_network_monitor) {
    OwnedFactoryAndThreads* owner =
        reinterpret_cast<OwnedFactoryAndThreads*>(native_factory);
    if (owner->network_monitor_factory()) {
      rtc::NetworkMonitorFactory::ReleaseFactory(
          owner->network_monitor_factory());
      owner->clear_network_monitor_factory();
    }
  }
}

static jlong JNI_PeerConnectionFactory_CreatePeerConnection(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    jlong factory,
    const JavaParamRef<jobject>& j_rtc_config,
    const JavaParamRef<jobject>& j_constraints,
    jlong observer_p) {
  rtc::scoped_refptr<PeerConnectionFactoryInterface> f(
      reinterpret_cast<PeerConnectionFactoryInterface*>(
          factoryFromJava(factory)));
  std::unique_ptr<PeerConnectionObserver> observer(
      reinterpret_cast<PeerConnectionObserver*>(observer_p));

  PeerConnectionInterface::RTCConfiguration rtc_config(
      PeerConnectionInterface::RTCConfigurationType::kAggressive);
  JavaToNativeRTCConfiguration(jni, j_rtc_config, &rtc_config);

  // Generate non-default certificate.
  rtc::KeyType key_type = GetRtcConfigKeyType(jni, j_rtc_config);
  if (key_type != rtc::KT_DEFAULT) {
    rtc::scoped_refptr<rtc::RTCCertificate> certificate =
        rtc::RTCCertificateGenerator::GenerateCertificate(
            rtc::KeyParams(key_type), rtc::nullopt);
    if (!certificate) {
      RTC_LOG(LS_ERROR) << "Failed to generate certificate. KeyType: "
                        << key_type;
      return 0;
    }
    rtc_config.certificates.push_back(certificate);
  }

  std::unique_ptr<MediaConstraintsInterface> constraints;
  if (!j_constraints.is_null()) {
    constraints = JavaToNativeMediaConstraints(jni, j_constraints);
    CopyConstraintsIntoRtcConfiguration(constraints.get(), &rtc_config);
  }
  rtc::scoped_refptr<PeerConnectionInterface> pc(
      f->CreatePeerConnection(rtc_config, nullptr, nullptr, observer.get()));
  return jlongFromPointer(
      new OwnedPeerConnection(pc, std::move(observer), std::move(constraints)));
}

static jlong JNI_PeerConnectionFactory_CreateVideoSource(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    jlong native_factory,
    const JavaParamRef<jobject>& j_surface_texture_helper,
    jboolean is_screencast) {
  OwnedFactoryAndThreads* factory =
      reinterpret_cast<OwnedFactoryAndThreads*>(native_factory);
  return jlongFromPointer(CreateVideoSource(
      jni, factory->signaling_thread(), factory->worker_thread(),
      j_surface_texture_helper, is_screencast));
}

static jlong JNI_PeerConnectionFactory_CreateVideoTrack(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    jlong native_factory,
    const JavaParamRef<jstring>& id,
    jlong native_source) {
  rtc::scoped_refptr<PeerConnectionFactoryInterface> factory(
      factoryFromJava(native_factory));
  rtc::scoped_refptr<VideoTrackInterface> track(factory->CreateVideoTrack(
      JavaToStdString(jni, id),
      reinterpret_cast<VideoTrackSourceInterface*>(native_source)));
  return jlongFromPointer(track.release());
}

static void JNI_PeerConnectionFactory_SetVideoHwAccelerationOptions(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    jlong native_factory,
    const JavaParamRef<jobject>& local_egl_context,
    const JavaParamRef<jobject>& remote_egl_context) {
  OwnedFactoryAndThreads* owned_factory =
      reinterpret_cast<OwnedFactoryAndThreads*>(native_factory);
  SetEglContext(jni, owned_factory->legacy_encoder_factory(),
                local_egl_context);
  SetEglContext(jni, owned_factory->legacy_decoder_factory(),
                remote_egl_context);
}

static jlong JNI_PeerConnectionFactory_GetNativePeerConnectionFactory(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    jlong native_factory) {
  return jlongFromPointer(factoryFromJava(native_factory));
}

}  // namespace jni
}  // namespace webrtc
