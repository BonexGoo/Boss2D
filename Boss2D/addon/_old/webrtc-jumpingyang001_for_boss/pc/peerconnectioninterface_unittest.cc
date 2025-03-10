/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_decoder_factory_h //original-code:"api/audio_codecs/builtin_audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_encoder_factory_h //original-code:"api/audio_codecs/builtin_audio_encoder_factory.h"
#include BOSS_WEBRTC_U_api__jsepsessiondescription_h //original-code:"api/jsepsessiondescription.h"
#include BOSS_WEBRTC_U_api__mediastreaminterface_h //original-code:"api/mediastreaminterface.h"
#include BOSS_WEBRTC_U_api__peerconnectioninterface_h //original-code:"api/peerconnectioninterface.h"
#include BOSS_WEBRTC_U_api__rtpreceiverinterface_h //original-code:"api/rtpreceiverinterface.h"
#include BOSS_WEBRTC_U_api__rtpsenderinterface_h //original-code:"api/rtpsenderinterface.h"
#include BOSS_WEBRTC_U_api__test__fakeconstraints_h //original-code:"api/test/fakeconstraints.h"
#include BOSS_WEBRTC_U_api__video_codecs__builtin_video_decoder_factory_h //original-code:"api/video_codecs/builtin_video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__builtin_video_encoder_factory_h //original-code:"api/video_codecs/builtin_video_encoder_factory.h"
#include BOSS_WEBRTC_U_logging__rtc_event_log__output__rtc_event_log_output_file_h //original-code:"logging/rtc_event_log/output/rtc_event_log_output_file.h"
#include BOSS_WEBRTC_U_media__base__fakevideocapturer_h //original-code:"media/base/fakevideocapturer.h"
#include BOSS_WEBRTC_U_media__engine__webrtcmediaengine_h //original-code:"media/engine/webrtcmediaengine.h"
#include BOSS_WEBRTC_U_media__sctp__sctptransportinternal_h //original-code:"media/sctp/sctptransportinternal.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_p2p__base__fakeportallocator_h //original-code:"p2p/base/fakeportallocator.h"
#include BOSS_WEBRTC_U_pc__audiotrack_h //original-code:"pc/audiotrack.h"
#include BOSS_WEBRTC_U_pc__mediasession_h //original-code:"pc/mediasession.h"
#include BOSS_WEBRTC_U_pc__mediastream_h //original-code:"pc/mediastream.h"
#include BOSS_WEBRTC_U_pc__peerconnection_h //original-code:"pc/peerconnection.h"
#include BOSS_WEBRTC_U_pc__rtpsender_h //original-code:"pc/rtpsender.h"
#include BOSS_WEBRTC_U_pc__streamcollection_h //original-code:"pc/streamcollection.h"
#include BOSS_WEBRTC_U_pc__test__fakeaudiocapturemodule_h //original-code:"pc/test/fakeaudiocapturemodule.h"
#include BOSS_WEBRTC_U_pc__test__fakertccertificategenerator_h //original-code:"pc/test/fakertccertificategenerator.h"
#include BOSS_WEBRTC_U_pc__test__fakevideotracksource_h //original-code:"pc/test/fakevideotracksource.h"
#include BOSS_WEBRTC_U_pc__test__mockpeerconnectionobservers_h //original-code:"pc/test/mockpeerconnectionobservers.h"
#include BOSS_WEBRTC_U_pc__test__testsdpstrings_h //original-code:"pc/test/testsdpstrings.h"
#include BOSS_WEBRTC_U_pc__videocapturertracksource_h //original-code:"pc/videocapturertracksource.h"
#include BOSS_WEBRTC_U_pc__videotrack_h //original-code:"pc/videotrack.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__stringutils_h //original-code:"rtc_base/stringutils.h"
#include BOSS_WEBRTC_U_rtc_base__virtualsocketserver_h //original-code:"rtc_base/virtualsocketserver.h"
#include "test/gmock.h"
#include "test/testsupport/fileutils.h"

#ifdef WEBRTC_ANDROID
#include BOSS_WEBRTC_U_pc__test__androidtestinitializer_h //original-code:"pc/test/androidtestinitializer.h"
#endif

static const char kStreamId1[] = "local_stream_1";
static const char kStreamId2[] = "local_stream_2";
static const char kStreamId3[] = "local_stream_3";
static const int kDefaultStunPort = 3478;
static const char kStunAddressOnly[] = "stun:address";
static const char kStunInvalidPort[] = "stun:address:-1";
static const char kStunAddressPortAndMore1[] = "stun:address:port:more";
static const char kStunAddressPortAndMore2[] = "stun:address:port more";
static const char kTurnIceServerUri[] = "turn:user@turn.example.org";
static const char kTurnUsername[] = "user";
static const char kTurnPassword[] = "password";
static const char kTurnHostname[] = "turn.example.org";
static const uint32_t kTimeout = 10000U;

static const char kStreams[][8] = {"stream1", "stream2"};
static const char kAudioTracks[][32] = {"audiotrack0", "audiotrack1"};
static const char kVideoTracks[][32] = {"videotrack0", "videotrack1"};

static const char kRecvonly[] = "recvonly";
static const char kSendrecv[] = "sendrecv";

// Reference SDP with a MediaStream with label "stream1" and audio track with
// id "audio_1" and a video track with id "video_1;
static const char kSdpStringWithStream1PlanB[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:audio\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n"
    "a=ssrc:1 cname:stream1\r\n"
    "a=ssrc:1 mslabel:stream1\r\n"
    "a=ssrc:1 label:audiotrack0\r\n"
    "m=video 1 RTP/AVPF 120\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:video\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:120 VP8/90000\r\n"
    "a=ssrc:2 cname:stream1\r\n"
    "a=ssrc:2 mslabel:stream1\r\n"
    "a=ssrc:2 label:videotrack0\r\n";
// Same string as above but with the MID changed to the Unified Plan default.
// This is needed so that this SDP can be used as an answer for a Unified Plan
// offer.
static const char kSdpStringWithStream1UnifiedPlan[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:0\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n"
    "a=ssrc:1 cname:stream1\r\n"
    "a=ssrc:1 mslabel:stream1\r\n"
    "a=ssrc:1 label:audiotrack0\r\n"
    "m=video 1 RTP/AVPF 120\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:1\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:120 VP8/90000\r\n"
    "a=ssrc:2 cname:stream1\r\n"
    "a=ssrc:2 mslabel:stream1\r\n"
    "a=ssrc:2 label:videotrack0\r\n";

// Reference SDP with a MediaStream with label "stream1" and audio track with
// id "audio_1";
static const char kSdpStringWithStream1AudioTrackOnly[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:audio\r\n"
    "a=sendrecv\r\n"
    "a=rtpmap:103 ISAC/16000\r\n"
    "a=ssrc:1 cname:stream1\r\n"
    "a=ssrc:1 mslabel:stream1\r\n"
    "a=ssrc:1 label:audiotrack0\r\n"
    "a=rtcp-mux\r\n";

// Reference SDP with two MediaStreams with label "stream1" and "stream2. Each
// MediaStreams have one audio track and one video track.
// This uses MSID.
static const char kSdpStringWithStream1And2PlanB[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "a=msid-semantic: WMS stream1 stream2\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:audio\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n"
    "a=ssrc:1 cname:stream1\r\n"
    "a=ssrc:1 msid:stream1 audiotrack0\r\n"
    "a=ssrc:3 cname:stream2\r\n"
    "a=ssrc:3 msid:stream2 audiotrack1\r\n"
    "m=video 1 RTP/AVPF 120\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:video\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:120 VP8/0\r\n"
    "a=ssrc:2 cname:stream1\r\n"
    "a=ssrc:2 msid:stream1 videotrack0\r\n"
    "a=ssrc:4 cname:stream2\r\n"
    "a=ssrc:4 msid:stream2 videotrack1\r\n";
static const char kSdpStringWithStream1And2UnifiedPlan[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "a=msid-semantic: WMS stream1 stream2\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:0\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n"
    "a=ssrc:1 cname:stream1\r\n"
    "a=ssrc:1 msid:stream1 audiotrack0\r\n"
    "m=video 1 RTP/AVPF 120\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:1\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:120 VP8/0\r\n"
    "a=ssrc:2 cname:stream1\r\n"
    "a=ssrc:2 msid:stream1 videotrack0\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:2\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n"
    "a=ssrc:3 cname:stream2\r\n"
    "a=ssrc:3 msid:stream2 audiotrack1\r\n"
    "m=video 1 RTP/AVPF 120\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:3\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:120 VP8/0\r\n"
    "a=ssrc:4 cname:stream2\r\n"
    "a=ssrc:4 msid:stream2 videotrack1\r\n";

// Reference SDP without MediaStreams. Msid is not supported.
static const char kSdpStringWithoutStreams[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:audio\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n"
    "m=video 1 RTP/AVPF 120\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:video\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:120 VP8/90000\r\n";

// Reference SDP without MediaStreams. Msid is supported.
static const char kSdpStringWithMsidWithoutStreams[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "a=msid-semantic: WMS\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:audio\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n"
    "m=video 1 RTP/AVPF 120\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:video\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:120 VP8/90000\r\n";

// Reference SDP without MediaStreams and audio only.
static const char kSdpStringWithoutStreamsAudioOnly[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:audio\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n";

// Reference SENDONLY SDP without MediaStreams. Msid is not supported.
static const char kSdpStringSendOnlyWithoutStreams[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:audio\r\n"
    "a=sendrecv\r\n"
    "a=sendonly\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n"
    "m=video 1 RTP/AVPF 120\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:video\r\n"
    "a=sendrecv\r\n"
    "a=sendonly\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:120 VP8/90000\r\n";

static const char kSdpStringInit[] =
    "v=0\r\n"
    "o=- 0 0 IN IP4 127.0.0.1\r\n"
    "s=-\r\n"
    "t=0 0\r\n"
    "a=msid-semantic: WMS\r\n";

static const char kSdpStringAudio[] =
    "m=audio 1 RTP/AVPF 103\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:audio\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:103 ISAC/16000\r\n";

static const char kSdpStringVideo[] =
    "m=video 1 RTP/AVPF 120\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=fingerprint:sha-256 58:AB:6E:F5:F1:E4:57:B7:E9:46:F4:86:04:28:F9:A7:ED:"
    "BD:AB:AE:40:EF:CE:9A:51:2C:2A:B1:9B:8B:78:84\r\n"
    "a=mid:video\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=rtpmap:120 VP8/90000\r\n";

static const char kSdpStringMs1Audio0[] =
    "a=ssrc:1 cname:stream1\r\n"
    "a=ssrc:1 msid:stream1 audiotrack0\r\n";

static const char kSdpStringMs1Video0[] =
    "a=ssrc:2 cname:stream1\r\n"
    "a=ssrc:2 msid:stream1 videotrack0\r\n";

static const char kSdpStringMs1Audio1[] =
    "a=ssrc:3 cname:stream1\r\n"
    "a=ssrc:3 msid:stream1 audiotrack1\r\n";

static const char kSdpStringMs1Video1[] =
    "a=ssrc:4 cname:stream1\r\n"
    "a=ssrc:4 msid:stream1 videotrack1\r\n";

static const char kDtlsSdesFallbackSdp[] =
    "v=0\r\n"
    "o=xxxxxx 7 2 IN IP4 0.0.0.0\r\n"
    "s=-\r\n"
    "c=IN IP4 0.0.0.0\r\n"
    "t=0 0\r\n"
    "a=group:BUNDLE audio\r\n"
    "a=msid-semantic: WMS\r\n"
    "m=audio 1 RTP/SAVPF 0\r\n"
    "a=sendrecv\r\n"
    "a=rtcp-mux\r\n"
    "a=mid:audio\r\n"
    "a=ssrc:1 cname:stream1\r\n"
    "a=ssrc:1 mslabel:stream1\r\n"
    "a=ssrc:1 label:audiotrack0\r\n"
    "a=ice-ufrag:e5785931\r\n"
    "a=ice-pwd:36fb7878390db89481c1d46daa4278d8\r\n"
    "a=rtpmap:0 pcmu/8000\r\n"
    "a=fingerprint:sha-1 "
    "4A:AD:B9:B1:3F:82:18:3B:54:02:12:DF:3E:5D:49:6B:19:E5:7C:AB\r\n"
    "a=setup:actpass\r\n"
    "a=crypto:0 AES_CM_128_HMAC_SHA1_80 "
    "inline:NzB4d1BINUAvLEw6UzF3WSJ+PSdFcGdUJShpX1Zj|2^20|1:32 "
    "dummy_session_params\r\n";

using ::testing::Exactly;
using ::testing::Values;
using cricket::StreamParams;
using webrtc::AudioSourceInterface;
using webrtc::AudioTrack;
using webrtc::AudioTrackInterface;
using webrtc::DataBuffer;
using webrtc::DataChannelInterface;
using webrtc::FakeConstraints;
using webrtc::IceCandidateInterface;
using webrtc::MediaConstraintsInterface;
using webrtc::MediaStream;
using webrtc::MediaStreamInterface;
using webrtc::MediaStreamTrackInterface;
using webrtc::MockCreateSessionDescriptionObserver;
using webrtc::MockDataChannelObserver;
using webrtc::MockPeerConnectionObserver;
using webrtc::MockSetSessionDescriptionObserver;
using webrtc::MockStatsObserver;
using webrtc::NotifierInterface;
using webrtc::ObserverInterface;
using webrtc::PeerConnectionInterface;
using webrtc::PeerConnectionObserver;
using webrtc::RTCError;
using webrtc::RTCErrorType;
using webrtc::RtpReceiverInterface;
using webrtc::RtpSenderInterface;
using webrtc::RtpSenderProxyWithInternal;
using webrtc::RtpSenderInternal;
using webrtc::RtpTransceiverDirection;
using webrtc::SdpParseError;
using webrtc::SdpSemantics;
using webrtc::SdpType;
using webrtc::SessionDescriptionInterface;
using webrtc::StreamCollection;
using webrtc::StreamCollectionInterface;
using webrtc::VideoTrackSourceInterface;
using webrtc::VideoTrack;
using webrtc::VideoTrackInterface;

using RTCConfiguration = PeerConnectionInterface::RTCConfiguration;
using RTCOfferAnswerOptions = PeerConnectionInterface::RTCOfferAnswerOptions;

namespace {

// Gets the first ssrc of given content type from the ContentInfo.
bool GetFirstSsrc(const cricket::ContentInfo* content_info, int* ssrc) {
  if (!content_info || !ssrc) {
    return false;
  }
  const cricket::MediaContentDescription* media_desc =
      content_info->media_description();
  if (!media_desc || media_desc->streams().empty()) {
    return false;
  }
  *ssrc = media_desc->streams().begin()->first_ssrc();
  return true;
}

// Get the ufrags out of an SDP blob. Useful for testing ICE restart
// behavior.
std::vector<std::string> GetUfrags(
    const webrtc::SessionDescriptionInterface* desc) {
  std::vector<std::string> ufrags;
  for (const cricket::TransportInfo& info :
       desc->description()->transport_infos()) {
    ufrags.push_back(info.description.ice_ufrag);
  }
  return ufrags;
}

void SetSsrcToZero(std::string* sdp) {
  const char kSdpSsrcAtribute[] = "a=ssrc:";
  const char kSdpSsrcAtributeZero[] = "a=ssrc:0";
  size_t ssrc_pos = 0;
  while ((ssrc_pos = sdp->find(kSdpSsrcAtribute, ssrc_pos)) !=
         std::string::npos) {
    size_t end_ssrc = sdp->find(" ", ssrc_pos);
    sdp->replace(ssrc_pos, end_ssrc - ssrc_pos, kSdpSsrcAtributeZero);
    ssrc_pos = end_ssrc;
  }
}

// Check if |streams| contains the specified track.
bool ContainsTrack(const std::vector<cricket::StreamParams>& streams,
                   const std::string& stream_id,
                   const std::string& track_id) {
  for (const cricket::StreamParams& params : streams) {
    if (params.first_stream_id() == stream_id && params.id == track_id) {
      return true;
    }
  }
  return false;
}

// Check if |senders| contains the specified sender, by id.
bool ContainsSender(
    const std::vector<rtc::scoped_refptr<RtpSenderInterface>>& senders,
    const std::string& id) {
  for (const auto& sender : senders) {
    if (sender->id() == id) {
      return true;
    }
  }
  return false;
}

// Check if |senders| contains the specified sender, by id and stream id.
bool ContainsSender(
    const std::vector<rtc::scoped_refptr<RtpSenderInterface>>& senders,
    const std::string& id,
    const std::string& stream_id) {
  for (const auto& sender : senders) {
    if (sender->id() == id && sender->stream_ids()[0] == stream_id) {
      return true;
    }
  }
  return false;
}

// Create a collection of streams.
// CreateStreamCollection(1) creates a collection that
// correspond to kSdpStringWithStream1.
// CreateStreamCollection(2) correspond to kSdpStringWithStream1And2.
rtc::scoped_refptr<StreamCollection> CreateStreamCollection(
    int number_of_streams,
    int tracks_per_stream) {
  rtc::scoped_refptr<StreamCollection> local_collection(
      StreamCollection::Create());

  for (int i = 0; i < number_of_streams; ++i) {
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream(
        webrtc::MediaStream::Create(kStreams[i]));

    for (int j = 0; j < tracks_per_stream; ++j) {
      // Add a local audio track.
      rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
          webrtc::AudioTrack::Create(kAudioTracks[i * tracks_per_stream + j],
                                     nullptr));
      stream->AddTrack(audio_track);

      // Add a local video track.
      rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
          webrtc::VideoTrack::Create(kVideoTracks[i * tracks_per_stream + j],
                                     webrtc::FakeVideoTrackSource::Create(),
                                     rtc::Thread::Current()));
      stream->AddTrack(video_track);
    }

    local_collection->AddStream(stream);
  }
  return local_collection;
}

// Check equality of StreamCollections.
bool CompareStreamCollections(StreamCollectionInterface* s1,
                              StreamCollectionInterface* s2) {
  if (s1 == nullptr || s2 == nullptr || s1->count() != s2->count()) {
    return false;
  }

  for (size_t i = 0; i != s1->count(); ++i) {
    if (s1->at(i)->id() != s2->at(i)->id()) {
      return false;
    }
    webrtc::AudioTrackVector audio_tracks1 = s1->at(i)->GetAudioTracks();
    webrtc::AudioTrackVector audio_tracks2 = s2->at(i)->GetAudioTracks();
    webrtc::VideoTrackVector video_tracks1 = s1->at(i)->GetVideoTracks();
    webrtc::VideoTrackVector video_tracks2 = s2->at(i)->GetVideoTracks();

    if (audio_tracks1.size() != audio_tracks2.size()) {
      return false;
    }
    for (size_t j = 0; j != audio_tracks1.size(); ++j) {
      if (audio_tracks1[j]->id() != audio_tracks2[j]->id()) {
        return false;
      }
    }
    if (video_tracks1.size() != video_tracks2.size()) {
      return false;
    }
    for (size_t j = 0; j != video_tracks1.size(); ++j) {
      if (video_tracks1[j]->id() != video_tracks2[j]->id()) {
        return false;
      }
    }
  }
  return true;
}

// Helper class to test Observer.
class MockTrackObserver : public ObserverInterface {
 public:
  explicit MockTrackObserver(NotifierInterface* notifier)
      : notifier_(notifier) {
    notifier_->RegisterObserver(this);
  }

  ~MockTrackObserver() { Unregister(); }

  void Unregister() {
    if (notifier_) {
      notifier_->UnregisterObserver(this);
      notifier_ = nullptr;
    }
  }

  MOCK_METHOD0(OnChanged, void());

 private:
  NotifierInterface* notifier_;
};

}  // namespace

// The PeerConnectionMediaConfig tests below verify that configuration and
// constraints are propagated into the PeerConnection's MediaConfig. These
// settings are intended for MediaChannel constructors, but that is not
// exercised by these unittest.
class PeerConnectionFactoryForTest : public webrtc::PeerConnectionFactory {
 public:
  static rtc::scoped_refptr<PeerConnectionFactoryForTest>
  CreatePeerConnectionFactoryForTest() {
    auto audio_encoder_factory = webrtc::CreateBuiltinAudioEncoderFactory();
    auto audio_decoder_factory = webrtc::CreateBuiltinAudioDecoderFactory();
    auto video_encoder_factory = webrtc::CreateBuiltinVideoEncoderFactory();
    auto video_decoder_factory = webrtc::CreateBuiltinVideoDecoderFactory();

    // Use fake audio device module since we're only testing the interface
    // level, and using a real one could make tests flaky when run in parallel.
    auto media_engine = std::unique_ptr<cricket::MediaEngineInterface>(
        cricket::WebRtcMediaEngineFactory::Create(
            FakeAudioCaptureModule::Create(), audio_encoder_factory,
            audio_decoder_factory, std::move(video_encoder_factory),
            std::move(video_decoder_factory), nullptr,
            webrtc::AudioProcessingBuilder().Create()));

    std::unique_ptr<webrtc::CallFactoryInterface> call_factory =
        webrtc::CreateCallFactory();

    std::unique_ptr<webrtc::RtcEventLogFactoryInterface> event_log_factory =
        webrtc::CreateRtcEventLogFactory();

    return new rtc::RefCountedObject<PeerConnectionFactoryForTest>(
        rtc::Thread::Current(), rtc::Thread::Current(), rtc::Thread::Current(),
        std::move(media_engine), std::move(call_factory),
        std::move(event_log_factory));
  }

  PeerConnectionFactoryForTest(
      rtc::Thread* network_thread,
      rtc::Thread* worker_thread,
      rtc::Thread* signaling_thread,
      std::unique_ptr<cricket::MediaEngineInterface> media_engine,
      std::unique_ptr<webrtc::CallFactoryInterface> call_factory,
      std::unique_ptr<webrtc::RtcEventLogFactoryInterface> event_log_factory)
      : webrtc::PeerConnectionFactory(network_thread,
                                      worker_thread,
                                      signaling_thread,
                                      std::move(media_engine),
                                      std::move(call_factory),
                                      std::move(event_log_factory)) {}

  rtc::scoped_refptr<FakeAudioCaptureModule> fake_audio_capture_module_;
};

// TODO(steveanton): Convert to use the new PeerConnectionWrapper.
class PeerConnectionInterfaceBaseTest : public testing::Test {
 protected:
  explicit PeerConnectionInterfaceBaseTest(SdpSemantics sdp_semantics)
      : vss_(new rtc::VirtualSocketServer()),
        main_(vss_.get()),
        sdp_semantics_(sdp_semantics) {
#ifdef WEBRTC_ANDROID
    webrtc::InitializeAndroidObjects();
#endif
  }

  virtual void SetUp() {
    // Use fake audio capture module since we're only testing the interface
    // level, and using a real one could make tests flaky when run in parallel.
    fake_audio_capture_module_ = FakeAudioCaptureModule::Create();
    pc_factory_ = webrtc::CreatePeerConnectionFactory(
        rtc::Thread::Current(), rtc::Thread::Current(), rtc::Thread::Current(),
        rtc::scoped_refptr<webrtc::AudioDeviceModule>(
            fake_audio_capture_module_),
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        webrtc::CreateBuiltinVideoEncoderFactory(),
        webrtc::CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
        nullptr /* audio_processing */);
    ASSERT_TRUE(pc_factory_);
    pc_factory_for_test_ =
        PeerConnectionFactoryForTest::CreatePeerConnectionFactoryForTest();
    pc_factory_for_test_->Initialize();
  }

  void CreatePeerConnection() {
    CreatePeerConnection(PeerConnectionInterface::RTCConfiguration(), nullptr);
  }

  // DTLS does not work in a loopback call, so is disabled for most of the
  // tests in this file.
  void CreatePeerConnectionWithoutDtls() {
    FakeConstraints no_dtls_constraints;
    no_dtls_constraints.AddMandatory(
        webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, false);

    CreatePeerConnection(PeerConnectionInterface::RTCConfiguration(),
                         &no_dtls_constraints);
  }

  void CreatePeerConnection(webrtc::MediaConstraintsInterface* constraints) {
    CreatePeerConnection(PeerConnectionInterface::RTCConfiguration(),
                         constraints);
  }

  void CreatePeerConnectionWithIceTransportsType(
      PeerConnectionInterface::IceTransportsType type) {
    PeerConnectionInterface::RTCConfiguration config;
    config.type = type;
    return CreatePeerConnection(config, nullptr);
  }

  void CreatePeerConnectionWithIceServer(const std::string& uri,
                                         const std::string& password) {
    PeerConnectionInterface::RTCConfiguration config;
    PeerConnectionInterface::IceServer server;
    server.uri = uri;
    server.password = password;
    config.servers.push_back(server);
    CreatePeerConnection(config, nullptr);
  }

  void CreatePeerConnection(
      const PeerConnectionInterface::RTCConfiguration& config,
      webrtc::MediaConstraintsInterface* constraints) {
    std::unique_ptr<cricket::FakePortAllocator> port_allocator(
        new cricket::FakePortAllocator(rtc::Thread::Current(), nullptr));
    port_allocator_ = port_allocator.get();

    // Create certificate generator unless DTLS constraint is explicitly set to
    // false.
    std::unique_ptr<rtc::RTCCertificateGeneratorInterface> cert_generator;
    bool dtls;
    if (FindConstraint(constraints,
                       webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                       &dtls, nullptr) &&
        dtls) {
      fake_certificate_generator_ = new FakeRTCCertificateGenerator();
      cert_generator.reset(fake_certificate_generator_);
    }
    RTCConfiguration modified_config = config;
    modified_config.sdp_semantics = sdp_semantics_;
    pc_ = pc_factory_->CreatePeerConnection(
        modified_config, constraints, std::move(port_allocator),
        std::move(cert_generator), &observer_);
    ASSERT_TRUE(pc_.get() != NULL);
    observer_.SetPeerConnectionInterface(pc_.get());
    EXPECT_EQ(PeerConnectionInterface::kStable, observer_.state_);
  }

  void CreatePeerConnectionExpectFail(const std::string& uri) {
    PeerConnectionInterface::RTCConfiguration config;
    PeerConnectionInterface::IceServer server;
    server.uri = uri;
    config.servers.push_back(server);
    config.sdp_semantics = sdp_semantics_;
    rtc::scoped_refptr<PeerConnectionInterface> pc =
        pc_factory_->CreatePeerConnection(config, nullptr, nullptr, nullptr,
                                          &observer_);
    EXPECT_EQ(nullptr, pc);
  }

  void CreatePeerConnectionExpectFail(
      PeerConnectionInterface::RTCConfiguration config) {
    PeerConnectionInterface::IceServer server;
    server.uri = kTurnIceServerUri;
    server.password = kTurnPassword;
    config.servers.push_back(server);
    config.sdp_semantics = sdp_semantics_;
    rtc::scoped_refptr<PeerConnectionInterface> pc =
        pc_factory_->CreatePeerConnection(config, nullptr, nullptr, &observer_);
    EXPECT_EQ(nullptr, pc);
  }

  void CreatePeerConnectionWithDifferentConfigurations() {
    CreatePeerConnectionWithIceServer(kStunAddressOnly, "");
    EXPECT_EQ(1u, port_allocator_->stun_servers().size());
    EXPECT_EQ(0u, port_allocator_->turn_servers().size());
    EXPECT_EQ("address", port_allocator_->stun_servers().begin()->hostname());
    EXPECT_EQ(kDefaultStunPort,
              port_allocator_->stun_servers().begin()->port());

    CreatePeerConnectionExpectFail(kStunInvalidPort);
    CreatePeerConnectionExpectFail(kStunAddressPortAndMore1);
    CreatePeerConnectionExpectFail(kStunAddressPortAndMore2);

    CreatePeerConnectionWithIceServer(kTurnIceServerUri, kTurnPassword);
    EXPECT_EQ(0u, port_allocator_->stun_servers().size());
    EXPECT_EQ(1u, port_allocator_->turn_servers().size());
    EXPECT_EQ(kTurnUsername,
              port_allocator_->turn_servers()[0].credentials.username);
    EXPECT_EQ(kTurnPassword,
              port_allocator_->turn_servers()[0].credentials.password);
    EXPECT_EQ(kTurnHostname,
              port_allocator_->turn_servers()[0].ports[0].address.hostname());
  }

  void ReleasePeerConnection() {
    pc_ = NULL;
    observer_.SetPeerConnectionInterface(NULL);
  }

  rtc::scoped_refptr<VideoTrackInterface> CreateVideoTrack(
      const std::string& label) {
    auto video_source = pc_factory_->CreateVideoSource(
        absl::make_unique<cricket::FakeVideoCapturer>(), nullptr);
    return pc_factory_->CreateVideoTrack(label, video_source);
  }

  void AddVideoTrack(const std::string& track_label,
                     const std::vector<std::string>& stream_ids = {}) {
    auto sender_or_error =
        pc_->AddTrack(CreateVideoTrack(track_label), stream_ids);
    ASSERT_EQ(RTCErrorType::NONE, sender_or_error.error().type());
    EXPECT_TRUE_WAIT(observer_.renegotiation_needed_, kTimeout);
    observer_.renegotiation_needed_ = false;
  }

  void AddVideoStream(const std::string& label) {
    rtc::scoped_refptr<MediaStreamInterface> stream(
        pc_factory_->CreateLocalMediaStream(label));
    stream->AddTrack(CreateVideoTrack(label + "v0"));
    ASSERT_TRUE(pc_->AddStream(stream));
    EXPECT_TRUE_WAIT(observer_.renegotiation_needed_, kTimeout);
    observer_.renegotiation_needed_ = false;
  }

  rtc::scoped_refptr<AudioTrackInterface> CreateAudioTrack(
      const std::string& label) {
    return pc_factory_->CreateAudioTrack(label, nullptr);
  }

  void AddAudioTrack(const std::string& track_label,
                     const std::vector<std::string>& stream_ids = {}) {
    auto sender_or_error =
        pc_->AddTrack(CreateAudioTrack(track_label), stream_ids);
    ASSERT_EQ(RTCErrorType::NONE, sender_or_error.error().type());
    EXPECT_TRUE_WAIT(observer_.renegotiation_needed_, kTimeout);
    observer_.renegotiation_needed_ = false;
  }

  void AddAudioStream(const std::string& label) {
    rtc::scoped_refptr<MediaStreamInterface> stream(
        pc_factory_->CreateLocalMediaStream(label));
    stream->AddTrack(CreateAudioTrack(label + "a0"));
    ASSERT_TRUE(pc_->AddStream(stream));
    EXPECT_TRUE_WAIT(observer_.renegotiation_needed_, kTimeout);
    observer_.renegotiation_needed_ = false;
  }

  void AddAudioVideoStream(const std::string& stream_id,
                           const std::string& audio_track_label,
                           const std::string& video_track_label) {
    // Create a local stream.
    rtc::scoped_refptr<MediaStreamInterface> stream(
        pc_factory_->CreateLocalMediaStream(stream_id));
    stream->AddTrack(CreateAudioTrack(audio_track_label));
    stream->AddTrack(CreateVideoTrack(video_track_label));
    ASSERT_TRUE(pc_->AddStream(stream));
    EXPECT_TRUE_WAIT(observer_.renegotiation_needed_, kTimeout);
    observer_.renegotiation_needed_ = false;
  }

  rtc::scoped_refptr<RtpReceiverInterface> GetFirstReceiverOfType(
      cricket::MediaType media_type) {
    for (auto receiver : pc_->GetReceivers()) {
      if (receiver->media_type() == media_type) {
        return receiver;
      }
    }
    return nullptr;
  }

  bool DoCreateOfferAnswer(std::unique_ptr<SessionDescriptionInterface>* desc,
                           bool offer,
                           MediaConstraintsInterface* constraints) {
    rtc::scoped_refptr<MockCreateSessionDescriptionObserver> observer(
        new rtc::RefCountedObject<MockCreateSessionDescriptionObserver>());
    if (offer) {
      pc_->CreateOffer(observer, constraints);
    } else {
      pc_->CreateAnswer(observer, constraints);
    }
    EXPECT_EQ_WAIT(true, observer->called(), kTimeout);
    *desc = observer->MoveDescription();
    return observer->result();
  }

  bool DoCreateOffer(std::unique_ptr<SessionDescriptionInterface>* desc,
                     MediaConstraintsInterface* constraints) {
    return DoCreateOfferAnswer(desc, true, constraints);
  }

  bool DoCreateAnswer(std::unique_ptr<SessionDescriptionInterface>* desc,
                      MediaConstraintsInterface* constraints) {
    return DoCreateOfferAnswer(desc, false, constraints);
  }

  bool DoSetSessionDescription(
      std::unique_ptr<SessionDescriptionInterface> desc,
      bool local) {
    rtc::scoped_refptr<MockSetSessionDescriptionObserver> observer(
        new rtc::RefCountedObject<MockSetSessionDescriptionObserver>());
    if (local) {
      pc_->SetLocalDescription(observer, desc.release());
    } else {
      pc_->SetRemoteDescription(observer, desc.release());
    }
    if (pc_->signaling_state() != PeerConnectionInterface::kClosed) {
      EXPECT_EQ_WAIT(true, observer->called(), kTimeout);
    }
    return observer->result();
  }

  bool DoSetLocalDescription(
      std::unique_ptr<SessionDescriptionInterface> desc) {
    return DoSetSessionDescription(std::move(desc), true);
  }

  bool DoSetRemoteDescription(
      std::unique_ptr<SessionDescriptionInterface> desc) {
    return DoSetSessionDescription(std::move(desc), false);
  }

  // Calls PeerConnection::GetStats and check the return value.
  // It does not verify the values in the StatReports since a RTCP packet might
  // be required.
  bool DoGetStats(MediaStreamTrackInterface* track) {
    rtc::scoped_refptr<MockStatsObserver> observer(
        new rtc::RefCountedObject<MockStatsObserver>());
    if (!pc_->GetStats(observer, track,
                       PeerConnectionInterface::kStatsOutputLevelStandard))
      return false;
    EXPECT_TRUE_WAIT(observer->called(), kTimeout);
    return observer->called();
  }

  // Call the standards-compliant GetStats function.
  bool DoGetRTCStats() {
    rtc::scoped_refptr<webrtc::MockRTCStatsCollectorCallback> callback(
        new rtc::RefCountedObject<webrtc::MockRTCStatsCollectorCallback>());
    pc_->GetStats(callback);
    EXPECT_TRUE_WAIT(callback->called(), kTimeout);
    return callback->called();
  }

  void InitiateCall() {
    CreatePeerConnectionWithoutDtls();
    // Create a local stream with audio&video tracks.
    if (sdp_semantics_ == SdpSemantics::kPlanB) {
      AddAudioVideoStream(kStreamId1, "audio_track", "video_track");
    } else {
      // Unified Plan does not support AddStream, so just add an audio and video
      // track.
      AddAudioTrack(kAudioTracks[0], {kStreamId1});
      AddVideoTrack(kVideoTracks[0], {kStreamId1});
    }
    CreateOfferReceiveAnswer();
  }

  // Verify that RTP Header extensions has been negotiated for audio and video.
  void VerifyRemoteRtpHeaderExtensions() {
    const cricket::MediaContentDescription* desc =
        cricket::GetFirstAudioContentDescription(
            pc_->remote_description()->description());
    ASSERT_TRUE(desc != NULL);
    EXPECT_GT(desc->rtp_header_extensions().size(), 0u);

    desc = cricket::GetFirstVideoContentDescription(
        pc_->remote_description()->description());
    ASSERT_TRUE(desc != NULL);
    EXPECT_GT(desc->rtp_header_extensions().size(), 0u);
  }

  void CreateOfferAsRemoteDescription() {
    std::unique_ptr<SessionDescriptionInterface> offer;
    ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
    std::string sdp;
    EXPECT_TRUE(offer->ToString(&sdp));
    std::unique_ptr<SessionDescriptionInterface> remote_offer(
        webrtc::CreateSessionDescription(SdpType::kOffer, sdp));
    EXPECT_TRUE(DoSetRemoteDescription(std::move(remote_offer)));
    EXPECT_EQ(PeerConnectionInterface::kHaveRemoteOffer, observer_.state_);
  }

  void CreateAndSetRemoteOffer(const std::string& sdp) {
    std::unique_ptr<SessionDescriptionInterface> remote_offer(
        webrtc::CreateSessionDescription(SdpType::kOffer, sdp));
    EXPECT_TRUE(DoSetRemoteDescription(std::move(remote_offer)));
    EXPECT_EQ(PeerConnectionInterface::kHaveRemoteOffer, observer_.state_);
  }

  void CreateAnswerAsLocalDescription() {
    std::unique_ptr<SessionDescriptionInterface> answer;
    ASSERT_TRUE(DoCreateAnswer(&answer, nullptr));

    // TODO(perkj): Currently SetLocalDescription fails if any parameters in an
    // audio codec change, even if the parameter has nothing to do with
    // receiving. Not all parameters are serialized to SDP.
    // Since CreatePrAnswerAsLocalDescription serialize/deserialize
    // the SessionDescription, it is necessary to do that here to in order to
    // get ReceiveOfferCreatePrAnswerAndAnswer and RenegotiateAudioOnly to pass.
    // https://code.google.com/p/webrtc/issues/detail?id=1356
    std::string sdp;
    EXPECT_TRUE(answer->ToString(&sdp));
    std::unique_ptr<SessionDescriptionInterface> new_answer(
        webrtc::CreateSessionDescription(SdpType::kAnswer, sdp));
    EXPECT_TRUE(DoSetLocalDescription(std::move(new_answer)));
    EXPECT_EQ(PeerConnectionInterface::kStable, observer_.state_);
  }

  void CreatePrAnswerAsLocalDescription() {
    std::unique_ptr<SessionDescriptionInterface> answer;
    ASSERT_TRUE(DoCreateAnswer(&answer, nullptr));

    std::string sdp;
    EXPECT_TRUE(answer->ToString(&sdp));
    std::unique_ptr<SessionDescriptionInterface> pr_answer(
        webrtc::CreateSessionDescription(SdpType::kPrAnswer, sdp));
    EXPECT_TRUE(DoSetLocalDescription(std::move(pr_answer)));
    EXPECT_EQ(PeerConnectionInterface::kHaveLocalPrAnswer, observer_.state_);
  }

  void CreateOfferReceiveAnswer() {
    CreateOfferAsLocalDescription();
    std::string sdp;
    EXPECT_TRUE(pc_->local_description()->ToString(&sdp));
    CreateAnswerAsRemoteDescription(sdp);
  }

  void CreateOfferAsLocalDescription() {
    std::unique_ptr<SessionDescriptionInterface> offer;
    ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
    // TODO(perkj): Currently SetLocalDescription fails if any parameters in an
    // audio codec change, even if the parameter has nothing to do with
    // receiving. Not all parameters are serialized to SDP.
    // Since CreatePrAnswerAsLocalDescription serialize/deserialize
    // the SessionDescription, it is necessary to do that here to in order to
    // get ReceiveOfferCreatePrAnswerAndAnswer and RenegotiateAudioOnly to pass.
    // https://code.google.com/p/webrtc/issues/detail?id=1356
    std::string sdp;
    EXPECT_TRUE(offer->ToString(&sdp));
    std::unique_ptr<SessionDescriptionInterface> new_offer(
        webrtc::CreateSessionDescription(SdpType::kOffer, sdp));

    EXPECT_TRUE(DoSetLocalDescription(std::move(new_offer)));
    EXPECT_EQ(PeerConnectionInterface::kHaveLocalOffer, observer_.state_);
    // Wait for the ice_complete message, so that SDP will have candidates.
    EXPECT_TRUE_WAIT(observer_.ice_gathering_complete_, kTimeout);
  }

  void CreateAnswerAsRemoteDescription(const std::string& sdp) {
    std::unique_ptr<SessionDescriptionInterface> answer(
        webrtc::CreateSessionDescription(SdpType::kAnswer, sdp));
    ASSERT_TRUE(answer);
    EXPECT_TRUE(DoSetRemoteDescription(std::move(answer)));
    EXPECT_EQ(PeerConnectionInterface::kStable, observer_.state_);
  }

  void CreatePrAnswerAndAnswerAsRemoteDescription(const std::string& sdp) {
    std::unique_ptr<SessionDescriptionInterface> pr_answer(
        webrtc::CreateSessionDescription(SdpType::kPrAnswer, sdp));
    ASSERT_TRUE(pr_answer);
    EXPECT_TRUE(DoSetRemoteDescription(std::move(pr_answer)));
    EXPECT_EQ(PeerConnectionInterface::kHaveRemotePrAnswer, observer_.state_);
    std::unique_ptr<SessionDescriptionInterface> answer(
        webrtc::CreateSessionDescription(SdpType::kAnswer, sdp));
    ASSERT_TRUE(answer);
    EXPECT_TRUE(DoSetRemoteDescription(std::move(answer)));
    EXPECT_EQ(PeerConnectionInterface::kStable, observer_.state_);
  }

  // Waits until a remote stream with the given id is signaled. This helper
  // function will verify both OnAddTrack and OnAddStream (Plan B only) are
  // called with the given stream id and expected number of tracks.
  void WaitAndVerifyOnAddStream(const std::string& stream_id,
                                int expected_num_tracks) {
    // Verify that both OnAddStream and OnAddTrack are called.
    EXPECT_EQ_WAIT(stream_id, observer_.GetLastAddedStreamId(), kTimeout);
    EXPECT_EQ_WAIT(expected_num_tracks,
                   observer_.CountAddTrackEventsForStream(stream_id), kTimeout);
  }

  // Creates an offer and applies it as a local session description.
  // Creates an answer with the same SDP an the offer but removes all lines
  // that start with a:ssrc"
  void CreateOfferReceiveAnswerWithoutSsrc() {
    CreateOfferAsLocalDescription();
    std::string sdp;
    EXPECT_TRUE(pc_->local_description()->ToString(&sdp));
    SetSsrcToZero(&sdp);
    CreateAnswerAsRemoteDescription(sdp);
  }

  // This function creates a MediaStream with label kStreams[0] and
  // |number_of_audio_tracks| and |number_of_video_tracks| tracks and the
  // corresponding SessionDescriptionInterface. The SessionDescriptionInterface
  // is returned and the MediaStream is stored in
  // |reference_collection_|
  std::unique_ptr<SessionDescriptionInterface>
  CreateSessionDescriptionAndReference(size_t number_of_audio_tracks,
                                       size_t number_of_video_tracks) {
    EXPECT_LE(number_of_audio_tracks, 2u);
    EXPECT_LE(number_of_video_tracks, 2u);

    reference_collection_ = StreamCollection::Create();
    std::string sdp_ms1 = std::string(kSdpStringInit);

    std::string mediastream_id = kStreams[0];

    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream(
        webrtc::MediaStream::Create(mediastream_id));
    reference_collection_->AddStream(stream);

    if (number_of_audio_tracks > 0) {
      sdp_ms1 += std::string(kSdpStringAudio);
      sdp_ms1 += std::string(kSdpStringMs1Audio0);
      AddAudioTrack(kAudioTracks[0], stream);
    }
    if (number_of_audio_tracks > 1) {
      sdp_ms1 += kSdpStringMs1Audio1;
      AddAudioTrack(kAudioTracks[1], stream);
    }

    if (number_of_video_tracks > 0) {
      sdp_ms1 += std::string(kSdpStringVideo);
      sdp_ms1 += std::string(kSdpStringMs1Video0);
      AddVideoTrack(kVideoTracks[0], stream);
    }
    if (number_of_video_tracks > 1) {
      sdp_ms1 += kSdpStringMs1Video1;
      AddVideoTrack(kVideoTracks[1], stream);
    }

    return std::unique_ptr<SessionDescriptionInterface>(
        webrtc::CreateSessionDescription(SdpType::kOffer, sdp_ms1));
  }

  void AddAudioTrack(const std::string& track_id,
                     MediaStreamInterface* stream) {
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
        webrtc::AudioTrack::Create(track_id, nullptr));
    ASSERT_TRUE(stream->AddTrack(audio_track));
  }

  void AddVideoTrack(const std::string& track_id,
                     MediaStreamInterface* stream) {
    rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
        webrtc::VideoTrack::Create(track_id,
                                   webrtc::FakeVideoTrackSource::Create(),
                                   rtc::Thread::Current()));
    ASSERT_TRUE(stream->AddTrack(video_track));
  }

  std::unique_ptr<SessionDescriptionInterface> CreateOfferWithOneAudioTrack() {
    CreatePeerConnectionWithoutDtls();
    AddAudioTrack(kAudioTracks[0]);
    std::unique_ptr<SessionDescriptionInterface> offer;
    EXPECT_TRUE(DoCreateOffer(&offer, nullptr));
    return offer;
  }

  std::unique_ptr<SessionDescriptionInterface> CreateOfferWithOneAudioStream() {
    CreatePeerConnectionWithoutDtls();
    AddAudioStream(kStreamId1);
    std::unique_ptr<SessionDescriptionInterface> offer;
    EXPECT_TRUE(DoCreateOffer(&offer, nullptr));
    return offer;
  }

  std::unique_ptr<SessionDescriptionInterface> CreateAnswerWithOneAudioTrack() {
    EXPECT_TRUE(DoSetRemoteDescription(CreateOfferWithOneAudioTrack()));
    std::unique_ptr<SessionDescriptionInterface> answer;
    EXPECT_TRUE(DoCreateAnswer(&answer, nullptr));
    return answer;
  }

  std::unique_ptr<SessionDescriptionInterface>
  CreateAnswerWithOneAudioStream() {
    EXPECT_TRUE(DoSetRemoteDescription(CreateOfferWithOneAudioStream()));
    std::unique_ptr<SessionDescriptionInterface> answer;
    EXPECT_TRUE(DoCreateAnswer(&answer, nullptr));
    return answer;
  }

  const std::string& GetFirstAudioStreamCname(
      const SessionDescriptionInterface* desc) {
    const cricket::AudioContentDescription* audio_desc =
        cricket::GetFirstAudioContentDescription(desc->description());
    return audio_desc->streams()[0].cname;
  }

  std::unique_ptr<SessionDescriptionInterface> CreateOfferWithOptions(
      const RTCOfferAnswerOptions& offer_answer_options) {
    RTC_DCHECK(pc_);
    rtc::scoped_refptr<MockCreateSessionDescriptionObserver> observer(
        new rtc::RefCountedObject<MockCreateSessionDescriptionObserver>());
    pc_->CreateOffer(observer, offer_answer_options);
    EXPECT_EQ_WAIT(true, observer->called(), kTimeout);
    return observer->MoveDescription();
  }

  void CreateOfferWithOptionsAsRemoteDescription(
      std::unique_ptr<SessionDescriptionInterface>* desc,
      const RTCOfferAnswerOptions& offer_answer_options) {
    *desc = CreateOfferWithOptions(offer_answer_options);
    ASSERT_TRUE(desc != nullptr);
    std::string sdp;
    EXPECT_TRUE((*desc)->ToString(&sdp));
    std::unique_ptr<SessionDescriptionInterface> remote_offer(
        webrtc::CreateSessionDescription(SdpType::kOffer, sdp));
    EXPECT_TRUE(DoSetRemoteDescription(std::move(remote_offer)));
    EXPECT_EQ(PeerConnectionInterface::kHaveRemoteOffer, observer_.state_);
  }

  void CreateOfferWithOptionsAsLocalDescription(
      std::unique_ptr<SessionDescriptionInterface>* desc,
      const RTCOfferAnswerOptions& offer_answer_options) {
    *desc = CreateOfferWithOptions(offer_answer_options);
    ASSERT_TRUE(desc != nullptr);
    std::string sdp;
    EXPECT_TRUE((*desc)->ToString(&sdp));
    std::unique_ptr<SessionDescriptionInterface> new_offer(
        webrtc::CreateSessionDescription(SdpType::kOffer, sdp));

    EXPECT_TRUE(DoSetLocalDescription(std::move(new_offer)));
    EXPECT_EQ(PeerConnectionInterface::kHaveLocalOffer, observer_.state_);
  }

  bool HasCNCodecs(const cricket::ContentInfo* content) {
    RTC_DCHECK(content);
    RTC_DCHECK(content->media_description());
    for (const cricket::AudioCodec& codec :
         content->media_description()->as_audio()->codecs()) {
      if (codec.name == "CN") {
        return true;
      }
    }
    return false;
  }

  const char* GetSdpStringWithStream1() const {
    if (sdp_semantics_ == SdpSemantics::kPlanB) {
      return kSdpStringWithStream1PlanB;
    } else {
      return kSdpStringWithStream1UnifiedPlan;
    }
  }

  const char* GetSdpStringWithStream1And2() const {
    if (sdp_semantics_ == SdpSemantics::kPlanB) {
      return kSdpStringWithStream1And2PlanB;
    } else {
      return kSdpStringWithStream1And2UnifiedPlan;
    }
  }

  std::unique_ptr<rtc::VirtualSocketServer> vss_;
  rtc::AutoSocketServerThread main_;
  rtc::scoped_refptr<FakeAudioCaptureModule> fake_audio_capture_module_;
  cricket::FakePortAllocator* port_allocator_ = nullptr;
  FakeRTCCertificateGenerator* fake_certificate_generator_ = nullptr;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory_;
  rtc::scoped_refptr<PeerConnectionFactoryForTest> pc_factory_for_test_;
  rtc::scoped_refptr<PeerConnectionInterface> pc_;
  MockPeerConnectionObserver observer_;
  rtc::scoped_refptr<StreamCollection> reference_collection_;
  const SdpSemantics sdp_semantics_;
};

class PeerConnectionInterfaceTest
    : public PeerConnectionInterfaceBaseTest,
      public ::testing::WithParamInterface<SdpSemantics> {
 protected:
  PeerConnectionInterfaceTest() : PeerConnectionInterfaceBaseTest(GetParam()) {}
};

class PeerConnectionInterfaceTestPlanB
    : public PeerConnectionInterfaceBaseTest {
 protected:
  PeerConnectionInterfaceTestPlanB()
      : PeerConnectionInterfaceBaseTest(SdpSemantics::kPlanB) {}
};

// Generate different CNAMEs when PeerConnections are created.
// The CNAMEs are expected to be generated randomly. It is possible
// that the test fails, though the possibility is very low.
TEST_P(PeerConnectionInterfaceTest, CnameGenerationInOffer) {
  std::unique_ptr<SessionDescriptionInterface> offer1 =
      CreateOfferWithOneAudioTrack();
  std::unique_ptr<SessionDescriptionInterface> offer2 =
      CreateOfferWithOneAudioTrack();
  EXPECT_NE(GetFirstAudioStreamCname(offer1.get()),
            GetFirstAudioStreamCname(offer2.get()));
}

TEST_P(PeerConnectionInterfaceTest, CnameGenerationInAnswer) {
  std::unique_ptr<SessionDescriptionInterface> answer1 =
      CreateAnswerWithOneAudioTrack();
  std::unique_ptr<SessionDescriptionInterface> answer2 =
      CreateAnswerWithOneAudioTrack();
  EXPECT_NE(GetFirstAudioStreamCname(answer1.get()),
            GetFirstAudioStreamCname(answer2.get()));
}

TEST_P(PeerConnectionInterfaceTest,
       CreatePeerConnectionWithDifferentConfigurations) {
  CreatePeerConnectionWithDifferentConfigurations();
}

TEST_P(PeerConnectionInterfaceTest,
       CreatePeerConnectionWithDifferentIceTransportsTypes) {
  CreatePeerConnectionWithIceTransportsType(PeerConnectionInterface::kNone);
  EXPECT_EQ(cricket::CF_NONE, port_allocator_->candidate_filter());
  CreatePeerConnectionWithIceTransportsType(PeerConnectionInterface::kRelay);
  EXPECT_EQ(cricket::CF_RELAY, port_allocator_->candidate_filter());
  CreatePeerConnectionWithIceTransportsType(PeerConnectionInterface::kNoHost);
  EXPECT_EQ(cricket::CF_ALL & ~cricket::CF_HOST,
            port_allocator_->candidate_filter());
  CreatePeerConnectionWithIceTransportsType(PeerConnectionInterface::kAll);
  EXPECT_EQ(cricket::CF_ALL, port_allocator_->candidate_filter());
}

// Test that when a PeerConnection is created with a nonzero candidate pool
// size, the pooled PortAllocatorSession is created with all the attributes
// in the RTCConfiguration.
TEST_P(PeerConnectionInterfaceTest, CreatePeerConnectionWithPooledCandidates) {
  PeerConnectionInterface::RTCConfiguration config;
  PeerConnectionInterface::IceServer server;
  server.uri = kStunAddressOnly;
  config.servers.push_back(server);
  config.type = PeerConnectionInterface::kRelay;
  config.disable_ipv6 = true;
  config.tcp_candidate_policy =
      PeerConnectionInterface::kTcpCandidatePolicyDisabled;
  config.candidate_network_policy =
      PeerConnectionInterface::kCandidateNetworkPolicyLowCost;
  config.ice_candidate_pool_size = 1;
  CreatePeerConnection(config, nullptr);

  const cricket::FakePortAllocatorSession* session =
      static_cast<const cricket::FakePortAllocatorSession*>(
          port_allocator_->GetPooledSession());
  ASSERT_NE(nullptr, session);
  EXPECT_EQ(1UL, session->stun_servers().size());
  EXPECT_EQ(0U, session->flags() & cricket::PORTALLOCATOR_ENABLE_IPV6);
  EXPECT_LT(0U, session->flags() & cricket::PORTALLOCATOR_DISABLE_TCP);
  EXPECT_LT(0U,
            session->flags() & cricket::PORTALLOCATOR_DISABLE_COSTLY_NETWORKS);
}

// Test that network-related RTCConfiguration members are applied to the
// PortAllocator when CreatePeerConnection is called. Specifically:
// - disable_ipv6_on_wifi
// - max_ipv6_networks
// - tcp_candidate_policy
// - candidate_network_policy
// - prune_turn_ports
//
// Note that the candidate filter (RTCConfiguration::type) is already tested
// above.
TEST_P(PeerConnectionInterfaceTest,
       CreatePeerConnectionAppliesNetworkConfigToPortAllocator) {
  // Create fake port allocator.
  std::unique_ptr<cricket::FakePortAllocator> port_allocator(
      new cricket::FakePortAllocator(rtc::Thread::Current(), nullptr));
  cricket::FakePortAllocator* raw_port_allocator = port_allocator.get();

  // Create RTCConfiguration with some network-related fields relevant to
  // PortAllocator populated.
  PeerConnectionInterface::RTCConfiguration config;
  config.disable_ipv6_on_wifi = true;
  config.max_ipv6_networks = 10;
  config.tcp_candidate_policy =
      PeerConnectionInterface::kTcpCandidatePolicyDisabled;
  config.candidate_network_policy =
      PeerConnectionInterface::kCandidateNetworkPolicyLowCost;
  config.prune_turn_ports = true;

  // Create the PC factory and PC with the above config.
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory(
      webrtc::CreatePeerConnectionFactory(
          rtc::Thread::Current(), rtc::Thread::Current(),
          rtc::Thread::Current(), fake_audio_capture_module_,
          webrtc::CreateBuiltinAudioEncoderFactory(),
          webrtc::CreateBuiltinAudioDecoderFactory(),
          webrtc::CreateBuiltinVideoEncoderFactory(),
          webrtc::CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
          nullptr /* audio_processing */));
  rtc::scoped_refptr<PeerConnectionInterface> pc(
      pc_factory->CreatePeerConnection(
          config, nullptr, std::move(port_allocator), nullptr, &observer_));

  // Now validate that the config fields set above were applied to the
  // PortAllocator, as flags or otherwise.
  EXPECT_FALSE(raw_port_allocator->flags() &
               cricket::PORTALLOCATOR_ENABLE_IPV6_ON_WIFI);
  EXPECT_EQ(10, raw_port_allocator->max_ipv6_networks());
  EXPECT_TRUE(raw_port_allocator->flags() & cricket::PORTALLOCATOR_DISABLE_TCP);
  EXPECT_TRUE(raw_port_allocator->flags() &
              cricket::PORTALLOCATOR_DISABLE_COSTLY_NETWORKS);
  EXPECT_TRUE(raw_port_allocator->prune_turn_ports());
}

// Check that GetConfiguration returns the configuration the PeerConnection was
// constructed with, before SetConfiguration is called.
TEST_P(PeerConnectionInterfaceTest, GetConfigurationAfterCreatePeerConnection) {
  PeerConnectionInterface::RTCConfiguration config;
  config.type = PeerConnectionInterface::kRelay;
  CreatePeerConnection(config, nullptr);

  PeerConnectionInterface::RTCConfiguration returned_config =
      pc_->GetConfiguration();
  EXPECT_EQ(PeerConnectionInterface::kRelay, returned_config.type);
}

// Check that GetConfiguration returns the last configuration passed into
// SetConfiguration.
TEST_P(PeerConnectionInterfaceTest, GetConfigurationAfterSetConfiguration) {
  CreatePeerConnection();

  PeerConnectionInterface::RTCConfiguration config = pc_->GetConfiguration();
  config.type = PeerConnectionInterface::kRelay;
  EXPECT_TRUE(pc_->SetConfiguration(config));

  PeerConnectionInterface::RTCConfiguration returned_config =
      pc_->GetConfiguration();
  EXPECT_EQ(PeerConnectionInterface::kRelay, returned_config.type);
}

TEST_P(PeerConnectionInterfaceTest, SetConfigurationFailsAfterClose) {
  CreatePeerConnection();

  pc_->Close();

  EXPECT_FALSE(
      pc_->SetConfiguration(PeerConnectionInterface::RTCConfiguration()));
}

TEST_F(PeerConnectionInterfaceTestPlanB, AddStreams) {
  CreatePeerConnectionWithoutDtls();
  AddVideoStream(kStreamId1);
  AddAudioStream(kStreamId2);
  ASSERT_EQ(2u, pc_->local_streams()->count());

  // Test we can add multiple local streams to one peerconnection.
  rtc::scoped_refptr<MediaStreamInterface> stream(
      pc_factory_->CreateLocalMediaStream(kStreamId3));
  rtc::scoped_refptr<AudioTrackInterface> audio_track(
      pc_factory_->CreateAudioTrack(kStreamId3,
                                    static_cast<AudioSourceInterface*>(NULL)));
  stream->AddTrack(audio_track.get());
  EXPECT_TRUE(pc_->AddStream(stream));
  EXPECT_EQ(3u, pc_->local_streams()->count());

  // Remove the third stream.
  pc_->RemoveStream(pc_->local_streams()->at(2));
  EXPECT_EQ(2u, pc_->local_streams()->count());

  // Remove the second stream.
  pc_->RemoveStream(pc_->local_streams()->at(1));
  EXPECT_EQ(1u, pc_->local_streams()->count());

  // Remove the first stream.
  pc_->RemoveStream(pc_->local_streams()->at(0));
  EXPECT_EQ(0u, pc_->local_streams()->count());
}

// Test that the created offer includes streams we added.
// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB, AddedStreamsPresentInOffer) {
  CreatePeerConnectionWithoutDtls();
  AddAudioVideoStream(kStreamId1, "audio_track", "video_track");
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));

  const cricket::AudioContentDescription* audio_desc =
      cricket::GetFirstAudioContentDescription(offer->description());
  EXPECT_TRUE(ContainsTrack(audio_desc->streams(), kStreamId1, "audio_track"));

  const cricket::VideoContentDescription* video_desc =
      cricket::GetFirstVideoContentDescription(offer->description());
  EXPECT_TRUE(ContainsTrack(video_desc->streams(), kStreamId1, "video_track"));

  // Add another stream and ensure the offer includes both the old and new
  // streams.
  AddAudioVideoStream(kStreamId2, "audio_track2", "video_track2");
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));

  audio_desc = cricket::GetFirstAudioContentDescription(offer->description());
  EXPECT_TRUE(ContainsTrack(audio_desc->streams(), kStreamId1, "audio_track"));
  EXPECT_TRUE(ContainsTrack(audio_desc->streams(), kStreamId2, "audio_track2"));

  video_desc = cricket::GetFirstVideoContentDescription(offer->description());
  EXPECT_TRUE(ContainsTrack(video_desc->streams(), kStreamId1, "video_track"));
  EXPECT_TRUE(ContainsTrack(video_desc->streams(), kStreamId2, "video_track2"));
}

// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB, RemoveStream) {
  CreatePeerConnectionWithoutDtls();
  AddVideoStream(kStreamId1);
  ASSERT_EQ(1u, pc_->local_streams()->count());
  pc_->RemoveStream(pc_->local_streams()->at(0));
  EXPECT_EQ(0u, pc_->local_streams()->count());
}

// Test for AddTrack and RemoveTrack methods.
// Tests that the created offer includes tracks we added,
// and that the RtpSenders are created correctly.
// Also tests that RemoveTrack removes the tracks from subsequent offers.
// Only tested with Plan B since Unified Plan is covered in more detail by tests
// in peerconnection_jsep_unittests.cc
TEST_F(PeerConnectionInterfaceTestPlanB, AddTrackRemoveTrack) {
  CreatePeerConnectionWithoutDtls();
  rtc::scoped_refptr<AudioTrackInterface> audio_track(
      pc_factory_->CreateAudioTrack("audio_track", nullptr));
  rtc::scoped_refptr<VideoTrackInterface> video_track(
      pc_factory_->CreateVideoTrack(
          "video_track", pc_factory_->CreateVideoSource(
                             std::unique_ptr<cricket::VideoCapturer>(
                                 new cricket::FakeVideoCapturer()))));
  auto audio_sender = pc_->AddTrack(audio_track, {kStreamId1}).MoveValue();
  auto video_sender = pc_->AddTrack(video_track, {kStreamId1}).MoveValue();
  EXPECT_EQ(1UL, audio_sender->stream_ids().size());
  EXPECT_EQ(kStreamId1, audio_sender->stream_ids()[0]);
  EXPECT_EQ("audio_track", audio_sender->id());
  EXPECT_EQ(audio_track, audio_sender->track());
  EXPECT_EQ(1UL, video_sender->stream_ids().size());
  EXPECT_EQ(kStreamId1, video_sender->stream_ids()[0]);
  EXPECT_EQ("video_track", video_sender->id());
  EXPECT_EQ(video_track, video_sender->track());

  // Now create an offer and check for the senders.
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));

  const cricket::ContentInfo* audio_content =
      cricket::GetFirstAudioContent(offer->description());
  EXPECT_TRUE(ContainsTrack(audio_content->media_description()->streams(),
                            kStreamId1, "audio_track"));

  const cricket::ContentInfo* video_content =
      cricket::GetFirstVideoContent(offer->description());
  EXPECT_TRUE(ContainsTrack(video_content->media_description()->streams(),
                            kStreamId1, "video_track"));

  EXPECT_TRUE(DoSetLocalDescription(std::move(offer)));

  // Now try removing the tracks.
  EXPECT_TRUE(pc_->RemoveTrack(audio_sender));
  EXPECT_TRUE(pc_->RemoveTrack(video_sender));

  // Create a new offer and ensure it doesn't contain the removed senders.
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));

  audio_content = cricket::GetFirstAudioContent(offer->description());
  EXPECT_FALSE(ContainsTrack(audio_content->media_description()->streams(),
                             kStreamId1, "audio_track"));

  video_content = cricket::GetFirstVideoContent(offer->description());
  EXPECT_FALSE(ContainsTrack(video_content->media_description()->streams(),
                             kStreamId1, "video_track"));

  EXPECT_TRUE(DoSetLocalDescription(std::move(offer)));

  // Calling RemoveTrack on a sender no longer attached to a PeerConnection
  // should return false.
  EXPECT_FALSE(pc_->RemoveTrack(audio_sender));
  EXPECT_FALSE(pc_->RemoveTrack(video_sender));
}

// Test creating senders without a stream specified,
// expecting a random stream ID to be generated.
TEST_P(PeerConnectionInterfaceTest, AddTrackWithoutStream) {
  CreatePeerConnectionWithoutDtls();
  rtc::scoped_refptr<AudioTrackInterface> audio_track(
      pc_factory_->CreateAudioTrack("audio_track", nullptr));
  rtc::scoped_refptr<VideoTrackInterface> video_track(
      pc_factory_->CreateVideoTrack(
          "video_track", pc_factory_->CreateVideoSource(
                             std::unique_ptr<cricket::VideoCapturer>(
                                 new cricket::FakeVideoCapturer()))));
  auto audio_sender =
      pc_->AddTrack(audio_track, std::vector<std::string>()).MoveValue();
  auto video_sender =
      pc_->AddTrack(video_track, std::vector<std::string>()).MoveValue();
  EXPECT_EQ("audio_track", audio_sender->id());
  EXPECT_EQ(audio_track, audio_sender->track());
  EXPECT_EQ("video_track", video_sender->id());
  EXPECT_EQ(video_track, video_sender->track());
  if (sdp_semantics_ == SdpSemantics::kPlanB) {
    // If the ID is truly a random GUID, it should be infinitely unlikely they
    // will be the same.
    EXPECT_NE(video_sender->stream_ids(), audio_sender->stream_ids());
  } else {
    // We allows creating tracks without stream ids under Unified Plan
    // semantics.
    EXPECT_EQ(0u, video_sender->stream_ids().size());
    EXPECT_EQ(0u, audio_sender->stream_ids().size());
  }
}

// Test that we can call GetStats() after AddTrack but before connecting
// the PeerConnection to a peer.
TEST_P(PeerConnectionInterfaceTest, AddTrackBeforeConnecting) {
  CreatePeerConnectionWithoutDtls();
  rtc::scoped_refptr<AudioTrackInterface> audio_track(
      pc_factory_->CreateAudioTrack("audio_track", nullptr));
  rtc::scoped_refptr<VideoTrackInterface> video_track(
      pc_factory_->CreateVideoTrack(
          "video_track", pc_factory_->CreateVideoSource(
                             std::unique_ptr<cricket::VideoCapturer>(
                                 new cricket::FakeVideoCapturer()))));
  auto audio_sender = pc_->AddTrack(audio_track, std::vector<std::string>());
  auto video_sender = pc_->AddTrack(video_track, std::vector<std::string>());
  EXPECT_TRUE(DoGetStats(nullptr));
}

TEST_P(PeerConnectionInterfaceTest, AttachmentIdIsSetOnAddTrack) {
  CreatePeerConnectionWithoutDtls();
  rtc::scoped_refptr<AudioTrackInterface> audio_track(
      pc_factory_->CreateAudioTrack("audio_track", nullptr));
  rtc::scoped_refptr<VideoTrackInterface> video_track(
      pc_factory_->CreateVideoTrack(
          "video_track", pc_factory_->CreateVideoSource(
                             std::unique_ptr<cricket::VideoCapturer>(
                                 new cricket::FakeVideoCapturer()))));
  auto audio_sender = pc_->AddTrack(audio_track, std::vector<std::string>());
  ASSERT_TRUE(audio_sender.ok());
  auto* audio_sender_proxy =
      static_cast<RtpSenderProxyWithInternal<RtpSenderInternal>*>(
          audio_sender.value().get());
  EXPECT_NE(0, audio_sender_proxy->internal()->AttachmentId());

  auto video_sender = pc_->AddTrack(video_track, std::vector<std::string>());
  ASSERT_TRUE(video_sender.ok());
  auto* video_sender_proxy =
      static_cast<RtpSenderProxyWithInternal<RtpSenderInternal>*>(
          video_sender.value().get());
  EXPECT_NE(0, video_sender_proxy->internal()->AttachmentId());
}

// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB, AttachmentIdIsSetOnAddStream) {
  CreatePeerConnectionWithoutDtls();
  AddVideoStream(kStreamId1);
  auto senders = pc_->GetSenders();
  ASSERT_EQ(1u, senders.size());
  auto* sender_proxy =
      static_cast<RtpSenderProxyWithInternal<RtpSenderInternal>*>(
          senders[0].get());
  EXPECT_NE(0, sender_proxy->internal()->AttachmentId());
}

TEST_P(PeerConnectionInterfaceTest, CreateOfferReceiveAnswer) {
  InitiateCall();
  WaitAndVerifyOnAddStream(kStreamId1, 2);
  VerifyRemoteRtpHeaderExtensions();
}

TEST_P(PeerConnectionInterfaceTest, CreateOfferReceivePrAnswerAndAnswer) {
  CreatePeerConnectionWithoutDtls();
  AddVideoTrack(kVideoTracks[0], {kStreamId1});
  CreateOfferAsLocalDescription();
  std::string offer;
  EXPECT_TRUE(pc_->local_description()->ToString(&offer));
  CreatePrAnswerAndAnswerAsRemoteDescription(offer);
  WaitAndVerifyOnAddStream(kStreamId1, 1);
}

TEST_P(PeerConnectionInterfaceTest, ReceiveOfferCreateAnswer) {
  CreatePeerConnectionWithoutDtls();
  AddVideoTrack(kVideoTracks[0], {kStreamId1});

  CreateOfferAsRemoteDescription();
  CreateAnswerAsLocalDescription();

  WaitAndVerifyOnAddStream(kStreamId1, 1);
}

TEST_P(PeerConnectionInterfaceTest, ReceiveOfferCreatePrAnswerAndAnswer) {
  CreatePeerConnectionWithoutDtls();
  AddVideoTrack(kVideoTracks[0], {kStreamId1});

  CreateOfferAsRemoteDescription();
  CreatePrAnswerAsLocalDescription();
  CreateAnswerAsLocalDescription();

  WaitAndVerifyOnAddStream(kStreamId1, 1);
}

// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB, Renegotiate) {
  InitiateCall();
  ASSERT_EQ(1u, pc_->remote_streams()->count());
  pc_->RemoveStream(pc_->local_streams()->at(0));
  CreateOfferReceiveAnswer();
  EXPECT_EQ(0u, pc_->remote_streams()->count());
  AddVideoStream(kStreamId1);
  CreateOfferReceiveAnswer();
}

// Tests that after negotiating an audio only call, the respondent can perform a
// renegotiation that removes the audio stream.
TEST_F(PeerConnectionInterfaceTestPlanB, RenegotiateAudioOnly) {
  CreatePeerConnectionWithoutDtls();
  AddAudioStream(kStreamId1);
  CreateOfferAsRemoteDescription();
  CreateAnswerAsLocalDescription();

  ASSERT_EQ(1u, pc_->remote_streams()->count());
  pc_->RemoveStream(pc_->local_streams()->at(0));
  CreateOfferReceiveAnswer();
  EXPECT_EQ(0u, pc_->remote_streams()->count());
}

// Test that candidates are generated and that we can parse our own candidates.
TEST_P(PeerConnectionInterfaceTest, IceCandidates) {
  CreatePeerConnectionWithoutDtls();

  EXPECT_FALSE(pc_->AddIceCandidate(observer_.last_candidate()));
  // SetRemoteDescription takes ownership of offer.
  std::unique_ptr<SessionDescriptionInterface> offer;
  AddVideoTrack(kVideoTracks[0]);
  EXPECT_TRUE(DoCreateOffer(&offer, nullptr));
  EXPECT_TRUE(DoSetRemoteDescription(std::move(offer)));

  // SetLocalDescription takes ownership of answer.
  std::unique_ptr<SessionDescriptionInterface> answer;
  EXPECT_TRUE(DoCreateAnswer(&answer, nullptr));
  EXPECT_TRUE(DoSetLocalDescription(std::move(answer)));

  EXPECT_TRUE_WAIT(observer_.last_candidate() != nullptr, kTimeout);
  EXPECT_TRUE_WAIT(observer_.ice_gathering_complete_, kTimeout);

  EXPECT_TRUE(pc_->AddIceCandidate(observer_.last_candidate()));
}

// Test that CreateOffer and CreateAnswer will fail if the track labels are
// not unique.
TEST_F(PeerConnectionInterfaceTestPlanB, CreateOfferAnswerWithInvalidStream) {
  CreatePeerConnectionWithoutDtls();
  // Create a regular offer for the CreateAnswer test later.
  std::unique_ptr<SessionDescriptionInterface> offer;
  EXPECT_TRUE(DoCreateOffer(&offer, nullptr));
  EXPECT_TRUE(offer);
  offer.reset();

  // Create a local stream with audio&video tracks having same label.
  AddAudioTrack("track_label", {kStreamId1});
  AddVideoTrack("track_label", {kStreamId1});

  // Test CreateOffer
  EXPECT_FALSE(DoCreateOffer(&offer, nullptr));

  // Test CreateAnswer
  std::unique_ptr<SessionDescriptionInterface> answer;
  EXPECT_FALSE(DoCreateAnswer(&answer, nullptr));
}

// Test that we will get different SSRCs for each tracks in the offer and answer
// we created.
TEST_P(PeerConnectionInterfaceTest, SsrcInOfferAnswer) {
  CreatePeerConnectionWithoutDtls();
  // Create a local stream with audio&video tracks having different labels.
  AddAudioTrack(kAudioTracks[0], {kStreamId1});
  AddVideoTrack(kVideoTracks[0], {kStreamId1});

  // Test CreateOffer
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  int audio_ssrc = 0;
  int video_ssrc = 0;
  EXPECT_TRUE(
      GetFirstSsrc(GetFirstAudioContent(offer->description()), &audio_ssrc));
  EXPECT_TRUE(
      GetFirstSsrc(GetFirstVideoContent(offer->description()), &video_ssrc));
  EXPECT_NE(audio_ssrc, video_ssrc);

  // Test CreateAnswer
  EXPECT_TRUE(DoSetRemoteDescription(std::move(offer)));
  std::unique_ptr<SessionDescriptionInterface> answer;
  ASSERT_TRUE(DoCreateAnswer(&answer, nullptr));
  audio_ssrc = 0;
  video_ssrc = 0;
  EXPECT_TRUE(
      GetFirstSsrc(GetFirstAudioContent(answer->description()), &audio_ssrc));
  EXPECT_TRUE(
      GetFirstSsrc(GetFirstVideoContent(answer->description()), &video_ssrc));
  EXPECT_NE(audio_ssrc, video_ssrc);
}

// Test that it's possible to call AddTrack on a MediaStream after adding
// the stream to a PeerConnection.
// TODO(deadbeef): Remove this test once this behavior is no longer supported.
// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB, AddTrackAfterAddStream) {
  CreatePeerConnectionWithoutDtls();
  // Create audio stream and add to PeerConnection.
  AddAudioStream(kStreamId1);
  MediaStreamInterface* stream = pc_->local_streams()->at(0);

  // Add video track to the audio-only stream.
  rtc::scoped_refptr<VideoTrackInterface> video_track(
      pc_factory_->CreateVideoTrack(
          "video_label", pc_factory_->CreateVideoSource(
                             std::unique_ptr<cricket::VideoCapturer>(
                                 new cricket::FakeVideoCapturer()))));
  stream->AddTrack(video_track.get());

  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));

  const cricket::MediaContentDescription* video_desc =
      cricket::GetFirstVideoContentDescription(offer->description());
  EXPECT_TRUE(video_desc != nullptr);
}

// Test that it's possible to call RemoveTrack on a MediaStream after adding
// the stream to a PeerConnection.
// TODO(deadbeef): Remove this test once this behavior is no longer supported.
// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB, RemoveTrackAfterAddStream) {
  CreatePeerConnectionWithoutDtls();
  // Create audio/video stream and add to PeerConnection.
  AddAudioVideoStream(kStreamId1, "audio_label", "video_label");
  MediaStreamInterface* stream = pc_->local_streams()->at(0);

  // Remove the video track.
  stream->RemoveTrack(stream->GetVideoTracks()[0]);

  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));

  const cricket::MediaContentDescription* video_desc =
      cricket::GetFirstVideoContentDescription(offer->description());
  EXPECT_TRUE(video_desc == nullptr);
}

// Test creating a sender with a stream ID, and ensure the ID is populated
// in the offer.
// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB, CreateSenderWithStream) {
  CreatePeerConnectionWithoutDtls();
  pc_->CreateSender("video", kStreamId1);

  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));

  const cricket::MediaContentDescription* video_desc =
      cricket::GetFirstVideoContentDescription(offer->description());
  ASSERT_TRUE(video_desc != nullptr);
  ASSERT_EQ(1u, video_desc->streams().size());
  EXPECT_EQ(kStreamId1, video_desc->streams()[0].first_stream_id());
}

// Test that we can specify a certain track that we want statistics about.
TEST_P(PeerConnectionInterfaceTest, GetStatsForSpecificTrack) {
  InitiateCall();
  ASSERT_LT(0u, pc_->GetSenders().size());
  ASSERT_LT(0u, pc_->GetReceivers().size());
  rtc::scoped_refptr<MediaStreamTrackInterface> remote_audio =
      pc_->GetReceivers()[0]->track();
  EXPECT_TRUE(DoGetStats(remote_audio));

  // Remove the stream. Since we are sending to our selves the local
  // and the remote stream is the same.
  pc_->RemoveTrack(pc_->GetSenders()[0]);
  // Do a re-negotiation.
  CreateOfferReceiveAnswer();

  // Test that we still can get statistics for the old track. Even if it is not
  // sent any longer.
  EXPECT_TRUE(DoGetStats(remote_audio));
}

// Test that we can get stats on a video track.
TEST_P(PeerConnectionInterfaceTest, GetStatsForVideoTrack) {
  InitiateCall();
  auto video_receiver = GetFirstReceiverOfType(cricket::MEDIA_TYPE_VIDEO);
  ASSERT_TRUE(video_receiver);
  EXPECT_TRUE(DoGetStats(video_receiver->track()));
}

// Test that we don't get statistics for an invalid track.
TEST_P(PeerConnectionInterfaceTest, GetStatsForInvalidTrack) {
  InitiateCall();
  rtc::scoped_refptr<AudioTrackInterface> unknown_audio_track(
      pc_factory_->CreateAudioTrack("unknown track", NULL));
  EXPECT_FALSE(DoGetStats(unknown_audio_track));
}

TEST_P(PeerConnectionInterfaceTest, GetRTCStatsBeforeAndAfterCalling) {
  CreatePeerConnectionWithoutDtls();
  EXPECT_TRUE(DoGetRTCStats());
  // Clearing stats cache is needed now, but should be temporary.
  // https://bugs.chromium.org/p/webrtc/issues/detail?id=8693
  pc_->ClearStatsCache();
  AddAudioTrack(kAudioTracks[0], {kStreamId1});
  AddVideoTrack(kVideoTracks[0], {kStreamId1});
  EXPECT_TRUE(DoGetRTCStats());
  pc_->ClearStatsCache();
  CreateOfferReceiveAnswer();
  EXPECT_TRUE(DoGetRTCStats());
}

// This test setup two RTP data channels in loop back.
TEST_P(PeerConnectionInterfaceTest, TestDataChannel) {
  FakeConstraints constraints;
  constraints.SetAllowRtpDataChannels();
  CreatePeerConnection(&constraints);
  rtc::scoped_refptr<DataChannelInterface> data1 =
      pc_->CreateDataChannel("test1", NULL);
  rtc::scoped_refptr<DataChannelInterface> data2 =
      pc_->CreateDataChannel("test2", NULL);
  ASSERT_TRUE(data1 != NULL);
  std::unique_ptr<MockDataChannelObserver> observer1(
      new MockDataChannelObserver(data1));
  std::unique_ptr<MockDataChannelObserver> observer2(
      new MockDataChannelObserver(data2));

  EXPECT_EQ(DataChannelInterface::kConnecting, data1->state());
  EXPECT_EQ(DataChannelInterface::kConnecting, data2->state());
  std::string data_to_send1 = "testing testing";
  std::string data_to_send2 = "testing something else";
  EXPECT_FALSE(data1->Send(DataBuffer(data_to_send1)));

  CreateOfferReceiveAnswer();
  EXPECT_TRUE_WAIT(observer1->IsOpen(), kTimeout);
  EXPECT_TRUE_WAIT(observer2->IsOpen(), kTimeout);

  EXPECT_EQ(DataChannelInterface::kOpen, data1->state());
  EXPECT_EQ(DataChannelInterface::kOpen, data2->state());
  EXPECT_TRUE(data1->Send(DataBuffer(data_to_send1)));
  EXPECT_TRUE(data2->Send(DataBuffer(data_to_send2)));

  EXPECT_EQ_WAIT(data_to_send1, observer1->last_message(), kTimeout);
  EXPECT_EQ_WAIT(data_to_send2, observer2->last_message(), kTimeout);

  data1->Close();
  EXPECT_EQ(DataChannelInterface::kClosing, data1->state());
  CreateOfferReceiveAnswer();
  EXPECT_FALSE(observer1->IsOpen());
  EXPECT_EQ(DataChannelInterface::kClosed, data1->state());
  EXPECT_TRUE(observer2->IsOpen());

  data_to_send2 = "testing something else again";
  EXPECT_TRUE(data2->Send(DataBuffer(data_to_send2)));

  EXPECT_EQ_WAIT(data_to_send2, observer2->last_message(), kTimeout);
}

// This test verifies that sendnig binary data over RTP data channels should
// fail.
TEST_P(PeerConnectionInterfaceTest, TestSendBinaryOnRtpDataChannel) {
  FakeConstraints constraints;
  constraints.SetAllowRtpDataChannels();
  CreatePeerConnection(&constraints);
  rtc::scoped_refptr<DataChannelInterface> data1 =
      pc_->CreateDataChannel("test1", NULL);
  rtc::scoped_refptr<DataChannelInterface> data2 =
      pc_->CreateDataChannel("test2", NULL);
  ASSERT_TRUE(data1 != NULL);
  std::unique_ptr<MockDataChannelObserver> observer1(
      new MockDataChannelObserver(data1));
  std::unique_ptr<MockDataChannelObserver> observer2(
      new MockDataChannelObserver(data2));

  EXPECT_EQ(DataChannelInterface::kConnecting, data1->state());
  EXPECT_EQ(DataChannelInterface::kConnecting, data2->state());

  CreateOfferReceiveAnswer();
  EXPECT_TRUE_WAIT(observer1->IsOpen(), kTimeout);
  EXPECT_TRUE_WAIT(observer2->IsOpen(), kTimeout);

  EXPECT_EQ(DataChannelInterface::kOpen, data1->state());
  EXPECT_EQ(DataChannelInterface::kOpen, data2->state());

  rtc::CopyOnWriteBuffer buffer("test", 4);
  EXPECT_FALSE(data1->Send(DataBuffer(buffer, true)));
}

// This test setup a RTP data channels in loop back and test that a channel is
// opened even if the remote end answer with a zero SSRC.
TEST_P(PeerConnectionInterfaceTest, TestSendOnlyDataChannel) {
  FakeConstraints constraints;
  constraints.SetAllowRtpDataChannels();
  CreatePeerConnection(&constraints);
  rtc::scoped_refptr<DataChannelInterface> data1 =
      pc_->CreateDataChannel("test1", NULL);
  std::unique_ptr<MockDataChannelObserver> observer1(
      new MockDataChannelObserver(data1));

  CreateOfferReceiveAnswerWithoutSsrc();

  EXPECT_TRUE_WAIT(observer1->IsOpen(), kTimeout);

  data1->Close();
  EXPECT_EQ(DataChannelInterface::kClosing, data1->state());
  CreateOfferReceiveAnswerWithoutSsrc();
  EXPECT_EQ(DataChannelInterface::kClosed, data1->state());
  EXPECT_FALSE(observer1->IsOpen());
}

// This test that if a data channel is added in an answer a receive only channel
// channel is created.
TEST_P(PeerConnectionInterfaceTest, TestReceiveOnlyDataChannel) {
  FakeConstraints constraints;
  constraints.SetAllowRtpDataChannels();
  CreatePeerConnection(&constraints);

  std::string offer_label = "offer_channel";
  rtc::scoped_refptr<DataChannelInterface> offer_channel =
      pc_->CreateDataChannel(offer_label, NULL);

  CreateOfferAsLocalDescription();

  // Replace the data channel label in the offer and apply it as an answer.
  std::string receive_label = "answer_channel";
  std::string sdp;
  EXPECT_TRUE(pc_->local_description()->ToString(&sdp));
  rtc::replace_substrs(offer_label.c_str(), offer_label.length(),
                       receive_label.c_str(), receive_label.length(), &sdp);
  CreateAnswerAsRemoteDescription(sdp);

  // Verify that a new incoming data channel has been created and that
  // it is open but can't we written to.
  ASSERT_TRUE(observer_.last_datachannel_ != NULL);
  DataChannelInterface* received_channel = observer_.last_datachannel_;
  EXPECT_EQ(DataChannelInterface::kConnecting, received_channel->state());
  EXPECT_EQ(receive_label, received_channel->label());
  EXPECT_FALSE(received_channel->Send(DataBuffer("something")));

  // Verify that the channel we initially offered has been rejected.
  EXPECT_EQ(DataChannelInterface::kClosed, offer_channel->state());

  // Do another offer / answer exchange and verify that the data channel is
  // opened.
  CreateOfferReceiveAnswer();
  EXPECT_EQ_WAIT(DataChannelInterface::kOpen, received_channel->state(),
                 kTimeout);
}

// This test that no data channel is returned if a reliable channel is
// requested.
// TODO(perkj): Remove this test once reliable channels are implemented.
TEST_P(PeerConnectionInterfaceTest, CreateReliableRtpDataChannelShouldFail) {
  FakeConstraints constraints;
  constraints.SetAllowRtpDataChannels();
  CreatePeerConnection(&constraints);

  std::string label = "test";
  webrtc::DataChannelInit config;
  config.reliable = true;
  rtc::scoped_refptr<DataChannelInterface> channel =
      pc_->CreateDataChannel(label, &config);
  EXPECT_TRUE(channel == NULL);
}

// Verifies that duplicated label is not allowed for RTP data channel.
TEST_P(PeerConnectionInterfaceTest, RtpDuplicatedLabelNotAllowed) {
  FakeConstraints constraints;
  constraints.SetAllowRtpDataChannels();
  CreatePeerConnection(&constraints);

  std::string label = "test";
  rtc::scoped_refptr<DataChannelInterface> channel =
      pc_->CreateDataChannel(label, nullptr);
  EXPECT_NE(channel, nullptr);

  rtc::scoped_refptr<DataChannelInterface> dup_channel =
      pc_->CreateDataChannel(label, nullptr);
  EXPECT_EQ(dup_channel, nullptr);
}

// This tests that a SCTP data channel is returned using different
// DataChannelInit configurations.
TEST_P(PeerConnectionInterfaceTest, CreateSctpDataChannel) {
  FakeConstraints constraints;
  constraints.SetAllowDtlsSctpDataChannels();
  CreatePeerConnection(&constraints);

  webrtc::DataChannelInit config;

  rtc::scoped_refptr<DataChannelInterface> channel =
      pc_->CreateDataChannel("1", &config);
  EXPECT_TRUE(channel != NULL);
  EXPECT_TRUE(channel->reliable());
  EXPECT_TRUE(observer_.renegotiation_needed_);
  observer_.renegotiation_needed_ = false;

  config.ordered = false;
  channel = pc_->CreateDataChannel("2", &config);
  EXPECT_TRUE(channel != NULL);
  EXPECT_TRUE(channel->reliable());
  EXPECT_FALSE(observer_.renegotiation_needed_);

  config.ordered = true;
  config.maxRetransmits = 0;
  channel = pc_->CreateDataChannel("3", &config);
  EXPECT_TRUE(channel != NULL);
  EXPECT_FALSE(channel->reliable());
  EXPECT_FALSE(observer_.renegotiation_needed_);

  config.maxRetransmits = -1;
  config.maxRetransmitTime = 0;
  channel = pc_->CreateDataChannel("4", &config);
  EXPECT_TRUE(channel != NULL);
  EXPECT_FALSE(channel->reliable());
  EXPECT_FALSE(observer_.renegotiation_needed_);
}

// This tests that no data channel is returned if both maxRetransmits and
// maxRetransmitTime are set for SCTP data channels.
TEST_P(PeerConnectionInterfaceTest,
       CreateSctpDataChannelShouldFailForInvalidConfig) {
  FakeConstraints constraints;
  constraints.SetAllowDtlsSctpDataChannels();
  CreatePeerConnection(&constraints);

  std::string label = "test";
  webrtc::DataChannelInit config;
  config.maxRetransmits = 0;
  config.maxRetransmitTime = 0;

  rtc::scoped_refptr<DataChannelInterface> channel =
      pc_->CreateDataChannel(label, &config);
  EXPECT_TRUE(channel == NULL);
}

// The test verifies that creating a SCTP data channel with an id already in use
// or out of range should fail.
TEST_P(PeerConnectionInterfaceTest,
       CreateSctpDataChannelWithInvalidIdShouldFail) {
  FakeConstraints constraints;
  constraints.SetAllowDtlsSctpDataChannels();
  CreatePeerConnection(&constraints);

  webrtc::DataChannelInit config;
  rtc::scoped_refptr<DataChannelInterface> channel;

  config.id = 1;
  channel = pc_->CreateDataChannel("1", &config);
  EXPECT_TRUE(channel != NULL);
  EXPECT_EQ(1, channel->id());

  channel = pc_->CreateDataChannel("x", &config);
  EXPECT_TRUE(channel == NULL);

  config.id = cricket::kMaxSctpSid;
  channel = pc_->CreateDataChannel("max", &config);
  EXPECT_TRUE(channel != NULL);
  EXPECT_EQ(config.id, channel->id());

  config.id = cricket::kMaxSctpSid + 1;
  channel = pc_->CreateDataChannel("x", &config);
  EXPECT_TRUE(channel == NULL);
}

// Verifies that duplicated label is allowed for SCTP data channel.
TEST_P(PeerConnectionInterfaceTest, SctpDuplicatedLabelAllowed) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);

  std::string label = "test";
  rtc::scoped_refptr<DataChannelInterface> channel =
      pc_->CreateDataChannel(label, nullptr);
  EXPECT_NE(channel, nullptr);

  rtc::scoped_refptr<DataChannelInterface> dup_channel =
      pc_->CreateDataChannel(label, nullptr);
  EXPECT_NE(dup_channel, nullptr);
}

// This test verifies that OnRenegotiationNeeded is fired for every new RTP
// DataChannel.
TEST_P(PeerConnectionInterfaceTest, RenegotiationNeededForNewRtpDataChannel) {
  FakeConstraints constraints;
  constraints.SetAllowRtpDataChannels();
  CreatePeerConnection(&constraints);

  rtc::scoped_refptr<DataChannelInterface> dc1 =
      pc_->CreateDataChannel("test1", NULL);
  EXPECT_TRUE(observer_.renegotiation_needed_);
  observer_.renegotiation_needed_ = false;

  rtc::scoped_refptr<DataChannelInterface> dc2 =
      pc_->CreateDataChannel("test2", NULL);
  EXPECT_TRUE(observer_.renegotiation_needed_);
}

// This test that a data channel closes when a PeerConnection is deleted/closed.
TEST_P(PeerConnectionInterfaceTest, DataChannelCloseWhenPeerConnectionClose) {
  FakeConstraints constraints;
  constraints.SetAllowRtpDataChannels();
  CreatePeerConnection(&constraints);

  rtc::scoped_refptr<DataChannelInterface> data1 =
      pc_->CreateDataChannel("test1", NULL);
  rtc::scoped_refptr<DataChannelInterface> data2 =
      pc_->CreateDataChannel("test2", NULL);
  ASSERT_TRUE(data1 != NULL);
  std::unique_ptr<MockDataChannelObserver> observer1(
      new MockDataChannelObserver(data1));
  std::unique_ptr<MockDataChannelObserver> observer2(
      new MockDataChannelObserver(data2));

  CreateOfferReceiveAnswer();
  EXPECT_TRUE_WAIT(observer1->IsOpen(), kTimeout);
  EXPECT_TRUE_WAIT(observer2->IsOpen(), kTimeout);

  ReleasePeerConnection();
  EXPECT_EQ(DataChannelInterface::kClosed, data1->state());
  EXPECT_EQ(DataChannelInterface::kClosed, data2->state());
}

// This tests that RTP data channels can be rejected in an answer.
TEST_P(PeerConnectionInterfaceTest, TestRejectRtpDataChannelInAnswer) {
  FakeConstraints constraints;
  constraints.SetAllowRtpDataChannels();
  CreatePeerConnection(&constraints);

  rtc::scoped_refptr<DataChannelInterface> offer_channel(
      pc_->CreateDataChannel("offer_channel", NULL));

  CreateOfferAsLocalDescription();

  // Create an answer where the m-line for data channels are rejected.
  std::string sdp;
  EXPECT_TRUE(pc_->local_description()->ToString(&sdp));
  std::unique_ptr<SessionDescriptionInterface> answer(
      webrtc::CreateSessionDescription(SdpType::kAnswer, sdp));
  ASSERT_TRUE(answer);
  cricket::ContentInfo* data_info =
      cricket::GetFirstDataContent(answer->description());
  data_info->rejected = true;

  DoSetRemoteDescription(std::move(answer));
  EXPECT_EQ(DataChannelInterface::kClosed, offer_channel->state());
}

#ifdef HAVE_SCTP
// This tests that SCTP data channels can be rejected in an answer.
TEST_P(PeerConnectionInterfaceTest, TestRejectSctpDataChannelInAnswer)
#else
TEST_P(PeerConnectionInterfaceTest, DISABLED_TestRejectSctpDataChannelInAnswer)
#endif
{
  FakeConstraints constraints;
  CreatePeerConnection(&constraints);

  rtc::scoped_refptr<DataChannelInterface> offer_channel(
      pc_->CreateDataChannel("offer_channel", NULL));

  CreateOfferAsLocalDescription();

  // Create an answer where the m-line for data channels are rejected.
  std::string sdp;
  EXPECT_TRUE(pc_->local_description()->ToString(&sdp));
  std::unique_ptr<SessionDescriptionInterface> answer(
      webrtc::CreateSessionDescription(SdpType::kAnswer, sdp));
  ASSERT_TRUE(answer);
  cricket::ContentInfo* data_info =
      cricket::GetFirstDataContent(answer->description());
  data_info->rejected = true;

  DoSetRemoteDescription(std::move(answer));
  EXPECT_EQ(DataChannelInterface::kClosed, offer_channel->state());
}

// Test that we can create a session description from an SDP string from
// FireFox, use it as a remote session description, generate an answer and use
// the answer as a local description.
TEST_P(PeerConnectionInterfaceTest, ReceiveFireFoxOffer) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  AddAudioTrack("audio_label");
  AddVideoTrack("video_label");
  std::unique_ptr<SessionDescriptionInterface> desc(
      webrtc::CreateSessionDescription(SdpType::kOffer,
                                       webrtc::kFireFoxSdpOffer, nullptr));
  EXPECT_TRUE(DoSetSessionDescription(std::move(desc), false));
  CreateAnswerAsLocalDescription();
  ASSERT_TRUE(pc_->local_description() != NULL);
  ASSERT_TRUE(pc_->remote_description() != NULL);

  const cricket::ContentInfo* content =
      cricket::GetFirstAudioContent(pc_->local_description()->description());
  ASSERT_TRUE(content != NULL);
  EXPECT_FALSE(content->rejected);

  content =
      cricket::GetFirstVideoContent(pc_->local_description()->description());
  ASSERT_TRUE(content != NULL);
  EXPECT_FALSE(content->rejected);
#ifdef HAVE_SCTP
  content =
      cricket::GetFirstDataContent(pc_->local_description()->description());
  ASSERT_TRUE(content != NULL);
  EXPECT_FALSE(content->rejected);
#endif
}

// Test that fallback from DTLS to SDES is not supported.
// The fallback was previously supported but was removed to simplify the code
// and because it's non-standard.
TEST_P(PeerConnectionInterfaceTest, DtlsSdesFallbackNotSupported) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  // Wait for fake certificate to be generated. Previously, this is what caused
  // the "a=crypto" lines to be rejected.
  AddAudioTrack("audio_label");
  AddVideoTrack("video_label");
  ASSERT_NE(nullptr, fake_certificate_generator_);
  EXPECT_EQ_WAIT(1, fake_certificate_generator_->generated_certificates(),
                 kTimeout);
  std::unique_ptr<SessionDescriptionInterface> desc(
      webrtc::CreateSessionDescription(SdpType::kOffer, kDtlsSdesFallbackSdp,
                                       nullptr));
  EXPECT_FALSE(DoSetSessionDescription(std::move(desc), /*local=*/false));
}

// Test that we can create an audio only offer and receive an answer with a
// limited set of audio codecs and receive an updated offer with more audio
// codecs, where the added codecs are not supported.
TEST_P(PeerConnectionInterfaceTest, ReceiveUpdatedAudioOfferWithBadCodecs) {
  CreatePeerConnectionWithoutDtls();
  AddAudioTrack("audio_label");
  CreateOfferAsLocalDescription();

  const char* answer_sdp =
      (sdp_semantics_ == SdpSemantics::kPlanB ? webrtc::kAudioSdpPlanB
                                              : webrtc::kAudioSdpUnifiedPlan);
  std::unique_ptr<SessionDescriptionInterface> answer(
      webrtc::CreateSessionDescription(SdpType::kAnswer, answer_sdp, nullptr));
  EXPECT_TRUE(DoSetSessionDescription(std::move(answer), false));

  const char* reoffer_sdp =
      (sdp_semantics_ == SdpSemantics::kPlanB
           ? webrtc::kAudioSdpWithUnsupportedCodecsPlanB
           : webrtc::kAudioSdpWithUnsupportedCodecsUnifiedPlan);
  std::unique_ptr<SessionDescriptionInterface> updated_offer(
      webrtc::CreateSessionDescription(SdpType::kOffer, reoffer_sdp, nullptr));
  EXPECT_TRUE(DoSetSessionDescription(std::move(updated_offer), false));
  CreateAnswerAsLocalDescription();
}

// Test that if we're receiving (but not sending) a track, subsequent offers
// will have m-lines with a=recvonly.
TEST_P(PeerConnectionInterfaceTest, CreateSubsequentRecvOnlyOffer) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(GetSdpStringWithStream1());
  CreateAnswerAsLocalDescription();

  // At this point we should be receiving stream 1, but not sending anything.
  // A new offer should be recvonly.
  std::unique_ptr<SessionDescriptionInterface> offer;
  DoCreateOffer(&offer, nullptr);

  const cricket::ContentInfo* video_content =
      cricket::GetFirstVideoContent(offer->description());
  ASSERT_EQ(RtpTransceiverDirection::kRecvOnly,
            video_content->media_description()->direction());

  const cricket::ContentInfo* audio_content =
      cricket::GetFirstAudioContent(offer->description());
  ASSERT_EQ(RtpTransceiverDirection::kRecvOnly,
            audio_content->media_description()->direction());
}

// Test that if we're receiving (but not sending) a track, and the
// offerToReceiveVideo/offerToReceiveAudio constraints are explicitly set to
// false, the generated m-lines will be a=inactive.
TEST_P(PeerConnectionInterfaceTest, CreateSubsequentInactiveOffer) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(GetSdpStringWithStream1());
  CreateAnswerAsLocalDescription();

  // At this point we should be receiving stream 1, but not sending anything.
  // A new offer would be recvonly, but we'll set the "no receive" constraints
  // to make it inactive.
  std::unique_ptr<SessionDescriptionInterface> offer;
  FakeConstraints offer_constraints;
  offer_constraints.AddMandatory(
      webrtc::MediaConstraintsInterface::kOfferToReceiveVideo, false);
  offer_constraints.AddMandatory(
      webrtc::MediaConstraintsInterface::kOfferToReceiveAudio, false);
  DoCreateOffer(&offer, &offer_constraints);

  const cricket::ContentInfo* video_content =
      cricket::GetFirstVideoContent(offer->description());
  ASSERT_EQ(RtpTransceiverDirection::kInactive,
            video_content->media_description()->direction());

  const cricket::ContentInfo* audio_content =
      cricket::GetFirstAudioContent(offer->description());
  ASSERT_EQ(RtpTransceiverDirection::kInactive,
            audio_content->media_description()->direction());
}

// Test that we can use SetConfiguration to change the ICE servers of the
// PortAllocator.
TEST_P(PeerConnectionInterfaceTest, SetConfigurationChangesIceServers) {
  CreatePeerConnection();

  PeerConnectionInterface::RTCConfiguration config = pc_->GetConfiguration();
  PeerConnectionInterface::IceServer server;
  server.uri = "stun:test_hostname";
  config.servers.push_back(server);
  EXPECT_TRUE(pc_->SetConfiguration(config));

  EXPECT_EQ(1u, port_allocator_->stun_servers().size());
  EXPECT_EQ("test_hostname",
            port_allocator_->stun_servers().begin()->hostname());
}

TEST_P(PeerConnectionInterfaceTest, SetConfigurationChangesCandidateFilter) {
  CreatePeerConnection();
  PeerConnectionInterface::RTCConfiguration config = pc_->GetConfiguration();
  config.type = PeerConnectionInterface::kRelay;
  EXPECT_TRUE(pc_->SetConfiguration(config));
  EXPECT_EQ(cricket::CF_RELAY, port_allocator_->candidate_filter());
}

TEST_P(PeerConnectionInterfaceTest, SetConfigurationChangesPruneTurnPortsFlag) {
  PeerConnectionInterface::RTCConfiguration config;
  config.prune_turn_ports = false;
  CreatePeerConnection(config, nullptr);
  config = pc_->GetConfiguration();
  EXPECT_FALSE(port_allocator_->prune_turn_ports());

  config.prune_turn_ports = true;
  EXPECT_TRUE(pc_->SetConfiguration(config));
  EXPECT_TRUE(port_allocator_->prune_turn_ports());
}

// Test that the ice check interval can be changed. This does not verify that
// the setting makes it all the way to P2PTransportChannel, as that would
// require a very complex set of mocks.
TEST_P(PeerConnectionInterfaceTest, SetConfigurationChangesIceCheckInterval) {
  PeerConnectionInterface::RTCConfiguration config;
  config.ice_check_min_interval = absl::nullopt;
  CreatePeerConnection(config, nullptr);
  config = pc_->GetConfiguration();
  config.ice_check_min_interval = 100;
  EXPECT_TRUE(pc_->SetConfiguration(config));
  PeerConnectionInterface::RTCConfiguration new_config =
      pc_->GetConfiguration();
  EXPECT_EQ(new_config.ice_check_min_interval, 100);
}

// Test that when SetConfiguration changes both the pool size and other
// attributes, the pooled session is created with the updated attributes.
TEST_P(PeerConnectionInterfaceTest,
       SetConfigurationCreatesPooledSessionCorrectly) {
  CreatePeerConnection();
  PeerConnectionInterface::RTCConfiguration config = pc_->GetConfiguration();
  config.ice_candidate_pool_size = 1;
  PeerConnectionInterface::IceServer server;
  server.uri = kStunAddressOnly;
  config.servers.push_back(server);
  config.type = PeerConnectionInterface::kRelay;
  EXPECT_TRUE(pc_->SetConfiguration(config));

  const cricket::FakePortAllocatorSession* session =
      static_cast<const cricket::FakePortAllocatorSession*>(
          port_allocator_->GetPooledSession());
  ASSERT_NE(nullptr, session);
  EXPECT_EQ(1UL, session->stun_servers().size());
}

// Test that after SetLocalDescription, changing the pool size is not allowed,
// and an invalid modification error is returned.
TEST_P(PeerConnectionInterfaceTest,
       CantChangePoolSizeAfterSetLocalDescription) {
  CreatePeerConnection();
  // Start by setting a size of 1.
  PeerConnectionInterface::RTCConfiguration config = pc_->GetConfiguration();
  config.ice_candidate_pool_size = 1;
  EXPECT_TRUE(pc_->SetConfiguration(config));

  // Set remote offer; can still change pool size at this point.
  CreateOfferAsRemoteDescription();
  config.ice_candidate_pool_size = 2;
  EXPECT_TRUE(pc_->SetConfiguration(config));

  // Set local answer; now it's too late.
  CreateAnswerAsLocalDescription();
  config.ice_candidate_pool_size = 3;
  RTCError error;
  EXPECT_FALSE(pc_->SetConfiguration(config, &error));
  EXPECT_EQ(RTCErrorType::INVALID_MODIFICATION, error.type());
}

// Test that after setting an answer, extra pooled sessions are discarded. The
// ICE candidate pool is only intended to be used for the first offer/answer.
TEST_P(PeerConnectionInterfaceTest,
       ExtraPooledSessionsDiscardedAfterApplyingAnswer) {
  CreatePeerConnection();

  // Set a larger-than-necessary size.
  PeerConnectionInterface::RTCConfiguration config = pc_->GetConfiguration();
  config.ice_candidate_pool_size = 4;
  EXPECT_TRUE(pc_->SetConfiguration(config));

  // Do offer/answer.
  CreateOfferAsRemoteDescription();
  CreateAnswerAsLocalDescription();

  // Expect no pooled sessions to be left.
  const cricket::PortAllocatorSession* session =
      port_allocator_->GetPooledSession();
  EXPECT_EQ(nullptr, session);
}

// After Close is called, pooled candidates should be discarded so as to not
// waste network resources.
TEST_P(PeerConnectionInterfaceTest, PooledSessionsDiscardedAfterClose) {
  CreatePeerConnection();

  PeerConnectionInterface::RTCConfiguration config = pc_->GetConfiguration();
  config.ice_candidate_pool_size = 3;
  EXPECT_TRUE(pc_->SetConfiguration(config));
  pc_->Close();

  // Expect no pooled sessions to be left.
  const cricket::PortAllocatorSession* session =
      port_allocator_->GetPooledSession();
  EXPECT_EQ(nullptr, session);
}

// Test that SetConfiguration returns an invalid modification error if
// modifying a field in the configuration that isn't allowed to be modified.
TEST_P(PeerConnectionInterfaceTest,
       SetConfigurationReturnsInvalidModificationError) {
  PeerConnectionInterface::RTCConfiguration config;
  config.bundle_policy = PeerConnectionInterface::kBundlePolicyBalanced;
  config.rtcp_mux_policy = PeerConnectionInterface::kRtcpMuxPolicyNegotiate;
  config.continual_gathering_policy = PeerConnectionInterface::GATHER_ONCE;
  CreatePeerConnection(config, nullptr);

  PeerConnectionInterface::RTCConfiguration modified_config =
      pc_->GetConfiguration();
  modified_config.bundle_policy =
      PeerConnectionInterface::kBundlePolicyMaxBundle;
  RTCError error;
  EXPECT_FALSE(pc_->SetConfiguration(modified_config, &error));
  EXPECT_EQ(RTCErrorType::INVALID_MODIFICATION, error.type());

  modified_config = pc_->GetConfiguration();
  modified_config.rtcp_mux_policy =
      PeerConnectionInterface::kRtcpMuxPolicyRequire;
  error.set_type(RTCErrorType::NONE);
  EXPECT_FALSE(pc_->SetConfiguration(modified_config, &error));
  EXPECT_EQ(RTCErrorType::INVALID_MODIFICATION, error.type());

  modified_config = pc_->GetConfiguration();
  modified_config.continual_gathering_policy =
      PeerConnectionInterface::GATHER_CONTINUALLY;
  error.set_type(RTCErrorType::NONE);
  EXPECT_FALSE(pc_->SetConfiguration(modified_config, &error));
  EXPECT_EQ(RTCErrorType::INVALID_MODIFICATION, error.type());
}

// Test that SetConfiguration returns a range error if the candidate pool size
// is negative or larger than allowed by the spec.
TEST_P(PeerConnectionInterfaceTest,
       SetConfigurationReturnsRangeErrorForBadCandidatePoolSize) {
  PeerConnectionInterface::RTCConfiguration config;
  CreatePeerConnection(config, nullptr);
  config = pc_->GetConfiguration();

  config.ice_candidate_pool_size = -1;
  RTCError error;
  EXPECT_FALSE(pc_->SetConfiguration(config, &error));
  EXPECT_EQ(RTCErrorType::INVALID_RANGE, error.type());

  config.ice_candidate_pool_size = INT_MAX;
  error.set_type(RTCErrorType::NONE);
  EXPECT_FALSE(pc_->SetConfiguration(config, &error));
  EXPECT_EQ(RTCErrorType::INVALID_RANGE, error.type());
}

// Test that SetConfiguration returns a syntax error if parsing an ICE server
// URL failed.
TEST_P(PeerConnectionInterfaceTest,
       SetConfigurationReturnsSyntaxErrorFromBadIceUrls) {
  PeerConnectionInterface::RTCConfiguration config;
  CreatePeerConnection(config, nullptr);
  config = pc_->GetConfiguration();

  PeerConnectionInterface::IceServer bad_server;
  bad_server.uri = "stunn:www.example.com";
  config.servers.push_back(bad_server);
  RTCError error;
  EXPECT_FALSE(pc_->SetConfiguration(config, &error));
  EXPECT_EQ(RTCErrorType::SYNTAX_ERROR, error.type());
}

// Test that SetConfiguration returns an invalid parameter error if a TURN
// IceServer is missing a username or password.
TEST_P(PeerConnectionInterfaceTest,
       SetConfigurationReturnsInvalidParameterIfCredentialsMissing) {
  PeerConnectionInterface::RTCConfiguration config;
  CreatePeerConnection(config, nullptr);
  config = pc_->GetConfiguration();

  PeerConnectionInterface::IceServer bad_server;
  bad_server.uri = "turn:www.example.com";
  // Missing password.
  bad_server.username = "foo";
  config.servers.push_back(bad_server);
  RTCError error;
  EXPECT_FALSE(pc_->SetConfiguration(config, &error));
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, error.type());
}

// Test that PeerConnection::Close changes the states to closed and all remote
// tracks change state to ended.
TEST_P(PeerConnectionInterfaceTest, CloseAndTestStreamsAndStates) {
  // Initialize a PeerConnection and negotiate local and remote session
  // description.
  InitiateCall();

  // With Plan B, verify the stream count. The analog with Unified Plan is the
  // RtpTransceiver count.
  if (sdp_semantics_ == SdpSemantics::kPlanB) {
    ASSERT_EQ(1u, pc_->local_streams()->count());
    ASSERT_EQ(1u, pc_->remote_streams()->count());
  } else {
    ASSERT_EQ(2u, pc_->GetTransceivers().size());
  }

  pc_->Close();

  EXPECT_EQ(PeerConnectionInterface::kClosed, pc_->signaling_state());
  EXPECT_EQ(PeerConnectionInterface::kIceConnectionClosed,
            pc_->ice_connection_state());
  EXPECT_EQ(PeerConnectionInterface::kIceGatheringComplete,
            pc_->ice_gathering_state());

  if (sdp_semantics_ == SdpSemantics::kPlanB) {
    EXPECT_EQ(1u, pc_->local_streams()->count());
    EXPECT_EQ(1u, pc_->remote_streams()->count());
  } else {
    // Verify that the RtpTransceivers are still present but all stopped.
    EXPECT_EQ(2u, pc_->GetTransceivers().size());
    for (auto transceiver : pc_->GetTransceivers()) {
      EXPECT_TRUE(transceiver->stopped());
    }
  }

  auto audio_receiver = GetFirstReceiverOfType(cricket::MEDIA_TYPE_AUDIO);
  ASSERT_TRUE(audio_receiver);
  auto video_receiver = GetFirstReceiverOfType(cricket::MEDIA_TYPE_VIDEO);
  ASSERT_TRUE(video_receiver);

  // Track state may be updated asynchronously.
  EXPECT_EQ_WAIT(MediaStreamTrackInterface::kEnded,
                 audio_receiver->track()->state(), kTimeout);
  EXPECT_EQ_WAIT(MediaStreamTrackInterface::kEnded,
                 video_receiver->track()->state(), kTimeout);
}

// Test that PeerConnection methods fails gracefully after
// PeerConnection::Close has been called.
// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB, CloseAndTestMethods) {
  CreatePeerConnectionWithoutDtls();
  AddAudioVideoStream(kStreamId1, "audio_label", "video_label");
  CreateOfferAsRemoteDescription();
  CreateAnswerAsLocalDescription();

  ASSERT_EQ(1u, pc_->local_streams()->count());
  rtc::scoped_refptr<MediaStreamInterface> local_stream =
      pc_->local_streams()->at(0);

  pc_->Close();

  pc_->RemoveStream(local_stream);
  EXPECT_FALSE(pc_->AddStream(local_stream));

  EXPECT_TRUE(pc_->CreateDataChannel("test", NULL) == NULL);

  EXPECT_TRUE(pc_->local_description() != NULL);
  EXPECT_TRUE(pc_->remote_description() != NULL);

  std::unique_ptr<SessionDescriptionInterface> offer;
  EXPECT_FALSE(DoCreateOffer(&offer, nullptr));
  std::unique_ptr<SessionDescriptionInterface> answer;
  EXPECT_FALSE(DoCreateAnswer(&answer, nullptr));

  std::string sdp;
  ASSERT_TRUE(pc_->remote_description()->ToString(&sdp));
  std::unique_ptr<SessionDescriptionInterface> remote_offer(
      webrtc::CreateSessionDescription(SdpType::kOffer, sdp));
  EXPECT_FALSE(DoSetRemoteDescription(std::move(remote_offer)));

  ASSERT_TRUE(pc_->local_description()->ToString(&sdp));
  std::unique_ptr<SessionDescriptionInterface> local_offer(
      webrtc::CreateSessionDescription(SdpType::kOffer, sdp));
  EXPECT_FALSE(DoSetLocalDescription(std::move(local_offer)));
}

// Test that GetStats can still be called after PeerConnection::Close.
TEST_P(PeerConnectionInterfaceTest, CloseAndGetStats) {
  InitiateCall();
  pc_->Close();
  DoGetStats(NULL);
}

// NOTE: The series of tests below come from what used to be
// mediastreamsignaling_unittest.cc, and are mostly aimed at testing that
// setting a remote or local description has the expected effects.

// This test verifies that the remote MediaStreams corresponding to a received
// SDP string is created. In this test the two separate MediaStreams are
// signaled.
TEST_P(PeerConnectionInterfaceTest, UpdateRemoteStreams) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(GetSdpStringWithStream1());

  rtc::scoped_refptr<StreamCollection> reference(CreateStreamCollection(1, 1));
  EXPECT_TRUE(
      CompareStreamCollections(observer_.remote_streams(), reference.get()));
  MediaStreamInterface* remote_stream = observer_.remote_streams()->at(0);
  EXPECT_TRUE(remote_stream->GetVideoTracks()[0]->GetSource() != nullptr);

  // Create a session description based on another SDP with another
  // MediaStream.
  CreateAndSetRemoteOffer(GetSdpStringWithStream1And2());

  rtc::scoped_refptr<StreamCollection> reference2(CreateStreamCollection(2, 1));
  EXPECT_TRUE(
      CompareStreamCollections(observer_.remote_streams(), reference2.get()));
}

// This test verifies that when remote tracks are added/removed from SDP, the
// created remote streams are updated appropriately.
// Don't run under Unified Plan since this test uses Plan B SDP to test Plan B
// specific behavior.
TEST_F(PeerConnectionInterfaceTestPlanB,
       AddRemoveTrackFromExistingRemoteMediaStream) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  std::unique_ptr<SessionDescriptionInterface> desc_ms1 =
      CreateSessionDescriptionAndReference(1, 1);
  EXPECT_TRUE(DoSetRemoteDescription(std::move(desc_ms1)));
  EXPECT_TRUE(CompareStreamCollections(observer_.remote_streams(),
                                       reference_collection_));

  // Add extra audio and video tracks to the same MediaStream.
  std::unique_ptr<SessionDescriptionInterface> desc_ms1_two_tracks =
      CreateSessionDescriptionAndReference(2, 2);
  EXPECT_TRUE(DoSetRemoteDescription(std::move(desc_ms1_two_tracks)));
  EXPECT_TRUE(CompareStreamCollections(observer_.remote_streams(),
                                       reference_collection_));
  rtc::scoped_refptr<AudioTrackInterface> audio_track2 =
      observer_.remote_streams()->at(0)->GetAudioTracks()[1];
  EXPECT_EQ(webrtc::MediaStreamTrackInterface::kLive, audio_track2->state());
  rtc::scoped_refptr<VideoTrackInterface> video_track2 =
      observer_.remote_streams()->at(0)->GetVideoTracks()[1];
  EXPECT_EQ(webrtc::MediaStreamTrackInterface::kLive, video_track2->state());

  // Remove the extra audio and video tracks.
  std::unique_ptr<SessionDescriptionInterface> desc_ms2 =
      CreateSessionDescriptionAndReference(1, 1);
  MockTrackObserver audio_track_observer(audio_track2);
  MockTrackObserver video_track_observer(video_track2);

  EXPECT_CALL(audio_track_observer, OnChanged()).Times(Exactly(1));
  EXPECT_CALL(video_track_observer, OnChanged()).Times(Exactly(1));
  EXPECT_TRUE(DoSetRemoteDescription(std::move(desc_ms2)));
  EXPECT_TRUE(CompareStreamCollections(observer_.remote_streams(),
                                       reference_collection_));
  // Track state may be updated asynchronously.
  EXPECT_EQ_WAIT(webrtc::MediaStreamTrackInterface::kEnded,
                 audio_track2->state(), kTimeout);
  EXPECT_EQ_WAIT(webrtc::MediaStreamTrackInterface::kEnded,
                 video_track2->state(), kTimeout);
}

// This tests that remote tracks are ended if a local session description is set
// that rejects the media content type.
TEST_P(PeerConnectionInterfaceTest, RejectMediaContent) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  // First create and set a remote offer, then reject its video content in our
  // answer.
  CreateAndSetRemoteOffer(kSdpStringWithStream1PlanB);
  auto audio_receiver = GetFirstReceiverOfType(cricket::MEDIA_TYPE_AUDIO);
  ASSERT_TRUE(audio_receiver);
  auto video_receiver = GetFirstReceiverOfType(cricket::MEDIA_TYPE_VIDEO);
  ASSERT_TRUE(video_receiver);

  rtc::scoped_refptr<MediaStreamTrackInterface> remote_audio =
      audio_receiver->track();
  EXPECT_EQ(webrtc::MediaStreamTrackInterface::kLive, remote_audio->state());
  rtc::scoped_refptr<MediaStreamTrackInterface> remote_video =
      video_receiver->track();
  EXPECT_EQ(MediaStreamTrackInterface::kLive, remote_video->state());

  std::unique_ptr<SessionDescriptionInterface> local_answer;
  EXPECT_TRUE(DoCreateAnswer(&local_answer, nullptr));
  cricket::ContentInfo* video_info =
      local_answer->description()->GetContentByName("video");
  video_info->rejected = true;
  EXPECT_TRUE(DoSetLocalDescription(std::move(local_answer)));
  EXPECT_EQ(MediaStreamTrackInterface::kEnded, remote_video->state());
  EXPECT_EQ(MediaStreamTrackInterface::kLive, remote_audio->state());

  // Now create an offer where we reject both video and audio.
  std::unique_ptr<SessionDescriptionInterface> local_offer;
  EXPECT_TRUE(DoCreateOffer(&local_offer, nullptr));
  video_info = local_offer->description()->GetContentByName("video");
  ASSERT_TRUE(video_info != nullptr);
  video_info->rejected = true;
  cricket::ContentInfo* audio_info =
      local_offer->description()->GetContentByName("audio");
  ASSERT_TRUE(audio_info != nullptr);
  audio_info->rejected = true;
  EXPECT_TRUE(DoSetLocalDescription(std::move(local_offer)));
  // Track state may be updated asynchronously.
  EXPECT_EQ_WAIT(MediaStreamTrackInterface::kEnded, remote_audio->state(),
                 kTimeout);
  EXPECT_EQ_WAIT(MediaStreamTrackInterface::kEnded, remote_video->state(),
                 kTimeout);
}

// This tests that we won't crash if the remote track has been removed outside
// of PeerConnection and then PeerConnection tries to reject the track.
// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB, RemoveTrackThenRejectMediaContent) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(GetSdpStringWithStream1());
  MediaStreamInterface* remote_stream = observer_.remote_streams()->at(0);
  remote_stream->RemoveTrack(remote_stream->GetVideoTracks()[0]);
  remote_stream->RemoveTrack(remote_stream->GetAudioTracks()[0]);

  std::unique_ptr<SessionDescriptionInterface> local_answer(
      webrtc::CreateSessionDescription(SdpType::kAnswer,
                                       GetSdpStringWithStream1(), nullptr));
  cricket::ContentInfo* video_info =
      local_answer->description()->GetContentByName("video");
  video_info->rejected = true;
  cricket::ContentInfo* audio_info =
      local_answer->description()->GetContentByName("audio");
  audio_info->rejected = true;
  EXPECT_TRUE(DoSetLocalDescription(std::move(local_answer)));

  // No crash is a pass.
}

// This tests that if a recvonly remote description is set, no remote streams
// will be created, even if the description contains SSRCs/MSIDs.
// See: https://code.google.com/p/webrtc/issues/detail?id=5054
TEST_P(PeerConnectionInterfaceTest, RecvonlyDescriptionDoesntCreateStream) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);

  std::string recvonly_offer = GetSdpStringWithStream1();
  rtc::replace_substrs(kSendrecv, strlen(kSendrecv), kRecvonly,
                       strlen(kRecvonly), &recvonly_offer);
  CreateAndSetRemoteOffer(recvonly_offer);

  EXPECT_EQ(0u, observer_.remote_streams()->count());
}

// This tests that a default MediaStream is created if a remote session
// description doesn't contain any streams and no MSID support.
// It also tests that the default stream is updated if a video m-line is added
// in a subsequent session description.
// Don't run under Unified Plan since this behavior is Plan B specific.
TEST_F(PeerConnectionInterfaceTestPlanB, SdpWithoutMsidCreatesDefaultStream) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(kSdpStringWithoutStreamsAudioOnly);

  ASSERT_EQ(1u, observer_.remote_streams()->count());
  MediaStreamInterface* remote_stream = observer_.remote_streams()->at(0);

  EXPECT_EQ(1u, remote_stream->GetAudioTracks().size());
  EXPECT_EQ(0u, remote_stream->GetVideoTracks().size());
  EXPECT_EQ("default", remote_stream->id());

  CreateAndSetRemoteOffer(kSdpStringWithoutStreams);
  ASSERT_EQ(1u, observer_.remote_streams()->count());
  ASSERT_EQ(1u, remote_stream->GetAudioTracks().size());
  EXPECT_EQ("defaulta0", remote_stream->GetAudioTracks()[0]->id());
  EXPECT_EQ(MediaStreamTrackInterface::kLive,
            remote_stream->GetAudioTracks()[0]->state());
  ASSERT_EQ(1u, remote_stream->GetVideoTracks().size());
  EXPECT_EQ("defaultv0", remote_stream->GetVideoTracks()[0]->id());
  EXPECT_EQ(MediaStreamTrackInterface::kLive,
            remote_stream->GetVideoTracks()[0]->state());
}

// This tests that a default MediaStream is created if a remote session
// description doesn't contain any streams and media direction is send only.
// Don't run under Unified Plan since this behavior is Plan B specific.
TEST_F(PeerConnectionInterfaceTestPlanB,
       SendOnlySdpWithoutMsidCreatesDefaultStream) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(kSdpStringSendOnlyWithoutStreams);

  ASSERT_EQ(1u, observer_.remote_streams()->count());
  MediaStreamInterface* remote_stream = observer_.remote_streams()->at(0);

  EXPECT_EQ(1u, remote_stream->GetAudioTracks().size());
  EXPECT_EQ(1u, remote_stream->GetVideoTracks().size());
  EXPECT_EQ("default", remote_stream->id());
}

// This tests that it won't crash when PeerConnection tries to remove
// a remote track that as already been removed from the MediaStream.
// Don't run under Unified Plan since this behavior is Plan B specific.
TEST_F(PeerConnectionInterfaceTestPlanB, RemoveAlreadyGoneRemoteStream) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(GetSdpStringWithStream1());
  MediaStreamInterface* remote_stream = observer_.remote_streams()->at(0);
  remote_stream->RemoveTrack(remote_stream->GetAudioTracks()[0]);
  remote_stream->RemoveTrack(remote_stream->GetVideoTracks()[0]);

  CreateAndSetRemoteOffer(kSdpStringWithoutStreams);

  // No crash is a pass.
}

// This tests that a default MediaStream is created if the remote session
// description doesn't contain any streams and don't contain an indication if
// MSID is supported.
// Don't run under Unified Plan since this behavior is Plan B specific.
TEST_F(PeerConnectionInterfaceTestPlanB,
       SdpWithoutMsidAndStreamsCreatesDefaultStream) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(kSdpStringWithoutStreams);

  ASSERT_EQ(1u, observer_.remote_streams()->count());
  MediaStreamInterface* remote_stream = observer_.remote_streams()->at(0);
  EXPECT_EQ(1u, remote_stream->GetAudioTracks().size());
  EXPECT_EQ(1u, remote_stream->GetVideoTracks().size());
}

// This tests that a default MediaStream is not created if the remote session
// description doesn't contain any streams but does support MSID.
// Don't run under Unified Plan since this behavior is Plan B specific.
TEST_F(PeerConnectionInterfaceTestPlanB, SdpWithMsidDontCreatesDefaultStream) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(kSdpStringWithMsidWithoutStreams);
  EXPECT_EQ(0u, observer_.remote_streams()->count());
}

// This tests that when setting a new description, the old default tracks are
// not destroyed and recreated.
// See: https://bugs.chromium.org/p/webrtc/issues/detail?id=5250
// Don't run under Unified Plan since this behavior is Plan B specific.
TEST_F(PeerConnectionInterfaceTestPlanB,
       DefaultTracksNotDestroyedAndRecreated) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(kSdpStringWithoutStreamsAudioOnly);

  ASSERT_EQ(1u, observer_.remote_streams()->count());
  MediaStreamInterface* remote_stream = observer_.remote_streams()->at(0);
  ASSERT_EQ(1u, remote_stream->GetAudioTracks().size());

  // Set the track to "disabled", then set a new description and ensure the
  // track is still disabled, which ensures it hasn't been recreated.
  remote_stream->GetAudioTracks()[0]->set_enabled(false);
  CreateAndSetRemoteOffer(kSdpStringWithoutStreamsAudioOnly);
  ASSERT_EQ(1u, remote_stream->GetAudioTracks().size());
  EXPECT_FALSE(remote_stream->GetAudioTracks()[0]->enabled());
}

// This tests that a default MediaStream is not created if a remote session
// description is updated to not have any MediaStreams.
// Don't run under Unified Plan since this behavior is Plan B specific.
TEST_F(PeerConnectionInterfaceTestPlanB, VerifyDefaultStreamIsNotCreated) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(GetSdpStringWithStream1());
  rtc::scoped_refptr<StreamCollection> reference(CreateStreamCollection(1, 1));
  EXPECT_TRUE(
      CompareStreamCollections(observer_.remote_streams(), reference.get()));

  CreateAndSetRemoteOffer(kSdpStringWithoutStreams);
  EXPECT_EQ(0u, observer_.remote_streams()->count());
}

// This tests that a default MediaStream is created if a remote SDP comes from
// an endpoint that doesn't signal SSRCs, but signals media stream IDs.
TEST_F(PeerConnectionInterfaceTestPlanB,
       SdpWithMsidWithoutSsrcCreatesDefaultStream) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  std::string sdp_string = kSdpStringWithoutStreamsAudioOnly;
  // Add a=msid lines to simulate a Unified Plan endpoint that only
  // signals stream IDs with a=msid lines.
  sdp_string.append("a=msid:audio_stream_id audio_track_id\n");

  CreateAndSetRemoteOffer(sdp_string);

  ASSERT_EQ(1u, observer_.remote_streams()->count());
  MediaStreamInterface* remote_stream = observer_.remote_streams()->at(0);
  EXPECT_EQ("default", remote_stream->id());
  ASSERT_EQ(1u, remote_stream->GetAudioTracks().size());
}

// This tests that when a Plan B endpoint receives an SDP that signals no media
// stream IDs indicated by the special character "-" in the a=msid line, that
// a default stream ID will be used for the MediaStream ID. This can occur
// when a Unified Plan endpoint signals no media stream IDs, but signals both
// a=ssrc msid and a=msid lines for interop signaling with Plan B.
TEST_F(PeerConnectionInterfaceTestPlanB,
       SdpWithEmptyMsidAndSsrcCreatesDefaultStreamId) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  // Add a a=msid line to the SDP. This is prioritized when parsing the SDP, so
  // the sender's stream ID will be interpreted as no stream IDs.
  std::string sdp_string = kSdpStringWithStream1AudioTrackOnly;
  sdp_string.append("a=msid:- audiotrack0\n");

  CreateAndSetRemoteOffer(sdp_string);

  ASSERT_EQ(1u, observer_.remote_streams()->count());
  // Because SSRCs are signaled the track ID will be what was signaled in the
  // a=msid line.
  EXPECT_EQ("audiotrack0", observer_.last_added_track_label_);
  MediaStreamInterface* remote_stream = observer_.remote_streams()->at(0);
  EXPECT_EQ("default", remote_stream->id());
  ASSERT_EQ(1u, remote_stream->GetAudioTracks().size());

  // Previously a bug ocurred when setting the remote description a second time.
  // This is because we checked equality of the remote StreamParams stream ID
  // (empty), and the previously set stream ID for the remote sender
  // ("default"). This cause a track to be removed, then added, when really
  // nothing should occur because it is the same track.
  CreateAndSetRemoteOffer(sdp_string);
  EXPECT_EQ(0u, observer_.remove_track_events_.size());
  EXPECT_EQ(1u, observer_.add_track_events_.size());
  EXPECT_EQ("audiotrack0", observer_.last_added_track_label_);
  remote_stream = observer_.remote_streams()->at(0);
  EXPECT_EQ("default", remote_stream->id());
  ASSERT_EQ(1u, remote_stream->GetAudioTracks().size());
}

// This tests that an RtpSender is created when the local description is set
// after adding a local stream.
// TODO(deadbeef): This test and the one below it need to be updated when
// an RtpSender's lifetime isn't determined by when a local description is set.
// Don't run under Unified Plan since this behavior is Plan B specific.
TEST_F(PeerConnectionInterfaceTestPlanB, LocalDescriptionChanged) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);

  // Create an offer with 1 stream with 2 tracks of each type.
  rtc::scoped_refptr<StreamCollection> stream_collection =
      CreateStreamCollection(1, 2);
  pc_->AddStream(stream_collection->at(0));
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  EXPECT_TRUE(DoSetLocalDescription(std::move(offer)));

  auto senders = pc_->GetSenders();
  EXPECT_EQ(4u, senders.size());
  EXPECT_TRUE(ContainsSender(senders, kAudioTracks[0]));
  EXPECT_TRUE(ContainsSender(senders, kVideoTracks[0]));
  EXPECT_TRUE(ContainsSender(senders, kAudioTracks[1]));
  EXPECT_TRUE(ContainsSender(senders, kVideoTracks[1]));

  // Remove an audio and video track.
  pc_->RemoveStream(stream_collection->at(0));
  stream_collection = CreateStreamCollection(1, 1);
  pc_->AddStream(stream_collection->at(0));
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  EXPECT_TRUE(DoSetLocalDescription(std::move(offer)));

  senders = pc_->GetSenders();
  EXPECT_EQ(2u, senders.size());
  EXPECT_TRUE(ContainsSender(senders, kAudioTracks[0]));
  EXPECT_TRUE(ContainsSender(senders, kVideoTracks[0]));
  EXPECT_FALSE(ContainsSender(senders, kAudioTracks[1]));
  EXPECT_FALSE(ContainsSender(senders, kVideoTracks[1]));
}

// This tests that an RtpSender is created when the local description is set
// before adding a local stream.
// Don't run under Unified Plan since this behavior is Plan B specific.
TEST_F(PeerConnectionInterfaceTestPlanB,
       AddLocalStreamAfterLocalDescriptionChanged) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);

  rtc::scoped_refptr<StreamCollection> stream_collection =
      CreateStreamCollection(1, 2);
  // Add a stream to create the offer, but remove it afterwards.
  pc_->AddStream(stream_collection->at(0));
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  pc_->RemoveStream(stream_collection->at(0));

  EXPECT_TRUE(DoSetLocalDescription(std::move(offer)));
  auto senders = pc_->GetSenders();
  EXPECT_EQ(0u, senders.size());

  pc_->AddStream(stream_collection->at(0));
  senders = pc_->GetSenders();
  EXPECT_EQ(4u, senders.size());
  EXPECT_TRUE(ContainsSender(senders, kAudioTracks[0]));
  EXPECT_TRUE(ContainsSender(senders, kVideoTracks[0]));
  EXPECT_TRUE(ContainsSender(senders, kAudioTracks[1]));
  EXPECT_TRUE(ContainsSender(senders, kVideoTracks[1]));
}

// This tests that the expected behavior occurs if the SSRC on a local track is
// changed when SetLocalDescription is called.
TEST_P(PeerConnectionInterfaceTest,
       ChangeSsrcOnTrackInLocalSessionDescription) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);

  AddAudioTrack(kAudioTracks[0]);
  AddVideoTrack(kVideoTracks[0]);
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  // Grab a copy of the offer before it gets passed into the PC.
  std::unique_ptr<SessionDescriptionInterface> modified_offer =
      webrtc::CreateSessionDescription(
          webrtc::SdpType::kOffer, offer->session_id(),
          offer->session_version(),
          absl::WrapUnique(offer->description()->Copy()));
  EXPECT_TRUE(DoSetLocalDescription(std::move(offer)));

  auto senders = pc_->GetSenders();
  EXPECT_EQ(2u, senders.size());
  EXPECT_TRUE(ContainsSender(senders, kAudioTracks[0]));
  EXPECT_TRUE(ContainsSender(senders, kVideoTracks[0]));

  // Change the ssrc of the audio and video track.
  cricket::MediaContentDescription* desc =
      cricket::GetFirstAudioContentDescription(modified_offer->description());
  ASSERT_TRUE(desc != NULL);
  for (StreamParams& stream : desc->mutable_streams()) {
    for (unsigned int& ssrc : stream.ssrcs) {
      ++ssrc;
    }
  }

  desc =
      cricket::GetFirstVideoContentDescription(modified_offer->description());
  ASSERT_TRUE(desc != NULL);
  for (StreamParams& stream : desc->mutable_streams()) {
    for (unsigned int& ssrc : stream.ssrcs) {
      ++ssrc;
    }
  }

  EXPECT_TRUE(DoSetLocalDescription(std::move(modified_offer)));
  senders = pc_->GetSenders();
  EXPECT_EQ(2u, senders.size());
  EXPECT_TRUE(ContainsSender(senders, kAudioTracks[0]));
  EXPECT_TRUE(ContainsSender(senders, kVideoTracks[0]));
  // TODO(deadbeef): Once RtpSenders expose parameters, check that the SSRC
  // changed.
}

// This tests that the expected behavior occurs if a new session description is
// set with the same tracks, but on a different MediaStream.
// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB,
       SignalSameTracksInSeparateMediaStream) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);

  rtc::scoped_refptr<StreamCollection> stream_collection =
      CreateStreamCollection(2, 1);
  pc_->AddStream(stream_collection->at(0));
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  EXPECT_TRUE(DoSetLocalDescription(std::move(offer)));

  auto senders = pc_->GetSenders();
  EXPECT_EQ(2u, senders.size());
  EXPECT_TRUE(ContainsSender(senders, kAudioTracks[0], kStreams[0]));
  EXPECT_TRUE(ContainsSender(senders, kVideoTracks[0], kStreams[0]));

  // Add a new MediaStream but with the same tracks as in the first stream.
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream_1(
      webrtc::MediaStream::Create(kStreams[1]));
  stream_1->AddTrack(stream_collection->at(0)->GetVideoTracks()[0]);
  stream_1->AddTrack(stream_collection->at(0)->GetAudioTracks()[0]);
  pc_->AddStream(stream_1);

  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  EXPECT_TRUE(DoSetLocalDescription(std::move(offer)));

  auto new_senders = pc_->GetSenders();
  // Should be the same senders as before, but with updated stream id.
  // Note that this behavior is subject to change in the future.
  // We may decide the PC should ignore existing tracks in AddStream.
  EXPECT_EQ(senders, new_senders);
  EXPECT_TRUE(ContainsSender(new_senders, kAudioTracks[0], kStreams[1]));
  EXPECT_TRUE(ContainsSender(new_senders, kVideoTracks[0], kStreams[1]));
}

// This tests that PeerConnectionObserver::OnAddTrack is correctly called.
TEST_P(PeerConnectionInterfaceTest, OnAddTrackCallback) {
  FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           true);
  CreatePeerConnection(&constraints);
  CreateAndSetRemoteOffer(kSdpStringWithStream1AudioTrackOnly);
  EXPECT_EQ(observer_.num_added_tracks_, 1);
  EXPECT_EQ(observer_.last_added_track_label_, kAudioTracks[0]);

  // Create and set the updated remote SDP.
  CreateAndSetRemoteOffer(kSdpStringWithStream1PlanB);
  EXPECT_EQ(observer_.num_added_tracks_, 2);
  EXPECT_EQ(observer_.last_added_track_label_, kVideoTracks[0]);
}

// Test that when SetConfiguration is called and the configuration is
// changing, the next offer causes an ICE restart.
TEST_P(PeerConnectionInterfaceTest, SetConfigurationCausingIceRestart) {
  PeerConnectionInterface::RTCConfiguration config;
  config.type = PeerConnectionInterface::kRelay;
  // Need to pass default constraints to prevent disabling of DTLS...
  FakeConstraints default_constraints;
  CreatePeerConnection(config, &default_constraints);
  config = pc_->GetConfiguration();
  AddAudioTrack(kAudioTracks[0], {kStreamId1});
  AddVideoTrack(kVideoTracks[0], {kStreamId1});

  // Do initial offer/answer so there's something to restart.
  CreateOfferAsLocalDescription();
  CreateAnswerAsRemoteDescription(GetSdpStringWithStream1());

  // Grab the ufrags.
  std::vector<std::string> initial_ufrags = GetUfrags(pc_->local_description());

  // Change ICE policy, which should trigger an ICE restart on the next offer.
  config.type = PeerConnectionInterface::kAll;
  EXPECT_TRUE(pc_->SetConfiguration(config));
  CreateOfferAsLocalDescription();

  // Grab the new ufrags.
  std::vector<std::string> subsequent_ufrags =
      GetUfrags(pc_->local_description());

  // Sanity check.
  EXPECT_EQ(initial_ufrags.size(), subsequent_ufrags.size());
  // Check that each ufrag is different.
  for (int i = 0; i < static_cast<int>(initial_ufrags.size()); ++i) {
    EXPECT_NE(initial_ufrags[i], subsequent_ufrags[i]);
  }
}

// Test that when SetConfiguration is called and the configuration *isn't*
// changing, the next offer does *not* cause an ICE restart.
TEST_P(PeerConnectionInterfaceTest, SetConfigurationNotCausingIceRestart) {
  PeerConnectionInterface::RTCConfiguration config;
  config.type = PeerConnectionInterface::kRelay;
  // Need to pass default constraints to prevent disabling of DTLS...
  FakeConstraints default_constraints;
  CreatePeerConnection(config, &default_constraints);
  config = pc_->GetConfiguration();
  AddAudioTrack(kAudioTracks[0]);
  AddVideoTrack(kVideoTracks[0]);

  // Do initial offer/answer so there's something to restart.
  CreateOfferAsLocalDescription();
  CreateAnswerAsRemoteDescription(GetSdpStringWithStream1());

  // Grab the ufrags.
  std::vector<std::string> initial_ufrags = GetUfrags(pc_->local_description());

  // Call SetConfiguration with a config identical to what the PC was
  // constructed with.
  EXPECT_TRUE(pc_->SetConfiguration(config));
  CreateOfferAsLocalDescription();

  // Grab the new ufrags.
  std::vector<std::string> subsequent_ufrags =
      GetUfrags(pc_->local_description());

  EXPECT_EQ(initial_ufrags, subsequent_ufrags);
}

// Test for a weird corner case scenario:
// 1. Audio/video session established.
// 2. SetConfiguration changes ICE config; ICE restart needed.
// 3. ICE restart initiated by remote peer, but only for one m= section.
// 4. Next createOffer should initiate an ICE restart, but only for the other
//    m= section; it would be pointless to do an ICE restart for the m= section
//    that was already restarted.
TEST_P(PeerConnectionInterfaceTest, SetConfigurationCausingPartialIceRestart) {
  PeerConnectionInterface::RTCConfiguration config;
  config.type = PeerConnectionInterface::kRelay;
  // Need to pass default constraints to prevent disabling of DTLS...
  FakeConstraints default_constraints;
  CreatePeerConnection(config, &default_constraints);
  config = pc_->GetConfiguration();
  AddAudioTrack(kAudioTracks[0], {kStreamId1});
  AddVideoTrack(kVideoTracks[0], {kStreamId1});

  // Do initial offer/answer so there's something to restart.
  CreateOfferAsLocalDescription();
  CreateAnswerAsRemoteDescription(GetSdpStringWithStream1());

  // Change ICE policy, which should set the "needs-ice-restart" flag.
  config.type = PeerConnectionInterface::kAll;
  EXPECT_TRUE(pc_->SetConfiguration(config));

  // Do ICE restart for the first m= section, initiated by remote peer.
  std::unique_ptr<webrtc::SessionDescriptionInterface> remote_offer(
      webrtc::CreateSessionDescription(SdpType::kOffer,
                                       GetSdpStringWithStream1(), nullptr));
  ASSERT_TRUE(remote_offer);
  remote_offer->description()->transport_infos()[0].description.ice_ufrag =
      "modified";
  EXPECT_TRUE(DoSetRemoteDescription(std::move(remote_offer)));
  CreateAnswerAsLocalDescription();

  // Grab the ufrags.
  std::vector<std::string> initial_ufrags = GetUfrags(pc_->local_description());
  ASSERT_EQ(2U, initial_ufrags.size());

  // Create offer and grab the new ufrags.
  CreateOfferAsLocalDescription();
  std::vector<std::string> subsequent_ufrags =
      GetUfrags(pc_->local_description());
  ASSERT_EQ(2U, subsequent_ufrags.size());

  // Ensure that only the ufrag for the second m= section changed.
  EXPECT_EQ(initial_ufrags[0], subsequent_ufrags[0]);
  EXPECT_NE(initial_ufrags[1], subsequent_ufrags[1]);
}

// Tests that the methods to return current/pending descriptions work as
// expected at different points in the offer/answer exchange. This test does
// one offer/answer exchange as the offerer, then another as the answerer.
TEST_P(PeerConnectionInterfaceTest, CurrentAndPendingDescriptions) {
  // This disables DTLS so we can apply an answer to ourselves.
  CreatePeerConnection();

  // Create initial local offer and get SDP (which will also be used as
  // answer/pranswer);
  std::unique_ptr<SessionDescriptionInterface> local_offer;
  ASSERT_TRUE(DoCreateOffer(&local_offer, nullptr));
  std::string sdp;
  EXPECT_TRUE(local_offer->ToString(&sdp));

  // Set local offer.
  SessionDescriptionInterface* local_offer_ptr = local_offer.get();
  EXPECT_TRUE(DoSetLocalDescription(std::move(local_offer)));
  EXPECT_EQ(local_offer_ptr, pc_->pending_local_description());
  EXPECT_EQ(nullptr, pc_->pending_remote_description());
  EXPECT_EQ(nullptr, pc_->current_local_description());
  EXPECT_EQ(nullptr, pc_->current_remote_description());

  // Set remote pranswer.
  std::unique_ptr<SessionDescriptionInterface> remote_pranswer(
      webrtc::CreateSessionDescription(SdpType::kPrAnswer, sdp));
  SessionDescriptionInterface* remote_pranswer_ptr = remote_pranswer.get();
  EXPECT_TRUE(DoSetRemoteDescription(std::move(remote_pranswer)));
  EXPECT_EQ(local_offer_ptr, pc_->pending_local_description());
  EXPECT_EQ(remote_pranswer_ptr, pc_->pending_remote_description());
  EXPECT_EQ(nullptr, pc_->current_local_description());
  EXPECT_EQ(nullptr, pc_->current_remote_description());

  // Set remote answer.
  std::unique_ptr<SessionDescriptionInterface> remote_answer(
      webrtc::CreateSessionDescription(SdpType::kAnswer, sdp));
  SessionDescriptionInterface* remote_answer_ptr = remote_answer.get();
  EXPECT_TRUE(DoSetRemoteDescription(std::move(remote_answer)));
  EXPECT_EQ(nullptr, pc_->pending_local_description());
  EXPECT_EQ(nullptr, pc_->pending_remote_description());
  EXPECT_EQ(local_offer_ptr, pc_->current_local_description());
  EXPECT_EQ(remote_answer_ptr, pc_->current_remote_description());

  // Set remote offer.
  std::unique_ptr<SessionDescriptionInterface> remote_offer(
      webrtc::CreateSessionDescription(SdpType::kOffer, sdp));
  SessionDescriptionInterface* remote_offer_ptr = remote_offer.get();
  EXPECT_TRUE(DoSetRemoteDescription(std::move(remote_offer)));
  EXPECT_EQ(remote_offer_ptr, pc_->pending_remote_description());
  EXPECT_EQ(nullptr, pc_->pending_local_description());
  EXPECT_EQ(local_offer_ptr, pc_->current_local_description());
  EXPECT_EQ(remote_answer_ptr, pc_->current_remote_description());

  // Set local pranswer.
  std::unique_ptr<SessionDescriptionInterface> local_pranswer(
      webrtc::CreateSessionDescription(SdpType::kPrAnswer, sdp));
  SessionDescriptionInterface* local_pranswer_ptr = local_pranswer.get();
  EXPECT_TRUE(DoSetLocalDescription(std::move(local_pranswer)));
  EXPECT_EQ(remote_offer_ptr, pc_->pending_remote_description());
  EXPECT_EQ(local_pranswer_ptr, pc_->pending_local_description());
  EXPECT_EQ(local_offer_ptr, pc_->current_local_description());
  EXPECT_EQ(remote_answer_ptr, pc_->current_remote_description());

  // Set local answer.
  std::unique_ptr<SessionDescriptionInterface> local_answer(
      webrtc::CreateSessionDescription(SdpType::kAnswer, sdp));
  SessionDescriptionInterface* local_answer_ptr = local_answer.get();
  EXPECT_TRUE(DoSetLocalDescription(std::move(local_answer)));
  EXPECT_EQ(nullptr, pc_->pending_remote_description());
  EXPECT_EQ(nullptr, pc_->pending_local_description());
  EXPECT_EQ(remote_offer_ptr, pc_->current_remote_description());
  EXPECT_EQ(local_answer_ptr, pc_->current_local_description());
}

// Tests that it won't crash when calling StartRtcEventLog or StopRtcEventLog
// after the PeerConnection is closed.
// This version tests the StartRtcEventLog version that receives a file.
TEST_P(PeerConnectionInterfaceTest,
       StartAndStopLoggingToFileAfterPeerConnectionClosed) {
  CreatePeerConnection();
  // The RtcEventLog will be reset when the PeerConnection is closed.
  pc_->Close();

  auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
  std::string filename = webrtc::test::OutputPath() +
                         test_info->test_case_name() + test_info->name();
  rtc::PlatformFile file = rtc::CreatePlatformFile(filename);

  constexpr int64_t max_size_bytes = 1024;

  EXPECT_FALSE(pc_->StartRtcEventLog(file, max_size_bytes));
  pc_->StopRtcEventLog();

  // Cleanup.
  rtc::ClosePlatformFile(file);
  rtc::RemoveFile(filename);
}

// Tests that it won't crash when calling StartRtcEventLog or StopRtcEventLog
// after the PeerConnection is closed.
// This version tests the StartRtcEventLog version that receives an object
// of type |RtcEventLogOutput|.
TEST_P(PeerConnectionInterfaceTest,
       StartAndStopLoggingToOutputAfterPeerConnectionClosed) {
  CreatePeerConnection();
  // The RtcEventLog will be reset when the PeerConnection is closed.
  pc_->Close();

  rtc::PlatformFile file = 0;
  int64_t max_size_bytes = 1024;
  EXPECT_FALSE(pc_->StartRtcEventLog(
      absl::make_unique<webrtc::RtcEventLogOutputFile>(file, max_size_bytes),
      webrtc::RtcEventLog::kImmediateOutput));
  pc_->StopRtcEventLog();
}

// Test that generated offers/answers include "ice-option:trickle".
TEST_P(PeerConnectionInterfaceTest, OffersAndAnswersHaveTrickleIceOption) {
  CreatePeerConnection();

  // First, create an offer with audio/video.
  FakeConstraints constraints;
  constraints.SetMandatoryReceiveAudio(true);
  constraints.SetMandatoryReceiveVideo(true);
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, &constraints));
  cricket::SessionDescription* desc = offer->description();
  ASSERT_EQ(2u, desc->transport_infos().size());
  EXPECT_TRUE(desc->transport_infos()[0].description.HasOption("trickle"));
  EXPECT_TRUE(desc->transport_infos()[1].description.HasOption("trickle"));

  // Apply the offer as a remote description, then create an answer.
  EXPECT_TRUE(DoSetRemoteDescription(std::move(offer)));
  std::unique_ptr<SessionDescriptionInterface> answer;
  ASSERT_TRUE(DoCreateAnswer(&answer, &constraints));
  desc = answer->description();
  ASSERT_EQ(2u, desc->transport_infos().size());
  EXPECT_TRUE(desc->transport_infos()[0].description.HasOption("trickle"));
  EXPECT_TRUE(desc->transport_infos()[1].description.HasOption("trickle"));
}

// Test that ICE renomination isn't offered if it's not enabled in the PC's
// RTCConfiguration.
TEST_P(PeerConnectionInterfaceTest, IceRenominationNotOffered) {
  PeerConnectionInterface::RTCConfiguration config;
  config.enable_ice_renomination = false;
  CreatePeerConnection(config, nullptr);
  AddAudioTrack("foo");

  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  cricket::SessionDescription* desc = offer->description();
  EXPECT_EQ(1u, desc->transport_infos().size());
  EXPECT_FALSE(
      desc->transport_infos()[0].description.GetIceParameters().renomination);
}

// Test that the ICE renomination option is present in generated offers/answers
// if it's enabled in the PC's RTCConfiguration.
TEST_P(PeerConnectionInterfaceTest, IceRenominationOptionInOfferAndAnswer) {
  PeerConnectionInterface::RTCConfiguration config;
  config.enable_ice_renomination = true;
  CreatePeerConnection(config, nullptr);
  AddAudioTrack("foo");

  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  cricket::SessionDescription* desc = offer->description();
  EXPECT_EQ(1u, desc->transport_infos().size());
  EXPECT_TRUE(
      desc->transport_infos()[0].description.GetIceParameters().renomination);

  // Set the offer as a remote description, then create an answer and ensure it
  // has the renomination flag too.
  EXPECT_TRUE(DoSetRemoteDescription(std::move(offer)));
  std::unique_ptr<SessionDescriptionInterface> answer;
  ASSERT_TRUE(DoCreateAnswer(&answer, nullptr));
  desc = answer->description();
  EXPECT_EQ(1u, desc->transport_infos().size());
  EXPECT_TRUE(
      desc->transport_infos()[0].description.GetIceParameters().renomination);
}

// Test that if CreateOffer is called with the deprecated "offer to receive
// audio/video" constraints, they're processed and result in an offer with
// audio/video sections just as if RTCOfferAnswerOptions had been used.
TEST_P(PeerConnectionInterfaceTest, CreateOfferWithOfferToReceiveConstraints) {
  CreatePeerConnection();

  FakeConstraints constraints;
  constraints.SetMandatoryReceiveAudio(true);
  constraints.SetMandatoryReceiveVideo(true);
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, &constraints));

  cricket::SessionDescription* desc = offer->description();
  const cricket::ContentInfo* audio = cricket::GetFirstAudioContent(desc);
  const cricket::ContentInfo* video = cricket::GetFirstVideoContent(desc);
  ASSERT_NE(nullptr, audio);
  ASSERT_NE(nullptr, video);
  EXPECT_FALSE(audio->rejected);
  EXPECT_FALSE(video->rejected);
}

// Test that if CreateAnswer is called with the deprecated "offer to receive
// audio/video" constraints, they're processed and can be used to reject an
// offered m= section just as can be done with RTCOfferAnswerOptions;
// Don't run under Unified Plan since this behavior is not supported.
TEST_F(PeerConnectionInterfaceTestPlanB,
       CreateAnswerWithOfferToReceiveConstraints) {
  CreatePeerConnection();

  // First, create an offer with audio/video and apply it as a remote
  // description.
  FakeConstraints constraints;
  constraints.SetMandatoryReceiveAudio(true);
  constraints.SetMandatoryReceiveVideo(true);
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, &constraints));
  EXPECT_TRUE(DoSetRemoteDescription(std::move(offer)));

  // Now create answer that rejects audio/video.
  constraints.SetMandatoryReceiveAudio(false);
  constraints.SetMandatoryReceiveVideo(false);
  std::unique_ptr<SessionDescriptionInterface> answer;
  ASSERT_TRUE(DoCreateAnswer(&answer, &constraints));

  cricket::SessionDescription* desc = answer->description();
  const cricket::ContentInfo* audio = cricket::GetFirstAudioContent(desc);
  const cricket::ContentInfo* video = cricket::GetFirstVideoContent(desc);
  ASSERT_NE(nullptr, audio);
  ASSERT_NE(nullptr, video);
  EXPECT_TRUE(audio->rejected);
  EXPECT_TRUE(video->rejected);
}

// Test that negotiation can succeed with a data channel only, and with the max
// bundle policy. Previously there was a bug that prevented this.
#ifdef HAVE_SCTP
TEST_P(PeerConnectionInterfaceTest, DataChannelOnlyOfferWithMaxBundlePolicy) {
#else
TEST_P(PeerConnectionInterfaceTest,
       DISABLED_DataChannelOnlyOfferWithMaxBundlePolicy) {
#endif  // HAVE_SCTP
  PeerConnectionInterface::RTCConfiguration config;
  config.bundle_policy = PeerConnectionInterface::kBundlePolicyMaxBundle;
  CreatePeerConnection(config, nullptr);

  // First, create an offer with only a data channel and apply it as a remote
  // description.
  pc_->CreateDataChannel("test", nullptr);
  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  EXPECT_TRUE(DoSetRemoteDescription(std::move(offer)));

  // Create and set answer as well.
  std::unique_ptr<SessionDescriptionInterface> answer;
  ASSERT_TRUE(DoCreateAnswer(&answer, nullptr));
  EXPECT_TRUE(DoSetLocalDescription(std::move(answer)));
}

TEST_P(PeerConnectionInterfaceTest, SetBitrateWithoutMinSucceeds) {
  CreatePeerConnection();
  PeerConnectionInterface::BitrateParameters bitrate;
  bitrate.current_bitrate_bps = 100000;
  EXPECT_TRUE(pc_->SetBitrate(bitrate).ok());
}

TEST_P(PeerConnectionInterfaceTest, SetBitrateNegativeMinFails) {
  CreatePeerConnection();
  PeerConnectionInterface::BitrateParameters bitrate;
  bitrate.min_bitrate_bps = -1;
  EXPECT_FALSE(pc_->SetBitrate(bitrate).ok());
}

TEST_P(PeerConnectionInterfaceTest, SetBitrateCurrentLessThanMinFails) {
  CreatePeerConnection();
  PeerConnectionInterface::BitrateParameters bitrate;
  bitrate.min_bitrate_bps = 5;
  bitrate.current_bitrate_bps = 3;
  EXPECT_FALSE(pc_->SetBitrate(bitrate).ok());
}

TEST_P(PeerConnectionInterfaceTest, SetBitrateCurrentNegativeFails) {
  CreatePeerConnection();
  PeerConnectionInterface::BitrateParameters bitrate;
  bitrate.current_bitrate_bps = -1;
  EXPECT_FALSE(pc_->SetBitrate(bitrate).ok());
}

TEST_P(PeerConnectionInterfaceTest, SetBitrateMaxLessThanCurrentFails) {
  CreatePeerConnection();
  PeerConnectionInterface::BitrateParameters bitrate;
  bitrate.current_bitrate_bps = 10;
  bitrate.max_bitrate_bps = 8;
  EXPECT_FALSE(pc_->SetBitrate(bitrate).ok());
}

TEST_P(PeerConnectionInterfaceTest, SetBitrateMaxLessThanMinFails) {
  CreatePeerConnection();
  PeerConnectionInterface::BitrateParameters bitrate;
  bitrate.min_bitrate_bps = 10;
  bitrate.max_bitrate_bps = 8;
  EXPECT_FALSE(pc_->SetBitrate(bitrate).ok());
}

TEST_P(PeerConnectionInterfaceTest, SetBitrateMaxNegativeFails) {
  CreatePeerConnection();
  PeerConnectionInterface::BitrateParameters bitrate;
  bitrate.max_bitrate_bps = -1;
  EXPECT_FALSE(pc_->SetBitrate(bitrate).ok());
}

// ice_regather_interval_range requires WebRTC to be configured for continual
// gathering already.
TEST_P(PeerConnectionInterfaceTest,
       SetIceRegatherIntervalRangeWithoutContinualGatheringFails) {
  PeerConnectionInterface::RTCConfiguration config;
  config.ice_regather_interval_range.emplace(1000, 2000);
  config.continual_gathering_policy =
      PeerConnectionInterface::ContinualGatheringPolicy::GATHER_ONCE;
  CreatePeerConnectionExpectFail(config);
}

// Ensures that there is no error when ice_regather_interval_range is set with
// continual gathering enabled.
TEST_P(PeerConnectionInterfaceTest,
       SetIceRegatherIntervalRangeWithContinualGathering) {
  PeerConnectionInterface::RTCConfiguration config;
  config.ice_regather_interval_range.emplace(1000, 2000);
  config.continual_gathering_policy =
      PeerConnectionInterface::ContinualGatheringPolicy::GATHER_CONTINUALLY;
  CreatePeerConnection(config, nullptr);
}

// The current bitrate from BitrateSettings is currently clamped
// by Call's BitrateConstraints, which comes from the SDP or a default value.
// This test checks that a call to SetBitrate with a current bitrate that will
// be clamped succeeds.
TEST_P(PeerConnectionInterfaceTest, SetBitrateCurrentLessThanImplicitMin) {
  CreatePeerConnection();
  PeerConnectionInterface::BitrateParameters bitrate;
  bitrate.current_bitrate_bps = 1;
  EXPECT_TRUE(pc_->SetBitrate(bitrate).ok());
}

// The following tests verify that the offer can be created correctly.
TEST_P(PeerConnectionInterfaceTest,
       CreateOfferFailsWithInvalidOfferToReceiveAudio) {
  RTCOfferAnswerOptions rtc_options;

  // Setting offer_to_receive_audio to a value lower than kUndefined or greater
  // than kMaxOfferToReceiveMedia should be treated as invalid.
  rtc_options.offer_to_receive_audio = RTCOfferAnswerOptions::kUndefined - 1;
  CreatePeerConnection();
  EXPECT_FALSE(CreateOfferWithOptions(rtc_options));

  rtc_options.offer_to_receive_audio =
      RTCOfferAnswerOptions::kMaxOfferToReceiveMedia + 1;
  EXPECT_FALSE(CreateOfferWithOptions(rtc_options));
}

TEST_P(PeerConnectionInterfaceTest,
       CreateOfferFailsWithInvalidOfferToReceiveVideo) {
  RTCOfferAnswerOptions rtc_options;

  // Setting offer_to_receive_video to a value lower than kUndefined or greater
  // than kMaxOfferToReceiveMedia should be treated as invalid.
  rtc_options.offer_to_receive_video = RTCOfferAnswerOptions::kUndefined - 1;
  CreatePeerConnection();
  EXPECT_FALSE(CreateOfferWithOptions(rtc_options));

  rtc_options.offer_to_receive_video =
      RTCOfferAnswerOptions::kMaxOfferToReceiveMedia + 1;
  EXPECT_FALSE(CreateOfferWithOptions(rtc_options));
}

// Test that the audio and video content will be added to an offer if both
// |offer_to_receive_audio| and |offer_to_receive_video| options are 1.
TEST_P(PeerConnectionInterfaceTest, CreateOfferWithAudioVideoOptions) {
  RTCOfferAnswerOptions rtc_options;
  rtc_options.offer_to_receive_audio = 1;
  rtc_options.offer_to_receive_video = 1;

  std::unique_ptr<SessionDescriptionInterface> offer;
  CreatePeerConnection();
  offer = CreateOfferWithOptions(rtc_options);
  ASSERT_TRUE(offer);
  EXPECT_NE(nullptr, GetFirstAudioContent(offer->description()));
  EXPECT_NE(nullptr, GetFirstVideoContent(offer->description()));
}

// Test that only audio content will be added to the offer if only
// |offer_to_receive_audio| options is 1.
TEST_P(PeerConnectionInterfaceTest, CreateOfferWithAudioOnlyOptions) {
  RTCOfferAnswerOptions rtc_options;
  rtc_options.offer_to_receive_audio = 1;
  rtc_options.offer_to_receive_video = 0;

  std::unique_ptr<SessionDescriptionInterface> offer;
  CreatePeerConnection();
  offer = CreateOfferWithOptions(rtc_options);
  ASSERT_TRUE(offer);
  EXPECT_NE(nullptr, GetFirstAudioContent(offer->description()));
  EXPECT_EQ(nullptr, GetFirstVideoContent(offer->description()));
}

// Test that only video content will be added if only |offer_to_receive_video|
// options is 1.
TEST_P(PeerConnectionInterfaceTest, CreateOfferWithVideoOnlyOptions) {
  RTCOfferAnswerOptions rtc_options;
  rtc_options.offer_to_receive_audio = 0;
  rtc_options.offer_to_receive_video = 1;

  std::unique_ptr<SessionDescriptionInterface> offer;
  CreatePeerConnection();
  offer = CreateOfferWithOptions(rtc_options);
  ASSERT_TRUE(offer);
  EXPECT_EQ(nullptr, GetFirstAudioContent(offer->description()));
  EXPECT_NE(nullptr, GetFirstVideoContent(offer->description()));
}

// Test that no media content will be added to the offer if using default
// RTCOfferAnswerOptions.
TEST_P(PeerConnectionInterfaceTest, CreateOfferWithDefaultOfferAnswerOptions) {
  RTCOfferAnswerOptions rtc_options;

  std::unique_ptr<SessionDescriptionInterface> offer;
  CreatePeerConnection();
  offer = CreateOfferWithOptions(rtc_options);
  ASSERT_TRUE(offer);
  EXPECT_EQ(nullptr, GetFirstAudioContent(offer->description()));
  EXPECT_EQ(nullptr, GetFirstVideoContent(offer->description()));
}

// Test that if |ice_restart| is true, the ufrag/pwd will change, otherwise
// ufrag/pwd will be the same in the new offer.
TEST_P(PeerConnectionInterfaceTest, CreateOfferWithIceRestart) {
  CreatePeerConnection();

  RTCOfferAnswerOptions rtc_options;
  rtc_options.ice_restart = false;
  rtc_options.offer_to_receive_audio = 1;

  std::unique_ptr<SessionDescriptionInterface> offer;
  CreateOfferWithOptionsAsLocalDescription(&offer, rtc_options);
  std::string mid = cricket::GetFirstAudioContent(offer->description())->name;
  auto ufrag1 =
      offer->description()->GetTransportInfoByName(mid)->description.ice_ufrag;
  auto pwd1 =
      offer->description()->GetTransportInfoByName(mid)->description.ice_pwd;

  // |ice_restart| is false, the ufrag/pwd shouldn't change.
  CreateOfferWithOptionsAsLocalDescription(&offer, rtc_options);
  auto ufrag2 =
      offer->description()->GetTransportInfoByName(mid)->description.ice_ufrag;
  auto pwd2 =
      offer->description()->GetTransportInfoByName(mid)->description.ice_pwd;

  // |ice_restart| is true, the ufrag/pwd should change.
  rtc_options.ice_restart = true;
  CreateOfferWithOptionsAsLocalDescription(&offer, rtc_options);
  auto ufrag3 =
      offer->description()->GetTransportInfoByName(mid)->description.ice_ufrag;
  auto pwd3 =
      offer->description()->GetTransportInfoByName(mid)->description.ice_pwd;

  EXPECT_EQ(ufrag1, ufrag2);
  EXPECT_EQ(pwd1, pwd2);
  EXPECT_NE(ufrag2, ufrag3);
  EXPECT_NE(pwd2, pwd3);
}

// Test that if |use_rtp_mux| is true, the bundling will be enabled in the
// offer; if it is false, there won't be any bundle group in the offer.
TEST_P(PeerConnectionInterfaceTest, CreateOfferWithRtpMux) {
  RTCOfferAnswerOptions rtc_options;
  rtc_options.offer_to_receive_audio = 1;
  rtc_options.offer_to_receive_video = 1;

  std::unique_ptr<SessionDescriptionInterface> offer;
  CreatePeerConnection();

  rtc_options.use_rtp_mux = true;
  offer = CreateOfferWithOptions(rtc_options);
  ASSERT_TRUE(offer);
  EXPECT_NE(nullptr, GetFirstAudioContent(offer->description()));
  EXPECT_NE(nullptr, GetFirstVideoContent(offer->description()));
  EXPECT_TRUE(offer->description()->HasGroup(cricket::GROUP_TYPE_BUNDLE));

  rtc_options.use_rtp_mux = false;
  offer = CreateOfferWithOptions(rtc_options);
  ASSERT_TRUE(offer);
  EXPECT_NE(nullptr, GetFirstAudioContent(offer->description()));
  EXPECT_NE(nullptr, GetFirstVideoContent(offer->description()));
  EXPECT_FALSE(offer->description()->HasGroup(cricket::GROUP_TYPE_BUNDLE));
}

// This test ensures OnRenegotiationNeeded is called when we add track with
// MediaStream -> AddTrack in the same way it is called when we add track with
// PeerConnection -> AddTrack.
// The test can be removed once addStream is rewritten in terms of addTrack
// https://bugs.chromium.org/p/webrtc/issues/detail?id=7815
// Don't run under Unified Plan since the stream API is not available.
TEST_F(PeerConnectionInterfaceTestPlanB,
       MediaStreamAddTrackRemoveTrackRenegotiate) {
  CreatePeerConnectionWithoutDtls();
  rtc::scoped_refptr<MediaStreamInterface> stream(
      pc_factory_->CreateLocalMediaStream(kStreamId1));
  pc_->AddStream(stream);
  rtc::scoped_refptr<AudioTrackInterface> audio_track(
      pc_factory_->CreateAudioTrack("audio_track", nullptr));
  rtc::scoped_refptr<VideoTrackInterface> video_track(
      pc_factory_->CreateVideoTrack(
          "video_track", pc_factory_->CreateVideoSource(
                             std::unique_ptr<cricket::VideoCapturer>(
                                 new cricket::FakeVideoCapturer()))));
  stream->AddTrack(audio_track);
  EXPECT_TRUE_WAIT(observer_.renegotiation_needed_, kTimeout);
  observer_.renegotiation_needed_ = false;

  stream->AddTrack(video_track);
  EXPECT_TRUE_WAIT(observer_.renegotiation_needed_, kTimeout);
  observer_.renegotiation_needed_ = false;

  stream->RemoveTrack(audio_track);
  EXPECT_TRUE_WAIT(observer_.renegotiation_needed_, kTimeout);
  observer_.renegotiation_needed_ = false;

  stream->RemoveTrack(video_track);
  EXPECT_TRUE_WAIT(observer_.renegotiation_needed_, kTimeout);
  observer_.renegotiation_needed_ = false;
}

// Tests that an error is returned if a description is applied that has fewer
// media sections than the existing description.
TEST_P(PeerConnectionInterfaceTest,
       MediaSectionCountEnforcedForSubsequentOffer) {
  CreatePeerConnection();
  AddAudioTrack("audio_label");
  AddVideoTrack("video_label");

  std::unique_ptr<SessionDescriptionInterface> offer;
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  EXPECT_TRUE(DoSetRemoteDescription(std::move(offer)));

  // A remote offer with fewer media sections should be rejected.
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  offer->description()->contents().pop_back();
  offer->description()->contents().pop_back();
  ASSERT_TRUE(offer->description()->contents().empty());
  EXPECT_FALSE(DoSetRemoteDescription(std::move(offer)));

  std::unique_ptr<SessionDescriptionInterface> answer;
  ASSERT_TRUE(DoCreateAnswer(&answer, nullptr));
  EXPECT_TRUE(DoSetLocalDescription(std::move(answer)));

  // A subsequent local offer with fewer media sections should be rejected.
  ASSERT_TRUE(DoCreateOffer(&offer, nullptr));
  offer->description()->contents().pop_back();
  offer->description()->contents().pop_back();
  ASSERT_TRUE(offer->description()->contents().empty());
  EXPECT_FALSE(DoSetLocalDescription(std::move(offer)));
}

INSTANTIATE_TEST_CASE_P(PeerConnectionInterfaceTest,
                        PeerConnectionInterfaceTest,
                        Values(SdpSemantics::kPlanB,
                               SdpSemantics::kUnifiedPlan));

class PeerConnectionMediaConfigTest : public testing::Test {
 protected:
  void SetUp() override {
    pcf_ = PeerConnectionFactoryForTest::CreatePeerConnectionFactoryForTest();
    pcf_->Initialize();
  }
  const cricket::MediaConfig TestCreatePeerConnection(
      const PeerConnectionInterface::RTCConfiguration& config,
      const MediaConstraintsInterface* constraints) {
    rtc::scoped_refptr<PeerConnectionInterface> pc(pcf_->CreatePeerConnection(
        config, constraints, nullptr, nullptr, &observer_));
    EXPECT_TRUE(pc.get());
    return pc->GetConfiguration().media_config;
  }

  rtc::scoped_refptr<PeerConnectionFactoryForTest> pcf_;
  MockPeerConnectionObserver observer_;
};

// This test verifies the default behaviour with no constraints and a
// default RTCConfiguration.
TEST_F(PeerConnectionMediaConfigTest, TestDefaults) {
  PeerConnectionInterface::RTCConfiguration config;
  FakeConstraints constraints;

  const cricket::MediaConfig& media_config =
      TestCreatePeerConnection(config, &constraints);

  EXPECT_FALSE(media_config.enable_dscp);
  EXPECT_TRUE(media_config.video.enable_cpu_adaptation);
  EXPECT_TRUE(media_config.video.enable_prerenderer_smoothing);
  EXPECT_FALSE(media_config.video.suspend_below_min_bitrate);
  EXPECT_FALSE(media_config.video.experiment_cpu_load_estimator);
}

// This test verifies the DSCP constraint is recognized and passed to
// the PeerConnection.
TEST_F(PeerConnectionMediaConfigTest, TestDscpConstraintTrue) {
  PeerConnectionInterface::RTCConfiguration config;
  FakeConstraints constraints;

  constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDscp, true);
  const cricket::MediaConfig& media_config =
      TestCreatePeerConnection(config, &constraints);

  EXPECT_TRUE(media_config.enable_dscp);
}

// This test verifies the cpu overuse detection constraint is
// recognized and passed to the PeerConnection.
TEST_F(PeerConnectionMediaConfigTest, TestCpuOveruseConstraintFalse) {
  PeerConnectionInterface::RTCConfiguration config;
  FakeConstraints constraints;

  constraints.AddOptional(
      webrtc::MediaConstraintsInterface::kCpuOveruseDetection, false);
  const cricket::MediaConfig media_config =
      TestCreatePeerConnection(config, &constraints);

  EXPECT_FALSE(media_config.video.enable_cpu_adaptation);
}

// This test verifies that the enable_prerenderer_smoothing flag is
// propagated from RTCConfiguration to the PeerConnection.
TEST_F(PeerConnectionMediaConfigTest, TestDisablePrerendererSmoothingTrue) {
  PeerConnectionInterface::RTCConfiguration config;
  FakeConstraints constraints;

  config.set_prerenderer_smoothing(false);
  const cricket::MediaConfig& media_config =
      TestCreatePeerConnection(config, &constraints);

  EXPECT_FALSE(media_config.video.enable_prerenderer_smoothing);
}

// This test verifies that the experiment_cpu_load_estimator flag is
// propagated from RTCConfiguration to the PeerConnection.
TEST_F(PeerConnectionMediaConfigTest, TestEnableExperimentCpuLoadEstimator) {
  PeerConnectionInterface::RTCConfiguration config;
  FakeConstraints constraints;

  config.set_experiment_cpu_load_estimator(true);
  const cricket::MediaConfig& media_config =
      TestCreatePeerConnection(config, &constraints);

  EXPECT_TRUE(media_config.video.experiment_cpu_load_estimator);
}

// This test verifies the suspend below min bitrate constraint is
// recognized and passed to the PeerConnection.
TEST_F(PeerConnectionMediaConfigTest,
       TestSuspendBelowMinBitrateConstraintTrue) {
  PeerConnectionInterface::RTCConfiguration config;
  FakeConstraints constraints;

  constraints.AddOptional(
      webrtc::MediaConstraintsInterface::kEnableVideoSuspendBelowMinBitrate,
      true);
  const cricket::MediaConfig media_config =
      TestCreatePeerConnection(config, &constraints);

  EXPECT_TRUE(media_config.video.suspend_below_min_bitrate);
}

// Tests a few random fields being different.
TEST(RTCConfigurationTest, ComparisonOperators) {
  PeerConnectionInterface::RTCConfiguration a;
  PeerConnectionInterface::RTCConfiguration b;
  EXPECT_EQ(a, b);

  PeerConnectionInterface::RTCConfiguration c;
  c.servers.push_back(PeerConnectionInterface::IceServer());
  EXPECT_NE(a, c);

  PeerConnectionInterface::RTCConfiguration d;
  d.type = PeerConnectionInterface::kRelay;
  EXPECT_NE(a, d);

  PeerConnectionInterface::RTCConfiguration e;
  e.audio_jitter_buffer_max_packets = 5;
  EXPECT_NE(a, e);

  PeerConnectionInterface::RTCConfiguration f;
  f.ice_connection_receiving_timeout = 1337;
  EXPECT_NE(a, f);

  PeerConnectionInterface::RTCConfiguration g;
  g.disable_ipv6 = true;
  EXPECT_NE(a, g);

  PeerConnectionInterface::RTCConfiguration h(
      PeerConnectionInterface::RTCConfigurationType::kAggressive);
  EXPECT_NE(a, h);
}
