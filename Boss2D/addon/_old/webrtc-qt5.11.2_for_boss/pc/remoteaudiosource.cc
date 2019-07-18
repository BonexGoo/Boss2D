/*
 *  Copyright 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__remoteaudiosource_h //original-code:"pc/remoteaudiosource.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__ptr_util_h //original-code:"rtc_base/ptr_util.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"

namespace webrtc {

class RemoteAudioSource::Sink : public AudioSinkInterface {
 public:
  explicit Sink(RemoteAudioSource* source) : source_(source) {}
  ~Sink() override { source_->OnAudioChannelGone(); }

 private:
  void OnData(const AudioSinkInterface::Data& audio) override {
    if (source_)
      source_->OnData(audio);
  }

  const rtc::scoped_refptr<RemoteAudioSource> source_;
  RTC_DISALLOW_IMPLICIT_CONSTRUCTORS(Sink);
};

rtc::scoped_refptr<RemoteAudioSource> RemoteAudioSource::Create(
    rtc::Thread* worker_thread,
    cricket::VoiceMediaChannel* media_channel,
    uint32_t ssrc) {
  rtc::scoped_refptr<RemoteAudioSource> ret(
      new rtc::RefCountedObject<RemoteAudioSource>());
  ret->Initialize(worker_thread, media_channel, ssrc);
  return ret;
}

RemoteAudioSource::RemoteAudioSource()
    : main_thread_(rtc::Thread::Current()),
      state_(MediaSourceInterface::kLive) {
  RTC_DCHECK(main_thread_);
}

RemoteAudioSource::~RemoteAudioSource() {
  RTC_DCHECK(main_thread_->IsCurrent());
  RTC_DCHECK(audio_observers_.empty());
  RTC_DCHECK(sinks_.empty());
}

void RemoteAudioSource::Initialize(rtc::Thread* worker_thread,
                                   cricket::VoiceMediaChannel* media_channel,
                                   uint32_t ssrc) {
  RTC_DCHECK(main_thread_->IsCurrent());
  // To make sure we always get notified when the channel goes out of scope,
  // we register for callbacks here and not on demand in AddSink.
  if (media_channel) {  // May be null in tests.
    worker_thread->Invoke<void>(RTC_FROM_HERE, [&] {
      media_channel->SetRawAudioSink(ssrc, rtc::MakeUnique<Sink>(this));
    });
  }
}

MediaSourceInterface::SourceState RemoteAudioSource::state() const {
  RTC_DCHECK(main_thread_->IsCurrent());
  return state_;
}

bool RemoteAudioSource::remote() const {
  RTC_DCHECK(main_thread_->IsCurrent());
  return true;
}

void RemoteAudioSource::SetVolume(double volume) {
  RTC_DCHECK_GE(volume, 0);
  RTC_DCHECK_LE(volume, 10);
  for (auto* observer : audio_observers_)
    observer->OnSetVolume(volume);
}

void RemoteAudioSource::RegisterAudioObserver(AudioObserver* observer) {
  RTC_DCHECK(observer != NULL);
  RTC_DCHECK(std::find(audio_observers_.begin(), audio_observers_.end(),
                       observer) == audio_observers_.end());
  audio_observers_.push_back(observer);
}

void RemoteAudioSource::UnregisterAudioObserver(AudioObserver* observer) {
  RTC_DCHECK(observer != NULL);
  audio_observers_.remove(observer);
}

void RemoteAudioSource::AddSink(AudioTrackSinkInterface* sink) {
  RTC_DCHECK(main_thread_->IsCurrent());
  RTC_DCHECK(sink);

  if (state_ != MediaSourceInterface::kLive) {
    RTC_LOG(LS_ERROR) << "Can't register sink as the source isn't live.";
    return;
  }

  rtc::CritScope lock(&sink_lock_);
  RTC_DCHECK(std::find(sinks_.begin(), sinks_.end(), sink) == sinks_.end());
  sinks_.push_back(sink);
}

void RemoteAudioSource::RemoveSink(AudioTrackSinkInterface* sink) {
  RTC_DCHECK(main_thread_->IsCurrent());
  RTC_DCHECK(sink);

  rtc::CritScope lock(&sink_lock_);
  sinks_.remove(sink);
}

void RemoteAudioSource::OnData(const AudioSinkInterface::Data& audio) {
  // Called on the externally-owned audio callback thread, via/from webrtc.
  rtc::CritScope lock(&sink_lock_);
  for (auto* sink : sinks_) {
    sink->OnData(audio.data, 16, audio.sample_rate, audio.channels,
                 audio.samples_per_channel);
  }
}

void RemoteAudioSource::OnAudioChannelGone() {
  // Called when the audio channel is deleted.  It may be the worker thread
  // in libjingle or may be a different worker thread.
  // This object needs to live long enough for the cleanup logic in OnMessage to
  // run, so take a reference to it as the data. Sometimes the message may not
  // be processed (because the thread was destroyed shortly after this call),
  // but that is fine because the thread destructor will take care of destroying
  // the message data which will release the reference on RemoteAudioSource.
  main_thread_->Post(RTC_FROM_HERE, this, 0,
                     new rtc::ScopedRefMessageData<RemoteAudioSource>(this));
}

void RemoteAudioSource::OnMessage(rtc::Message* msg) {
  RTC_DCHECK(main_thread_->IsCurrent());
  sinks_.clear();
  state_ = MediaSourceInterface::kEnded;
  FireOnChanged();
  // Will possibly delete this RemoteAudioSource since it is reference counted
  // in the message.
  delete msg->pdata;
}

}  // namespace webrtc
