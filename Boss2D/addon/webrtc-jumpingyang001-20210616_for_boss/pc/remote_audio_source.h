/*
 *  Copyright 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_REMOTE_AUDIO_SOURCE_H_
#define PC_REMOTE_AUDIO_SOURCE_H_

#include <stdint.h>

#include <list>
#include <string>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__call__audio_sink_h //original-code:"api/call/audio_sink.h"
#include BOSS_WEBRTC_U_api__media_stream_interface_h //original-code:"api/media_stream_interface.h"
#include BOSS_WEBRTC_U_api__notifier_h //original-code:"api/notifier.h"
#include BOSS_WEBRTC_U_media__base__media_channel_h //original-code:"media/base/media_channel.h"
#include BOSS_WEBRTC_U_pc__channel_h //original-code:"pc/channel.h"
#include BOSS_WEBRTC_U_rtc_base__message_handler_h //original-code:"rtc_base/message_handler.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__thread_message_h //original-code:"rtc_base/thread_message.h"

namespace rtc {
struct Message;
class Thread;
}  // namespace rtc

namespace webrtc {

// This class implements the audio source used by the remote audio track.
// This class works by configuring itself as a sink with the underlying media
// engine, then when receiving data will fan out to all added sinks.
class RemoteAudioSource : public Notifier<AudioSourceInterface>,
                          rtc::MessageHandler {
 public:
  // In Unified Plan, receivers map to m= sections and their tracks and sources
  // survive SSRCs being reconfigured. The life cycle of the remote audio source
  // is associated with the life cycle of the m= section, and thus even if an
  // audio channel is destroyed the RemoteAudioSource should kSurvive.
  //
  // In Plan B however, remote audio sources map 1:1 with an SSRCs and if an
  // audio channel is destroyed, the RemoteAudioSource should kEnd.
  enum class OnAudioChannelGoneAction {
    kSurvive,
    kEnd,
  };

  explicit RemoteAudioSource(
      rtc::Thread* worker_thread,
      OnAudioChannelGoneAction on_audio_channel_gone_action);

  // Register and unregister remote audio source with the underlying media
  // engine.
  void Start(cricket::VoiceMediaChannel* media_channel,
             absl::optional<uint32_t> ssrc);
  void Stop(cricket::VoiceMediaChannel* media_channel,
            absl::optional<uint32_t> ssrc);
  void SetState(SourceState new_state);

  // MediaSourceInterface implementation.
  MediaSourceInterface::SourceState state() const override;
  bool remote() const override;

  // AudioSourceInterface implementation.
  void SetVolume(double volume) override;
  void RegisterAudioObserver(AudioObserver* observer) override;
  void UnregisterAudioObserver(AudioObserver* observer) override;

  void AddSink(AudioTrackSinkInterface* sink) override;
  void RemoveSink(AudioTrackSinkInterface* sink) override;

 protected:
  ~RemoteAudioSource() override;

 private:
  // These are callbacks from the media engine.
  class AudioDataProxy;

  void OnData(const AudioSinkInterface::Data& audio);
  void OnAudioChannelGone();

  void OnMessage(rtc::Message* msg) override;

  rtc::Thread* const main_thread_;
  rtc::Thread* const worker_thread_;
  const OnAudioChannelGoneAction on_audio_channel_gone_action_;
  std::list<AudioObserver*> audio_observers_;
  Mutex sink_lock_;
  std::list<AudioTrackSinkInterface*> sinks_;
  SourceState state_;
};

}  // namespace webrtc

#endif  // PC_REMOTE_AUDIO_SOURCE_H_
