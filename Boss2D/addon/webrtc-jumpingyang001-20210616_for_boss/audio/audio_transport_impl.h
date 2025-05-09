/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef AUDIO_AUDIO_TRANSPORT_IMPL_H_
#define AUDIO_AUDIO_TRANSPORT_IMPL_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__audio__audio_mixer_h //original-code:"api/audio/audio_mixer.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_common_audio__resampler__include__push_resampler_h //original-code:"common_audio/resampler/include/push_resampler.h"
#include BOSS_WEBRTC_U_modules__async_audio_processing__async_audio_processing_h //original-code:"modules/async_audio_processing/async_audio_processing.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__audio_device_h //original-code:"modules/audio_device/include/audio_device.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_modules__audio_processing__typing_detection_h //original-code:"modules/audio_processing/typing_detection.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class AudioSender;

class AudioTransportImpl : public AudioTransport {
 public:
  AudioTransportImpl(
      AudioMixer* mixer,
      AudioProcessing* audio_processing,
      AsyncAudioProcessing::Factory* async_audio_processing_factory);

  AudioTransportImpl() = delete;
  AudioTransportImpl(const AudioTransportImpl&) = delete;
  AudioTransportImpl& operator=(const AudioTransportImpl&) = delete;

  ~AudioTransportImpl() override;

  int32_t RecordedDataIsAvailable(const void* audioSamples,
                                  const size_t nSamples,
                                  const size_t nBytesPerSample,
                                  const size_t nChannels,
                                  const uint32_t samplesPerSec,
                                  const uint32_t totalDelayMS,
                                  const int32_t clockDrift,
                                  const uint32_t currentMicLevel,
                                  const bool keyPressed,
                                  uint32_t& newMicLevel) override;

  int32_t NeedMorePlayData(const size_t nSamples,
                           const size_t nBytesPerSample,
                           const size_t nChannels,
                           const uint32_t samplesPerSec,
                           void* audioSamples,
                           size_t& nSamplesOut,
                           int64_t* elapsed_time_ms,
                           int64_t* ntp_time_ms) override;

  void PullRenderData(int bits_per_sample,
                      int sample_rate,
                      size_t number_of_channels,
                      size_t number_of_frames,
                      void* audio_data,
                      int64_t* elapsed_time_ms,
                      int64_t* ntp_time_ms) override;

  void UpdateAudioSenders(std::vector<AudioSender*> senders,
                          int send_sample_rate_hz,
                          size_t send_num_channels);
  void SetStereoChannelSwapping(bool enable);
  bool typing_noise_detected() const;

 private:
  void SendProcessedData(std::unique_ptr<AudioFrame> audio_frame);

  // Shared.
  AudioProcessing* audio_processing_ = nullptr;

  // Capture side.

  // Thread-safe.
  const std::unique_ptr<AsyncAudioProcessing> async_audio_processing_;

  mutable Mutex capture_lock_;
  std::vector<AudioSender*> audio_senders_ RTC_GUARDED_BY(capture_lock_);
  int send_sample_rate_hz_ RTC_GUARDED_BY(capture_lock_) = 8000;
  size_t send_num_channels_ RTC_GUARDED_BY(capture_lock_) = 1;
  bool typing_noise_detected_ RTC_GUARDED_BY(capture_lock_) = false;
  bool swap_stereo_channels_ RTC_GUARDED_BY(capture_lock_) = false;
  PushResampler<int16_t> capture_resampler_;
  TypingDetection typing_detection_;

  // Render side.

  rtc::scoped_refptr<AudioMixer> mixer_;
  AudioFrame mixed_frame_;
  // Converts mixed audio to the audio device output rate.
  PushResampler<int16_t> render_resampler_;
};
}  // namespace webrtc

#endif  // AUDIO_AUDIO_TRANSPORT_IMPL_H_
