/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC_DUMP_AEC_DUMP_IMPL_H_
#define MODULES_AUDIO_PROCESSING_AEC_DUMP_AEC_DUMP_IMPL_H_

#include <memory>
#include <string>
#include <vector>

#include BOSS_WEBRTC_U_modules__audio_processing__aec_dump__capture_stream_info_h //original-code:"modules/audio_processing/aec_dump/capture_stream_info.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec_dump__write_to_file_task_h //original-code:"modules/audio_processing/aec_dump/write_to_file_task.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__aec_dump_h //original-code:"modules/audio_processing/include/aec_dump.h"
#include BOSS_WEBRTC_U_rtc_base__ignore_wundef_h //original-code:"rtc_base/ignore_wundef.h"
#include BOSS_WEBRTC_U_rtc_base__race_checker_h //original-code:"rtc_base/race_checker.h"
#include BOSS_WEBRTC_U_rtc_base__system__file_wrapper_h //original-code:"rtc_base/system/file_wrapper.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

// Files generated at build-time by the protobuf compiler.
RTC_PUSH_IGNORING_WUNDEF()
#ifdef WEBRTC_ANDROID_PLATFORM_BUILD
#include "external/webrtc/webrtc/modules/audio_processing/debug.pb.h"
#else
#include "modules/audio_processing/debug.pb.h"
#endif
RTC_POP_IGNORING_WUNDEF()

namespace rtc {
class TaskQueue;
}  // namespace rtc

namespace webrtc {

// Task-queue based implementation of AecDump. It is thread safe by
// relying on locks in TaskQueue.
class AecDumpImpl : public AecDump {
 public:
  // Does member variables initialization shared across all c-tors.
  AecDumpImpl(FileWrapper debug_file,
              int64_t max_log_size_bytes,
              rtc::TaskQueue* worker_queue);

  ~AecDumpImpl() override;

  void WriteInitMessage(const ProcessingConfig& api_format,
                        int64_t time_now_ms) override;
  void AddCaptureStreamInput(const AudioFrameView<const float>& src) override;
  void AddCaptureStreamOutput(const AudioFrameView<const float>& src) override;
  void AddCaptureStreamInput(const int16_t* const data,
                             int num_channels,
                             int samples_per_channel) override;
  void AddCaptureStreamOutput(const int16_t* const data,
                              int num_channels,
                              int samples_per_channel) override;
  void AddAudioProcessingState(const AudioProcessingState& state) override;
  void WriteCaptureStreamMessage() override;

  void WriteRenderStreamMessage(const int16_t* const data,
                                int num_channels,
                                int samples_per_channel) override;
  void WriteRenderStreamMessage(
      const AudioFrameView<const float>& src) override;

  void WriteConfig(const InternalAPMConfig& config) override;

  void WriteRuntimeSetting(
      const AudioProcessing::RuntimeSetting& runtime_setting) override;

 private:
  std::unique_ptr<WriteToFileTask> CreateWriteToFileTask();

  FileWrapper debug_file_;
  int64_t num_bytes_left_for_log_ = 0;
  rtc::RaceChecker race_checker_;
  rtc::TaskQueue* worker_queue_;
  CaptureStreamInfo capture_stream_info_;
};
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC_DUMP_AEC_DUMP_IMPL_H_
