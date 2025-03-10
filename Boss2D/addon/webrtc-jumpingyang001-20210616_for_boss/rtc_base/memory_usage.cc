/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__memory_usage_h //original-code:"rtc_base/memory_usage.h"

#if defined(WEBRTC_LINUX)
#include <unistd.h>

#include <cstdio>
#elif defined(WEBRTC_MAC)
#include <mach/mach.h>
#elif defined(WEBRTC_WIN)
// clang-format off
// clang formating would change include order.
#include <windows.h>
#include <psapi.h>  // must come after windows.h
// clang-format on
#endif

#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace rtc {

int64_t GetProcessResidentSizeBytes() {
#if defined(WEBRTC_LINUX)
  FILE* file = fopen("/proc/self/statm", "r");
  if (file == nullptr) {
    RTC_LOG(LS_ERROR) << "Failed to open /proc/self/statm";
    return -1;
  }
  int result = -1;
  if (fscanf(file, "%*s%d", &result) != 1) {
    fclose(file);
    RTC_LOG(LS_ERROR) << "Failed to parse /proc/self/statm";
    return -1;
  }
  fclose(file);
  return static_cast<int64_t>(result) * sysconf(_SC_PAGESIZE);
#elif defined(WEBRTC_MAC)
  task_basic_info_64 info;
  mach_msg_type_number_t info_count = TASK_BASIC_INFO_64_COUNT;
  if (task_info(mach_task_self(), TASK_BASIC_INFO_64,
                reinterpret_cast<task_info_t>(&info),
                &info_count) != KERN_SUCCESS) {
    RTC_LOG_ERR(LS_ERROR) << "task_info() failed";
    return -1;
  }
  return info.resident_size;
#elif defined(WEBRTC_WIN)
  PROCESS_MEMORY_COUNTERS pmc;
  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)) == 0) {
    RTC_LOG_ERR(LS_ERROR) << "GetProcessMemoryInfo() failed";
    return -1;
  }
  return pmc.WorkingSetSize;
#elif defined(WEBRTC_FUCHSIA)
  RTC_LOG_ERR(LS_ERROR) << "GetProcessResidentSizeBytes() not implemented";
  return 0;
#else
  // Not implemented yet.
  static_assert(false,
                "GetProcessVirtualMemoryUsageBytes() platform support not yet "
                "implemented.");
#endif
}

}  // namespace rtc
