/*
 *  Copyright 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__audiotrack_h //original-code:"pc/audiotrack.h"

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__refcountedobject_h //original-code:"rtc_base/refcountedobject.h"

namespace webrtc {

// static
rtc::scoped_refptr<AudioTrack> AudioTrack::Create(
    const std::string& id,
    const rtc::scoped_refptr<AudioSourceInterface>& source) {
  return new rtc::RefCountedObject<AudioTrack>(id, source);
}

AudioTrack::AudioTrack(const std::string& label,
                       const rtc::scoped_refptr<AudioSourceInterface>& source)
    : MediaStreamTrack<AudioTrackInterface>(label), audio_source_(source) {
  if (audio_source_) {
    audio_source_->RegisterObserver(this);
    OnChanged();
  }
}

AudioTrack::~AudioTrack() {
  RTC_DCHECK(thread_checker_.CalledOnValidThread());
  set_state(MediaStreamTrackInterface::kEnded);
  if (audio_source_)
    audio_source_->UnregisterObserver(this);
}

std::string AudioTrack::kind() const {
  RTC_DCHECK(thread_checker_.CalledOnValidThread());
  return kAudioKind;
}

AudioSourceInterface* AudioTrack::GetSource() const {
  RTC_DCHECK(thread_checker_.CalledOnValidThread());
  return audio_source_.get();
}

void AudioTrack::AddSink(AudioTrackSinkInterface* sink) {
  RTC_DCHECK(thread_checker_.CalledOnValidThread());
  if (audio_source_)
    audio_source_->AddSink(sink);
}

void AudioTrack::RemoveSink(AudioTrackSinkInterface* sink) {
  RTC_DCHECK(thread_checker_.CalledOnValidThread());
  if (audio_source_)
    audio_source_->RemoveSink(sink);
}

void AudioTrack::OnChanged() {
  RTC_DCHECK(thread_checker_.CalledOnValidThread());
  if (audio_source_->state() == MediaSourceInterface::kEnded) {
    set_state(kEnded);
  } else {
    set_state(kLive);
  }
}

}  // namespace webrtc
