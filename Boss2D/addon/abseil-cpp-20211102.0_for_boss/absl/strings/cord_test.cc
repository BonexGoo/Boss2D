// Copyright 2020 The Abseil Authors.
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

#include "absl/strings/cord.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <iterator>
#include <map>
#include <numeric>
#include <random>
#include <sstream>
#include <type_traits>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include BOSS_ABSEILCPP_U_absl__base__casts_h //original-code:"absl/base/casts.h"
#include BOSS_ABSEILCPP_U_absl__base__config_h //original-code:"absl/base/config.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__endian_h //original-code:"absl/base/internal/endian.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__raw_logging_h //original-code:"absl/base/internal/raw_logging.h"
#include BOSS_ABSEILCPP_U_absl__base__macros_h //original-code:"absl/base/macros.h"
#include BOSS_ABSEILCPP_U_absl__container__fixed_array_h //original-code:"absl/container/fixed_array.h"
#include "absl/random/random.h"
#include "absl/strings/cord_test_helpers.h"
#include "absl/strings/cordz_test_helpers.h"
#include BOSS_ABSEILCPP_U_absl__strings__str_cat_h //original-code:"absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"

typedef std::mt19937_64 RandomEngine;

static std::string RandomLowercaseString(RandomEngine* rng);
static std::string RandomLowercaseString(RandomEngine* rng, size_t length);

static int GetUniformRandomUpTo(RandomEngine* rng, int upper_bound) {
  if (upper_bound > 0) {
    std::uniform_int_distribution<int> uniform(0, upper_bound - 1);
    return uniform(*rng);
  } else {
    return 0;
  }
}

static size_t GetUniformRandomUpTo(RandomEngine* rng, size_t upper_bound) {
  if (upper_bound > 0) {
    std::uniform_int_distribution<size_t> uniform(0, upper_bound - 1);
    return uniform(*rng);
  } else {
    return 0;
  }
}

static int32_t GenerateSkewedRandom(RandomEngine* rng, int max_log) {
  const uint32_t base = (*rng)() % (max_log + 1);
  const uint32_t mask = ((base < 32) ? (1u << base) : 0u) - 1u;
  return (*rng)() & mask;
}

static std::string RandomLowercaseString(RandomEngine* rng) {
  int length;
  std::bernoulli_distribution one_in_1k(0.001);
  std::bernoulli_distribution one_in_10k(0.0001);
  // With low probability, make a large fragment
  if (one_in_10k(*rng)) {
    length = GetUniformRandomUpTo(rng, 1048576);
  } else if (one_in_1k(*rng)) {
    length = GetUniformRandomUpTo(rng, 10000);
  } else {
    length = GenerateSkewedRandom(rng, 10);
  }
  return RandomLowercaseString(rng, length);
}

static std::string RandomLowercaseString(RandomEngine* rng, size_t length) {
  std::string result(length, '\0');
  std::uniform_int_distribution<int> chars('a', 'z');
  std::generate(result.begin(), result.end(),
                [&]() { return static_cast<char>(chars(*rng)); });
  return result;
}

static void DoNothing(absl::string_view /* data */, void* /* arg */) {}

static void DeleteExternalString(absl::string_view data, void* arg) {
  std::string* s = reinterpret_cast<std::string*>(arg);
  EXPECT_EQ(data, *s);
  delete s;
}

// Add "s" to *dst via `MakeCordFromExternal`
static void AddExternalMemory(absl::string_view s, absl::Cord* dst) {
  std::string* str = new std::string(s.data(), s.size());
  dst->Append(absl::MakeCordFromExternal(*str, [str](absl::string_view data) {
    DeleteExternalString(data, str);
  }));
}

static void DumpGrowth() {
  absl::Cord str;
  for (int i = 0; i < 1000; i++) {
    char c = 'a' + i % 26;
    str.Append(absl::string_view(&c, 1));
  }
}

// Make a Cord with some number of fragments.  Return the size (in bytes)
// of the smallest fragment.
static size_t AppendWithFragments(const std::string& s, RandomEngine* rng,
                                  absl::Cord* cord) {
  size_t j = 0;
  const size_t max_size = s.size() / 5;  // Make approx. 10 fragments
  size_t min_size = max_size;            // size of smallest fragment
  while (j < s.size()) {
    size_t N = 1 + GetUniformRandomUpTo(rng, max_size);
    if (N > (s.size() - j)) {
      N = s.size() - j;
    }
    if (N < min_size) {
      min_size = N;
    }

    std::bernoulli_distribution coin_flip(0.5);
    if (coin_flip(*rng)) {
      // Grow by adding an external-memory.
      AddExternalMemory(absl::string_view(s.data() + j, N), cord);
    } else {
      cord->Append(absl::string_view(s.data() + j, N));
    }
    j += N;
  }
  return min_size;
}

// Add an external memory that contains the specified std::string to cord
static void AddNewStringBlock(const std::string& str, absl::Cord* dst) {
  char* data = new char[str.size()];
  memcpy(data, str.data(), str.size());
  dst->Append(absl::MakeCordFromExternal(
      absl::string_view(data, str.size()),
      [](absl::string_view s) { delete[] s.data(); }));
}

// Make a Cord out of many different types of nodes.
static absl::Cord MakeComposite() {
  absl::Cord cord;
  cord.Append("the");
  AddExternalMemory(" quick brown", &cord);
  AddExternalMemory(" fox jumped", &cord);

  absl::Cord full(" over");
  AddExternalMemory(" the lazy", &full);
  AddNewStringBlock(" dog slept the whole day away", &full);
  absl::Cord substring = full.Subcord(0, 18);

  // Make substring long enough to defeat the copying fast path in Append.
  substring.Append(std::string(1000, '.'));
  cord.Append(substring);
  cord = cord.Subcord(0, cord.size() - 998);  // Remove most of extra junk

  return cord;
}

namespace absl {
ABSL_NAMESPACE_BEGIN

class CordTestPeer {
 public:
  static void ForEachChunk(
      const Cord& c, absl::FunctionRef<void(absl::string_view)> callback) {
    c.ForEachChunk(callback);
  }

  static bool IsTree(const Cord& c) { return c.contents_.is_tree(); }

  static cord_internal::CordzInfo* GetCordzInfo(const Cord& c) {
    return c.contents_.cordz_info();
  }

  static Cord MakeSubstring(Cord src, size_t offset, size_t length) {
    ABSL_RAW_CHECK(src.contents_.is_tree(), "Can not be inlined");
    Cord cord;
    auto* rep = new cord_internal::CordRepSubstring;
    rep->tag = cord_internal::SUBSTRING;
    rep->child = cord_internal::CordRep::Ref(src.contents_.tree());
    rep->start = offset;
    rep->length = length;
    cord.contents_.EmplaceTree(rep,
                               cord_internal::CordzUpdateTracker::kSubCord);
    return cord;
  }
};

ABSL_NAMESPACE_END
}  // namespace absl

// The CordTest fixture runs all tests with and without Cord Btree enabled.
class CordTest : public testing::TestWithParam<bool> {
 public:
  CordTest() : was_btree_(absl::cord_internal::cord_btree_enabled.load()) {
    absl::cord_internal::cord_btree_enabled.store(UseBtree());
  }
  ~CordTest() override {
    absl::cord_internal::cord_btree_enabled.store(was_btree_);
  }

  // Returns true if test is running with btree enabled.
  bool UseBtree() const { return GetParam(); }

  // Returns human readable string representation of the test parameter.
  static std::string ToString(testing::TestParamInfo<bool> param) {
    return param.param ? "Btree" : "Concat";
  }

 private:
  const bool was_btree_;
};

INSTANTIATE_TEST_SUITE_P(WithParam, CordTest, testing::Bool(),
                         CordTest::ToString);

TEST_P(CordTest, AllFlatSizes) {
  using absl::strings_internal::CordTestAccess;

  for (size_t s = 0; s < CordTestAccess::MaxFlatLength(); s++) {
    // Make a string of length s.
    std::string src;
    while (src.size() < s) {
      src.push_back('a' + (src.size() % 26));
    }

    absl::Cord dst(src);
    EXPECT_EQ(std::string(dst), src) << s;
  }
}

// We create a Cord at least 128GB in size using the fact that Cords can
// internally reference-count; thus the Cord is enormous without actually
// consuming very much memory.
TEST_P(CordTest, GigabyteCordFromExternal) {
  const size_t one_gig = 1024U * 1024U * 1024U;
  size_t max_size = 2 * one_gig;
  if (sizeof(max_size) > 4) max_size = 128 * one_gig;

  size_t length = 128 * 1024;
  char* data = new char[length];
  absl::Cord from = absl::MakeCordFromExternal(
      absl::string_view(data, length),
      [](absl::string_view sv) { delete[] sv.data(); });

  // This loop may seem odd due to its combination of exponential doubling of
  // size and incremental size increases.  We do it incrementally to be sure the
  // Cord will need rebalancing and will exercise code that, in the past, has
  // caused crashes in production.  We grow exponentially so that the code will
  // execute in a reasonable amount of time.
  absl::Cord c;
  c.Append(from);
  while (c.size() < max_size) {
    c.Append(c);
    c.Append(from);
    c.Append(from);
    c.Append(from);
    c.Append(from);
  }

  for (int i = 0; i < 1024; ++i) {
    c.Append(from);
  }
  ABSL_RAW_LOG(INFO, "Made a Cord with %zu bytes!", c.size());
  // Note: on a 32-bit build, this comes out to   2,818,048,000 bytes.
  // Note: on a 64-bit build, this comes out to 171,932,385,280 bytes.
}

static absl::Cord MakeExternalCord(int size) {
  char* buffer = new char[size];
  memset(buffer, 'x', size);
  absl::Cord cord;
  cord.Append(absl::MakeCordFromExternal(
      absl::string_view(buffer, size),
      [](absl::string_view s) { delete[] s.data(); }));
  return cord;
}

// Extern to fool clang that this is not constant. Needed to suppress
// a warning of unsafe code we want to test.
extern bool my_unique_true_boolean;
bool my_unique_true_boolean = true;

TEST_P(CordTest, Assignment) {
  absl::Cord x(absl::string_view("hi there"));
  absl::Cord y(x);
  ASSERT_EQ(std::string(x), "hi there");
  ASSERT_EQ(std::string(y), "hi there");
  ASSERT_TRUE(x == y);
  ASSERT_TRUE(x <= y);
  ASSERT_TRUE(y <= x);

  x = absl::string_view("foo");
  ASSERT_EQ(std::string(x), "foo");
  ASSERT_EQ(std::string(y), "hi there");
  ASSERT_TRUE(x < y);
  ASSERT_TRUE(y > x);
  ASSERT_TRUE(x != y);
  ASSERT_TRUE(x <= y);
  ASSERT_TRUE(y >= x);

  x = "foo";
  ASSERT_EQ(x, "foo");

  // Test that going from inline rep to tree we don't leak memory.
  std::vector<std::pair<absl::string_view, absl::string_view>>
      test_string_pairs = {{"hi there", "foo"},
                           {"loooooong coooooord", "short cord"},
                           {"short cord", "loooooong coooooord"},
                           {"loooooong coooooord1", "loooooong coooooord2"}};
  for (std::pair<absl::string_view, absl::string_view> test_strings :
       test_string_pairs) {
    absl::Cord tmp(test_strings.first);
    absl::Cord z(std::move(tmp));
    ASSERT_EQ(std::string(z), test_strings.first);
    tmp = test_strings.second;
    z = std::move(tmp);
    ASSERT_EQ(std::string(z), test_strings.second);
  }
  {
    // Test that self-move assignment doesn't crash/leak.
    // Do not write such code!
    absl::Cord my_small_cord("foo");
    absl::Cord my_big_cord("loooooong coooooord");
    // Bypass clang's warning on self move-assignment.
    absl::Cord* my_small_alias =
        my_unique_true_boolean ? &my_small_cord : &my_big_cord;
    absl::Cord* my_big_alias =
        !my_unique_true_boolean ? &my_small_cord : &my_big_cord;

    *my_small_alias = std::move(my_small_cord);
    *my_big_alias = std::move(my_big_cord);
    // my_small_cord and my_big_cord are in an unspecified but valid
    // state, and will be correctly destroyed here.
  }
}

TEST_P(CordTest, StartsEndsWith) {
  absl::Cord x(absl::string_view("abcde"));
  absl::Cord empty("");

  ASSERT_TRUE(x.StartsWith(absl::Cord("abcde")));
  ASSERT_TRUE(x.StartsWith(absl::Cord("abc")));
  ASSERT_TRUE(x.StartsWith(absl::Cord("")));
  ASSERT_TRUE(empty.StartsWith(absl::Cord("")));
  ASSERT_TRUE(x.EndsWith(absl::Cord("abcde")));
  ASSERT_TRUE(x.EndsWith(absl::Cord("cde")));
  ASSERT_TRUE(x.EndsWith(absl::Cord("")));
  ASSERT_TRUE(empty.EndsWith(absl::Cord("")));

  ASSERT_TRUE(!x.StartsWith(absl::Cord("xyz")));
  ASSERT_TRUE(!empty.StartsWith(absl::Cord("xyz")));
  ASSERT_TRUE(!x.EndsWith(absl::Cord("xyz")));
  ASSERT_TRUE(!empty.EndsWith(absl::Cord("xyz")));

  ASSERT_TRUE(x.StartsWith("abcde"));
  ASSERT_TRUE(x.StartsWith("abc"));
  ASSERT_TRUE(x.StartsWith(""));
  ASSERT_TRUE(empty.StartsWith(""));
  ASSERT_TRUE(x.EndsWith("abcde"));
  ASSERT_TRUE(x.EndsWith("cde"));
  ASSERT_TRUE(x.EndsWith(""));
  ASSERT_TRUE(empty.EndsWith(""));

  ASSERT_TRUE(!x.StartsWith("xyz"));
  ASSERT_TRUE(!empty.StartsWith("xyz"));
  ASSERT_TRUE(!x.EndsWith("xyz"));
  ASSERT_TRUE(!empty.EndsWith("xyz"));
}

TEST_P(CordTest, Subcord) {
  RandomEngine rng(GTEST_FLAG_GET(random_seed));
  const std::string s = RandomLowercaseString(&rng, 1024);

  absl::Cord a;
  AppendWithFragments(s, &rng, &a);
  ASSERT_EQ(s, std::string(a));

  // Check subcords of a, from a variety of interesting points.
  std::set<size_t> positions;
  for (int i = 0; i <= 32; ++i) {
    positions.insert(i);
    positions.insert(i * 32 - 1);
    positions.insert(i * 32);
    positions.insert(i * 32 + 1);
    positions.insert(a.size() - i);
  }
  positions.insert(237);
  positions.insert(732);
  for (size_t pos : positions) {
    if (pos > a.size()) continue;
    for (size_t end_pos : positions) {
      if (end_pos < pos || end_pos > a.size()) continue;
      absl::Cord sa = a.Subcord(pos, end_pos - pos);
      ASSERT_EQ(absl::string_view(s).substr(pos, end_pos - pos),
                std::string(sa))
          << a;
    }
  }

  // Do the same thing for an inline cord.
  const std::string sh = "short";
  absl::Cord c(sh);
  for (size_t pos = 0; pos <= sh.size(); ++pos) {
    for (size_t n = 0; n <= sh.size() - pos; ++n) {
      absl::Cord sc = c.Subcord(pos, n);
      ASSERT_EQ(sh.substr(pos, n), std::string(sc)) << c;
    }
  }

  // Check subcords of subcords.
  absl::Cord sa = a.Subcord(0, a.size());
  std::string ss = s.substr(0, s.size());
  while (sa.size() > 1) {
    sa = sa.Subcord(1, sa.size() - 2);
    ss = ss.substr(1, ss.size() - 2);
    ASSERT_EQ(ss, std::string(sa)) << a;
    if (HasFailure()) break;  // halt cascade
  }

  // It is OK to ask for too much.
  sa = a.Subcord(0, a.size() + 1);
  EXPECT_EQ(s, std::string(sa));

  // It is OK to ask for something beyond the end.
  sa = a.Subcord(a.size() + 1, 0);
  EXPECT_TRUE(sa.empty());
  sa = a.Subcord(a.size() + 1, 1);
  EXPECT_TRUE(sa.empty());
}

TEST_P(CordTest, Swap) {
  absl::string_view a("Dexter");
  absl::string_view b("Mandark");
  absl::Cord x(a);
  absl::Cord y(b);
  swap(x, y);
  ASSERT_EQ(x, absl::Cord(b));
  ASSERT_EQ(y, absl::Cord(a));
  x.swap(y);
  ASSERT_EQ(x, absl::Cord(a));
  ASSERT_EQ(y, absl::Cord(b));
}

static void VerifyCopyToString(const absl::Cord& cord) {
  std::string initially_empty;
  absl::CopyCordToString(cord, &initially_empty);
  EXPECT_EQ(initially_empty, cord);

  constexpr size_t kInitialLength = 1024;
  std::string has_initial_contents(kInitialLength, 'x');
  const char* address_before_copy = has_initial_contents.data();
  absl::CopyCordToString(cord, &has_initial_contents);
  EXPECT_EQ(has_initial_contents, cord);

  if (cord.size() <= kInitialLength) {
    EXPECT_EQ(has_initial_contents.data(), address_before_copy)
        << "CopyCordToString allocated new string storage; "
           "has_initial_contents = \""
        << has_initial_contents << "\"";
  }
}

TEST_P(CordTest, CopyToString) {
  VerifyCopyToString(absl::Cord());
  VerifyCopyToString(absl::Cord("small cord"));
  VerifyCopyToString(
      absl::MakeFragmentedCord({"fragmented ", "cord ", "to ", "test ",
                                "copying ", "to ", "a ", "string."}));
}

TEST_P(CordTest, TryFlatEmpty) {
  absl::Cord c;
  EXPECT_EQ(c.TryFlat(), "");
}

TEST_P(CordTest, TryFlatFlat) {
  absl::Cord c("hello");
  EXPECT_EQ(c.TryFlat(), "hello");
}

TEST_P(CordTest, TryFlatSubstrInlined) {
  absl::Cord c("hello");
  c.RemovePrefix(1);
  EXPECT_EQ(c.TryFlat(), "ello");
}

TEST_P(CordTest, TryFlatSubstrFlat) {
  absl::Cord c("longer than 15 bytes");
  absl::Cord sub = absl::CordTestPeer::MakeSubstring(c, 1, c.size() - 1);
  EXPECT_EQ(sub.TryFlat(), "onger than 15 bytes");
}

TEST_P(CordTest, TryFlatConcat) {
  absl::Cord c = absl::MakeFragmentedCord({"hel", "lo"});
  EXPECT_EQ(c.TryFlat(), absl::nullopt);
}

TEST_P(CordTest, TryFlatExternal) {
  absl::Cord c = absl::MakeCordFromExternal("hell", [](absl::string_view) {});
  EXPECT_EQ(c.TryFlat(), "hell");
}

TEST_P(CordTest, TryFlatSubstrExternal) {
  absl::Cord c = absl::MakeCordFromExternal("hell", [](absl::string_view) {});
  absl::Cord sub = absl::CordTestPeer::MakeSubstring(c, 1, c.size() - 1);
  EXPECT_EQ(sub.TryFlat(), "ell");
}

TEST_P(CordTest, TryFlatSubstrConcat) {
  absl::Cord c = absl::MakeFragmentedCord({"hello", " world"});
  absl::Cord sub = absl::CordTestPeer::MakeSubstring(c, 1, c.size() - 1);
  EXPECT_EQ(sub.TryFlat(), absl::nullopt);
  c.RemovePrefix(1);
  EXPECT_EQ(c.TryFlat(), absl::nullopt);
}

TEST_P(CordTest, TryFlatCommonlyAssumedInvariants) {
  // The behavior tested below is not part of the API contract of Cord, but it's
  // something we intend to be true in our current implementation.  This test
  // exists to detect and prevent accidental breakage of the implementation.
  absl::string_view fragments[] = {"A fragmented test",
                                   " cord",
                                   " to test subcords",
                                   " of ",
                                   "a",
                                   " cord for",
                                   " each chunk "
                                   "returned by the ",
                                   "iterator"};
  absl::Cord c = absl::MakeFragmentedCord(fragments);
  int fragment = 0;
  int offset = 0;
  absl::Cord::CharIterator itc = c.char_begin();
  for (absl::string_view sv : c.Chunks()) {
    absl::string_view expected = fragments[fragment];
    absl::Cord subcord1 = c.Subcord(offset, sv.length());
    absl::Cord subcord2 = absl::Cord::AdvanceAndRead(&itc, sv.size());
    EXPECT_EQ(subcord1.TryFlat(), expected);
    EXPECT_EQ(subcord2.TryFlat(), expected);
    ++fragment;
    offset += sv.length();
  }
}

static bool IsFlat(const absl::Cord& c) {
  return c.chunk_begin() == c.chunk_end() || ++c.chunk_begin() == c.chunk_end();
}

static void VerifyFlatten(absl::Cord c) {
  std::string old_contents(c);
  absl::string_view old_flat;
  bool already_flat_and_non_empty = IsFlat(c) && !c.empty();
  if (already_flat_and_non_empty) {
    old_flat = *c.chunk_begin();
  }
  absl::string_view new_flat = c.Flatten();

  // Verify that the contents of the flattened Cord are correct.
  EXPECT_EQ(new_flat, old_contents);
  EXPECT_EQ(std::string(c), old_contents);

  // If the Cord contained data and was already flat, verify that the data
  // wasn't copied.
  if (already_flat_and_non_empty) {
    EXPECT_EQ(old_flat.data(), new_flat.data())
        << "Allocated new memory even though the Cord was already flat.";
  }

  // Verify that the flattened Cord is in fact flat.
  EXPECT_TRUE(IsFlat(c));
}

TEST_P(CordTest, Flatten) {
  VerifyFlatten(absl::Cord());
  VerifyFlatten(absl::Cord("small cord"));
  VerifyFlatten(absl::Cord("larger than small buffer optimization"));
  VerifyFlatten(absl::MakeFragmentedCord({"small ", "fragmented ", "cord"}));

  // Test with a cord that is longer than the largest flat buffer
  RandomEngine rng(GTEST_FLAG_GET(random_seed));
  VerifyFlatten(absl::Cord(RandomLowercaseString(&rng, 8192)));
}

// Test data
namespace {
class TestData {
 private:
  std::vector<std::string> data_;

  // Return a std::string of the specified length.
  static std::string MakeString(int length) {
    std::string result;
    char buf[30];
    snprintf(buf, sizeof(buf), "(%d)", length);
    while (result.size() < length) {
      result += buf;
    }
    result.resize(length);
    return result;
  }

 public:
  TestData() {
    // short strings increasing in length by one
    for (int i = 0; i < 30; i++) {
      data_.push_back(MakeString(i));
    }

    // strings around half kMaxFlatLength
    static const int kMaxFlatLength = 4096 - 9;
    static const int kHalf = kMaxFlatLength / 2;

    for (int i = -10; i <= +10; i++) {
      data_.push_back(MakeString(kHalf + i));
    }

    for (int i = -10; i <= +10; i++) {
      data_.push_back(MakeString(kMaxFlatLength + i));
    }
  }

  size_t size() const { return data_.size(); }
  const std::string& data(size_t i) const { return data_[i]; }
};
}  // namespace

TEST_P(CordTest, MultipleLengths) {
  TestData d;
  for (size_t i = 0; i < d.size(); i++) {
    std::string a = d.data(i);

    {  // Construct from Cord
      absl::Cord tmp(a);
      absl::Cord x(tmp);
      EXPECT_EQ(a, std::string(x)) << "'" << a << "'";
    }

    {  // Construct from absl::string_view
      absl::Cord x(a);
      EXPECT_EQ(a, std::string(x)) << "'" << a << "'";
    }

    {  // Append cord to self
      absl::Cord self(a);
      self.Append(self);
      EXPECT_EQ(a + a, std::string(self)) << "'" << a << "' + '" << a << "'";
    }

    {  // Prepend cord to self
      absl::Cord self(a);
      self.Prepend(self);
      EXPECT_EQ(a + a, std::string(self)) << "'" << a << "' + '" << a << "'";
    }

    // Try to append/prepend others
    for (size_t j = 0; j < d.size(); j++) {
      std::string b = d.data(j);

      {  // CopyFrom Cord
        absl::Cord x(a);
        absl::Cord y(b);
        x = y;
        EXPECT_EQ(b, std::string(x)) << "'" << a << "' + '" << b << "'";
      }

      {  // CopyFrom absl::string_view
        absl::Cord x(a);
        x = b;
        EXPECT_EQ(b, std::string(x)) << "'" << a << "' + '" << b << "'";
      }

      {  // Cord::Append(Cord)
        absl::Cord x(a);
        absl::Cord y(b);
        x.Append(y);
        EXPECT_EQ(a + b, std::string(x)) << "'" << a << "' + '" << b << "'";
      }

      {  // Cord::Append(absl::string_view)
        absl::Cord x(a);
        x.Append(b);
        EXPECT_EQ(a + b, std::string(x)) << "'" << a << "' + '" << b << "'";
      }

      {  // Cord::Prepend(Cord)
        absl::Cord x(a);
        absl::Cord y(b);
        x.Prepend(y);
        EXPECT_EQ(b + a, std::string(x)) << "'" << b << "' + '" << a << "'";
      }

      {  // Cord::Prepend(absl::string_view)
        absl::Cord x(a);
        x.Prepend(b);
        EXPECT_EQ(b + a, std::string(x)) << "'" << b << "' + '" << a << "'";
      }
    }
  }
}

namespace {

TEST_P(CordTest, RemoveSuffixWithExternalOrSubstring) {
  absl::Cord cord = absl::MakeCordFromExternal(
      "foo bar baz", [](absl::string_view s) { DoNothing(s, nullptr); });

  EXPECT_EQ("foo bar baz", std::string(cord));

  // This RemoveSuffix() will wrap the EXTERNAL node in a SUBSTRING node.
  cord.RemoveSuffix(4);
  EXPECT_EQ("foo bar", std::string(cord));

  // This RemoveSuffix() will adjust the SUBSTRING node in-place.
  cord.RemoveSuffix(4);
  EXPECT_EQ("foo", std::string(cord));
}

TEST_P(CordTest, RemoveSuffixMakesZeroLengthNode) {
  absl::Cord c;
  c.Append(absl::Cord(std::string(100, 'x')));
  absl::Cord other_ref = c;  // Prevent inplace appends
  c.Append(absl::Cord(std::string(200, 'y')));
  c.RemoveSuffix(200);
  EXPECT_EQ(std::string(100, 'x'), std::string(c));
}

}  // namespace

// CordSpliceTest contributed by hendrie.
namespace {

// Create a cord with an external memory block filled with 'z'
absl::Cord CordWithZedBlock(size_t size) {
  char* data = new char[size];
  if (size > 0) {
    memset(data, 'z', size);
  }
  absl::Cord cord = absl::MakeCordFromExternal(
      absl::string_view(data, size),
      [](absl::string_view s) { delete[] s.data(); });
  return cord;
}

// Establish that ZedBlock does what we think it does.
TEST_P(CordTest, CordSpliceTestZedBlock) {
  absl::Cord blob = CordWithZedBlock(10);
  EXPECT_EQ(10, blob.size());
  std::string s;
  absl::CopyCordToString(blob, &s);
  EXPECT_EQ("zzzzzzzzzz", s);
}

TEST_P(CordTest, CordSpliceTestZedBlock0) {
  absl::Cord blob = CordWithZedBlock(0);
  EXPECT_EQ(0, blob.size());
  std::string s;
  absl::CopyCordToString(blob, &s);
  EXPECT_EQ("", s);
}

TEST_P(CordTest, CordSpliceTestZedBlockSuffix1) {
  absl::Cord blob = CordWithZedBlock(10);
  EXPECT_EQ(10, blob.size());
  absl::Cord suffix(blob);
  suffix.RemovePrefix(9);
  EXPECT_EQ(1, suffix.size());
  std::string s;
  absl::CopyCordToString(suffix, &s);
  EXPECT_EQ("z", s);
}

// Remove all of a prefix block
TEST_P(CordTest, CordSpliceTestZedBlockSuffix0) {
  absl::Cord blob = CordWithZedBlock(10);
  EXPECT_EQ(10, blob.size());
  absl::Cord suffix(blob);
  suffix.RemovePrefix(10);
  EXPECT_EQ(0, suffix.size());
  std::string s;
  absl::CopyCordToString(suffix, &s);
  EXPECT_EQ("", s);
}

absl::Cord BigCord(size_t len, char v) {
  std::string s(len, v);
  return absl::Cord(s);
}

// Splice block into cord.
absl::Cord SpliceCord(const absl::Cord& blob, int64_t offset,
                      const absl::Cord& block) {
  ABSL_RAW_CHECK(offset >= 0, "");
  ABSL_RAW_CHECK(offset + block.size() <= blob.size(), "");
  absl::Cord result(blob);
  result.RemoveSuffix(blob.size() - offset);
  result.Append(block);
  absl::Cord suffix(blob);
  suffix.RemovePrefix(offset + block.size());
  result.Append(suffix);
  ABSL_RAW_CHECK(blob.size() == result.size(), "");
  return result;
}

// Taking an empty suffix of a block breaks appending.
TEST_P(CordTest, CordSpliceTestRemoveEntireBlock1) {
  absl::Cord zero = CordWithZedBlock(10);
  absl::Cord suffix(zero);
  suffix.RemovePrefix(10);
  absl::Cord result;
  result.Append(suffix);
}

TEST_P(CordTest, CordSpliceTestRemoveEntireBlock2) {
  absl::Cord zero = CordWithZedBlock(10);
  absl::Cord prefix(zero);
  prefix.RemoveSuffix(10);
  absl::Cord suffix(zero);
  suffix.RemovePrefix(10);
  absl::Cord result(prefix);
  result.Append(suffix);
}

TEST_P(CordTest, CordSpliceTestRemoveEntireBlock3) {
  absl::Cord blob = CordWithZedBlock(10);
  absl::Cord block = BigCord(10, 'b');
  blob = SpliceCord(blob, 0, block);
}

struct CordCompareTestCase {
  template <typename LHS, typename RHS>
  CordCompareTestCase(const LHS& lhs, const RHS& rhs)
      : lhs_cord(lhs), rhs_cord(rhs) {}

  absl::Cord lhs_cord;
  absl::Cord rhs_cord;
};

const auto sign = [](int x) { return x == 0 ? 0 : (x > 0 ? 1 : -1); };

void VerifyComparison(const CordCompareTestCase& test_case) {
  std::string lhs_string(test_case.lhs_cord);
  std::string rhs_string(test_case.rhs_cord);
  int expected = sign(lhs_string.compare(rhs_string));
  EXPECT_EQ(expected, test_case.lhs_cord.Compare(test_case.rhs_cord))
      << "LHS=" << lhs_string << "; RHS=" << rhs_string;
  EXPECT_EQ(expected, test_case.lhs_cord.Compare(rhs_string))
      << "LHS=" << lhs_string << "; RHS=" << rhs_string;
  EXPECT_EQ(-expected, test_case.rhs_cord.Compare(test_case.lhs_cord))
      << "LHS=" << rhs_string << "; RHS=" << lhs_string;
  EXPECT_EQ(-expected, test_case.rhs_cord.Compare(lhs_string))
      << "LHS=" << rhs_string << "; RHS=" << lhs_string;
}

TEST_P(CordTest, Compare) {
  absl::Cord subcord("aaaaaBBBBBcccccDDDDD");
  subcord = subcord.Subcord(3, 10);

  absl::Cord tmp("aaaaaaaaaaaaaaaa");
  tmp.Append("BBBBBBBBBBBBBBBB");
  absl::Cord concat = absl::Cord("cccccccccccccccc");
  concat.Append("DDDDDDDDDDDDDDDD");
  concat.Prepend(tmp);

  absl::Cord concat2("aaaaaaaaaaaaa");
  concat2.Append("aaaBBBBBBBBBBBBBBBBccccc");
  concat2.Append("cccccccccccDDDDDDDDDDDDDD");
  concat2.Append("DD");

  std::vector<CordCompareTestCase> test_cases = {{
      // Inline cords
      {"abcdef", "abcdef"},
      {"abcdef", "abcdee"},
      {"abcdef", "abcdeg"},
      {"bbcdef", "abcdef"},
      {"bbcdef", "abcdeg"},
      {"abcdefa", "abcdef"},
      {"abcdef", "abcdefa"},

      // Small flat cords
      {"aaaaaBBBBBcccccDDDDD", "aaaaaBBBBBcccccDDDDD"},
      {"aaaaaBBBBBcccccDDDDD", "aaaaaBBBBBxccccDDDDD"},
      {"aaaaaBBBBBcxcccDDDDD", "aaaaaBBBBBcccccDDDDD"},
      {"aaaaaBBBBBxccccDDDDD", "aaaaaBBBBBcccccDDDDX"},
      {"aaaaaBBBBBcccccDDDDDa", "aaaaaBBBBBcccccDDDDD"},
      {"aaaaaBBBBBcccccDDDDD", "aaaaaBBBBBcccccDDDDDa"},

      // Subcords
      {subcord, subcord},
      {subcord, "aaBBBBBccc"},
      {subcord, "aaBBBBBccd"},
      {subcord, "aaBBBBBccb"},
      {subcord, "aaBBBBBxcb"},
      {subcord, "aaBBBBBccca"},
      {subcord, "aaBBBBBcc"},

      // Concats
      {concat, concat},
      {concat,
       "aaaaaaaaaaaaaaaaBBBBBBBBBBBBBBBBccccccccccccccccDDDDDDDDDDDDDDDD"},
      {concat,
       "aaaaaaaaaaaaaaaaBBBBBBBBBBBBBBBBcccccccccccccccxDDDDDDDDDDDDDDDD"},
      {concat,
       "aaaaaaaaaaaaaaaaBBBBBBBBBBBBBBBBacccccccccccccccDDDDDDDDDDDDDDDD"},
      {concat,
       "aaaaaaaaaaaaaaaaBBBBBBBBBBBBBBBBccccccccccccccccDDDDDDDDDDDDDDD"},
      {concat,
       "aaaaaaaaaaaaaaaaBBBBBBBBBBBBBBBBccccccccccccccccDDDDDDDDDDDDDDDDe"},

      {concat, concat2},
  }};

  for (const auto& tc : test_cases) {
    VerifyComparison(tc);
  }
}

TEST_P(CordTest, CompareAfterAssign) {
  absl::Cord a("aaaaaa1111111");
  absl::Cord b("aaaaaa2222222");
  a = "cccccc";
  b = "cccccc";
  EXPECT_EQ(a, b);
  EXPECT_FALSE(a < b);

  a = "aaaa";
  b = "bbbbb";
  a = "";
  b = "";
  EXPECT_EQ(a, b);
  EXPECT_FALSE(a < b);
}

// Test CompareTo() and ComparePrefix() against string and substring
// comparison methods from basic_string.
static void TestCompare(const absl::Cord& c, const absl::Cord& d,
                        RandomEngine* rng) {
  typedef std::basic_string<uint8_t> ustring;
  ustring cs(reinterpret_cast<const uint8_t*>(std::string(c).data()), c.size());
  ustring ds(reinterpret_cast<const uint8_t*>(std::string(d).data()), d.size());
  // ustring comparison is ideal because we expect Cord comparisons to be
  // based on unsigned byte comparisons regardless of whether char is signed.
  int expected = sign(cs.compare(ds));
  EXPECT_EQ(expected, sign(c.Compare(d))) << c << ", " << d;
}

TEST_P(CordTest, CompareComparisonIsUnsigned) {
  RandomEngine rng(GTEST_FLAG_GET(random_seed));
  std::uniform_int_distribution<uint32_t> uniform_uint8(0, 255);
  char x = static_cast<char>(uniform_uint8(rng));
  TestCompare(
      absl::Cord(std::string(GetUniformRandomUpTo(&rng, 100), x)),
      absl::Cord(std::string(GetUniformRandomUpTo(&rng, 100), x ^ 0x80)), &rng);
}

TEST_P(CordTest, CompareRandomComparisons) {
  const int kIters = 5000;
  RandomEngine rng(GTEST_FLAG_GET(random_seed));

  int n = GetUniformRandomUpTo(&rng, 5000);
  absl::Cord a[] = {MakeExternalCord(n),
                    absl::Cord("ant"),
                    absl::Cord("elephant"),
                    absl::Cord("giraffe"),
                    absl::Cord(std::string(GetUniformRandomUpTo(&rng, 100),
                                           GetUniformRandomUpTo(&rng, 100))),
                    absl::Cord(""),
                    absl::Cord("x"),
                    absl::Cord("A"),
                    absl::Cord("B"),
                    absl::Cord("C")};
  for (int i = 0; i < kIters; i++) {
    absl::Cord c, d;
    for (int j = 0; j < (i % 7) + 1; j++) {
      c.Append(a[GetUniformRandomUpTo(&rng, ABSL_ARRAYSIZE(a))]);
      d.Append(a[GetUniformRandomUpTo(&rng, ABSL_ARRAYSIZE(a))]);
    }
    std::bernoulli_distribution coin_flip(0.5);
    TestCompare(coin_flip(rng) ? c : absl::Cord(std::string(c)),
                coin_flip(rng) ? d : absl::Cord(std::string(d)), &rng);
  }
}

template <typename T1, typename T2>
void CompareOperators() {
  const T1 a("a");
  const T2 b("b");

  EXPECT_TRUE(a == a);
  // For pointer type (i.e. `const char*`), operator== compares the address
  // instead of the string, so `a == const char*("a")` isn't necessarily true.
  EXPECT_TRUE(std::is_pointer<T1>::value || a == T1("a"));
  EXPECT_TRUE(std::is_pointer<T2>::value || a == T2("a"));
  EXPECT_FALSE(a == b);

  EXPECT_TRUE(a != b);
  EXPECT_FALSE(a != a);

  EXPECT_TRUE(a < b);
  EXPECT_FALSE(b < a);

  EXPECT_TRUE(b > a);
  EXPECT_FALSE(a > b);

  EXPECT_TRUE(a >= a);
  EXPECT_TRUE(b >= a);
  EXPECT_FALSE(a >= b);

  EXPECT_TRUE(a <= a);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(b <= a);
}

TEST_P(CordTest, ComparisonOperators_Cord_Cord) {
  CompareOperators<absl::Cord, absl::Cord>();
}

TEST_P(CordTest, ComparisonOperators_Cord_StringPiece) {
  CompareOperators<absl::Cord, absl::string_view>();
}

TEST_P(CordTest, ComparisonOperators_StringPiece_Cord) {
  CompareOperators<absl::string_view, absl::Cord>();
}

TEST_P(CordTest, ComparisonOperators_Cord_string) {
  CompareOperators<absl::Cord, std::string>();
}

TEST_P(CordTest, ComparisonOperators_string_Cord) {
  CompareOperators<std::string, absl::Cord>();
}

TEST_P(CordTest, ComparisonOperators_stdstring_Cord) {
  CompareOperators<std::string, absl::Cord>();
}

TEST_P(CordTest, ComparisonOperators_Cord_stdstring) {
  CompareOperators<absl::Cord, std::string>();
}

TEST_P(CordTest, ComparisonOperators_charstar_Cord) {
  CompareOperators<const char*, absl::Cord>();
}

TEST_P(CordTest, ComparisonOperators_Cord_charstar) {
  CompareOperators<absl::Cord, const char*>();
}

TEST_P(CordTest, ConstructFromExternalReleaserInvoked) {
  // Empty external memory means the releaser should be called immediately.
  {
    bool invoked = false;
    auto releaser = [&invoked](absl::string_view) { invoked = true; };
    {
      auto c = absl::MakeCordFromExternal("", releaser);
      EXPECT_TRUE(invoked);
    }
  }

  // If the size of the data is small enough, a future constructor
  // implementation may copy the bytes and immediately invoke the releaser
  // instead of creating an external node. We make a large dummy std::string to
  // make this test independent of such an optimization.
  std::string large_dummy(2048, 'c');
  {
    bool invoked = false;
    auto releaser = [&invoked](absl::string_view) { invoked = true; };
    {
      auto c = absl::MakeCordFromExternal(large_dummy, releaser);
      EXPECT_FALSE(invoked);
    }
    EXPECT_TRUE(invoked);
  }

  {
    bool invoked = false;
    auto releaser = [&invoked](absl::string_view) { invoked = true; };
    {
      absl::Cord copy;
      {
        auto c = absl::MakeCordFromExternal(large_dummy, releaser);
        copy = c;
        EXPECT_FALSE(invoked);
      }
      EXPECT_FALSE(invoked);
    }
    EXPECT_TRUE(invoked);
  }
}

TEST_P(CordTest, ConstructFromExternalCompareContents) {
  RandomEngine rng(GTEST_FLAG_GET(random_seed));

  for (int length = 1; length <= 2048; length *= 2) {
    std::string data = RandomLowercaseString(&rng, length);
    auto* external = new std::string(data);
    auto cord =
        absl::MakeCordFromExternal(*external, [external](absl::string_view sv) {
          EXPECT_EQ(external->data(), sv.data());
          EXPECT_EQ(external->size(), sv.size());
          delete external;
        });
    EXPECT_EQ(data, cord);
  }
}

TEST_P(CordTest, ConstructFromExternalLargeReleaser) {
  RandomEngine rng(GTEST_FLAG_GET(random_seed));
  constexpr size_t kLength = 256;
  std::string data = RandomLowercaseString(&rng, kLength);
  std::array<char, kLength> data_array;
  for (size_t i = 0; i < kLength; ++i) data_array[i] = data[i];
  bool invoked = false;
  auto releaser = [data_array, &invoked](absl::string_view data) {
    EXPECT_EQ(data, absl::string_view(data_array.data(), data_array.size()));
    invoked = true;
  };
  (void)absl::MakeCordFromExternal(data, releaser);
  EXPECT_TRUE(invoked);
}

TEST_P(CordTest, ConstructFromExternalFunctionPointerReleaser) {
  static absl::string_view data("hello world");
  static bool invoked;
  auto* releaser =
      static_cast<void (*)(absl::string_view)>([](absl::string_view sv) {
        EXPECT_EQ(data, sv);
        invoked = true;
      });
  invoked = false;
  (void)absl::MakeCordFromExternal(data, releaser);
  EXPECT_TRUE(invoked);

  invoked = false;
  (void)absl::MakeCordFromExternal(data, *releaser);
  EXPECT_TRUE(invoked);
}

TEST_P(CordTest, ConstructFromExternalMoveOnlyReleaser) {
  struct Releaser {
    explicit Releaser(bool* invoked) : invoked(invoked) {}
    Releaser(Releaser&& other) noexcept : invoked(other.invoked) {}
    void operator()(absl::string_view) const { *invoked = true; }

    bool* invoked;
  };

  bool invoked = false;
  (void)absl::MakeCordFromExternal("dummy", Releaser(&invoked));
  EXPECT_TRUE(invoked);
}

TEST_P(CordTest, ConstructFromExternalNoArgLambda) {
  bool invoked = false;
  (void)absl::MakeCordFromExternal("dummy", [&invoked]() { invoked = true; });
  EXPECT_TRUE(invoked);
}

TEST_P(CordTest, ConstructFromExternalStringViewArgLambda) {
  bool invoked = false;
  (void)absl::MakeCordFromExternal(
      "dummy", [&invoked](absl::string_view) { invoked = true; });
  EXPECT_TRUE(invoked);
}

TEST_P(CordTest, ConstructFromExternalNonTrivialReleaserDestructor) {
  struct Releaser {
    explicit Releaser(bool* destroyed) : destroyed(destroyed) {}
    ~Releaser() { *destroyed = true; }
    void operator()(absl::string_view) const {}

    bool* destroyed;
  };

  bool destroyed = false;
  Releaser releaser(&destroyed);
  (void)absl::MakeCordFromExternal("dummy", releaser);
  EXPECT_TRUE(destroyed);
}

TEST_P(CordTest, ConstructFromExternalReferenceQualifierOverloads) {
  struct Releaser {
    void operator()(absl::string_view) & { *lvalue_invoked = true; }
    void operator()(absl::string_view) && { *rvalue_invoked = true; }

    bool* lvalue_invoked;
    bool* rvalue_invoked;
  };

  bool lvalue_invoked = false;
  bool rvalue_invoked = false;
  Releaser releaser = {&lvalue_invoked, &rvalue_invoked};
  (void)absl::MakeCordFromExternal("", releaser);
  EXPECT_FALSE(lvalue_invoked);
  EXPECT_TRUE(rvalue_invoked);
  rvalue_invoked = false;

  (void)absl::MakeCordFromExternal("dummy", releaser);
  EXPECT_FALSE(lvalue_invoked);
  EXPECT_TRUE(rvalue_invoked);
  rvalue_invoked = false;

  // NOLINTNEXTLINE: suppress clang-tidy std::move on trivially copyable type.
  (void)absl::MakeCordFromExternal("dummy", std::move(releaser));
  EXPECT_FALSE(lvalue_invoked);
  EXPECT_TRUE(rvalue_invoked);
}

TEST_P(CordTest, ExternalMemoryBasicUsage) {
  static const char* strings[] = {"", "hello", "there"};
  for (const char* str : strings) {
    absl::Cord dst("(prefix)");
    AddExternalMemory(str, &dst);
    dst.Append("(suffix)");
    EXPECT_EQ((std::string("(prefix)") + str + std::string("(suffix)")),
              std::string(dst));
  }
}

TEST_P(CordTest, ExternalMemoryRemovePrefixSuffix) {
  // Exhaustively try all sub-strings.
  absl::Cord cord = MakeComposite();
  std::string s = std::string(cord);
  for (int offset = 0; offset <= s.size(); offset++) {
    for (int length = 0; length <= s.size() - offset; length++) {
      absl::Cord result(cord);
      result.RemovePrefix(offset);
      result.RemoveSuffix(result.size() - length);
      EXPECT_EQ(s.substr(offset, length), std::string(result))
          << offset << " " << length;
    }
  }
}

TEST_P(CordTest, ExternalMemoryGet) {
  absl::Cord cord("hello");
  AddExternalMemory(" world!", &cord);
  AddExternalMemory(" how are ", &cord);
  cord.Append(" you?");
  std::string s = std::string(cord);
  for (int i = 0; i < s.size(); i++) {
    EXPECT_EQ(s[i], cord[i]);
  }
}

// CordMemoryUsage tests verify the correctness of the EstimatedMemoryUsage()
// These tests take into account that the reported memory usage is approximate
// and non-deterministic. For all tests, We verify that the reported memory
// usage is larger than `size()`, and less than `size() * 1.5` as a cord should
// never reserve more 'extra' capacity than half of its size as it grows.
// Additionally we have some whiteboxed expectations based on our knowledge of
// the layout and size of empty and inlined cords, and flat nodes.

TEST_P(CordTest, CordMemoryUsageEmpty) {
  EXPECT_EQ(sizeof(absl::Cord), absl::Cord().EstimatedMemoryUsage());
}

TEST_P(CordTest, CordMemoryUsageEmbedded) {
  absl::Cord a("hello");
  EXPECT_EQ(a.EstimatedMemoryUsage(), sizeof(absl::Cord));
}

TEST_P(CordTest, CordMemoryUsageEmbeddedAppend) {
  absl::Cord a("a");
  absl::Cord b("bcd");
  EXPECT_EQ(b.EstimatedMemoryUsage(), sizeof(absl::Cord));
  a.Append(b);
  EXPECT_EQ(a.EstimatedMemoryUsage(), sizeof(absl::Cord));
}

TEST_P(CordTest, CordMemoryUsageExternalMemory) {
  static const int kLength = 1000;
  absl::Cord cord;
  AddExternalMemory(std::string(kLength, 'x'), &cord);
  EXPECT_GT(cord.EstimatedMemoryUsage(), kLength);
  EXPECT_LE(cord.EstimatedMemoryUsage(), kLength * 1.5);
}

TEST_P(CordTest, CordMemoryUsageFlat) {
  static const int kLength = 125;
  absl::Cord a(std::string(kLength, 'a'));
  EXPECT_GT(a.EstimatedMemoryUsage(), kLength);
  EXPECT_LE(a.EstimatedMemoryUsage(), kLength * 1.5);
}

TEST_P(CordTest, CordMemoryUsageAppendFlat) {
  using absl::strings_internal::CordTestAccess;
  absl::Cord a(std::string(CordTestAccess::MaxFlatLength(), 'a'));
  size_t length = a.EstimatedMemoryUsage();
  a.Append(std::string(CordTestAccess::MaxFlatLength(), 'b'));
  size_t delta = a.EstimatedMemoryUsage() - length;
  EXPECT_GT(delta, CordTestAccess::MaxFlatLength());
  EXPECT_LE(delta, CordTestAccess::MaxFlatLength() * 1.5);
}

TEST_P(CordTest, CordMemoryUsageAppendExternal) {
  static const int kLength = 1000;
  using absl::strings_internal::CordTestAccess;
  absl::Cord a(std::string(CordTestAccess::MaxFlatLength(), 'a'));
  size_t length = a.EstimatedMemoryUsage();
  AddExternalMemory(std::string(kLength, 'b'), &a);
  size_t delta = a.EstimatedMemoryUsage() - length;
  EXPECT_GT(delta, kLength);
  EXPECT_LE(delta, kLength * 1.5);
}

TEST_P(CordTest, CordMemoryUsageSubString) {
  static const int kLength = 2000;
  using absl::strings_internal::CordTestAccess;
  absl::Cord a(std::string(kLength, 'a'));
  size_t length = a.EstimatedMemoryUsage();
  AddExternalMemory(std::string(kLength, 'b'), &a);
  absl::Cord b = a.Subcord(0, kLength + kLength / 2);
  size_t delta = b.EstimatedMemoryUsage() - length;
  EXPECT_GT(delta, kLength);
  EXPECT_LE(delta, kLength * 1.5);
}

// Regtest for a change that had to be rolled back because it expanded out
// of the InlineRep too soon, which was observable through MemoryUsage().
TEST_P(CordTest, CordMemoryUsageInlineRep) {
  constexpr size_t kMaxInline = 15;  // Cord::InlineRep::N
  const std::string small_string(kMaxInline, 'x');
  absl::Cord c1(small_string);

  absl::Cord c2;
  c2.Append(small_string);
  EXPECT_EQ(c1, c2);
  EXPECT_EQ(c1.EstimatedMemoryUsage(), c2.EstimatedMemoryUsage());
}

}  // namespace

// Regtest for 7510292 (fix a bug introduced by 7465150)
TEST_P(CordTest, Concat_Append) {
  // Create a rep of type CONCAT
  absl::Cord s1("foobarbarbarbarbar");
  s1.Append("abcdefgabcdefgabcdefgabcdefgabcdefgabcdefgabcdefg");
  size_t size = s1.size();

  // Create a copy of s1 and append to it.
  absl::Cord s2 = s1;
  s2.Append("x");

  // 7465150 modifies s1 when it shouldn't.
  EXPECT_EQ(s1.size(), size);
  EXPECT_EQ(s2.size(), size + 1);
}

TEST_P(CordTest, DiabolicalGrowth) {
  // This test exercises a diabolical Append(<one char>) on a cord, making the
  // cord shared before each Append call resulting in a terribly fragmented
  // resulting cord.
  // TODO(b/183983616): Apply some minimum compaction when copying a shared
  // source cord into a mutable copy for updates in CordRepRing.
  RandomEngine rng(GTEST_FLAG_GET(random_seed));
  const std::string expected = RandomLowercaseString(&rng, 5000);
  absl::Cord cord;
  for (char c : expected) {
    absl::Cord shared(cord);
    cord.Append(absl::string_view(&c, 1));
  }
  std::string value;
  absl::CopyCordToString(cord, &value);
  EXPECT_EQ(value, expected);
  ABSL_RAW_LOG(INFO, "Diabolical size allocated = %zu",
               cord.EstimatedMemoryUsage());
}

// The following tests check support for >4GB cords in 64-bit binaries, and
// 2GB-4GB cords in 32-bit binaries.  This function returns the large cord size
// that's appropriate for the binary.

// Construct a huge cord with the specified valid prefix.
static absl::Cord MakeHuge(absl::string_view prefix) {
  absl::Cord cord;
  if (sizeof(size_t) > 4) {
    // In 64-bit binaries, test 64-bit Cord support.
    const size_t size =
        static_cast<size_t>(std::numeric_limits<uint32_t>::max()) + 314;
    cord.Append(absl::MakeCordFromExternal(
        absl::string_view(prefix.data(), size),
        [](absl::string_view s) { DoNothing(s, nullptr); }));
  } else {
    // Cords are limited to 32-bit lengths in 32-bit binaries.  The following
    // tests check for use of "signed int" to represent Cord length/offset.
    // However absl::string_view does not allow lengths >= (1u<<31), so we need
    // to append in two parts;
    const size_t s1 = (1u << 31) - 1;
    // For shorter cord, `Append` copies the data rather than allocating a new
    // node. The threshold is currently set to 511, so `s2` needs to be bigger
    // to not trigger the copy.
    const size_t s2 = 600;
    cord.Append(absl::MakeCordFromExternal(
        absl::string_view(prefix.data(), s1),
        [](absl::string_view s) { DoNothing(s, nullptr); }));
    cord.Append(absl::MakeCordFromExternal(
        absl::string_view("", s2),
        [](absl::string_view s) { DoNothing(s, nullptr); }));
  }
  return cord;
}

TEST_P(CordTest, HugeCord) {
  absl::Cord cord = MakeHuge("huge cord");
  EXPECT_LE(cord.size(), cord.EstimatedMemoryUsage());
  EXPECT_GE(cord.size() + 100, cord.EstimatedMemoryUsage());
}

// Tests that Append() works ok when handed a self reference
TEST_P(CordTest, AppendSelf) {
  // We run the test until data is ~16K
  // This guarantees it covers small, medium and large data.
  std::string control_data = "Abc";
  absl::Cord data(control_data);
  while (control_data.length() < 0x4000) {
    data.Append(data);
    control_data.append(control_data);
    ASSERT_EQ(control_data, data);
  }
}

TEST_P(CordTest, MakeFragmentedCordFromInitializerList) {
  absl::Cord fragmented =
      absl::MakeFragmentedCord({"A ", "fragmented ", "Cord"});

  EXPECT_EQ("A fragmented Cord", fragmented);

  auto chunk_it = fragmented.chunk_begin();

  ASSERT_TRUE(chunk_it != fragmented.chunk_end());
  EXPECT_EQ("A ", *chunk_it);

  ASSERT_TRUE(++chunk_it != fragmented.chunk_end());
  EXPECT_EQ("fragmented ", *chunk_it);

  ASSERT_TRUE(++chunk_it != fragmented.chunk_end());
  EXPECT_EQ("Cord", *chunk_it);

  ASSERT_TRUE(++chunk_it == fragmented.chunk_end());
}

TEST_P(CordTest, MakeFragmentedCordFromVector) {
  std::vector<absl::string_view> chunks = {"A ", "fragmented ", "Cord"};
  absl::Cord fragmented = absl::MakeFragmentedCord(chunks);

  EXPECT_EQ("A fragmented Cord", fragmented);

  auto chunk_it = fragmented.chunk_begin();

  ASSERT_TRUE(chunk_it != fragmented.chunk_end());
  EXPECT_EQ("A ", *chunk_it);

  ASSERT_TRUE(++chunk_it != fragmented.chunk_end());
  EXPECT_EQ("fragmented ", *chunk_it);

  ASSERT_TRUE(++chunk_it != fragmented.chunk_end());
  EXPECT_EQ("Cord", *chunk_it);

  ASSERT_TRUE(++chunk_it == fragmented.chunk_end());
}

TEST_P(CordTest, CordChunkIteratorTraits) {
  static_assert(std::is_copy_constructible<absl::Cord::ChunkIterator>::value,
                "");
  static_assert(std::is_copy_assignable<absl::Cord::ChunkIterator>::value, "");

  // Move semantics to satisfy swappable via std::swap
  static_assert(std::is_move_constructible<absl::Cord::ChunkIterator>::value,
                "");
  static_assert(std::is_move_assignable<absl::Cord::ChunkIterator>::value, "");

  static_assert(
      std::is_same<
          std::iterator_traits<absl::Cord::ChunkIterator>::iterator_category,
          std::input_iterator_tag>::value,
      "");
  static_assert(
      std::is_same<std::iterator_traits<absl::Cord::ChunkIterator>::value_type,
                   absl::string_view>::value,
      "");
  static_assert(
      std::is_same<
          std::iterator_traits<absl::Cord::ChunkIterator>::difference_type,
          ptrdiff_t>::value,
      "");
  static_assert(
      std::is_same<std::iterator_traits<absl::Cord::ChunkIterator>::pointer,
                   const absl::string_view*>::value,
      "");
  static_assert(
      std::is_same<std::iterator_traits<absl::Cord::ChunkIterator>::reference,
                   absl::string_view>::value,
      "");
}

static void VerifyChunkIterator(const absl::Cord& cord,
                                size_t expected_chunks) {
  EXPECT_EQ(cord.chunk_begin() == cord.chunk_end(), cord.empty()) << cord;
  EXPECT_EQ(cord.chunk_begin() != cord.chunk_end(), !cord.empty());

  absl::Cord::ChunkRange range = cord.Chunks();
  EXPECT_EQ(range.begin() == range.end(), cord.empty());
  EXPECT_EQ(range.begin() != range.end(), !cord.empty());

  std::string content(cord);
  size_t pos = 0;
  auto pre_iter = cord.chunk_begin(), post_iter = cord.chunk_begin();
  size_t n_chunks = 0;
  while (pre_iter != cord.chunk_end() && post_iter != cord.chunk_end()) {
    EXPECT_FALSE(pre_iter == cord.chunk_end());   // NOLINT: explicitly test ==
    EXPECT_FALSE(post_iter == cord.chunk_end());  // NOLINT

    EXPECT_EQ(pre_iter, post_iter);
    EXPECT_EQ(*pre_iter, *post_iter);

    EXPECT_EQ(pre_iter->data(), (*pre_iter).data());
    EXPECT_EQ(pre_iter->size(), (*pre_iter).size());

    absl::string_view chunk = *pre_iter;
    EXPECT_FALSE(chunk.empty());
    EXPECT_LE(pos + chunk.size(), content.size());
    EXPECT_EQ(absl::string_view(content.c_str() + pos, chunk.size()), chunk);

    int n_equal_iterators = 0;
    for (absl::Cord::ChunkIterator it = range.begin(); it != range.end();
         ++it) {
      n_equal_iterators += static_cast<int>(it == pre_iter);
    }
    EXPECT_EQ(n_equal_iterators, 1);

    ++pre_iter;
    EXPECT_EQ(*post_iter++, chunk);

    pos += chunk.size();
    ++n_chunks;
  }
  EXPECT_EQ(expected_chunks, n_chunks);
  EXPECT_EQ(pos, content.size());
  EXPECT_TRUE(pre_iter == cord.chunk_end());   // NOLINT: explicitly test ==
  EXPECT_TRUE(post_iter == cord.chunk_end());  // NOLINT
}

TEST_P(CordTest, CordChunkIteratorOperations) {
  absl::Cord empty_cord;
  VerifyChunkIterator(empty_cord, 0);

  absl::Cord small_buffer_cord("small cord");
  VerifyChunkIterator(small_buffer_cord, 1);

  absl::Cord flat_node_cord("larger than small buffer optimization");
  VerifyChunkIterator(flat_node_cord, 1);

  VerifyChunkIterator(
      absl::MakeFragmentedCord({"a ", "small ", "fragmented ", "cord ", "for ",
                                "testing ", "chunk ", "iterations."}),
      8);

  absl::Cord reused_nodes_cord(std::string(40, 'c'));
  reused_nodes_cord.Prepend(absl::Cord(std::string(40, 'b')));
  reused_nodes_cord.Prepend(absl::Cord(std::string(40, 'a')));
  size_t expected_chunks = 3;
  for (int i = 0; i < 8; ++i) {
    reused_nodes_cord.Prepend(reused_nodes_cord);
    expected_chunks *= 2;
    VerifyChunkIterator(reused_nodes_cord, expected_chunks);
  }

  RandomEngine rng(GTEST_FLAG_GET(random_seed));
  absl::Cord flat_cord(RandomLowercaseString(&rng, 256));
  absl::Cord subcords;
  for (int i = 0; i < 128; ++i) subcords.Prepend(flat_cord.Subcord(i, 128));
  VerifyChunkIterator(subcords, 128);
}

TEST_P(CordTest, CharIteratorTraits) {
  static_assert(std::is_copy_constructible<absl::Cord::CharIterator>::value,
                "");
  static_assert(std::is_copy_assignable<absl::Cord::CharIterator>::value, "");

  // Move semantics to satisfy swappable via std::swap
  static_assert(std::is_move_constructible<absl::Cord::CharIterator>::value,
                "");
  static_assert(std::is_move_assignable<absl::Cord::CharIterator>::value, "");

  static_assert(
      std::is_same<
          std::iterator_traits<absl::Cord::CharIterator>::iterator_category,
          std::input_iterator_tag>::value,
      "");
  static_assert(
      std::is_same<std::iterator_traits<absl::Cord::CharIterator>::value_type,
                   char>::value,
      "");
  static_assert(
      std::is_same<
          std::iterator_traits<absl::Cord::CharIterator>::difference_type,
          ptrdiff_t>::value,
      "");
  static_assert(
      std::is_same<std::iterator_traits<absl::Cord::CharIterator>::pointer,
                   const char*>::value,
      "");
  static_assert(
      std::is_same<std::iterator_traits<absl::Cord::CharIterator>::reference,
                   const char&>::value,
      "");
}

static void VerifyCharIterator(const absl::Cord& cord) {
  EXPECT_EQ(cord.char_begin() == cord.char_end(), cord.empty());
  EXPECT_EQ(cord.char_begin() != cord.char_end(), !cord.empty());

  absl::Cord::CharRange range = cord.Chars();
  EXPECT_EQ(range.begin() == range.end(), cord.empty());
  EXPECT_EQ(range.begin() != range.end(), !cord.empty());

  size_t i = 0;
  absl::Cord::CharIterator pre_iter = cord.char_begin();
  absl::Cord::CharIterator post_iter = cord.char_begin();
  std::string content(cord);
  while (pre_iter != cord.char_end() && post_iter != cord.char_end()) {
    EXPECT_FALSE(pre_iter == cord.char_end());   // NOLINT: explicitly test ==
    EXPECT_FALSE(post_iter == cord.char_end());  // NOLINT

    EXPECT_LT(i, cord.size());
    EXPECT_EQ(content[i], *pre_iter);

    EXPECT_EQ(pre_iter, post_iter);
    EXPECT_EQ(*pre_iter, *post_iter);
    EXPECT_EQ(&*pre_iter, &*post_iter);

    EXPECT_EQ(&*pre_iter, pre_iter.operator->());

    const char* character_address = &*pre_iter;
    absl::Cord::CharIterator copy = pre_iter;
    ++copy;
    EXPECT_EQ(character_address, &*pre_iter);

    int n_equal_iterators = 0;
    for (absl::Cord::CharIterator it = range.begin(); it != range.end(); ++it) {
      n_equal_iterators += static_cast<int>(it == pre_iter);
    }
    EXPECT_EQ(n_equal_iterators, 1);

    absl::Cord::CharIterator advance_iter = range.begin();
    absl::Cord::Advance(&advance_iter, i);
    EXPECT_EQ(pre_iter, advance_iter);

    advance_iter = range.begin();
    EXPECT_EQ(absl::Cord::AdvanceAndRead(&advance_iter, i), cord.Subcord(0, i));
    EXPECT_EQ(pre_iter, advance_iter);

    advance_iter = pre_iter;
    absl::Cord::Advance(&advance_iter, cord.size() - i);
    EXPECT_EQ(range.end(), advance_iter);

    advance_iter = pre_iter;
    EXPECT_EQ(absl::Cord::AdvanceAndRead(&advance_iter, cord.size() - i),
              cord.Subcord(i, cord.size() - i));
    EXPECT_EQ(range.end(), advance_iter);

    ++i;
    ++pre_iter;
    post_iter++;
  }
  EXPECT_EQ(i, cord.size());
  EXPECT_TRUE(pre_iter == cord.char_end());   // NOLINT: explicitly test ==
  EXPECT_TRUE(post_iter == cord.char_end());  // NOLINT

  absl::Cord::CharIterator zero_advanced_end = cord.char_end();
  absl::Cord::Advance(&zero_advanced_end, 0);
  EXPECT_EQ(zero_advanced_end, cord.char_end());

  absl::Cord::CharIterator it = cord.char_begin();
  for (absl::string_view chunk : cord.Chunks()) {
    while (!chunk.empty()) {
      EXPECT_EQ(absl::Cord::ChunkRemaining(it), chunk);
      chunk.remove_prefix(1);
      ++it;
    }
  }
}

TEST_P(CordTest, CharIteratorOperations) {
  absl::Cord empty_cord;
  VerifyCharIterator(empty_cord);

  absl::Cord small_buffer_cord("small cord");
  VerifyCharIterator(small_buffer_cord);

  absl::Cord flat_node_cord("larger than small buffer optimization");
  VerifyCharIterator(flat_node_cord);

  VerifyCharIterator(
      absl::MakeFragmentedCord({"a ", "small ", "fragmented ", "cord ", "for ",
                                "testing ", "character ", "iteration."}));

  absl::Cord reused_nodes_cord("ghi");
  reused_nodes_cord.Prepend(absl::Cord("def"));
  reused_nodes_cord.Prepend(absl::Cord("abc"));
  for (int i = 0; i < 4; ++i) {
    reused_nodes_cord.Prepend(reused_nodes_cord);
    VerifyCharIterator(reused_nodes_cord);
  }

  RandomEngine rng(GTEST_FLAG_GET(random_seed));
  absl::Cord flat_cord(RandomLowercaseString(&rng, 256));
  absl::Cord subcords;
  for (int i = 0; i < 4; ++i) subcords.Prepend(flat_cord.Subcord(16 * i, 128));
  VerifyCharIterator(subcords);
}

TEST_P(CordTest, CharIteratorAdvanceAndRead) {
  // Create a Cord holding 6 flats of 2500 bytes each, and then iterate over it
  // reading 150, 1500, 2500 and 3000 bytes. This will result in all possible
  // partial, full and straddled read combinations including reads below
  // kMaxBytesToCopy. b/197776822 surfaced a bug for a specific partial, small
  // read 'at end' on Cord which caused a failure on attempting to read past the
  // end in CordRepBtreeReader which was not covered by any existing test.
  constexpr int kBlocks = 6;
  constexpr size_t kBlockSize = 2500;
  constexpr size_t kChunkSize1 = 1500;
  constexpr size_t kChunkSize2 = 2500;
  constexpr size_t kChunkSize3 = 3000;
  constexpr size_t kChunkSize4 = 150;
  RandomEngine rng;
  std::string data = RandomLowercaseString(&rng, kBlocks * kBlockSize);
  absl::Cord cord;
  for (int i = 0; i < kBlocks; ++i) {
    const std::string block = data.substr(i * kBlockSize, kBlockSize);
    cord.Append(absl::Cord(block));
  }

  for (size_t chunk_size :
       {kChunkSize1, kChunkSize2, kChunkSize3, kChunkSize4}) {
    absl::Cord::CharIterator it = cord.char_begin();
    size_t offset = 0;
    while (offset < data.length()) {
      const size_t n = std::min<size_t>(data.length() - offset, chunk_size);
      absl::Cord chunk = cord.AdvanceAndRead(&it, n);
      ASSERT_EQ(chunk.size(), n);
      ASSERT_EQ(chunk.Compare(data.substr(offset, n)), 0);
      offset += n;
    }
  }
}

TEST_P(CordTest, StreamingOutput) {
  absl::Cord c =
      absl::MakeFragmentedCord({"A ", "small ", "fragmented ", "Cord", "."});
  std::stringstream output;
  output << c;
  EXPECT_EQ("A small fragmented Cord.", output.str());
}

TEST_P(CordTest, ForEachChunk) {
  for (int num_elements : {1, 10, 200}) {
    SCOPED_TRACE(num_elements);
    std::vector<std::string> cord_chunks;
    for (int i = 0; i < num_elements; ++i) {
      cord_chunks.push_back(absl::StrCat("[", i, "]"));
    }
    absl::Cord c = absl::MakeFragmentedCord(cord_chunks);

    std::vector<std::string> iterated_chunks;
    absl::CordTestPeer::ForEachChunk(c,
                                     [&iterated_chunks](absl::string_view sv) {
                                       iterated_chunks.emplace_back(sv);
                                     });
    EXPECT_EQ(iterated_chunks, cord_chunks);
  }
}

TEST_P(CordTest, SmallBufferAssignFromOwnData) {
  constexpr size_t kMaxInline = 15;
  std::string contents = "small buff cord";
  EXPECT_EQ(contents.size(), kMaxInline);
  for (size_t pos = 0; pos < contents.size(); ++pos) {
    for (size_t count = contents.size() - pos; count > 0; --count) {
      absl::Cord c(contents);
      absl::string_view flat = c.Flatten();
      c = flat.substr(pos, count);
      EXPECT_EQ(c, contents.substr(pos, count))
          << "pos = " << pos << "; count = " << count;
    }
  }
}

TEST_P(CordTest, Format) {
  absl::Cord c;
  absl::Format(&c, "There were %04d little %s.", 3, "pigs");
  EXPECT_EQ(c, "There were 0003 little pigs.");
  absl::Format(&c, "And %-3llx bad wolf!", 1);
  EXPECT_EQ(c, "There were 0003 little pigs.And 1   bad wolf!");
}

TEST_P(CordTest, Hardening) {
  absl::Cord cord("hello");
  // These statement should abort the program in all builds modes.
  EXPECT_DEATH_IF_SUPPORTED(cord.RemovePrefix(6), "");
  EXPECT_DEATH_IF_SUPPORTED(cord.RemoveSuffix(6), "");

  bool test_hardening = false;
  ABSL_HARDENING_ASSERT([&]() {
    // This only runs when ABSL_HARDENING_ASSERT is active.
    test_hardening = true;
    return true;
  }());
  if (!test_hardening) return;

  EXPECT_DEATH_IF_SUPPORTED(cord[5], "");
  EXPECT_DEATH_IF_SUPPORTED(*cord.chunk_end(), "");
  EXPECT_DEATH_IF_SUPPORTED(static_cast<void>(cord.chunk_end()->empty()), "");
  EXPECT_DEATH_IF_SUPPORTED(++cord.chunk_end(), "");
}

// This test mimics a specific (and rare) application repeatedly splitting a
// cord, inserting (overwriting) a string value, and composing a new cord from
// the three pieces. This is hostile towards a Btree implementation: A split of
// a node at any level is likely to have the right-most edge of the left split,
// and the left-most edge of the right split shared. For example, splitting a
// leaf node with 6 edges will result likely in a 1-6, 2-5, 3-4, etc. split,
// sharing the 'split node'. When recomposing such nodes, we 'injected' an edge
// in that node. As this happens with some probability on each level of the
// tree, this will quickly grow the tree until it reaches maximum height.
TEST_P(CordTest, BtreeHostileSplitInsertJoin) {
  absl::BitGen bitgen;

  // Start with about 1GB of data
  std::string data(1 << 10, 'x');
  absl::Cord buffer(data);
  absl::Cord cord;
  for (int i = 0; i < 1000000; ++i) {
    cord.Append(buffer);
  }

  for (int j = 0; j < 1000; ++j) {
    size_t offset = absl::Uniform(bitgen, 0u, cord.size());
    size_t length = absl::Uniform(bitgen, 100u, data.size());
    if (cord.size() == offset) {
      cord.Append(absl::string_view(data.data(), length));
    } else {
      absl::Cord suffix;
      if (offset + length < cord.size()) {
        suffix = cord;
        suffix.RemovePrefix(offset + length);
      }
      if (cord.size() > offset) {
        cord.RemoveSuffix(cord.size() - offset);
      }
      cord.Append(absl::string_view(data.data(), length));
      if (!suffix.empty()) {
        cord.Append(suffix);
      }
    }
  }
}

class AfterExitCordTester {
 public:
  bool Set(absl::Cord* cord, absl::string_view expected) {
    cord_ = cord;
    expected_ = expected;
    return true;
  }

  ~AfterExitCordTester() {
    EXPECT_EQ(*cord_, expected_);
  }
 private:
  absl::Cord* cord_;
  absl::string_view expected_;
};

template <typename Str>
void TestConstinitConstructor(Str) {
  const auto expected = Str::value;
  // Defined before `cord` to be destroyed after it.
  static AfterExitCordTester exit_tester;  // NOLINT
  ABSL_CONST_INIT static absl::Cord cord(Str{});  // NOLINT
  static bool init_exit_tester = exit_tester.Set(&cord, expected);
  (void)init_exit_tester;

  EXPECT_EQ(cord, expected);
  // Copy the object and test the copy, and the original.
  {
    absl::Cord copy = cord;
    EXPECT_EQ(copy, expected);
  }
  // The original still works
  EXPECT_EQ(cord, expected);

  // Try making adding more structure to the tree.
  {
    absl::Cord copy = cord;
    std::string expected_copy(expected);
    for (int i = 0; i < 10; ++i) {
      copy.Append(cord);
      absl::StrAppend(&expected_copy, expected);
      EXPECT_EQ(copy, expected_copy);
    }
  }

  // Make sure we are using the right branch during constant evaluation.
  EXPECT_EQ(absl::CordTestPeer::IsTree(cord), cord.size() >= 16);

  for (int i = 0; i < 10; ++i) {
    // Make a few more Cords from the same global rep.
    // This tests what happens when the refcount for it gets below 1.
    EXPECT_EQ(expected, absl::Cord(Str{}));
  }
}

constexpr int SimpleStrlen(const char* p) {
  return *p ? 1 + SimpleStrlen(p + 1) : 0;
}

struct ShortView {
  constexpr absl::string_view operator()() const {
    return absl::string_view("SSO string", SimpleStrlen("SSO string"));
  }
};

struct LongView {
  constexpr absl::string_view operator()() const {
    return absl::string_view("String that does not fit SSO.",
                             SimpleStrlen("String that does not fit SSO."));
  }
};


TEST_P(CordTest, ConstinitConstructor) {
  TestConstinitConstructor(
      absl::strings_internal::MakeStringConstant(ShortView{}));
  TestConstinitConstructor(
      absl::strings_internal::MakeStringConstant(LongView{}));
}
