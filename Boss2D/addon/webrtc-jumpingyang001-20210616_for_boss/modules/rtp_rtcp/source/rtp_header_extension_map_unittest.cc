/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_header_extension_map_h //original-code:"modules/rtp_rtcp/include/rtp_header_extension_map.h"

#include <vector>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_header_extensions_h //original-code:"modules/rtp_rtcp/source/rtp_header_extensions.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

TEST(RtpHeaderExtensionTest, RegisterByType) {
  RtpHeaderExtensionMap map;
  EXPECT_FALSE(map.IsRegistered(TransmissionOffset::kId));

  EXPECT_TRUE(map.RegisterByType(3, TransmissionOffset::kId));

  EXPECT_TRUE(map.IsRegistered(TransmissionOffset::kId));
  EXPECT_EQ(3, map.GetId(TransmissionOffset::kId));
  EXPECT_EQ(TransmissionOffset::kId, map.GetType(3));
}

TEST(RtpHeaderExtensionTest, RegisterByUri) {
  RtpHeaderExtensionMap map;

  EXPECT_TRUE(map.RegisterByUri(3, TransmissionOffset::kUri));

  EXPECT_TRUE(map.IsRegistered(TransmissionOffset::kId));
  EXPECT_EQ(3, map.GetId(TransmissionOffset::kId));
  EXPECT_EQ(TransmissionOffset::kId, map.GetType(3));
}

TEST(RtpHeaderExtensionTest, RegisterWithTrait) {
  RtpHeaderExtensionMap map;

  EXPECT_TRUE(map.Register<TransmissionOffset>(3));

  EXPECT_TRUE(map.IsRegistered(TransmissionOffset::kId));
  EXPECT_EQ(3, map.GetId(TransmissionOffset::kId));
  EXPECT_EQ(TransmissionOffset::kId, map.GetType(3));
}

TEST(RtpHeaderExtensionTest, RegisterDuringContruction) {
  const std::vector<RtpExtension> config = {{TransmissionOffset::kUri, 1},
                                            {AbsoluteSendTime::kUri, 3}};
  const RtpHeaderExtensionMap map(config);

  EXPECT_EQ(1, map.GetId(TransmissionOffset::kId));
  EXPECT_EQ(3, map.GetId(AbsoluteSendTime::kId));
}

TEST(RtpHeaderExtensionTest, RegisterTwoByteHeaderExtensions) {
  RtpHeaderExtensionMap map;
  // Two-byte header extension needed for id: [15-255].
  EXPECT_TRUE(map.Register<TransmissionOffset>(18));
  EXPECT_TRUE(map.Register<AbsoluteSendTime>(255));
}

TEST(RtpHeaderExtensionTest, RegisterIllegalArg) {
  RtpHeaderExtensionMap map;
  // Valid range for id: [1-255].
  EXPECT_FALSE(map.Register<TransmissionOffset>(0));
  EXPECT_FALSE(map.Register<TransmissionOffset>(256));
}

TEST(RtpHeaderExtensionTest, Idempotent) {
  RtpHeaderExtensionMap map;

  EXPECT_TRUE(map.Register<TransmissionOffset>(3));
  EXPECT_TRUE(map.Register<TransmissionOffset>(3));

  map.Deregister(TransmissionOffset::kId);
  map.Deregister(TransmissionOffset::kId);
}

TEST(RtpHeaderExtensionTest, NonUniqueId) {
  RtpHeaderExtensionMap map;
  EXPECT_TRUE(map.Register<TransmissionOffset>(3));

  EXPECT_FALSE(map.Register<AudioLevel>(3));
  EXPECT_TRUE(map.Register<AudioLevel>(4));
}

TEST(RtpHeaderExtensionTest, GetType) {
  RtpHeaderExtensionMap map;
  EXPECT_EQ(RtpHeaderExtensionMap::kInvalidType, map.GetType(3));
  EXPECT_TRUE(map.Register<TransmissionOffset>(3));

  EXPECT_EQ(TransmissionOffset::kId, map.GetType(3));
}

TEST(RtpHeaderExtensionTest, GetId) {
  RtpHeaderExtensionMap map;
  EXPECT_EQ(RtpHeaderExtensionMap::kInvalidId,
            map.GetId(TransmissionOffset::kId));
  EXPECT_TRUE(map.Register<TransmissionOffset>(3));

  EXPECT_EQ(3, map.GetId(TransmissionOffset::kId));
}

}  // namespace webrtc
