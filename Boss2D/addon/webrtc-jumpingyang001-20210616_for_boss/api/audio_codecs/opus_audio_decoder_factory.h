/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_AUDIO_CODECS_OPUS_AUDIO_DECODER_FACTORY_H_
#define API_AUDIO_CODECS_OPUS_AUDIO_DECODER_FACTORY_H_

#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_factory_h //original-code:"api/audio_codecs/audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"

namespace webrtc {

// Creates a new factory that can create only Opus audio decoders. Works like
// CreateAudioDecoderFactory<AudioDecoderOpus>(), but is easier to use and is
// not inline because it isn't a template.
rtc::scoped_refptr<AudioDecoderFactory> CreateOpusAudioDecoderFactory();

}  // namespace webrtc

#endif  // API_AUDIO_CODECS_OPUS_AUDIO_DECODER_FACTORY_H_
