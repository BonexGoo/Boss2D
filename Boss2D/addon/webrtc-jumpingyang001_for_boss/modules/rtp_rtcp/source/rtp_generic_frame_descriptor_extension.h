/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_RTP_RTCP_SOURCE_RTP_GENERIC_FRAME_DESCRIPTOR_EXTENSION_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_GENERIC_FRAME_DESCRIPTOR_EXTENSION_H_

#include <stddef.h>
#include <stdint.h>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_generic_frame_descriptor_h //original-code:"modules/rtp_rtcp/source/rtp_generic_frame_descriptor.h"

namespace webrtc {

class RtpGenericFrameDescriptorExtension {
 public:
  static constexpr RTPExtensionType kId = kRtpExtensionGenericFrameDescriptor;
  static constexpr char kUri[] =
      "http://www.webrtc.org/experiments/rtp-hdrext/generic-frame-descriptor";

  static bool Parse(rtc::ArrayView<const uint8_t> data,
                    RtpGenericFrameDescriptor* descriptor);
  static size_t ValueSize(const RtpGenericFrameDescriptor&);
  static bool Write(rtc::ArrayView<uint8_t> data,
                    const RtpGenericFrameDescriptor& descriptor);
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_RTP_GENERIC_FRAME_DESCRIPTOR_EXTENSION_H_
