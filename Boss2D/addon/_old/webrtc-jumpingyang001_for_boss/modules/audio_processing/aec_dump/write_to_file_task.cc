/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__aec_dump__write_to_file_task_h //original-code:"modules/audio_processing/aec_dump/write_to_file_task.h"

#include BOSS_WEBRTC_U_rtc_base__protobuf_utils_h //original-code:"rtc_base/protobuf_utils.h"

namespace webrtc {

WriteToFileTask::WriteToFileTask(webrtc::FileWrapper* debug_file,
                                 int64_t* num_bytes_left_for_log)
    : debug_file_(debug_file),
      num_bytes_left_for_log_(num_bytes_left_for_log) {}

WriteToFileTask::~WriteToFileTask() = default;

audioproc::Event* WriteToFileTask::GetEvent() {
  return &event_;
}

bool WriteToFileTask::IsRoomForNextEvent(size_t event_byte_size) const {
  int64_t next_message_size = event_byte_size + sizeof(int32_t);
  return (*num_bytes_left_for_log_ < 0) ||
         (*num_bytes_left_for_log_ >= next_message_size);
}

void WriteToFileTask::UpdateBytesLeft(size_t event_byte_size) {
  RTC_DCHECK(IsRoomForNextEvent(event_byte_size));
  if (*num_bytes_left_for_log_ >= 0) {
    *num_bytes_left_for_log_ -= (sizeof(int32_t) + event_byte_size);
  }
}

bool WriteToFileTask::Run() {
  if (!debug_file_->is_open()) {
    return true;
  }

  ProtoString event_string;
  event_.SerializeToString(&event_string);

  const size_t event_byte_size = event_.ByteSizeLong();

  if (!IsRoomForNextEvent(event_byte_size)) {
    debug_file_->CloseFile();
    return true;
  }

  UpdateBytesLeft(event_byte_size);

  // Write message preceded by its size.
  if (!debug_file_->Write(&event_byte_size, sizeof(int32_t))) {
    RTC_NOTREACHED();
  }
  if (!debug_file_->Write(event_string.data(), event_string.length())) {
    RTC_NOTREACHED();
  }
  return true;  // Delete task from queue at once.
}

}  // namespace webrtc
