/*
 *  Copyright 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__stats_collector_h //original-code:"pc/stats_collector.h"

#include <stdio.h>

#include <memory>

#include BOSS_ABSEILCPP_U_absl__algorithm__container_h //original-code:"absl/algorithm/container.h"
#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_h //original-code:"api/audio_codecs/audio_encoder.h"
#include BOSS_WEBRTC_U_api__candidate_h //original-code:"api/candidate.h"
#include BOSS_WEBRTC_U_api__data_channel_interface_h //original-code:"api/data_channel_interface.h"
#include BOSS_WEBRTC_U_api__media_stream_track_h //original-code:"api/media_stream_track.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_call__call_h //original-code:"call/call.h"
#include BOSS_WEBRTC_U_media__base__media_channel_h //original-code:"media/base/media_channel.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_statistics_h //original-code:"modules/audio_processing/include/audio_processing_statistics.h"
#include BOSS_WEBRTC_U_pc__media_stream_h //original-code:"pc/media_stream.h"
#include BOSS_WEBRTC_U_pc__sctp_data_channel_h //original-code:"pc/sctp_data_channel.h"
#include "pc/test/fake_peer_connection_for_stats.h"
#include "pc/test/fake_video_track_source.h"
#include "pc/test/mock_rtp_receiver_internal.h"
#include "pc/test/mock_rtp_sender_internal.h"
#include BOSS_WEBRTC_U_pc__transport_stats_h //original-code:"pc/transport_stats.h"
#include BOSS_WEBRTC_U_pc__video_track_h //original-code:"pc/video_track.h"
#include BOSS_WEBRTC_U_rtc_base__fake_ssl_identity_h //original-code:"rtc_base/fake_ssl_identity.h"
#include BOSS_WEBRTC_U_rtc_base__message_digest_h //original-code:"rtc_base/message_digest.h"
#include BOSS_WEBRTC_U_rtc_base__net_helper_h //original-code:"rtc_base/net_helper.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__rtc_certificate_h //original-code:"rtc_base/rtc_certificate.h"
#include BOSS_WEBRTC_U_rtc_base__socket_address_h //original-code:"rtc_base/socket_address.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_identity_h //original-code:"rtc_base/ssl_identity.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_stream_adapter_h //original-code:"rtc_base/ssl_stream_adapter.h"
#include BOSS_WEBRTC_U_rtc_base__string_encode_h //original-code:"rtc_base/string_encode.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__base64__base64_h //original-code:"rtc_base/third_party/base64/base64.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

using cricket::ConnectionInfo;
using cricket::SsrcReceiverInfo;
using cricket::TransportChannelStats;
using cricket::VideoMediaInfo;
using cricket::VideoReceiverInfo;
using cricket::VideoSenderInfo;
using cricket::VoiceMediaInfo;
using cricket::VoiceReceiverInfo;
using cricket::VoiceSenderInfo;
using ::testing::Return;
using ::testing::UnorderedElementsAre;

namespace webrtc {

namespace internal {
// This value comes from openssl/tls1.h
static const int TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA = 0xC014;
}  // namespace internal

// Error return values
const char kNotFound[] = "NOT FOUND";

// Constant names for track identification.
const char kLocalTrackId[] = "local_track_id";
const char kRemoteTrackId[] = "remote_track_id";
const uint32_t kSsrcOfTrack = 1234;

class FakeAudioProcessor : public AudioProcessorInterface {
 public:
  FakeAudioProcessor() {}
  ~FakeAudioProcessor() {}

 private:
  AudioProcessorInterface::AudioProcessorStatistics GetStats(
      bool has_recv_streams) override {
    AudioProcessorStatistics stats;
    stats.typing_noise_detected = true;
    if (has_recv_streams) {
      stats.apm_statistics.echo_return_loss = 2.0;
      stats.apm_statistics.echo_return_loss_enhancement = 3.0;
      stats.apm_statistics.delay_median_ms = 4;
      stats.apm_statistics.delay_standard_deviation_ms = 5;
    }
    return stats;
  }
};

class FakeAudioTrack : public MediaStreamTrack<AudioTrackInterface> {
 public:
  explicit FakeAudioTrack(const std::string& id)
      : MediaStreamTrack<AudioTrackInterface>(id),
        processor_(rtc::make_ref_counted<FakeAudioProcessor>()) {}
  std::string kind() const override { return "audio"; }
  AudioSourceInterface* GetSource() const override { return NULL; }
  void AddSink(AudioTrackSinkInterface* sink) override {}
  void RemoveSink(AudioTrackSinkInterface* sink) override {}
  bool GetSignalLevel(int* level) override {
    *level = 1;
    return true;
  }
  rtc::scoped_refptr<AudioProcessorInterface> GetAudioProcessor() override {
    return processor_;
  }

 private:
  rtc::scoped_refptr<FakeAudioProcessor> processor_;
};

// This fake audio processor is used to verify that the undesired initial values
// (-1) will be filtered out.
class FakeAudioProcessorWithInitValue : public AudioProcessorInterface {
 public:
  FakeAudioProcessorWithInitValue() {}
  ~FakeAudioProcessorWithInitValue() {}

 private:
  AudioProcessorInterface::AudioProcessorStatistics GetStats(
      bool /*has_recv_streams*/) override {
    AudioProcessorStatistics stats;
    stats.typing_noise_detected = false;
    return stats;
  }
};

class FakeAudioTrackWithInitValue
    : public MediaStreamTrack<AudioTrackInterface> {
 public:
  explicit FakeAudioTrackWithInitValue(const std::string& id)
      : MediaStreamTrack<AudioTrackInterface>(id),
        processor_(rtc::make_ref_counted<FakeAudioProcessorWithInitValue>()) {}
  std::string kind() const override { return "audio"; }
  AudioSourceInterface* GetSource() const override { return NULL; }
  void AddSink(AudioTrackSinkInterface* sink) override {}
  void RemoveSink(AudioTrackSinkInterface* sink) override {}
  bool GetSignalLevel(int* level) override {
    *level = 1;
    return true;
  }
  rtc::scoped_refptr<AudioProcessorInterface> GetAudioProcessor() override {
    return processor_;
  }

 private:
  rtc::scoped_refptr<FakeAudioProcessorWithInitValue> processor_;
};

bool GetValue(const StatsReport* report,
              StatsReport::StatsValueName name,
              std::string* value) {
  const StatsReport::Value* v = report->FindValue(name);
  if (!v)
    return false;
  *value = v->ToString();
  return true;
}

std::string ExtractStatsValue(const StatsReport::StatsType& type,
                              const StatsReports& reports,
                              StatsReport::StatsValueName name) {
  for (const auto* r : reports) {
    std::string ret;
    if (r->type() == type && GetValue(r, name, &ret))
      return ret;
  }

  return kNotFound;
}

StatsReport::Id TypedIdFromIdString(StatsReport::StatsType type,
                                    const std::string& value) {
  EXPECT_FALSE(value.empty());
  StatsReport::Id id;
  if (value.empty())
    return id;

  // This has assumptions about how the ID is constructed.  As is, this is
  // OK since this is for testing purposes only, but if we ever need this
  // in production, we should add a generic method that does this.
  size_t index = value.find('_');
  EXPECT_NE(index, std::string::npos);
  if (index == std::string::npos || index == (value.length() - 1))
    return id;

  id = StatsReport::NewTypedId(type, value.substr(index + 1));
  EXPECT_EQ(id->ToString(), value);
  return id;
}

StatsReport::Id IdFromCertIdString(const std::string& cert_id) {
  return TypedIdFromIdString(StatsReport::kStatsReportTypeCertificate, cert_id);
}

// Finds the |n|-th report of type |type| in |reports|.
// |n| starts from 1 for finding the first report.
const StatsReport* FindNthReportByType(const StatsReports& reports,
                                       const StatsReport::StatsType& type,
                                       int n) {
  for (size_t i = 0; i < reports.size(); ++i) {
    if (reports[i]->type() == type) {
      n--;
      if (n == 0)
        return reports[i];
    }
  }
  return nullptr;
}

// Returns the value of the stat identified by |name| in the |n|-th report of
// type |type| in |reports|.
// |n| starts from 1 for finding the first report.
// If either the |n|-th report is not found, or the stat is not present in that
// report, then nullopt is returned.
absl::optional<std::string> GetValueInNthReportByType(
    const StatsReports& reports,
    StatsReport::StatsType type,
    StatsReport::StatsValueName name,
    int n) {
  const StatsReport* report = FindNthReportByType(reports, type, n);
  if (!report) {
    return absl::nullopt;
  }
  std::string value;
  if (!GetValue(report, name, &value)) {
    return absl::nullopt;
  }
  return value;
}

std::vector<const StatsReport*> GetReportsByType(const StatsReports& reports,
                                                 StatsReport::StatsType type) {
  std::vector<const StatsReport*> filtered_reports;
  for (const StatsReport* report : reports) {
    if (report->type() == type) {
      filtered_reports.push_back(report);
    }
  }
  return filtered_reports;
}

const StatsReport* FindReportById(const StatsReports& reports,
                                  const StatsReport::Id& id) {
  for (const auto* r : reports) {
    if (r->id()->Equals(id))
      return r;
  }
  return nullptr;
}

std::string ExtractSsrcStatsValue(const StatsReports& reports,
                                  StatsReport::StatsValueName name) {
  return ExtractStatsValue(StatsReport::kStatsReportTypeSsrc, reports, name);
}

std::string ExtractBweStatsValue(const StatsReports& reports,
                                 StatsReport::StatsValueName name) {
  return ExtractStatsValue(StatsReport::kStatsReportTypeBwe, reports, name);
}

std::string DerToPem(const std::string& der) {
  return rtc::SSLIdentity::DerToPem(
      rtc::kPemTypeCertificate,
      reinterpret_cast<const unsigned char*>(der.c_str()), der.length());
}

std::vector<std::string> DersToPems(const std::vector<std::string>& ders) {
  std::vector<std::string> pems(ders.size());
  absl::c_transform(ders, pems.begin(), DerToPem);
  return pems;
}

void CheckCertChainReports(const StatsReports& reports,
                           const std::vector<std::string>& ders,
                           const StatsReport::Id& start_id) {
  StatsReport::Id cert_id;
  const StatsReport::Id* certificate_id = &start_id;
  size_t i = 0;
  while (true) {
    const StatsReport* report = FindReportById(reports, *certificate_id);
    ASSERT_TRUE(report != NULL);

    std::string der_base64;
    EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameDer, &der_base64));
    std::string der = rtc::Base64::Decode(der_base64, rtc::Base64::DO_STRICT);
    EXPECT_EQ(ders[i], der);

    std::string fingerprint_algorithm;
    EXPECT_TRUE(GetValue(report,
                         StatsReport::kStatsValueNameFingerprintAlgorithm,
                         &fingerprint_algorithm));
    // The digest algorithm for a FakeSSLCertificate is always SHA-1.
    std::string sha_1_str = rtc::DIGEST_SHA_1;
    EXPECT_EQ(sha_1_str, fingerprint_algorithm);

    std::string fingerprint;
    EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameFingerprint,
                         &fingerprint));
    EXPECT_FALSE(fingerprint.empty());

    ++i;
    std::string issuer_id;
    if (!GetValue(report, StatsReport::kStatsValueNameIssuerId, &issuer_id)) {
      break;
    }

    cert_id = IdFromCertIdString(issuer_id);
    certificate_id = &cert_id;
  }
  EXPECT_EQ(ders.size(), i);
}

void VerifyVoiceReceiverInfoReport(const StatsReport* report,
                                   const cricket::VoiceReceiverInfo& info) {
  std::string value_in_report;
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameAudioOutputLevel,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.audio_level), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameBytesReceived,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.payload_bytes_rcvd +
                          info.header_and_padding_bytes_rcvd),
            value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameJitterReceived,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.jitter_ms), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameJitterBufferMs,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.jitter_buffer_ms), value_in_report);
  EXPECT_TRUE(GetValue(report,
                       StatsReport::kStatsValueNamePreferredJitterBufferMs,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.jitter_buffer_preferred_ms), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameCurrentDelayMs,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.delay_estimate_ms), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameExpandRate,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.expand_rate), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameSpeechExpandRate,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.speech_expand_rate), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameAccelerateRate,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.accelerate_rate), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNamePreemptiveExpandRate,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.preemptive_expand_rate), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameSecondaryDecodedRate,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.secondary_decoded_rate), value_in_report);
  EXPECT_TRUE(GetValue(report,
                       StatsReport::kStatsValueNameSecondaryDiscardedRate,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.secondary_discarded_rate), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNamePacketsReceived,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.packets_rcvd), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameDecodingCTSG,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.decoding_calls_to_silence_generator),
            value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameDecodingCTN,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.decoding_calls_to_neteq), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameDecodingNormal,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.decoding_normal), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameDecodingPLC,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.decoding_plc), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameDecodingCodecPLC,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.decoding_codec_plc), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameDecodingCNG,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.decoding_cng), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameDecodingPLCCNG,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.decoding_plc_cng), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameDecodingMutedOutput,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(info.decoding_muted_output), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameCodecName,
                       &value_in_report));
}

void VerifyVoiceSenderInfoReport(const StatsReport* report,
                                 const cricket::VoiceSenderInfo& sinfo) {
  std::string value_in_report;
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameCodecName,
                       &value_in_report));
  EXPECT_EQ(sinfo.codec_name, value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameBytesSent,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(sinfo.payload_bytes_sent +
                          sinfo.header_and_padding_bytes_sent),
            value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNamePacketsSent,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(sinfo.packets_sent), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNamePacketsLost,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(sinfo.packets_lost), value_in_report);
  EXPECT_TRUE(
      GetValue(report, StatsReport::kStatsValueNameRtt, &value_in_report));
  EXPECT_EQ(rtc::ToString(sinfo.rtt_ms), value_in_report);
  EXPECT_TRUE(
      GetValue(report, StatsReport::kStatsValueNameRtt, &value_in_report));
  EXPECT_EQ(rtc::ToString(sinfo.rtt_ms), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameJitterReceived,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(sinfo.jitter_ms), value_in_report);
  if (sinfo.apm_statistics.delay_median_ms) {
    EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameEchoDelayMedian,
                         &value_in_report));
    EXPECT_EQ(rtc::ToString(*sinfo.apm_statistics.delay_median_ms),
              value_in_report);
  } else {
    EXPECT_FALSE(GetValue(report, StatsReport::kStatsValueNameEchoDelayMedian,
                          &value_in_report));
  }
  if (sinfo.apm_statistics.delay_standard_deviation_ms) {
    EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameEchoDelayStdDev,
                         &value_in_report));
    EXPECT_EQ(rtc::ToString(*sinfo.apm_statistics.delay_standard_deviation_ms),
              value_in_report);
  } else {
    EXPECT_FALSE(GetValue(report, StatsReport::kStatsValueNameEchoDelayStdDev,
                          &value_in_report));
  }
  if (sinfo.apm_statistics.echo_return_loss) {
    EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameEchoReturnLoss,
                         &value_in_report));
    EXPECT_EQ(rtc::ToString(*sinfo.apm_statistics.echo_return_loss),
              value_in_report);
  } else {
    EXPECT_FALSE(GetValue(report, StatsReport::kStatsValueNameEchoReturnLoss,
                          &value_in_report));
  }
  if (sinfo.apm_statistics.echo_return_loss_enhancement) {
    EXPECT_TRUE(GetValue(report,
                         StatsReport::kStatsValueNameEchoReturnLossEnhancement,
                         &value_in_report));
    EXPECT_EQ(rtc::ToString(*sinfo.apm_statistics.echo_return_loss_enhancement),
              value_in_report);
  } else {
    EXPECT_FALSE(GetValue(report,
                          StatsReport::kStatsValueNameEchoReturnLossEnhancement,
                          &value_in_report));
  }
  if (sinfo.apm_statistics.residual_echo_likelihood) {
    EXPECT_TRUE(GetValue(report,
                         StatsReport::kStatsValueNameResidualEchoLikelihood,
                         &value_in_report));
    EXPECT_EQ(rtc::ToString(*sinfo.apm_statistics.residual_echo_likelihood),
              value_in_report);
  } else {
    EXPECT_FALSE(GetValue(report,
                          StatsReport::kStatsValueNameResidualEchoLikelihood,
                          &value_in_report));
  }
  if (sinfo.apm_statistics.residual_echo_likelihood_recent_max) {
    EXPECT_TRUE(GetValue(
        report, StatsReport::kStatsValueNameResidualEchoLikelihoodRecentMax,
        &value_in_report));
    EXPECT_EQ(rtc::ToString(
                  *sinfo.apm_statistics.residual_echo_likelihood_recent_max),
              value_in_report);
  } else {
    EXPECT_FALSE(GetValue(
        report, StatsReport::kStatsValueNameResidualEchoLikelihoodRecentMax,
        &value_in_report));
  }
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameAudioInputLevel,
                       &value_in_report));
  EXPECT_EQ(rtc::ToString(sinfo.audio_level), value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameTypingNoiseState,
                       &value_in_report));
  std::string typing_detected = sinfo.typing_noise_detected ? "true" : "false";
  EXPECT_EQ(typing_detected, value_in_report);
  EXPECT_TRUE(GetValue(report,
                       StatsReport::kStatsValueNameAnaBitrateActionCounter,
                       &value_in_report));
  ASSERT_TRUE(sinfo.ana_statistics.bitrate_action_counter);
  EXPECT_EQ(rtc::ToString(*sinfo.ana_statistics.bitrate_action_counter),
            value_in_report);
  EXPECT_TRUE(GetValue(report,
                       StatsReport::kStatsValueNameAnaChannelActionCounter,
                       &value_in_report));
  ASSERT_TRUE(sinfo.ana_statistics.channel_action_counter);
  EXPECT_EQ(rtc::ToString(*sinfo.ana_statistics.channel_action_counter),
            value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameAnaDtxActionCounter,
                       &value_in_report));
  ASSERT_TRUE(sinfo.ana_statistics.dtx_action_counter);
  EXPECT_EQ(rtc::ToString(*sinfo.ana_statistics.dtx_action_counter),
            value_in_report);
  EXPECT_TRUE(GetValue(report, StatsReport::kStatsValueNameAnaFecActionCounter,
                       &value_in_report));
  ASSERT_TRUE(sinfo.ana_statistics.fec_action_counter);
  EXPECT_EQ(rtc::ToString(*sinfo.ana_statistics.fec_action_counter),
            value_in_report);
  EXPECT_TRUE(GetValue(
      report, StatsReport::kStatsValueNameAnaFrameLengthIncreaseCounter,
      &value_in_report));
  ASSERT_TRUE(sinfo.ana_statistics.frame_length_increase_counter);
  EXPECT_EQ(rtc::ToString(*sinfo.ana_statistics.frame_length_increase_counter),
            value_in_report);
  EXPECT_TRUE(GetValue(
      report, StatsReport::kStatsValueNameAnaFrameLengthDecreaseCounter,
      &value_in_report));
  ASSERT_TRUE(sinfo.ana_statistics.frame_length_decrease_counter);
  EXPECT_EQ(rtc::ToString(*sinfo.ana_statistics.frame_length_decrease_counter),
            value_in_report);
  EXPECT_TRUE(GetValue(report,
                       StatsReport::kStatsValueNameAnaUplinkPacketLossFraction,
                       &value_in_report));
  ASSERT_TRUE(sinfo.ana_statistics.uplink_packet_loss_fraction);
  EXPECT_EQ(rtc::ToString(*sinfo.ana_statistics.uplink_packet_loss_fraction),
            value_in_report);
}

// Helper methods to avoid duplication of code.
void InitVoiceSenderInfo(cricket::VoiceSenderInfo* voice_sender_info,
                         uint32_t ssrc = kSsrcOfTrack) {
  voice_sender_info->add_ssrc(ssrc);
  voice_sender_info->codec_name = "fake_codec";
  voice_sender_info->payload_bytes_sent = 88;
  voice_sender_info->header_and_padding_bytes_sent = 12;
  voice_sender_info->packets_sent = 101;
  voice_sender_info->rtt_ms = 102;
  voice_sender_info->fraction_lost = 103;
  voice_sender_info->jitter_ms = 104;
  voice_sender_info->packets_lost = 105;
  voice_sender_info->audio_level = 107;
  voice_sender_info->apm_statistics.echo_return_loss = 108;
  voice_sender_info->apm_statistics.echo_return_loss_enhancement = 109;
  voice_sender_info->apm_statistics.delay_median_ms = 110;
  voice_sender_info->apm_statistics.delay_standard_deviation_ms = 111;
  voice_sender_info->typing_noise_detected = false;
  voice_sender_info->ana_statistics.bitrate_action_counter = 112;
  voice_sender_info->ana_statistics.channel_action_counter = 113;
  voice_sender_info->ana_statistics.dtx_action_counter = 114;
  voice_sender_info->ana_statistics.fec_action_counter = 115;
  voice_sender_info->ana_statistics.frame_length_increase_counter = 116;
  voice_sender_info->ana_statistics.frame_length_decrease_counter = 117;
  voice_sender_info->ana_statistics.uplink_packet_loss_fraction = 118.0;
}

void UpdateVoiceSenderInfoFromAudioTrack(
    AudioTrackInterface* audio_track,
    cricket::VoiceSenderInfo* voice_sender_info,
    bool has_remote_tracks) {
  audio_track->GetSignalLevel(&voice_sender_info->audio_level);
  AudioProcessorInterface::AudioProcessorStatistics audio_processor_stats =
      audio_track->GetAudioProcessor()->GetStats(has_remote_tracks);
  voice_sender_info->typing_noise_detected =
      audio_processor_stats.typing_noise_detected;
  voice_sender_info->apm_statistics = audio_processor_stats.apm_statistics;
}

void InitVoiceReceiverInfo(cricket::VoiceReceiverInfo* voice_receiver_info) {
  voice_receiver_info->add_ssrc(kSsrcOfTrack);
  voice_receiver_info->payload_bytes_rcvd = 98;
  voice_receiver_info->header_and_padding_bytes_rcvd = 12;
  voice_receiver_info->packets_rcvd = 111;
  voice_receiver_info->packets_lost = 114;
  voice_receiver_info->jitter_ms = 116;
  voice_receiver_info->jitter_buffer_ms = 117;
  voice_receiver_info->jitter_buffer_preferred_ms = 118;
  voice_receiver_info->delay_estimate_ms = 119;
  voice_receiver_info->audio_level = 120;
  voice_receiver_info->expand_rate = 121;
  voice_receiver_info->speech_expand_rate = 122;
  voice_receiver_info->secondary_decoded_rate = 123;
  voice_receiver_info->accelerate_rate = 124;
  voice_receiver_info->preemptive_expand_rate = 125;
  voice_receiver_info->secondary_discarded_rate = 126;
  voice_receiver_info->decoding_codec_plc = 127;
}

class StatsCollectorForTest : public StatsCollector {
 public:
  explicit StatsCollectorForTest(PeerConnectionInternal* pc)
      : StatsCollector(pc), time_now_(19477) {}

  double GetTimeNow() override { return time_now_; }

 private:
  double time_now_;
};

class StatsCollectorTest : public ::testing::Test {
 protected:
  rtc::scoped_refptr<FakePeerConnectionForStats> CreatePeerConnection() {
    return rtc::make_ref_counted<FakePeerConnectionForStats>();
  }

  std::unique_ptr<StatsCollectorForTest> CreateStatsCollector(
      PeerConnectionInternal* pc) {
    return std::make_unique<StatsCollectorForTest>(pc);
  }

  void VerifyAudioTrackStats(FakeAudioTrack* audio_track,
                             StatsCollectorForTest* stats,
                             const VoiceMediaInfo& voice_info,
                             StatsReports* reports) {
    stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
    stats->ClearUpdateStatsCacheForTest();
    stats->GetStats(nullptr, reports);

    // Verify the existence of the track report.
    const StatsReport* report =
        FindNthReportByType(*reports, StatsReport::kStatsReportTypeSsrc, 1);
    ASSERT_TRUE(report);
    EXPECT_EQ(stats->GetTimeNow(), report->timestamp());
    std::string track_id =
        ExtractSsrcStatsValue(*reports, StatsReport::kStatsValueNameTrackId);
    EXPECT_EQ(audio_track->id(), track_id);
    std::string ssrc_id =
        ExtractSsrcStatsValue(*reports, StatsReport::kStatsValueNameSsrc);
    EXPECT_EQ(rtc::ToString(kSsrcOfTrack), ssrc_id);

    std::string media_type =
        ExtractSsrcStatsValue(*reports, StatsReport::kStatsValueNameMediaType);
    EXPECT_EQ("audio", media_type);

    // Verifies the values in the track report.
    if (!voice_info.senders.empty()) {
      VerifyVoiceSenderInfoReport(report, voice_info.senders[0]);
    }
    if (!voice_info.receivers.empty()) {
      VerifyVoiceReceiverInfoReport(report, voice_info.receivers[0]);
    }

    // Verify we get the same result by passing a track to GetStats().
    StatsReports track_reports;  // returned values.
    stats->GetStats(audio_track, &track_reports);
    const StatsReport* track_report = FindNthReportByType(
        track_reports, StatsReport::kStatsReportTypeSsrc, 1);
    ASSERT_TRUE(track_report);
    EXPECT_EQ(stats->GetTimeNow(), track_report->timestamp());
    track_id = ExtractSsrcStatsValue(track_reports,
                                     StatsReport::kStatsValueNameTrackId);
    EXPECT_EQ(audio_track->id(), track_id);
    ssrc_id =
        ExtractSsrcStatsValue(track_reports, StatsReport::kStatsValueNameSsrc);
    EXPECT_EQ(rtc::ToString(kSsrcOfTrack), ssrc_id);
    if (!voice_info.senders.empty()) {
      VerifyVoiceSenderInfoReport(track_report, voice_info.senders[0]);
    }
    if (!voice_info.receivers.empty()) {
      VerifyVoiceReceiverInfoReport(track_report, voice_info.receivers[0]);
    }
  }

  void TestCertificateReports(const rtc::FakeSSLIdentity& local_identity,
                              const std::vector<std::string>& local_ders,
                              const rtc::FakeSSLIdentity& remote_identity,
                              const std::vector<std::string>& remote_ders) {
    const std::string kTransportName = "transport";

    auto pc = CreatePeerConnection();
    auto stats = CreateStatsCollector(pc);

    pc->AddVoiceChannel("audio", kTransportName);

    // Fake stats to process.
    TransportChannelStats channel_stats;
    channel_stats.component = 1;
    channel_stats.srtp_crypto_suite = rtc::SRTP_AES128_CM_SHA1_80;
    channel_stats.ssl_cipher_suite =
        internal::TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA;
    pc->SetTransportStats(kTransportName, channel_stats);

    // Fake certificate to report.
    rtc::scoped_refptr<rtc::RTCCertificate> local_certificate(
        rtc::RTCCertificate::Create(local_identity.Clone()));
    pc->SetLocalCertificate(kTransportName, local_certificate);
    pc->SetRemoteCertChain(kTransportName,
                           remote_identity.cert_chain().Clone());

    stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);

    StatsReports reports;
    stats->GetStats(nullptr, &reports);

    const StatsReport* channel_report =
        FindNthReportByType(reports, StatsReport::kStatsReportTypeComponent, 1);
    EXPECT_TRUE(channel_report);

    // Check local certificate chain.
    std::string local_certificate_id =
        ExtractStatsValue(StatsReport::kStatsReportTypeComponent, reports,
                          StatsReport::kStatsValueNameLocalCertificateId);
    if (local_ders.size() > 0) {
      EXPECT_NE(kNotFound, local_certificate_id);
      StatsReport::Id id(IdFromCertIdString(local_certificate_id));
      CheckCertChainReports(reports, local_ders, id);
    } else {
      EXPECT_EQ(kNotFound, local_certificate_id);
    }

    // Check remote certificate chain.
    std::string remote_certificate_id =
        ExtractStatsValue(StatsReport::kStatsReportTypeComponent, reports,
                          StatsReport::kStatsValueNameRemoteCertificateId);
    if (remote_ders.size() > 0) {
      EXPECT_NE(kNotFound, remote_certificate_id);
      StatsReport::Id id(IdFromCertIdString(remote_certificate_id));
      CheckCertChainReports(reports, remote_ders, id);
    } else {
      EXPECT_EQ(kNotFound, remote_certificate_id);
    }

    // Check negotiated ciphers.
    std::string dtls_cipher_suite =
        ExtractStatsValue(StatsReport::kStatsReportTypeComponent, reports,
                          StatsReport::kStatsValueNameDtlsCipher);
    EXPECT_EQ(rtc::SSLStreamAdapter::SslCipherSuiteToName(
                  internal::TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA),
              dtls_cipher_suite);
    std::string srtp_crypto_suite =
        ExtractStatsValue(StatsReport::kStatsReportTypeComponent, reports,
                          StatsReport::kStatsValueNameSrtpCipher);
    EXPECT_EQ(rtc::SrtpCryptoSuiteToName(rtc::SRTP_AES128_CM_SHA1_80),
              srtp_crypto_suite);
  }
};

static rtc::scoped_refptr<MockRtpSenderInternal> CreateMockSender(
    rtc::scoped_refptr<MediaStreamTrackInterface> track,
    uint32_t ssrc) {
  auto sender = rtc::make_ref_counted<MockRtpSenderInternal>();
  EXPECT_CALL(*sender, track()).WillRepeatedly(Return(track));
  EXPECT_CALL(*sender, ssrc()).WillRepeatedly(Return(ssrc));
  EXPECT_CALL(*sender, media_type())
      .WillRepeatedly(
          Return(track->kind() == MediaStreamTrackInterface::kAudioKind
                     ? cricket::MEDIA_TYPE_AUDIO
                     : cricket::MEDIA_TYPE_VIDEO));
  return sender;
}

static rtc::scoped_refptr<MockRtpReceiverInternal> CreateMockReceiver(
    rtc::scoped_refptr<MediaStreamTrackInterface> track,
    uint32_t ssrc) {
  auto receiver = rtc::make_ref_counted<MockRtpReceiverInternal>();
  EXPECT_CALL(*receiver, track()).WillRepeatedly(Return(track));
  EXPECT_CALL(*receiver, ssrc()).WillRepeatedly(Return(ssrc));
  EXPECT_CALL(*receiver, media_type())
      .WillRepeatedly(
          Return(track->kind() == MediaStreamTrackInterface::kAudioKind
                     ? cricket::MEDIA_TYPE_AUDIO
                     : cricket::MEDIA_TYPE_VIDEO));
  return receiver;
}

class StatsCollectorTrackTest : public StatsCollectorTest,
                                public ::testing::WithParamInterface<bool> {
 public:
  // Adds a outgoing video track with a given SSRC into the stats.
  // If GetParam() returns true, the track is also inserted into the local
  // stream, which is created if necessary.
  void AddOutgoingVideoTrack(FakePeerConnectionForStats* pc,
                             StatsCollectorForTest* stats) {
    track_ = VideoTrack::Create(kLocalTrackId, FakeVideoTrackSource::Create(),
                                rtc::Thread::Current());
    if (GetParam()) {
      if (!stream_)
        stream_ = MediaStream::Create("streamid");
      stream_->AddTrack(track_);
      stats->AddStream(stream_);
    } else {
      stats->AddTrack(track_);
    }
    pc->AddSender(CreateMockSender(track_, kSsrcOfTrack));
  }

  // Adds a incoming video track with a given SSRC into the stats.
  void AddIncomingVideoTrack(FakePeerConnectionForStats* pc,
                             StatsCollectorForTest* stats) {
    track_ = VideoTrack::Create(kRemoteTrackId, FakeVideoTrackSource::Create(),
                                rtc::Thread::Current());
    if (GetParam()) {
      stream_ = MediaStream::Create("streamid");
      stream_->AddTrack(track_);
      stats->AddStream(stream_);
    } else {
      stats->AddTrack(track_);
    }
    pc->AddReceiver(CreateMockReceiver(track_, kSsrcOfTrack));
  }

  // Adds a outgoing audio track with a given SSRC into the stats,
  // and register it into the stats object.
  // If GetParam() returns true, the track is also inserted into the local
  // stream, which is created if necessary.
  rtc::scoped_refptr<RtpSenderInterface> AddOutgoingAudioTrack(
      FakePeerConnectionForStats* pc,
      StatsCollectorForTest* stats) {
    audio_track_ = rtc::make_ref_counted<FakeAudioTrack>(kLocalTrackId);
    if (GetParam()) {
      if (!stream_)
        stream_ = MediaStream::Create("streamid");
      stream_->AddTrack(audio_track_);
      stats->AddStream(stream_);
    } else {
      stats->AddTrack(audio_track_);
    }
    return pc->AddSender(CreateMockSender(audio_track_, kSsrcOfTrack));
  }

  // Adds a incoming audio track with a given SSRC into the stats.
  void AddIncomingAudioTrack(FakePeerConnectionForStats* pc,
                             StatsCollectorForTest* stats) {
    audio_track_ = rtc::make_ref_counted<FakeAudioTrack>(kRemoteTrackId);
    if (GetParam()) {
      if (stream_ == NULL)
        stream_ = MediaStream::Create("streamid");
      stream_->AddTrack(audio_track_);
      stats->AddStream(stream_);
    } else {
      stats->AddTrack(audio_track_);
    }
    pc->AddReceiver(CreateMockReceiver(audio_track_, kSsrcOfTrack));
  }

  rtc::scoped_refptr<MediaStream> stream_;
  rtc::scoped_refptr<VideoTrack> track_;
  rtc::scoped_refptr<FakeAudioTrack> audio_track_;
};

TEST_F(StatsCollectorTest, FilterOutNegativeDataChannelId) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  pc->AddSctpDataChannel("hacks");

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  const StatsReport* report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeDataChannel, 1);

  std::string value_in_report;
  EXPECT_FALSE(GetValue(report, StatsReport::kStatsValueNameDataChannelId,
                        &value_in_report));
}

// Verify that ExtractDataInfo populates reports.
TEST_F(StatsCollectorTest, ExtractDataInfo) {
  const std::string kDataChannelLabel = "hacks";
  constexpr int kDataChannelId = 31337;
  const std::string kConnectingString = DataChannelInterface::DataStateString(
      DataChannelInterface::DataState::kConnecting);

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  InternalDataChannelInit init;
  init.id = kDataChannelId;
  pc->AddSctpDataChannel(kDataChannelLabel, init);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  const StatsReport* report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeDataChannel, 1);

  StatsReport::Id report_id = StatsReport::NewTypedIntId(
      StatsReport::kStatsReportTypeDataChannel, kDataChannelId);

  EXPECT_TRUE(report_id->Equals(report->id()));

  EXPECT_EQ(stats->GetTimeNow(), report->timestamp());
  EXPECT_EQ(kDataChannelLabel,
            ExtractStatsValue(StatsReport::kStatsReportTypeDataChannel, reports,
                              StatsReport::kStatsValueNameLabel));
  EXPECT_EQ(rtc::ToString(kDataChannelId),
            ExtractStatsValue(StatsReport::kStatsReportTypeDataChannel, reports,
                              StatsReport::kStatsValueNameDataChannelId));
  EXPECT_EQ(kConnectingString,
            ExtractStatsValue(StatsReport::kStatsReportTypeDataChannel, reports,
                              StatsReport::kStatsValueNameState));
  EXPECT_EQ("",
            ExtractStatsValue(StatsReport::kStatsReportTypeDataChannel, reports,
                              StatsReport::kStatsValueNameProtocol));
}

// This test verifies that 64-bit counters are passed successfully.
TEST_P(StatsCollectorTrackTest, BytesCounterHandles64Bits) {
  // The number of bytes must be larger than 0xFFFFFFFF for this test.
  constexpr int64_t kBytesSent = 12345678901234LL;

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  VideoSenderInfo video_sender_info;
  video_sender_info.add_ssrc(1234);
  video_sender_info.payload_bytes_sent = kBytesSent;
  video_sender_info.header_and_padding_bytes_sent = 0;
  VideoMediaInfo video_info;
  video_info.aggregated_senders.push_back(video_sender_info);

  auto* video_media_channel = pc->AddVideoChannel("video", "transport");
  video_media_channel->SetStats(video_info);

  AddOutgoingVideoTrack(pc, stats.get());

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  EXPECT_EQ(
      rtc::ToString(kBytesSent),
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameBytesSent));
}

// Test that audio BWE information is reported via stats.
TEST_P(StatsCollectorTrackTest, AudioBandwidthEstimationInfoIsReported) {
  // Set up an SSRC just to test that we get both kinds of stats back: SSRC and
  // BWE.
  constexpr int64_t kBytesSent = 12345678901234LL;
  constexpr int kSendBandwidth = 1234567;
  constexpr int kRecvBandwidth = 12345678;
  constexpr int kPacerDelay = 123;

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  VoiceSenderInfo voice_sender_info;
  voice_sender_info.add_ssrc(1234);
  voice_sender_info.payload_bytes_sent = kBytesSent - 12;
  voice_sender_info.header_and_padding_bytes_sent = 12;
  VoiceMediaInfo voice_info;
  voice_info.senders.push_back(voice_sender_info);

  auto* voice_media_channel = pc->AddVoiceChannel("audio", "transport");
  voice_media_channel->SetStats(voice_info);

  AddOutgoingAudioTrack(pc, stats.get());

  Call::Stats call_stats;
  call_stats.send_bandwidth_bps = kSendBandwidth;
  call_stats.recv_bandwidth_bps = kRecvBandwidth;
  call_stats.pacer_delay_ms = kPacerDelay;
  pc->SetCallStats(call_stats);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  EXPECT_EQ(
      rtc::ToString(kBytesSent),
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameBytesSent));
  EXPECT_EQ(rtc::ToString(kSendBandwidth),
            ExtractBweStatsValue(
                reports, StatsReport::kStatsValueNameAvailableSendBandwidth));
  EXPECT_EQ(
      rtc::ToString(kRecvBandwidth),
      ExtractBweStatsValue(
          reports, StatsReport::kStatsValueNameAvailableReceiveBandwidth));
  EXPECT_EQ(
      rtc::ToString(kPacerDelay),
      ExtractBweStatsValue(reports, StatsReport::kStatsValueNameBucketDelay));
}

// Test that video BWE information is reported via stats.
TEST_P(StatsCollectorTrackTest, VideoBandwidthEstimationInfoIsReported) {
  // Set up an SSRC just to test that we get both kinds of stats back: SSRC and
  // BWE.
  constexpr int64_t kBytesSent = 12345678901234LL;
  constexpr int kSendBandwidth = 1234567;
  constexpr int kRecvBandwidth = 12345678;
  constexpr int kPacerDelay = 123;

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  VideoSenderInfo video_sender_info;
  video_sender_info.add_ssrc(1234);
  video_sender_info.payload_bytes_sent = kBytesSent - 12;
  video_sender_info.header_and_padding_bytes_sent = 12;

  VideoMediaInfo video_info;
  video_info.aggregated_senders.push_back(video_sender_info);

  auto* video_media_channel = pc->AddVideoChannel("video", "transport");
  video_media_channel->SetStats(video_info);

  AddOutgoingVideoTrack(pc, stats.get());

  Call::Stats call_stats;
  call_stats.send_bandwidth_bps = kSendBandwidth;
  call_stats.recv_bandwidth_bps = kRecvBandwidth;
  call_stats.pacer_delay_ms = kPacerDelay;
  pc->SetCallStats(call_stats);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  EXPECT_EQ(
      rtc::ToString(kBytesSent),
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameBytesSent));
  EXPECT_EQ(rtc::ToString(kSendBandwidth),
            ExtractBweStatsValue(
                reports, StatsReport::kStatsValueNameAvailableSendBandwidth));
  EXPECT_EQ(
      rtc::ToString(kRecvBandwidth),
      ExtractBweStatsValue(
          reports, StatsReport::kStatsValueNameAvailableReceiveBandwidth));
  EXPECT_EQ(
      rtc::ToString(kPacerDelay),
      ExtractBweStatsValue(reports, StatsReport::kStatsValueNameBucketDelay));
}

// This test verifies that an object of type "googSession" always
// exists in the returned stats.
TEST_F(StatsCollectorTest, SessionObjectExists) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  EXPECT_TRUE(
      FindNthReportByType(reports, StatsReport::kStatsReportTypeSession, 1));
}

// This test verifies that only one object of type "googSession" exists
// in the returned stats.
TEST_F(StatsCollectorTest, OnlyOneSessionObjectExists) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  EXPECT_TRUE(
      FindNthReportByType(reports, StatsReport::kStatsReportTypeSession, 1));
  EXPECT_FALSE(
      FindNthReportByType(reports, StatsReport::kStatsReportTypeSession, 2));
}

// This test verifies that the empty track report exists in the returned stats
// without calling StatsCollector::UpdateStats.
TEST_P(StatsCollectorTrackTest, TrackObjectExistsWithoutUpdateStats) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  pc->AddVideoChannel("video", "transport");
  AddOutgoingVideoTrack(pc, stats.get());

  // Verfies the existence of the track report.
  StatsReports reports;
  stats->GetStats(nullptr, &reports);
  ASSERT_EQ(1u, reports.size());
  EXPECT_EQ(StatsReport::kStatsReportTypeTrack, reports[0]->type());
  EXPECT_EQ(0, reports[0]->timestamp());

  std::string trackValue =
      ExtractStatsValue(StatsReport::kStatsReportTypeTrack, reports,
                        StatsReport::kStatsValueNameTrackId);
  EXPECT_EQ(kLocalTrackId, trackValue);
}

// This test verifies that the empty track report exists in the returned stats
// when StatsCollector::UpdateStats is called with ssrc stats.
TEST_P(StatsCollectorTrackTest, TrackAndSsrcObjectExistAfterUpdateSsrcStats) {
  constexpr int64_t kBytesSent = 12345678901234LL;

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  VideoSenderInfo video_sender_info;
  video_sender_info.add_ssrc(1234);
  video_sender_info.payload_bytes_sent = kBytesSent - 12;
  video_sender_info.header_and_padding_bytes_sent = 12;
  VideoMediaInfo video_info;
  video_info.aggregated_senders.push_back(video_sender_info);

  auto* video_media_channel = pc->AddVideoChannel("video", "transport");
  video_media_channel->SetStats(video_info);

  AddOutgoingVideoTrack(pc, stats.get());

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  // |reports| should contain at least one session report, one track report,
  // and one ssrc report.
  EXPECT_LE(3u, reports.size());
  const StatsReport* track_report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeTrack, 1);
  EXPECT_TRUE(track_report);

  // Get report for the specific |track|.
  reports.clear();
  stats->GetStats(track_, &reports);
  // |reports| should contain at least one session report, one track report,
  // and one ssrc report.
  EXPECT_LE(3u, reports.size());
  track_report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeTrack, 1);
  ASSERT_TRUE(track_report);
  EXPECT_EQ(stats->GetTimeNow(), track_report->timestamp());

  std::string ssrc_id =
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameSsrc);
  EXPECT_EQ(rtc::ToString(kSsrcOfTrack), ssrc_id);

  std::string track_id =
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameTrackId);
  EXPECT_EQ(kLocalTrackId, track_id);

  std::string media_type =
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameMediaType);
  EXPECT_EQ("video", media_type);
}

// This test verifies that an SSRC object has the identifier of a Transport
// stats object, and that this transport stats object exists in stats.
TEST_P(StatsCollectorTrackTest, TransportObjectLinkedFromSsrcObject) {
  constexpr int64_t kBytesSent = 12345678901234LL;

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  VideoSenderInfo video_sender_info;
  video_sender_info.add_ssrc(1234);
  video_sender_info.payload_bytes_sent = kBytesSent - 12;
  video_sender_info.header_and_padding_bytes_sent = 12;
  VideoMediaInfo video_info;
  video_info.aggregated_senders.push_back(video_sender_info);

  auto* video_media_channel = pc->AddVideoChannel("video", "transport");
  video_media_channel->SetStats(video_info);

  AddOutgoingVideoTrack(pc, stats.get());

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  std::string transport_id =
      ExtractStatsValue(StatsReport::kStatsReportTypeSsrc, reports,
                        StatsReport::kStatsValueNameTransportId);
  ASSERT_NE(kNotFound, transport_id);

  // Transport id component ID will always be 1.
  // This has assumptions about how the ID is constructed.  As is, this is
  // OK since this is for testing purposes only, but if we ever need this
  // in production, we should add a generic method that does this.
  size_t index = transport_id.find('-');
  ASSERT_NE(std::string::npos, index);
  std::string content = transport_id.substr(index + 1);
  index = content.rfind('-');
  ASSERT_NE(std::string::npos, index);
  content = content.substr(0, index);
  StatsReport::Id id(StatsReport::NewComponentId(content, 1));
  ASSERT_EQ(transport_id, id->ToString());
  const StatsReport* transport_report = FindReportById(reports, id);
  ASSERT_TRUE(transport_report);
}

// This test verifies that a remote stats object will not be created for
// an outgoing SSRC where remote stats are not returned.
TEST_P(StatsCollectorTrackTest, RemoteSsrcInfoIsAbsent) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  pc->AddVideoChannel("video", "transport");
  AddOutgoingVideoTrack(pc, stats.get());

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  const StatsReport* remote_report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeRemoteSsrc, 1);
  EXPECT_FALSE(remote_report);
}

// This test verifies that a remote stats object will be created for
// an outgoing SSRC where stats are returned.
TEST_P(StatsCollectorTrackTest, RemoteSsrcInfoIsPresent) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  SsrcReceiverInfo remote_ssrc_stats;
  remote_ssrc_stats.timestamp = 12345.678;
  remote_ssrc_stats.ssrc = kSsrcOfTrack;
  VideoSenderInfo video_sender_info;
  video_sender_info.add_ssrc(kSsrcOfTrack);
  video_sender_info.remote_stats.push_back(remote_ssrc_stats);
  VideoMediaInfo video_info;
  video_info.aggregated_senders.push_back(video_sender_info);

  auto* video_media_channel = pc->AddVideoChannel("video", "transport");
  video_media_channel->SetStats(video_info);

  AddOutgoingVideoTrack(pc, stats.get());

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  const StatsReport* remote_report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeRemoteSsrc, 1);
  ASSERT_TRUE(remote_report);
  EXPECT_EQ(12345.678, remote_report->timestamp());
}

// This test verifies that the empty track report exists in the returned stats
// when StatsCollector::UpdateStats is called with ssrc stats.
TEST_P(StatsCollectorTrackTest, ReportsFromRemoteTrack) {
  constexpr int64_t kNumOfPacketsConcealed = 54321;

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  VideoReceiverInfo video_receiver_info;
  video_receiver_info.add_ssrc(1234);
  video_receiver_info.packets_concealed = kNumOfPacketsConcealed;
  VideoMediaInfo video_info;
  video_info.receivers.push_back(video_receiver_info);

  auto* video_media_info = pc->AddVideoChannel("video", "transport");
  video_media_info->SetStats(video_info);

  AddIncomingVideoTrack(pc, stats.get());

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  // |reports| should contain at least one session report, one track report,
  // and one ssrc report.
  EXPECT_LE(3u, reports.size());
  const StatsReport* track_report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeTrack, 1);
  ASSERT_TRUE(track_report);
  EXPECT_EQ(stats->GetTimeNow(), track_report->timestamp());

  std::string ssrc_id =
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameSsrc);
  EXPECT_EQ(rtc::ToString(kSsrcOfTrack), ssrc_id);

  std::string track_id =
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameTrackId);
  EXPECT_EQ(kRemoteTrackId, track_id);
}

// This test verifies the Ice Candidate report should contain the correct
// information from local/remote candidates.
TEST_F(StatsCollectorTest, IceCandidateReport) {
  const std::string kTransportName = "transport";
  const rtc::AdapterType kNetworkType = rtc::ADAPTER_TYPE_ETHERNET;
  constexpr uint32_t kPriority = 1000;

  constexpr int kLocalPort = 2000;
  const std::string kLocalIp = "192.168.0.1";
  const rtc::SocketAddress kLocalAddress(kLocalIp, kLocalPort);

  constexpr int kRemotePort = 2001;
  const std::string kRemoteIp = "192.168.0.2";
  const rtc::SocketAddress kRemoteAddress(kRemoteIp, kRemotePort);

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  cricket::Candidate local;
  EXPECT_GT(local.id().length(), 0u);
  local.set_type(cricket::LOCAL_PORT_TYPE);
  local.set_protocol(cricket::UDP_PROTOCOL_NAME);
  local.set_address(kLocalAddress);
  local.set_priority(kPriority);
  local.set_network_type(kNetworkType);

  cricket::Candidate remote;
  EXPECT_GT(remote.id().length(), 0u);
  remote.set_type(cricket::PRFLX_PORT_TYPE);
  remote.set_protocol(cricket::UDP_PROTOCOL_NAME);
  remote.set_address(kRemoteAddress);
  remote.set_priority(kPriority);
  remote.set_network_type(kNetworkType);

  ConnectionInfo connection_info;
  connection_info.local_candidate = local;
  connection_info.remote_candidate = remote;
  TransportChannelStats channel_stats;
  channel_stats.ice_transport_stats.connection_infos.push_back(connection_info);

  pc->AddVoiceChannel("audio", kTransportName);
  pc->SetTransportStats(kTransportName, channel_stats);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  // Verify the local candidate report is populated correctly.
  EXPECT_EQ(
      "Cand-" + local.id(),
      ExtractStatsValue(StatsReport::kStatsReportTypeCandidatePair, reports,
                        StatsReport::kStatsValueNameLocalCandidateId));
  EXPECT_EQ(
      kLocalIp,
      ExtractStatsValue(StatsReport::kStatsReportTypeIceLocalCandidate, reports,
                        StatsReport::kStatsValueNameCandidateIPAddress));
  EXPECT_EQ(
      rtc::ToString(kLocalPort),
      ExtractStatsValue(StatsReport::kStatsReportTypeIceLocalCandidate, reports,
                        StatsReport::kStatsValueNameCandidatePortNumber));
  EXPECT_EQ(
      cricket::UDP_PROTOCOL_NAME,
      ExtractStatsValue(StatsReport::kStatsReportTypeIceLocalCandidate, reports,
                        StatsReport::kStatsValueNameCandidateTransportType));
  EXPECT_EQ(
      rtc::ToString(kPriority),
      ExtractStatsValue(StatsReport::kStatsReportTypeIceLocalCandidate, reports,
                        StatsReport::kStatsValueNameCandidatePriority));
  EXPECT_EQ(
      IceCandidateTypeToStatsType(cricket::LOCAL_PORT_TYPE),
      ExtractStatsValue(StatsReport::kStatsReportTypeIceLocalCandidate, reports,
                        StatsReport::kStatsValueNameCandidateType));
  EXPECT_EQ(
      AdapterTypeToStatsType(kNetworkType),
      ExtractStatsValue(StatsReport::kStatsReportTypeIceLocalCandidate, reports,
                        StatsReport::kStatsValueNameCandidateNetworkType));

  // Verify the remote candidate report is populated correctly.
  EXPECT_EQ(
      "Cand-" + remote.id(),
      ExtractStatsValue(StatsReport::kStatsReportTypeCandidatePair, reports,
                        StatsReport::kStatsValueNameRemoteCandidateId));
  EXPECT_EQ(kRemoteIp,
            ExtractStatsValue(StatsReport::kStatsReportTypeIceRemoteCandidate,
                              reports,
                              StatsReport::kStatsValueNameCandidateIPAddress));
  EXPECT_EQ(rtc::ToString(kRemotePort),
            ExtractStatsValue(StatsReport::kStatsReportTypeIceRemoteCandidate,
                              reports,
                              StatsReport::kStatsValueNameCandidatePortNumber));
  EXPECT_EQ(cricket::UDP_PROTOCOL_NAME,
            ExtractStatsValue(
                StatsReport::kStatsReportTypeIceRemoteCandidate, reports,
                StatsReport::kStatsValueNameCandidateTransportType));
  EXPECT_EQ(rtc::ToString(kPriority),
            ExtractStatsValue(StatsReport::kStatsReportTypeIceRemoteCandidate,
                              reports,
                              StatsReport::kStatsValueNameCandidatePriority));
  EXPECT_EQ(
      IceCandidateTypeToStatsType(cricket::PRFLX_PORT_TYPE),
      ExtractStatsValue(StatsReport::kStatsReportTypeIceRemoteCandidate,
                        reports, StatsReport::kStatsValueNameCandidateType));
  EXPECT_EQ(kNotFound,
            ExtractStatsValue(
                StatsReport::kStatsReportTypeIceRemoteCandidate, reports,
                StatsReport::kStatsValueNameCandidateNetworkType));
}

// This test verifies that all chained certificates are correctly
// reported
TEST_F(StatsCollectorTest, ChainedCertificateReportsCreated) {
  // Build local certificate chain.
  std::vector<std::string> local_ders(5);
  local_ders[0] = "These";
  local_ders[1] = "are";
  local_ders[2] = "some";
  local_ders[3] = "der";
  local_ders[4] = "values";
  rtc::FakeSSLIdentity local_identity(DersToPems(local_ders));

  // Build remote certificate chain
  std::vector<std::string> remote_ders(4);
  remote_ders[0] = "A";
  remote_ders[1] = "non-";
  remote_ders[2] = "intersecting";
  remote_ders[3] = "set";
  rtc::FakeSSLIdentity remote_identity(DersToPems(remote_ders));

  TestCertificateReports(local_identity, local_ders, remote_identity,
                         remote_ders);
}

// This test verifies that all certificates without chains are correctly
// reported.
TEST_F(StatsCollectorTest, ChainlessCertificateReportsCreated) {
  // Build local certificate.
  std::string local_der = "This is the local der.";
  rtc::FakeSSLIdentity local_identity(DerToPem(local_der));

  // Build remote certificate.
  std::string remote_der = "This is somebody else's der.";
  rtc::FakeSSLIdentity remote_identity(DerToPem(remote_der));

  TestCertificateReports(local_identity, std::vector<std::string>(1, local_der),
                         remote_identity,
                         std::vector<std::string>(1, remote_der));
}

// This test verifies that the stats are generated correctly when no
// transport is present.
TEST_F(StatsCollectorTest, NoTransport) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  // This will cause the fake PeerConnection to generate a TransportStats entry
  // but with only a single dummy TransportChannelStats.
  pc->AddVoiceChannel("audio", "transport");

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  // Check that the local certificate is absent.
  std::string local_certificate_id =
      ExtractStatsValue(StatsReport::kStatsReportTypeComponent, reports,
                        StatsReport::kStatsValueNameLocalCertificateId);
  ASSERT_EQ(kNotFound, local_certificate_id);

  // Check that the remote certificate is absent.
  std::string remote_certificate_id =
      ExtractStatsValue(StatsReport::kStatsReportTypeComponent, reports,
                        StatsReport::kStatsValueNameRemoteCertificateId);
  ASSERT_EQ(kNotFound, remote_certificate_id);

  // Check that the negotiated ciphers are absent.
  std::string dtls_cipher_suite =
      ExtractStatsValue(StatsReport::kStatsReportTypeComponent, reports,
                        StatsReport::kStatsValueNameDtlsCipher);
  ASSERT_EQ(kNotFound, dtls_cipher_suite);
  std::string srtp_crypto_suite =
      ExtractStatsValue(StatsReport::kStatsReportTypeComponent, reports,
                        StatsReport::kStatsValueNameSrtpCipher);
  ASSERT_EQ(kNotFound, srtp_crypto_suite);
}

// This test verifies that a remote certificate with an unsupported digest
// algorithm is correctly ignored.
TEST_F(StatsCollectorTest, UnsupportedDigestIgnored) {
  // Build a local certificate.
  std::string local_der = "This is the local der.";
  rtc::FakeSSLIdentity local_identity(DerToPem(local_der));

  // Build a remote certificate with an unsupported digest algorithm.
  std::string remote_der = "This is somebody else's der.";
  rtc::FakeSSLCertificate remote_cert(DerToPem(remote_der));
  remote_cert.set_digest_algorithm("foobar");
  rtc::FakeSSLIdentity remote_identity(remote_cert);

  TestCertificateReports(local_identity, std::vector<std::string>(1, local_der),
                         remote_identity, std::vector<std::string>());
}

// This test verifies that the audio/video related stats which are -1 initially
// will be filtered out.
TEST_P(StatsCollectorTrackTest, FilterOutNegativeInitialValues) {
  // This test uses streams, but only works for the stream case.
  if (!GetParam()) {
    return;
  }

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  // Create a local stream with a local audio track and adds it to the stats.
  stream_ = MediaStream::Create("streamid");
  auto local_track =
      rtc::make_ref_counted<FakeAudioTrackWithInitValue>(kLocalTrackId);
  stream_->AddTrack(local_track);
  pc->AddSender(CreateMockSender(local_track, kSsrcOfTrack));
  if (GetParam()) {
    stats->AddStream(stream_);
  }
  stats->AddLocalAudioTrack(local_track.get(), kSsrcOfTrack);

  // Create a remote stream with a remote audio track and adds it to the stats.
  rtc::scoped_refptr<MediaStream> remote_stream(
      MediaStream::Create("remotestreamid"));
  auto remote_track =
      rtc::make_ref_counted<FakeAudioTrackWithInitValue>(kRemoteTrackId);
  remote_stream->AddTrack(remote_track);
  pc->AddReceiver(CreateMockReceiver(remote_track, kSsrcOfTrack));
  if (GetParam()) {
    stats->AddStream(remote_stream);
  }

  VoiceSenderInfo voice_sender_info;
  voice_sender_info.add_ssrc(kSsrcOfTrack);
  // These values are set to -1 initially in audio_send_stream.
  // The voice_sender_info will read the values from audio_send_stream.
  voice_sender_info.rtt_ms = -1;
  voice_sender_info.packets_lost = -1;
  voice_sender_info.jitter_ms = -1;

  // Some of the contents in |voice_sender_info| needs to be updated from the
  // |audio_track_|.
  UpdateVoiceSenderInfoFromAudioTrack(local_track.get(), &voice_sender_info,
                                      true);

  VoiceReceiverInfo voice_receiver_info;
  voice_receiver_info.add_ssrc(kSsrcOfTrack);
  voice_receiver_info.capture_start_ntp_time_ms = -1;
  voice_receiver_info.audio_level = -1;

  // Constructs an ssrc stats update.
  VoiceMediaInfo voice_info;
  voice_info.senders.push_back(voice_sender_info);
  voice_info.receivers.push_back(voice_receiver_info);

  auto* voice_media_channel = pc->AddVoiceChannel("voice", "transport");
  voice_media_channel->SetStats(voice_info);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);

  // Get stats for the local track.
  StatsReports reports;
  stats->GetStats(local_track.get(), &reports);
  const StatsReport* report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeSsrc, 1);
  ASSERT_TRUE(report);
  // The -1 will not be added to the stats report.
  std::string value_in_report;
  EXPECT_FALSE(
      GetValue(report, StatsReport::kStatsValueNameRtt, &value_in_report));
  EXPECT_FALSE(GetValue(report, StatsReport::kStatsValueNamePacketsLost,
                        &value_in_report));
  EXPECT_FALSE(GetValue(report, StatsReport::kStatsValueNameJitterReceived,
                        &value_in_report));
  EXPECT_FALSE(GetValue(report, StatsReport::kStatsValueNameEchoDelayMedian,
                        &value_in_report));
  EXPECT_FALSE(GetValue(report, StatsReport::kStatsValueNameEchoDelayStdDev,
                        &value_in_report));

  // Get stats for the remote track.
  reports.clear();
  stats->GetStats(remote_track.get(), &reports);
  report = FindNthReportByType(reports, StatsReport::kStatsReportTypeSsrc, 1);
  ASSERT_TRUE(report);
  EXPECT_FALSE(GetValue(report,
                        StatsReport::kStatsValueNameCaptureStartNtpTimeMs,
                        &value_in_report));
  EXPECT_FALSE(GetValue(report, StatsReport::kStatsValueNameAudioInputLevel,
                        &value_in_report));
}

// This test verifies that a local stats object can get statistics via
// AudioTrackInterface::GetStats() method.
TEST_P(StatsCollectorTrackTest, GetStatsFromLocalAudioTrack) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  AddOutgoingAudioTrack(pc, stats.get());
  stats->AddLocalAudioTrack(audio_track_, kSsrcOfTrack);

  VoiceSenderInfo voice_sender_info;
  InitVoiceSenderInfo(&voice_sender_info);
  UpdateVoiceSenderInfoFromAudioTrack(audio_track_, &voice_sender_info, false);
  VoiceMediaInfo voice_info;
  voice_info.senders.push_back(voice_sender_info);

  auto* voice_media_channel = pc->AddVoiceChannel("audio", "transport");
  voice_media_channel->SetStats(voice_info);

  StatsReports reports;  // returned values.
  VerifyAudioTrackStats(audio_track_, stats.get(), voice_info, &reports);

  // Verify that there is no remote report for the local audio track because
  // we did not set it up.
  const StatsReport* remote_report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeRemoteSsrc, 1);
  EXPECT_TRUE(remote_report == NULL);
}

// This test verifies that audio receive streams populate stats reports
// correctly.
TEST_P(StatsCollectorTrackTest, GetStatsFromRemoteStream) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  AddIncomingAudioTrack(pc, stats.get());

  VoiceReceiverInfo voice_receiver_info;
  InitVoiceReceiverInfo(&voice_receiver_info);
  voice_receiver_info.codec_name = "fake_codec";
  VoiceMediaInfo voice_info;
  voice_info.receivers.push_back(voice_receiver_info);

  auto* voice_media_channel = pc->AddVoiceChannel("audio", "transport");
  voice_media_channel->SetStats(voice_info);

  StatsReports reports;  // returned values.
  VerifyAudioTrackStats(audio_track_, stats.get(), voice_info, &reports);
}

// This test verifies that a local stats object won't update its statistics
// after a RemoveLocalAudioTrack() call.
TEST_P(StatsCollectorTrackTest, GetStatsAfterRemoveAudioStream) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  AddOutgoingAudioTrack(pc, stats.get());
  stats->AddLocalAudioTrack(audio_track_.get(), kSsrcOfTrack);

  VoiceSenderInfo voice_sender_info;
  InitVoiceSenderInfo(&voice_sender_info);
  VoiceMediaInfo voice_info;
  voice_info.senders.push_back(voice_sender_info);

  auto* voice_media_channel = pc->AddVoiceChannel("audio", "transport");
  voice_media_channel->SetStats(voice_info);

  stats->RemoveLocalAudioTrack(audio_track_.get(), kSsrcOfTrack);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  // The report will exist since we don't remove them in RemoveStream().
  const StatsReport* report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeSsrc, 1);
  ASSERT_TRUE(report);
  EXPECT_EQ(stats->GetTimeNow(), report->timestamp());
  std::string track_id =
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameTrackId);
  EXPECT_EQ(kLocalTrackId, track_id);
  std::string ssrc_id =
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameSsrc);
  EXPECT_EQ(rtc::ToString(kSsrcOfTrack), ssrc_id);

  // Verifies the values in the track report, no value will be changed by the
  // AudioTrackInterface::GetSignalValue() and
  // AudioProcessorInterface::GetStats();
  VerifyVoiceSenderInfoReport(report, voice_sender_info);
}

// This test verifies that when ongoing and incoming audio tracks are using
// the same ssrc, they populate stats reports correctly.
TEST_P(StatsCollectorTrackTest, LocalAndRemoteTracksWithSameSsrc) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  // Create a local stream with a local audio track and adds it to the stats.
  AddOutgoingAudioTrack(pc, stats.get());
  stats->AddLocalAudioTrack(audio_track_.get(), kSsrcOfTrack);

  // Create a remote stream with a remote audio track and adds it to the stats.
  rtc::scoped_refptr<MediaStream> remote_stream(
      MediaStream::Create("remotestreamid"));
  auto remote_track = rtc::make_ref_counted<FakeAudioTrack>(kRemoteTrackId);
  pc->AddReceiver(CreateMockReceiver(remote_track, kSsrcOfTrack));
  remote_stream->AddTrack(remote_track);
  stats->AddStream(remote_stream);

  VoiceSenderInfo voice_sender_info;
  InitVoiceSenderInfo(&voice_sender_info);
  // Some of the contents in |voice_sender_info| needs to be updated from the
  // |audio_track_|.
  UpdateVoiceSenderInfoFromAudioTrack(audio_track_.get(), &voice_sender_info,
                                      true);

  VoiceReceiverInfo voice_receiver_info;
  InitVoiceReceiverInfo(&voice_receiver_info);

  // Constructs an ssrc stats update.
  VoiceMediaInfo voice_info;
  voice_info.senders.push_back(voice_sender_info);
  voice_info.receivers.push_back(voice_receiver_info);

  // Instruct the session to return stats containing the transport channel.
  auto* voice_media_channel = pc->AddVoiceChannel("audio", "transport");
  voice_media_channel->SetStats(voice_info);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);

  // Get stats for the local track.
  StatsReports reports;  // returned values.
  stats->GetStats(audio_track_.get(), &reports);

  const StatsReport* track_report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeSsrc, 1);
  ASSERT_TRUE(track_report);
  EXPECT_EQ(stats->GetTimeNow(), track_report->timestamp());
  std::string track_id =
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameTrackId);
  EXPECT_EQ(kLocalTrackId, track_id);
  VerifyVoiceSenderInfoReport(track_report, voice_sender_info);

  // Get stats for the remote track.
  reports.clear();
  stats->GetStats(remote_track.get(), &reports);
  track_report =
      FindNthReportByType(reports, StatsReport::kStatsReportTypeSsrc, 1);
  ASSERT_TRUE(track_report);
  EXPECT_EQ(stats->GetTimeNow(), track_report->timestamp());
  track_id =
      ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameTrackId);
  EXPECT_EQ(kRemoteTrackId, track_id);
  VerifyVoiceReceiverInfoReport(track_report, voice_receiver_info);
}

// This test verifies that when two outgoing audio tracks are using the same
// ssrc at different times, they populate stats reports correctly.
// TODO(xians): Figure out if it is possible to encapsulate the setup and
// avoid duplication of code in test cases.
TEST_P(StatsCollectorTrackTest, TwoLocalTracksWithSameSsrc) {
  // This test only makes sense when we're using streams.
  if (!GetParam()) {
    return;
  }

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  // Create a local stream with a local audio track and adds it to the stats.
  auto sender = AddOutgoingAudioTrack(pc, stats.get());
  stats->AddLocalAudioTrack(audio_track_, kSsrcOfTrack);

  VoiceSenderInfo voice_sender_info;
  InitVoiceSenderInfo(&voice_sender_info);
  UpdateVoiceSenderInfoFromAudioTrack(audio_track_, &voice_sender_info, false);
  voice_sender_info.add_ssrc(kSsrcOfTrack);
  VoiceMediaInfo voice_info;
  voice_info.senders.push_back(voice_sender_info);

  auto* voice_media_channel = pc->AddVoiceChannel("voice", "transport");
  voice_media_channel->SetStats(voice_info);

  StatsReports reports;  // returned values.
  VerifyAudioTrackStats(audio_track_, stats.get(), voice_info, &reports);

  // Remove the previous audio track from the stream.
  stream_->RemoveTrack(audio_track_.get());
  stats->RemoveLocalAudioTrack(audio_track_.get(), kSsrcOfTrack);
  pc->RemoveSender(sender);

  // Create a new audio track and adds it to the stream and stats.
  static const std::string kNewTrackId = "new_track_id";
  auto new_audio_track = rtc::make_ref_counted<FakeAudioTrack>(kNewTrackId);
  pc->AddSender(CreateMockSender(new_audio_track, kSsrcOfTrack));
  stream_->AddTrack(new_audio_track);

  stats->AddLocalAudioTrack(new_audio_track, kSsrcOfTrack);
  stats->ClearUpdateStatsCacheForTest();

  VoiceSenderInfo new_voice_sender_info;
  InitVoiceSenderInfo(&new_voice_sender_info);
  UpdateVoiceSenderInfoFromAudioTrack(new_audio_track, &new_voice_sender_info,
                                      false);
  VoiceMediaInfo new_voice_info;
  new_voice_info.senders.push_back(new_voice_sender_info);
  voice_media_channel->SetStats(new_voice_info);

  reports.clear();
  VerifyAudioTrackStats(new_audio_track, stats.get(), new_voice_info, &reports);
}

// Test that if there are two local senders with the same track then two SSRC
// reports will be created, one for each sender, with the same track ID and one
// track report will be created for the shared track.
TEST_P(StatsCollectorTrackTest, TwoLocalSendersWithSameTrack) {
  constexpr uint32_t kFirstSsrc = 22;
  constexpr uint32_t kSecondSsrc = 33;

  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  auto local_track =
      rtc::make_ref_counted<FakeAudioTrackWithInitValue>(kLocalTrackId);
  pc->AddSender(CreateMockSender(local_track, kFirstSsrc));
  stats->AddLocalAudioTrack(local_track.get(), kFirstSsrc);
  pc->AddSender(CreateMockSender(local_track, kSecondSsrc));
  stats->AddLocalAudioTrack(local_track.get(), kSecondSsrc);

  VoiceSenderInfo first_sender_info;
  InitVoiceSenderInfo(&first_sender_info, kFirstSsrc);
  UpdateVoiceSenderInfoFromAudioTrack(local_track.get(), &first_sender_info,
                                      false);

  VoiceSenderInfo second_sender_info;
  InitVoiceSenderInfo(&second_sender_info, kSecondSsrc);
  UpdateVoiceSenderInfoFromAudioTrack(local_track.get(), &second_sender_info,
                                      false);

  VoiceMediaInfo voice_info;
  voice_info.senders.push_back(first_sender_info);
  voice_info.senders.push_back(second_sender_info);

  auto* voice_media_channel = pc->AddVoiceChannel("voice", "transport");
  voice_media_channel->SetStats(voice_info);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);

  StatsReports reports;
  stats->GetStats(local_track.get(), &reports);
  RTC_LOG(LS_INFO) << reports.size();

  // Both SSRC reports have the same track ID.
  EXPECT_EQ(kLocalTrackId, GetValueInNthReportByType(
                               reports, StatsReport::kStatsReportTypeSsrc,
                               StatsReport::kStatsValueNameTrackId, 1));
  EXPECT_EQ(kLocalTrackId, GetValueInNthReportByType(
                               reports, StatsReport::kStatsReportTypeSsrc,
                               StatsReport::kStatsValueNameTrackId, 2));

  // The SSRC in each SSRC report is different and correspond to the sender
  // SSRC.
  std::vector<absl::optional<std::string>> ssrcs = {
      GetValueInNthReportByType(reports, StatsReport::kStatsReportTypeSsrc,
                                StatsReport::kStatsValueNameSsrc, 1),
      GetValueInNthReportByType(reports, StatsReport::kStatsReportTypeSsrc,
                                StatsReport::kStatsValueNameSsrc, 2)};
  EXPECT_THAT(ssrcs, UnorderedElementsAre(rtc::ToString(kFirstSsrc),
                                          rtc::ToString(kSecondSsrc)));

  // There is one track report with the same track ID as the SSRC reports.
  EXPECT_EQ(
      1u, GetReportsByType(reports, StatsReport::kStatsReportTypeTrack).size());
  EXPECT_EQ(kLocalTrackId, GetValueInNthReportByType(
                               reports, StatsReport::kStatsReportTypeTrack,
                               StatsReport::kStatsValueNameTrackId, 1));
}

// This test verifies that stats are correctly set in video send ssrc stats.
TEST_P(StatsCollectorTrackTest, VerifyVideoSendSsrcStats) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  AddOutgoingVideoTrack(pc, stats.get());

  VideoSenderInfo video_sender_info;
  video_sender_info.add_ssrc(1234);
  video_sender_info.frames_encoded = 10;
  video_sender_info.qp_sum = 11;
  VideoMediaInfo video_info;
  video_info.aggregated_senders.push_back(video_sender_info);

  auto* video_media_channel = pc->AddVideoChannel("video", "transport");
  video_media_channel->SetStats(video_info);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  EXPECT_EQ(rtc::ToString(video_sender_info.frames_encoded),
            ExtractSsrcStatsValue(reports,
                                  StatsReport::kStatsValueNameFramesEncoded));
  EXPECT_EQ(rtc::ToString(*video_sender_info.qp_sum),
            ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameQpSum));
}

// This test verifies that stats are correctly set in video receive ssrc stats.
TEST_P(StatsCollectorTrackTest, VerifyVideoReceiveSsrcStatsNew) {
  auto pc = CreatePeerConnection();
  auto stats = CreateStatsCollector(pc);

  AddIncomingVideoTrack(pc, stats.get());

  VideoReceiverInfo video_receiver_info;
  video_receiver_info.add_ssrc(1234);
  video_receiver_info.frames_decoded = 10;
  video_receiver_info.qp_sum = 11;
  VideoMediaInfo video_info;
  video_info.receivers.push_back(video_receiver_info);

  auto* video_media_channel = pc->AddVideoChannel("video", "transport");
  video_media_channel->SetStats(video_info);

  stats->UpdateStats(PeerConnectionInterface::kStatsOutputLevelStandard);
  StatsReports reports;
  stats->GetStats(nullptr, &reports);

  EXPECT_EQ(rtc::ToString(video_receiver_info.frames_decoded),
            ExtractSsrcStatsValue(reports,
                                  StatsReport::kStatsValueNameFramesDecoded));
  EXPECT_EQ(rtc::ToString(*video_receiver_info.qp_sum),
            ExtractSsrcStatsValue(reports, StatsReport::kStatsValueNameQpSum));
}

INSTANTIATE_TEST_SUITE_P(HasStream, StatsCollectorTrackTest, ::testing::Bool());

}  // namespace webrtc
