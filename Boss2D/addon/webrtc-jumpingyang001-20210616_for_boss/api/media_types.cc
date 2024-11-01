/*
 *  Copyright 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__media_types_h //original-code:"api/media_types.h"

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace cricket {

const char kMediaTypeVideo[] = "video";
const char kMediaTypeAudio[] = "audio";
const char kMediaTypeData[] = "data";

std::string MediaTypeToString(MediaType type) {
  switch (type) {
    case MEDIA_TYPE_AUDIO:
      return kMediaTypeAudio;
    case MEDIA_TYPE_VIDEO:
      return kMediaTypeVideo;
    case MEDIA_TYPE_DATA:
      return kMediaTypeData;
    case MEDIA_TYPE_UNSUPPORTED:
      // Unsupported media stores the m=<mediatype> differently.
      RTC_NOTREACHED();
      return "";
  }
  RTC_CHECK_NOTREACHED();
}

}  // namespace cricket
