/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_AUDIO_CODECS_ILBC_AUDIO_DECODER_ILBC_H_
#define API_AUDIO_CODECS_ILBC_AUDIO_DECODER_ILBC_H_

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_codec_pair_id_h //original-code:"api/audio_codecs/audio_codec_pair_id.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_h //original-code:"api/audio_codecs/audio_decoder.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_format_h //original-code:"api/audio_codecs/audio_format.h"

namespace webrtc {

// ILBC decoder API for use as a template parameter to
// CreateAudioDecoderFactory<...>().
//
// NOTE: This struct is still under development and may change without notice.
struct AudioDecoderIlbc {
  struct Config {};  // Empty---no config values needed!
  static absl::optional<Config> SdpToConfig(const SdpAudioFormat& audio_format);
  static void AppendSupportedDecoders(std::vector<AudioCodecSpec>* specs);
  static std::unique_ptr<AudioDecoder> MakeAudioDecoder(
      Config config,
      absl::optional<AudioCodecPairId> codec_pair_id = absl::nullopt);
};

}  // namespace webrtc

#endif  // API_AUDIO_CODECS_ILBC_AUDIO_DECODER_ILBC_H_
