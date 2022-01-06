/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__compound_packet_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/compound_packet.h"

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet_h //original-code:"modules/rtp_rtcp/source/rtcp_packet.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__bye_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/bye.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__fir_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/fir.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__receiver_report_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/receiver_report.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__sender_report_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/sender_report.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_test__rtcp_packet_parser_h //original-code:"test/rtcp_packet_parser.h"

using ::testing::_;
using ::testing::Invoke;
using ::testing::MockFunction;
using webrtc::rtcp::Bye;
using webrtc::rtcp::CompoundPacket;
using webrtc::rtcp::Fir;
using webrtc::rtcp::ReceiverReport;
using webrtc::rtcp::ReportBlock;
using webrtc::rtcp::SenderReport;
using webrtc::test::RtcpPacketParser;

namespace webrtc {

const uint32_t kSenderSsrc = 0x12345678;
const uint32_t kRemoteSsrc = 0x23456789;
const uint8_t kSeqNo = 13;

TEST(RtcpCompoundPacketTest, AppendPacket) {
  CompoundPacket compound;
  auto fir = std::make_unique<Fir>();
  fir->AddRequestTo(kRemoteSsrc, kSeqNo);
  ReportBlock rb;
  auto rr = std::make_unique<ReceiverReport>();
  rr->SetSenderSsrc(kSenderSsrc);
  EXPECT_TRUE(rr->AddReportBlock(rb));
  compound.Append(std::move(rr));
  compound.Append(std::move(fir));

  rtc::Buffer packet = compound.Build();
  RtcpPacketParser parser;
  parser.Parse(packet.data(), packet.size());
  EXPECT_EQ(1, parser.receiver_report()->num_packets());
  EXPECT_EQ(kSenderSsrc, parser.receiver_report()->sender_ssrc());
  EXPECT_EQ(1u, parser.receiver_report()->report_blocks().size());
  EXPECT_EQ(1, parser.fir()->num_packets());
}

TEST(RtcpCompoundPacketTest, AppendPacketWithOwnAppendedPacket) {
  CompoundPacket root;
  auto leaf = std::make_unique<CompoundPacket>();

  auto fir = std::make_unique<Fir>();
  fir->AddRequestTo(kRemoteSsrc, kSeqNo);
  auto bye = std::make_unique<Bye>();
  ReportBlock rb;

  auto rr = std::make_unique<ReceiverReport>();
  EXPECT_TRUE(rr->AddReportBlock(rb));
  leaf->Append(std::move(rr));
  leaf->Append(std::move(fir));

  auto sr = std::make_unique<SenderReport>();
  root.Append(std::move(sr));
  root.Append(std::move(bye));
  root.Append(std::move(leaf));

  rtc::Buffer packet = root.Build();
  RtcpPacketParser parser;
  parser.Parse(packet.data(), packet.size());
  EXPECT_EQ(1, parser.sender_report()->num_packets());
  EXPECT_EQ(1, parser.receiver_report()->num_packets());
  EXPECT_EQ(1u, parser.receiver_report()->report_blocks().size());
  EXPECT_EQ(1, parser.bye()->num_packets());
  EXPECT_EQ(1, parser.fir()->num_packets());
}

TEST(RtcpCompoundPacketTest, BuildWithInputBuffer) {
  CompoundPacket compound;
  auto fir = std::make_unique<Fir>();
  fir->AddRequestTo(kRemoteSsrc, kSeqNo);
  ReportBlock rb;
  auto rr = std::make_unique<ReceiverReport>();
  rr->SetSenderSsrc(kSenderSsrc);
  EXPECT_TRUE(rr->AddReportBlock(rb));
  compound.Append(std::move(rr));
  compound.Append(std::move(fir));

  const size_t kRrLength = 8;
  const size_t kReportBlockLength = 24;
  const size_t kFirLength = 20;

  const size_t kBufferSize = kRrLength + kReportBlockLength + kFirLength;
  MockFunction<void(rtc::ArrayView<const uint8_t>)> callback;
  EXPECT_CALL(callback, Call(_))
      .WillOnce(Invoke([&](rtc::ArrayView<const uint8_t> packet) {
        RtcpPacketParser parser;
        parser.Parse(packet.data(), packet.size());
        EXPECT_EQ(1, parser.receiver_report()->num_packets());
        EXPECT_EQ(1u, parser.receiver_report()->report_blocks().size());
        EXPECT_EQ(1, parser.fir()->num_packets());
      }));

  EXPECT_TRUE(compound.Build(kBufferSize, callback.AsStdFunction()));
}

TEST(RtcpCompoundPacketTest, BuildWithTooSmallBuffer_FragmentedSend) {
  CompoundPacket compound;
  auto fir = std::make_unique<Fir>();
  fir->AddRequestTo(kRemoteSsrc, kSeqNo);
  ReportBlock rb;
  auto rr = std::make_unique<ReceiverReport>();
  rr->SetSenderSsrc(kSenderSsrc);
  EXPECT_TRUE(rr->AddReportBlock(rb));
  compound.Append(std::move(rr));
  compound.Append(std::move(fir));

  const size_t kRrLength = 8;
  const size_t kReportBlockLength = 24;

  const size_t kBufferSize = kRrLength + kReportBlockLength;
  MockFunction<void(rtc::ArrayView<const uint8_t>)> callback;
  EXPECT_CALL(callback, Call(_))
      .WillOnce(Invoke([&](rtc::ArrayView<const uint8_t> packet) {
        RtcpPacketParser parser;
        parser.Parse(packet.data(), packet.size());
        EXPECT_EQ(1, parser.receiver_report()->num_packets());
        EXPECT_EQ(1U, parser.receiver_report()->report_blocks().size());
        EXPECT_EQ(0, parser.fir()->num_packets());
      }))
      .WillOnce(Invoke([&](rtc::ArrayView<const uint8_t> packet) {
        RtcpPacketParser parser;
        parser.Parse(packet.data(), packet.size());
        EXPECT_EQ(0, parser.receiver_report()->num_packets());
        EXPECT_EQ(0U, parser.receiver_report()->report_blocks().size());
        EXPECT_EQ(1, parser.fir()->num_packets());
      }));

  EXPECT_TRUE(compound.Build(kBufferSize, callback.AsStdFunction()));
}

}  // namespace webrtc
