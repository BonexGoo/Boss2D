/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__peerconnectionwrapper_h //original-code:"pc/peerconnectionwrapper.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_api__jsepsessiondescription_h //original-code:"api/jsepsessiondescription.h"
#include BOSS_WEBRTC_U_media__base__fakevideocapturer_h //original-code:"media/base/fakevideocapturer.h"
#include BOSS_WEBRTC_U_pc__sdputils_h //original-code:"pc/sdputils.h"
#include BOSS_WEBRTC_U_rtc_base__function_view_h //original-code:"rtc_base/function_view.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__ptr_util_h //original-code:"rtc_base/ptr_util.h"

namespace webrtc {

namespace {
const uint32_t kDefaultTimeout = 10000U;
}

PeerConnectionWrapper::PeerConnectionWrapper(
    rtc::scoped_refptr<PeerConnectionFactoryInterface> pc_factory,
    rtc::scoped_refptr<PeerConnectionInterface> pc,
    std::unique_ptr<MockPeerConnectionObserver> observer)
    : pc_factory_(std::move(pc_factory)),
      observer_(std::move(observer)),
      pc_(std::move(pc)) {
  RTC_DCHECK(pc_factory_);
  RTC_DCHECK(pc_);
  RTC_DCHECK(observer_);
  observer_->SetPeerConnectionInterface(pc_.get());
}

PeerConnectionWrapper::~PeerConnectionWrapper() = default;

PeerConnectionFactoryInterface* PeerConnectionWrapper::pc_factory() {
  return pc_factory_.get();
}

PeerConnectionInterface* PeerConnectionWrapper::pc() {
  return pc_.get();
}

MockPeerConnectionObserver* PeerConnectionWrapper::observer() {
  return observer_.get();
}

std::unique_ptr<SessionDescriptionInterface>
PeerConnectionWrapper::CreateOffer() {
  return CreateOffer(PeerConnectionInterface::RTCOfferAnswerOptions());
}

std::unique_ptr<SessionDescriptionInterface> PeerConnectionWrapper::CreateOffer(
    const PeerConnectionInterface::RTCOfferAnswerOptions& options,
    std::string* error_out) {
  return CreateSdp(
      [this, options](CreateSessionDescriptionObserver* observer) {
        pc()->CreateOffer(observer, options);
      },
      error_out);
}

std::unique_ptr<SessionDescriptionInterface>
PeerConnectionWrapper::CreateOfferAndSetAsLocal() {
  return CreateOfferAndSetAsLocal(
      PeerConnectionInterface::RTCOfferAnswerOptions());
}

std::unique_ptr<SessionDescriptionInterface>
PeerConnectionWrapper::CreateOfferAndSetAsLocal(
    const PeerConnectionInterface::RTCOfferAnswerOptions& options) {
  auto offer = CreateOffer(options);
  if (!offer) {
    return nullptr;
  }
  EXPECT_TRUE(SetLocalDescription(CloneSessionDescription(offer.get())));
  return offer;
}

std::unique_ptr<SessionDescriptionInterface>
PeerConnectionWrapper::CreateAnswer() {
  return CreateAnswer(PeerConnectionInterface::RTCOfferAnswerOptions());
}

std::unique_ptr<SessionDescriptionInterface>
PeerConnectionWrapper::CreateAnswer(
    const PeerConnectionInterface::RTCOfferAnswerOptions& options,
    std::string* error_out) {
  return CreateSdp(
      [this, options](CreateSessionDescriptionObserver* observer) {
        pc()->CreateAnswer(observer, options);
      },
      error_out);
}

std::unique_ptr<SessionDescriptionInterface>
PeerConnectionWrapper::CreateAnswerAndSetAsLocal() {
  return CreateAnswerAndSetAsLocal(
      PeerConnectionInterface::RTCOfferAnswerOptions());
}

std::unique_ptr<SessionDescriptionInterface>
PeerConnectionWrapper::CreateAnswerAndSetAsLocal(
    const PeerConnectionInterface::RTCOfferAnswerOptions& options) {
  auto answer = CreateAnswer(options);
  if (!answer) {
    return nullptr;
  }
  EXPECT_TRUE(SetLocalDescription(CloneSessionDescription(answer.get())));
  return answer;
}

std::unique_ptr<SessionDescriptionInterface> PeerConnectionWrapper::CreateSdp(
    rtc::FunctionView<void(CreateSessionDescriptionObserver*)> fn,
    std::string* error_out) {
  rtc::scoped_refptr<MockCreateSessionDescriptionObserver> observer(
      new rtc::RefCountedObject<MockCreateSessionDescriptionObserver>());
  fn(observer);
  EXPECT_EQ_WAIT(true, observer->called(), kDefaultTimeout);
  if (error_out && !observer->result()) {
    *error_out = observer->error();
  }
  return observer->MoveDescription();
}

bool PeerConnectionWrapper::SetLocalDescription(
    std::unique_ptr<SessionDescriptionInterface> desc,
    std::string* error_out) {
  return SetSdp(
      [this, &desc](SetSessionDescriptionObserver* observer) {
        pc()->SetLocalDescription(observer, desc.release());
      },
      error_out);
}

bool PeerConnectionWrapper::SetRemoteDescription(
    std::unique_ptr<SessionDescriptionInterface> desc,
    std::string* error_out) {
  return SetSdp(
      [this, &desc](SetSessionDescriptionObserver* observer) {
        pc()->SetRemoteDescription(observer, desc.release());
      },
      error_out);
}

bool PeerConnectionWrapper::SetRemoteDescription(
    std::unique_ptr<SessionDescriptionInterface> desc,
    RTCError* error_out) {
  rtc::scoped_refptr<MockSetRemoteDescriptionObserver> observer =
      new MockSetRemoteDescriptionObserver();
  pc()->SetRemoteDescription(std::move(desc), observer);
  EXPECT_EQ_WAIT(true, observer->called(), kDefaultTimeout);
  bool ok = observer->error().ok();
  if (error_out)
    *error_out = std::move(observer->error());
  return ok;
}

bool PeerConnectionWrapper::SetSdp(
    rtc::FunctionView<void(SetSessionDescriptionObserver*)> fn,
    std::string* error_out) {
  rtc::scoped_refptr<MockSetSessionDescriptionObserver> observer(
      new rtc::RefCountedObject<MockSetSessionDescriptionObserver>());
  fn(observer);
  EXPECT_EQ_WAIT(true, observer->called(), kDefaultTimeout);
  if (error_out && !observer->result()) {
    *error_out = observer->error();
  }
  return observer->result();
}

bool PeerConnectionWrapper::ExchangeOfferAnswerWith(
    PeerConnectionWrapper* answerer) {
  RTC_DCHECK(answerer);
  if (answerer == this) {
    RTC_LOG(LS_ERROR) << "Cannot exchange offer/answer with ourself!";
    return false;
  }
  auto offer = CreateOffer();
  EXPECT_TRUE(offer);
  if (!offer) {
    return false;
  }
  bool set_local_offer =
      SetLocalDescription(CloneSessionDescription(offer.get()));
  EXPECT_TRUE(set_local_offer);
  if (!set_local_offer) {
    return false;
  }
  bool set_remote_offer = answerer->SetRemoteDescription(std::move(offer));
  EXPECT_TRUE(set_remote_offer);
  if (!set_remote_offer) {
    return false;
  }
  auto answer = answerer->CreateAnswer();
  EXPECT_TRUE(answer);
  if (!answer) {
    return false;
  }
  bool set_local_answer =
      answerer->SetLocalDescription(CloneSessionDescription(answer.get()));
  EXPECT_TRUE(set_local_answer);
  if (!set_local_answer) {
    return false;
  }
  bool set_remote_answer = SetRemoteDescription(std::move(answer));
  EXPECT_TRUE(set_remote_answer);
  return set_remote_answer;
}

rtc::scoped_refptr<RtpTransceiverInterface>
PeerConnectionWrapper::AddTransceiver(cricket::MediaType media_type) {
  RTCErrorOr<rtc::scoped_refptr<RtpTransceiverInterface>> result =
      pc()->AddTransceiver(media_type);
  EXPECT_EQ(RTCErrorType::NONE, result.error().type());
  return result.MoveValue();
}

rtc::scoped_refptr<RtpTransceiverInterface>
PeerConnectionWrapper::AddTransceiver(cricket::MediaType media_type,
                                      const RtpTransceiverInit& init) {
  RTCErrorOr<rtc::scoped_refptr<RtpTransceiverInterface>> result =
      pc()->AddTransceiver(media_type, init);
  EXPECT_EQ(RTCErrorType::NONE, result.error().type());
  return result.MoveValue();
}

rtc::scoped_refptr<RtpTransceiverInterface>
PeerConnectionWrapper::AddTransceiver(
    rtc::scoped_refptr<MediaStreamTrackInterface> track) {
  RTCErrorOr<rtc::scoped_refptr<RtpTransceiverInterface>> result =
      pc()->AddTransceiver(track);
  EXPECT_EQ(RTCErrorType::NONE, result.error().type());
  return result.MoveValue();
}

rtc::scoped_refptr<RtpTransceiverInterface>
PeerConnectionWrapper::AddTransceiver(
    rtc::scoped_refptr<MediaStreamTrackInterface> track,
    const RtpTransceiverInit& init) {
  RTCErrorOr<rtc::scoped_refptr<RtpTransceiverInterface>> result =
      pc()->AddTransceiver(track, init);
  EXPECT_EQ(RTCErrorType::NONE, result.error().type());
  return result.MoveValue();
}

rtc::scoped_refptr<AudioTrackInterface> PeerConnectionWrapper::CreateAudioTrack(
    const std::string& label) {
  return pc_factory()->CreateAudioTrack(label, nullptr);
}

rtc::scoped_refptr<VideoTrackInterface> PeerConnectionWrapper::CreateVideoTrack(
    const std::string& label) {
  auto video_source = pc_factory()->CreateVideoSource(
      rtc::MakeUnique<cricket::FakeVideoCapturer>());
  return pc_factory()->CreateVideoTrack(label, video_source);
}

rtc::scoped_refptr<RtpSenderInterface> PeerConnectionWrapper::AddTrack(
    rtc::scoped_refptr<MediaStreamTrackInterface> track,
    const std::vector<std::string>& stream_labels) {
  RTCErrorOr<rtc::scoped_refptr<RtpSenderInterface>> result =
      pc()->AddTrack(track, stream_labels);
  EXPECT_EQ(RTCErrorType::NONE, result.error().type());
  return result.MoveValue();
}

rtc::scoped_refptr<RtpSenderInterface> PeerConnectionWrapper::AddAudioTrack(
    const std::string& track_label,
    const std::vector<std::string>& stream_labels) {
  return AddTrack(CreateAudioTrack(track_label), stream_labels);
}

rtc::scoped_refptr<RtpSenderInterface> PeerConnectionWrapper::AddVideoTrack(
    const std::string& track_label,
    const std::vector<std::string>& stream_labels) {
  return AddTrack(CreateVideoTrack(track_label), stream_labels);
}

rtc::scoped_refptr<DataChannelInterface>
PeerConnectionWrapper::CreateDataChannel(const std::string& label) {
  return pc()->CreateDataChannel(label, nullptr);
}

PeerConnectionInterface::SignalingState
PeerConnectionWrapper::signaling_state() {
  return pc()->signaling_state();
}

bool PeerConnectionWrapper::IsIceGatheringDone() {
  return observer()->ice_gathering_complete_;
}

bool PeerConnectionWrapper::IsIceConnected() {
  return observer()->ice_connected_;
}

rtc::scoped_refptr<const webrtc::RTCStatsReport>
PeerConnectionWrapper::GetStats() {
  rtc::scoped_refptr<webrtc::MockRTCStatsCollectorCallback> callback(
      new rtc::RefCountedObject<webrtc::MockRTCStatsCollectorCallback>());
  pc()->GetStats(callback);
  EXPECT_TRUE_WAIT(callback->called(), kDefaultTimeout);
  return callback->report();
}

}  // namespace webrtc
