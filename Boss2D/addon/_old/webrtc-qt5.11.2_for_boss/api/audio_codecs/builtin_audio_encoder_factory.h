/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_AUDIO_CODECS_BUILTIN_AUDIO_ENCODER_FACTORY_H_
#define API_AUDIO_CODECS_BUILTIN_AUDIO_ENCODER_FACTORY_H_

#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_factory_h //original-code:"api/audio_codecs/audio_encoder_factory.h"
#include BOSS_WEBRTC_U_rtc_base__scoped_ref_ptr_h //original-code:"rtc_base/scoped_ref_ptr.h"

namespace webrtc {

// Creates a new factory that can create the built-in types of audio encoders.
// NOTE: This function is still under development and may change without notice.
rtc::scoped_refptr<AudioEncoderFactory> CreateBuiltinAudioEncoderFactory();

}  // namespace webrtc

#endif  // API_AUDIO_CODECS_BUILTIN_AUDIO_ENCODER_FACTORY_H_
