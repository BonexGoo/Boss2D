/*
 *  Copyright 2004 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__bundlefilter_h //original-code:"pc/bundlefilter.h"

#include BOSS_WEBRTC_U_media__base__rtputils_h //original-code:"media/base/rtputils.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace cricket {

BundleFilter::BundleFilter() {
}

BundleFilter::~BundleFilter() {
}

bool BundleFilter::DemuxPacket(const uint8_t* data, size_t len) {
  // For RTP packets, we check whether the payload type can be found.
  if (!IsRtpPacket(data, len)) {
    return false;
  }

  int payload_type = 0;
  if (!GetRtpPayloadType(data, len, &payload_type)) {
    return false;
  }
  return FindPayloadType(payload_type);
}

void BundleFilter::AddPayloadType(int payload_type) {
  payload_types_.insert(payload_type);
}

bool BundleFilter::FindPayloadType(int pl_type) const {
  return payload_types_.find(pl_type) != payload_types_.end();
}

void BundleFilter::ClearAllPayloadTypes() {
  payload_types_.clear();
}

}  // namespace cricket
