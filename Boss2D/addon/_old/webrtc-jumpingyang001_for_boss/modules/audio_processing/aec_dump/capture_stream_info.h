/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC_DUMP_CAPTURE_STREAM_INFO_H_
#define MODULES_AUDIO_PROCESSING_AEC_DUMP_CAPTURE_STREAM_INFO_H_

#include <memory>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_api__audio__audio_frame_h //original-code:"api/audio/audio_frame.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec_dump__write_to_file_task_h //original-code:"modules/audio_processing/aec_dump/write_to_file_task.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__aec_dump_h //original-code:"modules/audio_processing/include/aec_dump.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__ignore_wundef_h //original-code:"rtc_base/ignore_wundef.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

// Files generated at build-time by the protobuf compiler.
RTC_PUSH_IGNORING_WUNDEF()
#ifdef WEBRTC_ANDROID_PLATFORM_BUILD
#include "external/webrtc/webrtc/modules/audio_processing/debug.pb.h"
#else
#include "modules/audio_processing/debug.pb.h"
#endif
RTC_POP_IGNORING_WUNDEF()

namespace webrtc {

class CaptureStreamInfo {
 public:
  explicit CaptureStreamInfo(std::unique_ptr<WriteToFileTask> task);
  ~CaptureStreamInfo();
  void AddInput(const AudioFrameView<const float>& src);
  void AddOutput(const AudioFrameView<const float>& src);

  void AddInput(const AudioFrame& frame);
  void AddOutput(const AudioFrame& frame);

  void AddAudioProcessingState(const AecDump::AudioProcessingState& state);

  std::unique_ptr<WriteToFileTask> GetTask() {
    RTC_DCHECK(task_);
    return std::move(task_);
  }

  void SetTask(std::unique_ptr<WriteToFileTask> task) {
    RTC_DCHECK(!task_);
    RTC_DCHECK(task);
    task_ = std::move(task);
    task_->GetEvent()->set_type(audioproc::Event::STREAM);
  }

 private:
  std::unique_ptr<WriteToFileTask> task_;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC_DUMP_CAPTURE_STREAM_INFO_H_
