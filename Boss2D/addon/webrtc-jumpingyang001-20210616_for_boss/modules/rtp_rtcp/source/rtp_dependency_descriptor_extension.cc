/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_dependency_descriptor_extension_h //original-code:"modules/rtp_rtcp/source/rtp_dependency_descriptor_extension.h"

#include <bitset>
#include <cstdint>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__transport__rtp__dependency_descriptor_h //original-code:"api/transport/rtp/dependency_descriptor.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_dependency_descriptor_reader_h //original-code:"modules/rtp_rtcp/source/rtp_dependency_descriptor_reader.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_dependency_descriptor_writer_h //original-code:"modules/rtp_rtcp/source/rtp_dependency_descriptor_writer.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__divide_round_h //original-code:"rtc_base/numerics/divide_round.h"

namespace webrtc {

constexpr RTPExtensionType RtpDependencyDescriptorExtension::kId;
constexpr char RtpDependencyDescriptorExtension::kUri[];
constexpr std::bitset<32> RtpDependencyDescriptorExtension::kAllChainsAreActive;

bool RtpDependencyDescriptorExtension::Parse(
    rtc::ArrayView<const uint8_t> data,
    const FrameDependencyStructure* structure,
    DependencyDescriptor* descriptor) {
  RtpDependencyDescriptorReader reader(data, structure, descriptor);
  return reader.ParseSuccessful();
}

size_t RtpDependencyDescriptorExtension::ValueSize(
    const FrameDependencyStructure& structure,
    std::bitset<32> active_chains,
    const DependencyDescriptor& descriptor) {
  RtpDependencyDescriptorWriter writer(/*data=*/{}, structure, active_chains,
                                       descriptor);
  return DivideRoundUp(writer.ValueSizeBits(), 8);
}

bool RtpDependencyDescriptorExtension::Write(
    rtc::ArrayView<uint8_t> data,
    const FrameDependencyStructure& structure,
    std::bitset<32> active_chains,
    const DependencyDescriptor& descriptor) {
  RtpDependencyDescriptorWriter writer(data, structure, active_chains,
                                       descriptor);
  return writer.Write();
}

}  // namespace webrtc
