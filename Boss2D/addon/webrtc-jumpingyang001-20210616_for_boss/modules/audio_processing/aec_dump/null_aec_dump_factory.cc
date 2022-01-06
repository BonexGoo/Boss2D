/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__aec_dump__aec_dump_factory_h //original-code:"modules/audio_processing/aec_dump/aec_dump_factory.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__aec_dump_h //original-code:"modules/audio_processing/include/aec_dump.h"

namespace webrtc {

std::unique_ptr<AecDump> AecDumpFactory::Create(webrtc::FileWrapper file,
                                                int64_t max_log_size_bytes,
                                                rtc::TaskQueue* worker_queue) {
  return nullptr;
}

std::unique_ptr<AecDump> AecDumpFactory::Create(std::string file_name,
                                                int64_t max_log_size_bytes,
                                                rtc::TaskQueue* worker_queue) {
  return nullptr;
}

std::unique_ptr<AecDump> AecDumpFactory::Create(FILE* handle,
                                                int64_t max_log_size_bytes,
                                                rtc::TaskQueue* worker_queue) {
  return nullptr;
}
}  // namespace webrtc
