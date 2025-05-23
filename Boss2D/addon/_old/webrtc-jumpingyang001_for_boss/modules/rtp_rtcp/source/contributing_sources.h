/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_CONTRIBUTING_SOURCES_H_
#define MODULES_RTP_RTCP_SOURCE_CONTRIBUTING_SOURCES_H_

#include <stdint.h>

#include <map>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__rtpreceiverinterface_h //original-code:"api/rtpreceiverinterface.h"

namespace webrtc {

class ContributingSources {
 public:
  ContributingSources();
  ~ContributingSources();

  // TODO(bugs.webrtc.org/3333): Needs to be extended with audio-level, to
  // support RFC6465.
  void Update(int64_t now_ms, rtc::ArrayView<const uint32_t> csrcs);

  // Returns contributing sources seen the last 10 s.
  std::vector<RtpSource> GetSources(int64_t now_ms) const;

 private:
  void DeleteOldEntries(int64_t now_ms);

  // Indexed by csrc.
  std::map<uint32_t, int64_t> last_seen_ms_;
  absl::optional<int64_t> next_pruning_ms_;
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_CONTRIBUTING_SOURCES_H_
