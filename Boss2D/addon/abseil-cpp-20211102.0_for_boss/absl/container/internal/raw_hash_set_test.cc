// Copyright 2018 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include BOSS_ABSEILCPP_U_absl__container__internal__raw_hash_set_h //original-code:"absl/container/internal/raw_hash_set.h"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_ABSEILCPP_U_absl__base__config_h //original-code:"absl/base/config.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__cycleclock_h //original-code:"absl/base/internal/cycleclock.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__raw_logging_h //original-code:"absl/base/internal/raw_logging.h"
#include BOSS_ABSEILCPP_U_absl__container__internal__container_memory_h //original-code:"absl/container/internal/container_memory.h"
#include BOSS_ABSEILCPP_U_absl__container__internal__hash_function_defaults_h //original-code:"absl/container/internal/hash_function_defaults.h"
#include BOSS_ABSEILCPP_U_absl__container__internal__hash_policy_testing_h //original-code:"absl/container/internal/hash_policy_testing.h"
#include BOSS_ABSEILCPP_U_absl__container__internal__hashtable_debug_h //original-code:"absl/container/internal/hashtable_debug.h"
#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"

namespace absl {
ABSL_NAMESPACE_BEGIN
namespace container_internal {

struct RawHashSetTestOnlyAccess {
  template <typename C>
  static auto GetSlots(const C& c) -> decltype(c.slots_) {
    return c.slots_;
  }
};

namespace {

using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Lt;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

// Convenience function to static cast to ctrl_t.
ctrl_t CtrlT(int i) { return static_cast<ctrl_t>(i); }

TEST(Util, NormalizeCapacity) {
  EXPECT_EQ(1, NormalizeCapacity(0));
  EXPECT_EQ(1, NormalizeCapacity(1));
  EXPECT_EQ(3, NormalizeCapacity(2));
  EXPECT_EQ(3, NormalizeCapacity(3));
  EXPECT_EQ(7, NormalizeCapacity(4));
  EXPECT_EQ(7, NormalizeCapacity(7));
  EXPECT_EQ(15, NormalizeCapacity(8));
  EXPECT_EQ(15, NormalizeCapacity(15));
  EXPECT_EQ(15 * 2 + 1, NormalizeCapacity(15 + 1));
  EXPECT_EQ(15 * 2 + 1, NormalizeCapacity(15 + 2));
}

TEST(Util, GrowthAndCapacity) {
  // Verify that GrowthToCapacity gives the minimum capacity that has enough
  // growth.
  for (size_t growth = 0; growth < 10000; ++growth) {
    SCOPED_TRACE(growth);
    size_t capacity = NormalizeCapacity(GrowthToLowerboundCapacity(growth));
    // The capacity is large enough for `growth`.
    EXPECT_THAT(CapacityToGrowth(capacity), Ge(growth));
    // For (capacity+1) < kWidth, growth should equal capacity.
    if (capacity + 1 < Group::kWidth) {
      EXPECT_THAT(CapacityToGrowth(capacity), Eq(capacity));
    } else {
      EXPECT_THAT(CapacityToGrowth(capacity), Lt(capacity));
    }
    if (growth != 0 && capacity > 1) {
      // There is no smaller capacity that works.
      EXPECT_THAT(CapacityToGrowth(capacity / 2), Lt(growth));
    }
  }

  for (size_t capacity = Group::kWidth - 1; capacity < 10000;
       capacity = 2 * capacity + 1) {
    SCOPED_TRACE(capacity);
    size_t growth = CapacityToGrowth(capacity);
    EXPECT_THAT(growth, Lt(capacity));
    EXPECT_LE(GrowthToLowerboundCapacity(growth), capacity);
    EXPECT_EQ(NormalizeCapacity(GrowthToLowerboundCapacity(growth)), capacity);
  }
}

TEST(Util, probe_seq) {
  probe_seq<16> seq(0, 127);
  auto gen = [&]() {
    size_t res = seq.offset();
    seq.next();
    return res;
  };
  std::vector<size_t> offsets(8);
  std::generate_n(offsets.begin(), 8, gen);
  EXPECT_THAT(offsets, ElementsAre(0, 16, 48, 96, 32, 112, 80, 64));
  seq = probe_seq<16>(128, 127);
  std::generate_n(offsets.begin(), 8, gen);
  EXPECT_THAT(offsets, ElementsAre(0, 16, 48, 96, 32, 112, 80, 64));
}

TEST(BitMask, Smoke) {
  EXPECT_FALSE((BitMask<uint8_t, 8>(0)));
  EXPECT_TRUE((BitMask<uint8_t, 8>(5)));

  EXPECT_THAT((BitMask<uint8_t, 8>(0)), ElementsAre());
  EXPECT_THAT((BitMask<uint8_t, 8>(0x1)), ElementsAre(0));
  EXPECT_THAT((BitMask<uint8_t, 8>(0x2)), ElementsAre(1));
  EXPECT_THAT((BitMask<uint8_t, 8>(0x3)), ElementsAre(0, 1));
  EXPECT_THAT((BitMask<uint8_t, 8>(0x4)), ElementsAre(2));
  EXPECT_THAT((BitMask<uint8_t, 8>(0x5)), ElementsAre(0, 2));
  EXPECT_THAT((BitMask<uint8_t, 8>(0x55)), ElementsAre(0, 2, 4, 6));
  EXPECT_THAT((BitMask<uint8_t, 8>(0xAA)), ElementsAre(1, 3, 5, 7));
}

TEST(BitMask, WithShift) {
  // See the non-SSE version of Group for details on what this math is for.
  uint64_t ctrl = 0x1716151413121110;
  uint64_t hash = 0x12;
  constexpr uint64_t msbs = 0x8080808080808080ULL;
  constexpr uint64_t lsbs = 0x0101010101010101ULL;
  auto x = ctrl ^ (lsbs * hash);
  uint64_t mask = (x - lsbs) & ~x & msbs;
  EXPECT_EQ(0x0000000080800000, mask);

  BitMask<uint64_t, 8, 3> b(mask);
  EXPECT_EQ(*b, 2);
}

TEST(BitMask, LeadingTrailing) {
  EXPECT_EQ((BitMask<uint32_t, 16>(0x00001a40).LeadingZeros()), 3);
  EXPECT_EQ((BitMask<uint32_t, 16>(0x00001a40).TrailingZeros()), 6);

  EXPECT_EQ((BitMask<uint32_t, 16>(0x00000001).LeadingZeros()), 15);
  EXPECT_EQ((BitMask<uint32_t, 16>(0x00000001).TrailingZeros()), 0);

  EXPECT_EQ((BitMask<uint32_t, 16>(0x00008000).LeadingZeros()), 0);
  EXPECT_EQ((BitMask<uint32_t, 16>(0x00008000).TrailingZeros()), 15);

  EXPECT_EQ((BitMask<uint64_t, 8, 3>(0x0000008080808000).LeadingZeros()), 3);
  EXPECT_EQ((BitMask<uint64_t, 8, 3>(0x0000008080808000).TrailingZeros()), 1);

  EXPECT_EQ((BitMask<uint64_t, 8, 3>(0x0000000000000080).LeadingZeros()), 7);
  EXPECT_EQ((BitMask<uint64_t, 8, 3>(0x0000000000000080).TrailingZeros()), 0);

  EXPECT_EQ((BitMask<uint64_t, 8, 3>(0x8000000000000000).LeadingZeros()), 0);
  EXPECT_EQ((BitMask<uint64_t, 8, 3>(0x8000000000000000).TrailingZeros()), 7);
}

TEST(Group, EmptyGroup) {
  for (h2_t h = 0; h != 128; ++h) EXPECT_FALSE(Group{EmptyGroup()}.Match(h));
}

TEST(Group, Match) {
  if (Group::kWidth == 16) {
    ctrl_t group[] = {ctrl_t::kEmpty, CtrlT(1), ctrl_t::kDeleted,  CtrlT(3),
                      ctrl_t::kEmpty, CtrlT(5), ctrl_t::kSentinel, CtrlT(7),
                      CtrlT(7),       CtrlT(5), CtrlT(3),          CtrlT(1),
                      CtrlT(1),       CtrlT(1), CtrlT(1),          CtrlT(1)};
    EXPECT_THAT(Group{group}.Match(0), ElementsAre());
    EXPECT_THAT(Group{group}.Match(1), ElementsAre(1, 11, 12, 13, 14, 15));
    EXPECT_THAT(Group{group}.Match(3), ElementsAre(3, 10));
    EXPECT_THAT(Group{group}.Match(5), ElementsAre(5, 9));
    EXPECT_THAT(Group{group}.Match(7), ElementsAre(7, 8));
  } else if (Group::kWidth == 8) {
    ctrl_t group[] = {ctrl_t::kEmpty,    CtrlT(1), CtrlT(2),
                      ctrl_t::kDeleted,  CtrlT(2), CtrlT(1),
                      ctrl_t::kSentinel, CtrlT(1)};
    EXPECT_THAT(Group{group}.Match(0), ElementsAre());
    EXPECT_THAT(Group{group}.Match(1), ElementsAre(1, 5, 7));
    EXPECT_THAT(Group{group}.Match(2), ElementsAre(2, 4));
  } else {
    FAIL() << "No test coverage for Group::kWidth==" << Group::kWidth;
  }
}

TEST(Group, MatchEmpty) {
  if (Group::kWidth == 16) {
    ctrl_t group[] = {ctrl_t::kEmpty, CtrlT(1), ctrl_t::kDeleted,  CtrlT(3),
                      ctrl_t::kEmpty, CtrlT(5), ctrl_t::kSentinel, CtrlT(7),
                      CtrlT(7),       CtrlT(5), CtrlT(3),          CtrlT(1),
                      CtrlT(1),       CtrlT(1), CtrlT(1),          CtrlT(1)};
    EXPECT_THAT(Group{group}.MatchEmpty(), ElementsAre(0, 4));
  } else if (Group::kWidth == 8) {
    ctrl_t group[] = {ctrl_t::kEmpty,    CtrlT(1), CtrlT(2),
                      ctrl_t::kDeleted,  CtrlT(2), CtrlT(1),
                      ctrl_t::kSentinel, CtrlT(1)};
    EXPECT_THAT(Group{group}.MatchEmpty(), ElementsAre(0));
  } else {
    FAIL() << "No test coverage for Group::kWidth==" << Group::kWidth;
  }
}

TEST(Group, MatchEmptyOrDeleted) {
  if (Group::kWidth == 16) {
    ctrl_t group[] = {ctrl_t::kEmpty, CtrlT(1), ctrl_t::kDeleted,  CtrlT(3),
                      ctrl_t::kEmpty, CtrlT(5), ctrl_t::kSentinel, CtrlT(7),
                      CtrlT(7),       CtrlT(5), CtrlT(3),          CtrlT(1),
                      CtrlT(1),       CtrlT(1), CtrlT(1),          CtrlT(1)};
    EXPECT_THAT(Group{group}.MatchEmptyOrDeleted(), ElementsAre(0, 2, 4));
  } else if (Group::kWidth == 8) {
    ctrl_t group[] = {ctrl_t::kEmpty,    CtrlT(1), CtrlT(2),
                      ctrl_t::kDeleted,  CtrlT(2), CtrlT(1),
                      ctrl_t::kSentinel, CtrlT(1)};
    EXPECT_THAT(Group{group}.MatchEmptyOrDeleted(), ElementsAre(0, 3));
  } else {
    FAIL() << "No test coverage for Group::kWidth==" << Group::kWidth;
  }
}

TEST(Batch, DropDeletes) {
  constexpr size_t kCapacity = 63;
  constexpr size_t kGroupWidth = container_internal::Group::kWidth;
  std::vector<ctrl_t> ctrl(kCapacity + 1 + kGroupWidth);
  ctrl[kCapacity] = ctrl_t::kSentinel;
  std::vector<ctrl_t> pattern = {
      ctrl_t::kEmpty, CtrlT(2), ctrl_t::kDeleted, CtrlT(2),
      ctrl_t::kEmpty, CtrlT(1), ctrl_t::kDeleted};
  for (size_t i = 0; i != kCapacity; ++i) {
    ctrl[i] = pattern[i % pattern.size()];
    if (i < kGroupWidth - 1)
      ctrl[i + kCapacity + 1] = pattern[i % pattern.size()];
  }
  ConvertDeletedToEmptyAndFullToDeleted(ctrl.data(), kCapacity);
  ASSERT_EQ(ctrl[kCapacity], ctrl_t::kSentinel);
  for (size_t i = 0; i < kCapacity + kGroupWidth; ++i) {
    ctrl_t expected = pattern[i % (kCapacity + 1) % pattern.size()];
    if (i == kCapacity) expected = ctrl_t::kSentinel;
    if (expected == ctrl_t::kDeleted) expected = ctrl_t::kEmpty;
    if (IsFull(expected)) expected = ctrl_t::kDeleted;
    EXPECT_EQ(ctrl[i], expected)
        << i << " " << static_cast<int>(pattern[i % pattern.size()]);
  }
}

TEST(Group, CountLeadingEmptyOrDeleted) {
  const std::vector<ctrl_t> empty_examples = {ctrl_t::kEmpty, ctrl_t::kDeleted};
  const std::vector<ctrl_t> full_examples = {
      CtrlT(0), CtrlT(1), CtrlT(2),   CtrlT(3),
      CtrlT(5), CtrlT(9), CtrlT(127), ctrl_t::kSentinel};

  for (ctrl_t empty : empty_examples) {
    std::vector<ctrl_t> e(Group::kWidth, empty);
    EXPECT_EQ(Group::kWidth, Group{e.data()}.CountLeadingEmptyOrDeleted());
    for (ctrl_t full : full_examples) {
      for (size_t i = 0; i != Group::kWidth; ++i) {
        std::vector<ctrl_t> f(Group::kWidth, empty);
        f[i] = full;
        EXPECT_EQ(i, Group{f.data()}.CountLeadingEmptyOrDeleted());
      }
      std::vector<ctrl_t> f(Group::kWidth, empty);
      f[Group::kWidth * 2 / 3] = full;
      f[Group::kWidth / 2] = full;
      EXPECT_EQ(
          Group::kWidth / 2, Group{f.data()}.CountLeadingEmptyOrDeleted());
    }
  }
}

template <class T>
struct ValuePolicy {
  using slot_type = T;
  using key_type = T;
  using init_type = T;

  template <class Allocator, class... Args>
  static void construct(Allocator* alloc, slot_type* slot, Args&&... args) {
    absl::allocator_traits<Allocator>::construct(*alloc, slot,
                                                 std::forward<Args>(args)...);
  }

  template <class Allocator>
  static void destroy(Allocator* alloc, slot_type* slot) {
    absl::allocator_traits<Allocator>::destroy(*alloc, slot);
  }

  template <class Allocator>
  static void transfer(Allocator* alloc, slot_type* new_slot,
                       slot_type* old_slot) {
    construct(alloc, new_slot, std::move(*old_slot));
    destroy(alloc, old_slot);
  }

  static T& element(slot_type* slot) { return *slot; }

  template <class F, class... Args>
  static decltype(absl::container_internal::DecomposeValue(
      std::declval<F>(), std::declval<Args>()...))
  apply(F&& f, Args&&... args) {
    return absl::container_internal::DecomposeValue(
        std::forward<F>(f), std::forward<Args>(args)...);
  }
};

using IntPolicy = ValuePolicy<int64_t>;
using Uint8Policy = ValuePolicy<uint8_t>;

class StringPolicy {
  template <class F, class K, class V,
            class = typename std::enable_if<
                std::is_convertible<const K&, absl::string_view>::value>::type>
  decltype(std::declval<F>()(
      std::declval<const absl::string_view&>(), std::piecewise_construct,
      std::declval<std::tuple<K>>(),
      std::declval<V>())) static apply_impl(F&& f,
                                            std::pair<std::tuple<K>, V> p) {
    const absl::string_view& key = std::get<0>(p.first);
    return std::forward<F>(f)(key, std::piecewise_construct, std::move(p.first),
                              std::move(p.second));
  }

 public:
  struct slot_type {
    struct ctor {};

    template <class... Ts>
    slot_type(ctor, Ts&&... ts) : pair(std::forward<Ts>(ts)...) {}

    std::pair<std::string, std::string> pair;
  };

  using key_type = std::string;
  using init_type = std::pair<std::string, std::string>;

  template <class allocator_type, class... Args>
  static void construct(allocator_type* alloc, slot_type* slot, Args... args) {
    std::allocator_traits<allocator_type>::construct(
        *alloc, slot, typename slot_type::ctor(), std::forward<Args>(args)...);
  }

  template <class allocator_type>
  static void destroy(allocator_type* alloc, slot_type* slot) {
    std::allocator_traits<allocator_type>::destroy(*alloc, slot);
  }

  template <class allocator_type>
  static void transfer(allocator_type* alloc, slot_type* new_slot,
                       slot_type* old_slot) {
    construct(alloc, new_slot, std::move(old_slot->pair));
    destroy(alloc, old_slot);
  }

  static std::pair<std::string, std::string>& element(slot_type* slot) {
    return slot->pair;
  }

  template <class F, class... Args>
  static auto apply(F&& f, Args&&... args)
      -> decltype(apply_impl(std::forward<F>(f),
                             PairArgs(std::forward<Args>(args)...))) {
    return apply_impl(std::forward<F>(f),
                      PairArgs(std::forward<Args>(args)...));
  }
};

struct StringHash : absl::Hash<absl::string_view> {
  using is_transparent = void;
};
struct StringEq : std::equal_to<absl::string_view> {
  using is_transparent = void;
};

struct StringTable
    : raw_hash_set<StringPolicy, StringHash, StringEq, std::allocator<int>> {
  using Base = typename StringTable::raw_hash_set;
  StringTable() {}
  using Base::Base;
};

struct IntTable
    : raw_hash_set<IntPolicy, container_internal::hash_default_hash<int64_t>,
                   std::equal_to<int64_t>, std::allocator<int64_t>> {
  using Base = typename IntTable::raw_hash_set;
  using Base::Base;
};

struct Uint8Table
    : raw_hash_set<Uint8Policy, container_internal::hash_default_hash<uint8_t>,
                   std::equal_to<uint8_t>, std::allocator<uint8_t>> {
  using Base = typename Uint8Table::raw_hash_set;
  using Base::Base;
};

template <typename T>
struct CustomAlloc : std::allocator<T> {
  CustomAlloc() {}

  template <typename U>
  CustomAlloc(const CustomAlloc<U>& other) {}

  template<class U> struct rebind {
    using other = CustomAlloc<U>;
  };
};

struct CustomAllocIntTable
    : raw_hash_set<IntPolicy, container_internal::hash_default_hash<int64_t>,
                   std::equal_to<int64_t>, CustomAlloc<int64_t>> {
  using Base = typename CustomAllocIntTable::raw_hash_set;
  using Base::Base;
};

struct BadFastHash {
  template <class T>
  size_t operator()(const T&) const {
    return 0;
  }
};

struct BadTable : raw_hash_set<IntPolicy, BadFastHash, std::equal_to<int>,
                               std::allocator<int>> {
  using Base = typename BadTable::raw_hash_set;
  BadTable() {}
  using Base::Base;
};

TEST(Table, EmptyFunctorOptimization) {
  static_assert(std::is_empty<std::equal_to<absl::string_view>>::value, "");
  static_assert(std::is_empty<std::allocator<int>>::value, "");

  struct MockTable {
    void* ctrl;
    void* slots;
    size_t size;
    size_t capacity;
    size_t growth_left;
    void* infoz;
  };
  struct MockTableInfozDisabled {
    void* ctrl;
    void* slots;
    size_t size;
    size_t capacity;
    size_t growth_left;
  };
  struct StatelessHash {
    size_t operator()(absl::string_view) const { return 0; }
  };
  struct StatefulHash : StatelessHash {
    size_t dummy;
  };

  if (std::is_empty<HashtablezInfoHandle>::value) {
    EXPECT_EQ(sizeof(MockTableInfozDisabled),
              sizeof(raw_hash_set<StringPolicy, StatelessHash,
                                  std::equal_to<absl::string_view>,
                                  std::allocator<int>>));

    EXPECT_EQ(sizeof(MockTableInfozDisabled) + sizeof(StatefulHash),
              sizeof(raw_hash_set<StringPolicy, StatefulHash,
                                  std::equal_to<absl::string_view>,
                                  std::allocator<int>>));
  } else {
    EXPECT_EQ(sizeof(MockTable),
              sizeof(raw_hash_set<StringPolicy, StatelessHash,
                                  std::equal_to<absl::string_view>,
                                  std::allocator<int>>));

    EXPECT_EQ(sizeof(MockTable) + sizeof(StatefulHash),
              sizeof(raw_hash_set<StringPolicy, StatefulHash,
                                  std::equal_to<absl::string_view>,
                                  std::allocator<int>>));
  }
}

TEST(Table, Empty) {
  IntTable t;
  EXPECT_EQ(0, t.size());
  EXPECT_TRUE(t.empty());
}

TEST(Table, LookupEmpty) {
  IntTable t;
  auto it = t.find(0);
  EXPECT_TRUE(it == t.end());
}

TEST(Table, Insert1) {
  IntTable t;
  EXPECT_TRUE(t.find(0) == t.end());
  auto res = t.emplace(0);
  EXPECT_TRUE(res.second);
  EXPECT_THAT(*res.first, 0);
  EXPECT_EQ(1, t.size());
  EXPECT_THAT(*t.find(0), 0);
}

TEST(Table, Insert2) {
  IntTable t;
  EXPECT_TRUE(t.find(0) == t.end());
  auto res = t.emplace(0);
  EXPECT_TRUE(res.second);
  EXPECT_THAT(*res.first, 0);
  EXPECT_EQ(1, t.size());
  EXPECT_TRUE(t.find(1) == t.end());
  res = t.emplace(1);
  EXPECT_TRUE(res.second);
  EXPECT_THAT(*res.first, 1);
  EXPECT_EQ(2, t.size());
  EXPECT_THAT(*t.find(0), 0);
  EXPECT_THAT(*t.find(1), 1);
}

TEST(Table, InsertCollision) {
  BadTable t;
  EXPECT_TRUE(t.find(1) == t.end());
  auto res = t.emplace(1);
  EXPECT_TRUE(res.second);
  EXPECT_THAT(*res.first, 1);
  EXPECT_EQ(1, t.size());

  EXPECT_TRUE(t.find(2) == t.end());
  res = t.emplace(2);
  EXPECT_THAT(*res.first, 2);
  EXPECT_TRUE(res.second);
  EXPECT_EQ(2, t.size());

  EXPECT_THAT(*t.find(1), 1);
  EXPECT_THAT(*t.find(2), 2);
}

// Test that we do not add existent element in case we need to search through
// many groups with deleted elements
TEST(Table, InsertCollisionAndFindAfterDelete) {
  BadTable t;  // all elements go to the same group.
  // Have at least 2 groups with Group::kWidth collisions
  // plus some extra collisions in the last group.
  constexpr size_t kNumInserts = Group::kWidth * 2 + 5;
  for (size_t i = 0; i < kNumInserts; ++i) {
    auto res = t.emplace(i);
    EXPECT_TRUE(res.second);
    EXPECT_THAT(*res.first, i);
    EXPECT_EQ(i + 1, t.size());
  }

  // Remove elements one by one and check
  // that we still can find all other elements.
  for (size_t i = 0; i < kNumInserts; ++i) {
    EXPECT_EQ(1, t.erase(i)) << i;
    for (size_t j = i + 1; j < kNumInserts; ++j) {
      EXPECT_THAT(*t.find(j), j);
      auto res = t.emplace(j);
      EXPECT_FALSE(res.second) << i << " " << j;
      EXPECT_THAT(*res.first, j);
      EXPECT_EQ(kNumInserts - i - 1, t.size());
    }
  }
  EXPECT_TRUE(t.empty());
}

TEST(Table, InsertWithinCapacity) {
  IntTable t;
  t.reserve(10);
  const size_t original_capacity = t.capacity();
  const auto addr = [&](int i) {
    return reinterpret_cast<uintptr_t>(&*t.find(i));
  };
  // Inserting an element does not change capacity.
  t.insert(0);
  EXPECT_THAT(t.capacity(), original_capacity);
  const uintptr_t original_addr_0 = addr(0);
  // Inserting another element does not rehash.
  t.insert(1);
  EXPECT_THAT(t.capacity(), original_capacity);
  EXPECT_THAT(addr(0), original_addr_0);
  // Inserting lots of duplicate elements does not rehash.
  for (int i = 0; i < 100; ++i) {
    t.insert(i % 10);
  }
  EXPECT_THAT(t.capacity(), original_capacity);
  EXPECT_THAT(addr(0), original_addr_0);
  // Inserting a range of duplicate elements does not rehash.
  std::vector<int> dup_range;
  for (int i = 0; i < 100; ++i) {
    dup_range.push_back(i % 10);
  }
  t.insert(dup_range.begin(), dup_range.end());
  EXPECT_THAT(t.capacity(), original_capacity);
  EXPECT_THAT(addr(0), original_addr_0);
}

TEST(Table, LazyEmplace) {
  StringTable t;
  bool called = false;
  auto it = t.lazy_emplace("abc", [&](const StringTable::constructor& f) {
    called = true;
    f("abc", "ABC");
  });
  EXPECT_TRUE(called);
  EXPECT_THAT(*it, Pair("abc", "ABC"));
  called = false;
  it = t.lazy_emplace("abc", [&](const StringTable::constructor& f) {
    called = true;
    f("abc", "DEF");
  });
  EXPECT_FALSE(called);
  EXPECT_THAT(*it, Pair("abc", "ABC"));
}

TEST(Table, ContainsEmpty) {
  IntTable t;

  EXPECT_FALSE(t.contains(0));
}

TEST(Table, Contains1) {
  IntTable t;

  EXPECT_TRUE(t.insert(0).second);
  EXPECT_TRUE(t.contains(0));
  EXPECT_FALSE(t.contains(1));

  EXPECT_EQ(1, t.erase(0));
  EXPECT_FALSE(t.contains(0));
}

TEST(Table, Contains2) {
  IntTable t;

  EXPECT_TRUE(t.insert(0).second);
  EXPECT_TRUE(t.contains(0));
  EXPECT_FALSE(t.contains(1));

  t.clear();
  EXPECT_FALSE(t.contains(0));
}

int decompose_constructed;
int decompose_copy_constructed;
int decompose_copy_assigned;
int decompose_move_constructed;
int decompose_move_assigned;
struct DecomposeType {
  DecomposeType(int i = 0) : i(i) {  // NOLINT
    ++decompose_constructed;
  }

  explicit DecomposeType(const char* d) : DecomposeType(*d) {}

  DecomposeType(const DecomposeType& other) : i(other.i) {
    ++decompose_copy_constructed;
  }
  DecomposeType& operator=(const DecomposeType& other) {
    ++decompose_copy_assigned;
    i = other.i;
    return *this;
  }
  DecomposeType(DecomposeType&& other) : i(other.i) {
    ++decompose_move_constructed;
  }
  DecomposeType& operator=(DecomposeType&& other) {
    ++decompose_move_assigned;
    i = other.i;
    return *this;
  }

  int i;
};

struct DecomposeHash {
  using is_transparent = void;
  size_t operator()(const DecomposeType& a) const { return a.i; }
  size_t operator()(int a) const { return a; }
  size_t operator()(const char* a) const { return *a; }
};

struct DecomposeEq {
  using is_transparent = void;
  bool operator()(const DecomposeType& a, const DecomposeType& b) const {
    return a.i == b.i;
  }
  bool operator()(const DecomposeType& a, int b) const { return a.i == b; }
  bool operator()(const DecomposeType& a, const char* b) const {
    return a.i == *b;
  }
};

struct DecomposePolicy {
  using slot_type = DecomposeType;
  using key_type = DecomposeType;
  using init_type = DecomposeType;

  template <typename T>
  static void construct(void*, DecomposeType* slot, T&& v) {
    ::new (slot) DecomposeType(std::forward<T>(v));
  }
  static void destroy(void*, DecomposeType* slot) { slot->~DecomposeType(); }
  static DecomposeType& element(slot_type* slot) { return *slot; }

  template <class F, class T>
  static auto apply(F&& f, const T& x) -> decltype(std::forward<F>(f)(x, x)) {
    return std::forward<F>(f)(x, x);
  }
};

template <typename Hash, typename Eq>
void TestDecompose(bool construct_three) {
  DecomposeType elem{0};
  const int one = 1;
  const char* three_p = "3";
  const auto& three = three_p;
  const int elem_vector_count = 256;
  std::vector<DecomposeType> elem_vector(elem_vector_count, DecomposeType{0});
  std::iota(elem_vector.begin(), elem_vector.end(), 0);

  using DecomposeSet =
      raw_hash_set<DecomposePolicy, Hash, Eq, std::allocator<int>>;
  DecomposeSet set1;

  decompose_constructed = 0;
  int expected_constructed = 0;
  EXPECT_EQ(expected_constructed, decompose_constructed);
  set1.insert(elem);
  EXPECT_EQ(expected_constructed, decompose_constructed);
  set1.insert(1);
  EXPECT_EQ(++expected_constructed, decompose_constructed);
  set1.emplace("3");
  EXPECT_EQ(++expected_constructed, decompose_constructed);
  EXPECT_EQ(expected_constructed, decompose_constructed);

  {  // insert(T&&)
    set1.insert(1);
    EXPECT_EQ(expected_constructed, decompose_constructed);
  }

  {  // insert(const T&)
    set1.insert(one);
    EXPECT_EQ(expected_constructed, decompose_constructed);
  }

  {  // insert(hint, T&&)
    set1.insert(set1.begin(), 1);
    EXPECT_EQ(expected_constructed, decompose_constructed);
  }

  {  // insert(hint, const T&)
    set1.insert(set1.begin(), one);
    EXPECT_EQ(expected_constructed, decompose_constructed);
  }

  {  // emplace(...)
    set1.emplace(1);
    EXPECT_EQ(expected_constructed, decompose_constructed);
    set1.emplace("3");
    expected_constructed += construct_three;
    EXPECT_EQ(expected_constructed, decompose_constructed);
    set1.emplace(one);
    EXPECT_EQ(expected_constructed, decompose_constructed);
    set1.emplace(three);
    expected_constructed += construct_three;
    EXPECT_EQ(expected_constructed, decompose_constructed);
  }

  {  // emplace_hint(...)
    set1.emplace_hint(set1.begin(), 1);
    EXPECT_EQ(expected_constructed, decompose_constructed);
    set1.emplace_hint(set1.begin(), "3");
    expected_constructed += construct_three;
    EXPECT_EQ(expected_constructed, decompose_constructed);
    set1.emplace_hint(set1.begin(), one);
    EXPECT_EQ(expected_constructed, decompose_constructed);
    set1.emplace_hint(set1.begin(), three);
    expected_constructed += construct_three;
    EXPECT_EQ(expected_constructed, decompose_constructed);
  }

  decompose_copy_constructed = 0;
  decompose_copy_assigned = 0;
  decompose_move_constructed = 0;
  decompose_move_assigned = 0;
  int expected_copy_constructed = 0;
  int expected_move_constructed = 0;
  {  // raw_hash_set(first, last) with random-access iterators
    DecomposeSet set2(elem_vector.begin(), elem_vector.end());
    // Expect exactly one copy-constructor call for each element if no
    // rehashing is done.
    expected_copy_constructed += elem_vector_count;
    EXPECT_EQ(expected_copy_constructed, decompose_copy_constructed);
    EXPECT_EQ(expected_move_constructed, decompose_move_constructed);
    EXPECT_EQ(0, decompose_move_assigned);
    EXPECT_EQ(0, decompose_copy_assigned);
  }

  {  // raw_hash_set(first, last) with forward iterators
    std::list<DecomposeType> elem_list(elem_vector.begin(), elem_vector.end());
    expected_copy_constructed = decompose_copy_constructed;
    DecomposeSet set2(elem_list.begin(), elem_list.end());
    // Expect exactly N elements copied into set, expect at most 2*N elements
    // moving internally for all resizing needed (for a growth factor of 2).
    expected_copy_constructed += elem_vector_count;
    EXPECT_EQ(expected_copy_constructed, decompose_copy_constructed);
    expected_move_constructed += elem_vector_count;
    EXPECT_LT(expected_move_constructed, decompose_move_constructed);
    expected_move_constructed += elem_vector_count;
    EXPECT_GE(expected_move_constructed, decompose_move_constructed);
    EXPECT_EQ(0, decompose_move_assigned);
    EXPECT_EQ(0, decompose_copy_assigned);
    expected_copy_constructed = decompose_copy_constructed;
    expected_move_constructed = decompose_move_constructed;
  }

  {  // insert(first, last)
    DecomposeSet set2;
    set2.insert(elem_vector.begin(), elem_vector.end());
    // Expect exactly N elements copied into set, expect at most 2*N elements
    // moving internally for all resizing needed (for a growth factor of 2).
    const int expected_new_elements = elem_vector_count;
    const int expected_max_element_moves = 2 * elem_vector_count;
    expected_copy_constructed += expected_new_elements;
    EXPECT_EQ(expected_copy_constructed, decompose_copy_constructed);
    expected_move_constructed += expected_max_element_moves;
    EXPECT_GE(expected_move_constructed, decompose_move_constructed);
    EXPECT_EQ(0, decompose_move_assigned);
    EXPECT_EQ(0, decompose_copy_assigned);
    expected_copy_constructed = decompose_copy_constructed;
    expected_move_constructed = decompose_move_constructed;
  }
}

TEST(Table, Decompose) {
  TestDecompose<DecomposeHash, DecomposeEq>(false);

  struct TransparentHashIntOverload {
    size_t operator()(const DecomposeType& a) const { return a.i; }
    size_t operator()(int a) const { return a; }
  };
  struct TransparentEqIntOverload {
    bool operator()(const DecomposeType& a, const DecomposeType& b) const {
      return a.i == b.i;
    }
    bool operator()(const DecomposeType& a, int b) const { return a.i == b; }
  };
  TestDecompose<TransparentHashIntOverload, DecomposeEq>(true);
  TestDecompose<TransparentHashIntOverload, TransparentEqIntOverload>(true);
  TestDecompose<DecomposeHash, TransparentEqIntOverload>(true);
}

// Returns the largest m such that a table with m elements has the same number
// of buckets as a table with n elements.
size_t MaxDensitySize(size_t n) {
  IntTable t;
  t.reserve(n);
  for (size_t i = 0; i != n; ++i) t.emplace(i);
  const size_t c = t.bucket_count();
  while (c == t.bucket_count()) t.emplace(n++);
  return t.size() - 1;
}

struct Modulo1000Hash {
  size_t operator()(int x) const { return x % 1000; }
};

struct Modulo1000HashTable
    : public raw_hash_set<IntPolicy, Modulo1000Hash, std::equal_to<int>,
                          std::allocator<int>> {};

// Test that rehash with no resize happen in case of many deleted slots.
TEST(Table, RehashWithNoResize) {
  Modulo1000HashTable t;
  // Adding the same length (and the same hash) strings
  // to have at least kMinFullGroups groups
  // with Group::kWidth collisions. Then fill up to MaxDensitySize;
  const size_t kMinFullGroups = 7;
  std::vector<int> keys;
  for (size_t i = 0; i < MaxDensitySize(Group::kWidth * kMinFullGroups); ++i) {
    int k = i * 1000;
    t.emplace(k);
    keys.push_back(k);
  }
  const size_t capacity = t.capacity();

  // Remove elements from all groups except the first and the last one.
  // All elements removed from full groups will be marked as ctrl_t::kDeleted.
  const size_t erase_begin = Group::kWidth / 2;
  const size_t erase_end = (t.size() / Group::kWidth - 1) * Group::kWidth;
  for (size_t i = erase_begin; i < erase_end; ++i) {
    EXPECT_EQ(1, t.erase(keys[i])) << i;
  }
  keys.erase(keys.begin() + erase_begin, keys.begin() + erase_end);

  auto last_key = keys.back();
  size_t last_key_num_probes = GetHashtableDebugNumProbes(t, last_key);

  // Make sure that we have to make a lot of probes for last key.
  ASSERT_GT(last_key_num_probes, kMinFullGroups);

  int x = 1;
  // Insert and erase one element, before inplace rehash happen.
  while (last_key_num_probes == GetHashtableDebugNumProbes(t, last_key)) {
    t.emplace(x);
    ASSERT_EQ(capacity, t.capacity());
    // All elements should be there.
    ASSERT_TRUE(t.find(x) != t.end()) << x;
    for (const auto& k : keys) {
      ASSERT_TRUE(t.find(k) != t.end()) << k;
    }
    t.erase(x);
    ++x;
  }
}

TEST(Table, InsertEraseStressTest) {
  IntTable t;
  const size_t kMinElementCount = 250;
  std::deque<int> keys;
  size_t i = 0;
  for (; i < MaxDensitySize(kMinElementCount); ++i) {
    t.emplace(i);
    keys.push_back(i);
  }
  const size_t kNumIterations = 1000000;
  for (; i < kNumIterations; ++i) {
    ASSERT_EQ(1, t.erase(keys.front()));
    keys.pop_front();
    t.emplace(i);
    keys.push_back(i);
  }
}

TEST(Table, InsertOverloads) {
  StringTable t;
  // These should all trigger the insert(init_type) overload.
  t.insert({{}, {}});
  t.insert({"ABC", {}});
  t.insert({"DEF", "!!!"});

  EXPECT_THAT(t, UnorderedElementsAre(Pair("", ""), Pair("ABC", ""),
                                      Pair("DEF", "!!!")));
}

TEST(Table, LargeTable) {
  IntTable t;
  for (int64_t i = 0; i != 100000; ++i) t.emplace(i << 40);
  for (int64_t i = 0; i != 100000; ++i) ASSERT_EQ(i << 40, *t.find(i << 40));
}

// Timeout if copy is quadratic as it was in Rust.
TEST(Table, EnsureNonQuadraticAsInRust) {
  static const size_t kLargeSize = 1 << 15;

  IntTable t;
  for (size_t i = 0; i != kLargeSize; ++i) {
    t.insert(i);
  }

  // If this is quadratic, the test will timeout.
  IntTable t2;
  for (const auto& entry : t) t2.insert(entry);
}

TEST(Table, ClearBug) {
  IntTable t;
  constexpr size_t capacity = container_internal::Group::kWidth - 1;
  constexpr size_t max_size = capacity / 2 + 1;
  for (size_t i = 0; i < max_size; ++i) {
    t.insert(i);
  }
  ASSERT_EQ(capacity, t.capacity());
  intptr_t original = reinterpret_cast<intptr_t>(&*t.find(2));
  t.clear();
  ASSERT_EQ(capacity, t.capacity());
  for (size_t i = 0; i < max_size; ++i) {
    t.insert(i);
  }
  ASSERT_EQ(capacity, t.capacity());
  intptr_t second = reinterpret_cast<intptr_t>(&*t.find(2));
  // We are checking that original and second are close enough to each other
  // that they are probably still in the same group.  This is not strictly
  // guaranteed.
  EXPECT_LT(std::abs(original - second),
            capacity * sizeof(IntTable::value_type));
}

TEST(Table, Erase) {
  IntTable t;
  EXPECT_TRUE(t.find(0) == t.end());
  auto res = t.emplace(0);
  EXPECT_TRUE(res.second);
  EXPECT_EQ(1, t.size());
  t.erase(res.first);
  EXPECT_EQ(0, t.size());
  EXPECT_TRUE(t.find(0) == t.end());
}

TEST(Table, EraseMaintainsValidIterator) {
  IntTable t;
  const int kNumElements = 100;
  for (int i = 0; i < kNumElements; i ++) {
    EXPECT_TRUE(t.emplace(i).second);
  }
  EXPECT_EQ(t.size(), kNumElements);

  int num_erase_calls = 0;
  auto it = t.begin();
  while (it != t.end()) {
    t.erase(it++);
    num_erase_calls++;
  }

  EXPECT_TRUE(t.empty());
  EXPECT_EQ(num_erase_calls, kNumElements);
}

// Collect N bad keys by following algorithm:
// 1. Create an empty table and reserve it to 2 * N.
// 2. Insert N random elements.
// 3. Take first Group::kWidth - 1 to bad_keys array.
// 4. Clear the table without resize.
// 5. Go to point 2 while N keys not collected
std::vector<int64_t> CollectBadMergeKeys(size_t N) {
  static constexpr int kGroupSize = Group::kWidth - 1;

  auto topk_range = [](size_t b, size_t e,
                       IntTable* t) -> std::vector<int64_t> {
    for (size_t i = b; i != e; ++i) {
      t->emplace(i);
    }
    std::vector<int64_t> res;
    res.reserve(kGroupSize);
    auto it = t->begin();
    for (size_t i = b; i != e && i != b + kGroupSize; ++i, ++it) {
      res.push_back(*it);
    }
    return res;
  };

  std::vector<int64_t> bad_keys;
  bad_keys.reserve(N);
  IntTable t;
  t.reserve(N * 2);

  for (size_t b = 0; bad_keys.size() < N; b += N) {
    auto keys = topk_range(b, b + N, &t);
    bad_keys.insert(bad_keys.end(), keys.begin(), keys.end());
    t.erase(t.begin(), t.end());
    EXPECT_TRUE(t.empty());
  }
  return bad_keys;
}

struct ProbeStats {
  // Number of elements with specific probe length over all tested tables.
  std::vector<size_t> all_probes_histogram;
  // Ratios total_probe_length/size for every tested table.
  std::vector<double> single_table_ratios;

  friend ProbeStats operator+(const ProbeStats& a, const ProbeStats& b) {
    ProbeStats res = a;
    res.all_probes_histogram.resize(std::max(res.all_probes_histogram.size(),
                                             b.all_probes_histogram.size()));
    std::transform(b.all_probes_histogram.begin(), b.all_probes_histogram.end(),
                   res.all_probes_histogram.begin(),
                   res.all_probes_histogram.begin(), std::plus<size_t>());
    res.single_table_ratios.insert(res.single_table_ratios.end(),
                                   b.single_table_ratios.begin(),
                                   b.single_table_ratios.end());
    return res;
  }

  // Average ratio total_probe_length/size over tables.
  double AvgRatio() const {
    return std::accumulate(single_table_ratios.begin(),
                           single_table_ratios.end(), 0.0) /
           single_table_ratios.size();
  }

  // Maximum ratio total_probe_length/size over tables.
  double MaxRatio() const {
    return *std::max_element(single_table_ratios.begin(),
                             single_table_ratios.end());
  }

  // Percentile ratio total_probe_length/size over tables.
  double PercentileRatio(double Percentile = 0.95) const {
    auto r = single_table_ratios;
    auto mid = r.begin() + static_cast<size_t>(r.size() * Percentile);
    if (mid != r.end()) {
      std::nth_element(r.begin(), mid, r.end());
      return *mid;
    } else {
      return MaxRatio();
    }
  }

  // Maximum probe length over all elements and all tables.
  size_t MaxProbe() const { return all_probes_histogram.size(); }

  // Fraction of elements with specified probe length.
  std::vector<double> ProbeNormalizedHistogram() const {
    double total_elements = std::accumulate(all_probes_histogram.begin(),
                                            all_probes_histogram.end(), 0ull);
    std::vector<double> res;
    for (size_t p : all_probes_histogram) {
      res.push_back(p / total_elements);
    }
    return res;
  }

  size_t PercentileProbe(double Percentile = 0.99) const {
    size_t idx = 0;
    for (double p : ProbeNormalizedHistogram()) {
      if (Percentile > p) {
        Percentile -= p;
        ++idx;
      } else {
        return idx;
      }
    }
    return idx;
  }

  friend std::ostream& operator<<(std::ostream& out, const ProbeStats& s) {
    out << "{AvgRatio:" << s.AvgRatio() << ", MaxRatio:" << s.MaxRatio()
        << ", PercentileRatio:" << s.PercentileRatio()
        << ", MaxProbe:" << s.MaxProbe() << ", Probes=[";
    for (double p : s.ProbeNormalizedHistogram()) {
      out << p << ",";
    }
    out << "]}";

    return out;
  }
};

struct ExpectedStats {
  double avg_ratio;
  double max_ratio;
  std::vector<std::pair<double, double>> pecentile_ratios;
  std::vector<std::pair<double, double>> pecentile_probes;

  friend std::ostream& operator<<(std::ostream& out, const ExpectedStats& s) {
    out << "{AvgRatio:" << s.avg_ratio << ", MaxRatio:" << s.max_ratio
        << ", PercentileRatios: [";
    for (auto el : s.pecentile_ratios) {
      out << el.first << ":" << el.second << ", ";
    }
    out << "], PercentileProbes: [";
    for (auto el : s.pecentile_probes) {
      out << el.first << ":" << el.second << ", ";
    }
    out << "]}";

    return out;
  }
};

void VerifyStats(size_t size, const ExpectedStats& exp,
                 const ProbeStats& stats) {
  EXPECT_LT(stats.AvgRatio(), exp.avg_ratio) << size << " " << stats;
  EXPECT_LT(stats.MaxRatio(), exp.max_ratio) << size << " " << stats;
  for (auto pr : exp.pecentile_ratios) {
    EXPECT_LE(stats.PercentileRatio(pr.first), pr.second)
        << size << " " << pr.first << " " << stats;
  }

  for (auto pr : exp.pecentile_probes) {
    EXPECT_LE(stats.PercentileProbe(pr.first), pr.second)
        << size << " " << pr.first << " " << stats;
  }
}

using ProbeStatsPerSize = std::map<size_t, ProbeStats>;

// Collect total ProbeStats on num_iters iterations of the following algorithm:
// 1. Create new table and reserve it to keys.size() * 2
// 2. Insert all keys xored with seed
// 3. Collect ProbeStats from final table.
ProbeStats CollectProbeStatsOnKeysXoredWithSeed(
    const std::vector<int64_t>& keys, size_t num_iters) {
  const size_t reserve_size = keys.size() * 2;

  ProbeStats stats;

  int64_t seed = 0x71b1a19b907d6e33;
  while (num_iters--) {
    seed = static_cast<int64_t>(static_cast<uint64_t>(seed) * 17 + 13);
    IntTable t1;
    t1.reserve(reserve_size);
    for (const auto& key : keys) {
      t1.emplace(key ^ seed);
    }

    auto probe_histogram = GetHashtableDebugNumProbesHistogram(t1);
    stats.all_probes_histogram.resize(
        std::max(stats.all_probes_histogram.size(), probe_histogram.size()));
    std::transform(probe_histogram.begin(), probe_histogram.end(),
                   stats.all_probes_histogram.begin(),
                   stats.all_probes_histogram.begin(), std::plus<size_t>());

    size_t total_probe_seq_length = 0;
    for (size_t i = 0; i < probe_histogram.size(); ++i) {
      total_probe_seq_length += i * probe_histogram[i];
    }
    stats.single_table_ratios.push_back(total_probe_seq_length * 1.0 /
                                        keys.size());
    t1.erase(t1.begin(), t1.end());
  }
  return stats;
}

ExpectedStats XorSeedExpectedStats() {
  constexpr bool kRandomizesInserts =
#ifdef NDEBUG
      false;
#else   // NDEBUG
      true;
#endif  // NDEBUG

  // The effective load factor is larger in non-opt mode because we insert
  // elements out of order.
  switch (container_internal::Group::kWidth) {
    case 8:
      if (kRandomizesInserts) {
  return {0.05,
          1.0,
          {{0.95, 0.5}},
          {{0.95, 0}, {0.99, 2}, {0.999, 4}, {0.9999, 10}}};
      } else {
  return {0.05,
          2.0,
          {{0.95, 0.1}},
          {{0.95, 0}, {0.99, 2}, {0.999, 4}, {0.9999, 10}}};
      }
    case 16:
      if (kRandomizesInserts) {
        return {0.1,
                1.0,
                {{0.95, 0.1}},
                {{0.95, 0}, {0.99, 1}, {0.999, 8}, {0.9999, 15}}};
      } else {
        return {0.05,
                1.0,
                {{0.95, 0.05}},
                {{0.95, 0}, {0.99, 1}, {0.999, 4}, {0.9999, 10}}};
      }
  }
  ABSL_RAW_LOG(FATAL, "%s", "Unknown Group width");
  return {};
}

TEST(Table, DISABLED_EnsureNonQuadraticTopNXorSeedByProbeSeqLength) {
  ProbeStatsPerSize stats;
  std::vector<size_t> sizes = {Group::kWidth << 5, Group::kWidth << 10};
  for (size_t size : sizes) {
    stats[size] =
        CollectProbeStatsOnKeysXoredWithSeed(CollectBadMergeKeys(size), 200);
  }
  auto expected = XorSeedExpectedStats();
  for (size_t size : sizes) {
    auto& stat = stats[size];
    VerifyStats(size, expected, stat);
  }
}

// Collect total ProbeStats on num_iters iterations of the following algorithm:
// 1. Create new table
// 2. Select 10% of keys and insert 10 elements key * 17 + j * 13
// 3. Collect ProbeStats from final table
ProbeStats CollectProbeStatsOnLinearlyTransformedKeys(
    const std::vector<int64_t>& keys, size_t num_iters) {
  ProbeStats stats;

  std::random_device rd;
  std::mt19937 rng(rd());
  auto linear_transform = [](size_t x, size_t y) { return x * 17 + y * 13; };
  std::uniform_int_distribution<size_t> dist(0, keys.size()-1);
  while (num_iters--) {
    IntTable t1;
    size_t num_keys = keys.size() / 10;
    size_t start = dist(rng);
    for (size_t i = 0; i != num_keys; ++i) {
      for (size_t j = 0; j != 10; ++j) {
        t1.emplace(linear_transform(keys[(i + start) % keys.size()], j));
      }
    }

    auto probe_histogram = GetHashtableDebugNumProbesHistogram(t1);
    stats.all_probes_histogram.resize(
        std::max(stats.all_probes_histogram.size(), probe_histogram.size()));
    std::transform(probe_histogram.begin(), probe_histogram.end(),
                   stats.all_probes_histogram.begin(),
                   stats.all_probes_histogram.begin(), std::plus<size_t>());

    size_t total_probe_seq_length = 0;
    for (size_t i = 0; i < probe_histogram.size(); ++i) {
      total_probe_seq_length += i * probe_histogram[i];
    }
    stats.single_table_ratios.push_back(total_probe_seq_length * 1.0 /
                                        t1.size());
    t1.erase(t1.begin(), t1.end());
  }
  return stats;
}

ExpectedStats LinearTransformExpectedStats() {
  constexpr bool kRandomizesInserts =
#ifdef NDEBUG
      false;
#else   // NDEBUG
      true;
#endif  // NDEBUG

  // The effective load factor is larger in non-opt mode because we insert
  // elements out of order.
  switch (container_internal::Group::kWidth) {
    case 8:
      if (kRandomizesInserts) {
        return {0.1,
                0.5,
                {{0.95, 0.3}},
                {{0.95, 0}, {0.99, 1}, {0.999, 8}, {0.9999, 15}}};
      } else {
        return {0.15,
                0.5,
                {{0.95, 0.3}},
                {{0.95, 0}, {0.99, 3}, {0.999, 15}, {0.9999, 25}}};
      }
    case 16:
      if (kRandomizesInserts) {
        return {0.1,
                0.4,
                {{0.95, 0.3}},
                {{0.95, 0}, {0.99, 1}, {0.999, 8}, {0.9999, 15}}};
      } else {
        return {0.05,
                0.2,
                {{0.95, 0.1}},
                {{0.95, 0}, {0.99, 1}, {0.999, 6}, {0.9999, 10}}};
      }
  }
  ABSL_RAW_LOG(FATAL, "%s", "Unknown Group width");
  return {};
}

TEST(Table, DISABLED_EnsureNonQuadraticTopNLinearTransformByProbeSeqLength) {
  ProbeStatsPerSize stats;
  std::vector<size_t> sizes = {Group::kWidth << 5, Group::kWidth << 10};
  for (size_t size : sizes) {
    stats[size] = CollectProbeStatsOnLinearlyTransformedKeys(
        CollectBadMergeKeys(size), 300);
  }
  auto expected = LinearTransformExpectedStats();
  for (size_t size : sizes) {
    auto& stat = stats[size];
    VerifyStats(size, expected, stat);
  }
}

TEST(Table, EraseCollision) {
  BadTable t;

  // 1 2 3
  t.emplace(1);
  t.emplace(2);
  t.emplace(3);
  EXPECT_THAT(*t.find(1), 1);
  EXPECT_THAT(*t.find(2), 2);
  EXPECT_THAT(*t.find(3), 3);
  EXPECT_EQ(3, t.size());

  // 1 DELETED 3
  t.erase(t.find(2));
  EXPECT_THAT(*t.find(1), 1);
  EXPECT_TRUE(t.find(2) == t.end());
  EXPECT_THAT(*t.find(3), 3);
  EXPECT_EQ(2, t.size());

  // DELETED DELETED 3
  t.erase(t.find(1));
  EXPECT_TRUE(t.find(1) == t.end());
  EXPECT_TRUE(t.find(2) == t.end());
  EXPECT_THAT(*t.find(3), 3);
  EXPECT_EQ(1, t.size());

  // DELETED DELETED DELETED
  t.erase(t.find(3));
  EXPECT_TRUE(t.find(1) == t.end());
  EXPECT_TRUE(t.find(2) == t.end());
  EXPECT_TRUE(t.find(3) == t.end());
  EXPECT_EQ(0, t.size());
}

TEST(Table, EraseInsertProbing) {
  BadTable t(100);

  // 1 2 3 4
  t.emplace(1);
  t.emplace(2);
  t.emplace(3);
  t.emplace(4);

  // 1 DELETED 3 DELETED
  t.erase(t.find(2));
  t.erase(t.find(4));

  // 1 10 3 11 12
  t.emplace(10);
  t.emplace(11);
  t.emplace(12);

  EXPECT_EQ(5, t.size());
  EXPECT_THAT(t, UnorderedElementsAre(1, 10, 3, 11, 12));
}

TEST(Table, Clear) {
  IntTable t;
  EXPECT_TRUE(t.find(0) == t.end());
  t.clear();
  EXPECT_TRUE(t.find(0) == t.end());
  auto res = t.emplace(0);
  EXPECT_TRUE(res.second);
  EXPECT_EQ(1, t.size());
  t.clear();
  EXPECT_EQ(0, t.size());
  EXPECT_TRUE(t.find(0) == t.end());
}

TEST(Table, Swap) {
  IntTable t;
  EXPECT_TRUE(t.find(0) == t.end());
  auto res = t.emplace(0);
  EXPECT_TRUE(res.second);
  EXPECT_EQ(1, t.size());
  IntTable u;
  t.swap(u);
  EXPECT_EQ(0, t.size());
  EXPECT_EQ(1, u.size());
  EXPECT_TRUE(t.find(0) == t.end());
  EXPECT_THAT(*u.find(0), 0);
}

TEST(Table, Rehash) {
  IntTable t;
  EXPECT_TRUE(t.find(0) == t.end());
  t.emplace(0);
  t.emplace(1);
  EXPECT_EQ(2, t.size());
  t.rehash(128);
  EXPECT_EQ(2, t.size());
  EXPECT_THAT(*t.find(0), 0);
  EXPECT_THAT(*t.find(1), 1);
}

TEST(Table, RehashDoesNotRehashWhenNotNecessary) {
  IntTable t;
  t.emplace(0);
  t.emplace(1);
  auto* p = &*t.find(0);
  t.rehash(1);
  EXPECT_EQ(p, &*t.find(0));
}

TEST(Table, RehashZeroDoesNotAllocateOnEmptyTable) {
  IntTable t;
  t.rehash(0);
  EXPECT_EQ(0, t.bucket_count());
}

TEST(Table, RehashZeroDeallocatesEmptyTable) {
  IntTable t;
  t.emplace(0);
  t.clear();
  EXPECT_NE(0, t.bucket_count());
  t.rehash(0);
  EXPECT_EQ(0, t.bucket_count());
}

TEST(Table, RehashZeroForcesRehash) {
  IntTable t;
  t.emplace(0);
  t.emplace(1);
  auto* p = &*t.find(0);
  t.rehash(0);
  EXPECT_NE(p, &*t.find(0));
}

TEST(Table, ConstructFromInitList) {
  using P = std::pair<std::string, std::string>;
  struct Q {
    operator P() const { return {}; }
  };
  StringTable t = {P(), Q(), {}, {{}, {}}};
}

TEST(Table, CopyConstruct) {
  IntTable t;
  t.emplace(0);
  EXPECT_EQ(1, t.size());
  {
    IntTable u(t);
    EXPECT_EQ(1, u.size());
    EXPECT_THAT(*u.find(0), 0);
  }
  {
    IntTable u{t};
    EXPECT_EQ(1, u.size());
    EXPECT_THAT(*u.find(0), 0);
  }
  {
    IntTable u = t;
    EXPECT_EQ(1, u.size());
    EXPECT_THAT(*u.find(0), 0);
  }
}

TEST(Table, CopyConstructWithAlloc) {
  StringTable t;
  t.emplace("a", "b");
  EXPECT_EQ(1, t.size());
  StringTable u(t, Alloc<std::pair<std::string, std::string>>());
  EXPECT_EQ(1, u.size());
  EXPECT_THAT(*u.find("a"), Pair("a", "b"));
}

struct ExplicitAllocIntTable
    : raw_hash_set<IntPolicy, container_internal::hash_default_hash<int64_t>,
                   std::equal_to<int64_t>, Alloc<int64_t>> {
  ExplicitAllocIntTable() {}
};

TEST(Table, AllocWithExplicitCtor) {
  ExplicitAllocIntTable t;
  EXPECT_EQ(0, t.size());
}

TEST(Table, MoveConstruct) {
  {
    StringTable t;
    t.emplace("a", "b");
    EXPECT_EQ(1, t.size());

    StringTable u(std::move(t));
    EXPECT_EQ(1, u.size());
    EXPECT_THAT(*u.find("a"), Pair("a", "b"));
  }
  {
    StringTable t;
    t.emplace("a", "b");
    EXPECT_EQ(1, t.size());

    StringTable u{std::move(t)};
    EXPECT_EQ(1, u.size());
    EXPECT_THAT(*u.find("a"), Pair("a", "b"));
  }
  {
    StringTable t;
    t.emplace("a", "b");
    EXPECT_EQ(1, t.size());

    StringTable u = std::move(t);
    EXPECT_EQ(1, u.size());
    EXPECT_THAT(*u.find("a"), Pair("a", "b"));
  }
}

TEST(Table, MoveConstructWithAlloc) {
  StringTable t;
  t.emplace("a", "b");
  EXPECT_EQ(1, t.size());
  StringTable u(std::move(t), Alloc<std::pair<std::string, std::string>>());
  EXPECT_EQ(1, u.size());
  EXPECT_THAT(*u.find("a"), Pair("a", "b"));
}

TEST(Table, CopyAssign) {
  StringTable t;
  t.emplace("a", "b");
  EXPECT_EQ(1, t.size());
  StringTable u;
  u = t;
  EXPECT_EQ(1, u.size());
  EXPECT_THAT(*u.find("a"), Pair("a", "b"));
}

TEST(Table, CopySelfAssign) {
  StringTable t;
  t.emplace("a", "b");
  EXPECT_EQ(1, t.size());
  t = *&t;
  EXPECT_EQ(1, t.size());
  EXPECT_THAT(*t.find("a"), Pair("a", "b"));
}

TEST(Table, MoveAssign) {
  StringTable t;
  t.emplace("a", "b");
  EXPECT_EQ(1, t.size());
  StringTable u;
  u = std::move(t);
  EXPECT_EQ(1, u.size());
  EXPECT_THAT(*u.find("a"), Pair("a", "b"));
}

TEST(Table, Equality) {
  StringTable t;
  std::vector<std::pair<std::string, std::string>> v = {{"a", "b"},
                                                        {"aa", "bb"}};
  t.insert(std::begin(v), std::end(v));
  StringTable u = t;
  EXPECT_EQ(u, t);
}

TEST(Table, Equality2) {
  StringTable t;
  std::vector<std::pair<std::string, std::string>> v1 = {{"a", "b"},
                                                         {"aa", "bb"}};
  t.insert(std::begin(v1), std::end(v1));
  StringTable u;
  std::vector<std::pair<std::string, std::string>> v2 = {{"a", "a"},
                                                         {"aa", "aa"}};
  u.insert(std::begin(v2), std::end(v2));
  EXPECT_NE(u, t);
}

TEST(Table, Equality3) {
  StringTable t;
  std::vector<std::pair<std::string, std::string>> v1 = {{"b", "b"},
                                                         {"bb", "bb"}};
  t.insert(std::begin(v1), std::end(v1));
  StringTable u;
  std::vector<std::pair<std::string, std::string>> v2 = {{"a", "a"},
                                                         {"aa", "aa"}};
  u.insert(std::begin(v2), std::end(v2));
  EXPECT_NE(u, t);
}

TEST(Table, NumDeletedRegression) {
  IntTable t;
  t.emplace(0);
  t.erase(t.find(0));
  // construct over a deleted slot.
  t.emplace(0);
  t.clear();
}

TEST(Table, FindFullDeletedRegression) {
  IntTable t;
  for (int i = 0; i < 1000; ++i) {
    t.emplace(i);
    t.erase(t.find(i));
  }
  EXPECT_EQ(0, t.size());
}

TEST(Table, ReplacingDeletedSlotDoesNotRehash) {
  size_t n;
  {
    // Compute n such that n is the maximum number of elements before rehash.
    IntTable t;
    t.emplace(0);
    size_t c = t.bucket_count();
    for (n = 1; c == t.bucket_count(); ++n) t.emplace(n);
    --n;
  }
  IntTable t;
  t.rehash(n);
  const size_t c = t.bucket_count();
  for (size_t i = 0; i != n; ++i) t.emplace(i);
  EXPECT_EQ(c, t.bucket_count()) << "rehashing threshold = " << n;
  t.erase(0);
  t.emplace(0);
  EXPECT_EQ(c, t.bucket_count()) << "rehashing threshold = " << n;
}

TEST(Table, NoThrowMoveConstruct) {
  ASSERT_TRUE(
      std::is_nothrow_copy_constructible<absl::Hash<absl::string_view>>::value);
  ASSERT_TRUE(std::is_nothrow_copy_constructible<
              std::equal_to<absl::string_view>>::value);
  ASSERT_TRUE(std::is_nothrow_copy_constructible<std::allocator<int>>::value);
  EXPECT_TRUE(std::is_nothrow_move_constructible<StringTable>::value);
}

TEST(Table, NoThrowMoveAssign) {
  ASSERT_TRUE(
      std::is_nothrow_move_assignable<absl::Hash<absl::string_view>>::value);
  ASSERT_TRUE(
      std::is_nothrow_move_assignable<std::equal_to<absl::string_view>>::value);
  ASSERT_TRUE(std::is_nothrow_move_assignable<std::allocator<int>>::value);
  ASSERT_TRUE(
      absl::allocator_traits<std::allocator<int>>::is_always_equal::value);
  EXPECT_TRUE(std::is_nothrow_move_assignable<StringTable>::value);
}

TEST(Table, NoThrowSwappable) {
  ASSERT_TRUE(
      container_internal::IsNoThrowSwappable<absl::Hash<absl::string_view>>());
  ASSERT_TRUE(container_internal::IsNoThrowSwappable<
              std::equal_to<absl::string_view>>());
  ASSERT_TRUE(container_internal::IsNoThrowSwappable<std::allocator<int>>());
  EXPECT_TRUE(container_internal::IsNoThrowSwappable<StringTable>());
}

TEST(Table, HeterogeneousLookup) {
  struct Hash {
    size_t operator()(int64_t i) const { return i; }
    size_t operator()(double i) const {
      ADD_FAILURE();
      return i;
    }
  };
  struct Eq {
    bool operator()(int64_t a, int64_t b) const { return a == b; }
    bool operator()(double a, int64_t b) const {
      ADD_FAILURE();
      return a == b;
    }
    bool operator()(int64_t a, double b) const {
      ADD_FAILURE();
      return a == b;
    }
    bool operator()(double a, double b) const {
      ADD_FAILURE();
      return a == b;
    }
  };

  struct THash {
    using is_transparent = void;
    size_t operator()(int64_t i) const { return i; }
    size_t operator()(double i) const { return i; }
  };
  struct TEq {
    using is_transparent = void;
    bool operator()(int64_t a, int64_t b) const { return a == b; }
    bool operator()(double a, int64_t b) const { return a == b; }
    bool operator()(int64_t a, double b) const { return a == b; }
    bool operator()(double a, double b) const { return a == b; }
  };

  raw_hash_set<IntPolicy, Hash, Eq, Alloc<int64_t>> s{0, 1, 2};
  // It will convert to int64_t before the query.
  EXPECT_EQ(1, *s.find(double{1.1}));

  raw_hash_set<IntPolicy, THash, TEq, Alloc<int64_t>> ts{0, 1, 2};
  // It will try to use the double, and fail to find the object.
  EXPECT_TRUE(ts.find(1.1) == ts.end());
}

template <class Table>
using CallFind = decltype(std::declval<Table&>().find(17));

template <class Table>
using CallErase = decltype(std::declval<Table&>().erase(17));

template <class Table>
using CallExtract = decltype(std::declval<Table&>().extract(17));

template <class Table>
using CallPrefetch = decltype(std::declval<Table&>().prefetch(17));

template <class Table>
using CallCount = decltype(std::declval<Table&>().count(17));

template <template <typename> class C, class Table, class = void>
struct VerifyResultOf : std::false_type {};

template <template <typename> class C, class Table>
struct VerifyResultOf<C, Table, absl::void_t<C<Table>>> : std::true_type {};

TEST(Table, HeterogeneousLookupOverloads) {
  using NonTransparentTable =
      raw_hash_set<StringPolicy, absl::Hash<absl::string_view>,
                   std::equal_to<absl::string_view>, std::allocator<int>>;

  EXPECT_FALSE((VerifyResultOf<CallFind, NonTransparentTable>()));
  EXPECT_FALSE((VerifyResultOf<CallErase, NonTransparentTable>()));
  EXPECT_FALSE((VerifyResultOf<CallExtract, NonTransparentTable>()));
  EXPECT_FALSE((VerifyResultOf<CallPrefetch, NonTransparentTable>()));
  EXPECT_FALSE((VerifyResultOf<CallCount, NonTransparentTable>()));

  using TransparentTable = raw_hash_set<
      StringPolicy,
      absl::container_internal::hash_default_hash<absl::string_view>,
      absl::container_internal::hash_default_eq<absl::string_view>,
      std::allocator<int>>;

  EXPECT_TRUE((VerifyResultOf<CallFind, TransparentTable>()));
  EXPECT_TRUE((VerifyResultOf<CallErase, TransparentTable>()));
  EXPECT_TRUE((VerifyResultOf<CallExtract, TransparentTable>()));
  EXPECT_TRUE((VerifyResultOf<CallPrefetch, TransparentTable>()));
  EXPECT_TRUE((VerifyResultOf<CallCount, TransparentTable>()));
}

// TODO(alkis): Expand iterator tests.
TEST(Iterator, IsDefaultConstructible) {
  StringTable::iterator i;
  EXPECT_TRUE(i == StringTable::iterator());
}

TEST(ConstIterator, IsDefaultConstructible) {
  StringTable::const_iterator i;
  EXPECT_TRUE(i == StringTable::const_iterator());
}

TEST(Iterator, ConvertsToConstIterator) {
  StringTable::iterator i;
  EXPECT_TRUE(i == StringTable::const_iterator());
}

TEST(Iterator, Iterates) {
  IntTable t;
  for (size_t i = 3; i != 6; ++i) EXPECT_TRUE(t.emplace(i).second);
  EXPECT_THAT(t, UnorderedElementsAre(3, 4, 5));
}

TEST(Table, Merge) {
  StringTable t1, t2;
  t1.emplace("0", "-0");
  t1.emplace("1", "-1");
  t2.emplace("0", "~0");
  t2.emplace("2", "~2");

  EXPECT_THAT(t1, UnorderedElementsAre(Pair("0", "-0"), Pair("1", "-1")));
  EXPECT_THAT(t2, UnorderedElementsAre(Pair("0", "~0"), Pair("2", "~2")));

  t1.merge(t2);
  EXPECT_THAT(t1, UnorderedElementsAre(Pair("0", "-0"), Pair("1", "-1"),
                                       Pair("2", "~2")));
  EXPECT_THAT(t2, UnorderedElementsAre(Pair("0", "~0")));
}

TEST(Table, IteratorEmplaceConstructibleRequirement) {
  struct Value {
    explicit Value(absl::string_view view) : value(view) {}
    std::string value;

    bool operator==(const Value& other) const { return value == other.value; }
  };
  struct H {
    size_t operator()(const Value& v) const {
      return absl::Hash<std::string>{}(v.value);
    }
  };

  struct Table : raw_hash_set<ValuePolicy<Value>, H, std::equal_to<Value>,
                              std::allocator<Value>> {
    using Base = typename Table::raw_hash_set;
    using Base::Base;
  };

  std::string input[3]{"A", "B", "C"};

  Table t(std::begin(input), std::end(input));
  EXPECT_THAT(t, UnorderedElementsAre(Value{"A"}, Value{"B"}, Value{"C"}));

  input[0] = "D";
  input[1] = "E";
  input[2] = "F";
  t.insert(std::begin(input), std::end(input));
  EXPECT_THAT(t, UnorderedElementsAre(Value{"A"}, Value{"B"}, Value{"C"},
                                      Value{"D"}, Value{"E"}, Value{"F"}));
}

TEST(Nodes, EmptyNodeType) {
  using node_type = StringTable::node_type;
  node_type n;
  EXPECT_FALSE(n);
  EXPECT_TRUE(n.empty());

  EXPECT_TRUE((std::is_same<node_type::allocator_type,
                            StringTable::allocator_type>::value));
}

TEST(Nodes, ExtractInsert) {
  constexpr char k0[] = "Very long string zero.";
  constexpr char k1[] = "Very long string one.";
  constexpr char k2[] = "Very long string two.";
  StringTable t = {{k0, ""}, {k1, ""}, {k2, ""}};
  EXPECT_THAT(t,
              UnorderedElementsAre(Pair(k0, ""), Pair(k1, ""), Pair(k2, "")));

  auto node = t.extract(k0);
  EXPECT_THAT(t, UnorderedElementsAre(Pair(k1, ""), Pair(k2, "")));
  EXPECT_TRUE(node);
  EXPECT_FALSE(node.empty());

  StringTable t2;
  StringTable::insert_return_type res = t2.insert(std::move(node));
  EXPECT_TRUE(res.inserted);
  EXPECT_THAT(*res.position, Pair(k0, ""));
  EXPECT_FALSE(res.node);
  EXPECT_THAT(t2, UnorderedElementsAre(Pair(k0, "")));

  // Not there.
  EXPECT_THAT(t, UnorderedElementsAre(Pair(k1, ""), Pair(k2, "")));
  node = t.extract("Not there!");
  EXPECT_THAT(t, UnorderedElementsAre(Pair(k1, ""), Pair(k2, "")));
  EXPECT_FALSE(node);

  // Inserting nothing.
  res = t2.insert(std::move(node));
  EXPECT_FALSE(res.inserted);
  EXPECT_EQ(res.position, t2.end());
  EXPECT_FALSE(res.node);
  EXPECT_THAT(t2, UnorderedElementsAre(Pair(k0, "")));

  t.emplace(k0, "1");
  node = t.extract(k0);

  // Insert duplicate.
  res = t2.insert(std::move(node));
  EXPECT_FALSE(res.inserted);
  EXPECT_THAT(*res.position, Pair(k0, ""));
  EXPECT_TRUE(res.node);
  EXPECT_FALSE(node);
}

TEST(Nodes, HintInsert) {
  IntTable t = {1, 2, 3};
  auto node = t.extract(1);
  EXPECT_THAT(t, UnorderedElementsAre(2, 3));
  auto it = t.insert(t.begin(), std::move(node));
  EXPECT_THAT(t, UnorderedElementsAre(1, 2, 3));
  EXPECT_EQ(*it, 1);
  EXPECT_FALSE(node);

  node = t.extract(2);
  EXPECT_THAT(t, UnorderedElementsAre(1, 3));
  // reinsert 2 to make the next insert fail.
  t.insert(2);
  EXPECT_THAT(t, UnorderedElementsAre(1, 2, 3));
  it = t.insert(t.begin(), std::move(node));
  EXPECT_EQ(*it, 2);
  // The node was not emptied by the insert call.
  EXPECT_TRUE(node);
}

IntTable MakeSimpleTable(size_t size) {
  IntTable t;
  while (t.size() < size) t.insert(t.size());
  return t;
}

std::vector<int> OrderOfIteration(const IntTable& t) {
  return {t.begin(), t.end()};
}

// These IterationOrderChanges tests depend on non-deterministic behavior.
// We are injecting non-determinism from the pointer of the table, but do so in
// a way that only the page matters. We have to retry enough times to make sure
// we are touching different memory pages to cause the ordering to change.
// We also need to keep the old tables around to avoid getting the same memory
// blocks over and over.
TEST(Table, IterationOrderChangesByInstance) {
  for (size_t size : {2, 6, 12, 20}) {
    const auto reference_table = MakeSimpleTable(size);
    const auto reference = OrderOfIteration(reference_table);

    std::vector<IntTable> tables;
    bool found_difference = false;
    for (int i = 0; !found_difference && i < 5000; ++i) {
      tables.push_back(MakeSimpleTable(size));
      found_difference = OrderOfIteration(tables.back()) != reference;
    }
    if (!found_difference) {
      FAIL()
          << "Iteration order remained the same across many attempts with size "
          << size;
    }
  }
}

TEST(Table, IterationOrderChangesOnRehash) {
  std::vector<IntTable> garbage;
  for (int i = 0; i < 5000; ++i) {
    auto t = MakeSimpleTable(20);
    const auto reference = OrderOfIteration(t);
    // Force rehash to the same size.
    t.rehash(0);
    auto trial = OrderOfIteration(t);
    if (trial != reference) {
      // We are done.
      return;
    }
    garbage.push_back(std::move(t));
  }
  FAIL() << "Iteration order remained the same across many attempts.";
}

// Verify that pointers are invalidated as soon as a second element is inserted.
// This prevents dependency on pointer stability on small tables.
TEST(Table, UnstablePointers) {
  IntTable table;

  const auto addr = [&](int i) {
    return reinterpret_cast<uintptr_t>(&*table.find(i));
  };

  table.insert(0);
  const uintptr_t old_ptr = addr(0);

  // This causes a rehash.
  table.insert(1);

  EXPECT_NE(old_ptr, addr(0));
}

// Confirm that we assert if we try to erase() end().
TEST(TableDeathTest, EraseOfEndAsserts) {
  // Use an assert with side-effects to figure out if they are actually enabled.
  bool assert_enabled = false;
  assert([&]() {
    assert_enabled = true;
    return true;
  }());
  if (!assert_enabled) return;

  IntTable t;
  // Extra simple "regexp" as regexp support is highly varied across platforms.
  constexpr char kDeathMsg[] = "Invalid operation on iterator";
  EXPECT_DEATH_IF_SUPPORTED(t.erase(t.end()), kDeathMsg);
}

#if defined(ABSL_INTERNAL_HASHTABLEZ_SAMPLE)
TEST(RawHashSamplerTest, Sample) {
  // Enable the feature even if the prod default is off.
  SetHashtablezEnabled(true);
  SetHashtablezSampleParameter(100);

  auto& sampler = GlobalHashtablezSampler();
  size_t start_size = 0;
  std::unordered_set<const HashtablezInfo*> preexisting_info;
  start_size += sampler.Iterate([&](const HashtablezInfo& info) {
    preexisting_info.insert(&info);
    ++start_size;
  });

  std::vector<IntTable> tables;
  for (int i = 0; i < 1000000; ++i) {
    tables.emplace_back();

    const bool do_reserve = (i % 10 > 5);
    const bool do_rehash = !do_reserve && (i % 10 > 0);

    if (do_reserve) {
      // Don't reserve on all tables.
      tables.back().reserve(10 * (i % 10));
    }

    tables.back().insert(1);
    tables.back().insert(i % 5);

    if (do_rehash) {
      // Rehash some other tables.
      tables.back().rehash(10 * (i % 10));
    }
  }
  size_t end_size = 0;
  std::unordered_map<size_t, int> observed_checksums;
  std::unordered_map<ssize_t, int> reservations;
  end_size += sampler.Iterate([&](const HashtablezInfo& info) {
    if (preexisting_info.count(&info) == 0) {
      observed_checksums[info.hashes_bitwise_xor.load(
          std::memory_order_relaxed)]++;
      reservations[info.max_reserve.load(std::memory_order_relaxed)]++;
    }
    EXPECT_EQ(info.inline_element_size, sizeof(int64_t));
    ++end_size;
  });

  EXPECT_NEAR((end_size - start_size) / static_cast<double>(tables.size()),
              0.01, 0.005);
  EXPECT_EQ(observed_checksums.size(), 5);
  for (const auto& [_, count] : observed_checksums) {
    EXPECT_NEAR((100 * count) / static_cast<double>(tables.size()), 0.2, 0.05);
  }

  EXPECT_EQ(reservations.size(), 10);
  for (const auto& [reservation, count] : reservations) {
    EXPECT_GE(reservation, 0);
    EXPECT_LT(reservation, 100);

    EXPECT_NEAR((100 * count) / static_cast<double>(tables.size()), 0.1, 0.05)
        << reservation;
  }
}
#endif  // ABSL_INTERNAL_HASHTABLEZ_SAMPLE

TEST(RawHashSamplerTest, DoNotSampleCustomAllocators) {
  // Enable the feature even if the prod default is off.
  SetHashtablezEnabled(true);
  SetHashtablezSampleParameter(100);

  auto& sampler = GlobalHashtablezSampler();
  size_t start_size = 0;
  start_size += sampler.Iterate([&](const HashtablezInfo&) { ++start_size; });

  std::vector<CustomAllocIntTable> tables;
  for (int i = 0; i < 1000000; ++i) {
    tables.emplace_back();
    tables.back().insert(1);
  }
  size_t end_size = 0;
  end_size += sampler.Iterate([&](const HashtablezInfo&) { ++end_size; });

  EXPECT_NEAR((end_size - start_size) / static_cast<double>(tables.size()),
              0.00, 0.001);
}

#ifdef ABSL_HAVE_ADDRESS_SANITIZER
TEST(Sanitizer, PoisoningUnused) {
  IntTable t;
  t.reserve(5);
  // Insert something to force an allocation.
  int64_t& v1 = *t.insert(0).first;

  // Make sure there is something to test.
  ASSERT_GT(t.capacity(), 1);

  int64_t* slots = RawHashSetTestOnlyAccess::GetSlots(t);
  for (size_t i = 0; i < t.capacity(); ++i) {
    EXPECT_EQ(slots + i != &v1, __asan_address_is_poisoned(slots + i));
  }
}

TEST(Sanitizer, PoisoningOnErase) {
  IntTable t;
  int64_t& v = *t.insert(0).first;

  EXPECT_FALSE(__asan_address_is_poisoned(&v));
  t.erase(0);
  EXPECT_TRUE(__asan_address_is_poisoned(&v));
}
#endif  // ABSL_HAVE_ADDRESS_SANITIZER

TEST(Table, AlignOne) {
  // We previously had a bug in which we were copying a control byte over the
  // first slot when alignof(value_type) is 1. We test repeated
  // insertions/erases and verify that the behavior is correct.
  Uint8Table t;
  std::unordered_set<uint8_t> verifier;  // NOLINT

  // Do repeated insertions/erases from the table.
  for (int64_t i = 0; i < 100000; ++i) {
    SCOPED_TRACE(i);
    const uint8_t u = (i * -i) & 0xFF;
    auto it = t.find(u);
    auto verifier_it = verifier.find(u);
    if (it == t.end()) {
      ASSERT_EQ(verifier_it, verifier.end());
      t.insert(u);
      verifier.insert(u);
    } else {
      ASSERT_NE(verifier_it, verifier.end());
      t.erase(it);
      verifier.erase(verifier_it);
    }
  }

  EXPECT_EQ(t.size(), verifier.size());
  for (uint8_t u : t) {
    EXPECT_EQ(verifier.count(u), 1);
  }
}

}  // namespace
}  // namespace container_internal
ABSL_NAMESPACE_END
}  // namespace absl
