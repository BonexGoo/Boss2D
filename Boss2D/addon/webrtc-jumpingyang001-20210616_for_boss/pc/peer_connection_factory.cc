/*
 *  Copyright 2004 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__peer_connection_factory_h //original-code:"pc/peer_connection_factory.h"

#include <memory>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__strings__match_h //original-code:"absl/strings/match.h"
#include BOSS_WEBRTC_U_api__async_resolver_factory_h //original-code:"api/async_resolver_factory.h"
#include BOSS_WEBRTC_U_api__call__call_factory_interface_h //original-code:"api/call/call_factory_interface.h"
#include BOSS_WEBRTC_U_api__fec_controller_h //original-code:"api/fec_controller.h"
#include BOSS_WEBRTC_U_api__ice_transport_interface_h //original-code:"api/ice_transport_interface.h"
#include BOSS_WEBRTC_U_api__network_state_predictor_h //original-code:"api/network_state_predictor.h"
#include BOSS_WEBRTC_U_api__packet_socket_factory_h //original-code:"api/packet_socket_factory.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_h //original-code:"api/rtc_event_log/rtc_event_log.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__transport__bitrate_settings_h //original-code:"api/transport/bitrate_settings.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"
#include BOSS_WEBRTC_U_call__audio_state_h //original-code:"call/audio_state.h"
#include BOSS_WEBRTC_U_call__rtp_transport_controller_send_factory_h //original-code:"call/rtp_transport_controller_send_factory.h"
#include BOSS_WEBRTC_U_media__base__media_engine_h //original-code:"media/base/media_engine.h"
#include BOSS_WEBRTC_U_p2p__base__basic_async_resolver_factory_h //original-code:"p2p/base/basic_async_resolver_factory.h"
#include BOSS_WEBRTC_U_p2p__base__basic_packet_socket_factory_h //original-code:"p2p/base/basic_packet_socket_factory.h"
#include BOSS_WEBRTC_U_p2p__base__default_ice_transport_factory_h //original-code:"p2p/base/default_ice_transport_factory.h"
#include BOSS_WEBRTC_U_p2p__base__port_allocator_h //original-code:"p2p/base/port_allocator.h"
#include BOSS_WEBRTC_U_p2p__client__basic_port_allocator_h //original-code:"p2p/client/basic_port_allocator.h"
#include BOSS_WEBRTC_U_pc__audio_track_h //original-code:"pc/audio_track.h"
#include BOSS_WEBRTC_U_pc__local_audio_source_h //original-code:"pc/local_audio_source.h"
#include BOSS_WEBRTC_U_pc__media_stream_h //original-code:"pc/media_stream.h"
#include BOSS_WEBRTC_U_pc__media_stream_proxy_h //original-code:"pc/media_stream_proxy.h"
#include BOSS_WEBRTC_U_pc__media_stream_track_proxy_h //original-code:"pc/media_stream_track_proxy.h"
#include BOSS_WEBRTC_U_pc__peer_connection_h //original-code:"pc/peer_connection.h"
#include BOSS_WEBRTC_U_pc__peer_connection_factory_proxy_h //original-code:"pc/peer_connection_factory_proxy.h"
#include BOSS_WEBRTC_U_pc__peer_connection_proxy_h //original-code:"pc/peer_connection_proxy.h"
#include BOSS_WEBRTC_U_pc__rtp_parameters_conversion_h //original-code:"pc/rtp_parameters_conversion.h"
#include BOSS_WEBRTC_U_pc__session_description_h //original-code:"pc/session_description.h"
#include BOSS_WEBRTC_U_pc__video_track_h //original-code:"pc/video_track.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__field_trial_parser_h //original-code:"rtc_base/experiments/field_trial_parser.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__field_trial_units_h //original-code:"rtc_base/experiments/field_trial_units.h"
#include BOSS_WEBRTC_U_rtc_base__location_h //original-code:"rtc_base/location.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__rtc_certificate_generator_h //original-code:"rtc_base/rtc_certificate_generator.h"
#include BOSS_WEBRTC_U_rtc_base__system__file_wrapper_h //original-code:"rtc_base/system/file_wrapper.h"

namespace webrtc {

rtc::scoped_refptr<PeerConnectionFactoryInterface>
CreateModularPeerConnectionFactory(
    PeerConnectionFactoryDependencies dependencies) {
  // The PeerConnectionFactory must be created on the signaling thread.
  if (dependencies.signaling_thread &&
      !dependencies.signaling_thread->IsCurrent()) {
    return dependencies.signaling_thread
        ->Invoke<rtc::scoped_refptr<PeerConnectionFactoryInterface>>(
            RTC_FROM_HERE, [&dependencies] {
              return CreateModularPeerConnectionFactory(
                  std::move(dependencies));
            });
  }

  auto pc_factory = PeerConnectionFactory::Create(std::move(dependencies));
  if (!pc_factory) {
    return nullptr;
  }
  // Verify that the invocation and the initialization ended up agreeing on the
  // thread.
  RTC_DCHECK_RUN_ON(pc_factory->signaling_thread());
  return PeerConnectionFactoryProxy::Create(
      pc_factory->signaling_thread(), pc_factory->worker_thread(), pc_factory);
}

// Static
rtc::scoped_refptr<PeerConnectionFactory> PeerConnectionFactory::Create(
    PeerConnectionFactoryDependencies dependencies) {
  auto context = ConnectionContext::Create(&dependencies);
  if (!context) {
    return nullptr;
  }
  return rtc::make_ref_counted<PeerConnectionFactory>(context, &dependencies);
}

PeerConnectionFactory::PeerConnectionFactory(
    rtc::scoped_refptr<ConnectionContext> context,
    PeerConnectionFactoryDependencies* dependencies)
    : context_(context),
      task_queue_factory_(std::move(dependencies->task_queue_factory)),
      event_log_factory_(std::move(dependencies->event_log_factory)),
      fec_controller_factory_(std::move(dependencies->fec_controller_factory)),
      network_state_predictor_factory_(
          std::move(dependencies->network_state_predictor_factory)),
      injected_network_controller_factory_(
          std::move(dependencies->network_controller_factory)),
      neteq_factory_(std::move(dependencies->neteq_factory)),
      transport_controller_send_factory_(
          (dependencies->transport_controller_send_factory)
              ? std::move(dependencies->transport_controller_send_factory)
              : std::make_unique<RtpTransportControllerSendFactory>()) {}

PeerConnectionFactory::PeerConnectionFactory(
    PeerConnectionFactoryDependencies dependencies)
    : PeerConnectionFactory(ConnectionContext::Create(&dependencies),
                            &dependencies) {}

PeerConnectionFactory::~PeerConnectionFactory() {
  RTC_DCHECK_RUN_ON(signaling_thread());
}

void PeerConnectionFactory::SetOptions(const Options& options) {
  RTC_DCHECK_RUN_ON(signaling_thread());
  options_ = options;
}

RtpCapabilities PeerConnectionFactory::GetRtpSenderCapabilities(
    cricket::MediaType kind) const {
  RTC_DCHECK_RUN_ON(signaling_thread());
  switch (kind) {
    case cricket::MEDIA_TYPE_AUDIO: {
      cricket::AudioCodecs cricket_codecs;
      channel_manager()->GetSupportedAudioSendCodecs(&cricket_codecs);
      return ToRtpCapabilities(
          cricket_codecs,
          channel_manager()->GetDefaultEnabledAudioRtpHeaderExtensions());
    }
    case cricket::MEDIA_TYPE_VIDEO: {
      cricket::VideoCodecs cricket_codecs;
      channel_manager()->GetSupportedVideoSendCodecs(&cricket_codecs);
      return ToRtpCapabilities(
          cricket_codecs,
          channel_manager()->GetDefaultEnabledVideoRtpHeaderExtensions());
    }
    case cricket::MEDIA_TYPE_DATA:
      return RtpCapabilities();
    case cricket::MEDIA_TYPE_UNSUPPORTED:
      return RtpCapabilities();
  }
  RTC_DLOG(LS_ERROR) << "Got unexpected MediaType " << kind;
  RTC_CHECK_NOTREACHED();
}

RtpCapabilities PeerConnectionFactory::GetRtpReceiverCapabilities(
    cricket::MediaType kind) const {
  RTC_DCHECK_RUN_ON(signaling_thread());
  switch (kind) {
    case cricket::MEDIA_TYPE_AUDIO: {
      cricket::AudioCodecs cricket_codecs;
      channel_manager()->GetSupportedAudioReceiveCodecs(&cricket_codecs);
      return ToRtpCapabilities(
          cricket_codecs,
          channel_manager()->GetDefaultEnabledAudioRtpHeaderExtensions());
    }
    case cricket::MEDIA_TYPE_VIDEO: {
      cricket::VideoCodecs cricket_codecs;
      channel_manager()->GetSupportedVideoReceiveCodecs(&cricket_codecs);
      return ToRtpCapabilities(
          cricket_codecs,
          channel_manager()->GetDefaultEnabledVideoRtpHeaderExtensions());
    }
    case cricket::MEDIA_TYPE_DATA:
      return RtpCapabilities();
    case cricket::MEDIA_TYPE_UNSUPPORTED:
      return RtpCapabilities();
  }
  RTC_DLOG(LS_ERROR) << "Got unexpected MediaType " << kind;
  RTC_CHECK_NOTREACHED();
}

rtc::scoped_refptr<AudioSourceInterface>
PeerConnectionFactory::CreateAudioSource(const cricket::AudioOptions& options) {
  RTC_DCHECK(signaling_thread()->IsCurrent());
  rtc::scoped_refptr<LocalAudioSource> source(
      LocalAudioSource::Create(&options));
  return source;
}

bool PeerConnectionFactory::StartAecDump(FILE* file, int64_t max_size_bytes) {
  RTC_DCHECK_RUN_ON(worker_thread());
  return channel_manager()->StartAecDump(FileWrapper(file), max_size_bytes);
}

void PeerConnectionFactory::StopAecDump() {
  RTC_DCHECK_RUN_ON(worker_thread());
  channel_manager()->StopAecDump();
}

RTCErrorOr<rtc::scoped_refptr<PeerConnectionInterface>>
PeerConnectionFactory::CreatePeerConnectionOrError(
    const PeerConnectionInterface::RTCConfiguration& configuration,
    PeerConnectionDependencies dependencies) {
  RTC_DCHECK_RUN_ON(signaling_thread());
  RTC_DCHECK(!(dependencies.allocator && dependencies.packet_socket_factory))
      << "You can't set both allocator and packet_socket_factory; "
         "the former is going away (see bugs.webrtc.org/7447";

  // Set internal defaults if optional dependencies are not set.
  if (!dependencies.cert_generator) {
    dependencies.cert_generator =
        std::make_unique<rtc::RTCCertificateGenerator>(signaling_thread(),
                                                       network_thread());
  }
  if (!dependencies.allocator) {
    rtc::PacketSocketFactory* packet_socket_factory;
    if (dependencies.packet_socket_factory)
      packet_socket_factory = dependencies.packet_socket_factory.get();
    else
      packet_socket_factory = context_->default_socket_factory();

    dependencies.allocator = std::make_unique<cricket::BasicPortAllocator>(
        context_->default_network_manager(), packet_socket_factory,
        configuration.turn_customizer);
  }

  if (!dependencies.async_resolver_factory) {
    dependencies.async_resolver_factory =
        std::make_unique<webrtc::BasicAsyncResolverFactory>();
  }

  if (!dependencies.ice_transport_factory) {
    dependencies.ice_transport_factory =
        std::make_unique<DefaultIceTransportFactory>();
  }

  dependencies.allocator->SetNetworkIgnoreMask(options().network_ignore_mask);

  std::unique_ptr<RtcEventLog> event_log =
      worker_thread()->Invoke<std::unique_ptr<RtcEventLog>>(
          RTC_FROM_HERE, [this] { return CreateRtcEventLog_w(); });

  std::unique_ptr<Call> call = worker_thread()->Invoke<std::unique_ptr<Call>>(
      RTC_FROM_HERE,
      [this, &event_log] { return CreateCall_w(event_log.get()); });

  auto result = PeerConnection::Create(context_, options_, std::move(event_log),
                                       std::move(call), configuration,
                                       std::move(dependencies));
  if (!result.ok()) {
    return result.MoveError();
  }
  // We configure the proxy with a pointer to the network thread for methods
  // that need to be invoked there rather than on the signaling thread.
  // Internally, the proxy object has a member variable named |worker_thread_|
  // which will point to the network thread (and not the factory's
  // worker_thread()).  All such methods have thread checks though, so the code
  // should still be clear (outside of macro expansion).
  rtc::scoped_refptr<PeerConnectionInterface> result_proxy =
      PeerConnectionProxy::Create(signaling_thread(), network_thread(),
                                  result.MoveValue());
  return result_proxy;
}

rtc::scoped_refptr<MediaStreamInterface>
PeerConnectionFactory::CreateLocalMediaStream(const std::string& stream_id) {
  RTC_DCHECK(signaling_thread()->IsCurrent());
  return MediaStreamProxy::Create(signaling_thread(),
                                  MediaStream::Create(stream_id));
}

rtc::scoped_refptr<VideoTrackInterface> PeerConnectionFactory::CreateVideoTrack(
    const std::string& id,
    VideoTrackSourceInterface* source) {
  RTC_DCHECK(signaling_thread()->IsCurrent());
  rtc::scoped_refptr<VideoTrackInterface> track(
      VideoTrack::Create(id, source, worker_thread()));
  return VideoTrackProxy::Create(signaling_thread(), worker_thread(), track);
}

rtc::scoped_refptr<AudioTrackInterface> PeerConnectionFactory::CreateAudioTrack(
    const std::string& id,
    AudioSourceInterface* source) {
  RTC_DCHECK(signaling_thread()->IsCurrent());
  rtc::scoped_refptr<AudioTrackInterface> track(AudioTrack::Create(id, source));
  return AudioTrackProxy::Create(signaling_thread(), track);
}

cricket::ChannelManager* PeerConnectionFactory::channel_manager() {
  return context_->channel_manager();
}

std::unique_ptr<RtcEventLog> PeerConnectionFactory::CreateRtcEventLog_w() {
  RTC_DCHECK_RUN_ON(worker_thread());

  auto encoding_type = RtcEventLog::EncodingType::Legacy;
  if (IsTrialEnabled("WebRTC-RtcEventLogNewFormat"))
    encoding_type = RtcEventLog::EncodingType::NewFormat;
  return event_log_factory_
             ? event_log_factory_->CreateRtcEventLog(encoding_type)
             : std::make_unique<RtcEventLogNull>();
}

std::unique_ptr<Call> PeerConnectionFactory::CreateCall_w(
    RtcEventLog* event_log) {
  RTC_DCHECK_RUN_ON(worker_thread());

  webrtc::Call::Config call_config(event_log, network_thread());
  if (!channel_manager()->media_engine() || !context_->call_factory()) {
    return nullptr;
  }
  call_config.audio_state =
      channel_manager()->media_engine()->voice().GetAudioState();

  FieldTrialParameter<DataRate> min_bandwidth("min",
                                              DataRate::KilobitsPerSec(30));
  FieldTrialParameter<DataRate> start_bandwidth("start",
                                                DataRate::KilobitsPerSec(300));
  FieldTrialParameter<DataRate> max_bandwidth("max",
                                              DataRate::KilobitsPerSec(2000));
  ParseFieldTrial({&min_bandwidth, &start_bandwidth, &max_bandwidth},
                  trials().Lookup("WebRTC-PcFactoryDefaultBitrates"));

  call_config.bitrate_config.min_bitrate_bps =
      rtc::saturated_cast<int>(min_bandwidth->bps());
  call_config.bitrate_config.start_bitrate_bps =
      rtc::saturated_cast<int>(start_bandwidth->bps());
  call_config.bitrate_config.max_bitrate_bps =
      rtc::saturated_cast<int>(max_bandwidth->bps());

  call_config.fec_controller_factory = fec_controller_factory_.get();
  call_config.task_queue_factory = task_queue_factory_.get();
  call_config.network_state_predictor_factory =
      network_state_predictor_factory_.get();
  call_config.neteq_factory = neteq_factory_.get();

  if (IsTrialEnabled("WebRTC-Bwe-InjectedCongestionController")) {
    RTC_LOG(LS_INFO) << "Using injected network controller factory";
    call_config.network_controller_factory =
        injected_network_controller_factory_.get();
  } else {
    RTC_LOG(LS_INFO) << "Using default network controller factory";
  }

  call_config.trials = &trials();
  call_config.rtp_transport_controller_send_factory =
      transport_controller_send_factory_.get();
  return std::unique_ptr<Call>(
      context_->call_factory()->CreateCall(call_config));
}

bool PeerConnectionFactory::IsTrialEnabled(absl::string_view key) const {
  return absl::StartsWith(trials().Lookup(key), "Enabled");
}

}  // namespace webrtc
