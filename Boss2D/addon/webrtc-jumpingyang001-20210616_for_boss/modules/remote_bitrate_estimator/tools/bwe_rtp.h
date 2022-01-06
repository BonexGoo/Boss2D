/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_REMOTE_BITRATE_ESTIMATOR_TOOLS_BWE_RTP_H_
#define MODULES_REMOTE_BITRATE_ESTIMATOR_TOOLS_BWE_RTP_H_

#include <memory>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_header_extension_map_h //original-code:"modules/rtp_rtcp/include/rtp_header_extension_map.h"
#include BOSS_WEBRTC_U_test__rtp_file_reader_h //original-code:"test/rtp_file_reader.h"

bool ParseArgsAndSetupRtpReader(
    int argc,
    char** argv,
    std::unique_ptr<webrtc::test::RtpFileReader>& rtp_reader,
    webrtc::RtpHeaderExtensionMap& rtp_header_extensions);

#endif  // MODULES_REMOTE_BITRATE_ESTIMATOR_TOOLS_BWE_RTP_H_
