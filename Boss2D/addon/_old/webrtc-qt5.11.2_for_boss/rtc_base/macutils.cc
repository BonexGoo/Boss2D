/*
 *  Copyright 2007 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <cstring>
#include <memory>
#include <sstream>

#include <sys/utsname.h>

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__macutils_h //original-code:"rtc_base/macutils.h"
#include BOSS_WEBRTC_U_rtc_base__stringutils_h //original-code:"rtc_base/stringutils.h"

namespace rtc {

bool ToUtf8(const CFStringRef str16, std::string* str8) {
  if ((nullptr == str16) || (nullptr == str8)) {
    return false;
  }
  size_t maxlen = CFStringGetMaximumSizeForEncoding(CFStringGetLength(str16),
                                                    kCFStringEncodingUTF8) + 1;
  std::unique_ptr<char[]> buffer(new char[maxlen]);
  if (!buffer || !CFStringGetCString(str16, buffer.get(), maxlen,
                                     kCFStringEncodingUTF8)) {
    return false;
  }
  str8->assign(buffer.get());
  return true;
}

bool ToUtf16(const std::string& str8, CFStringRef* str16) {
  if (nullptr == str16) {
    return false;
  }
  *str16 = CFStringCreateWithBytes(kCFAllocatorDefault,
                                   reinterpret_cast<const UInt8*>(str8.data()),
                                   str8.length(), kCFStringEncodingUTF8,
                                   false);
  return nullptr != *str16;
}
}  // namespace rtc
