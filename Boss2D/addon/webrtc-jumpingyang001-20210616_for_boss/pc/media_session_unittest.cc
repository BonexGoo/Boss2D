/*
 *  Copyright 2004 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__media_session_h //original-code:"pc/media_session.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__algorithm__container_h //original-code:"absl/algorithm/container.h"
#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_ABSEILCPP_U_absl__strings__match_h //original-code:"absl/strings/match.h"
#include BOSS_WEBRTC_U_media__base__codec_h //original-code:"media/base/codec.h"
#include BOSS_WEBRTC_U_media__base__test_utils_h //original-code:"media/base/test_utils.h"
#include BOSS_WEBRTC_U_media__sctp__sctp_transport_internal_h //original-code:"media/sctp/sctp_transport_internal.h"
#include BOSS_WEBRTC_U_p2p__base__p2p_constants_h //original-code:"p2p/base/p2p_constants.h"
#include BOSS_WEBRTC_U_p2p__base__transport_description_h //original-code:"p2p/base/transport_description.h"
#include BOSS_WEBRTC_U_p2p__base__transport_info_h //original-code:"p2p/base/transport_info.h"
#include BOSS_WEBRTC_U_pc__rtp_media_utils_h //original-code:"pc/rtp_media_utils.h"
#include BOSS_WEBRTC_U_pc__srtp_filter_h //original-code:"pc/srtp_filter.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__fake_ssl_identity_h //original-code:"rtc_base/fake_ssl_identity.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__message_digest_h //original-code:"rtc_base/message_digest.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_adapter_h //original-code:"rtc_base/ssl_adapter.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"
#include BOSS_WEBRTC_U_rtc_base__unique_id_generator_h //original-code:"rtc_base/unique_id_generator.h"
#include BOSS_WEBRTC_U_test__field_trial_h //original-code:"test/field_trial.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

#define ASSERT_CRYPTO(cd, s, cs)      \
  ASSERT_EQ(s, cd->cryptos().size()); \
  ASSERT_EQ(cs, cd->cryptos()[0].cipher_suite)

typedef std::vector<cricket::Candidate> Candidates;

using cricket::AudioCodec;
using cricket::AudioContentDescription;
using cricket::ContentInfo;
using cricket::CryptoParamsVec;
using cricket::GetFirstAudioContent;
using cricket::GetFirstAudioContentDescription;
using cricket::GetFirstDataContent;
using cricket::GetFirstVideoContent;
using cricket::GetFirstVideoContentDescription;
using cricket::kAutoBandwidth;
using cricket::MEDIA_TYPE_AUDIO;
using cricket::MEDIA_TYPE_DATA;
using cricket::MEDIA_TYPE_VIDEO;
using cricket::MediaContentDescription;
using cricket::MediaDescriptionOptions;
using cricket::MediaProtocolType;
using cricket::MediaSessionDescriptionFactory;
using cricket::MediaSessionOptions;
using cricket::MediaType;
using cricket::RidDescription;
using cricket::RidDirection;
using cricket::SctpDataContentDescription;
using cricket::SEC_DISABLED;
using cricket::SEC_ENABLED;
using cricket::SEC_REQUIRED;
using cricket::SessionDescription;
using cricket::SimulcastDescription;
using cricket::SimulcastLayer;
using cricket::SimulcastLayerList;
using cricket::SsrcGroup;
using cricket::StreamParams;
using cricket::StreamParamsVec;
using cricket::TransportDescription;
using cricket::TransportDescriptionFactory;
using cricket::TransportInfo;
using cricket::VideoCodec;
using cricket::VideoContentDescription;
using rtc::CS_AEAD_AES_128_GCM;
using rtc::CS_AEAD_AES_256_GCM;
using rtc::CS_AES_CM_128_HMAC_SHA1_32;
using rtc::CS_AES_CM_128_HMAC_SHA1_80;
using rtc::UniqueRandomIdGenerator;
using ::testing::Contains;
using ::testing::Each;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::Eq;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::IsFalse;
using ::testing::Ne;
using ::testing::Not;
using ::testing::Pointwise;
using ::testing::SizeIs;
using webrtc::RtpExtension;
using webrtc::RtpTransceiverDirection;

static const AudioCodec kAudioCodecs1[] = {
    AudioCodec(103, "ISAC", 16000, -1, 1),
    AudioCodec(102, "iLBC", 8000, 13300, 1),
    AudioCodec(0, "PCMU", 8000, 64000, 1),
    AudioCodec(8, "PCMA", 8000, 64000, 1),
    AudioCodec(117, "red", 8000, 0, 1),
    AudioCodec(107, "CN", 48000, 0, 1)};

static const AudioCodec kAudioCodecs2[] = {
    AudioCodec(126, "foo", 16000, 22000, 1),
    AudioCodec(0, "PCMU", 8000, 64000, 1),
    AudioCodec(127, "iLBC", 8000, 13300, 1),
};

static const AudioCodec kAudioCodecsAnswer[] = {
    AudioCodec(102, "iLBC", 8000, 13300, 1),
    AudioCodec(0, "PCMU", 8000, 64000, 1),
};

static const VideoCodec kVideoCodecs1[] = {VideoCodec(96, "H264-SVC"),
                                           VideoCodec(97, "H264")};

static const VideoCodec kVideoCodecs1Reverse[] = {VideoCodec(97, "H264"),
                                                  VideoCodec(96, "H264-SVC")};

static const VideoCodec kVideoCodecs2[] = {VideoCodec(126, "H264"),
                                           VideoCodec(127, "H263")};

static const VideoCodec kVideoCodecsAnswer[] = {VideoCodec(97, "H264")};

static const RtpExtension kAudioRtpExtension1[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:ssrc-audio-level", 8),
    RtpExtension("http://google.com/testing/audio_something", 10),
};

static const RtpExtension kAudioRtpExtensionEncrypted1[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:ssrc-audio-level", 8),
    RtpExtension("http://google.com/testing/audio_something", 10),
    RtpExtension("urn:ietf:params:rtp-hdrext:ssrc-audio-level", 12, true),
    RtpExtension("http://google.com/testing/audio_something", 11, true),
};

static const RtpExtension kAudioRtpExtension2[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:ssrc-audio-level", 2),
    RtpExtension("http://google.com/testing/audio_something_else", 8),
    RtpExtension("http://google.com/testing/both_audio_and_video", 7),
};

static const RtpExtension kAudioRtpExtension3[] = {
    RtpExtension("http://google.com/testing/audio_something", 2),
    RtpExtension("http://google.com/testing/both_audio_and_video", 3),
};

static const RtpExtension kAudioRtpExtension3ForEncryption[] = {
    RtpExtension("http://google.com/testing/audio_something", 2),
    // Use RTP extension that supports encryption.
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 3),
};

static const RtpExtension kAudioRtpExtension3ForEncryptionOffer[] = {
    RtpExtension("http://google.com/testing/audio_something", 2),
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 3),
    RtpExtension("http://google.com/testing/audio_something", 14, true),
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 13, true),
};

static const RtpExtension kVideoRtpExtension3ForEncryptionOffer[] = {
    RtpExtension("http://google.com/testing/video_something", 4),
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 3),
    RtpExtension("http://google.com/testing/video_something", 12, true),
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 13, true),
};

static const RtpExtension kAudioRtpExtensionAnswer[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:ssrc-audio-level", 8),
};

static const RtpExtension kAudioRtpExtensionEncryptedAnswer[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:ssrc-audio-level", 12, true),
};

static const RtpExtension kVideoRtpExtension1[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 14),
    RtpExtension("http://google.com/testing/video_something", 13),
};

static const RtpExtension kVideoRtpExtensionEncrypted1[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 14),
    RtpExtension("http://google.com/testing/video_something", 13),
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 9, true),
    RtpExtension("http://google.com/testing/video_something", 7, true),
};

static const RtpExtension kVideoRtpExtension2[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 2),
    RtpExtension("http://google.com/testing/video_something_else", 14),
    RtpExtension("http://google.com/testing/both_audio_and_video", 7),
};

static const RtpExtension kVideoRtpExtension3[] = {
    RtpExtension("http://google.com/testing/video_something", 4),
    RtpExtension("http://google.com/testing/both_audio_and_video", 5),
};

static const RtpExtension kVideoRtpExtension3ForEncryption[] = {
    RtpExtension("http://google.com/testing/video_something", 4),
    // Use RTP extension that supports encryption.
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 5),
};

static const RtpExtension kVideoRtpExtensionAnswer[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 14),
};

static const RtpExtension kVideoRtpExtensionEncryptedAnswer[] = {
    RtpExtension("urn:ietf:params:rtp-hdrext:toffset", 9, true),
};

static const RtpExtension kRtpExtensionTransportSequenceNumber01[] = {
    RtpExtension("http://www.ietf.org/id/"
                 "draft-holmer-rmcat-transport-wide-cc-extensions-01",
                 1),
};

static const RtpExtension kRtpExtensionTransportSequenceNumber01And02[] = {
    RtpExtension("http://www.ietf.org/id/"
                 "draft-holmer-rmcat-transport-wide-cc-extensions-01",
                 1),
    RtpExtension(
        "http://www.webrtc.org/experiments/rtp-hdrext/transport-wide-cc-02",
        2),
};

static const RtpExtension kRtpExtensionTransportSequenceNumber02[] = {
    RtpExtension(
        "http://www.webrtc.org/experiments/rtp-hdrext/transport-wide-cc-02",
        2),
};

static const RtpExtension kRtpExtensionGenericFrameDescriptorUri00[] = {
    RtpExtension("http://www.webrtc.org/experiments/rtp-hdrext/"
                 "generic-frame-descriptor-00",
                 3),
};

static const uint32_t kSimulcastParamsSsrc[] = {10, 11, 20, 21, 30, 31};
static const uint32_t kSimSsrc[] = {10, 20, 30};
static const uint32_t kFec1Ssrc[] = {10, 11};
static const uint32_t kFec2Ssrc[] = {20, 21};
static const uint32_t kFec3Ssrc[] = {30, 31};

static const char kMediaStream1[] = "stream_1";
static const char kMediaStream2[] = "stream_2";
static const char kVideoTrack1[] = "video_1";
static const char kVideoTrack2[] = "video_2";
static const char kAudioTrack1[] = "audio_1";
static const char kAudioTrack2[] = "audio_2";
static const char kAudioTrack3[] = "audio_3";

static const char* kMediaProtocols[] = {"RTP/AVP", "RTP/SAVP", "RTP/AVPF",
                                        "RTP/SAVPF"};
static const char* kMediaProtocolsDtls[] = {
    "TCP/TLS/RTP/SAVPF", "TCP/TLS/RTP/SAVP", "UDP/TLS/RTP/SAVPF",
    "UDP/TLS/RTP/SAVP"};

// SRTP cipher name negotiated by the tests. This must be updated if the
// default changes.
static const char* kDefaultSrtpCryptoSuite = CS_AES_CM_128_HMAC_SHA1_80;
static const char* kDefaultSrtpCryptoSuiteGcm = CS_AEAD_AES_256_GCM;

// These constants are used to make the code using "AddMediaDescriptionOptions"
// more readable.
static constexpr bool kStopped = true;
static constexpr bool kActive = false;

static bool IsMediaContentOfType(const ContentInfo* content,
                                 MediaType media_type) {
  RTC_DCHECK(content);
  return content->media_description()->type() == media_type;
}

static RtpTransceiverDirection GetMediaDirection(const ContentInfo* content) {
  RTC_DCHECK(content);
  return content->media_description()->direction();
}

static void AddRtxCodec(const VideoCodec& rtx_codec,
                        std::vector<VideoCodec>* codecs) {
  ASSERT_FALSE(cricket::FindCodecById(*codecs, rtx_codec.id));
  codecs->push_back(rtx_codec);
}

template <class T>
static std::vector<std::string> GetCodecNames(const std::vector<T>& codecs) {
  std::vector<std::string> codec_names;
  codec_names.reserve(codecs.size());
  for (const auto& codec : codecs) {
    codec_names.push_back(codec.name);
  }
  return codec_names;
}

// This is used for test only. MIDs are not the identification of the
// MediaDescriptionOptions since some end points may not support MID and the SDP
// may not contain 'mid'.
std::vector<MediaDescriptionOptions>::iterator FindFirstMediaDescriptionByMid(
    const std::string& mid,
    MediaSessionOptions* opts) {
  return absl::c_find_if(
      opts->media_description_options,
      [&mid](const MediaDescriptionOptions& t) { return t.mid == mid; });
}

std::vector<MediaDescriptionOptions>::const_iterator
FindFirstMediaDescriptionByMid(const std::string& mid,
                               const MediaSessionOptions& opts) {
  return absl::c_find_if(
      opts.media_description_options,
      [&mid](const MediaDescriptionOptions& t) { return t.mid == mid; });
}

// Add a media section to the |session_options|.
static void AddMediaDescriptionOptions(MediaType type,
                                       const std::string& mid,
                                       RtpTransceiverDirection direction,
                                       bool stopped,
                                       MediaSessionOptions* opts) {
  opts->media_description_options.push_back(
      MediaDescriptionOptions(type, mid, direction, stopped));
}

static void AddAudioVideoSections(RtpTransceiverDirection direction,
                                  MediaSessionOptions* opts) {
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio", direction, kActive,
                             opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video", direction, kActive,
                             opts);
}

static void AddDataSection(RtpTransceiverDirection direction,
                           MediaSessionOptions* opts) {
  AddMediaDescriptionOptions(MEDIA_TYPE_DATA, "data", direction, kActive, opts);
}

static void AttachSenderToMediaDescriptionOptions(
    const std::string& mid,
    MediaType type,
    const std::string& track_id,
    const std::vector<std::string>& stream_ids,
    const std::vector<RidDescription>& rids,
    const SimulcastLayerList& simulcast_layers,
    int num_sim_layer,
    MediaSessionOptions* session_options) {
  auto it = FindFirstMediaDescriptionByMid(mid, session_options);
  switch (type) {
    case MEDIA_TYPE_AUDIO:
      it->AddAudioSender(track_id, stream_ids);
      break;
    case MEDIA_TYPE_VIDEO:
      it->AddVideoSender(track_id, stream_ids, rids, simulcast_layers,
                         num_sim_layer);
      break;
    default:
      RTC_NOTREACHED();
  }
}

static void AttachSenderToMediaDescriptionOptions(
    const std::string& mid,
    MediaType type,
    const std::string& track_id,
    const std::vector<std::string>& stream_ids,
    int num_sim_layer,
    MediaSessionOptions* session_options) {
  AttachSenderToMediaDescriptionOptions(mid, type, track_id, stream_ids, {},
                                        SimulcastLayerList(), num_sim_layer,
                                        session_options);
}

static void DetachSenderFromMediaSection(const std::string& mid,
                                         const std::string& track_id,
                                         MediaSessionOptions* session_options) {
  std::vector<cricket::SenderOptions>& sender_options_list =
      FindFirstMediaDescriptionByMid(mid, session_options)->sender_options;
  auto sender_it =
      absl::c_find_if(sender_options_list,
                      [track_id](const cricket::SenderOptions& sender_options) {
                        return sender_options.track_id == track_id;
                      });
  RTC_DCHECK(sender_it != sender_options_list.end());
  sender_options_list.erase(sender_it);
}

// Helper function used to create a default MediaSessionOptions for Plan B SDP.
// (https://tools.ietf.org/html/draft-uberti-rtcweb-plan-00).
static MediaSessionOptions CreatePlanBMediaSessionOptions() {
  MediaSessionOptions session_options;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &session_options);
  return session_options;
}

// prefers GCM SDES crypto suites by removing non-GCM defaults.
void PreferGcmCryptoParameters(CryptoParamsVec* cryptos) {
  cryptos->erase(
      std::remove_if(cryptos->begin(), cryptos->end(),
                     [](const cricket::CryptoParams& crypto) {
                       return crypto.cipher_suite != CS_AEAD_AES_256_GCM &&
                              crypto.cipher_suite != CS_AEAD_AES_128_GCM;
                     }),
      cryptos->end());
}

// TODO(zhihuang): Most of these tests were written while MediaSessionOptions
// was designed for Plan B SDP, where only one audio "m=" section and one video
// "m=" section could be generated, and ordering couldn't be controlled. Many of
// these tests may be obsolete as a result, and should be refactored or removed.
class MediaSessionDescriptionFactoryTest : public ::testing::Test {
 public:
  MediaSessionDescriptionFactoryTest()
      : f1_(&tdf1_, &ssrc_generator1), f2_(&tdf2_, &ssrc_generator2) {
    f1_.set_audio_codecs(MAKE_VECTOR(kAudioCodecs1),
                         MAKE_VECTOR(kAudioCodecs1));
    f1_.set_video_codecs(MAKE_VECTOR(kVideoCodecs1),
                         MAKE_VECTOR(kVideoCodecs1));
    f2_.set_audio_codecs(MAKE_VECTOR(kAudioCodecs2),
                         MAKE_VECTOR(kAudioCodecs2));
    f2_.set_video_codecs(MAKE_VECTOR(kVideoCodecs2),
                         MAKE_VECTOR(kVideoCodecs2));
    tdf1_.set_certificate(rtc::RTCCertificate::Create(
        std::unique_ptr<rtc::SSLIdentity>(new rtc::FakeSSLIdentity("id1"))));
    tdf2_.set_certificate(rtc::RTCCertificate::Create(
        std::unique_ptr<rtc::SSLIdentity>(new rtc::FakeSSLIdentity("id2"))));
  }

  // Create a video StreamParamsVec object with:
  // - one video stream with 3 simulcast streams and FEC,
  StreamParamsVec CreateComplexVideoStreamParamsVec() {
    SsrcGroup sim_group("SIM", MAKE_VECTOR(kSimSsrc));
    SsrcGroup fec_group1("FEC", MAKE_VECTOR(kFec1Ssrc));
    SsrcGroup fec_group2("FEC", MAKE_VECTOR(kFec2Ssrc));
    SsrcGroup fec_group3("FEC", MAKE_VECTOR(kFec3Ssrc));

    std::vector<SsrcGroup> ssrc_groups;
    ssrc_groups.push_back(sim_group);
    ssrc_groups.push_back(fec_group1);
    ssrc_groups.push_back(fec_group2);
    ssrc_groups.push_back(fec_group3);

    StreamParams simulcast_params;
    simulcast_params.id = kVideoTrack1;
    simulcast_params.ssrcs = MAKE_VECTOR(kSimulcastParamsSsrc);
    simulcast_params.ssrc_groups = ssrc_groups;
    simulcast_params.cname = "Video_SIM_FEC";
    simulcast_params.set_stream_ids({kMediaStream1});

    StreamParamsVec video_streams;
    video_streams.push_back(simulcast_params);

    return video_streams;
  }

  bool CompareCryptoParams(const CryptoParamsVec& c1,
                           const CryptoParamsVec& c2) {
    if (c1.size() != c2.size())
      return false;
    for (size_t i = 0; i < c1.size(); ++i)
      if (c1[i].tag != c2[i].tag || c1[i].cipher_suite != c2[i].cipher_suite ||
          c1[i].key_params != c2[i].key_params ||
          c1[i].session_params != c2[i].session_params)
        return false;
    return true;
  }

  // Returns true if the transport info contains "renomination" as an
  // ICE option.
  bool GetIceRenomination(const TransportInfo* transport_info) {
    return absl::c_linear_search(transport_info->description.transport_options,
                                 "renomination");
  }

  void TestTransportInfo(bool offer,
                         const MediaSessionOptions& options,
                         bool has_current_desc) {
    const std::string current_audio_ufrag = "current_audio_ufrag";
    const std::string current_audio_pwd = "current_audio_pwd";
    const std::string current_video_ufrag = "current_video_ufrag";
    const std::string current_video_pwd = "current_video_pwd";
    const std::string current_data_ufrag = "current_data_ufrag";
    const std::string current_data_pwd = "current_data_pwd";
    std::unique_ptr<SessionDescription> current_desc;
    std::unique_ptr<SessionDescription> desc;
    if (has_current_desc) {
      current_desc = std::make_unique<SessionDescription>();
      current_desc->AddTransportInfo(TransportInfo(
          "audio",
          TransportDescription(current_audio_ufrag, current_audio_pwd)));
      current_desc->AddTransportInfo(TransportInfo(
          "video",
          TransportDescription(current_video_ufrag, current_video_pwd)));
      current_desc->AddTransportInfo(TransportInfo(
          "data", TransportDescription(current_data_ufrag, current_data_pwd)));
    }
    if (offer) {
      desc = f1_.CreateOffer(options, current_desc.get());
    } else {
      std::unique_ptr<SessionDescription> offer;
      offer = f1_.CreateOffer(options, NULL);
      desc = f1_.CreateAnswer(offer.get(), options, current_desc.get());
    }
    ASSERT_TRUE(desc.get() != NULL);
    const TransportInfo* ti_audio = desc->GetTransportInfoByName("audio");
    if (options.has_audio()) {
      EXPECT_TRUE(ti_audio != NULL);
      if (has_current_desc) {
        EXPECT_EQ(current_audio_ufrag, ti_audio->description.ice_ufrag);
        EXPECT_EQ(current_audio_pwd, ti_audio->description.ice_pwd);
      } else {
        EXPECT_EQ(static_cast<size_t>(cricket::ICE_UFRAG_LENGTH),
                  ti_audio->description.ice_ufrag.size());
        EXPECT_EQ(static_cast<size_t>(cricket::ICE_PWD_LENGTH),
                  ti_audio->description.ice_pwd.size());
      }
      auto media_desc_options_it =
          FindFirstMediaDescriptionByMid("audio", options);
      EXPECT_EQ(
          media_desc_options_it->transport_options.enable_ice_renomination,
          GetIceRenomination(ti_audio));
    } else {
      EXPECT_TRUE(ti_audio == NULL);
    }
    const TransportInfo* ti_video = desc->GetTransportInfoByName("video");
    if (options.has_video()) {
      EXPECT_TRUE(ti_video != NULL);
      auto media_desc_options_it =
          FindFirstMediaDescriptionByMid("video", options);
      if (options.bundle_enabled) {
        EXPECT_EQ(ti_audio->description.ice_ufrag,
                  ti_video->description.ice_ufrag);
        EXPECT_EQ(ti_audio->description.ice_pwd, ti_video->description.ice_pwd);
      } else {
        if (has_current_desc) {
          EXPECT_EQ(current_video_ufrag, ti_video->description.ice_ufrag);
          EXPECT_EQ(current_video_pwd, ti_video->description.ice_pwd);
        } else {
          EXPECT_EQ(static_cast<size_t>(cricket::ICE_UFRAG_LENGTH),
                    ti_video->description.ice_ufrag.size());
          EXPECT_EQ(static_cast<size_t>(cricket::ICE_PWD_LENGTH),
                    ti_video->description.ice_pwd.size());
        }
      }
      EXPECT_EQ(
          media_desc_options_it->transport_options.enable_ice_renomination,
          GetIceRenomination(ti_video));
    } else {
      EXPECT_TRUE(ti_video == NULL);
    }
    const TransportInfo* ti_data = desc->GetTransportInfoByName("data");
    if (options.has_data()) {
      EXPECT_TRUE(ti_data != NULL);
      if (options.bundle_enabled) {
        EXPECT_EQ(ti_audio->description.ice_ufrag,
                  ti_data->description.ice_ufrag);
        EXPECT_EQ(ti_audio->description.ice_pwd, ti_data->description.ice_pwd);
      } else {
        if (has_current_desc) {
          EXPECT_EQ(current_data_ufrag, ti_data->description.ice_ufrag);
          EXPECT_EQ(current_data_pwd, ti_data->description.ice_pwd);
        } else {
          EXPECT_EQ(static_cast<size_t>(cricket::ICE_UFRAG_LENGTH),
                    ti_data->description.ice_ufrag.size());
          EXPECT_EQ(static_cast<size_t>(cricket::ICE_PWD_LENGTH),
                    ti_data->description.ice_pwd.size());
        }
      }
      auto media_desc_options_it =
          FindFirstMediaDescriptionByMid("data", options);
      EXPECT_EQ(
          media_desc_options_it->transport_options.enable_ice_renomination,
          GetIceRenomination(ti_data));

    } else {
      EXPECT_TRUE(ti_data == NULL);
    }
  }

  void TestCryptoWithBundle(bool offer) {
    f1_.set_secure(SEC_ENABLED);
    MediaSessionOptions options;
    AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
    std::unique_ptr<SessionDescription> ref_desc;
    std::unique_ptr<SessionDescription> desc;
    if (offer) {
      options.bundle_enabled = false;
      ref_desc = f1_.CreateOffer(options, NULL);
      options.bundle_enabled = true;
      desc = f1_.CreateOffer(options, ref_desc.get());
    } else {
      options.bundle_enabled = true;
      ref_desc = f1_.CreateOffer(options, NULL);
      desc = f1_.CreateAnswer(ref_desc.get(), options, NULL);
    }
    ASSERT_TRUE(desc);
    const cricket::MediaContentDescription* audio_media_desc =
        desc->GetContentDescriptionByName("audio");
    ASSERT_TRUE(audio_media_desc);
    const cricket::MediaContentDescription* video_media_desc =
        desc->GetContentDescriptionByName("video");
    ASSERT_TRUE(video_media_desc);
    EXPECT_TRUE(CompareCryptoParams(audio_media_desc->cryptos(),
                                    video_media_desc->cryptos()));
    EXPECT_EQ(1u, audio_media_desc->cryptos().size());
    EXPECT_EQ(kDefaultSrtpCryptoSuite,
              audio_media_desc->cryptos()[0].cipher_suite);

    // Verify the selected crypto is one from the reference audio
    // media content.
    const cricket::MediaContentDescription* ref_audio_media_desc =
        ref_desc->GetContentDescriptionByName("audio");
    bool found = false;
    for (size_t i = 0; i < ref_audio_media_desc->cryptos().size(); ++i) {
      if (ref_audio_media_desc->cryptos()[i].Matches(
              audio_media_desc->cryptos()[0])) {
        found = true;
        break;
      }
    }
    EXPECT_TRUE(found);
  }

  // This test that the audio and video media direction is set to
  // |expected_direction_in_answer| in an answer if the offer direction is set
  // to |direction_in_offer| and the answer is willing to both send and receive.
  void TestMediaDirectionInAnswer(
      RtpTransceiverDirection direction_in_offer,
      RtpTransceiverDirection expected_direction_in_answer) {
    MediaSessionOptions offer_opts;
    AddAudioVideoSections(direction_in_offer, &offer_opts);

    std::unique_ptr<SessionDescription> offer =
        f1_.CreateOffer(offer_opts, NULL);
    ASSERT_TRUE(offer.get() != NULL);
    ContentInfo* ac_offer = offer->GetContentByName("audio");
    ASSERT_TRUE(ac_offer != NULL);
    ContentInfo* vc_offer = offer->GetContentByName("video");
    ASSERT_TRUE(vc_offer != NULL);

    MediaSessionOptions answer_opts;
    AddAudioVideoSections(RtpTransceiverDirection::kSendRecv, &answer_opts);
    std::unique_ptr<SessionDescription> answer =
        f2_.CreateAnswer(offer.get(), answer_opts, NULL);
    const AudioContentDescription* acd_answer =
        GetFirstAudioContentDescription(answer.get());
    EXPECT_EQ(expected_direction_in_answer, acd_answer->direction());
    const VideoContentDescription* vcd_answer =
        GetFirstVideoContentDescription(answer.get());
    EXPECT_EQ(expected_direction_in_answer, vcd_answer->direction());
  }

  bool VerifyNoCNCodecs(const cricket::ContentInfo* content) {
    RTC_DCHECK(content);
    RTC_CHECK(content->media_description());
    const cricket::AudioContentDescription* audio_desc =
        content->media_description()->as_audio();
    RTC_CHECK(audio_desc);
    for (const cricket::AudioCodec& codec : audio_desc->codecs()) {
      if (codec.name == "CN") {
        return false;
      }
    }
    return true;
  }

  void TestVideoGcmCipher(bool gcm_offer, bool gcm_answer) {
    MediaSessionOptions offer_opts;
    AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &offer_opts);
    offer_opts.crypto_options.srtp.enable_gcm_crypto_suites = gcm_offer;

    MediaSessionOptions answer_opts;
    AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &answer_opts);
    answer_opts.crypto_options.srtp.enable_gcm_crypto_suites = gcm_answer;

    f1_.set_secure(SEC_ENABLED);
    f2_.set_secure(SEC_ENABLED);
    std::unique_ptr<SessionDescription> offer =
        f1_.CreateOffer(offer_opts, NULL);
    ASSERT_TRUE(offer.get() != NULL);
    if (gcm_offer && gcm_answer) {
      for (cricket::ContentInfo& content : offer->contents()) {
        auto cryptos = content.media_description()->cryptos();
        PreferGcmCryptoParameters(&cryptos);
        content.media_description()->set_cryptos(cryptos);
      }
    }
    std::unique_ptr<SessionDescription> answer =
        f2_.CreateAnswer(offer.get(), answer_opts, NULL);
    const ContentInfo* ac = answer->GetContentByName("audio");
    const ContentInfo* vc = answer->GetContentByName("video");
    ASSERT_TRUE(ac != NULL);
    ASSERT_TRUE(vc != NULL);
    EXPECT_EQ(MediaProtocolType::kRtp, ac->type);
    EXPECT_EQ(MediaProtocolType::kRtp, vc->type);
    const AudioContentDescription* acd = ac->media_description()->as_audio();
    const VideoContentDescription* vcd = vc->media_description()->as_video();
    EXPECT_EQ(MEDIA_TYPE_AUDIO, acd->type());
    EXPECT_THAT(acd->codecs(), ElementsAreArray(kAudioCodecsAnswer));
    EXPECT_EQ(kAutoBandwidth, acd->bandwidth());  // negotiated auto bw
    EXPECT_EQ(0U, acd->first_ssrc());             // no sender is attached
    EXPECT_TRUE(acd->rtcp_mux());                 // negotiated rtcp-mux
    if (gcm_offer && gcm_answer) {
      ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuiteGcm);
    } else {
      ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuite);
    }
    EXPECT_EQ(MEDIA_TYPE_VIDEO, vcd->type());
    EXPECT_THAT(vcd->codecs(), ElementsAreArray(kVideoCodecsAnswer));
    EXPECT_EQ(0U, vcd->first_ssrc());  // no sender is attached
    EXPECT_TRUE(vcd->rtcp_mux());      // negotiated rtcp-mux
    if (gcm_offer && gcm_answer) {
      ASSERT_CRYPTO(vcd, 1U, kDefaultSrtpCryptoSuiteGcm);
    } else {
      ASSERT_CRYPTO(vcd, 1U, kDefaultSrtpCryptoSuite);
    }
    EXPECT_EQ(cricket::kMediaProtocolSavpf, vcd->protocol());
  }

  void TestTransportSequenceNumberNegotiation(
      const cricket::RtpHeaderExtensions& local,
      const cricket::RtpHeaderExtensions& offered,
      const cricket::RtpHeaderExtensions& expectedAnswer) {
    MediaSessionOptions opts;
    AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);
    SetAudioVideoRtpHeaderExtensions(offered, offered, &opts);
    std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
    ASSERT_TRUE(offer.get() != NULL);
    SetAudioVideoRtpHeaderExtensions(local, local, &opts);
    std::unique_ptr<SessionDescription> answer =
        f2_.CreateAnswer(offer.get(), opts, NULL);

    EXPECT_EQ(
        expectedAnswer,
        GetFirstAudioContentDescription(answer.get())->rtp_header_extensions());
    EXPECT_EQ(
        expectedAnswer,
        GetFirstVideoContentDescription(answer.get())->rtp_header_extensions());
  }

  std::vector<webrtc::RtpHeaderExtensionCapability>
  HeaderExtensionCapabilitiesFromRtpExtensions(
      cricket::RtpHeaderExtensions extensions) {
    std::vector<webrtc::RtpHeaderExtensionCapability> capabilities;
    for (const auto& extension : extensions) {
      webrtc::RtpHeaderExtensionCapability capability(
          extension.uri, extension.id,
          webrtc::RtpTransceiverDirection::kSendRecv);
      capabilities.push_back(capability);
    }
    return capabilities;
  }

  void SetAudioVideoRtpHeaderExtensions(cricket::RtpHeaderExtensions audio_exts,
                                        cricket::RtpHeaderExtensions video_exts,
                                        MediaSessionOptions* opts) {
    auto audio_caps = HeaderExtensionCapabilitiesFromRtpExtensions(audio_exts);
    auto video_caps = HeaderExtensionCapabilitiesFromRtpExtensions(video_exts);
    for (auto& entry : opts->media_description_options) {
      switch (entry.type) {
        case MEDIA_TYPE_AUDIO:
          entry.header_extensions = audio_caps;
          break;
        case MEDIA_TYPE_VIDEO:
          entry.header_extensions = video_caps;
          break;
        default:
          break;
      }
    }
  }

 protected:
  UniqueRandomIdGenerator ssrc_generator1;
  UniqueRandomIdGenerator ssrc_generator2;
  MediaSessionDescriptionFactory f1_;
  MediaSessionDescriptionFactory f2_;
  TransportDescriptionFactory tdf1_;
  TransportDescriptionFactory tdf2_;
};

// Create a typical audio offer, and ensure it matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateAudioOffer) {
  f1_.set_secure(SEC_ENABLED);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(CreatePlanBMediaSessionOptions(), NULL);
  ASSERT_TRUE(offer.get() != NULL);
  const ContentInfo* ac = offer->GetContentByName("audio");
  const ContentInfo* vc = offer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc == NULL);
  EXPECT_EQ(MediaProtocolType::kRtp, ac->type);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  EXPECT_EQ(MEDIA_TYPE_AUDIO, acd->type());
  EXPECT_EQ(f1_.audio_sendrecv_codecs(), acd->codecs());
  EXPECT_EQ(0U, acd->first_ssrc());             // no sender is attached.
  EXPECT_EQ(kAutoBandwidth, acd->bandwidth());  // default bandwidth (auto)
  EXPECT_TRUE(acd->rtcp_mux());                 // rtcp-mux defaults on
  ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_EQ(cricket::kMediaProtocolSavpf, acd->protocol());
}

// Create a typical video offer, and ensure it matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateVideoOffer) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);
  f1_.set_secure(SEC_ENABLED);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  const ContentInfo* ac = offer->GetContentByName("audio");
  const ContentInfo* vc = offer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  EXPECT_EQ(MediaProtocolType::kRtp, ac->type);
  EXPECT_EQ(MediaProtocolType::kRtp, vc->type);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  const VideoContentDescription* vcd = vc->media_description()->as_video();
  EXPECT_EQ(MEDIA_TYPE_AUDIO, acd->type());
  EXPECT_EQ(f1_.audio_sendrecv_codecs(), acd->codecs());
  EXPECT_EQ(0U, acd->first_ssrc());             // no sender is attached
  EXPECT_EQ(kAutoBandwidth, acd->bandwidth());  // default bandwidth (auto)
  EXPECT_TRUE(acd->rtcp_mux());                 // rtcp-mux defaults on
  ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_EQ(cricket::kMediaProtocolSavpf, acd->protocol());
  EXPECT_EQ(MEDIA_TYPE_VIDEO, vcd->type());
  EXPECT_EQ(f1_.video_sendrecv_codecs(), vcd->codecs());
  EXPECT_EQ(0U, vcd->first_ssrc());             // no sender is attached
  EXPECT_EQ(kAutoBandwidth, vcd->bandwidth());  // default bandwidth (auto)
  EXPECT_TRUE(vcd->rtcp_mux());                 // rtcp-mux defaults on
  ASSERT_CRYPTO(vcd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_EQ(cricket::kMediaProtocolSavpf, vcd->protocol());
}

// Test creating an offer with bundle where the Codecs have the same dynamic
// RTP playlod type. The test verifies that the offer don't contain the
// duplicate RTP payload types.
TEST_F(MediaSessionDescriptionFactoryTest, TestBundleOfferWithSameCodecPlType) {
  const VideoCodec& offered_video_codec = f2_.video_sendrecv_codecs()[0];
  const AudioCodec& offered_audio_codec = f2_.audio_sendrecv_codecs()[0];
  ASSERT_EQ(offered_video_codec.id, offered_audio_codec.id);

  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);
  opts.bundle_enabled = true;
  std::unique_ptr<SessionDescription> offer = f2_.CreateOffer(opts, NULL);
  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(offer.get());
  const AudioContentDescription* acd =
      GetFirstAudioContentDescription(offer.get());
  ASSERT_TRUE(NULL != vcd);
  ASSERT_TRUE(NULL != acd);
  EXPECT_NE(vcd->codecs()[0].id, acd->codecs()[0].id);
  EXPECT_EQ(vcd->codecs()[0].name, offered_video_codec.name);
  EXPECT_EQ(acd->codecs()[0].name, offered_audio_codec.name);
}

// Test creating an updated offer with bundle, audio, video and data
// after an audio only session has been negotiated.
TEST_F(MediaSessionDescriptionFactoryTest,
       TestCreateUpdatedVideoOfferWithBundle) {
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kInactive, kStopped,
                             &opts);
  opts.bundle_enabled = true;
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  MediaSessionOptions updated_opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &updated_opts);
  updated_opts.bundle_enabled = true;
  std::unique_ptr<SessionDescription> updated_offer(
      f1_.CreateOffer(updated_opts, answer.get()));

  const AudioContentDescription* acd =
      GetFirstAudioContentDescription(updated_offer.get());
  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(updated_offer.get());
  EXPECT_TRUE(NULL != vcd);
  EXPECT_TRUE(NULL != acd);

  ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_EQ(cricket::kMediaProtocolSavpf, acd->protocol());
  ASSERT_CRYPTO(vcd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_EQ(cricket::kMediaProtocolSavpf, vcd->protocol());
}

// Create an SCTP data offer with bundle without error.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateSctpDataOffer) {
  MediaSessionOptions opts;
  opts.bundle_enabled = true;
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);
  f1_.set_secure(SEC_ENABLED);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  EXPECT_TRUE(offer.get() != NULL);
  EXPECT_TRUE(offer->GetContentByName("data") != NULL);
  auto dcd = GetFirstSctpDataContentDescription(offer.get());
  ASSERT_TRUE(dcd);
  // Since this transport is insecure, the protocol should be "SCTP".
  EXPECT_EQ(cricket::kMediaProtocolSctp, dcd->protocol());
}

// Create an SCTP data offer with bundle without error.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateSecureSctpDataOffer) {
  MediaSessionOptions opts;
  opts.bundle_enabled = true;
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);
  f1_.set_secure(SEC_ENABLED);
  tdf1_.set_secure(SEC_ENABLED);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  EXPECT_TRUE(offer.get() != NULL);
  EXPECT_TRUE(offer->GetContentByName("data") != NULL);
  auto dcd = GetFirstSctpDataContentDescription(offer.get());
  ASSERT_TRUE(dcd);
  // The protocol should now be "UDP/DTLS/SCTP"
  EXPECT_EQ(cricket::kMediaProtocolUdpDtlsSctp, dcd->protocol());
}

// Test creating an sctp data channel from an already generated offer.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateImplicitSctpDataOffer) {
  MediaSessionOptions opts;
  opts.bundle_enabled = true;
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);
  f1_.set_secure(SEC_ENABLED);
  std::unique_ptr<SessionDescription> offer1(f1_.CreateOffer(opts, NULL));
  ASSERT_TRUE(offer1.get() != NULL);
  const ContentInfo* data = offer1->GetContentByName("data");
  ASSERT_TRUE(data != NULL);
  ASSERT_EQ(cricket::kMediaProtocolSctp, data->media_description()->protocol());

  std::unique_ptr<SessionDescription> offer2(
      f1_.CreateOffer(opts, offer1.get()));
  data = offer2->GetContentByName("data");
  ASSERT_TRUE(data != NULL);
  EXPECT_EQ(cricket::kMediaProtocolSctp, data->media_description()->protocol());
}

// Test that if BUNDLE is enabled and all media sections are rejected then the
// BUNDLE group is not present in the re-offer.
TEST_F(MediaSessionDescriptionFactoryTest, ReOfferNoBundleGroupIfAllRejected) {
  MediaSessionOptions opts;
  opts.bundle_enabled = true;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);

  opts.media_description_options[0].stopped = true;
  std::unique_ptr<SessionDescription> reoffer =
      f1_.CreateOffer(opts, offer.get());

  EXPECT_FALSE(reoffer->GetGroupByName(cricket::GROUP_TYPE_BUNDLE));
}

// Test that if BUNDLE is enabled and the remote re-offer does not include a
// BUNDLE group since all media sections are rejected, then the re-answer also
// does not include a BUNDLE group.
TEST_F(MediaSessionDescriptionFactoryTest, ReAnswerNoBundleGroupIfAllRejected) {
  MediaSessionOptions opts;
  opts.bundle_enabled = true;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);

  opts.media_description_options[0].stopped = true;
  std::unique_ptr<SessionDescription> reoffer =
      f1_.CreateOffer(opts, offer.get());
  std::unique_ptr<SessionDescription> reanswer =
      f2_.CreateAnswer(reoffer.get(), opts, answer.get());

  EXPECT_FALSE(reanswer->GetGroupByName(cricket::GROUP_TYPE_BUNDLE));
}

// Test that if BUNDLE is enabled and the previous offerer-tagged media section
// was rejected then the new offerer-tagged media section is the non-rejected
// media section.
TEST_F(MediaSessionDescriptionFactoryTest, ReOfferChangeBundleOffererTagged) {
  MediaSessionOptions opts;
  opts.bundle_enabled = true;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);

  // Reject the audio m= section and add a video m= section.
  opts.media_description_options[0].stopped = true;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> reoffer =
      f1_.CreateOffer(opts, offer.get());

  const cricket::ContentGroup* bundle_group =
      reoffer->GetGroupByName(cricket::GROUP_TYPE_BUNDLE);
  ASSERT_TRUE(bundle_group);
  EXPECT_FALSE(bundle_group->HasContentName("audio"));
  EXPECT_TRUE(bundle_group->HasContentName("video"));
}

// Test that if BUNDLE is enabled and the previous offerer-tagged media section
// was rejected and a new media section is added, then the re-answer BUNDLE
// group will contain only the non-rejected media section.
TEST_F(MediaSessionDescriptionFactoryTest, ReAnswerChangedBundleOffererTagged) {
  MediaSessionOptions opts;
  opts.bundle_enabled = true;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);

  // Reject the audio m= section and add a video m= section.
  opts.media_description_options[0].stopped = true;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> reoffer =
      f1_.CreateOffer(opts, offer.get());
  std::unique_ptr<SessionDescription> reanswer =
      f2_.CreateAnswer(reoffer.get(), opts, answer.get());

  const cricket::ContentGroup* bundle_group =
      reanswer->GetGroupByName(cricket::GROUP_TYPE_BUNDLE);
  ASSERT_TRUE(bundle_group);
  EXPECT_FALSE(bundle_group->HasContentName("audio"));
  EXPECT_TRUE(bundle_group->HasContentName("video"));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       CreateAnswerForOfferWithMultipleBundleGroups) {
  // Create an offer with 4 m= sections, initially without BUNDLE groups.
  MediaSessionOptions opts;
  opts.bundle_enabled = false;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "3",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "4",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer->groups().empty());

  // Munge the offer to have two groups. Offers like these cannot be generated
  // without munging, but it is valid to receive such offers from remote
  // endpoints.
  cricket::ContentGroup bundle_group1(cricket::GROUP_TYPE_BUNDLE);
  bundle_group1.AddContentName("1");
  bundle_group1.AddContentName("2");
  cricket::ContentGroup bundle_group2(cricket::GROUP_TYPE_BUNDLE);
  bundle_group2.AddContentName("3");
  bundle_group2.AddContentName("4");
  offer->AddGroup(bundle_group1);
  offer->AddGroup(bundle_group2);

  // If BUNDLE is enabled, the answer to this offer should accept both BUNDLE
  // groups.
  opts.bundle_enabled = true;
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);

  std::vector<const cricket::ContentGroup*> answer_groups =
      answer->GetGroupsByName(cricket::GROUP_TYPE_BUNDLE);
  ASSERT_EQ(answer_groups.size(), 2u);
  EXPECT_EQ(answer_groups[0]->content_names().size(), 2u);
  EXPECT_TRUE(answer_groups[0]->HasContentName("1"));
  EXPECT_TRUE(answer_groups[0]->HasContentName("2"));
  EXPECT_EQ(answer_groups[1]->content_names().size(), 2u);
  EXPECT_TRUE(answer_groups[1]->HasContentName("3"));
  EXPECT_TRUE(answer_groups[1]->HasContentName("4"));

  // If BUNDLE is disabled, the answer to this offer should reject both BUNDLE
  // groups.
  opts.bundle_enabled = false;
  answer = f2_.CreateAnswer(offer.get(), opts, nullptr);

  answer_groups = answer->GetGroupsByName(cricket::GROUP_TYPE_BUNDLE);
  // Rejected groups are still listed, but they are empty.
  ASSERT_EQ(answer_groups.size(), 2u);
  EXPECT_TRUE(answer_groups[0]->content_names().empty());
  EXPECT_TRUE(answer_groups[1]->content_names().empty());
}

// Test that if the BUNDLE offerer-tagged media section is changed in a reoffer
// and there is still a non-rejected media section that was in the initial
// offer, then the ICE credentials do not change in the reoffer offerer-tagged
// media section.
TEST_F(MediaSessionDescriptionFactoryTest,
       ReOfferChangeBundleOffererTaggedKeepsIceCredentials) {
  MediaSessionOptions opts;
  opts.bundle_enabled = true;
  AddAudioVideoSections(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);

  // Reject the audio m= section.
  opts.media_description_options[0].stopped = true;
  std::unique_ptr<SessionDescription> reoffer =
      f1_.CreateOffer(opts, offer.get());

  const TransportDescription* offer_tagged =
      offer->GetTransportDescriptionByName("audio");
  ASSERT_TRUE(offer_tagged);
  const TransportDescription* reoffer_tagged =
      reoffer->GetTransportDescriptionByName("video");
  ASSERT_TRUE(reoffer_tagged);
  EXPECT_EQ(offer_tagged->ice_ufrag, reoffer_tagged->ice_ufrag);
  EXPECT_EQ(offer_tagged->ice_pwd, reoffer_tagged->ice_pwd);
}

// Test that if the BUNDLE offerer-tagged media section is changed in a reoffer
// and there is still a non-rejected media section that was in the initial
// offer, then the ICE credentials do not change in the reanswer answerer-tagged
// media section.
TEST_F(MediaSessionDescriptionFactoryTest,
       ReAnswerChangeBundleOffererTaggedKeepsIceCredentials) {
  MediaSessionOptions opts;
  opts.bundle_enabled = true;
  AddAudioVideoSections(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);

  // Reject the audio m= section.
  opts.media_description_options[0].stopped = true;
  std::unique_ptr<SessionDescription> reoffer =
      f1_.CreateOffer(opts, offer.get());
  std::unique_ptr<SessionDescription> reanswer =
      f2_.CreateAnswer(reoffer.get(), opts, answer.get());

  const TransportDescription* answer_tagged =
      answer->GetTransportDescriptionByName("audio");
  ASSERT_TRUE(answer_tagged);
  const TransportDescription* reanswer_tagged =
      reanswer->GetTransportDescriptionByName("video");
  ASSERT_TRUE(reanswer_tagged);
  EXPECT_EQ(answer_tagged->ice_ufrag, reanswer_tagged->ice_ufrag);
  EXPECT_EQ(answer_tagged->ice_pwd, reanswer_tagged->ice_pwd);
}

// Create an audio, video offer without legacy StreamParams.
TEST_F(MediaSessionDescriptionFactoryTest,
       TestCreateOfferWithoutLegacyStreams) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  const ContentInfo* ac = offer->GetContentByName("audio");
  const ContentInfo* vc = offer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  const VideoContentDescription* vcd = vc->media_description()->as_video();

  EXPECT_FALSE(vcd->has_ssrcs());  // No StreamParams.
  EXPECT_FALSE(acd->has_ssrcs());  // No StreamParams.
}

// Creates an audio+video sendonly offer.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateSendOnlyOffer) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kSendOnly, &opts);
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, 1, &opts);
  AttachSenderToMediaDescriptionOptions("audio", MEDIA_TYPE_AUDIO, kAudioTrack1,
                                        {kMediaStream1}, 1, &opts);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  EXPECT_EQ(2u, offer->contents().size());
  EXPECT_TRUE(IsMediaContentOfType(&offer->contents()[0], MEDIA_TYPE_AUDIO));
  EXPECT_TRUE(IsMediaContentOfType(&offer->contents()[1], MEDIA_TYPE_VIDEO));

  EXPECT_EQ(RtpTransceiverDirection::kSendOnly,
            GetMediaDirection(&offer->contents()[0]));
  EXPECT_EQ(RtpTransceiverDirection::kSendOnly,
            GetMediaDirection(&offer->contents()[1]));
}

// Verifies that the order of the media contents in the current
// SessionDescription is preserved in the new SessionDescription.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateOfferContentOrder) {
  MediaSessionOptions opts;
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);

  std::unique_ptr<SessionDescription> offer1(f1_.CreateOffer(opts, NULL));
  ASSERT_TRUE(offer1.get() != NULL);
  EXPECT_EQ(1u, offer1->contents().size());
  EXPECT_TRUE(IsMediaContentOfType(&offer1->contents()[0], MEDIA_TYPE_DATA));

  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer2(
      f1_.CreateOffer(opts, offer1.get()));
  ASSERT_TRUE(offer2.get() != NULL);
  EXPECT_EQ(2u, offer2->contents().size());
  EXPECT_TRUE(IsMediaContentOfType(&offer2->contents()[0], MEDIA_TYPE_DATA));
  EXPECT_TRUE(IsMediaContentOfType(&offer2->contents()[1], MEDIA_TYPE_VIDEO));

  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer3(
      f1_.CreateOffer(opts, offer2.get()));
  ASSERT_TRUE(offer3.get() != NULL);
  EXPECT_EQ(3u, offer3->contents().size());
  EXPECT_TRUE(IsMediaContentOfType(&offer3->contents()[0], MEDIA_TYPE_DATA));
  EXPECT_TRUE(IsMediaContentOfType(&offer3->contents()[1], MEDIA_TYPE_VIDEO));
  EXPECT_TRUE(IsMediaContentOfType(&offer3->contents()[2], MEDIA_TYPE_AUDIO));
}

// Create a typical audio answer, and ensure it matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateAudioAnswer) {
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(CreatePlanBMediaSessionOptions(), NULL);
  ASSERT_TRUE(offer.get() != NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), CreatePlanBMediaSessionOptions(), NULL);
  const ContentInfo* ac = answer->GetContentByName("audio");
  const ContentInfo* vc = answer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc == NULL);
  EXPECT_EQ(MediaProtocolType::kRtp, ac->type);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  EXPECT_EQ(MEDIA_TYPE_AUDIO, acd->type());
  EXPECT_THAT(acd->codecs(), ElementsAreArray(kAudioCodecsAnswer));
  EXPECT_EQ(0U, acd->first_ssrc());             // no sender is attached
  EXPECT_EQ(kAutoBandwidth, acd->bandwidth());  // negotiated auto bw
  EXPECT_TRUE(acd->rtcp_mux());                 // negotiated rtcp-mux
  ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_EQ(cricket::kMediaProtocolSavpf, acd->protocol());
}

// Create a typical audio answer with GCM ciphers enabled, and ensure it
// matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateAudioAnswerGcm) {
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  MediaSessionOptions opts = CreatePlanBMediaSessionOptions();
  opts.crypto_options.srtp.enable_gcm_crypto_suites = true;
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  for (cricket::ContentInfo& content : offer->contents()) {
    auto cryptos = content.media_description()->cryptos();
    PreferGcmCryptoParameters(&cryptos);
    content.media_description()->set_cryptos(cryptos);
  }
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);
  const ContentInfo* ac = answer->GetContentByName("audio");
  const ContentInfo* vc = answer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc == NULL);
  EXPECT_EQ(MediaProtocolType::kRtp, ac->type);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  EXPECT_EQ(MEDIA_TYPE_AUDIO, acd->type());
  EXPECT_THAT(acd->codecs(), ElementsAreArray(kAudioCodecsAnswer));
  EXPECT_EQ(0U, acd->first_ssrc());             // no sender is attached
  EXPECT_EQ(kAutoBandwidth, acd->bandwidth());  // negotiated auto bw
  EXPECT_TRUE(acd->rtcp_mux());                 // negotiated rtcp-mux
  ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuiteGcm);
  EXPECT_EQ(cricket::kMediaProtocolSavpf, acd->protocol());
}

// Create a typical video answer, and ensure it matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateVideoAnswer) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);
  const ContentInfo* ac = answer->GetContentByName("audio");
  const ContentInfo* vc = answer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  EXPECT_EQ(MediaProtocolType::kRtp, ac->type);
  EXPECT_EQ(MediaProtocolType::kRtp, vc->type);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  const VideoContentDescription* vcd = vc->media_description()->as_video();
  EXPECT_EQ(MEDIA_TYPE_AUDIO, acd->type());
  EXPECT_THAT(acd->codecs(), ElementsAreArray(kAudioCodecsAnswer));
  EXPECT_EQ(kAutoBandwidth, acd->bandwidth());  // negotiated auto bw
  EXPECT_EQ(0U, acd->first_ssrc());             // no sender is attached
  EXPECT_TRUE(acd->rtcp_mux());                 // negotiated rtcp-mux
  ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_EQ(MEDIA_TYPE_VIDEO, vcd->type());
  EXPECT_THAT(vcd->codecs(), ElementsAreArray(kVideoCodecsAnswer));
  EXPECT_EQ(0U, vcd->first_ssrc());  // no sender is attached
  EXPECT_TRUE(vcd->rtcp_mux());      // negotiated rtcp-mux
  ASSERT_CRYPTO(vcd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_EQ(cricket::kMediaProtocolSavpf, vcd->protocol());
}

// Create a typical video answer with GCM ciphers enabled, and ensure it
// matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateVideoAnswerGcm) {
  TestVideoGcmCipher(true, true);
}

// Create a typical video answer with GCM ciphers enabled for the offer only,
// and ensure it matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateVideoAnswerGcmOffer) {
  TestVideoGcmCipher(true, false);
}

// Create a typical video answer with GCM ciphers enabled for the answer only,
// and ensure it matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateVideoAnswerGcmAnswer) {
  TestVideoGcmCipher(false, true);
}

// The use_sctpmap flag should be set in an Sctp DataContentDescription by
// default. The answer's use_sctpmap flag should match the offer's.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateDataAnswerUsesSctpmap) {
  MediaSessionOptions opts;
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  ContentInfo* dc_offer = offer->GetContentByName("data");
  ASSERT_TRUE(dc_offer != NULL);
  SctpDataContentDescription* dcd_offer =
      dc_offer->media_description()->as_sctp();
  EXPECT_TRUE(dcd_offer->use_sctpmap());

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);
  const ContentInfo* dc_answer = answer->GetContentByName("data");
  ASSERT_TRUE(dc_answer != NULL);
  const SctpDataContentDescription* dcd_answer =
      dc_answer->media_description()->as_sctp();
  EXPECT_TRUE(dcd_answer->use_sctpmap());
}

// The answer's use_sctpmap flag should match the offer's.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateDataAnswerWithoutSctpmap) {
  MediaSessionOptions opts;
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  ContentInfo* dc_offer = offer->GetContentByName("data");
  ASSERT_TRUE(dc_offer != NULL);
  SctpDataContentDescription* dcd_offer =
      dc_offer->media_description()->as_sctp();
  dcd_offer->set_use_sctpmap(false);

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);
  const ContentInfo* dc_answer = answer->GetContentByName("data");
  ASSERT_TRUE(dc_answer != NULL);
  const SctpDataContentDescription* dcd_answer =
      dc_answer->media_description()->as_sctp();
  EXPECT_FALSE(dcd_answer->use_sctpmap());
}

// Test that a valid answer will be created for "DTLS/SCTP", "UDP/DTLS/SCTP"
// and "TCP/DTLS/SCTP" offers.
TEST_F(MediaSessionDescriptionFactoryTest,
       TestCreateDataAnswerToDifferentOfferedProtos) {
  // Need to enable DTLS offer/answer generation (disabled by default in this
  // test).
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  tdf1_.set_secure(SEC_ENABLED);
  tdf2_.set_secure(SEC_ENABLED);

  MediaSessionOptions opts;
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer.get() != nullptr);
  ContentInfo* dc_offer = offer->GetContentByName("data");
  ASSERT_TRUE(dc_offer != nullptr);
  SctpDataContentDescription* dcd_offer =
      dc_offer->media_description()->as_sctp();
  ASSERT_TRUE(dcd_offer);

  std::vector<std::string> protos = {"DTLS/SCTP", "UDP/DTLS/SCTP",
                                     "TCP/DTLS/SCTP"};
  for (const std::string& proto : protos) {
    dcd_offer->set_protocol(proto);
    std::unique_ptr<SessionDescription> answer =
        f2_.CreateAnswer(offer.get(), opts, nullptr);
    const ContentInfo* dc_answer = answer->GetContentByName("data");
    ASSERT_TRUE(dc_answer != nullptr);
    const SctpDataContentDescription* dcd_answer =
        dc_answer->media_description()->as_sctp();
    EXPECT_FALSE(dc_answer->rejected);
    EXPECT_EQ(proto, dcd_answer->protocol());
  }
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestCreateDataAnswerToOfferWithDefinedMessageSize) {
  // Need to enable DTLS offer/answer generation (disabled by default in this
  // test).
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  tdf1_.set_secure(SEC_ENABLED);
  tdf2_.set_secure(SEC_ENABLED);

  MediaSessionOptions opts;
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer.get() != nullptr);
  ContentInfo* dc_offer = offer->GetContentByName("data");
  ASSERT_TRUE(dc_offer != nullptr);
  SctpDataContentDescription* dcd_offer =
      dc_offer->media_description()->as_sctp();
  ASSERT_TRUE(dcd_offer);
  dcd_offer->set_max_message_size(1234);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  const ContentInfo* dc_answer = answer->GetContentByName("data");
  ASSERT_TRUE(dc_answer != nullptr);
  const SctpDataContentDescription* dcd_answer =
      dc_answer->media_description()->as_sctp();
  EXPECT_FALSE(dc_answer->rejected);
  EXPECT_EQ(1234, dcd_answer->max_message_size());
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestCreateDataAnswerToOfferWithZeroMessageSize) {
  // Need to enable DTLS offer/answer generation (disabled by default in this
  // test).
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  tdf1_.set_secure(SEC_ENABLED);
  tdf2_.set_secure(SEC_ENABLED);

  MediaSessionOptions opts;
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer.get() != nullptr);
  ContentInfo* dc_offer = offer->GetContentByName("data");
  ASSERT_TRUE(dc_offer != nullptr);
  SctpDataContentDescription* dcd_offer =
      dc_offer->media_description()->as_sctp();
  ASSERT_TRUE(dcd_offer);
  dcd_offer->set_max_message_size(0);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  const ContentInfo* dc_answer = answer->GetContentByName("data");
  ASSERT_TRUE(dc_answer != nullptr);
  const SctpDataContentDescription* dcd_answer =
      dc_answer->media_description()->as_sctp();
  EXPECT_FALSE(dc_answer->rejected);
  EXPECT_EQ(cricket::kSctpSendBufferSize, dcd_answer->max_message_size());
}

// Verifies that the order of the media contents in the offer is preserved in
// the answer.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateAnswerContentOrder) {
  MediaSessionOptions opts;

  // Creates a data only offer.
  AddDataSection(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer1(f1_.CreateOffer(opts, NULL));
  ASSERT_TRUE(offer1.get() != NULL);

  // Appends audio to the offer.
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer2(
      f1_.CreateOffer(opts, offer1.get()));
  ASSERT_TRUE(offer2.get() != NULL);

  // Appends video to the offer.
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer3(
      f1_.CreateOffer(opts, offer2.get()));
  ASSERT_TRUE(offer3.get() != NULL);

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer3.get(), opts, NULL);
  ASSERT_TRUE(answer.get() != NULL);
  EXPECT_EQ(3u, answer->contents().size());
  EXPECT_TRUE(IsMediaContentOfType(&answer->contents()[0], MEDIA_TYPE_DATA));
  EXPECT_TRUE(IsMediaContentOfType(&answer->contents()[1], MEDIA_TYPE_AUDIO));
  EXPECT_TRUE(IsMediaContentOfType(&answer->contents()[2], MEDIA_TYPE_VIDEO));
}

// TODO(deadbeef): Extend these tests to ensure the correct direction with other
// answerer settings.

// This test that the media direction is set to send/receive in an answer if
// the offer is send receive.
TEST_F(MediaSessionDescriptionFactoryTest, CreateAnswerToSendReceiveOffer) {
  TestMediaDirectionInAnswer(RtpTransceiverDirection::kSendRecv,
                             RtpTransceiverDirection::kSendRecv);
}

// This test that the media direction is set to receive only in an answer if
// the offer is send only.
TEST_F(MediaSessionDescriptionFactoryTest, CreateAnswerToSendOnlyOffer) {
  TestMediaDirectionInAnswer(RtpTransceiverDirection::kSendOnly,
                             RtpTransceiverDirection::kRecvOnly);
}

// This test that the media direction is set to send only in an answer if
// the offer is recv only.
TEST_F(MediaSessionDescriptionFactoryTest, CreateAnswerToRecvOnlyOffer) {
  TestMediaDirectionInAnswer(RtpTransceiverDirection::kRecvOnly,
                             RtpTransceiverDirection::kSendOnly);
}

// This test that the media direction is set to inactive in an answer if
// the offer is inactive.
TEST_F(MediaSessionDescriptionFactoryTest, CreateAnswerToInactiveOffer) {
  TestMediaDirectionInAnswer(RtpTransceiverDirection::kInactive,
                             RtpTransceiverDirection::kInactive);
}

// Test that the media protocol is RTP/AVPF if DTLS and SDES are disabled.
TEST_F(MediaSessionDescriptionFactoryTest, AudioOfferAnswerWithCryptoDisabled) {
  MediaSessionOptions opts = CreatePlanBMediaSessionOptions();
  f1_.set_secure(SEC_DISABLED);
  f2_.set_secure(SEC_DISABLED);
  tdf1_.set_secure(SEC_DISABLED);
  tdf2_.set_secure(SEC_DISABLED);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  const AudioContentDescription* offer_acd =
      GetFirstAudioContentDescription(offer.get());
  ASSERT_TRUE(offer_acd != NULL);
  EXPECT_EQ(cricket::kMediaProtocolAvpf, offer_acd->protocol());

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  const ContentInfo* ac_answer = answer->GetContentByName("audio");
  ASSERT_TRUE(ac_answer != NULL);
  EXPECT_FALSE(ac_answer->rejected);

  const AudioContentDescription* answer_acd =
      GetFirstAudioContentDescription(answer.get());
  ASSERT_TRUE(answer_acd != NULL);
  EXPECT_EQ(cricket::kMediaProtocolAvpf, answer_acd->protocol());
}

// Create a video offer and answer and ensure the RTP header extensions
// matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestOfferAnswerWithRtpExtensions) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);
  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension1),
                                   MAKE_VECTOR(kVideoRtpExtension1), &opts);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension2),
                                   MAKE_VECTOR(kVideoRtpExtension2), &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtension1),
      GetFirstAudioContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtension1),
      GetFirstVideoContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtensionAnswer),
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtensionAnswer),
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions());
}

// Create a audio/video offer and answer and ensure that the
// TransportSequenceNumber RTP header extensions are handled correctly. 02 is
// supported and should take precedence even though not listed among locally
// supported extensions.
TEST_F(MediaSessionDescriptionFactoryTest,
       TestOfferAnswerWithTransportSequenceNumberInOffer) {
  TestTransportSequenceNumberNegotiation(
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber01),   // Local.
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber01),   // Offer.
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber01));  // Expected answer.
}
TEST_F(MediaSessionDescriptionFactoryTest,
       TestOfferAnswerWithTransportSequenceNumber01And02InOffer) {
  TestTransportSequenceNumberNegotiation(
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber01),       // Local.
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber01And02),  // Offer.
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber02));  // Expected answer.
}
TEST_F(MediaSessionDescriptionFactoryTest,
       TestOfferAnswerWithTransportSequenceNumber02InOffer) {
  TestTransportSequenceNumberNegotiation(
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber01),   // Local.
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber02),   // Offer.
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber02));  // Expected answer.
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestNegotiateFrameDescriptorWhenUnexposedLocally) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  SetAudioVideoRtpHeaderExtensions(
      MAKE_VECTOR(kRtpExtensionGenericFrameDescriptorUri00),
      MAKE_VECTOR(kRtpExtensionGenericFrameDescriptorUri00), &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  SetAudioVideoRtpHeaderExtensions(
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber01),
      MAKE_VECTOR(kRtpExtensionTransportSequenceNumber01), &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  EXPECT_THAT(
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAreArray(kRtpExtensionGenericFrameDescriptorUri00));
  EXPECT_THAT(
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAreArray(kRtpExtensionGenericFrameDescriptorUri00));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestNegotiateFrameDescriptorWhenExposedLocally) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  SetAudioVideoRtpHeaderExtensions(
      MAKE_VECTOR(kRtpExtensionGenericFrameDescriptorUri00),
      MAKE_VECTOR(kRtpExtensionGenericFrameDescriptorUri00), &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  EXPECT_THAT(
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAreArray(kRtpExtensionGenericFrameDescriptorUri00));
  EXPECT_THAT(
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAreArray(kRtpExtensionGenericFrameDescriptorUri00));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       NegotiateDependencyDescriptorWhenUnexposedLocally) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  RtpExtension offer_dd(RtpExtension::kDependencyDescriptorUri, 7);
  SetAudioVideoRtpHeaderExtensions({}, {offer_dd}, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  RtpExtension local_tsn(RtpExtension::kTransportSequenceNumberUri, 5);
  SetAudioVideoRtpHeaderExtensions({}, {local_tsn}, &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  EXPECT_THAT(
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAre(offer_dd));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       NegotiateDependencyDescriptorWhenExposedLocally) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  RtpExtension offer_dd(RtpExtension::kDependencyDescriptorUri, 7);
  RtpExtension local_dd(RtpExtension::kDependencyDescriptorUri, 5);
  SetAudioVideoRtpHeaderExtensions({}, {offer_dd}, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  SetAudioVideoRtpHeaderExtensions({}, {local_dd}, &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  EXPECT_THAT(
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAre(offer_dd));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       NegotiateAbsoluteCaptureTimeWhenUnexposedLocally) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  const cricket::RtpHeaderExtensions offered_extensions = {
      RtpExtension(RtpExtension::kAbsoluteCaptureTimeUri, 7)};
  const cricket::RtpHeaderExtensions local_extensions = {
      RtpExtension(RtpExtension::kTransportSequenceNumberUri, 5)};
  SetAudioVideoRtpHeaderExtensions(offered_extensions, offered_extensions,
                                   &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  SetAudioVideoRtpHeaderExtensions(local_extensions, local_extensions, &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  EXPECT_THAT(
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAreArray(offered_extensions));
  EXPECT_THAT(
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAreArray(offered_extensions));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       NegotiateAbsoluteCaptureTimeWhenExposedLocally) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  const cricket::RtpHeaderExtensions offered_extensions = {
      RtpExtension(RtpExtension::kAbsoluteCaptureTimeUri, 7)};
  const cricket::RtpHeaderExtensions local_extensions = {
      RtpExtension(RtpExtension::kAbsoluteCaptureTimeUri, 5)};
  SetAudioVideoRtpHeaderExtensions(offered_extensions, offered_extensions,
                                   &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  SetAudioVideoRtpHeaderExtensions(local_extensions, local_extensions, &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  EXPECT_THAT(
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAreArray(offered_extensions));
  EXPECT_THAT(
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions(),
      ElementsAreArray(offered_extensions));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       DoNotNegotiateAbsoluteCaptureTimeWhenNotOffered) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  const cricket::RtpHeaderExtensions offered_extensions = {
      RtpExtension(RtpExtension::kTransportSequenceNumberUri, 7)};
  const cricket::RtpHeaderExtensions local_extensions = {
      RtpExtension(RtpExtension::kAbsoluteCaptureTimeUri, 5)};
  SetAudioVideoRtpHeaderExtensions(offered_extensions, offered_extensions,
                                   &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  SetAudioVideoRtpHeaderExtensions(local_extensions, local_extensions, &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  EXPECT_THAT(
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions(),
      IsEmpty());
  EXPECT_THAT(
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions(),
      IsEmpty());
}

TEST_F(MediaSessionDescriptionFactoryTest,
       OffersUnstoppedExtensionsWithAudioVideoExtensionStopped) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 1,
                                           RtpTransceiverDirection::kStopped),
      webrtc::RtpHeaderExtensionCapability("uri2", 3,
                                           RtpTransceiverDirection::kSendOnly)};
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 1,
                                           RtpTransceiverDirection::kStopped),
      webrtc::RtpHeaderExtensionCapability("uri3", 7,
                                           RtpTransceiverDirection::kSendOnly)};
  auto offer = f1_.CreateOffer(opts, nullptr);
  EXPECT_THAT(
      offer->contents(),
      ElementsAre(
          Property(&ContentInfo::media_description,
                   Pointee(Property(
                       &MediaContentDescription::rtp_header_extensions,
                       ElementsAre(Field(&RtpExtension::uri, "uri2"))))),
          Property(&ContentInfo::media_description,
                   Pointee(Property(
                       &MediaContentDescription::rtp_header_extensions,
                       ElementsAre(Field(&RtpExtension::uri, "uri3")))))));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       OffersUnstoppedExtensionsWithAudioExtensionStopped) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 1,
                                           RtpTransceiverDirection::kSendOnly),
      webrtc::RtpHeaderExtensionCapability("uri2", 3,
                                           RtpTransceiverDirection::kStopped)};
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri42", 42,
                                           RtpTransceiverDirection::kSendRecv),
      webrtc::RtpHeaderExtensionCapability("uri3", 7,
                                           RtpTransceiverDirection::kSendOnly)};
  auto offer = f1_.CreateOffer(opts, nullptr);
  EXPECT_THAT(
      offer->contents(),
      ElementsAre(
          Property(&ContentInfo::media_description,
                   Pointee(Property(
                       &MediaContentDescription::rtp_header_extensions,
                       ElementsAre(Field(&RtpExtension::uri, "uri1"))))),
          Property(
              &ContentInfo::media_description,
              Pointee(Property(
                  &MediaContentDescription::rtp_header_extensions,
                  UnorderedElementsAre(Field(&RtpExtension::uri, "uri3"),
                                       Field(&RtpExtension::uri, "uri42")))))));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       OffersUnstoppedExtensionsWithVideoExtensionStopped) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 5,
                                           RtpTransceiverDirection::kSendOnly),
      webrtc::RtpHeaderExtensionCapability("uri2", 7,
                                           RtpTransceiverDirection::kSendRecv)};
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri42", 42,
                                           RtpTransceiverDirection::kSendRecv),
      webrtc::RtpHeaderExtensionCapability("uri3", 7,
                                           RtpTransceiverDirection::kStopped)};
  auto offer = f1_.CreateOffer(opts, nullptr);
  EXPECT_THAT(
      offer->contents(),
      ElementsAre(
          Property(
              &ContentInfo::media_description,
              Pointee(Property(
                  &MediaContentDescription::rtp_header_extensions,
                  UnorderedElementsAre(Field(&RtpExtension::uri, "uri1"),
                                       Field(&RtpExtension::uri, "uri2"))))),
          Property(&ContentInfo::media_description,
                   Pointee(Property(
                       &MediaContentDescription::rtp_header_extensions,
                       ElementsAre(Field(&RtpExtension::uri, "uri42")))))));
}

TEST_F(MediaSessionDescriptionFactoryTest, AnswersUnstoppedExtensions) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 4,
                                           RtpTransceiverDirection::kStopped),
      webrtc::RtpHeaderExtensionCapability("uri2", 3,
                                           RtpTransceiverDirection::kSendOnly),
      webrtc::RtpHeaderExtensionCapability("uri3", 2,
                                           RtpTransceiverDirection::kRecvOnly),
      webrtc::RtpHeaderExtensionCapability("uri4", 1,
                                           RtpTransceiverDirection::kSendRecv)};
  auto offer = f1_.CreateOffer(opts, nullptr);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 4,
                                           RtpTransceiverDirection::kSendOnly),
      webrtc::RtpHeaderExtensionCapability("uri2", 3,
                                           RtpTransceiverDirection::kRecvOnly),
      webrtc::RtpHeaderExtensionCapability("uri3", 2,
                                           RtpTransceiverDirection::kStopped),
      webrtc::RtpHeaderExtensionCapability("uri4", 1,
                                           RtpTransceiverDirection::kSendRecv)};
  auto answer = f2_.CreateAnswer(offer.get(), opts, nullptr);
  EXPECT_THAT(
      answer->contents(),
      ElementsAre(Property(
          &ContentInfo::media_description,
          Pointee(Property(&MediaContentDescription::rtp_header_extensions,
                           ElementsAre(Field(&RtpExtension::uri, "uri2"),
                                       Field(&RtpExtension::uri, "uri4")))))));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       AppendsUnstoppedExtensionsToCurrentDescription) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 1,
                                           RtpTransceiverDirection::kSendRecv)};
  auto offer = f1_.CreateOffer(opts, nullptr);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 2,
                                           RtpTransceiverDirection::kSendRecv),
      webrtc::RtpHeaderExtensionCapability("uri2", 3,
                                           RtpTransceiverDirection::kRecvOnly),
      webrtc::RtpHeaderExtensionCapability("uri3", 5,
                                           RtpTransceiverDirection::kStopped),
      webrtc::RtpHeaderExtensionCapability("uri4", 6,
                                           RtpTransceiverDirection::kSendRecv)};
  auto offer2 = f1_.CreateOffer(opts, offer.get());
  EXPECT_THAT(
      offer2->contents(),
      ElementsAre(Property(
          &ContentInfo::media_description,
          Pointee(Property(&MediaContentDescription::rtp_header_extensions,
                           ElementsAre(Field(&RtpExtension::uri, "uri1"),
                                       Field(&RtpExtension::uri, "uri2"),
                                       Field(&RtpExtension::uri, "uri4")))))));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       AppendsStoppedExtensionIfKnownAndPresentInTheOffer) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 1,
                                           RtpTransceiverDirection::kSendRecv),
      webrtc::RtpHeaderExtensionCapability("uri2", 1,
                                           RtpTransceiverDirection::kSendRecv)};
  auto offer = f1_.CreateOffer(opts, nullptr);

  // Now add "uri2" as stopped to the options verify that the offer contains
  // uri2 since it's already present since before.
  opts.media_description_options.back().header_extensions = {
      webrtc::RtpHeaderExtensionCapability("uri1", 1,
                                           RtpTransceiverDirection::kSendRecv),
      webrtc::RtpHeaderExtensionCapability("uri2", 2,
                                           RtpTransceiverDirection::kStopped)};
  auto offer2 = f1_.CreateOffer(opts, offer.get());
  EXPECT_THAT(
      offer2->contents(),
      ElementsAre(Property(
          &ContentInfo::media_description,
          Pointee(Property(&MediaContentDescription::rtp_header_extensions,
                           ElementsAre(Field(&RtpExtension::uri, "uri1"),
                                       Field(&RtpExtension::uri, "uri2")))))));
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestOfferAnswerWithEncryptedRtpExtensionsBoth) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  f1_.set_enable_encrypted_rtp_header_extensions(true);
  f2_.set_enable_encrypted_rtp_header_extensions(true);

  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension1),
                                   MAKE_VECTOR(kVideoRtpExtension1), &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension2),
                                   MAKE_VECTOR(kVideoRtpExtension2), &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtensionEncrypted1),
      GetFirstAudioContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtensionEncrypted1),
      GetFirstVideoContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtensionEncryptedAnswer),
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtensionEncryptedAnswer),
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions());
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestOfferAnswerWithEncryptedRtpExtensionsOffer) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  f1_.set_enable_encrypted_rtp_header_extensions(true);

  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension1),
                                   MAKE_VECTOR(kVideoRtpExtension1), &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension2),
                                   MAKE_VECTOR(kVideoRtpExtension2), &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtensionEncrypted1),
      GetFirstAudioContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtensionEncrypted1),
      GetFirstVideoContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtensionAnswer),
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtensionAnswer),
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions());
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestOfferAnswerWithEncryptedRtpExtensionsAnswer) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  f2_.set_enable_encrypted_rtp_header_extensions(true);

  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension1),
                                   MAKE_VECTOR(kVideoRtpExtension1), &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension2),
                                   MAKE_VECTOR(kVideoRtpExtension2), &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtension1),
      GetFirstAudioContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtension1),
      GetFirstVideoContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtensionAnswer),
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtensionAnswer),
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions());
}

// Create an audio, video, data answer without legacy StreamParams.
TEST_F(MediaSessionDescriptionFactoryTest,
       TestCreateAnswerWithoutLegacyStreams) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);
  const ContentInfo* ac = answer->GetContentByName("audio");
  const ContentInfo* vc = answer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  const VideoContentDescription* vcd = vc->media_description()->as_video();

  EXPECT_FALSE(acd->has_ssrcs());  // No StreamParams.
  EXPECT_FALSE(vcd->has_ssrcs());  // No StreamParams.
}

// Create a typical video answer, and ensure it matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateVideoAnswerRtcpMux) {
  MediaSessionOptions offer_opts;
  AddAudioVideoSections(RtpTransceiverDirection::kSendRecv, &offer_opts);

  MediaSessionOptions answer_opts;
  AddAudioVideoSections(RtpTransceiverDirection::kSendRecv, &answer_opts);

  std::unique_ptr<SessionDescription> offer;
  std::unique_ptr<SessionDescription> answer;

  offer_opts.rtcp_mux_enabled = true;
  answer_opts.rtcp_mux_enabled = true;
  offer = f1_.CreateOffer(offer_opts, NULL);
  answer = f2_.CreateAnswer(offer.get(), answer_opts, NULL);
  ASSERT_TRUE(NULL != GetFirstAudioContentDescription(offer.get()));
  ASSERT_TRUE(NULL != GetFirstVideoContentDescription(offer.get()));
  ASSERT_TRUE(NULL != GetFirstAudioContentDescription(answer.get()));
  ASSERT_TRUE(NULL != GetFirstVideoContentDescription(answer.get()));
  EXPECT_TRUE(GetFirstAudioContentDescription(offer.get())->rtcp_mux());
  EXPECT_TRUE(GetFirstVideoContentDescription(offer.get())->rtcp_mux());
  EXPECT_TRUE(GetFirstAudioContentDescription(answer.get())->rtcp_mux());
  EXPECT_TRUE(GetFirstVideoContentDescription(answer.get())->rtcp_mux());

  offer_opts.rtcp_mux_enabled = true;
  answer_opts.rtcp_mux_enabled = false;
  offer = f1_.CreateOffer(offer_opts, NULL);
  answer = f2_.CreateAnswer(offer.get(), answer_opts, NULL);
  ASSERT_TRUE(NULL != GetFirstAudioContentDescription(offer.get()));
  ASSERT_TRUE(NULL != GetFirstVideoContentDescription(offer.get()));
  ASSERT_TRUE(NULL != GetFirstAudioContentDescription(answer.get()));
  ASSERT_TRUE(NULL != GetFirstVideoContentDescription(answer.get()));
  EXPECT_TRUE(GetFirstAudioContentDescription(offer.get())->rtcp_mux());
  EXPECT_TRUE(GetFirstVideoContentDescription(offer.get())->rtcp_mux());
  EXPECT_FALSE(GetFirstAudioContentDescription(answer.get())->rtcp_mux());
  EXPECT_FALSE(GetFirstVideoContentDescription(answer.get())->rtcp_mux());

  offer_opts.rtcp_mux_enabled = false;
  answer_opts.rtcp_mux_enabled = true;
  offer = f1_.CreateOffer(offer_opts, NULL);
  answer = f2_.CreateAnswer(offer.get(), answer_opts, NULL);
  ASSERT_TRUE(NULL != GetFirstAudioContentDescription(offer.get()));
  ASSERT_TRUE(NULL != GetFirstVideoContentDescription(offer.get()));
  ASSERT_TRUE(NULL != GetFirstAudioContentDescription(answer.get()));
  ASSERT_TRUE(NULL != GetFirstVideoContentDescription(answer.get()));
  EXPECT_FALSE(GetFirstAudioContentDescription(offer.get())->rtcp_mux());
  EXPECT_FALSE(GetFirstVideoContentDescription(offer.get())->rtcp_mux());
  EXPECT_FALSE(GetFirstAudioContentDescription(answer.get())->rtcp_mux());
  EXPECT_FALSE(GetFirstVideoContentDescription(answer.get())->rtcp_mux());

  offer_opts.rtcp_mux_enabled = false;
  answer_opts.rtcp_mux_enabled = false;
  offer = f1_.CreateOffer(offer_opts, NULL);
  answer = f2_.CreateAnswer(offer.get(), answer_opts, NULL);
  ASSERT_TRUE(NULL != GetFirstAudioContentDescription(offer.get()));
  ASSERT_TRUE(NULL != GetFirstVideoContentDescription(offer.get()));
  ASSERT_TRUE(NULL != GetFirstAudioContentDescription(answer.get()));
  ASSERT_TRUE(NULL != GetFirstVideoContentDescription(answer.get()));
  EXPECT_FALSE(GetFirstAudioContentDescription(offer.get())->rtcp_mux());
  EXPECT_FALSE(GetFirstVideoContentDescription(offer.get())->rtcp_mux());
  EXPECT_FALSE(GetFirstAudioContentDescription(answer.get())->rtcp_mux());
  EXPECT_FALSE(GetFirstVideoContentDescription(answer.get())->rtcp_mux());
}

// Create an audio-only answer to a video offer.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateAudioAnswerToVideo) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);

  opts.media_description_options[1].stopped = true;
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);
  const ContentInfo* ac = answer->GetContentByName("audio");
  const ContentInfo* vc = answer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  ASSERT_TRUE(vc->media_description() != NULL);
  EXPECT_TRUE(vc->rejected);
}

// Create an answer that rejects the contents which are rejected in the offer.
TEST_F(MediaSessionDescriptionFactoryTest,
       CreateAnswerToOfferWithRejectedMedia) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  ContentInfo* ac = offer->GetContentByName("audio");
  ContentInfo* vc = offer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  ac->rejected = true;
  vc->rejected = true;
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);
  ac = answer->GetContentByName("audio");
  vc = answer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  EXPECT_TRUE(ac->rejected);
  EXPECT_TRUE(vc->rejected);
}

TEST_F(MediaSessionDescriptionFactoryTest,
       OfferAndAnswerDoesNotHaveMixedByteSessionAttribute) {
  MediaSessionOptions opts;
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(opts, /*current_description=*/nullptr);
  offer->set_extmap_allow_mixed(false);

  std::unique_ptr<SessionDescription> answer(
      f2_.CreateAnswer(offer.get(), opts, /*current_description=*/nullptr));

  EXPECT_FALSE(answer->extmap_allow_mixed());
}

TEST_F(MediaSessionDescriptionFactoryTest,
       OfferAndAnswerHaveMixedByteSessionAttribute) {
  MediaSessionOptions opts;
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(opts, /*current_description=*/nullptr);
  offer->set_extmap_allow_mixed(true);

  std::unique_ptr<SessionDescription> answer_support(
      f2_.CreateAnswer(offer.get(), opts, /*current_description=*/nullptr));

  EXPECT_TRUE(answer_support->extmap_allow_mixed());
}

TEST_F(MediaSessionDescriptionFactoryTest,
       OfferAndAnswerDoesNotHaveMixedByteMediaAttributes) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(opts, /*current_description=*/nullptr);
  offer->set_extmap_allow_mixed(false);
  MediaContentDescription* audio_offer =
      offer->GetContentDescriptionByName("audio");
  MediaContentDescription* video_offer =
      offer->GetContentDescriptionByName("video");
  ASSERT_EQ(MediaContentDescription::kNo,
            audio_offer->extmap_allow_mixed_enum());
  ASSERT_EQ(MediaContentDescription::kNo,
            video_offer->extmap_allow_mixed_enum());

  std::unique_ptr<SessionDescription> answer(
      f2_.CreateAnswer(offer.get(), opts, /*current_description=*/nullptr));

  MediaContentDescription* audio_answer =
      answer->GetContentDescriptionByName("audio");
  MediaContentDescription* video_answer =
      answer->GetContentDescriptionByName("video");
  EXPECT_EQ(MediaContentDescription::kNo,
            audio_answer->extmap_allow_mixed_enum());
  EXPECT_EQ(MediaContentDescription::kNo,
            video_answer->extmap_allow_mixed_enum());
}

TEST_F(MediaSessionDescriptionFactoryTest,
       OfferAndAnswerHaveSameMixedByteMediaAttributes) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(opts, /*current_description=*/nullptr);
  offer->set_extmap_allow_mixed(false);
  MediaContentDescription* audio_offer =
      offer->GetContentDescriptionByName("audio");
  audio_offer->set_extmap_allow_mixed_enum(MediaContentDescription::kMedia);
  MediaContentDescription* video_offer =
      offer->GetContentDescriptionByName("video");
  video_offer->set_extmap_allow_mixed_enum(MediaContentDescription::kMedia);

  std::unique_ptr<SessionDescription> answer(
      f2_.CreateAnswer(offer.get(), opts, /*current_description=*/nullptr));

  MediaContentDescription* audio_answer =
      answer->GetContentDescriptionByName("audio");
  MediaContentDescription* video_answer =
      answer->GetContentDescriptionByName("video");
  EXPECT_EQ(MediaContentDescription::kMedia,
            audio_answer->extmap_allow_mixed_enum());
  EXPECT_EQ(MediaContentDescription::kMedia,
            video_answer->extmap_allow_mixed_enum());
}

TEST_F(MediaSessionDescriptionFactoryTest,
       OfferAndAnswerHaveDifferentMixedByteMediaAttributes) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kSendRecv, &opts);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(opts, /*current_description=*/nullptr);
  offer->set_extmap_allow_mixed(false);
  MediaContentDescription* audio_offer =
      offer->GetContentDescriptionByName("audio");
  audio_offer->set_extmap_allow_mixed_enum(MediaContentDescription::kNo);
  MediaContentDescription* video_offer =
      offer->GetContentDescriptionByName("video");
  video_offer->set_extmap_allow_mixed_enum(MediaContentDescription::kMedia);

  std::unique_ptr<SessionDescription> answer(
      f2_.CreateAnswer(offer.get(), opts, /*current_description=*/nullptr));

  MediaContentDescription* audio_answer =
      answer->GetContentDescriptionByName("audio");
  MediaContentDescription* video_answer =
      answer->GetContentDescriptionByName("video");
  EXPECT_EQ(MediaContentDescription::kNo,
            audio_answer->extmap_allow_mixed_enum());
  EXPECT_EQ(MediaContentDescription::kMedia,
            video_answer->extmap_allow_mixed_enum());
}

// Create an audio and video offer with:
// - one video track
// - two audio tracks
// and ensure it matches what we expect. Also updates the initial offer by
// adding a new video track and replaces one of the audio tracks.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateMultiStreamVideoOffer) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kSendRecv, &opts);
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, 1, &opts);
  AttachSenderToMediaDescriptionOptions("audio", MEDIA_TYPE_AUDIO, kAudioTrack1,
                                        {kMediaStream1}, 1, &opts);
  AttachSenderToMediaDescriptionOptions("audio", MEDIA_TYPE_AUDIO, kAudioTrack2,
                                        {kMediaStream1}, 1, &opts);

  f1_.set_secure(SEC_ENABLED);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);

  ASSERT_TRUE(offer.get() != NULL);
  const ContentInfo* ac = offer->GetContentByName("audio");
  const ContentInfo* vc = offer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  const VideoContentDescription* vcd = vc->media_description()->as_video();
  EXPECT_EQ(MEDIA_TYPE_AUDIO, acd->type());
  EXPECT_EQ(f1_.audio_sendrecv_codecs(), acd->codecs());

  const StreamParamsVec& audio_streams = acd->streams();
  ASSERT_EQ(2U, audio_streams.size());
  EXPECT_EQ(audio_streams[0].cname, audio_streams[1].cname);
  EXPECT_EQ(kAudioTrack1, audio_streams[0].id);
  ASSERT_EQ(1U, audio_streams[0].ssrcs.size());
  EXPECT_NE(0U, audio_streams[0].ssrcs[0]);
  EXPECT_EQ(kAudioTrack2, audio_streams[1].id);
  ASSERT_EQ(1U, audio_streams[1].ssrcs.size());
  EXPECT_NE(0U, audio_streams[1].ssrcs[0]);

  EXPECT_EQ(kAutoBandwidth, acd->bandwidth());  // default bandwidth (auto)
  EXPECT_TRUE(acd->rtcp_mux());                 // rtcp-mux defaults on
  ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuite);

  EXPECT_EQ(MEDIA_TYPE_VIDEO, vcd->type());
  EXPECT_EQ(f1_.video_sendrecv_codecs(), vcd->codecs());
  ASSERT_CRYPTO(vcd, 1U, kDefaultSrtpCryptoSuite);

  const StreamParamsVec& video_streams = vcd->streams();
  ASSERT_EQ(1U, video_streams.size());
  EXPECT_EQ(video_streams[0].cname, audio_streams[0].cname);
  EXPECT_EQ(kVideoTrack1, video_streams[0].id);
  EXPECT_EQ(kAutoBandwidth, vcd->bandwidth());  // default bandwidth (auto)
  EXPECT_TRUE(vcd->rtcp_mux());                 // rtcp-mux defaults on

  // Update the offer. Add a new video track that is not synched to the
  // other tracks and replace audio track 2 with audio track 3.
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack2,
                                        {kMediaStream2}, 1, &opts);
  DetachSenderFromMediaSection("audio", kAudioTrack2, &opts);
  AttachSenderToMediaDescriptionOptions("audio", MEDIA_TYPE_AUDIO, kAudioTrack3,
                                        {kMediaStream1}, 1, &opts);
  std::unique_ptr<SessionDescription> updated_offer(
      f1_.CreateOffer(opts, offer.get()));

  ASSERT_TRUE(updated_offer.get() != NULL);
  ac = updated_offer->GetContentByName("audio");
  vc = updated_offer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  const AudioContentDescription* updated_acd =
      ac->media_description()->as_audio();
  const VideoContentDescription* updated_vcd =
      vc->media_description()->as_video();

  EXPECT_EQ(acd->type(), updated_acd->type());
  EXPECT_EQ(acd->codecs(), updated_acd->codecs());
  EXPECT_EQ(vcd->type(), updated_vcd->type());
  EXPECT_EQ(vcd->codecs(), updated_vcd->codecs());
  ASSERT_CRYPTO(updated_acd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_TRUE(CompareCryptoParams(acd->cryptos(), updated_acd->cryptos()));
  ASSERT_CRYPTO(updated_vcd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_TRUE(CompareCryptoParams(vcd->cryptos(), updated_vcd->cryptos()));

  const StreamParamsVec& updated_audio_streams = updated_acd->streams();
  ASSERT_EQ(2U, updated_audio_streams.size());
  EXPECT_EQ(audio_streams[0], updated_audio_streams[0]);
  EXPECT_EQ(kAudioTrack3, updated_audio_streams[1].id);  // New audio track.
  ASSERT_EQ(1U, updated_audio_streams[1].ssrcs.size());
  EXPECT_NE(0U, updated_audio_streams[1].ssrcs[0]);
  EXPECT_EQ(updated_audio_streams[0].cname, updated_audio_streams[1].cname);

  const StreamParamsVec& updated_video_streams = updated_vcd->streams();
  ASSERT_EQ(2U, updated_video_streams.size());
  EXPECT_EQ(video_streams[0], updated_video_streams[0]);
  EXPECT_EQ(kVideoTrack2, updated_video_streams[1].id);
  // All the media streams in one PeerConnection share one RTCP CNAME.
  EXPECT_EQ(updated_video_streams[1].cname, updated_video_streams[0].cname);
}

// Create an offer with simulcast video stream.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateSimulcastVideoOffer) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  const int num_sim_layers = 3;
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, num_sim_layers, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);

  ASSERT_TRUE(offer.get() != NULL);
  const ContentInfo* vc = offer->GetContentByName("video");
  ASSERT_TRUE(vc != NULL);
  const VideoContentDescription* vcd = vc->media_description()->as_video();

  const StreamParamsVec& video_streams = vcd->streams();
  ASSERT_EQ(1U, video_streams.size());
  EXPECT_EQ(kVideoTrack1, video_streams[0].id);
  const SsrcGroup* sim_ssrc_group =
      video_streams[0].get_ssrc_group(cricket::kSimSsrcGroupSemantics);
  ASSERT_TRUE(sim_ssrc_group != NULL);
  EXPECT_EQ(static_cast<size_t>(num_sim_layers), sim_ssrc_group->ssrcs.size());
}

MATCHER(RidDescriptionEquals, "Verifies that two RidDescriptions are equal.") {
  const RidDescription& rid1 = ::testing::get<0>(arg);
  const RidDescription& rid2 = ::testing::get<1>(arg);
  return rid1.rid == rid2.rid && rid1.direction == rid2.direction;
}

static void CheckSimulcastInSessionDescription(
    const SessionDescription* description,
    const std::string& content_name,
    const std::vector<RidDescription>& send_rids,
    const SimulcastLayerList& send_layers) {
  ASSERT_NE(description, nullptr);
  const ContentInfo* content = description->GetContentByName(content_name);
  ASSERT_NE(content, nullptr);
  const MediaContentDescription* cd = content->media_description();
  ASSERT_NE(cd, nullptr);
  const StreamParamsVec& streams = cd->streams();
  ASSERT_THAT(streams, SizeIs(1));
  const StreamParams& stream = streams[0];
  ASSERT_THAT(stream.ssrcs, IsEmpty());
  EXPECT_TRUE(stream.has_rids());
  const std::vector<RidDescription> rids = stream.rids();

  EXPECT_THAT(rids, Pointwise(RidDescriptionEquals(), send_rids));

  EXPECT_TRUE(cd->HasSimulcast());
  const SimulcastDescription& simulcast = cd->simulcast_description();
  EXPECT_THAT(simulcast.send_layers(), SizeIs(send_layers.size()));
  EXPECT_THAT(simulcast.send_layers(), Pointwise(Eq(), send_layers));

  ASSERT_THAT(simulcast.receive_layers().GetAllLayers(), SizeIs(0));
}

// Create an offer with spec-compliant simulcast video stream.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateCompliantSimulcastOffer) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::vector<RidDescription> send_rids;
  send_rids.push_back(RidDescription("f", RidDirection::kSend));
  send_rids.push_back(RidDescription("h", RidDirection::kSend));
  send_rids.push_back(RidDescription("q", RidDirection::kSend));
  SimulcastLayerList simulcast_layers;
  simulcast_layers.AddLayer(SimulcastLayer(send_rids[0].rid, false));
  simulcast_layers.AddLayer(SimulcastLayer(send_rids[1].rid, true));
  simulcast_layers.AddLayer(SimulcastLayer(send_rids[2].rid, false));
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, send_rids,
                                        simulcast_layers, 0, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);

  CheckSimulcastInSessionDescription(offer.get(), "video", send_rids,
                                     simulcast_layers);
}

// Create an offer that signals RIDs (not SSRCs) without Simulcast.
// In this scenario, RIDs do not need to be negotiated (there is only one).
TEST_F(MediaSessionDescriptionFactoryTest, TestOfferWithRidsNoSimulcast) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  RidDescription rid("f", RidDirection::kSend);
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, {rid},
                                        SimulcastLayerList(), 0, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);

  ASSERT_NE(offer.get(), nullptr);
  const ContentInfo* content = offer->GetContentByName("video");
  ASSERT_NE(content, nullptr);
  const MediaContentDescription* cd = content->media_description();
  ASSERT_NE(cd, nullptr);
  const StreamParamsVec& streams = cd->streams();
  ASSERT_THAT(streams, SizeIs(1));
  const StreamParams& stream = streams[0];
  ASSERT_THAT(stream.ssrcs, IsEmpty());
  EXPECT_FALSE(stream.has_rids());
  EXPECT_FALSE(cd->HasSimulcast());
}

// Create an answer with spec-compliant simulcast video stream.
// In this scenario, the SFU is the caller requesting that we send Simulcast.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateCompliantSimulcastAnswer) {
  MediaSessionOptions offer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &offer_opts);
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, 1, &offer_opts);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(offer_opts, nullptr);

  MediaSessionOptions answer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &answer_opts);

  std::vector<RidDescription> rid_descriptions{
      RidDescription("f", RidDirection::kSend),
      RidDescription("h", RidDirection::kSend),
      RidDescription("q", RidDirection::kSend),
  };
  SimulcastLayerList simulcast_layers;
  simulcast_layers.AddLayer(SimulcastLayer(rid_descriptions[0].rid, false));
  simulcast_layers.AddLayer(SimulcastLayer(rid_descriptions[1].rid, true));
  simulcast_layers.AddLayer(SimulcastLayer(rid_descriptions[2].rid, false));
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, rid_descriptions,
                                        simulcast_layers, 0, &answer_opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), answer_opts, nullptr);

  CheckSimulcastInSessionDescription(answer.get(), "video", rid_descriptions,
                                     simulcast_layers);
}

// Create an answer that signals RIDs (not SSRCs) without Simulcast.
// In this scenario, RIDs do not need to be negotiated (there is only one).
// Note that RID Direction is not the same as the transceiver direction.
TEST_F(MediaSessionDescriptionFactoryTest, TestAnswerWithRidsNoSimulcast) {
  MediaSessionOptions offer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &offer_opts);
  RidDescription rid_offer("f", RidDirection::kSend);
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, {rid_offer},
                                        SimulcastLayerList(), 0, &offer_opts);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(offer_opts, nullptr);

  MediaSessionOptions answer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &answer_opts);

  RidDescription rid_answer("f", RidDirection::kReceive);
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, {rid_answer},
                                        SimulcastLayerList(), 0, &answer_opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), answer_opts, nullptr);

  ASSERT_NE(answer.get(), nullptr);
  const ContentInfo* content = offer->GetContentByName("video");
  ASSERT_NE(content, nullptr);
  const MediaContentDescription* cd = content->media_description();
  ASSERT_NE(cd, nullptr);
  const StreamParamsVec& streams = cd->streams();
  ASSERT_THAT(streams, SizeIs(1));
  const StreamParams& stream = streams[0];
  ASSERT_THAT(stream.ssrcs, IsEmpty());
  EXPECT_FALSE(stream.has_rids());
  EXPECT_FALSE(cd->HasSimulcast());
}

// Create an audio and video answer to a standard video offer with:
// - one video track
// - two audio tracks
// - two data tracks
// and ensure it matches what we expect. Also updates the initial answer by
// adding a new video track and removes one of the audio tracks.
TEST_F(MediaSessionDescriptionFactoryTest, TestCreateMultiStreamVideoAnswer) {
  MediaSessionOptions offer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &offer_opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &offer_opts);
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(offer_opts, NULL);

  MediaSessionOptions answer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &answer_opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &answer_opts);
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack1,
                                        {kMediaStream1}, 1, &answer_opts);
  AttachSenderToMediaDescriptionOptions("audio", MEDIA_TYPE_AUDIO, kAudioTrack1,
                                        {kMediaStream1}, 1, &answer_opts);
  AttachSenderToMediaDescriptionOptions("audio", MEDIA_TYPE_AUDIO, kAudioTrack2,
                                        {kMediaStream1}, 1, &answer_opts);

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), answer_opts, NULL);

  ASSERT_TRUE(answer.get() != NULL);
  const ContentInfo* ac = answer->GetContentByName("audio");
  const ContentInfo* vc = answer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  const VideoContentDescription* vcd = vc->media_description()->as_video();
  ASSERT_CRYPTO(acd, 1U, kDefaultSrtpCryptoSuite);
  ASSERT_CRYPTO(vcd, 1U, kDefaultSrtpCryptoSuite);

  EXPECT_EQ(MEDIA_TYPE_AUDIO, acd->type());
  EXPECT_THAT(acd->codecs(), ElementsAreArray(kAudioCodecsAnswer));

  const StreamParamsVec& audio_streams = acd->streams();
  ASSERT_EQ(2U, audio_streams.size());
  EXPECT_TRUE(audio_streams[0].cname == audio_streams[1].cname);
  EXPECT_EQ(kAudioTrack1, audio_streams[0].id);
  ASSERT_EQ(1U, audio_streams[0].ssrcs.size());
  EXPECT_NE(0U, audio_streams[0].ssrcs[0]);
  EXPECT_EQ(kAudioTrack2, audio_streams[1].id);
  ASSERT_EQ(1U, audio_streams[1].ssrcs.size());
  EXPECT_NE(0U, audio_streams[1].ssrcs[0]);

  EXPECT_EQ(kAutoBandwidth, acd->bandwidth());  // default bandwidth (auto)
  EXPECT_TRUE(acd->rtcp_mux());                 // rtcp-mux defaults on

  EXPECT_EQ(MEDIA_TYPE_VIDEO, vcd->type());
  EXPECT_THAT(vcd->codecs(), ElementsAreArray(kVideoCodecsAnswer));

  const StreamParamsVec& video_streams = vcd->streams();
  ASSERT_EQ(1U, video_streams.size());
  EXPECT_EQ(video_streams[0].cname, audio_streams[0].cname);
  EXPECT_EQ(kVideoTrack1, video_streams[0].id);
  EXPECT_EQ(kAutoBandwidth, vcd->bandwidth());  // default bandwidth (auto)
  EXPECT_TRUE(vcd->rtcp_mux());                 // rtcp-mux defaults on

  // Update the answer. Add a new video track that is not synched to the
  // other tracks and remove 1 audio track.
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, kVideoTrack2,
                                        {kMediaStream2}, 1, &answer_opts);
  DetachSenderFromMediaSection("audio", kAudioTrack2, &answer_opts);
  std::unique_ptr<SessionDescription> updated_answer(
      f2_.CreateAnswer(offer.get(), answer_opts, answer.get()));

  ASSERT_TRUE(updated_answer.get() != NULL);
  ac = updated_answer->GetContentByName("audio");
  vc = updated_answer->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  const AudioContentDescription* updated_acd =
      ac->media_description()->as_audio();
  const VideoContentDescription* updated_vcd =
      vc->media_description()->as_video();

  ASSERT_CRYPTO(updated_acd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_TRUE(CompareCryptoParams(acd->cryptos(), updated_acd->cryptos()));
  ASSERT_CRYPTO(updated_vcd, 1U, kDefaultSrtpCryptoSuite);
  EXPECT_TRUE(CompareCryptoParams(vcd->cryptos(), updated_vcd->cryptos()));

  EXPECT_EQ(acd->type(), updated_acd->type());
  EXPECT_EQ(acd->codecs(), updated_acd->codecs());
  EXPECT_EQ(vcd->type(), updated_vcd->type());
  EXPECT_EQ(vcd->codecs(), updated_vcd->codecs());

  const StreamParamsVec& updated_audio_streams = updated_acd->streams();
  ASSERT_EQ(1U, updated_audio_streams.size());
  EXPECT_TRUE(audio_streams[0] == updated_audio_streams[0]);

  const StreamParamsVec& updated_video_streams = updated_vcd->streams();
  ASSERT_EQ(2U, updated_video_streams.size());
  EXPECT_EQ(video_streams[0], updated_video_streams[0]);
  EXPECT_EQ(kVideoTrack2, updated_video_streams[1].id);
  // All media streams in one PeerConnection share one CNAME.
  EXPECT_EQ(updated_video_streams[1].cname, updated_video_streams[0].cname);
}

// Create an updated offer after creating an answer to the original offer and
// verify that the codecs that were part of the original answer are not changed
// in the updated offer.
TEST_F(MediaSessionDescriptionFactoryTest,
       RespondentCreatesOfferAfterCreatingAnswer) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  const AudioContentDescription* acd =
      GetFirstAudioContentDescription(answer.get());
  EXPECT_THAT(acd->codecs(), ElementsAreArray(kAudioCodecsAnswer));

  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(answer.get());
  EXPECT_THAT(vcd->codecs(), ElementsAreArray(kVideoCodecsAnswer));

  std::unique_ptr<SessionDescription> updated_offer(
      f2_.CreateOffer(opts, answer.get()));

  // The expected audio codecs are the common audio codecs from the first
  // offer/answer exchange plus the audio codecs only |f2_| offer, sorted in
  // preference order.
  // TODO(wu): |updated_offer| should not include the codec
  // (i.e. |kAudioCodecs2[0]|) the other side doesn't support.
  const AudioCodec kUpdatedAudioCodecOffer[] = {
      kAudioCodecsAnswer[0],
      kAudioCodecsAnswer[1],
      kAudioCodecs2[0],
  };

  // The expected video codecs are the common video codecs from the first
  // offer/answer exchange plus the video codecs only |f2_| offer, sorted in
  // preference order.
  const VideoCodec kUpdatedVideoCodecOffer[] = {
      kVideoCodecsAnswer[0],
      kVideoCodecs2[1],
  };

  const AudioContentDescription* updated_acd =
      GetFirstAudioContentDescription(updated_offer.get());
  EXPECT_THAT(updated_acd->codecs(), ElementsAreArray(kUpdatedAudioCodecOffer));

  const VideoContentDescription* updated_vcd =
      GetFirstVideoContentDescription(updated_offer.get());
  EXPECT_THAT(updated_vcd->codecs(), ElementsAreArray(kUpdatedVideoCodecOffer));
}

// Test that a reoffer does not reuse audio codecs from a previous media section
// that is being recycled.
TEST_F(MediaSessionDescriptionFactoryTest,
       ReOfferDoesNotReUseRecycledAudioCodecs) {
  f1_.set_video_codecs({}, {});
  f2_.set_video_codecs({}, {});

  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "a0",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);

  // Recycle the media section by changing its mid.
  opts.media_description_options[0].mid = "a1";
  std::unique_ptr<SessionDescription> reoffer =
      f2_.CreateOffer(opts, answer.get());

  // Expect that the results of the first negotiation are ignored. If the m=
  // section was not recycled the payload types would match the initial offerer.
  const AudioContentDescription* acd =
      GetFirstAudioContentDescription(reoffer.get());
  EXPECT_THAT(acd->codecs(), ElementsAreArray(kAudioCodecs2));
}

// Test that a reoffer does not reuse video codecs from a previous media section
// that is being recycled.
TEST_F(MediaSessionDescriptionFactoryTest,
       ReOfferDoesNotReUseRecycledVideoCodecs) {
  f1_.set_audio_codecs({}, {});
  f2_.set_audio_codecs({}, {});

  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "v0",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  auto answer = f2_.CreateAnswer(offer.get(), opts, nullptr);

  // Recycle the media section by changing its mid.
  opts.media_description_options[0].mid = "v1";
  std::unique_ptr<SessionDescription> reoffer =
      f2_.CreateOffer(opts, answer.get());

  // Expect that the results of the first negotiation are ignored. If the m=
  // section was not recycled the payload types would match the initial offerer.
  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(reoffer.get());
  EXPECT_THAT(vcd->codecs(), ElementsAreArray(kVideoCodecs2));
}

// Test that a reanswer does not reuse audio codecs from a previous media
// section that is being recycled.
TEST_F(MediaSessionDescriptionFactoryTest,
       ReAnswerDoesNotReUseRecycledAudioCodecs) {
  f1_.set_video_codecs({}, {});
  f2_.set_video_codecs({}, {});

  // Perform initial offer/answer in reverse (|f2_| as offerer) so that the
  // second offer/answer is forward (|f1_| as offerer).
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "a0",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f2_.CreateOffer(opts, nullptr);
  std::unique_ptr<SessionDescription> answer =
      f1_.CreateAnswer(offer.get(), opts, nullptr);

  // Recycle the media section by changing its mid.
  opts.media_description_options[0].mid = "a1";
  std::unique_ptr<SessionDescription> reoffer =
      f1_.CreateOffer(opts, answer.get());
  std::unique_ptr<SessionDescription> reanswer =
      f2_.CreateAnswer(reoffer.get(), opts, offer.get());

  // Expect that the results of the first negotiation are ignored. If the m=
  // section was not recycled the payload types would match the initial offerer.
  const AudioContentDescription* acd =
      GetFirstAudioContentDescription(reanswer.get());
  EXPECT_THAT(acd->codecs(), ElementsAreArray(kAudioCodecsAnswer));
}

// Test that a reanswer does not reuse video codecs from a previous media
// section that is being recycled.
TEST_F(MediaSessionDescriptionFactoryTest,
       ReAnswerDoesNotReUseRecycledVideoCodecs) {
  f1_.set_audio_codecs({}, {});
  f2_.set_audio_codecs({}, {});

  // Perform initial offer/answer in reverse (|f2_| as offerer) so that the
  // second offer/answer is forward (|f1_| as offerer).
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "v0",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f2_.CreateOffer(opts, nullptr);
  std::unique_ptr<SessionDescription> answer =
      f1_.CreateAnswer(offer.get(), opts, nullptr);

  // Recycle the media section by changing its mid.
  opts.media_description_options[0].mid = "v1";
  std::unique_ptr<SessionDescription> reoffer =
      f1_.CreateOffer(opts, answer.get());
  std::unique_ptr<SessionDescription> reanswer =
      f2_.CreateAnswer(reoffer.get(), opts, offer.get());

  // Expect that the results of the first negotiation are ignored. If the m=
  // section was not recycled the payload types would match the initial offerer.
  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(reanswer.get());
  EXPECT_THAT(vcd->codecs(), ElementsAreArray(kVideoCodecsAnswer));
}

// Create an updated offer after creating an answer to the original offer and
// verify that the codecs that were part of the original answer are not changed
// in the updated offer. In this test Rtx is enabled.
TEST_F(MediaSessionDescriptionFactoryTest,
       RespondentCreatesOfferAfterCreatingAnswerWithRtx) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::vector<VideoCodec> f1_codecs = MAKE_VECTOR(kVideoCodecs1);
  // This creates rtx for H264 with the payload type |f1_| uses.
  AddRtxCodec(VideoCodec::CreateRtxCodec(126, kVideoCodecs1[1].id), &f1_codecs);
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  std::vector<VideoCodec> f2_codecs = MAKE_VECTOR(kVideoCodecs2);
  // This creates rtx for H264 with the payload type |f2_| uses.
  AddRtxCodec(VideoCodec::CreateRtxCodec(125, kVideoCodecs2[0].id), &f2_codecs);
  f2_.set_video_codecs(f2_codecs, f2_codecs);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(answer.get());

  std::vector<VideoCodec> expected_codecs = MAKE_VECTOR(kVideoCodecsAnswer);
  AddRtxCodec(VideoCodec::CreateRtxCodec(126, kVideoCodecs1[1].id),
              &expected_codecs);

  EXPECT_EQ(expected_codecs, vcd->codecs());

  // Now, make sure we get same result (except for the order) if |f2_| creates
  // an updated offer even though the default payload types between |f1_| and
  // |f2_| are different.
  std::unique_ptr<SessionDescription> updated_offer(
      f2_.CreateOffer(opts, answer.get()));
  ASSERT_TRUE(updated_offer);
  std::unique_ptr<SessionDescription> updated_answer(
      f1_.CreateAnswer(updated_offer.get(), opts, answer.get()));

  const VideoContentDescription* updated_vcd =
      GetFirstVideoContentDescription(updated_answer.get());

  EXPECT_EQ(expected_codecs, updated_vcd->codecs());
}

// Regression test for:
// https://bugs.chromium.org/p/webrtc/issues/detail?id=8332
// Existing codecs should always appear before new codecs in re-offers. But
// under a specific set of circumstances, the existing RTX codec was ending up
// added to the end of the list.
TEST_F(MediaSessionDescriptionFactoryTest,
       RespondentCreatesOfferAfterCreatingAnswerWithRemappedRtxPayloadType) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  // We specifically choose different preferred payload types for VP8 to
  // trigger the issue.
  cricket::VideoCodec vp8_offerer(100, "VP8");
  cricket::VideoCodec vp8_offerer_rtx =
      VideoCodec::CreateRtxCodec(101, vp8_offerer.id);
  cricket::VideoCodec vp8_answerer(110, "VP8");
  cricket::VideoCodec vp8_answerer_rtx =
      VideoCodec::CreateRtxCodec(111, vp8_answerer.id);
  cricket::VideoCodec vp9(120, "VP9");
  cricket::VideoCodec vp9_rtx = VideoCodec::CreateRtxCodec(121, vp9.id);

  std::vector<VideoCodec> f1_codecs = {vp8_offerer, vp8_offerer_rtx};
  // We also specifically cause the answerer to prefer VP9, such that if it
  // *doesn't* honor the existing preferred codec (VP8) we'll notice.
  std::vector<VideoCodec> f2_codecs = {vp9, vp9_rtx, vp8_answerer,
                                       vp8_answerer_rtx};

  f1_.set_video_codecs(f1_codecs, f1_codecs);
  f2_.set_video_codecs(f2_codecs, f2_codecs);
  std::vector<AudioCodec> audio_codecs;
  f1_.set_audio_codecs(audio_codecs, audio_codecs);
  f2_.set_audio_codecs(audio_codecs, audio_codecs);

  // Offer will be {VP8, RTX for VP8}. Answer will be the same.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  // Updated offer *should* be {VP8, RTX for VP8, VP9, RTX for VP9}.
  // But if the bug is triggered, RTX for VP8 ends up last.
  std::unique_ptr<SessionDescription> updated_offer(
      f2_.CreateOffer(opts, answer.get()));

  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(updated_offer.get());
  std::vector<cricket::VideoCodec> codecs = vcd->codecs();
  ASSERT_EQ(4u, codecs.size());
  EXPECT_EQ(vp8_offerer, codecs[0]);
  EXPECT_EQ(vp8_offerer_rtx, codecs[1]);
  EXPECT_EQ(vp9, codecs[2]);
  EXPECT_EQ(vp9_rtx, codecs[3]);
}

// Create an updated offer that adds video after creating an audio only answer
// to the original offer. This test verifies that if a video codec and the RTX
// codec have the same default payload type as an audio codec that is already in
// use, the added codecs payload types are changed.
TEST_F(MediaSessionDescriptionFactoryTest,
       RespondentCreatesOfferWithVideoAndRtxAfterCreatingAudioAnswer) {
  std::vector<VideoCodec> f1_codecs = MAKE_VECTOR(kVideoCodecs1);
  // This creates rtx for H264 with the payload type |f1_| uses.
  AddRtxCodec(VideoCodec::CreateRtxCodec(126, kVideoCodecs1[1].id), &f1_codecs);
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  const AudioContentDescription* acd =
      GetFirstAudioContentDescription(answer.get());
  EXPECT_THAT(acd->codecs(), ElementsAreArray(kAudioCodecsAnswer));

  // Now - let |f2_| add video with RTX and let the payload type the RTX codec
  // reference  be the same as an audio codec that was negotiated in the
  // first offer/answer exchange.
  opts.media_description_options.clear();
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  std::vector<VideoCodec> f2_codecs = MAKE_VECTOR(kVideoCodecs2);
  int used_pl_type = acd->codecs()[0].id;
  f2_codecs[0].id = used_pl_type;  // Set the payload type for H264.
  AddRtxCodec(VideoCodec::CreateRtxCodec(125, used_pl_type), &f2_codecs);
  f2_.set_video_codecs(f2_codecs, f2_codecs);

  std::unique_ptr<SessionDescription> updated_offer(
      f2_.CreateOffer(opts, answer.get()));
  ASSERT_TRUE(updated_offer);
  std::unique_ptr<SessionDescription> updated_answer(
      f1_.CreateAnswer(updated_offer.get(), opts, answer.get()));

  const AudioContentDescription* updated_acd =
      GetFirstAudioContentDescription(answer.get());
  EXPECT_THAT(updated_acd->codecs(), ElementsAreArray(kAudioCodecsAnswer));

  const VideoContentDescription* updated_vcd =
      GetFirstVideoContentDescription(updated_answer.get());

  ASSERT_EQ("H264", updated_vcd->codecs()[0].name);
  ASSERT_EQ(cricket::kRtxCodecName, updated_vcd->codecs()[1].name);
  int new_h264_pl_type = updated_vcd->codecs()[0].id;
  EXPECT_NE(used_pl_type, new_h264_pl_type);
  VideoCodec rtx = updated_vcd->codecs()[1];
  int pt_referenced_by_rtx = rtc::FromString<int>(
      rtx.params[cricket::kCodecParamAssociatedPayloadType]);
  EXPECT_EQ(new_h264_pl_type, pt_referenced_by_rtx);
}

// Create an updated offer with RTX after creating an answer to an offer
// without RTX, and with different default payload types.
// Verify that the added RTX codec references the correct payload type.
TEST_F(MediaSessionDescriptionFactoryTest,
       RespondentCreatesOfferWithRtxAfterCreatingAnswerWithoutRtx) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  std::vector<VideoCodec> f2_codecs = MAKE_VECTOR(kVideoCodecs2);
  // This creates rtx for H264 with the payload type |f2_| uses.
  AddRtxCodec(VideoCodec::CreateRtxCodec(125, kVideoCodecs2[0].id), &f2_codecs);
  f2_.set_video_codecs(f2_codecs, f2_codecs);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer.get() != nullptr);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);

  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(answer.get());

  std::vector<VideoCodec> expected_codecs = MAKE_VECTOR(kVideoCodecsAnswer);
  EXPECT_EQ(expected_codecs, vcd->codecs());

  // Now, ensure that the RTX codec is created correctly when |f2_| creates an
  // updated offer, even though the default payload types are different from
  // those of |f1_|.
  std::unique_ptr<SessionDescription> updated_offer(
      f2_.CreateOffer(opts, answer.get()));
  ASSERT_TRUE(updated_offer);

  const VideoContentDescription* updated_vcd =
      GetFirstVideoContentDescription(updated_offer.get());

  // New offer should attempt to add H263, and RTX for H264.
  expected_codecs.push_back(kVideoCodecs2[1]);
  AddRtxCodec(VideoCodec::CreateRtxCodec(125, kVideoCodecs1[1].id),
              &expected_codecs);
  EXPECT_EQ(expected_codecs, updated_vcd->codecs());
}

// Test that RTX is ignored when there is no associated payload type parameter.
TEST_F(MediaSessionDescriptionFactoryTest, RtxWithoutApt) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::vector<VideoCodec> f1_codecs = MAKE_VECTOR(kVideoCodecs1);
  // This creates RTX without associated payload type parameter.
  AddRtxCodec(VideoCodec(126, cricket::kRtxCodecName), &f1_codecs);
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  std::vector<VideoCodec> f2_codecs = MAKE_VECTOR(kVideoCodecs2);
  // This creates RTX for H264 with the payload type |f2_| uses.
  AddRtxCodec(VideoCodec::CreateRtxCodec(125, kVideoCodecs2[0].id), &f2_codecs);
  f2_.set_video_codecs(f2_codecs, f2_codecs);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  // kCodecParamAssociatedPayloadType will always be added to the offer when RTX
  // is selected. Manually remove kCodecParamAssociatedPayloadType so that it
  // is possible to test that that RTX is dropped when
  // kCodecParamAssociatedPayloadType is missing in the offer.
  MediaContentDescription* media_desc =
      offer->GetContentDescriptionByName(cricket::CN_VIDEO);
  ASSERT_TRUE(media_desc);
  VideoContentDescription* desc = media_desc->as_video();
  std::vector<VideoCodec> codecs = desc->codecs();
  for (VideoCodec& codec : codecs) {
    if (absl::StartsWith(codec.name, cricket::kRtxCodecName)) {
      codec.params.clear();
    }
  }
  desc->set_codecs(codecs);

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  EXPECT_THAT(
      GetCodecNames(GetFirstVideoContentDescription(answer.get())->codecs()),
      Not(Contains(cricket::kRtxCodecName)));
}

// Test that RTX will be filtered out in the answer if its associated payload
// type doesn't match the local value.
TEST_F(MediaSessionDescriptionFactoryTest, FilterOutRtxIfAptDoesntMatch) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::vector<VideoCodec> f1_codecs = MAKE_VECTOR(kVideoCodecs1);
  // This creates RTX for H264 in sender.
  AddRtxCodec(VideoCodec::CreateRtxCodec(126, kVideoCodecs1[1].id), &f1_codecs);
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  std::vector<VideoCodec> f2_codecs = MAKE_VECTOR(kVideoCodecs2);
  // This creates RTX for H263 in receiver.
  AddRtxCodec(VideoCodec::CreateRtxCodec(125, kVideoCodecs2[1].id), &f2_codecs);
  f2_.set_video_codecs(f2_codecs, f2_codecs);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  // Associated payload type doesn't match, therefore, RTX codec is removed in
  // the answer.
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  EXPECT_THAT(
      GetCodecNames(GetFirstVideoContentDescription(answer.get())->codecs()),
      Not(Contains(cricket::kRtxCodecName)));
}

// Test that when multiple RTX codecs are offered, only the matched RTX codec
// is added in the answer, and the unsupported RTX codec is filtered out.
TEST_F(MediaSessionDescriptionFactoryTest,
       FilterOutUnsupportedRtxWhenCreatingAnswer) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::vector<VideoCodec> f1_codecs = MAKE_VECTOR(kVideoCodecs1);
  // This creates RTX for H264-SVC in sender.
  AddRtxCodec(VideoCodec::CreateRtxCodec(125, kVideoCodecs1[0].id), &f1_codecs);
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  // This creates RTX for H264 in sender.
  AddRtxCodec(VideoCodec::CreateRtxCodec(126, kVideoCodecs1[1].id), &f1_codecs);
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  std::vector<VideoCodec> f2_codecs = MAKE_VECTOR(kVideoCodecs2);
  // This creates RTX for H264 in receiver.
  AddRtxCodec(VideoCodec::CreateRtxCodec(124, kVideoCodecs2[0].id), &f2_codecs);
  f2_.set_video_codecs(f2_codecs, f1_codecs);

  // H264-SVC codec is removed in the answer, therefore, associated RTX codec
  // for H264-SVC should also be removed.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);
  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(answer.get());
  std::vector<VideoCodec> expected_codecs = MAKE_VECTOR(kVideoCodecsAnswer);
  AddRtxCodec(VideoCodec::CreateRtxCodec(126, kVideoCodecs1[1].id),
              &expected_codecs);

  EXPECT_EQ(expected_codecs, vcd->codecs());
}

// Test that after one RTX codec has been negotiated, a new offer can attempt
// to add another.
TEST_F(MediaSessionDescriptionFactoryTest, AddSecondRtxInNewOffer) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  std::vector<VideoCodec> f1_codecs = MAKE_VECTOR(kVideoCodecs1);
  // This creates RTX for H264 for the offerer.
  AddRtxCodec(VideoCodec::CreateRtxCodec(126, kVideoCodecs1[1].id), &f1_codecs);
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer);
  const VideoContentDescription* vcd =
      GetFirstVideoContentDescription(offer.get());

  std::vector<VideoCodec> expected_codecs = MAKE_VECTOR(kVideoCodecs1);
  AddRtxCodec(VideoCodec::CreateRtxCodec(126, kVideoCodecs1[1].id),
              &expected_codecs);
  EXPECT_EQ(expected_codecs, vcd->codecs());

  // Now, attempt to add RTX for H264-SVC.
  AddRtxCodec(VideoCodec::CreateRtxCodec(125, kVideoCodecs1[0].id), &f1_codecs);
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  std::unique_ptr<SessionDescription> updated_offer(
      f1_.CreateOffer(opts, offer.get()));
  ASSERT_TRUE(updated_offer);
  vcd = GetFirstVideoContentDescription(updated_offer.get());

  AddRtxCodec(VideoCodec::CreateRtxCodec(125, kVideoCodecs1[0].id),
              &expected_codecs);
  EXPECT_EQ(expected_codecs, vcd->codecs());
}

// Test that when RTX is used in conjunction with simulcast, an RTX ssrc is
// generated for each simulcast ssrc and correctly grouped.
TEST_F(MediaSessionDescriptionFactoryTest, SimSsrcsGenerateMultipleRtxSsrcs) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  // Add simulcast streams.
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, "stream1",
                                        {"stream1label"}, 3, &opts);

  // Use a single real codec, and then add RTX for it.
  std::vector<VideoCodec> f1_codecs;
  f1_codecs.push_back(VideoCodec(97, "H264"));
  AddRtxCodec(VideoCodec::CreateRtxCodec(125, 97), &f1_codecs);
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  // Ensure that the offer has an RTX ssrc for each regular ssrc, and that there
  // is a FID ssrc + grouping for each.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  MediaContentDescription* media_desc =
      offer->GetContentDescriptionByName(cricket::CN_VIDEO);
  ASSERT_TRUE(media_desc);
  VideoContentDescription* desc = media_desc->as_video();
  const StreamParamsVec& streams = desc->streams();
  // Single stream.
  ASSERT_EQ(1u, streams.size());
  // Stream should have 6 ssrcs: 3 for video, 3 for RTX.
  EXPECT_EQ(6u, streams[0].ssrcs.size());
  // And should have a SIM group for the simulcast.
  EXPECT_TRUE(streams[0].has_ssrc_group("SIM"));
  // And a FID group for RTX.
  EXPECT_TRUE(streams[0].has_ssrc_group("FID"));
  std::vector<uint32_t> primary_ssrcs;
  streams[0].GetPrimarySsrcs(&primary_ssrcs);
  EXPECT_EQ(3u, primary_ssrcs.size());
  std::vector<uint32_t> fid_ssrcs;
  streams[0].GetFidSsrcs(primary_ssrcs, &fid_ssrcs);
  EXPECT_EQ(3u, fid_ssrcs.size());
}

// Test that, when the FlexFEC codec is added, a FlexFEC ssrc is created
// together with a FEC-FR grouping. Guarded by WebRTC-FlexFEC-03 trial.
TEST_F(MediaSessionDescriptionFactoryTest, GenerateFlexfecSsrc) {
  webrtc::test::ScopedFieldTrials override_field_trials(
      "WebRTC-FlexFEC-03/Enabled/");
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  // Add single stream.
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, "stream1",
                                        {"stream1label"}, 1, &opts);

  // Use a single real codec, and then add FlexFEC for it.
  std::vector<VideoCodec> f1_codecs;
  f1_codecs.push_back(VideoCodec(97, "H264"));
  f1_codecs.push_back(VideoCodec(118, "flexfec-03"));
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  // Ensure that the offer has a single FlexFEC ssrc and that
  // there is no FEC-FR ssrc + grouping for each.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer.get() != nullptr);
  MediaContentDescription* media_desc =
      offer->GetContentDescriptionByName(cricket::CN_VIDEO);
  ASSERT_TRUE(media_desc);
  VideoContentDescription* desc = media_desc->as_video();
  const StreamParamsVec& streams = desc->streams();
  // Single stream.
  ASSERT_EQ(1u, streams.size());
  // Stream should have 2 ssrcs: 1 for video, 1 for FlexFEC.
  EXPECT_EQ(2u, streams[0].ssrcs.size());
  // And should have a FEC-FR group for FlexFEC.
  EXPECT_TRUE(streams[0].has_ssrc_group("FEC-FR"));
  std::vector<uint32_t> primary_ssrcs;
  streams[0].GetPrimarySsrcs(&primary_ssrcs);
  ASSERT_EQ(1u, primary_ssrcs.size());
  uint32_t flexfec_ssrc;
  EXPECT_TRUE(streams[0].GetFecFrSsrc(primary_ssrcs[0], &flexfec_ssrc));
  EXPECT_NE(flexfec_ssrc, 0u);
}

// Test that FlexFEC is disabled for simulcast.
// TODO(brandtr): Remove this test when we support simulcast, either through
// multiple FlexfecSenders, or through multistream protection.
TEST_F(MediaSessionDescriptionFactoryTest, SimSsrcsGenerateNoFlexfecSsrcs) {
  webrtc::test::ScopedFieldTrials override_field_trials(
      "WebRTC-FlexFEC-03/Enabled/");
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  // Add simulcast streams.
  AttachSenderToMediaDescriptionOptions("video", MEDIA_TYPE_VIDEO, "stream1",
                                        {"stream1label"}, 3, &opts);

  // Use a single real codec, and then add FlexFEC for it.
  std::vector<VideoCodec> f1_codecs;
  f1_codecs.push_back(VideoCodec(97, "H264"));
  f1_codecs.push_back(VideoCodec(118, "flexfec-03"));
  f1_.set_video_codecs(f1_codecs, f1_codecs);

  // Ensure that the offer has no FlexFEC ssrcs for each regular ssrc, and that
  // there is no FEC-FR ssrc + grouping for each.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer.get() != nullptr);
  MediaContentDescription* media_desc =
      offer->GetContentDescriptionByName(cricket::CN_VIDEO);
  ASSERT_TRUE(media_desc);
  VideoContentDescription* desc = media_desc->as_video();
  const StreamParamsVec& streams = desc->streams();
  // Single stream.
  ASSERT_EQ(1u, streams.size());
  // Stream should have 3 ssrcs: 3 for video, 0 for FlexFEC.
  EXPECT_EQ(3u, streams[0].ssrcs.size());
  // And should have a SIM group for the simulcast.
  EXPECT_TRUE(streams[0].has_ssrc_group("SIM"));
  // And not a FEC-FR group for FlexFEC.
  EXPECT_FALSE(streams[0].has_ssrc_group("FEC-FR"));
  std::vector<uint32_t> primary_ssrcs;
  streams[0].GetPrimarySsrcs(&primary_ssrcs);
  EXPECT_EQ(3u, primary_ssrcs.size());
  for (uint32_t primary_ssrc : primary_ssrcs) {
    uint32_t flexfec_ssrc;
    EXPECT_FALSE(streams[0].GetFecFrSsrc(primary_ssrc, &flexfec_ssrc));
  }
}

// Create an updated offer after creating an answer to the original offer and
// verify that the RTP header extensions that were part of the original answer
// are not changed in the updated offer.
TEST_F(MediaSessionDescriptionFactoryTest,
       RespondentCreatesOfferAfterCreatingAnswerWithRtpExtensions) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension1),
                                   MAKE_VECTOR(kVideoRtpExtension1), &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);
  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension2),
                                   MAKE_VECTOR(kVideoRtpExtension2), &opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, NULL);

  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtensionAnswer),
      GetFirstAudioContentDescription(answer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtensionAnswer),
      GetFirstVideoContentDescription(answer.get())->rtp_header_extensions());

  std::unique_ptr<SessionDescription> updated_offer(
      f2_.CreateOffer(opts, answer.get()));

  // The expected RTP header extensions in the new offer are the resulting
  // extensions from the first offer/answer exchange plus the extensions only
  // |f2_| offer.
  // Since the default local extension id |f2_| uses has already been used by
  // |f1_| for another extensions, it is changed to 13.
  const RtpExtension kUpdatedAudioRtpExtensions[] = {
      kAudioRtpExtensionAnswer[0],
      RtpExtension(kAudioRtpExtension2[1].uri, 13),
      kAudioRtpExtension2[2],
  };

  // Since the default local extension id |f2_| uses has already been used by
  // |f1_| for another extensions, is is changed to 12.
  const RtpExtension kUpdatedVideoRtpExtensions[] = {
      kVideoRtpExtensionAnswer[0],
      RtpExtension(kVideoRtpExtension2[1].uri, 12),
      kVideoRtpExtension2[2],
  };

  const AudioContentDescription* updated_acd =
      GetFirstAudioContentDescription(updated_offer.get());
  EXPECT_EQ(MAKE_VECTOR(kUpdatedAudioRtpExtensions),
            updated_acd->rtp_header_extensions());

  const VideoContentDescription* updated_vcd =
      GetFirstVideoContentDescription(updated_offer.get());
  EXPECT_EQ(MAKE_VECTOR(kUpdatedVideoRtpExtensions),
            updated_vcd->rtp_header_extensions());
}

// Verify that if the same RTP extension URI is used for audio and video, the
// same ID is used. Also verify that the ID isn't changed when creating an
// updated offer (this was previously a bug).
TEST_F(MediaSessionDescriptionFactoryTest, RtpExtensionIdReused) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  SetAudioVideoRtpHeaderExtensions(MAKE_VECTOR(kAudioRtpExtension3),
                                   MAKE_VECTOR(kVideoRtpExtension3), &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);

  // Since the audio extensions used ID 3 for "both_audio_and_video", so should
  // the video extensions.
  const RtpExtension kExpectedVideoRtpExtension[] = {
      kVideoRtpExtension3[0],
      kAudioRtpExtension3[1],
  };

  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtension3),
      GetFirstAudioContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kExpectedVideoRtpExtension),
      GetFirstVideoContentDescription(offer.get())->rtp_header_extensions());

  // Nothing should change when creating a new offer
  std::unique_ptr<SessionDescription> updated_offer(
      f1_.CreateOffer(opts, offer.get()));

  EXPECT_EQ(MAKE_VECTOR(kAudioRtpExtension3),
            GetFirstAudioContentDescription(updated_offer.get())
                ->rtp_header_extensions());
  EXPECT_EQ(MAKE_VECTOR(kExpectedVideoRtpExtension),
            GetFirstVideoContentDescription(updated_offer.get())
                ->rtp_header_extensions());
}

// Same as "RtpExtensionIdReused" above for encrypted RTP extensions.
TEST_F(MediaSessionDescriptionFactoryTest, RtpExtensionIdReusedEncrypted) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);

  f1_.set_enable_encrypted_rtp_header_extensions(true);
  f2_.set_enable_encrypted_rtp_header_extensions(true);

  SetAudioVideoRtpHeaderExtensions(
      MAKE_VECTOR(kAudioRtpExtension3ForEncryption),
      MAKE_VECTOR(kVideoRtpExtension3ForEncryption), &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, NULL);

  EXPECT_EQ(
      MAKE_VECTOR(kAudioRtpExtension3ForEncryptionOffer),
      GetFirstAudioContentDescription(offer.get())->rtp_header_extensions());
  EXPECT_EQ(
      MAKE_VECTOR(kVideoRtpExtension3ForEncryptionOffer),
      GetFirstVideoContentDescription(offer.get())->rtp_header_extensions());

  // Nothing should change when creating a new offer
  std::unique_ptr<SessionDescription> updated_offer(
      f1_.CreateOffer(opts, offer.get()));

  EXPECT_EQ(MAKE_VECTOR(kAudioRtpExtension3ForEncryptionOffer),
            GetFirstAudioContentDescription(updated_offer.get())
                ->rtp_header_extensions());
  EXPECT_EQ(MAKE_VECTOR(kVideoRtpExtension3ForEncryptionOffer),
            GetFirstVideoContentDescription(updated_offer.get())
                ->rtp_header_extensions());
}

TEST(MediaSessionDescription, CopySessionDescription) {
  SessionDescription source;
  cricket::ContentGroup group(cricket::CN_AUDIO);
  source.AddGroup(group);
  std::unique_ptr<AudioContentDescription> acd =
      std::make_unique<AudioContentDescription>();
  acd->set_codecs(MAKE_VECTOR(kAudioCodecs1));
  acd->AddLegacyStream(1);
  source.AddContent(cricket::CN_AUDIO, MediaProtocolType::kRtp, acd->Clone());
  std::unique_ptr<VideoContentDescription> vcd =
      std::make_unique<VideoContentDescription>();
  vcd->set_codecs(MAKE_VECTOR(kVideoCodecs1));
  vcd->AddLegacyStream(2);
  source.AddContent(cricket::CN_VIDEO, MediaProtocolType::kRtp, vcd->Clone());

  std::unique_ptr<SessionDescription> copy = source.Clone();
  ASSERT_TRUE(copy.get() != NULL);
  EXPECT_TRUE(copy->HasGroup(cricket::CN_AUDIO));
  const ContentInfo* ac = copy->GetContentByName("audio");
  const ContentInfo* vc = copy->GetContentByName("video");
  ASSERT_TRUE(ac != NULL);
  ASSERT_TRUE(vc != NULL);
  EXPECT_EQ(MediaProtocolType::kRtp, ac->type);
  const AudioContentDescription* acd_copy = ac->media_description()->as_audio();
  EXPECT_EQ(acd->codecs(), acd_copy->codecs());
  EXPECT_EQ(1u, acd->first_ssrc());

  EXPECT_EQ(MediaProtocolType::kRtp, vc->type);
  const VideoContentDescription* vcd_copy = vc->media_description()->as_video();
  EXPECT_EQ(vcd->codecs(), vcd_copy->codecs());
  EXPECT_EQ(2u, vcd->first_ssrc());
}

// The below TestTransportInfoXXX tests create different offers/answers, and
// ensure the TransportInfo in the SessionDescription matches what we expect.
TEST_F(MediaSessionDescriptionFactoryTest, TestTransportInfoOfferAudio) {
  MediaSessionOptions options;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &options);
  TestTransportInfo(true, options, false);
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestTransportInfoOfferIceRenomination) {
  MediaSessionOptions options;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &options);
  options.media_description_options[0]
      .transport_options.enable_ice_renomination = true;
  TestTransportInfo(true, options, false);
}

TEST_F(MediaSessionDescriptionFactoryTest, TestTransportInfoOfferAudioCurrent) {
  MediaSessionOptions options;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &options);
  TestTransportInfo(true, options, true);
}

TEST_F(MediaSessionDescriptionFactoryTest, TestTransportInfoOfferMultimedia) {
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  TestTransportInfo(true, options, false);
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestTransportInfoOfferMultimediaCurrent) {
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  TestTransportInfo(true, options, true);
}

TEST_F(MediaSessionDescriptionFactoryTest, TestTransportInfoOfferBundle) {
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  options.bundle_enabled = true;
  TestTransportInfo(true, options, false);
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestTransportInfoOfferBundleCurrent) {
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  options.bundle_enabled = true;
  TestTransportInfo(true, options, true);
}

TEST_F(MediaSessionDescriptionFactoryTest, TestTransportInfoAnswerAudio) {
  MediaSessionOptions options;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &options);
  TestTransportInfo(false, options, false);
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestTransportInfoAnswerIceRenomination) {
  MediaSessionOptions options;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &options);
  options.media_description_options[0]
      .transport_options.enable_ice_renomination = true;
  TestTransportInfo(false, options, false);
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestTransportInfoAnswerAudioCurrent) {
  MediaSessionOptions options;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &options);
  TestTransportInfo(false, options, true);
}

TEST_F(MediaSessionDescriptionFactoryTest, TestTransportInfoAnswerMultimedia) {
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  TestTransportInfo(false, options, false);
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestTransportInfoAnswerMultimediaCurrent) {
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  TestTransportInfo(false, options, true);
}

TEST_F(MediaSessionDescriptionFactoryTest, TestTransportInfoAnswerBundle) {
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  options.bundle_enabled = true;
  TestTransportInfo(false, options, false);
}

TEST_F(MediaSessionDescriptionFactoryTest,
       TestTransportInfoAnswerBundleCurrent) {
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  options.bundle_enabled = true;
  TestTransportInfo(false, options, true);
}

// Create an offer with bundle enabled and verify the crypto parameters are
// the common set of the available cryptos.
TEST_F(MediaSessionDescriptionFactoryTest, TestCryptoWithOfferBundle) {
  TestCryptoWithBundle(true);
}

// Create an answer with bundle enabled and verify the crypto parameters are
// the common set of the available cryptos.
TEST_F(MediaSessionDescriptionFactoryTest, TestCryptoWithAnswerBundle) {
  TestCryptoWithBundle(false);
}

// Verifies that creating answer fails if the offer has UDP/TLS/RTP/SAVPF but
// DTLS is not enabled locally.
TEST_F(MediaSessionDescriptionFactoryTest,
       TestOfferDtlsSavpfWithoutDtlsFailed) {
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  tdf1_.set_secure(SEC_DISABLED);
  tdf2_.set_secure(SEC_DISABLED);

  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(CreatePlanBMediaSessionOptions(), NULL);
  ASSERT_TRUE(offer.get() != NULL);
  ContentInfo* offer_content = offer->GetContentByName("audio");
  ASSERT_TRUE(offer_content != NULL);
  AudioContentDescription* offer_audio_desc =
      offer_content->media_description()->as_audio();
  offer_audio_desc->set_protocol(cricket::kMediaProtocolDtlsSavpf);

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), CreatePlanBMediaSessionOptions(), NULL);
  ASSERT_TRUE(answer != NULL);
  ContentInfo* answer_content = answer->GetContentByName("audio");
  ASSERT_TRUE(answer_content != NULL);

  ASSERT_TRUE(answer_content->rejected);
}

// Offers UDP/TLS/RTP/SAVPF and verifies the answer can be created and contains
// UDP/TLS/RTP/SAVPF.
TEST_F(MediaSessionDescriptionFactoryTest, TestOfferDtlsSavpfCreateAnswer) {
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  tdf1_.set_secure(SEC_ENABLED);
  tdf2_.set_secure(SEC_ENABLED);

  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(CreatePlanBMediaSessionOptions(), NULL);
  ASSERT_TRUE(offer.get() != NULL);
  ContentInfo* offer_content = offer->GetContentByName("audio");
  ASSERT_TRUE(offer_content != NULL);
  AudioContentDescription* offer_audio_desc =
      offer_content->media_description()->as_audio();
  offer_audio_desc->set_protocol(cricket::kMediaProtocolDtlsSavpf);

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), CreatePlanBMediaSessionOptions(), NULL);
  ASSERT_TRUE(answer != NULL);

  const ContentInfo* answer_content = answer->GetContentByName("audio");
  ASSERT_TRUE(answer_content != NULL);
  ASSERT_FALSE(answer_content->rejected);

  const AudioContentDescription* answer_audio_desc =
      answer_content->media_description()->as_audio();
  EXPECT_EQ(cricket::kMediaProtocolDtlsSavpf, answer_audio_desc->protocol());
}

// Test that we include both SDES and DTLS in the offer, but only include SDES
// in the answer if DTLS isn't negotiated.
TEST_F(MediaSessionDescriptionFactoryTest, TestCryptoDtls) {
  f1_.set_secure(SEC_ENABLED);
  f2_.set_secure(SEC_ENABLED);
  tdf1_.set_secure(SEC_ENABLED);
  tdf2_.set_secure(SEC_DISABLED);
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  std::unique_ptr<SessionDescription> offer, answer;
  const cricket::MediaContentDescription* audio_media_desc;
  const cricket::MediaContentDescription* video_media_desc;
  const cricket::TransportDescription* audio_trans_desc;
  const cricket::TransportDescription* video_trans_desc;

  // Generate an offer with SDES and DTLS support.
  offer = f1_.CreateOffer(options, NULL);
  ASSERT_TRUE(offer.get() != NULL);

  audio_media_desc = offer->GetContentDescriptionByName("audio");
  ASSERT_TRUE(audio_media_desc != NULL);
  video_media_desc = offer->GetContentDescriptionByName("video");
  ASSERT_TRUE(video_media_desc != NULL);
  EXPECT_EQ(1u, audio_media_desc->cryptos().size());
  EXPECT_EQ(1u, video_media_desc->cryptos().size());

  audio_trans_desc = offer->GetTransportDescriptionByName("audio");
  ASSERT_TRUE(audio_trans_desc != NULL);
  video_trans_desc = offer->GetTransportDescriptionByName("video");
  ASSERT_TRUE(video_trans_desc != NULL);
  ASSERT_TRUE(audio_trans_desc->identity_fingerprint.get() != NULL);
  ASSERT_TRUE(video_trans_desc->identity_fingerprint.get() != NULL);

  // Generate an answer with only SDES support, since tdf2 has crypto disabled.
  answer = f2_.CreateAnswer(offer.get(), options, NULL);
  ASSERT_TRUE(answer.get() != NULL);

  audio_media_desc = answer->GetContentDescriptionByName("audio");
  ASSERT_TRUE(audio_media_desc != NULL);
  video_media_desc = answer->GetContentDescriptionByName("video");
  ASSERT_TRUE(video_media_desc != NULL);
  EXPECT_EQ(1u, audio_media_desc->cryptos().size());
  EXPECT_EQ(1u, video_media_desc->cryptos().size());

  audio_trans_desc = answer->GetTransportDescriptionByName("audio");
  ASSERT_TRUE(audio_trans_desc != NULL);
  video_trans_desc = answer->GetTransportDescriptionByName("video");
  ASSERT_TRUE(video_trans_desc != NULL);
  ASSERT_TRUE(audio_trans_desc->identity_fingerprint.get() == NULL);
  ASSERT_TRUE(video_trans_desc->identity_fingerprint.get() == NULL);

  // Enable DTLS; the answer should now only have DTLS support.
  tdf2_.set_secure(SEC_ENABLED);
  answer = f2_.CreateAnswer(offer.get(), options, NULL);
  ASSERT_TRUE(answer.get() != NULL);

  audio_media_desc = answer->GetContentDescriptionByName("audio");
  ASSERT_TRUE(audio_media_desc != NULL);
  video_media_desc = answer->GetContentDescriptionByName("video");
  ASSERT_TRUE(video_media_desc != NULL);
  EXPECT_TRUE(audio_media_desc->cryptos().empty());
  EXPECT_TRUE(video_media_desc->cryptos().empty());
  EXPECT_EQ(cricket::kMediaProtocolSavpf, audio_media_desc->protocol());
  EXPECT_EQ(cricket::kMediaProtocolSavpf, video_media_desc->protocol());

  audio_trans_desc = answer->GetTransportDescriptionByName("audio");
  ASSERT_TRUE(audio_trans_desc != NULL);
  video_trans_desc = answer->GetTransportDescriptionByName("video");
  ASSERT_TRUE(video_trans_desc != NULL);
  ASSERT_TRUE(audio_trans_desc->identity_fingerprint.get() != NULL);
  ASSERT_TRUE(video_trans_desc->identity_fingerprint.get() != NULL);

  // Try creating offer again. DTLS enabled now, crypto's should be empty
  // in new offer.
  offer = f1_.CreateOffer(options, offer.get());
  ASSERT_TRUE(offer.get() != NULL);
  audio_media_desc = offer->GetContentDescriptionByName("audio");
  ASSERT_TRUE(audio_media_desc != NULL);
  video_media_desc = offer->GetContentDescriptionByName("video");
  ASSERT_TRUE(video_media_desc != NULL);
  EXPECT_TRUE(audio_media_desc->cryptos().empty());
  EXPECT_TRUE(video_media_desc->cryptos().empty());

  audio_trans_desc = offer->GetTransportDescriptionByName("audio");
  ASSERT_TRUE(audio_trans_desc != NULL);
  video_trans_desc = offer->GetTransportDescriptionByName("video");
  ASSERT_TRUE(video_trans_desc != NULL);
  ASSERT_TRUE(audio_trans_desc->identity_fingerprint.get() != NULL);
  ASSERT_TRUE(video_trans_desc->identity_fingerprint.get() != NULL);
}

// Test that an answer can't be created if cryptos are required but the offer is
// unsecure.
TEST_F(MediaSessionDescriptionFactoryTest, TestSecureAnswerToUnsecureOffer) {
  MediaSessionOptions options = CreatePlanBMediaSessionOptions();
  f1_.set_secure(SEC_DISABLED);
  tdf1_.set_secure(SEC_DISABLED);
  f2_.set_secure(SEC_REQUIRED);
  tdf1_.set_secure(SEC_ENABLED);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(options, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), options, NULL);
  EXPECT_TRUE(answer.get() == NULL);
}

// Test that we accept a DTLS offer without SDES and create an appropriate
// answer.
TEST_F(MediaSessionDescriptionFactoryTest, TestCryptoOfferDtlsButNotSdes) {
  f1_.set_secure(SEC_DISABLED);
  f2_.set_secure(SEC_ENABLED);
  tdf1_.set_secure(SEC_ENABLED);
  tdf2_.set_secure(SEC_ENABLED);
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);

  // Generate an offer with DTLS but without SDES.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(options, NULL);
  ASSERT_TRUE(offer.get() != NULL);

  const AudioContentDescription* audio_offer =
      GetFirstAudioContentDescription(offer.get());
  ASSERT_TRUE(audio_offer->cryptos().empty());
  const VideoContentDescription* video_offer =
      GetFirstVideoContentDescription(offer.get());
  ASSERT_TRUE(video_offer->cryptos().empty());

  const cricket::TransportDescription* audio_offer_trans_desc =
      offer->GetTransportDescriptionByName("audio");
  ASSERT_TRUE(audio_offer_trans_desc->identity_fingerprint.get() != NULL);
  const cricket::TransportDescription* video_offer_trans_desc =
      offer->GetTransportDescriptionByName("video");
  ASSERT_TRUE(video_offer_trans_desc->identity_fingerprint.get() != NULL);

  // Generate an answer with DTLS.
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), options, NULL);
  ASSERT_TRUE(answer.get() != NULL);

  const cricket::TransportDescription* audio_answer_trans_desc =
      answer->GetTransportDescriptionByName("audio");
  EXPECT_TRUE(audio_answer_trans_desc->identity_fingerprint.get() != NULL);
  const cricket::TransportDescription* video_answer_trans_desc =
      answer->GetTransportDescriptionByName("video");
  EXPECT_TRUE(video_answer_trans_desc->identity_fingerprint.get() != NULL);
}

// Verifies if vad_enabled option is set to false, CN codecs are not present in
// offer or answer.
TEST_F(MediaSessionDescriptionFactoryTest, TestVADEnableOption) {
  MediaSessionOptions options;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &options);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(options, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  const ContentInfo* audio_content = offer->GetContentByName("audio");
  EXPECT_FALSE(VerifyNoCNCodecs(audio_content));

  options.vad_enabled = false;
  offer = f1_.CreateOffer(options, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  audio_content = offer->GetContentByName("audio");
  EXPECT_TRUE(VerifyNoCNCodecs(audio_content));
  std::unique_ptr<SessionDescription> answer =
      f1_.CreateAnswer(offer.get(), options, NULL);
  ASSERT_TRUE(answer.get() != NULL);
  audio_content = answer->GetContentByName("audio");
  EXPECT_TRUE(VerifyNoCNCodecs(audio_content));
}

// Test that the generated MIDs match the existing offer.
TEST_F(MediaSessionDescriptionFactoryTest, TestMIDsMatchesExistingOffer) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio_modified",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video_modified",
                             RtpTransceiverDirection::kRecvOnly, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_DATA, "data_modified",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  // Create offer.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  std::unique_ptr<SessionDescription> updated_offer(
      f1_.CreateOffer(opts, offer.get()));

  const ContentInfo* audio_content = GetFirstAudioContent(updated_offer.get());
  const ContentInfo* video_content = GetFirstVideoContent(updated_offer.get());
  const ContentInfo* data_content = GetFirstDataContent(updated_offer.get());
  ASSERT_TRUE(audio_content != nullptr);
  ASSERT_TRUE(video_content != nullptr);
  ASSERT_TRUE(data_content != nullptr);
  EXPECT_EQ("audio_modified", audio_content->name);
  EXPECT_EQ("video_modified", video_content->name);
  EXPECT_EQ("data_modified", data_content->name);
}

// The following tests verify that the unified plan SDP is supported.
// Test that we can create an offer with multiple media sections of same media
// type.
TEST_F(MediaSessionDescriptionFactoryTest,
       CreateOfferWithMultipleAVMediaSections) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio_1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AttachSenderToMediaDescriptionOptions(
      "audio_1", MEDIA_TYPE_AUDIO, kAudioTrack1, {kMediaStream1}, 1, &opts);

  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video_1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AttachSenderToMediaDescriptionOptions(
      "video_1", MEDIA_TYPE_VIDEO, kVideoTrack1, {kMediaStream1}, 1, &opts);

  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio_2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AttachSenderToMediaDescriptionOptions(
      "audio_2", MEDIA_TYPE_AUDIO, kAudioTrack2, {kMediaStream2}, 1, &opts);

  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video_2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AttachSenderToMediaDescriptionOptions(
      "video_2", MEDIA_TYPE_VIDEO, kVideoTrack2, {kMediaStream2}, 1, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer);

  ASSERT_EQ(4u, offer->contents().size());
  EXPECT_FALSE(offer->contents()[0].rejected);
  const AudioContentDescription* acd =
      offer->contents()[0].media_description()->as_audio();
  ASSERT_EQ(1u, acd->streams().size());
  EXPECT_EQ(kAudioTrack1, acd->streams()[0].id);
  EXPECT_EQ(RtpTransceiverDirection::kSendRecv, acd->direction());

  EXPECT_FALSE(offer->contents()[1].rejected);
  const VideoContentDescription* vcd =
      offer->contents()[1].media_description()->as_video();
  ASSERT_EQ(1u, vcd->streams().size());
  EXPECT_EQ(kVideoTrack1, vcd->streams()[0].id);
  EXPECT_EQ(RtpTransceiverDirection::kSendRecv, vcd->direction());

  EXPECT_FALSE(offer->contents()[2].rejected);
  acd = offer->contents()[2].media_description()->as_audio();
  ASSERT_EQ(1u, acd->streams().size());
  EXPECT_EQ(kAudioTrack2, acd->streams()[0].id);
  EXPECT_EQ(RtpTransceiverDirection::kSendRecv, acd->direction());

  EXPECT_FALSE(offer->contents()[3].rejected);
  vcd = offer->contents()[3].media_description()->as_video();
  ASSERT_EQ(1u, vcd->streams().size());
  EXPECT_EQ(kVideoTrack2, vcd->streams()[0].id);
  EXPECT_EQ(RtpTransceiverDirection::kSendRecv, vcd->direction());
}

// Test that we can create an answer with multiple media sections of same media
// type.
TEST_F(MediaSessionDescriptionFactoryTest,
       CreateAnswerWithMultipleAVMediaSections) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio_1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AttachSenderToMediaDescriptionOptions(
      "audio_1", MEDIA_TYPE_AUDIO, kAudioTrack1, {kMediaStream1}, 1, &opts);

  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video_1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AttachSenderToMediaDescriptionOptions(
      "video_1", MEDIA_TYPE_VIDEO, kVideoTrack1, {kMediaStream1}, 1, &opts);

  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio_2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AttachSenderToMediaDescriptionOptions(
      "audio_2", MEDIA_TYPE_AUDIO, kAudioTrack2, {kMediaStream2}, 1, &opts);

  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video_2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AttachSenderToMediaDescriptionOptions(
      "video_2", MEDIA_TYPE_VIDEO, kVideoTrack2, {kMediaStream2}, 1, &opts);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);

  ASSERT_EQ(4u, answer->contents().size());
  EXPECT_FALSE(answer->contents()[0].rejected);
  const AudioContentDescription* acd =
      answer->contents()[0].media_description()->as_audio();
  ASSERT_EQ(1u, acd->streams().size());
  EXPECT_EQ(kAudioTrack1, acd->streams()[0].id);
  EXPECT_EQ(RtpTransceiverDirection::kSendRecv, acd->direction());

  EXPECT_FALSE(answer->contents()[1].rejected);
  const VideoContentDescription* vcd =
      answer->contents()[1].media_description()->as_video();
  ASSERT_EQ(1u, vcd->streams().size());
  EXPECT_EQ(kVideoTrack1, vcd->streams()[0].id);
  EXPECT_EQ(RtpTransceiverDirection::kSendRecv, vcd->direction());

  EXPECT_FALSE(answer->contents()[2].rejected);
  acd = answer->contents()[2].media_description()->as_audio();
  ASSERT_EQ(1u, acd->streams().size());
  EXPECT_EQ(kAudioTrack2, acd->streams()[0].id);
  EXPECT_EQ(RtpTransceiverDirection::kSendRecv, acd->direction());

  EXPECT_FALSE(answer->contents()[3].rejected);
  vcd = answer->contents()[3].media_description()->as_video();
  ASSERT_EQ(1u, vcd->streams().size());
  EXPECT_EQ(kVideoTrack2, vcd->streams()[0].id);
  EXPECT_EQ(RtpTransceiverDirection::kSendRecv, vcd->direction());
}

// Test that the media section will be rejected in offer if the corresponding
// MediaDescriptionOptions is stopped by the offerer.
TEST_F(MediaSessionDescriptionFactoryTest,
       CreateOfferWithMediaSectionStoppedByOfferer) {
  // Create an offer with two audio sections and one of them is stopped.
  MediaSessionOptions offer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &offer_opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio2",
                             RtpTransceiverDirection::kInactive, kStopped,
                             &offer_opts);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(offer_opts, nullptr);
  ASSERT_TRUE(offer);
  ASSERT_EQ(2u, offer->contents().size());
  EXPECT_FALSE(offer->contents()[0].rejected);
  EXPECT_TRUE(offer->contents()[1].rejected);
}

// Test that the media section will be rejected in answer if the corresponding
// MediaDescriptionOptions is stopped by the offerer.
TEST_F(MediaSessionDescriptionFactoryTest,
       CreateAnswerWithMediaSectionStoppedByOfferer) {
  // Create an offer with two audio sections and one of them is stopped.
  MediaSessionOptions offer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &offer_opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio2",
                             RtpTransceiverDirection::kInactive, kStopped,
                             &offer_opts);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(offer_opts, nullptr);
  ASSERT_TRUE(offer);
  ASSERT_EQ(2u, offer->contents().size());
  EXPECT_FALSE(offer->contents()[0].rejected);
  EXPECT_TRUE(offer->contents()[1].rejected);

  // Create an answer based on the offer.
  MediaSessionOptions answer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &answer_opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &answer_opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), answer_opts, nullptr);
  ASSERT_EQ(2u, answer->contents().size());
  EXPECT_FALSE(answer->contents()[0].rejected);
  EXPECT_TRUE(answer->contents()[1].rejected);
}

// Test that the media section will be rejected in answer if the corresponding
// MediaDescriptionOptions is stopped by the answerer.
TEST_F(MediaSessionDescriptionFactoryTest,
       CreateAnswerWithMediaSectionRejectedByAnswerer) {
  // Create an offer with two audio sections.
  MediaSessionOptions offer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &offer_opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &offer_opts);
  std::unique_ptr<SessionDescription> offer =
      f1_.CreateOffer(offer_opts, nullptr);
  ASSERT_TRUE(offer);
  ASSERT_EQ(2u, offer->contents().size());
  ASSERT_FALSE(offer->contents()[0].rejected);
  ASSERT_FALSE(offer->contents()[1].rejected);

  // The answerer rejects one of the audio sections.
  MediaSessionOptions answer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &answer_opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio2",
                             RtpTransceiverDirection::kInactive, kStopped,
                             &answer_opts);
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), answer_opts, nullptr);
  ASSERT_EQ(2u, answer->contents().size());
  EXPECT_FALSE(answer->contents()[0].rejected);
  EXPECT_TRUE(answer->contents()[1].rejected);

  // The TransportInfo of the rejected m= section is expected to be added in the
  // answer.
  EXPECT_EQ(offer->transport_infos().size(), answer->transport_infos().size());
}

// Test the generated media sections has the same order of the
// corresponding MediaDescriptionOptions.
TEST_F(MediaSessionDescriptionFactoryTest,
       CreateOfferRespectsMediaDescriptionOptionsOrder) {
  MediaSessionOptions opts;
  // This tests put video section first because normally audio comes first by
  // default.
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);

  ASSERT_TRUE(offer);
  ASSERT_EQ(2u, offer->contents().size());
  EXPECT_EQ("video", offer->contents()[0].name);
  EXPECT_EQ("audio", offer->contents()[1].name);
}

// Test that different media sections using the same codec have same payload
// type.
TEST_F(MediaSessionDescriptionFactoryTest,
       PayloadTypesSharedByMediaSectionsOfSameType) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  // Create an offer with two video sections using same codecs.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer);
  ASSERT_EQ(2u, offer->contents().size());
  const VideoContentDescription* vcd1 =
      offer->contents()[0].media_description()->as_video();
  const VideoContentDescription* vcd2 =
      offer->contents()[1].media_description()->as_video();
  EXPECT_EQ(vcd1->codecs().size(), vcd2->codecs().size());
  ASSERT_EQ(2u, vcd1->codecs().size());
  EXPECT_EQ(vcd1->codecs()[0].name, vcd2->codecs()[0].name);
  EXPECT_EQ(vcd1->codecs()[0].id, vcd2->codecs()[0].id);
  EXPECT_EQ(vcd1->codecs()[1].name, vcd2->codecs()[1].name);
  EXPECT_EQ(vcd1->codecs()[1].id, vcd2->codecs()[1].id);

  // Create answer and negotiate the codecs.
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  ASSERT_TRUE(answer);
  ASSERT_EQ(2u, answer->contents().size());
  vcd1 = answer->contents()[0].media_description()->as_video();
  vcd2 = answer->contents()[1].media_description()->as_video();
  EXPECT_EQ(vcd1->codecs().size(), vcd2->codecs().size());
  ASSERT_EQ(1u, vcd1->codecs().size());
  EXPECT_EQ(vcd1->codecs()[0].name, vcd2->codecs()[0].name);
  EXPECT_EQ(vcd1->codecs()[0].id, vcd2->codecs()[0].id);
}

// Test that the codec preference order per media section is respected in
// subsequent offer.
TEST_F(MediaSessionDescriptionFactoryTest,
       CreateOfferRespectsCodecPreferenceOrder) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  // Create an offer with two video sections using same codecs.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer);
  ASSERT_EQ(2u, offer->contents().size());
  VideoContentDescription* vcd1 =
      offer->contents()[0].media_description()->as_video();
  const VideoContentDescription* vcd2 =
      offer->contents()[1].media_description()->as_video();
  auto video_codecs = MAKE_VECTOR(kVideoCodecs1);
  EXPECT_EQ(video_codecs, vcd1->codecs());
  EXPECT_EQ(video_codecs, vcd2->codecs());

  // Change the codec preference of the first video section and create a
  // follow-up offer.
  auto video_codecs_reverse = MAKE_VECTOR(kVideoCodecs1Reverse);
  vcd1->set_codecs(video_codecs_reverse);
  std::unique_ptr<SessionDescription> updated_offer(
      f1_.CreateOffer(opts, offer.get()));
  vcd1 = updated_offer->contents()[0].media_description()->as_video();
  vcd2 = updated_offer->contents()[1].media_description()->as_video();
  // The video codec preference order should be respected.
  EXPECT_EQ(video_codecs_reverse, vcd1->codecs());
  EXPECT_EQ(video_codecs, vcd2->codecs());
}

// Test that the codec preference order per media section is respected in
// the answer.
TEST_F(MediaSessionDescriptionFactoryTest,
       CreateAnswerRespectsCodecPreferenceOrder) {
  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video1",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video2",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  // Create an offer with two video sections using same codecs.
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer);
  ASSERT_EQ(2u, offer->contents().size());
  VideoContentDescription* vcd1 =
      offer->contents()[0].media_description()->as_video();
  const VideoContentDescription* vcd2 =
      offer->contents()[1].media_description()->as_video();
  auto video_codecs = MAKE_VECTOR(kVideoCodecs1);
  EXPECT_EQ(video_codecs, vcd1->codecs());
  EXPECT_EQ(video_codecs, vcd2->codecs());

  // Change the codec preference of the first video section and create an
  // answer.
  auto video_codecs_reverse = MAKE_VECTOR(kVideoCodecs1Reverse);
  vcd1->set_codecs(video_codecs_reverse);
  std::unique_ptr<SessionDescription> answer =
      f1_.CreateAnswer(offer.get(), opts, nullptr);
  vcd1 = answer->contents()[0].media_description()->as_video();
  vcd2 = answer->contents()[1].media_description()->as_video();
  // The video codec preference order should be respected.
  EXPECT_EQ(video_codecs_reverse, vcd1->codecs());
  EXPECT_EQ(video_codecs, vcd2->codecs());
}

// Test that when creating an answer, the codecs use local parameters instead of
// the remote ones.
TEST_F(MediaSessionDescriptionFactoryTest, CreateAnswerWithLocalCodecParams) {
  const std::string audio_param_name = "audio_param";
  const std::string audio_value1 = "audio_v1";
  const std::string audio_value2 = "audio_v2";
  const std::string video_param_name = "video_param";
  const std::string video_value1 = "video_v1";
  const std::string video_value2 = "video_v2";

  auto audio_codecs1 = MAKE_VECTOR(kAudioCodecs1);
  auto audio_codecs2 = MAKE_VECTOR(kAudioCodecs1);
  auto video_codecs1 = MAKE_VECTOR(kVideoCodecs1);
  auto video_codecs2 = MAKE_VECTOR(kVideoCodecs1);

  // Set the parameters for codecs.
  audio_codecs1[0].SetParam(audio_param_name, audio_value1);
  video_codecs1[0].SetParam(video_param_name, video_value1);
  audio_codecs2[0].SetParam(audio_param_name, audio_value2);
  video_codecs2[0].SetParam(video_param_name, video_value2);

  f1_.set_audio_codecs(audio_codecs1, audio_codecs1);
  f1_.set_video_codecs(video_codecs1, video_codecs1);
  f2_.set_audio_codecs(audio_codecs2, audio_codecs2);
  f2_.set_video_codecs(video_codecs2, video_codecs2);

  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer);
  auto offer_acd = offer->contents()[0].media_description()->as_audio();
  auto offer_vcd = offer->contents()[1].media_description()->as_video();
  std::string value;
  EXPECT_TRUE(offer_acd->codecs()[0].GetParam(audio_param_name, &value));
  EXPECT_EQ(audio_value1, value);
  EXPECT_TRUE(offer_vcd->codecs()[0].GetParam(video_param_name, &value));
  EXPECT_EQ(video_value1, value);

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  ASSERT_TRUE(answer);
  auto answer_acd = answer->contents()[0].media_description()->as_audio();
  auto answer_vcd = answer->contents()[1].media_description()->as_video();
  // Use the parameters from the local codecs.
  EXPECT_TRUE(answer_acd->codecs()[0].GetParam(audio_param_name, &value));
  EXPECT_EQ(audio_value2, value);
  EXPECT_TRUE(answer_vcd->codecs()[0].GetParam(video_param_name, &value));
  EXPECT_EQ(video_value2, value);
}

// Test that matching packetization-mode is part of the criteria for matching
// H264 codecs (in addition to profile-level-id). Previously, this was not the
// case, so the first H264 codec with the same profile-level-id would match and
// the payload type in the answer would be incorrect.
// This is a regression test for bugs.webrtc.org/8808
TEST_F(MediaSessionDescriptionFactoryTest,
       H264MatchCriteriaIncludesPacketizationMode) {
  // Create two H264 codecs with the same profile level ID and different
  // packetization modes.
  VideoCodec h264_pm0(96, "H264");
  h264_pm0.params[cricket::kH264FmtpProfileLevelId] = "42c01f";
  h264_pm0.params[cricket::kH264FmtpPacketizationMode] = "0";
  VideoCodec h264_pm1(97, "H264");
  h264_pm1.params[cricket::kH264FmtpProfileLevelId] = "42c01f";
  h264_pm1.params[cricket::kH264FmtpPacketizationMode] = "1";

  // Offerer will send both codecs, answerer should choose the one with matching
  // packetization mode (and not the first one it sees).
  f1_.set_video_codecs({h264_pm0, h264_pm1}, {h264_pm0, h264_pm1});
  f2_.set_video_codecs({h264_pm1}, {h264_pm1});

  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_VIDEO, "video",
                             RtpTransceiverDirection::kSendRecv, kActive,
                             &opts);

  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer);

  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  ASSERT_TRUE(answer);

  // Answer should have one negotiated codec with packetization-mode=1 using the
  // offered payload type.
  ASSERT_EQ(1u, answer->contents().size());
  auto answer_vcd = answer->contents()[0].media_description()->as_video();
  ASSERT_EQ(1u, answer_vcd->codecs().size());
  auto answer_codec = answer_vcd->codecs()[0];
  EXPECT_EQ(h264_pm1.id, answer_codec.id);
}

class MediaProtocolTest : public ::testing::TestWithParam<const char*> {
 public:
  MediaProtocolTest()
      : f1_(&tdf1_, &ssrc_generator1), f2_(&tdf2_, &ssrc_generator2) {
    f1_.set_audio_codecs(MAKE_VECTOR(kAudioCodecs1),
                         MAKE_VECTOR(kAudioCodecs1));
    f1_.set_video_codecs(MAKE_VECTOR(kVideoCodecs1),
                         MAKE_VECTOR(kVideoCodecs1));
    f2_.set_audio_codecs(MAKE_VECTOR(kAudioCodecs2),
                         MAKE_VECTOR(kAudioCodecs2));
    f2_.set_video_codecs(MAKE_VECTOR(kVideoCodecs2),
                         MAKE_VECTOR(kVideoCodecs2));
    f1_.set_secure(SEC_ENABLED);
    f2_.set_secure(SEC_ENABLED);
    tdf1_.set_certificate(rtc::RTCCertificate::Create(
        std::unique_ptr<rtc::SSLIdentity>(new rtc::FakeSSLIdentity("id1"))));
    tdf2_.set_certificate(rtc::RTCCertificate::Create(
        std::unique_ptr<rtc::SSLIdentity>(new rtc::FakeSSLIdentity("id2"))));
    tdf1_.set_secure(SEC_ENABLED);
    tdf2_.set_secure(SEC_ENABLED);
  }

 protected:
  MediaSessionDescriptionFactory f1_;
  MediaSessionDescriptionFactory f2_;
  TransportDescriptionFactory tdf1_;
  TransportDescriptionFactory tdf2_;
  UniqueRandomIdGenerator ssrc_generator1;
  UniqueRandomIdGenerator ssrc_generator2;
};

TEST_P(MediaProtocolTest, TestAudioVideoAcceptance) {
  MediaSessionOptions opts;
  AddAudioVideoSections(RtpTransceiverDirection::kRecvOnly, &opts);
  std::unique_ptr<SessionDescription> offer = f1_.CreateOffer(opts, nullptr);
  ASSERT_TRUE(offer.get() != nullptr);
  // Set the protocol for all the contents.
  for (auto& content : offer.get()->contents()) {
    content.media_description()->set_protocol(GetParam());
  }
  std::unique_ptr<SessionDescription> answer =
      f2_.CreateAnswer(offer.get(), opts, nullptr);
  const ContentInfo* ac = answer->GetContentByName("audio");
  const ContentInfo* vc = answer->GetContentByName("video");
  ASSERT_TRUE(ac != nullptr);
  ASSERT_TRUE(vc != nullptr);
  EXPECT_FALSE(ac->rejected);  // the offer is accepted
  EXPECT_FALSE(vc->rejected);
  const AudioContentDescription* acd = ac->media_description()->as_audio();
  const VideoContentDescription* vcd = vc->media_description()->as_video();
  EXPECT_EQ(GetParam(), acd->protocol());
  EXPECT_EQ(GetParam(), vcd->protocol());
}

INSTANTIATE_TEST_SUITE_P(MediaProtocolPatternTest,
                         MediaProtocolTest,
                         ::testing::ValuesIn(kMediaProtocols));
INSTANTIATE_TEST_SUITE_P(MediaProtocolDtlsPatternTest,
                         MediaProtocolTest,
                         ::testing::ValuesIn(kMediaProtocolsDtls));

TEST_F(MediaSessionDescriptionFactoryTest, TestSetAudioCodecs) {
  TransportDescriptionFactory tdf;
  UniqueRandomIdGenerator ssrc_generator;
  MediaSessionDescriptionFactory sf(&tdf, &ssrc_generator);
  std::vector<AudioCodec> send_codecs = MAKE_VECTOR(kAudioCodecs1);
  std::vector<AudioCodec> recv_codecs = MAKE_VECTOR(kAudioCodecs2);

  // The merged list of codecs should contain any send codecs that are also
  // nominally in the recieve codecs list. Payload types should be picked from
  // the send codecs and a number-of-channels of 0 and 1 should be equivalent
  // (set to 1). This equals what happens when the send codecs are used in an
  // offer and the receive codecs are used in the following answer.
  const std::vector<AudioCodec> sendrecv_codecs =
      MAKE_VECTOR(kAudioCodecsAnswer);
  const std::vector<AudioCodec> no_codecs;

  RTC_CHECK_EQ(send_codecs[1].name, "iLBC")
      << "Please don't change shared test data!";
  RTC_CHECK_EQ(recv_codecs[2].name, "iLBC")
      << "Please don't change shared test data!";
  // Alter iLBC send codec to have zero channels, to test that that is handled
  // properly.
  send_codecs[1].channels = 0;

  // Alter iLBC receive codec to be lowercase, to test that case conversions
  // are handled properly.
  recv_codecs[2].name = "ilbc";

  // Test proper merge
  sf.set_audio_codecs(send_codecs, recv_codecs);
  EXPECT_EQ(send_codecs, sf.audio_send_codecs());
  EXPECT_EQ(recv_codecs, sf.audio_recv_codecs());
  EXPECT_EQ(sendrecv_codecs, sf.audio_sendrecv_codecs());

  // Test empty send codecs list
  sf.set_audio_codecs(no_codecs, recv_codecs);
  EXPECT_EQ(no_codecs, sf.audio_send_codecs());
  EXPECT_EQ(recv_codecs, sf.audio_recv_codecs());
  EXPECT_EQ(no_codecs, sf.audio_sendrecv_codecs());

  // Test empty recv codecs list
  sf.set_audio_codecs(send_codecs, no_codecs);
  EXPECT_EQ(send_codecs, sf.audio_send_codecs());
  EXPECT_EQ(no_codecs, sf.audio_recv_codecs());
  EXPECT_EQ(no_codecs, sf.audio_sendrecv_codecs());

  // Test all empty codec lists
  sf.set_audio_codecs(no_codecs, no_codecs);
  EXPECT_EQ(no_codecs, sf.audio_send_codecs());
  EXPECT_EQ(no_codecs, sf.audio_recv_codecs());
  EXPECT_EQ(no_codecs, sf.audio_sendrecv_codecs());
}

namespace {
// Compare the two vectors of codecs ignoring the payload type.
template <class Codec>
bool CodecsMatch(const std::vector<Codec>& codecs1,
                 const std::vector<Codec>& codecs2) {
  if (codecs1.size() != codecs2.size()) {
    return false;
  }

  for (size_t i = 0; i < codecs1.size(); ++i) {
    if (!codecs1[i].Matches(codecs2[i])) {
      return false;
    }
  }
  return true;
}

void TestAudioCodecsOffer(RtpTransceiverDirection direction) {
  TransportDescriptionFactory tdf;
  UniqueRandomIdGenerator ssrc_generator;
  MediaSessionDescriptionFactory sf(&tdf, &ssrc_generator);
  const std::vector<AudioCodec> send_codecs = MAKE_VECTOR(kAudioCodecs1);
  const std::vector<AudioCodec> recv_codecs = MAKE_VECTOR(kAudioCodecs2);
  const std::vector<AudioCodec> sendrecv_codecs =
      MAKE_VECTOR(kAudioCodecsAnswer);
  sf.set_audio_codecs(send_codecs, recv_codecs);

  MediaSessionOptions opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio", direction, kActive,
                             &opts);

  if (direction == RtpTransceiverDirection::kSendRecv ||
      direction == RtpTransceiverDirection::kSendOnly) {
    AttachSenderToMediaDescriptionOptions(
        "audio", MEDIA_TYPE_AUDIO, kAudioTrack1, {kMediaStream1}, 1, &opts);
  }

  std::unique_ptr<SessionDescription> offer = sf.CreateOffer(opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);
  ContentInfo* ac = offer->GetContentByName("audio");

  // If the factory didn't add any audio content to the offer, we cannot check
  // that the codecs put in are right. This happens when we neither want to
  // send nor receive audio. The checks are still in place if at some point
  // we'd instead create an inactive stream.
  if (ac) {
    AudioContentDescription* acd = ac->media_description()->as_audio();
    // sendrecv and inactive should both present lists as if the channel was
    // to be used for sending and receiving. Inactive essentially means it
    // might eventually be used anything, but we don't know more at this
    // moment.
    if (acd->direction() == RtpTransceiverDirection::kSendOnly) {
      EXPECT_TRUE(CodecsMatch<AudioCodec>(send_codecs, acd->codecs()));
    } else if (acd->direction() == RtpTransceiverDirection::kRecvOnly) {
      EXPECT_TRUE(CodecsMatch<AudioCodec>(recv_codecs, acd->codecs()));
    } else {
      EXPECT_TRUE(CodecsMatch<AudioCodec>(sendrecv_codecs, acd->codecs()));
    }
  }
}

static const AudioCodec kOfferAnswerCodecs[] = {
    AudioCodec(0, "codec0", 16000, -1, 1),
    AudioCodec(1, "codec1", 8000, 13300, 1),
    AudioCodec(2, "codec2", 8000, 64000, 1),
    AudioCodec(3, "codec3", 8000, 64000, 1),
    AudioCodec(4, "codec4", 8000, 0, 2),
    AudioCodec(5, "codec5", 32000, 0, 1),
    AudioCodec(6, "codec6", 48000, 0, 1)};

/* The codecs groups below are chosen as per the matrix below. The objective
 * is to have different sets of codecs in the inputs, to get unique sets of
 * codecs after negotiation, depending on offer and answer communication
 * directions. One-way directions in the offer should either result in the
 * opposite direction in the answer, or an inactive answer. Regardless, the
 * choice of codecs should be as if the answer contained the opposite
 * direction. Inactive offers should be treated as sendrecv/sendrecv.
 *
 *         |     Offer   |      Answer  |         Result
 *    codec|send recv sr | send recv sr | s/r  r/s sr/s sr/r sr/sr
 *     0   | x    -    - |  -    x    - |  x    -    -    -    -
 *     1   | x    x    x |  -    x    - |  x    -    -    x    -
 *     2   | -    x    - |  x    -    - |  -    x    -    -    -
 *     3   | x    x    x |  x    -    - |  -    x    x    -    -
 *     4   | -    x    - |  x    x    x |  -    x    -    -    -
 *     5   | x    -    - |  x    x    x |  x    -    -    -    -
 *     6   | x    x    x |  x    x    x |  x    x    x    x    x
 */
// Codecs used by offerer in the AudioCodecsAnswerTest
static const int kOfferSendCodecs[] = {0, 1, 3, 5, 6};
static const int kOfferRecvCodecs[] = {1, 2, 3, 4, 6};
// Codecs used in the answerer in the AudioCodecsAnswerTest.  The order is
// jumbled to catch the answer not following the order in the offer.
static const int kAnswerSendCodecs[] = {6, 5, 2, 3, 4};
static const int kAnswerRecvCodecs[] = {6, 5, 4, 1, 0};
// The resulting sets of codecs in the answer in the AudioCodecsAnswerTest
static const int kResultSend_RecvCodecs[] = {0, 1, 5, 6};
static const int kResultRecv_SendCodecs[] = {2, 3, 4, 6};
static const int kResultSendrecv_SendCodecs[] = {3, 6};
static const int kResultSendrecv_RecvCodecs[] = {1, 6};
static const int kResultSendrecv_SendrecvCodecs[] = {6};

template <typename T, int IDXS>
std::vector<T> VectorFromIndices(const T* array, const int (&indices)[IDXS]) {
  std::vector<T> out;
  out.reserve(IDXS);
  for (int idx : indices)
    out.push_back(array[idx]);

  return out;
}

void TestAudioCodecsAnswer(RtpTransceiverDirection offer_direction,
                           RtpTransceiverDirection answer_direction,
                           bool add_legacy_stream) {
  TransportDescriptionFactory offer_tdf;
  TransportDescriptionFactory answer_tdf;
  UniqueRandomIdGenerator ssrc_generator1, ssrc_generator2;
  MediaSessionDescriptionFactory offer_factory(&offer_tdf, &ssrc_generator1);
  MediaSessionDescriptionFactory answer_factory(&answer_tdf, &ssrc_generator2);
  offer_factory.set_audio_codecs(
      VectorFromIndices(kOfferAnswerCodecs, kOfferSendCodecs),
      VectorFromIndices(kOfferAnswerCodecs, kOfferRecvCodecs));
  answer_factory.set_audio_codecs(
      VectorFromIndices(kOfferAnswerCodecs, kAnswerSendCodecs),
      VectorFromIndices(kOfferAnswerCodecs, kAnswerRecvCodecs));

  MediaSessionOptions offer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio", offer_direction,
                             kActive, &offer_opts);

  if (webrtc::RtpTransceiverDirectionHasSend(offer_direction)) {
    AttachSenderToMediaDescriptionOptions("audio", MEDIA_TYPE_AUDIO,
                                          kAudioTrack1, {kMediaStream1}, 1,
                                          &offer_opts);
  }

  std::unique_ptr<SessionDescription> offer =
      offer_factory.CreateOffer(offer_opts, NULL);
  ASSERT_TRUE(offer.get() != NULL);

  MediaSessionOptions answer_opts;
  AddMediaDescriptionOptions(MEDIA_TYPE_AUDIO, "audio", answer_direction,
                             kActive, &answer_opts);

  if (webrtc::RtpTransceiverDirectionHasSend(answer_direction)) {
    AttachSenderToMediaDescriptionOptions("audio", MEDIA_TYPE_AUDIO,
                                          kAudioTrack1, {kMediaStream1}, 1,
                                          &answer_opts);
  }
  std::unique_ptr<SessionDescription> answer =
      answer_factory.CreateAnswer(offer.get(), answer_opts, NULL);
  const ContentInfo* ac = answer->GetContentByName("audio");

  // If the factory didn't add any audio content to the answer, we cannot
  // check that the codecs put in are right. This happens when we neither want
  // to send nor receive audio. The checks are still in place if at some point
  // we'd instead create an inactive stream.
  if (ac) {
    ASSERT_EQ(MEDIA_TYPE_AUDIO, ac->media_description()->type());
    const AudioContentDescription* acd = ac->media_description()->as_audio();

    std::vector<AudioCodec> target_codecs;
    // For offers with sendrecv or inactive, we should never reply with more
    // codecs than offered, with these codec sets.
    switch (offer_direction) {
      case RtpTransceiverDirection::kInactive:
        target_codecs = VectorFromIndices(kOfferAnswerCodecs,
                                          kResultSendrecv_SendrecvCodecs);
        break;
      case RtpTransceiverDirection::kSendOnly:
        target_codecs =
            VectorFromIndices(kOfferAnswerCodecs, kResultSend_RecvCodecs);
        break;
      case RtpTransceiverDirection::kRecvOnly:
        target_codecs =
            VectorFromIndices(kOfferAnswerCodecs, kResultRecv_SendCodecs);
        break;
      case RtpTransceiverDirection::kSendRecv:
        if (acd->direction() == RtpTransceiverDirection::kSendOnly) {
          target_codecs =
              VectorFromIndices(kOfferAnswerCodecs, kResultSendrecv_SendCodecs);
        } else if (acd->direction() == RtpTransceiverDirection::kRecvOnly) {
          target_codecs =
              VectorFromIndices(kOfferAnswerCodecs, kResultSendrecv_RecvCodecs);
        } else {
          target_codecs = VectorFromIndices(kOfferAnswerCodecs,
                                            kResultSendrecv_SendrecvCodecs);
        }
        break;
      case RtpTransceiverDirection::kStopped:
        // This does not happen in any current test.
        RTC_NOTREACHED();
    }

    auto format_codecs = [](const std::vector<AudioCodec>& codecs) {
      rtc::StringBuilder os;
      bool first = true;
      os << "{";
      for (const auto& c : codecs) {
        os << (first ? " " : ", ") << c.id;
        first = false;
      }
      os << " }";
      return os.Release();
    };

    EXPECT_TRUE(acd->codecs() == target_codecs)
        << "Expected: " << format_codecs(target_codecs)
        << ", got: " << format_codecs(acd->codecs()) << "; Offered: "
        << webrtc::RtpTransceiverDirectionToString(offer_direction)
        << ", answerer wants: "
        << webrtc::RtpTransceiverDirectionToString(answer_direction)
        << "; got: "
        << webrtc::RtpTransceiverDirectionToString(acd->direction());
  } else {
    EXPECT_EQ(offer_direction, RtpTransceiverDirection::kInactive)
        << "Only inactive offers are allowed to not generate any audio "
           "content";
  }
}

}  // namespace

class AudioCodecsOfferTest
    : public ::testing::TestWithParam<RtpTransceiverDirection> {};

TEST_P(AudioCodecsOfferTest, TestCodecsInOffer) {
  TestAudioCodecsOffer(GetParam());
}

INSTANTIATE_TEST_SUITE_P(MediaSessionDescriptionFactoryTest,
                         AudioCodecsOfferTest,
                         ::testing::Values(RtpTransceiverDirection::kSendOnly,
                                           RtpTransceiverDirection::kRecvOnly,
                                           RtpTransceiverDirection::kSendRecv,
                                           RtpTransceiverDirection::kInactive));

class AudioCodecsAnswerTest
    : public ::testing::TestWithParam<::testing::tuple<RtpTransceiverDirection,
                                                       RtpTransceiverDirection,
                                                       bool>> {};

TEST_P(AudioCodecsAnswerTest, TestCodecsInAnswer) {
  TestAudioCodecsAnswer(::testing::get<0>(GetParam()),
                        ::testing::get<1>(GetParam()),
                        ::testing::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
    MediaSessionDescriptionFactoryTest,
    AudioCodecsAnswerTest,
    ::testing::Combine(::testing::Values(RtpTransceiverDirection::kSendOnly,
                                         RtpTransceiverDirection::kRecvOnly,
                                         RtpTransceiverDirection::kSendRecv,
                                         RtpTransceiverDirection::kInactive),
                       ::testing::Values(RtpTransceiverDirection::kSendOnly,
                                         RtpTransceiverDirection::kRecvOnly,
                                         RtpTransceiverDirection::kSendRecv,
                                         RtpTransceiverDirection::kInactive),
                       ::testing::Bool()));
