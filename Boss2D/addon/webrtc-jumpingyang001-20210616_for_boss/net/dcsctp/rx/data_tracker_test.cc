/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__rx__data_tracker_h //original-code:"net/dcsctp/rx/data_tracker.h"

#include <cstdint>
#include <initializer_list>
#include <memory>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__sack_chunk_h //original-code:"net/dcsctp/packet/chunk/sack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__timer__fake_timeout_h //original-code:"net/dcsctp/timer/fake_timeout.h"
#include BOSS_WEBRTC_U_net__dcsctp__timer__timer_h //original-code:"net/dcsctp/timer/timer.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace dcsctp {
namespace {
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::SizeIs;
using ::testing::UnorderedElementsAre;

constexpr size_t kArwnd = 10000;
constexpr TSN kInitialTSN(11);

class DataTrackerTest : public testing::Test {
 protected:
  DataTrackerTest()
      : timeout_manager_([this]() { return now_; }),
        timer_manager_([this]() { return timeout_manager_.CreateTimeout(); }),
        timer_(timer_manager_.CreateTimer(
            "test/delayed_ack",
            []() { return absl::nullopt; },
            TimerOptions(DurationMs(0)))),
        buf_("log: ", timer_.get(), kInitialTSN) {}

  void Observer(std::initializer_list<uint32_t> tsns) {
    for (const uint32_t tsn : tsns) {
      buf_.Observe(TSN(tsn), AnyDataChunk::ImmediateAckFlag(false));
    }
  }

  TimeMs now_ = TimeMs(0);
  FakeTimeoutManager timeout_manager_;
  TimerManager timer_manager_;
  std::unique_ptr<Timer> timer_;
  DataTracker buf_;
};

TEST_F(DataTrackerTest, Empty) {
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(), IsEmpty());
  EXPECT_THAT(sack.duplicate_tsns(), IsEmpty());
}

TEST_F(DataTrackerTest, ObserverSingleInOrderPacket) {
  Observer({11});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(11));
  EXPECT_THAT(sack.gap_ack_blocks(), IsEmpty());
  EXPECT_THAT(sack.duplicate_tsns(), IsEmpty());
}

TEST_F(DataTrackerTest, ObserverManyInOrderMovesCumulativeTsnAck) {
  Observer({11, 12, 13});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(13));
  EXPECT_THAT(sack.gap_ack_blocks(), IsEmpty());
  EXPECT_THAT(sack.duplicate_tsns(), IsEmpty());
}

TEST_F(DataTrackerTest, ObserveOutOfOrderMovesCumulativeTsnAck) {
  Observer({12, 13, 14, 11});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(14));
  EXPECT_THAT(sack.gap_ack_blocks(), IsEmpty());
  EXPECT_THAT(sack.duplicate_tsns(), IsEmpty());
}

TEST_F(DataTrackerTest, SingleGap) {
  Observer({12});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 2)));
  EXPECT_THAT(sack.duplicate_tsns(), IsEmpty());
}

TEST_F(DataTrackerTest, ExampleFromRFC4960Section334) {
  Observer({11, 12, 14, 15, 17});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(12));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 3),
                                                 SackChunk::GapAckBlock(5, 5)));
  EXPECT_THAT(sack.duplicate_tsns(), IsEmpty());
}

TEST_F(DataTrackerTest, AckAlreadyReceivedChunk) {
  Observer({11});
  SackChunk sack1 = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack1.cumulative_tsn_ack(), TSN(11));
  EXPECT_THAT(sack1.gap_ack_blocks(), IsEmpty());

  // Receive old chunk
  Observer({8});
  SackChunk sack2 = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack2.cumulative_tsn_ack(), TSN(11));
  EXPECT_THAT(sack2.gap_ack_blocks(), IsEmpty());
}

TEST_F(DataTrackerTest, DoubleSendRetransmittedChunk) {
  Observer({11, 13, 14, 15});
  SackChunk sack1 = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack1.cumulative_tsn_ack(), TSN(11));
  EXPECT_THAT(sack1.gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(2, 4)));

  // Fill in the hole.
  Observer({12, 16, 17, 18});
  SackChunk sack2 = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack2.cumulative_tsn_ack(), TSN(18));
  EXPECT_THAT(sack2.gap_ack_blocks(), IsEmpty());

  // Receive chunk 12 again.
  Observer({12, 19, 20, 21});
  SackChunk sack3 = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack3.cumulative_tsn_ack(), TSN(21));
  EXPECT_THAT(sack3.gap_ack_blocks(), IsEmpty());
}

TEST_F(DataTrackerTest, ForwardTsnSimple) {
  // Messages (11, 12, 13), (14, 15) - first message expires.
  Observer({11, 12, 15});

  buf_.HandleForwardTsn(TSN(13));

  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(13));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 2)));
}

TEST_F(DataTrackerTest, ForwardTsnSkipsFromGapBlock) {
  // Messages (11, 12, 13), (14, 15) - first message expires.
  Observer({11, 12, 14});

  buf_.HandleForwardTsn(TSN(13));

  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(14));
  EXPECT_THAT(sack.gap_ack_blocks(), IsEmpty());
}

TEST_F(DataTrackerTest, ExampleFromRFC3758) {
  buf_.HandleForwardTsn(TSN(102));

  Observer({102, 104, 105, 107});

  buf_.HandleForwardTsn(TSN(103));

  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(105));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 2)));
}

TEST_F(DataTrackerTest, EmptyAllAcks) {
  Observer({11, 13, 14, 15});

  buf_.HandleForwardTsn(TSN(100));

  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(100));
  EXPECT_THAT(sack.gap_ack_blocks(), IsEmpty());
}

TEST_F(DataTrackerTest, SetsArwndCorrectly) {
  SackChunk sack1 = buf_.CreateSelectiveAck(/*a_rwnd=*/100);
  EXPECT_EQ(sack1.a_rwnd(), 100u);

  SackChunk sack2 = buf_.CreateSelectiveAck(/*a_rwnd=*/101);
  EXPECT_EQ(sack2.a_rwnd(), 101u);
}

TEST_F(DataTrackerTest, WillIncreaseCumAckTsn) {
  EXPECT_EQ(buf_.last_cumulative_acked_tsn(), TSN(10));
  EXPECT_FALSE(buf_.will_increase_cum_ack_tsn(TSN(10)));
  EXPECT_TRUE(buf_.will_increase_cum_ack_tsn(TSN(11)));
  EXPECT_FALSE(buf_.will_increase_cum_ack_tsn(TSN(12)));

  Observer({11, 12, 13, 14, 15});
  EXPECT_EQ(buf_.last_cumulative_acked_tsn(), TSN(15));
  EXPECT_FALSE(buf_.will_increase_cum_ack_tsn(TSN(15)));
  EXPECT_TRUE(buf_.will_increase_cum_ack_tsn(TSN(16)));
  EXPECT_FALSE(buf_.will_increase_cum_ack_tsn(TSN(17)));
}

TEST_F(DataTrackerTest, ForceShouldSendSackImmediately) {
  EXPECT_FALSE(buf_.ShouldSendAck());

  buf_.ForceImmediateSack();

  EXPECT_TRUE(buf_.ShouldSendAck());
}

TEST_F(DataTrackerTest, WillAcceptValidTSNs) {
  // The initial TSN is always one more than the last, which is our base.
  TSN last_tsn = TSN(*kInitialTSN - 1);
  int limit = static_cast<int>(DataTracker::kMaxAcceptedOutstandingFragments);

  for (int i = -limit; i <= limit; ++i) {
    EXPECT_TRUE(buf_.IsTSNValid(TSN(*last_tsn + i)));
  }
}

TEST_F(DataTrackerTest, WillNotAcceptInvalidTSNs) {
  // The initial TSN is always one more than the last, which is our base.
  TSN last_tsn = TSN(*kInitialTSN - 1);

  size_t limit = DataTracker::kMaxAcceptedOutstandingFragments;
  EXPECT_FALSE(buf_.IsTSNValid(TSN(*last_tsn + limit + 1)));
  EXPECT_FALSE(buf_.IsTSNValid(TSN(*last_tsn - (limit + 1))));
  EXPECT_FALSE(buf_.IsTSNValid(TSN(*last_tsn + 0x8000000)));
  EXPECT_FALSE(buf_.IsTSNValid(TSN(*last_tsn - 0x8000000)));
}

TEST_F(DataTrackerTest, ReportSingleDuplicateTsns) {
  Observer({11, 12, 11});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(12));
  EXPECT_THAT(sack.gap_ack_blocks(), IsEmpty());
  EXPECT_THAT(sack.duplicate_tsns(), UnorderedElementsAre(TSN(11)));
}

TEST_F(DataTrackerTest, ReportMultipleDuplicateTsns) {
  Observer({11, 12, 13, 14, 12, 13, 12, 13, 15, 16});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(16));
  EXPECT_THAT(sack.gap_ack_blocks(), IsEmpty());
  EXPECT_THAT(sack.duplicate_tsns(), UnorderedElementsAre(TSN(12), TSN(13)));
}

TEST_F(DataTrackerTest, ReportDuplicateTsnsInGapAckBlocks) {
  Observer({11, /*12,*/ 13, 14, 13, 14, 15, 16});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(11));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 5)));
  EXPECT_THAT(sack.duplicate_tsns(), UnorderedElementsAre(TSN(13), TSN(14)));
}

TEST_F(DataTrackerTest, ClearsDuplicateTsnsAfterCreatingSack) {
  Observer({11, 12, 13, 14, 12, 13, 12, 13, 15, 16});
  SackChunk sack1 = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack1.cumulative_tsn_ack(), TSN(16));
  EXPECT_THAT(sack1.gap_ack_blocks(), IsEmpty());
  EXPECT_THAT(sack1.duplicate_tsns(), UnorderedElementsAre(TSN(12), TSN(13)));

  Observer({17});
  SackChunk sack2 = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack2.cumulative_tsn_ack(), TSN(17));
  EXPECT_THAT(sack2.gap_ack_blocks(), IsEmpty());
  EXPECT_THAT(sack2.duplicate_tsns(), IsEmpty());
}

TEST_F(DataTrackerTest, LimitsNumberOfDuplicatesReported) {
  for (size_t i = 0; i < DataTracker::kMaxDuplicateTsnReported + 10; ++i) {
    TSN tsn(11 + i);
    buf_.Observe(tsn, AnyDataChunk::ImmediateAckFlag(false));
    buf_.Observe(tsn, AnyDataChunk::ImmediateAckFlag(false));
  }

  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_THAT(sack.gap_ack_blocks(), IsEmpty());
  EXPECT_THAT(sack.duplicate_tsns(),
              SizeIs(DataTracker::kMaxDuplicateTsnReported));
}

TEST_F(DataTrackerTest, LimitsNumberOfGapAckBlocksReported) {
  for (size_t i = 0; i < DataTracker::kMaxGapAckBlocksReported + 10; ++i) {
    TSN tsn(11 + i * 2);
    buf_.Observe(tsn, AnyDataChunk::ImmediateAckFlag(false));
  }

  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(11));
  EXPECT_THAT(sack.gap_ack_blocks(),
              SizeIs(DataTracker::kMaxGapAckBlocksReported));
}

TEST_F(DataTrackerTest, SendsSackForFirstPacketObserved) {
  Observer({11});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
}

TEST_F(DataTrackerTest, SendsSackEverySecondPacketWhenThereIsNoPacketLoss) {
  Observer({11});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  Observer({12});
  buf_.ObservePacketEnd();
  EXPECT_FALSE(buf_.ShouldSendAck());
  EXPECT_TRUE(timer_->is_running());
  Observer({13});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  Observer({14});
  buf_.ObservePacketEnd();
  EXPECT_FALSE(buf_.ShouldSendAck());
  EXPECT_TRUE(timer_->is_running());
  Observer({15});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
}

TEST_F(DataTrackerTest, SendsSackEveryPacketOnPacketLoss) {
  Observer({11});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  Observer({13});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  Observer({14});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  Observer({15});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  Observer({16});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  // Fill the hole.
  Observer({12});
  buf_.ObservePacketEnd();
  EXPECT_FALSE(buf_.ShouldSendAck());
  EXPECT_TRUE(timer_->is_running());
  // Goes back to every second packet
  Observer({17});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  Observer({18});
  buf_.ObservePacketEnd();
  EXPECT_FALSE(buf_.ShouldSendAck());
  EXPECT_TRUE(timer_->is_running());
}

TEST_F(DataTrackerTest, SendsSackOnDuplicateDataChunks) {
  Observer({11});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  Observer({11});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  Observer({12});
  buf_.ObservePacketEnd();
  EXPECT_FALSE(buf_.ShouldSendAck());
  EXPECT_TRUE(timer_->is_running());
  // Goes back to every second packet
  Observer({13});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
  // Duplicate again
  Observer({12});
  buf_.ObservePacketEnd();
  EXPECT_TRUE(buf_.ShouldSendAck());
  EXPECT_FALSE(timer_->is_running());
}

TEST_F(DataTrackerTest, GapAckBlockAddSingleBlock) {
  Observer({12});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 2)));
}

TEST_F(DataTrackerTest, GapAckBlockAddsAnother) {
  Observer({12});
  Observer({14});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 2),
                                                 SackChunk::GapAckBlock(4, 4)));
}

TEST_F(DataTrackerTest, GapAckBlockAddsDuplicate) {
  Observer({12});
  Observer({12});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 2)));
  EXPECT_THAT(sack.duplicate_tsns(), ElementsAre(TSN(12)));
}

TEST_F(DataTrackerTest, GapAckBlockExpandsToRight) {
  Observer({12});
  Observer({13});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 3)));
}

TEST_F(DataTrackerTest, GapAckBlockExpandsToRightWithOther) {
  Observer({12});
  Observer({20});
  Observer({30});
  Observer({21});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(2, 2),    //
                          SackChunk::GapAckBlock(10, 11),  //
                          SackChunk::GapAckBlock(20, 20)));
}

TEST_F(DataTrackerTest, GapAckBlockExpandsToLeft) {
  Observer({13});
  Observer({12});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(), ElementsAre(SackChunk::GapAckBlock(2, 3)));
}

TEST_F(DataTrackerTest, GapAckBlockExpandsToLeftWithOther) {
  Observer({12});
  Observer({21});
  Observer({30});
  Observer({20});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(2, 2),    //
                          SackChunk::GapAckBlock(10, 11),  //
                          SackChunk::GapAckBlock(20, 20)));
}

TEST_F(DataTrackerTest, GapAckBlockExpandsToLRightAndMerges) {
  Observer({12});
  Observer({20});
  Observer({22});
  Observer({30});
  Observer({21});
  SackChunk sack = buf_.CreateSelectiveAck(kArwnd);
  EXPECT_EQ(sack.cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(sack.gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(2, 2),    //
                          SackChunk::GapAckBlock(10, 12),  //
                          SackChunk::GapAckBlock(20, 20)));
}

TEST_F(DataTrackerTest, GapAckBlockMergesManyBlocksIntoOne) {
  Observer({22});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(12, 12)));
  Observer({30});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(12, 12),  //
                          SackChunk::GapAckBlock(20, 20)));
  Observer({24});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(12, 12),  //
                          SackChunk::GapAckBlock(14, 14),  //
                          SackChunk::GapAckBlock(20, 20)));
  Observer({28});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(12, 12),  //
                          SackChunk::GapAckBlock(14, 14),  //
                          SackChunk::GapAckBlock(18, 18),  //
                          SackChunk::GapAckBlock(20, 20)));
  Observer({26});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(12, 12),  //
                          SackChunk::GapAckBlock(14, 14),  //
                          SackChunk::GapAckBlock(16, 16),  //
                          SackChunk::GapAckBlock(18, 18),  //
                          SackChunk::GapAckBlock(20, 20)));
  Observer({29});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(12, 12),  //
                          SackChunk::GapAckBlock(14, 14),  //
                          SackChunk::GapAckBlock(16, 16),  //
                          SackChunk::GapAckBlock(18, 20)));
  Observer({23});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(12, 14),  //
                          SackChunk::GapAckBlock(16, 16),  //
                          SackChunk::GapAckBlock(18, 20)));
  Observer({27});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(12, 14),  //
                          SackChunk::GapAckBlock(16, 20)));

  Observer({25});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(12, 20)));
  Observer({20});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(10, 10),  //
                          SackChunk::GapAckBlock(12, 20)));
  Observer({32});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(10, 10),  //
                          SackChunk::GapAckBlock(12, 20),  //
                          SackChunk::GapAckBlock(22, 22)));
  Observer({21});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(10, 20),  //
                          SackChunk::GapAckBlock(22, 22)));
  Observer({31});
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(10, 22)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveBeforeCumAckTsn) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});

  buf_.HandleForwardTsn(TSN(8));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(10));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(2, 4),  //
                          SackChunk::GapAckBlock(10, 12),
                          SackChunk::GapAckBlock(20, 21)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveBeforeFirstBlock) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});

  buf_.HandleForwardTsn(TSN(11));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(14));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(6, 8),  //
                          SackChunk::GapAckBlock(16, 17)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveAtBeginningOfFirstBlock) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});

  buf_.HandleForwardTsn(TSN(12));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(14));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(6, 8),  //
                          SackChunk::GapAckBlock(16, 17)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveAtMiddleOfFirstBlock) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});
  buf_.HandleForwardTsn(TSN(13));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(14));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(6, 8),  //
                          SackChunk::GapAckBlock(16, 17)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveAtEndOfFirstBlock) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});
  buf_.HandleForwardTsn(TSN(14));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(14));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(6, 8),  //
                          SackChunk::GapAckBlock(16, 17)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveRightAfterFirstBlock) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});

  buf_.HandleForwardTsn(TSN(18));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(18));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(2, 4),  //
                          SackChunk::GapAckBlock(12, 13)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveRightBeforeSecondBlock) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});

  buf_.HandleForwardTsn(TSN(19));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(22));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(8, 9)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveRightAtStartOfSecondBlock) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});

  buf_.HandleForwardTsn(TSN(20));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(22));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(8, 9)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveRightAtMiddleOfSecondBlock) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});

  buf_.HandleForwardTsn(TSN(21));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(22));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(8, 9)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveRightAtEndOfSecondBlock) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});

  buf_.HandleForwardTsn(TSN(22));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(22));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(),
              ElementsAre(SackChunk::GapAckBlock(8, 9)));
}

TEST_F(DataTrackerTest, GapAckBlockRemoveeFarAfterAllBlocks) {
  Observer({12, 13, 14, 20, 21, 22, 30, 31});

  buf_.HandleForwardTsn(TSN(40));
  EXPECT_EQ(buf_.CreateSelectiveAck(kArwnd).cumulative_tsn_ack(), TSN(40));
  EXPECT_THAT(buf_.CreateSelectiveAck(kArwnd).gap_ack_blocks(), IsEmpty());
}

}  // namespace
}  // namespace dcsctp
