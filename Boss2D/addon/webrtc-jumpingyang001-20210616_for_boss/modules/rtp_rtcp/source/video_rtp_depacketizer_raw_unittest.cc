/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_raw_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer_raw.h"

#include <cstdint>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {
namespace {

TEST(VideoRtpDepacketizerRaw, PassRtpPayloadAsVideoPayload) {
  const uint8_t kPayload[] = {0x05, 0x25, 0x52};
  rtc::CopyOnWriteBuffer rtp_payload(kPayload);

  VideoRtpDepacketizerRaw depacketizer;
  absl::optional<VideoRtpDepacketizer::ParsedRtpPayload> parsed =
      depacketizer.Parse(rtp_payload);

  ASSERT_TRUE(parsed);
  EXPECT_EQ(parsed->video_payload.size(), rtp_payload.size());
  // Check there was no memcpy involved by verifying return and original buffers
  // point to the same buffer.
  EXPECT_EQ(parsed->video_payload.cdata(), rtp_payload.cdata());
}

TEST(VideoRtpDepacketizerRaw, UsesDefaultValuesForVideoHeader) {
  const uint8_t kPayload[] = {0x05, 0x25, 0x52};
  rtc::CopyOnWriteBuffer rtp_payload(kPayload);

  VideoRtpDepacketizerRaw depacketizer;
  absl::optional<VideoRtpDepacketizer::ParsedRtpPayload> parsed =
      depacketizer.Parse(rtp_payload);

  ASSERT_TRUE(parsed);
  EXPECT_FALSE(parsed->video_header.generic);
  EXPECT_EQ(parsed->video_header.codec, kVideoCodecGeneric);
}

}  // namespace
}  // namespace webrtc
