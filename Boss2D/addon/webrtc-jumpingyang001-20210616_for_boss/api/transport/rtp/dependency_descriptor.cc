/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__transport__rtp__dependency_descriptor_h //original-code:"api/transport/rtp/dependency_descriptor.h"

#include BOSS_ABSEILCPP_U_absl__container__inlined_vector_h //original-code:"absl/container/inlined_vector.h"
#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

constexpr int DependencyDescriptor::kMaxSpatialIds;
constexpr int DependencyDescriptor::kMaxTemporalIds;
constexpr int DependencyDescriptor::kMaxTemplates;
constexpr int DependencyDescriptor::kMaxDecodeTargets;

namespace webrtc_impl {

absl::InlinedVector<DecodeTargetIndication, 10> StringToDecodeTargetIndications(
    absl::string_view symbols) {
  absl::InlinedVector<DecodeTargetIndication, 10> dtis;
  dtis.reserve(symbols.size());
  for (char symbol : symbols) {
    DecodeTargetIndication indication;
    switch (symbol) {
      case '-':
        indication = DecodeTargetIndication::kNotPresent;
        break;
      case 'D':
        indication = DecodeTargetIndication::kDiscardable;
        break;
      case 'R':
        indication = DecodeTargetIndication::kRequired;
        break;
      case 'S':
        indication = DecodeTargetIndication::kSwitch;
        break;
      default:
        RTC_NOTREACHED();
    }
    dtis.push_back(indication);
  }
  return dtis;
}

}  // namespace webrtc_impl
}  // namespace webrtc
