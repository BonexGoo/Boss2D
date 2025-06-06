/*
 *  Copyright 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "pc/test/peer_connection_test_wrapper.h"

#include <stddef.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__audio__audio_mixer_h //original-code:"api/audio/audio_mixer.h"
#include BOSS_WEBRTC_U_api__create_peerconnection_factory_h //original-code:"api/create_peerconnection_factory.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__video_codecs__builtin_video_decoder_factory_h //original-code:"api/video_codecs/builtin_video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__builtin_video_encoder_factory_h //original-code:"api/video_codecs/builtin_video_encoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_factory_h //original-code:"api/video_codecs/video_encoder_factory.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__audio_device_h //original-code:"modules/audio_device/include/audio_device.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_p2p__base__fake_port_allocator_h //original-code:"p2p/base/fake_port_allocator.h"
#include BOSS_WEBRTC_U_p2p__base__port_allocator_h //original-code:"p2p/base/port_allocator.h"
#include "pc/test/fake_periodic_video_source.h"
#include "pc/test/fake_periodic_video_track_source.h"
#include "pc/test/fake_rtc_certificate_generator.h"
#include "pc/test/mock_peer_connection_observers.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__rtc_certificate_generator_h //original-code:"rtc_base/rtc_certificate_generator.h"
#include BOSS_WEBRTC_U_rtc_base__string_encode_h //original-code:"rtc_base/string_encode.h"
#include BOSS_WEBRTC_U_rtc_base__time_utils_h //original-code:"rtc_base/time_utils.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

using webrtc::FakeVideoTrackRenderer;
using webrtc::IceCandidateInterface;
using webrtc::MediaStreamInterface;
using webrtc::MediaStreamTrackInterface;
using webrtc::MockSetSessionDescriptionObserver;
using webrtc::PeerConnectionInterface;
using webrtc::RtpReceiverInterface;
using webrtc::SdpType;
using webrtc::SessionDescriptionInterface;
using webrtc::VideoTrackInterface;

namespace {
const char kStreamIdBase[] = "stream_id";
const char kVideoTrackLabelBase[] = "video_track";
const char kAudioTrackLabelBase[] = "audio_track";
constexpr int kMaxWait = 10000;
constexpr int kTestAudioFrameCount = 3;
constexpr int kTestVideoFrameCount = 3;
}  // namespace

void PeerConnectionTestWrapper::Connect(PeerConnectionTestWrapper* caller,
                                        PeerConnectionTestWrapper* callee) {
  caller->SignalOnIceCandidateReady.connect(
      callee, &PeerConnectionTestWrapper::AddIceCandidate);
  callee->SignalOnIceCandidateReady.connect(
      caller, &PeerConnectionTestWrapper::AddIceCandidate);

  caller->SignalOnSdpReady.connect(callee,
                                   &PeerConnectionTestWrapper::ReceiveOfferSdp);
  callee->SignalOnSdpReady.connect(
      caller, &PeerConnectionTestWrapper::ReceiveAnswerSdp);
}

PeerConnectionTestWrapper::PeerConnectionTestWrapper(
    const std::string& name,
    rtc::Thread* network_thread,
    rtc::Thread* worker_thread)
    : name_(name),
      network_thread_(network_thread),
      worker_thread_(worker_thread),
      pending_negotiation_(false) {
  pc_thread_checker_.Detach();
}

PeerConnectionTestWrapper::~PeerConnectionTestWrapper() {
  RTC_DCHECK_RUN_ON(&pc_thread_checker_);
  // Either network_thread or worker_thread might be active at this point.
  // Relying on ~PeerConnection to properly wait for them doesn't work,
  // as a vptr race might occur (before we enter the destruction body).
  // See: bugs.webrtc.org/9847
  if (pc()) {
    pc()->Close();
  }
}

bool PeerConnectionTestWrapper::CreatePc(
    const webrtc::PeerConnectionInterface::RTCConfiguration& config,
    rtc::scoped_refptr<webrtc::AudioEncoderFactory> audio_encoder_factory,
    rtc::scoped_refptr<webrtc::AudioDecoderFactory> audio_decoder_factory) {
  std::unique_ptr<cricket::PortAllocator> port_allocator(
      new cricket::FakePortAllocator(network_thread_, nullptr));

  RTC_DCHECK_RUN_ON(&pc_thread_checker_);

  fake_audio_capture_module_ = FakeAudioCaptureModule::Create();
  if (fake_audio_capture_module_ == NULL) {
    return false;
  }

  peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
      network_thread_, worker_thread_, rtc::Thread::Current(),
      rtc::scoped_refptr<webrtc::AudioDeviceModule>(fake_audio_capture_module_),
      audio_encoder_factory, audio_decoder_factory,
      webrtc::CreateBuiltinVideoEncoderFactory(),
      webrtc::CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
      nullptr /* audio_processing */);
  if (!peer_connection_factory_) {
    return false;
  }

  std::unique_ptr<rtc::RTCCertificateGeneratorInterface> cert_generator(
      new FakeRTCCertificateGenerator());
  webrtc::PeerConnectionDependencies deps(this);
  deps.allocator = std::move(port_allocator);
  deps.cert_generator = std::move(cert_generator);
  auto result = peer_connection_factory_->CreatePeerConnectionOrError(
      config, std::move(deps));
  if (result.ok()) {
    peer_connection_ = result.MoveValue();
    return true;
  } else {
    return false;
  }
}

rtc::scoped_refptr<webrtc::DataChannelInterface>
PeerConnectionTestWrapper::CreateDataChannel(
    const std::string& label,
    const webrtc::DataChannelInit& init) {
  auto result = peer_connection_->CreateDataChannelOrError(label, &init);
  if (!result.ok()) {
    RTC_LOG(LS_ERROR) << "CreateDataChannel failed: "
                      << ToString(result.error().type()) << " "
                      << result.error().message();
    return nullptr;
  }
  return result.MoveValue();
}

void PeerConnectionTestWrapper::WaitForNegotiation() {
  EXPECT_TRUE_WAIT(!pending_negotiation_, kMaxWait);
}

void PeerConnectionTestWrapper::OnSignalingChange(
    webrtc::PeerConnectionInterface::SignalingState new_state) {
  if (new_state == webrtc::PeerConnectionInterface::SignalingState::kStable) {
    pending_negotiation_ = false;
  }
}

void PeerConnectionTestWrapper::OnAddTrack(
    rtc::scoped_refptr<RtpReceiverInterface> receiver,
    const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams) {
  RTC_LOG(LS_INFO) << "PeerConnectionTestWrapper " << name_ << ": OnAddTrack";
  if (receiver->track()->kind() == MediaStreamTrackInterface::kVideoKind) {
    auto* video_track =
        static_cast<VideoTrackInterface*>(receiver->track().get());
    renderer_ = std::make_unique<FakeVideoTrackRenderer>(video_track);
  }
}

void PeerConnectionTestWrapper::OnIceCandidate(
    const IceCandidateInterface* candidate) {
  std::string sdp;
  EXPECT_TRUE(candidate->ToString(&sdp));
  // Give the user a chance to modify sdp for testing.
  SignalOnIceCandidateCreated(&sdp);
  SignalOnIceCandidateReady(candidate->sdp_mid(), candidate->sdp_mline_index(),
                            sdp);
}

void PeerConnectionTestWrapper::OnDataChannel(
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) {
  SignalOnDataChannel(data_channel);
}

void PeerConnectionTestWrapper::OnSuccess(SessionDescriptionInterface* desc) {
  // This callback should take the ownership of |desc|.
  std::unique_ptr<SessionDescriptionInterface> owned_desc(desc);
  std::string sdp;
  EXPECT_TRUE(desc->ToString(&sdp));

  RTC_LOG(LS_INFO) << "PeerConnectionTestWrapper " << name_ << ": "
                   << webrtc::SdpTypeToString(desc->GetType())
                   << " sdp created: " << sdp;

  // Give the user a chance to modify sdp for testing.
  SignalOnSdpCreated(&sdp);

  SetLocalDescription(desc->GetType(), sdp);

  SignalOnSdpReady(sdp);
}

void PeerConnectionTestWrapper::CreateOffer(
    const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options) {
  RTC_LOG(LS_INFO) << "PeerConnectionTestWrapper " << name_ << ": CreateOffer.";
  pending_negotiation_ = true;
  peer_connection_->CreateOffer(this, options);
}

void PeerConnectionTestWrapper::CreateAnswer(
    const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options) {
  RTC_LOG(LS_INFO) << "PeerConnectionTestWrapper " << name_
                   << ": CreateAnswer.";
  pending_negotiation_ = true;
  peer_connection_->CreateAnswer(this, options);
}

void PeerConnectionTestWrapper::ReceiveOfferSdp(const std::string& sdp) {
  SetRemoteDescription(SdpType::kOffer, sdp);
  CreateAnswer(webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
}

void PeerConnectionTestWrapper::ReceiveAnswerSdp(const std::string& sdp) {
  SetRemoteDescription(SdpType::kAnswer, sdp);
}

void PeerConnectionTestWrapper::SetLocalDescription(SdpType type,
                                                    const std::string& sdp) {
  RTC_LOG(LS_INFO) << "PeerConnectionTestWrapper " << name_
                   << ": SetLocalDescription " << webrtc::SdpTypeToString(type)
                   << " " << sdp;

  auto observer = rtc::make_ref_counted<MockSetSessionDescriptionObserver>();
  peer_connection_->SetLocalDescription(
      observer, webrtc::CreateSessionDescription(type, sdp).release());
}

void PeerConnectionTestWrapper::SetRemoteDescription(SdpType type,
                                                     const std::string& sdp) {
  RTC_LOG(LS_INFO) << "PeerConnectionTestWrapper " << name_
                   << ": SetRemoteDescription " << webrtc::SdpTypeToString(type)
                   << " " << sdp;

  auto observer = rtc::make_ref_counted<MockSetSessionDescriptionObserver>();
  peer_connection_->SetRemoteDescription(
      observer, webrtc::CreateSessionDescription(type, sdp).release());
}

void PeerConnectionTestWrapper::AddIceCandidate(const std::string& sdp_mid,
                                                int sdp_mline_index,
                                                const std::string& candidate) {
  std::unique_ptr<webrtc::IceCandidateInterface> owned_candidate(
      webrtc::CreateIceCandidate(sdp_mid, sdp_mline_index, candidate, NULL));
  EXPECT_TRUE(peer_connection_->AddIceCandidate(owned_candidate.get()));
}

void PeerConnectionTestWrapper::WaitForCallEstablished() {
  WaitForConnection();
  WaitForAudio();
  WaitForVideo();
}

void PeerConnectionTestWrapper::WaitForConnection() {
  EXPECT_TRUE_WAIT(CheckForConnection(), kMaxWait);
  RTC_LOG(LS_INFO) << "PeerConnectionTestWrapper " << name_ << ": Connected.";
}

bool PeerConnectionTestWrapper::CheckForConnection() {
  return (peer_connection_->ice_connection_state() ==
          PeerConnectionInterface::kIceConnectionConnected) ||
         (peer_connection_->ice_connection_state() ==
          PeerConnectionInterface::kIceConnectionCompleted);
}

void PeerConnectionTestWrapper::WaitForAudio() {
  EXPECT_TRUE_WAIT(CheckForAudio(), kMaxWait);
  RTC_LOG(LS_INFO) << "PeerConnectionTestWrapper " << name_
                   << ": Got enough audio frames.";
}

bool PeerConnectionTestWrapper::CheckForAudio() {
  return (fake_audio_capture_module_->frames_received() >=
          kTestAudioFrameCount);
}

void PeerConnectionTestWrapper::WaitForVideo() {
  EXPECT_TRUE_WAIT(CheckForVideo(), kMaxWait);
  RTC_LOG(LS_INFO) << "PeerConnectionTestWrapper " << name_
                   << ": Got enough video frames.";
}

bool PeerConnectionTestWrapper::CheckForVideo() {
  if (!renderer_) {
    return false;
  }
  return (renderer_->num_rendered_frames() >= kTestVideoFrameCount);
}

void PeerConnectionTestWrapper::GetAndAddUserMedia(
    bool audio,
    const cricket::AudioOptions& audio_options,
    bool video) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
      GetUserMedia(audio, audio_options, video);
  for (const auto& audio_track : stream->GetAudioTracks()) {
    EXPECT_TRUE(peer_connection_->AddTrack(audio_track, {stream->id()}).ok());
  }
  for (const auto& video_track : stream->GetVideoTracks()) {
    EXPECT_TRUE(peer_connection_->AddTrack(video_track, {stream->id()}).ok());
  }
}

rtc::scoped_refptr<webrtc::MediaStreamInterface>
PeerConnectionTestWrapper::GetUserMedia(
    bool audio,
    const cricket::AudioOptions& audio_options,
    bool video) {
  std::string stream_id =
      kStreamIdBase + rtc::ToString(num_get_user_media_calls_++);
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
      peer_connection_factory_->CreateLocalMediaStream(stream_id);

  if (audio) {
    cricket::AudioOptions options = audio_options;
    // Disable highpass filter so that we can get all the test audio frames.
    options.highpass_filter = false;
    rtc::scoped_refptr<webrtc::AudioSourceInterface> source =
        peer_connection_factory_->CreateAudioSource(options);
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
        peer_connection_factory_->CreateAudioTrack(kAudioTrackLabelBase,
                                                   source));
    stream->AddTrack(audio_track);
  }

  if (video) {
    // Set max frame rate to 10fps to reduce the risk of the tests to be flaky.
    webrtc::FakePeriodicVideoSource::Config config;
    config.frame_interval_ms = 100;
    config.timestamp_offset_ms = rtc::TimeMillis();

    auto source = rtc::make_ref_counted<webrtc::FakePeriodicVideoTrackSource>(
        config, /* remote */ false);

    std::string videotrack_label = stream_id + kVideoTrackLabelBase;
    rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
        peer_connection_factory_->CreateVideoTrack(videotrack_label, source));

    stream->AddTrack(video_track);
  }
  return stream;
}
