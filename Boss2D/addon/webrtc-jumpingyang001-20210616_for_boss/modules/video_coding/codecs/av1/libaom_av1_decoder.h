/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_VIDEO_CODING_CODECS_AV1_LIBAOM_AV1_DECODER_H_
#define MODULES_VIDEO_CODING_CODECS_AV1_LIBAOM_AV1_DECODER_H_

#include <memory>

#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"

namespace webrtc {

ABSL_CONST_INIT extern const bool kIsLibaomAv1DecoderSupported;

std::unique_ptr<VideoDecoder> CreateLibaomAv1Decoder();

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_AV1_LIBAOM_AV1_DECODER_H_
