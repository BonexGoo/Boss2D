/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef CALL_AUDIO_STATE_H_
#define CALL_AUDIO_STATE_H_

#include BOSS_WEBRTC_U_api__audio__audio_mixer_h //original-code:"api/audio/audio_mixer.h"
#include BOSS_WEBRTC_U_rtc_base__refcount_h //original-code:"rtc_base/refcount.h"
#include BOSS_WEBRTC_U_rtc_base__scoped_ref_ptr_h //original-code:"rtc_base/scoped_ref_ptr.h"

namespace webrtc {

class AudioDeviceModule;
class AudioProcessing;
class AudioTransport;
class VoiceEngine;

// AudioState holds the state which must be shared between multiple instances of
// webrtc::Call for audio processing purposes.
class AudioState : public rtc::RefCountInterface {
 public:
  struct Config {
    // TODO(solenberg): Remove once clients don't use it anymore.
    VoiceEngine* voice_engine = nullptr;

    // The audio mixer connected to active receive streams. One per
    // AudioState.
    rtc::scoped_refptr<AudioMixer> audio_mixer;

    // The audio processing module.
    rtc::scoped_refptr<webrtc::AudioProcessing> audio_processing;

    // TODO(solenberg): Temporary: audio device module.
    rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module;
  };

  struct Stats {
    // Audio peak level (max(abs())), linearly on the interval [0,32767].
    int32_t audio_level = -1;
    // Audio peak level (max(abs())), logarithmically on the interval [0,9].
    int8_t quantized_audio_level = -1;
    // See: https://w3c.github.io/webrtc-stats/#dom-rtcmediastreamtrackstats-totalaudioenergy
    double total_energy = 0.0f;
    double total_duration = 0.0f;
  };

  virtual AudioProcessing* audio_processing() = 0;
  virtual AudioTransport* audio_transport() = 0;

  // Enable/disable playout of the audio channels. Enabled by default.
  // This will stop playout of the underlying audio device but start a task
  // which will poll for audio data every 10ms to ensure that audio processing
  // happens and the audio stats are updated.
  virtual void SetPlayout(bool enabled) = 0;

  // Enable/disable recording of the audio channels. Enabled by default.
  // This will stop recording of the underlying audio device and no audio
  // packets will be encoded or transmitted.
  virtual void SetRecording(bool enabled) = 0;

  virtual Stats GetAudioInputStats() const = 0;
  virtual void SetStereoChannelSwapping(bool enable) = 0;

  // TODO(solenberg): Replace scoped_refptr with shared_ptr once we can use it.
  static rtc::scoped_refptr<AudioState> Create(
      const AudioState::Config& config);

  virtual ~AudioState() {}
};
}  // namespace webrtc

#endif  // CALL_AUDIO_STATE_H_
