/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contains tests that check the interaction between the
// PeerConnection and the underlying media engine, as well as tests that check
// the media-related aspects of SDP.

#include <tuple>

#include BOSS_WEBRTC_U_call__callfactoryinterface_h //original-code:"call/callfactoryinterface.h"
#include BOSS_WEBRTC_U_logging__rtc_event_log__rtc_event_log_factory_h //original-code:"logging/rtc_event_log/rtc_event_log_factory.h"
#include BOSS_WEBRTC_U_media__base__fakemediaengine_h //original-code:"media/base/fakemediaengine.h"
#include BOSS_WEBRTC_U_p2p__base__fakeportallocator_h //original-code:"p2p/base/fakeportallocator.h"
#include BOSS_WEBRTC_U_pc__mediasession_h //original-code:"pc/mediasession.h"
#include BOSS_WEBRTC_U_pc__peerconnectionwrapper_h //original-code:"pc/peerconnectionwrapper.h"
#include BOSS_WEBRTC_U_pc__rtpmediautils_h //original-code:"pc/rtpmediautils.h"
#include BOSS_WEBRTC_U_pc__sdputils_h //original-code:"pc/sdputils.h"
#ifdef WEBRTC_ANDROID
#include "pc/test/androidtestinitializer.h"
#endif
#include "pc/test/fakertccertificategenerator.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__ptr_util_h //original-code:"rtc_base/ptr_util.h"
#include BOSS_WEBRTC_U_rtc_base__virtualsocketserver_h //original-code:"rtc_base/virtualsocketserver.h"
#include "test/gmock.h"

namespace webrtc {

using cricket::FakeMediaEngine;
using RTCConfiguration = PeerConnectionInterface::RTCConfiguration;
using RTCOfferAnswerOptions = PeerConnectionInterface::RTCOfferAnswerOptions;
using ::testing::Bool;
using ::testing::Combine;
using ::testing::Values;
using ::testing::ElementsAre;

class PeerConnectionWrapperForMediaTest : public PeerConnectionWrapper {
 public:
  using PeerConnectionWrapper::PeerConnectionWrapper;

  FakeMediaEngine* media_engine() { return media_engine_; }
  void set_media_engine(FakeMediaEngine* media_engine) {
    media_engine_ = media_engine;
  }

 private:
  FakeMediaEngine* media_engine_;
};

class PeerConnectionMediaTest : public ::testing::Test {
 protected:
  typedef std::unique_ptr<PeerConnectionWrapperForMediaTest> WrapperPtr;

  PeerConnectionMediaTest()
      : vss_(new rtc::VirtualSocketServer()), main_(vss_.get()) {
#ifdef WEBRTC_ANDROID
    InitializeAndroidObjects();
#endif
  }

  WrapperPtr CreatePeerConnection() {
    return CreatePeerConnection(RTCConfiguration());
  }

  WrapperPtr CreatePeerConnection(const RTCConfiguration& config) {
    auto media_engine = rtc::MakeUnique<FakeMediaEngine>();
    auto* media_engine_ptr = media_engine.get();
    auto pc_factory = CreateModularPeerConnectionFactory(
        rtc::Thread::Current(), rtc::Thread::Current(), rtc::Thread::Current(),
        std::move(media_engine), CreateCallFactory(),
        CreateRtcEventLogFactory());

    auto fake_port_allocator = rtc::MakeUnique<cricket::FakePortAllocator>(
        rtc::Thread::Current(), nullptr);
    auto observer = rtc::MakeUnique<MockPeerConnectionObserver>();
    auto pc = pc_factory->CreatePeerConnection(
        config, std::move(fake_port_allocator), nullptr, observer.get());
    if (!pc) {
      return nullptr;
    }

    auto wrapper = rtc::MakeUnique<PeerConnectionWrapperForMediaTest>(
        pc_factory, pc, std::move(observer));
    wrapper->set_media_engine(media_engine_ptr);
    return wrapper;
  }

  // Accepts the same arguments as CreatePeerConnection and adds default audio
  // and video tracks.
  template <typename... Args>
  WrapperPtr CreatePeerConnectionWithAudioVideo(Args&&... args) {
    auto wrapper = CreatePeerConnection(std::forward<Args>(args)...);
    if (!wrapper) {
      return nullptr;
    }
    wrapper->AddAudioTrack("a");
    wrapper->AddVideoTrack("v");
    return wrapper;
  }

  const cricket::MediaContentDescription* GetMediaContent(
      const SessionDescriptionInterface* sdesc,
      const std::string& mid) {
    const auto* content_desc =
        sdesc->description()->GetContentDescriptionByName(mid);
    return static_cast<const cricket::MediaContentDescription*>(content_desc);
  }

  RtpTransceiverDirection GetMediaContentDirection(
      const SessionDescriptionInterface* sdesc,
      const std::string& mid) {
    auto* media_content = GetMediaContent(sdesc, mid);
    RTC_DCHECK(media_content);
    return media_content->direction();
  }

  std::unique_ptr<rtc::VirtualSocketServer> vss_;
  rtc::AutoSocketServerThread main_;
};

TEST_F(PeerConnectionMediaTest,
       FailToSetRemoteDescriptionIfCreateMediaChannelFails) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();
  callee->media_engine()->set_fail_create_channel(true);

  std::string error;
  ASSERT_FALSE(callee->SetRemoteDescription(caller->CreateOffer(), &error));
  EXPECT_EQ("Failed to set remote offer sdp: Failed to create voice channel.",
            error);
}

TEST_F(PeerConnectionMediaTest,
       FailToSetLocalDescriptionIfCreateMediaChannelFails) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  caller->media_engine()->set_fail_create_channel(true);

  std::string error;
  ASSERT_FALSE(caller->SetLocalDescription(caller->CreateOffer(), &error));
  EXPECT_EQ("Failed to set local offer sdp: Failed to create voice channel.",
            error);
}

std::vector<std::string> GetIds(
    const std::vector<cricket::StreamParams>& streams) {
  std::vector<std::string> ids;
  for (const auto& stream : streams) {
    ids.push_back(stream.id);
  }
  return ids;
}

// Test that exchanging an offer and answer with each side having an audio and
// video stream creates the appropriate send/recv streams in the underlying
// media engine on both sides.
TEST_F(PeerConnectionMediaTest, AudioVideoOfferAnswerCreateSendRecvStreams) {
  const std::string kCallerAudioId = "caller_a";
  const std::string kCallerVideoId = "caller_v";
  const std::string kCalleeAudioId = "callee_a";
  const std::string kCalleeVideoId = "callee_v";

  auto caller = CreatePeerConnection();
  caller->AddAudioTrack(kCallerAudioId);
  caller->AddVideoTrack(kCallerVideoId);

  auto callee = CreatePeerConnection();
  callee->AddAudioTrack(kCalleeAudioId);
  callee->AddVideoTrack(kCalleeVideoId);

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  auto* caller_voice = caller->media_engine()->GetVoiceChannel(0);
  EXPECT_THAT(GetIds(caller_voice->recv_streams()),
              ElementsAre(kCalleeAudioId));
  EXPECT_THAT(GetIds(caller_voice->send_streams()),
              ElementsAre(kCallerAudioId));

  auto* caller_video = caller->media_engine()->GetVideoChannel(0);
  EXPECT_THAT(GetIds(caller_video->recv_streams()),
              ElementsAre(kCalleeVideoId));
  EXPECT_THAT(GetIds(caller_video->send_streams()),
              ElementsAre(kCallerVideoId));

  auto* callee_voice = callee->media_engine()->GetVoiceChannel(0);
  EXPECT_THAT(GetIds(callee_voice->recv_streams()),
              ElementsAre(kCallerAudioId));
  EXPECT_THAT(GetIds(callee_voice->send_streams()),
              ElementsAre(kCalleeAudioId));

  auto* callee_video = callee->media_engine()->GetVideoChannel(0);
  EXPECT_THAT(GetIds(callee_video->recv_streams()),
              ElementsAre(kCallerVideoId));
  EXPECT_THAT(GetIds(callee_video->send_streams()),
              ElementsAre(kCalleeVideoId));
}

// Test that removing streams from a subsequent offer causes the receive streams
// on the callee to be removed.
TEST_F(PeerConnectionMediaTest, EmptyRemoteOfferRemovesRecvStreams) {
  auto caller = CreatePeerConnection();
  auto caller_audio_track = caller->AddAudioTrack("a");
  auto caller_video_track = caller->AddVideoTrack("v");
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  // Remove both tracks from caller.
  caller->pc()->RemoveTrack(caller_audio_track);
  caller->pc()->RemoveTrack(caller_video_track);

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  auto callee_voice = callee->media_engine()->GetVoiceChannel(0);
  EXPECT_EQ(1u, callee_voice->send_streams().size());
  EXPECT_EQ(0u, callee_voice->recv_streams().size());

  auto callee_video = callee->media_engine()->GetVideoChannel(0);
  EXPECT_EQ(1u, callee_video->send_streams().size());
  EXPECT_EQ(0u, callee_video->recv_streams().size());
}

// Test that removing streams from a subsequent answer causes the send streams
// on the callee to be removed when applied locally.
TEST_F(PeerConnectionMediaTest, EmptyLocalAnswerRemovesSendStreams) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnection();
  auto callee_audio_track = callee->AddAudioTrack("a");
  auto callee_video_track = callee->AddVideoTrack("v");

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  // Remove both tracks from callee.
  callee->pc()->RemoveTrack(callee_audio_track);
  callee->pc()->RemoveTrack(callee_video_track);

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  auto callee_voice = callee->media_engine()->GetVoiceChannel(0);
  EXPECT_EQ(0u, callee_voice->send_streams().size());
  EXPECT_EQ(1u, callee_voice->recv_streams().size());

  auto callee_video = callee->media_engine()->GetVideoChannel(0);
  EXPECT_EQ(0u, callee_video->send_streams().size());
  EXPECT_EQ(1u, callee_video->recv_streams().size());
}

// Test that a new stream in a subsequent offer causes a new receive stream to
// be created on the callee.
TEST_F(PeerConnectionMediaTest, NewStreamInRemoteOfferAddsRecvStreams) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnection();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  // Add second set of tracks to the caller.
  caller->AddAudioTrack("a2");
  caller->AddVideoTrack("v2");

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  auto callee_voice = callee->media_engine()->GetVoiceChannel(0);
  EXPECT_EQ(2u, callee_voice->recv_streams().size());
  auto callee_video = callee->media_engine()->GetVideoChannel(0);
  EXPECT_EQ(2u, callee_video->recv_streams().size());
}

// Test that a new stream in a subsequent answer causes a new send stream to be
// created on the callee when added locally.
TEST_F(PeerConnectionMediaTest, NewStreamInLocalAnswerAddsSendStreams) {
  auto caller = CreatePeerConnection();
  auto callee = CreatePeerConnectionWithAudioVideo();

  RTCOfferAnswerOptions options;
  options.offer_to_receive_audio =
      RTCOfferAnswerOptions::kOfferToReceiveMediaTrue;
  options.offer_to_receive_video =
      RTCOfferAnswerOptions::kOfferToReceiveMediaTrue;

  ASSERT_TRUE(
      callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal(options)));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  // Add second set of tracks to the callee.
  callee->AddAudioTrack("a2");
  callee->AddVideoTrack("v2");

  ASSERT_TRUE(
      callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal(options)));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  auto callee_voice = callee->media_engine()->GetVoiceChannel(0);
  EXPECT_EQ(2u, callee_voice->send_streams().size());
  auto callee_video = callee->media_engine()->GetVideoChannel(0);
  EXPECT_EQ(2u, callee_video->send_streams().size());
}

// A PeerConnection with no local streams and no explicit answer constraints
// should not reject any offered media sections.
TEST_F(PeerConnectionMediaTest,
       CreateAnswerWithNoStreamsAndDefaultOptionsDoesNotReject) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnection();
  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  auto answer = callee->CreateAnswer();

  const auto* audio_content =
      cricket::GetFirstAudioContent(answer->description());
  ASSERT_TRUE(audio_content);
  EXPECT_FALSE(audio_content->rejected);

  const auto* video_content =
      cricket::GetFirstVideoContent(answer->description());
  ASSERT_TRUE(video_content);
  EXPECT_FALSE(video_content->rejected);
}

class PeerConnectionMediaOfferDirectionTest
    : public PeerConnectionMediaTest,
      public ::testing::WithParamInterface<
          std::tuple<bool, int, RtpTransceiverDirection>> {
 protected:
  PeerConnectionMediaOfferDirectionTest() {
    send_media_ = std::get<0>(GetParam());
    offer_to_receive_ = std::get<1>(GetParam());
    expected_direction_ = std::get<2>(GetParam());
  }

  bool send_media_;
  int offer_to_receive_;
  RtpTransceiverDirection expected_direction_;
};

// Tests that the correct direction is set on the media description according
// to the presence of a local media track and the offer_to_receive setting.
TEST_P(PeerConnectionMediaOfferDirectionTest, VerifyDirection) {
  auto caller = CreatePeerConnection();
  if (send_media_) {
    caller->AddAudioTrack("a");
  }

  RTCOfferAnswerOptions options;
  options.offer_to_receive_audio = offer_to_receive_;
  auto offer = caller->CreateOffer(options);

  auto* media_content = GetMediaContent(offer.get(), cricket::CN_AUDIO);
  if (expected_direction_ == RtpTransceiverDirection::kInactive) {
    EXPECT_FALSE(media_content);
  } else {
    EXPECT_EQ(expected_direction_, media_content->direction());
  }
}

// Note that in these tests, MD_INACTIVE indicates that no media section is
// included in the offer, not that the media direction is inactive.
INSTANTIATE_TEST_CASE_P(
    PeerConnectionMediaTest,
    PeerConnectionMediaOfferDirectionTest,
    Values(std::make_tuple(false, -1, RtpTransceiverDirection::kInactive),
           std::make_tuple(false, 0, RtpTransceiverDirection::kInactive),
           std::make_tuple(false, 1, RtpTransceiverDirection::kRecvOnly),
           std::make_tuple(true, -1, RtpTransceiverDirection::kSendRecv),
           std::make_tuple(true, 0, RtpTransceiverDirection::kSendOnly),
           std::make_tuple(true, 1, RtpTransceiverDirection::kSendRecv)));

class PeerConnectionMediaAnswerDirectionTest
    : public PeerConnectionMediaTest,
      public ::testing::WithParamInterface<
          std::tuple<RtpTransceiverDirection, bool, int>> {
 protected:
  PeerConnectionMediaAnswerDirectionTest() {
    offer_direction_ = std::get<0>(GetParam());
    send_media_ = std::get<1>(GetParam());
    offer_to_receive_ = std::get<2>(GetParam());
  }

  RtpTransceiverDirection offer_direction_;
  bool send_media_;
  int offer_to_receive_;
};

// Tests that the direction in an answer is correct according to direction sent
// in the offer, the presence of a local media track on the receive side and the
// offer_to_receive setting.
TEST_P(PeerConnectionMediaAnswerDirectionTest, VerifyDirection) {
  auto caller = CreatePeerConnection();
  caller->AddAudioTrack("a");

  // Create the offer with an audio section and set its direction.
  auto offer = caller->CreateOffer();
  cricket::GetFirstAudioContentDescription(offer->description())
      ->set_direction(offer_direction_);

  auto callee = CreatePeerConnection();
  if (send_media_) {
    callee->AddAudioTrack("a");
  }
  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  // Create the answer according to the test parameters.
  RTCOfferAnswerOptions options;
  options.offer_to_receive_audio = offer_to_receive_;
  auto answer = callee->CreateAnswer(options);

  // The expected direction in the answer is the intersection of each side's
  // capability to send/recv media.
  // For the offerer, the direction is given in the offer (offer_direction_).
  // For the answerer, the direction has two components:
  // 1. Send if the answerer has a local track to send.
  // 2. Receive if the answerer has explicitly set the offer_to_receive to 1 or
  //    if it has been left as default.
  bool offer_send = RtpTransceiverDirectionHasSend(offer_direction_);
  bool offer_recv = RtpTransceiverDirectionHasRecv(offer_direction_);

  // The negotiated components determine the direction set in the answer.
  bool negotiate_send = (send_media_ && offer_recv);
  bool negotiate_recv = ((offer_to_receive_ != 0) && offer_send);

  auto expected_direction =
      RtpTransceiverDirectionFromSendRecv(negotiate_send, negotiate_recv);
  EXPECT_EQ(expected_direction,
            GetMediaContentDirection(answer.get(), cricket::CN_AUDIO));
}

// Tests that the media section is rejected if and only if the callee has no
// local media track and has set offer_to_receive to 0, no matter which
// direction the caller indicated in the offer.
TEST_P(PeerConnectionMediaAnswerDirectionTest, VerifyRejected) {
  auto caller = CreatePeerConnection();
  caller->AddAudioTrack("a");

  // Create the offer with an audio section and set its direction.
  auto offer = caller->CreateOffer();
  cricket::GetFirstAudioContentDescription(offer->description())
      ->set_direction(offer_direction_);

  auto callee = CreatePeerConnection();
  if (send_media_) {
    callee->AddAudioTrack("a");
  }
  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  // Create the answer according to the test parameters.
  RTCOfferAnswerOptions options;
  options.offer_to_receive_audio = offer_to_receive_;
  auto answer = callee->CreateAnswer(options);

  // The media section is rejected if and only if offer_to_receive is explicitly
  // set to 0 and there is no media to send.
  auto* audio_content = cricket::GetFirstAudioContent(answer->description());
  ASSERT_TRUE(audio_content);
  EXPECT_EQ((offer_to_receive_ == 0 && !send_media_), audio_content->rejected);
}

INSTANTIATE_TEST_CASE_P(PeerConnectionMediaTest,
                        PeerConnectionMediaAnswerDirectionTest,
                        Combine(Values(RtpTransceiverDirection::kInactive,
                                       RtpTransceiverDirection::kSendOnly,
                                       RtpTransceiverDirection::kRecvOnly,
                                       RtpTransceiverDirection::kSendRecv),
                                Bool(),
                                Values(-1, 0, 1)));

TEST_F(PeerConnectionMediaTest, OfferHasDifferentDirectionForAudioVideo) {
  auto caller = CreatePeerConnection();
  caller->AddVideoTrack("v");

  RTCOfferAnswerOptions options;
  options.offer_to_receive_audio = 1;
  options.offer_to_receive_video = 0;
  auto offer = caller->CreateOffer(options);

  EXPECT_EQ(RtpTransceiverDirection::kRecvOnly,
            GetMediaContentDirection(offer.get(), cricket::CN_AUDIO));
  EXPECT_EQ(RtpTransceiverDirection::kSendOnly,
            GetMediaContentDirection(offer.get(), cricket::CN_VIDEO));
}

TEST_F(PeerConnectionMediaTest, AnswerHasDifferentDirectionsForAudioVideo) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnection();
  callee->AddVideoTrack("v");

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));

  RTCOfferAnswerOptions options;
  options.offer_to_receive_audio = 1;
  options.offer_to_receive_video = 0;
  auto answer = callee->CreateAnswer(options);

  EXPECT_EQ(RtpTransceiverDirection::kRecvOnly,
            GetMediaContentDirection(answer.get(), cricket::CN_AUDIO));
  EXPECT_EQ(RtpTransceiverDirection::kSendOnly,
            GetMediaContentDirection(answer.get(), cricket::CN_VIDEO));
}

void AddComfortNoiseCodecsToSend(cricket::FakeMediaEngine* media_engine) {
  const cricket::AudioCodec kComfortNoiseCodec8k(102, "CN", 8000, 0, 1);
  const cricket::AudioCodec kComfortNoiseCodec16k(103, "CN", 16000, 0, 1);

  auto codecs = media_engine->audio_send_codecs();
  codecs.push_back(kComfortNoiseCodec8k);
  codecs.push_back(kComfortNoiseCodec16k);
  media_engine->SetAudioCodecs(codecs);
}

bool HasAnyComfortNoiseCodecs(const cricket::SessionDescription* desc) {
  const auto* audio_desc = cricket::GetFirstAudioContentDescription(desc);
  for (const auto& codec : audio_desc->codecs()) {
    if (codec.name == "CN") {
      return true;
    }
  }
  return false;
}

TEST_F(PeerConnectionMediaTest,
       CreateOfferWithNoVoiceActivityDetectionIncludesNoComfortNoiseCodecs) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  AddComfortNoiseCodecsToSend(caller->media_engine());

  RTCOfferAnswerOptions options;
  options.voice_activity_detection = false;
  auto offer = caller->CreateOffer(options);

  EXPECT_FALSE(HasAnyComfortNoiseCodecs(offer->description()));
}

TEST_F(PeerConnectionMediaTest,
       CreateAnswerWithNoVoiceActivityDetectionIncludesNoComfortNoiseCodecs) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  AddComfortNoiseCodecsToSend(caller->media_engine());
  auto callee = CreatePeerConnectionWithAudioVideo();
  AddComfortNoiseCodecsToSend(callee->media_engine());

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));

  RTCOfferAnswerOptions options;
  options.voice_activity_detection = false;
  auto answer = callee->CreateAnswer(options);

  EXPECT_FALSE(HasAnyComfortNoiseCodecs(answer->description()));
}

// The following test group verifies that we reject answers with invalid media
// sections as per RFC 3264.

class PeerConnectionMediaInvalidMediaTest
    : public PeerConnectionMediaTest,
      public ::testing::WithParamInterface<
          std::tuple<std::string,
                     std::function<void(cricket::SessionDescription*)>,
                     std::string>> {
 protected:
  PeerConnectionMediaInvalidMediaTest() {
    mutator_ = std::get<1>(GetParam());
    expected_error_ = std::get<2>(GetParam());
  }

  std::function<void(cricket::SessionDescription*)> mutator_;
  std::string expected_error_;
};

TEST_P(PeerConnectionMediaInvalidMediaTest, FailToSetRemoteAnswer) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));

  auto answer = callee->CreateAnswer();
  mutator_(answer->description());

  std::string error;
  ASSERT_FALSE(caller->SetRemoteDescription(std::move(answer), &error));
  EXPECT_EQ("Failed to set remote answer sdp: " + expected_error_, error);
}

TEST_P(PeerConnectionMediaInvalidMediaTest, FailToSetLocalAnswer) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));

  auto answer = callee->CreateAnswer();
  mutator_(answer->description());

  std::string error;
  ASSERT_FALSE(callee->SetLocalDescription(std::move(answer), &error));
  EXPECT_EQ("Failed to set local answer sdp: " + expected_error_, error);
}

void RemoveVideoContent(cricket::SessionDescription* desc) {
  auto content_name = cricket::GetFirstVideoContent(desc)->name;
  desc->RemoveContentByName(content_name);
  desc->RemoveTransportInfoByName(content_name);
}

void RenameVideoContent(cricket::SessionDescription* desc) {
  auto* video_content = cricket::GetFirstVideoContent(desc);
  auto* transport_info = desc->GetTransportInfoByName(video_content->name);
  video_content->name = "video_renamed";
  transport_info->content_name = video_content->name;
}

void ReverseMediaContent(cricket::SessionDescription* desc) {
  std::reverse(desc->contents().begin(), desc->contents().end());
  std::reverse(desc->transport_infos().begin(), desc->transport_infos().end());
}

void ChangeMediaTypeAudioToVideo(cricket::SessionDescription* desc) {
  desc->RemoveContentByName(cricket::CN_AUDIO);
  auto* video_content = desc->GetContentByName(cricket::CN_VIDEO);
  desc->AddContent(cricket::CN_AUDIO, video_content->type,
                   video_content->media_description()->Copy());
}

constexpr char kMLinesOutOfOrder[] =
    "The order of m-lines in answer doesn't match order in offer. Rejecting "
    "answer.";

INSTANTIATE_TEST_CASE_P(
    PeerConnectionMediaTest,
    PeerConnectionMediaInvalidMediaTest,
    Values(
        std::make_tuple("remove video", RemoveVideoContent, kMLinesOutOfOrder),
        std::make_tuple("rename video", RenameVideoContent, kMLinesOutOfOrder),
        std::make_tuple("reverse media sections",
                        ReverseMediaContent,
                        kMLinesOutOfOrder),
        std::make_tuple("change audio type to video type",
                        ChangeMediaTypeAudioToVideo,
                        kMLinesOutOfOrder)));

// Test that the correct media engine send/recv streams are created when doing
// a series of offer/answers where audio/video are both sent, then audio is
// rejected, then both audio/video sent again.
TEST_F(PeerConnectionMediaTest, TestAVOfferWithAudioOnlyAnswer) {
  RTCOfferAnswerOptions options_reject_video;
  options_reject_video.offer_to_receive_audio =
      RTCOfferAnswerOptions::kOfferToReceiveMediaTrue;
  options_reject_video.offer_to_receive_video = 0;

  auto caller = CreatePeerConnection();
  caller->AddAudioTrack("a");
  caller->AddVideoTrack("v");
  auto callee = CreatePeerConnection();

  // Caller initially offers to send/recv audio and video.
  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  // Callee accepts the audio as recv only but rejects the video.
  ASSERT_TRUE(caller->SetRemoteDescription(
      callee->CreateAnswerAndSetAsLocal(options_reject_video)));

  auto caller_voice = caller->media_engine()->GetVoiceChannel(0);
  ASSERT_TRUE(caller_voice);
  EXPECT_EQ(0u, caller_voice->recv_streams().size());
  EXPECT_EQ(1u, caller_voice->send_streams().size());
  auto caller_video = caller->media_engine()->GetVideoChannel(0);
  EXPECT_FALSE(caller_video);

  // Callee adds its own audio/video stream and offers to receive audio/video
  // too.
  callee->AddAudioTrack("a");
  auto callee_video_track = callee->AddVideoTrack("v");
  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  auto callee_voice = callee->media_engine()->GetVoiceChannel(0);
  ASSERT_TRUE(callee_voice);
  EXPECT_EQ(1u, callee_voice->recv_streams().size());
  EXPECT_EQ(1u, callee_voice->send_streams().size());
  auto callee_video = callee->media_engine()->GetVideoChannel(0);
  ASSERT_TRUE(callee_video);
  EXPECT_EQ(1u, callee_video->recv_streams().size());
  EXPECT_EQ(1u, callee_video->send_streams().size());

  // Callee removes video but keeps audio and rejects the video once again.
  callee->pc()->RemoveTrack(callee_video_track);
  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      callee->SetLocalDescription(callee->CreateAnswer(options_reject_video)));

  callee_voice = callee->media_engine()->GetVoiceChannel(0);
  ASSERT_TRUE(callee_voice);
  EXPECT_EQ(1u, callee_voice->recv_streams().size());
  EXPECT_EQ(1u, callee_voice->send_streams().size());
  callee_video = callee->media_engine()->GetVideoChannel(0);
  EXPECT_FALSE(callee_video);
}

// Test that the correct media engine send/recv streams are created when doing
// a series of offer/answers where audio/video are both sent, then video is
// rejected, then both audio/video sent again.
TEST_F(PeerConnectionMediaTest, TestAVOfferWithVideoOnlyAnswer) {
  // Disable the bundling here. If the media is bundled on audio
  // transport, then we can't reject the audio because switching the bundled
  // transport is not currently supported.
  // (https://bugs.chromium.org/p/webrtc/issues/detail?id=6704)
  RTCOfferAnswerOptions options_no_bundle;
  options_no_bundle.use_rtp_mux = false;
  RTCOfferAnswerOptions options_reject_audio = options_no_bundle;
  options_reject_audio.offer_to_receive_audio = 0;
  options_reject_audio.offer_to_receive_video =
      RTCOfferAnswerOptions::kMaxOfferToReceiveMedia;

  auto caller = CreatePeerConnection();
  caller->AddAudioTrack("a");
  caller->AddVideoTrack("v");
  auto callee = CreatePeerConnection();

  // Caller initially offers to send/recv audio and video.
  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  // Callee accepts the video as recv only but rejects the audio.
  ASSERT_TRUE(caller->SetRemoteDescription(
      callee->CreateAnswerAndSetAsLocal(options_reject_audio)));

  auto caller_voice = caller->media_engine()->GetVoiceChannel(0);
  EXPECT_FALSE(caller_voice);
  auto caller_video = caller->media_engine()->GetVideoChannel(0);
  ASSERT_TRUE(caller_video);
  EXPECT_EQ(0u, caller_video->recv_streams().size());
  EXPECT_EQ(1u, caller_video->send_streams().size());

  // Callee adds its own audio/video stream and offers to receive audio/video
  // too.
  auto callee_audio_track = callee->AddAudioTrack("a");
  callee->AddVideoTrack("v");
  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(caller->SetRemoteDescription(
      callee->CreateAnswerAndSetAsLocal(options_no_bundle)));

  auto callee_voice = callee->media_engine()->GetVoiceChannel(0);
  ASSERT_TRUE(callee_voice);
  EXPECT_EQ(1u, callee_voice->recv_streams().size());
  EXPECT_EQ(1u, callee_voice->send_streams().size());
  auto callee_video = callee->media_engine()->GetVideoChannel(0);
  ASSERT_TRUE(callee_video);
  EXPECT_EQ(1u, callee_video->recv_streams().size());
  EXPECT_EQ(1u, callee_video->send_streams().size());

  // Callee removes audio but keeps video and rejects the audio once again.
  callee->pc()->RemoveTrack(callee_audio_track);
  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      callee->SetLocalDescription(callee->CreateAnswer(options_reject_audio)));

  callee_voice = callee->media_engine()->GetVoiceChannel(0);
  EXPECT_FALSE(callee_voice);
  callee_video = callee->media_engine()->GetVideoChannel(0);
  ASSERT_TRUE(callee_video);
  EXPECT_EQ(1u, callee_video->recv_streams().size());
  EXPECT_EQ(1u, callee_video->send_streams().size());
}

// Tests that if the underlying video encoder fails to be initialized (signaled
// by failing to set send codecs), the PeerConnection signals the error to the
// client.
TEST_F(PeerConnectionMediaTest, MediaEngineErrorPropagatedToClients) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));

  auto video_channel = caller->media_engine()->GetVideoChannel(0);
  video_channel->set_fail_set_send_codecs(true);

  std::string error;
  ASSERT_FALSE(caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal(),
                                            &error));
  EXPECT_EQ(
      "Failed to set remote answer sdp: Session error code: ERROR_CONTENT. "
      "Session error description: Failed to set remote video description send "
      "parameters..",
      error);
}

// Tests that if the underlying video encoder fails once then subsequent
// attempts at setting the local/remote description will also fail, even if
// SetSendCodecs no longer fails.
TEST_F(PeerConnectionMediaTest,
       FailToApplyDescriptionIfVideoEncoderHasEverFailed) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));

  auto video_channel = caller->media_engine()->GetVideoChannel(0);
  video_channel->set_fail_set_send_codecs(true);

  EXPECT_FALSE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  video_channel->set_fail_set_send_codecs(false);

  EXPECT_FALSE(caller->SetRemoteDescription(callee->CreateAnswer()));
  EXPECT_FALSE(caller->SetLocalDescription(caller->CreateOffer()));
}

void RenameContent(cricket::SessionDescription* desc,
                   const std::string& old_name,
                   const std::string& new_name) {
  auto* content = desc->GetContentByName(old_name);
  RTC_DCHECK(content);
  content->name = new_name;
  auto* transport = desc->GetTransportInfoByName(old_name);
  RTC_DCHECK(transport);
  transport->content_name = new_name;
}

// Tests that an answer responds with the same MIDs as the offer.
TEST_F(PeerConnectionMediaTest, AnswerHasSameMidsAsOffer) {
  const std::string kAudioMid = "not default1";
  const std::string kVideoMid = "not default2";

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  auto offer = caller->CreateOffer();
  RenameContent(offer->description(), cricket::CN_AUDIO, kAudioMid);
  RenameContent(offer->description(), cricket::CN_VIDEO, kVideoMid);
  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  auto answer = callee->CreateAnswer();
  EXPECT_EQ(kAudioMid,
            cricket::GetFirstAudioContent(answer->description())->name);
  EXPECT_EQ(kVideoMid,
            cricket::GetFirstVideoContent(answer->description())->name);
}

// Test that if the callee creates a re-offer, the MIDs are the same as the
// original offer.
TEST_F(PeerConnectionMediaTest, ReOfferHasSameMidsAsFirstOffer) {
  const std::string kAudioMid = "not default1";
  const std::string kVideoMid = "not default2";

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  auto offer = caller->CreateOffer();
  RenameContent(offer->description(), cricket::CN_AUDIO, kAudioMid);
  RenameContent(offer->description(), cricket::CN_VIDEO, kVideoMid);
  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));
  ASSERT_TRUE(callee->SetLocalDescription(callee->CreateAnswer()));

  auto reoffer = callee->CreateOffer();
  EXPECT_EQ(kAudioMid,
            cricket::GetFirstAudioContent(reoffer->description())->name);
  EXPECT_EQ(kVideoMid,
            cricket::GetFirstVideoContent(reoffer->description())->name);
}

TEST_F(PeerConnectionMediaTest,
       CombinedAudioVideoBweConfigPropagatedToMediaEngine) {
  RTCConfiguration config;
  config.combined_audio_video_bwe.emplace(true);
  auto caller = CreatePeerConnectionWithAudioVideo(config);

  ASSERT_TRUE(caller->SetLocalDescription(caller->CreateOffer()));

  auto caller_voice = caller->media_engine()->GetVoiceChannel(0);
  ASSERT_TRUE(caller_voice);
  const cricket::AudioOptions& audio_options = caller_voice->options();
  EXPECT_EQ(config.combined_audio_video_bwe,
            audio_options.combined_audio_video_bwe);
}

}  // namespace webrtc
