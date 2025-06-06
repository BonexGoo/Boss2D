/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_rtc_base__hash_h //original-code:"rtc_base/hash.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {
namespace {

TEST(PairHashTest, CanInsertIntoSet) {
  using MyPair = std::pair<int, int>;

  std::unordered_set<MyPair, PairHash> pairs;

  pairs.insert({1, 2});
  pairs.insert({3, 4});

  EXPECT_NE(pairs.find({1, 2}), pairs.end());
  EXPECT_NE(pairs.find({3, 4}), pairs.end());
  EXPECT_EQ(pairs.find({1, 3}), pairs.end());
  EXPECT_EQ(pairs.find({3, 3}), pairs.end());
}

TEST(PairHashTest, CanInsertIntoMap) {
  using MyPair = std::pair<std::string, int>;

  std::unordered_map<MyPair, int, PairHash> pairs;

  pairs[{"1", 2}] = 99;
  pairs[{"3", 4}] = 100;

  EXPECT_EQ((pairs[{"1", 2}]), 99);
  EXPECT_EQ((pairs[{"3", 4}]), 100);
  EXPECT_EQ(pairs.find({"1", 3}), pairs.end());
  EXPECT_EQ(pairs.find({"3", 3}), pairs.end());
}
}  // namespace
}  // namespace webrtc
