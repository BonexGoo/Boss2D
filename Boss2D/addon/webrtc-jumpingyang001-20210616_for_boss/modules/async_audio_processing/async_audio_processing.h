/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_ASYNC_AUDIO_PROCESSING_ASYNC_AUDIO_PROCESSING_H_
#define MODULES_ASYNC_AUDIO_PROCESSING_ASYNC_AUDIO_PROCESSING_H_

#include <memory>

#include BOSS_WEBRTC_U_api__audio__audio_frame_processor_h //original-code:"api/audio/audio_frame_processor.h"
#include BOSS_WEBRTC_U_rtc_base__ref_count_h //original-code:"rtc_base/ref_count.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"

namespace webrtc {

class AudioFrame;
class TaskQueueFactory;

// Helper class taking care of interactions with AudioFrameProcessor
// in asynchronous manner. Offloads AudioFrameProcessor::Process calls
// to a dedicated task queue. Makes sure that it's always safe for
// AudioFrameProcessor to pass processed frames back to its sink.
class AsyncAudioProcessing final {
 public:
  // Helper class passing AudioFrameProcessor and TaskQueueFactory into
  // AsyncAudioProcessing constructor.
  class Factory : public rtc::RefCountInterface {
   public:
    Factory(const Factory&) = delete;
    Factory& operator=(const Factory&) = delete;

    ~Factory();
    Factory(AudioFrameProcessor& frame_processor,
            TaskQueueFactory& task_queue_factory);

    std::unique_ptr<AsyncAudioProcessing> CreateAsyncAudioProcessing(
        AudioFrameProcessor::OnAudioFrameCallback on_frame_processed_callback);

   private:
    AudioFrameProcessor& frame_processor_;
    TaskQueueFactory& task_queue_factory_;
  };

  AsyncAudioProcessing(const AsyncAudioProcessing&) = delete;
  AsyncAudioProcessing& operator=(const AsyncAudioProcessing&) = delete;

  ~AsyncAudioProcessing();

  // Creates AsyncAudioProcessing which will pass audio frames to
  // |frame_processor| on |task_queue_| and reply with processed frames passed
  // into |on_frame_processed_callback|, which is posted back onto
  // |task_queue_|. |task_queue_| is created using the provided
  // |task_queue_factory|.
  AsyncAudioProcessing(
      AudioFrameProcessor& frame_processor,
      TaskQueueFactory& task_queue_factory,
      AudioFrameProcessor::OnAudioFrameCallback on_frame_processed_callback);

  // Accepts |frame| for asynchronous processing. Thread-safe.
  void Process(std::unique_ptr<AudioFrame> frame);

 private:
  AudioFrameProcessor::OnAudioFrameCallback on_frame_processed_callback_;
  AudioFrameProcessor& frame_processor_;
  rtc::TaskQueue task_queue_;
};

}  // namespace webrtc

#endif  // MODULES_ASYNC_AUDIO_PROCESSING_ASYNC_AUDIO_PROCESSING_H_
