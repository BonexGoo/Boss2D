/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#include BOSS_WEBRTC_U_modules__video_coding__codecs__h264__include__h264_h //original-code:"modules/video_coding/codecs/h264/include/h264.h"

#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_media__base__h264_profile_level_id_h //original-code:"media/base/h264_profile_level_id.h"

#if defined(WEBRTC_USE_H264)
#include BOSS_WEBRTC_U_modules__video_coding__codecs__h264__h264_decoder_impl_h //original-code:"modules/video_coding/codecs/h264/h264_decoder_impl.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__h264__h264_encoder_impl_h //original-code:"modules/video_coding/codecs/h264/h264_encoder_impl.h"
#endif

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace webrtc {

namespace {

#if defined(WEBRTC_USE_H264)
bool g_rtc_use_h264 = true;
#endif

// If H.264 OpenH264/FFmpeg codec is supported.
bool IsH264CodecSupported() {
#if defined(WEBRTC_USE_H264)
  return g_rtc_use_h264;
#else
  return false;
#endif
}

SdpVideoFormat CreateH264Format(H264::Profile profile,
                                H264::Level level,
                                const std::string& packetization_mode) {
  const absl::optional<std::string> profile_string =
      H264::ProfileLevelIdToString(H264::ProfileLevelId(profile, level));
  RTC_CHECK(profile_string);
  return SdpVideoFormat(
      cricket::kH264CodecName,
      {{cricket::kH264FmtpProfileLevelId, *profile_string},
       {cricket::kH264FmtpLevelAsymmetryAllowed, "1"},
       {cricket::kH264FmtpPacketizationMode, packetization_mode}});
}

}  // namespace

void DisableRtcUseH264() {
#if defined(WEBRTC_USE_H264)
  g_rtc_use_h264 = false;
#endif
}

std::vector<SdpVideoFormat> SupportedH264Codecs() {
  if (!IsH264CodecSupported())
    return std::vector<SdpVideoFormat>();
  // We only support encoding Constrained Baseline Profile (CBP), but the
  // decoder supports more profiles. We can list all profiles here that are
  // supported by the decoder and that are also supersets of CBP, i.e. the
  // decoder for that profile is required to be able to decode CBP. This means
  // we can encode and send CBP even though we negotiated a potentially
  // higher profile. See the H264 spec for more information.
  //
  // We support both packetization modes 0 (mandatory) and 1 (optional,
  // preferred).
  return {
      CreateH264Format(H264::kProfileBaseline, H264::kLevel3_1, "1"),
      CreateH264Format(H264::kProfileBaseline, H264::kLevel3_1, "0"),
      CreateH264Format(H264::kProfileConstrainedBaseline, H264::kLevel3_1, "1"),
      CreateH264Format(H264::kProfileConstrainedBaseline, H264::kLevel3_1,
                       "0")};
}

std::unique_ptr<H264Encoder> H264Encoder::Create(
    const cricket::VideoCodec& codec) {
  RTC_DCHECK(H264Encoder::IsSupported());
#if defined(WEBRTC_USE_H264)
  RTC_CHECK(g_rtc_use_h264);
  RTC_LOG(LS_INFO) << "Creating H264EncoderImpl.";
  return absl::make_unique<H264EncoderImpl>(codec);
#else
  RTC_NOTREACHED();
  return nullptr;
#endif
}

bool H264Encoder::IsSupported() {
  return IsH264CodecSupported();
}

std::unique_ptr<H264Decoder> H264Decoder::Create() {
  RTC_DCHECK(H264Decoder::IsSupported());
#if defined(WEBRTC_USE_H264)
  RTC_CHECK(g_rtc_use_h264);
  RTC_LOG(LS_INFO) << "Creating H264DecoderImpl.";
  return absl::make_unique<H264DecoderImpl>();
#else
  RTC_NOTREACHED();
  return nullptr;
#endif
}

bool H264Decoder::IsSupported() {
  return IsH264CodecSupported();
}

}  // namespace webrtc
