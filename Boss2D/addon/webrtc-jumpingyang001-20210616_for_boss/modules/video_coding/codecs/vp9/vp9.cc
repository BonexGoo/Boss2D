/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp9__include__vp9_h //original-code:"modules/video_coding/codecs/vp9/include/vp9.h"

#include <memory>

#include BOSS_WEBRTC_U_api__transport__field_trial_based_config_h //original-code:"api/transport/field_trial_based_config.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__vp9_profile_h //original-code:"api/video_codecs/vp9_profile.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp9__libvpx_vp9_decoder_h //original-code:"modules/video_coding/codecs/vp9/libvpx_vp9_decoder.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp9__libvpx_vp9_encoder_h //original-code:"modules/video_coding/codecs/vp9/libvpx_vp9_encoder.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include "vpx/vp8cx.h"
#include "vpx/vp8dx.h"
#include "vpx/vpx_codec.h"

namespace webrtc {

std::vector<SdpVideoFormat> SupportedVP9Codecs() {
#ifdef RTC_ENABLE_VP9
  // Profile 2 might not be available on some platforms until
  // https://bugs.chromium.org/p/webm/issues/detail?id=1544 is solved.
  static bool vpx_supports_high_bit_depth =
      (vpx_codec_get_caps(vpx_codec_vp9_cx()) & VPX_CODEC_CAP_HIGHBITDEPTH) !=
          0 &&
      (vpx_codec_get_caps(vpx_codec_vp9_dx()) & VPX_CODEC_CAP_HIGHBITDEPTH) !=
          0;

  std::vector<SdpVideoFormat> supported_formats{SdpVideoFormat(
      cricket::kVp9CodecName,
      {{kVP9FmtpProfileId, VP9ProfileToString(VP9Profile::kProfile0)}})};
  if (vpx_supports_high_bit_depth) {
    supported_formats.push_back(SdpVideoFormat(
        cricket::kVp9CodecName,
        {{kVP9FmtpProfileId, VP9ProfileToString(VP9Profile::kProfile2)}}));
  }

  return supported_formats;
#else
  return std::vector<SdpVideoFormat>();
#endif
}

std::vector<SdpVideoFormat> SupportedVP9DecoderCodecs() {
#ifdef RTC_ENABLE_VP9
  std::vector<SdpVideoFormat> supported_formats = SupportedVP9Codecs();
  // The WebRTC internal decoder supports VP9 profile 1. However, there's
  // currently no way of sending VP9 profile 1 using the internal encoder.
  // It would require extended support for I444, I422, and I440 buffers.
  supported_formats.push_back(SdpVideoFormat(
      cricket::kVp9CodecName,
      {{kVP9FmtpProfileId, VP9ProfileToString(VP9Profile::kProfile1)}}));
  return supported_formats;
#else
  return std::vector<SdpVideoFormat>();
#endif
}

std::unique_ptr<VP9Encoder> VP9Encoder::Create() {
#ifdef RTC_ENABLE_VP9
  return std::make_unique<LibvpxVp9Encoder>(cricket::VideoCodec(),
                                            LibvpxInterface::Create(),
                                            FieldTrialBasedConfig());
#else
  RTC_NOTREACHED();
  return nullptr;
#endif
}

std::unique_ptr<VP9Encoder> VP9Encoder::Create(
    const cricket::VideoCodec& codec) {
#ifdef RTC_ENABLE_VP9
  return std::make_unique<LibvpxVp9Encoder>(codec, LibvpxInterface::Create(),
                                            FieldTrialBasedConfig());
#else
  RTC_NOTREACHED();
  return nullptr;
#endif
}

std::unique_ptr<VP9Decoder> VP9Decoder::Create() {
#ifdef RTC_ENABLE_VP9
  return std::make_unique<LibvpxVp9Decoder>();
#else
  RTC_NOTREACHED();
  return nullptr;
#endif
}

}  // namespace webrtc
