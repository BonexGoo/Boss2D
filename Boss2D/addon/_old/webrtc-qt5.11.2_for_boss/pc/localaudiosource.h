/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_LOCALAUDIOSOURCE_H_
#define PC_LOCALAUDIOSOURCE_H_

#include BOSS_WEBRTC_U_api__mediastreaminterface_h //original-code:"api/mediastreaminterface.h"
#include BOSS_WEBRTC_U_api__notifier_h //original-code:"api/notifier.h"
#include BOSS_WEBRTC_U_media__base__mediachannel_h //original-code:"media/base/mediachannel.h"

// LocalAudioSource implements AudioSourceInterface.
// This contains settings for switching audio processing on and off.

namespace webrtc {

class MediaConstraintsInterface;

class LocalAudioSource : public Notifier<AudioSourceInterface> {
 public:
  // Creates an instance of LocalAudioSource.
  static rtc::scoped_refptr<LocalAudioSource> Create(
      const MediaConstraintsInterface* constraints);

  static rtc::scoped_refptr<LocalAudioSource> Create(
      const cricket::AudioOptions* audio_options);

  SourceState state() const override { return kLive; }
  bool remote() const override { return false; }

  virtual const cricket::AudioOptions& options() const { return options_; }

  void AddSink(AudioTrackSinkInterface* sink) override {}
  void RemoveSink(AudioTrackSinkInterface* sink) override {}

 protected:
  LocalAudioSource() {}
  ~LocalAudioSource() override {}

 private:
  void Initialize(const MediaConstraintsInterface* constraints);
  void Initialize(const cricket::AudioOptions* audio_options);

  cricket::AudioOptions options_;
};

}  // namespace webrtc

#endif  // PC_LOCALAUDIOSOURCE_H_
