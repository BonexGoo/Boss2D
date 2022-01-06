/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_ANDROID_SRC_JNI_PC_AUDIO_H_
#define SDK_ANDROID_SRC_JNI_PC_AUDIO_H_

// Adding 'nogncheck' to disable the gn include headers check.
// We don't want this target depend on audio related targets
#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_factory_h //original-code:"api/audio_codecs/audio_decoder_factory.h"             // nogncheck
#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_factory_h //original-code:"api/audio_codecs/audio_encoder_factory.h"             // nogncheck
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"  // nogncheck
#include BOSS_WEBRTC_U_rtc_base__scoped_ref_ptr_h //original-code:"rtc_base/scoped_ref_ptr.h"

namespace webrtc {
namespace jni {

rtc::scoped_refptr<AudioDecoderFactory> CreateAudioDecoderFactory();

rtc::scoped_refptr<AudioEncoderFactory> CreateAudioEncoderFactory();

rtc::scoped_refptr<AudioProcessing> CreateAudioProcessing();

}  // namespace jni
}  // namespace webrtc

#endif  // SDK_ANDROID_SRC_JNI_PC_AUDIO_H_
