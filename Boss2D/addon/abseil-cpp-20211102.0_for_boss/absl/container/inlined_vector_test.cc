// Copyright 2019 The Abseil Authors.
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

#include BOSS_ABSEILCPP_U_absl__container__inlined_vector_h //original-code:"absl/container/inlined_vector.h"

#include <algorithm>
#include <forward_list>
#include <list>
#include <memory>
#include <scoped_allocator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__exception_testing_h //original-code:"absl/base/internal/exception_testing.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__raw_logging_h //original-code:"absl/base/internal/raw_logging.h"
#include BOSS_ABSEILCPP_U_absl__base__macros_h //original-code:"absl/base/macros.h"
#include BOSS_ABSEILCPP_U_absl__base__options_h //original-code:"absl/base/options.h"
#include BOSS_ABSEILCPP_U_absl__container__internal__counting_allocator_h //original-code:"absl/container/internal/counting_allocator.h"
#include BOSS_ABSEILCPP_U_absl__container__internal__test_instance_tracker_h //original-code:"absl/container/internal/test_instance_tracker.h"
#include "absl/hash/hash_testing.h"
#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_ABSEILCPP_U_absl__strings__str_cat_h //original-code:"absl/strings/str_cat.h"

namespace {

using absl::container_internal::CountingAllocator;
using absl::test_internal::CopyableMovableInstance;
using absl::test_internal::CopyableOnlyInstance;
using absl::test_internal::InstanceTracker;
using testing::AllOf;
using testing::Each;
using testing::ElementsAre;
using testing::ElementsAreArray;
using testing::Eq;
using testing::Gt;
using testing::PrintToString;

using IntVec = absl::InlinedVector<int, 8>;

MATCHER_P(SizeIs, n, "") {
  return testing::ExplainMatchResult(n, arg.size(), result_listener);
}

MATCHER_P(CapacityIs, n, "") {
  return testing::ExplainMatchResult(n, arg.capacity(), result_listener);
}

MATCHER_P(ValueIs, e, "") {
  return testing::ExplainMatchResult(e, arg.value(), result_listener);
}

// TODO(bsamwel): Add support for movable-only types.

// Test fixture for typed tests on BaseCountedInstance derived classes, see
// test_instance_tracker.h.
template <typename T>
class InstanceTest : public ::testing::Test {};
TYPED_TEST_SUITE_P(InstanceTest);

// A simple reference counted class to make sure that the proper elements are
// destroyed in the erase(begin, end) test.
class RefCounted {
 public:
  RefCounted(int value, int* count) : value_(value), count_(count) { Ref(); }

  RefCounted(const RefCounted& v) : value_(v.value_), count_(v.count_) {
    Ref();
  }

  ~RefCounted() {
    Unref();
    count_ = nullptr;
  }

  friend void swap(RefCounted& a, RefCounted& b) {
    using std::swap;
    swap(a.value_, b.value_);
    swap(a.count_, b.count_);
  }

  RefCounted& operator=(RefCounted v) {
    using std::swap;
    swap(*this, v);
    return *this;
  }

  void Ref() const {
    ABSL_RAW_CHECK(count_ != nullptr, "");
    ++(*count_);
  }

  void Unref() const {
    --(*count_);
    ABSL_RAW_CHECK(*count_ >= 0, "");
  }

  int value_;
  int* count_;
};

using RefCountedVec = absl::InlinedVector<RefCounted, 8>;

// A class with a vtable pointer
class Dynamic {
 public:
  virtual ~Dynamic() {}
};

using DynamicVec = absl::InlinedVector<Dynamic, 8>;

// Append 0..len-1 to *v
template <typename Container>
static void Fill(Container* v, int len, int offset = 0) {
  for (int i = 0; i < len; i++) {
    v->push_back(i + offset);
  }
}

static IntVec Fill(int len, int offset = 0) {
  IntVec v;
  Fill(&v, len, offset);
  return v;
}

TEST(IntVec, SimpleOps) {
  for (int len = 0; len < 20; len++) {
    IntVec v;
    const IntVec& cv = v;  // const alias

    Fill(&v, len);
    EXPECT_EQ(len, v.size());
    EXPECT_LE(len, v.capacity());

    for (int i = 0; i < len; i++) {
      EXPECT_EQ(i, v[i]);
      EXPECT_EQ(i, v.at(i));
    }
    EXPECT_EQ(v.begin(), v.data());
    EXPECT_EQ(cv.begin(), cv.data());

    int counter = 0;
    for (IntVec::iterator iter = v.begin(); iter != v.end(); ++iter) {
      EXPECT_EQ(counter, *iter);
      counter++;
    }
    EXPECT_EQ(counter, len);

    counter = 0;
    for (IntVec::const_iterator iter = v.begin(); iter != v.end(); ++iter) {
      EXPECT_EQ(counter, *iter);
      counter++;
    }
    EXPECT_EQ(counter, len);

    counter = 0;
    for (IntVec::const_iterator iter = v.cbegin(); iter != v.cend(); ++iter) {
      EXPECT_EQ(counter, *iter);
      counter++;
    }
    EXPECT_EQ(counter, len);

    if (len > 0) {
      EXPECT_EQ(0, v.front());
      EXPECT_EQ(len - 1, v.back());
      v.pop_back();
      EXPECT_EQ(len - 1, v.size());
      for (int i = 0; i < v.size(); ++i) {
        EXPECT_EQ(i, v[i]);
        EXPECT_EQ(i, v.at(i));
      }
    }
  }
}

TEST(IntVec, PopBackNoOverflow) {
  IntVec v = {1};
  v.pop_back();
  EXPECT_EQ(v.size(), 0);
}

TEST(IntVec, AtThrows) {
  IntVec v = {1, 2, 3};
  EXPECT_EQ(v.at(2), 3);
  ABSL_BASE_INTERNAL_EXPECT_FAIL(v.at(3), std::out_of_range,
                                 "failed bounds check");
}

TEST(IntVec, ReverseIterator) {
  for (int len = 0; len < 20; len++) {
    IntVec v;
    Fill(&v, len);

    int counter = len;
    for (IntVec::reverse_iterator iter = v.rbegin(); iter != v.rend(); ++iter) {
      counter--;
      EXPECT_EQ(counter, *iter);
    }
    EXPECT_EQ(counter, 0);

    counter = len;
    for (IntVec::const_reverse_iterator iter = v.rbegin(); iter != v.rend();
         ++iter) {
      counter--;
      EXPECT_EQ(counter, *iter);
    }
    EXPECT_EQ(counter, 0);

    counter = len;
    for (IntVec::const_reverse_iterator iter = v.crbegin(); iter != v.crend();
         ++iter) {
      counter--;
      EXPECT_EQ(counter, *iter);
    }
    EXPECT_EQ(counter, 0);
  }
}

TEST(IntVec, Erase) {
  for (int len = 1; len < 20; len++) {
    for (int i = 0; i < len; ++i) {
      IntVec v;
      Fill(&v, len);
      v.erase(v.begin() + i);
      EXPECT_EQ(len - 1, v.size());
      for (int j = 0; j < i; ++j) {
        EXPECT_EQ(j, v[j]);
      }
      for (int j = i; j < len - 1; ++j) {
        EXPECT_EQ(j + 1, v[j]);
      }
    }
  }
}

TEST(IntVec, Hardened) {
  IntVec v;
  Fill(&v, 10);
  EXPECT_EQ(v[9], 9);
#if !defined(NDEBUG) || ABSL_OPTION_HARDENED
  EXPECT_DEATH_IF_SUPPORTED(v[10], "");
  EXPECT_DEATH_IF_SUPPORTED(v[-1], "");
#endif
}

// At the end of this test loop, the elements between [erase_begin, erase_end)
// should have reference counts == 0, and all others elements should have
// reference counts == 1.
TEST(RefCountedVec, EraseBeginEnd) {
  for (int len = 1; len < 20; ++len) {
    for (int erase_begin = 0; erase_begin < len; ++erase_begin) {
      for (int erase_end = erase_begin; erase_end <= len; ++erase_end) {
        std::vector<int> counts(len, 0);
        RefCountedVec v;
        for (int i = 0; i < len; ++i) {
          v.push_back(RefCounted(i, &counts[i]));
        }

        int erase_len = erase_end - erase_begin;

        v.erase(v.begin() + erase_begin, v.begin() + erase_end);

        EXPECT_EQ(len - erase_len, v.size());

        // Check the elements before the first element erased.
        for (int i = 0; i < erase_begin; ++i) {
          EXPECT_EQ(i, v[i].value_);
        }

        // Check the elements after the first element erased.
        for (int i = erase_begin; i < v.size(); ++i) {
          EXPECT_EQ(i + erase_len, v[i].value_);
        }

        // Check that the elements at the beginning are preserved.
        for (int i = 0; i < erase_begin; ++i) {
          EXPECT_EQ(1, counts[i]);
        }

        // Check that the erased elements are destroyed
        for (int i = erase_begin; i < erase_end; ++i) {
          EXPECT_EQ(0, counts[i]);
        }

        // Check that the elements at the end are preserved.
        for (int i = erase_end; i < len; ++i) {
          EXPECT_EQ(1, counts[i]);
        }
      }
    }
  }
}

struct NoDefaultCtor {
  explicit NoDefaultCtor(int) {}
};
struct NoCopy {
  NoCopy() {}
  NoCopy(const NoCopy&) = delete;
};
struct NoAssign {
  NoAssign() {}
  NoAssign& operator=(const NoAssign&) = delete;
};
struct MoveOnly {
  MoveOnly() {}
  MoveOnly(MoveOnly&&) = default;
  MoveOnly& operator=(MoveOnly&&) = default;
};
TEST(InlinedVectorTest, NoDefaultCtor) {
  absl::InlinedVector<NoDefaultCtor, 1> v(10, NoDefaultCtor(2));
  (void)v;
}
TEST(InlinedVectorTest, NoCopy) {
  absl::InlinedVector<NoCopy, 1> v(10);
  (void)v;
}
TEST(InlinedVectorTest, NoAssign) {
  absl::InlinedVector<NoAssign, 1> v(10);
  (void)v;
}
TEST(InlinedVectorTest, MoveOnly) {
  absl::InlinedVector<MoveOnly, 2> v;
  v.push_back(MoveOnly{});
  v.push_back(MoveOnly{});
  v.push_back(MoveOnly{});
  v.erase(v.begin());
  v.push_back(MoveOnly{});
  v.erase(v.begin(), v.begin() + 1);
  v.insert(v.begin(), MoveOnly{});
  v.emplace(v.begin());
  v.emplace(v.begin(), MoveOnly{});
}
TEST(InlinedVectorTest, Noexcept) {
  EXPECT_TRUE(std::is_nothrow_move_constructible<IntVec>::value);
  EXPECT_TRUE((std::is_nothrow_move_constructible<
               absl::InlinedVector<MoveOnly, 2>>::value));

  struct MoveCanThrow {
    MoveCanThrow(MoveCanThrow&&) {}
  };
  EXPECT_EQ(absl::default_allocator_is_nothrow::value,
            (std::is_nothrow_move_constructible<
                absl::InlinedVector<MoveCanThrow, 2>>::value));
}

TEST(InlinedVectorTest, EmplaceBack) {
  absl::InlinedVector<std::pair<std::string, int>, 1> v;

  auto& inlined_element = v.emplace_back("answer", 42);
  EXPECT_EQ(&inlined_element, &v[0]);
  EXPECT_EQ(inlined_element.first, "answer");
  EXPECT_EQ(inlined_element.second, 42);

  auto& allocated_element = v.emplace_back("taxicab", 1729);
  EXPECT_EQ(&allocated_element, &v[1]);
  EXPECT_EQ(allocated_element.first, "taxicab");
  EXPECT_EQ(allocated_element.second, 1729);
}

TEST(InlinedVectorTest, ShrinkToFitGrowingVector) {
  absl::InlinedVector<std::pair<std::string, int>, 1> v;

  v.shrink_to_fit();
  EXPECT_EQ(v.capacity(), 1);

  v.emplace_back("answer", 42);
  v.shrink_to_fit();
  EXPECT_EQ(v.capacity(), 1);

  v.emplace_back("taxicab", 1729);
  EXPECT_GE(v.capacity(), 2);
  v.shrink_to_fit();
  EXPECT_EQ(v.capacity(), 2);

  v.reserve(100);
  EXPECT_GE(v.capacity(), 100);
  v.shrink_to_fit();
  EXPECT_EQ(v.capacity(), 2);
}

TEST(InlinedVectorTest, ShrinkToFitEdgeCases) {
  {
    absl::InlinedVector<std::pair<std::string, int>, 1> v;
    v.emplace_back("answer", 42);
    v.emplace_back("taxicab", 1729);
    EXPECT_GE(v.capacity(), 2);
    v.pop_back();
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 1);
    EXPECT_EQ(v[0].first, "answer");
    EXPECT_EQ(v[0].second, 42);
  }

  {
    absl::InlinedVector<std::string, 2> v(100);
    v.resize(0);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 2);  // inlined capacity
  }

  {
    absl::InlinedVector<std::string, 2> v(100);
    v.resize(1);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 2);  // inlined capacity
  }

  {
    absl::InlinedVector<std::string, 2> v(100);
    v.resize(2);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 2);
  }

  {
    absl::InlinedVector<std::string, 2> v(100);
    v.resize(3);
    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 3);
  }
}

TEST(IntVec, Insert) {
  for (int len = 0; len < 20; len++) {
    for (int pos = 0; pos <= len; pos++) {
      {
        // Single element
        std::vector<int> std_v;
        Fill(&std_v, len);
        IntVec v;
        Fill(&v, len);

        std_v.insert(std_v.begin() + pos, 9999);
        IntVec::iterator it = v.insert(v.cbegin() + pos, 9999);
        EXPECT_THAT(v, ElementsAreArray(std_v));
        EXPECT_EQ(it, v.cbegin() + pos);
      }
      {
        // n elements
        std::vector<int> std_v;
        Fill(&std_v, len);
        IntVec v;
        Fill(&v, len);

        IntVec::size_type n = 5;
        std_v.insert(std_v.begin() + pos, n, 9999);
        IntVec::iterator it = v.insert(v.cbegin() + pos, n, 9999);
        EXPECT_THAT(v, ElementsAreArray(std_v));
        EXPECT_EQ(it, v.cbegin() + pos);
      }
      {
        // Iterator range (random access iterator)
        std::vector<int> std_v;
        Fill(&std_v, len);
        IntVec v;
        Fill(&v, len);

        const std::vector<int> input = {9999, 8888, 7777};
        std_v.insert(std_v.begin() + pos, input.cbegin(), input.cend());
        IntVec::iterator it =
            v.insert(v.cbegin() + pos, input.cbegin(), input.cend());
        EXPECT_THAT(v, ElementsAreArray(std_v));
        EXPECT_EQ(it, v.cbegin() + pos);
      }
      {
        // Iterator range (forward iterator)
        std::vector<int> std_v;
        Fill(&std_v, len);
        IntVec v;
        Fill(&v, len);

        const std::forward_list<int> input = {9999, 8888, 7777};
        std_v.insert(std_v.begin() + pos, input.cbegin(), input.cend());
        IntVec::iterator it =
            v.insert(v.cbegin() + pos, input.cbegin(), input.cend());
        EXPECT_THAT(v, ElementsAreArray(std_v));
        EXPECT_EQ(it, v.cbegin() + pos);
      }
      {
        // Iterator range (input iterator)
        std::vector<int> std_v;
        Fill(&std_v, len);
        IntVec v;
        Fill(&v, len);

        std_v.insert(std_v.begin() + pos, {9999, 8888, 7777});
        std::istringstream input("9999 8888 7777");
        IntVec::iterator it =
            v.insert(v.cbegin() + pos, std::istream_iterator<int>(input),
                     std::istream_iterator<int>());
        EXPECT_THAT(v, ElementsAreArray(std_v));
        EXPECT_EQ(it, v.cbegin() + pos);
      }
      {
        // Initializer list
        std::vector<int> std_v;
        Fill(&std_v, len);
        IntVec v;
        Fill(&v, len);

        std_v.insert(std_v.begin() + pos, {9999, 8888});
        IntVec::iterator it = v.insert(v.cbegin() + pos, {9999, 8888});
        EXPECT_THAT(v, ElementsAreArray(std_v));
        EXPECT_EQ(it, v.cbegin() + pos);
      }
    }
  }
}

TEST(RefCountedVec, InsertConstructorDestructor) {
  // Make sure the proper construction/destruction happen during insert
  // operations.
  for (int len = 0; len < 20; len++) {
    SCOPED_TRACE(len);
    for (int pos = 0; pos <= len; pos++) {
      SCOPED_TRACE(pos);
      std::vector<int> counts(len, 0);
      int inserted_count = 0;
      RefCountedVec v;
      for (int i = 0; i < len; ++i) {
        SCOPED_TRACE(i);
        v.push_back(RefCounted(i, &counts[i]));
      }

      EXPECT_THAT(counts, Each(Eq(1)));

      RefCounted insert_element(9999, &inserted_count);
      EXPECT_EQ(1, inserted_count);
      v.insert(v.begin() + pos, insert_element);
      EXPECT_EQ(2, inserted_count);
      // Check that the elements at the end are preserved.
      EXPECT_THAT(counts, Each(Eq(1)));
      EXPECT_EQ(2, inserted_count);
    }
  }
}

TEST(IntVec, Resize) {
  for (int len = 0; len < 20; len++) {
    IntVec v;
    Fill(&v, len);

    // Try resizing up and down by k elements
    static const int kResizeElem = 1000000;
    for (int k = 0; k < 10; k++) {
      // Enlarging resize
      v.resize(len + k, kResizeElem);
      EXPECT_EQ(len + k, v.size());
      EXPECT_LE(len + k, v.capacity());
      for (int i = 0; i < len + k; i++) {
        if (i < len) {
          EXPECT_EQ(i, v[i]);
        } else {
          EXPECT_EQ(kResizeElem, v[i]);
        }
      }

      // Shrinking resize
      v.resize(len, kResizeElem);
      EXPECT_EQ(len, v.size());
      EXPECT_LE(len, v.capacity());
      for (int i = 0; i < len; i++) {
        EXPECT_EQ(i, v[i]);
      }
    }
  }
}

TEST(IntVec, InitWithLength) {
  for (int len = 0; len < 20; len++) {
    IntVec v(len, 7);
    EXPECT_EQ(len, v.size());
    EXPECT_LE(len, v.capacity());
    for (int i = 0; i < len; i++) {
      EXPECT_EQ(7, v[i]);
    }
  }
}

TEST(IntVec, CopyConstructorAndAssignment) {
  for (int len = 0; len < 20; len++) {
    IntVec v;
    Fill(&v, len);
    EXPECT_EQ(len, v.size());
    EXPECT_LE(len, v.capacity());

    IntVec v2(v);
    EXPECT_TRUE(v == v2) << PrintToString(v) << PrintToString(v2);

    for (int start_len = 0; start_len < 20; start_len++) {
      IntVec v3;
      Fill(&v3, start_len, 99);  // Add dummy elements that should go away
      v3 = v;
      EXPECT_TRUE(v == v3) << PrintToString(v) << PrintToString(v3);
    }
  }
}

TEST(IntVec, AliasingCopyAssignment) {
  for (int len = 0; len < 20; ++len) {
    IntVec original;
    Fill(&original, len);
    IntVec dup = original;
    dup = *&dup;
    EXPECT_EQ(dup, original);
  }
}

TEST(IntVec, MoveConstructorAndAssignment) {
  for (int len = 0; len < 20; len++) {
    IntVec v_in;
    const int inlined_capacity = v_in.capacity();
    Fill(&v_in, len);
    EXPECT_EQ(len, v_in.size());
    EXPECT_LE(len, v_in.capacity());

    {
      IntVec v_temp(v_in);
      auto* old_data = v_temp.data();
      IntVec v_out(std::move(v_temp));
      EXPECT_TRUE(v_in == v_out) << PrintToString(v_in) << PrintToString(v_out);
      if (v_in.size() > inlined_capacity) {
        // Allocation is moved as a whole, data stays in place.
        EXPECT_TRUE(v_out.data() == old_data);
      } else {
        EXPECT_FALSE(v_out.data() == old_data);
      }
    }
    for (int start_len = 0; start_len < 20; start_len++) {
      IntVec v_out;
      Fill(&v_out, start_len, 99);  // Add dummy elements that should go away
      IntVec v_temp(v_in);
      auto* old_data = v_temp.data();
      v_out = std::move(v_temp);
      EXPECT_TRUE(v_in == v_out) << PrintToString(v_in) << PrintToString(v_out);
      if (v_in.size() > inlined_capacity) {
        // Allocation is moved as a whole, data stays in place.
        EXPECT_TRUE(v_out.data() == old_data);
      } else {
        EXPECT_FALSE(v_out.data() == old_data);
      }
    }
  }
}

class NotTriviallyDestructible {
 public:
  NotTriviallyDestructible() : p_(new int(1)) {}
  explicit NotTriviallyDestructible(int i) : p_(new int(i)) {}

  NotTriviallyDestructible(const NotTriviallyDestructible& other)
      : p_(new int(*other.p_)) {}

  NotTriviallyDestructible& operator=(const NotTriviallyDestructible& other) {
    p_ = absl::make_unique<int>(*other.p_);
    return *this;
  }

  bool operator==(const NotTriviallyDestructible& other) const {
    return *p_ == *other.p_;
  }

 private:
  std::unique_ptr<int> p_;
};

TEST(AliasingTest, Emplace) {
  for (int i = 2; i < 20; ++i) {
    absl::InlinedVector<NotTriviallyDestructible, 10> vec;
    for (int j = 0; j < i; ++j) {
      vec.push_back(NotTriviallyDestructible(j));
    }
    vec.emplace(vec.begin(), vec[0]);
    EXPECT_EQ(vec[0], vec[1]);
    vec.emplace(vec.begin() + i / 2, vec[i / 2]);
    EXPECT_EQ(vec[i / 2], vec[i / 2 + 1]);
    vec.emplace(vec.end() - 1, vec.back());
    EXPECT_EQ(vec[vec.size() - 2], vec.back());
  }
}

TEST(AliasingTest, InsertWithCount) {
  for (int i = 1; i < 20; ++i) {
    absl::InlinedVector<NotTriviallyDestructible, 10> vec;
    for (int j = 0; j < i; ++j) {
      vec.push_back(NotTriviallyDestructible(j));
    }
    for (int n = 0; n < 5; ++n) {
      // We use back where we can because it's guaranteed to become invalidated
      vec.insert(vec.begin(), n, vec.back());
      auto b = vec.begin();
      EXPECT_TRUE(
          std::all_of(b, b + n, [&vec](const NotTriviallyDestructible& x) {
            return x == vec.back();
          }));

      auto m_idx = vec.size() / 2;
      vec.insert(vec.begin() + m_idx, n, vec.back());
      auto m = vec.begin() + m_idx;
      EXPECT_TRUE(
          std::all_of(m, m + n, [&vec](const NotTriviallyDestructible& x) {
            return x == vec.back();
          }));

      // We want distinct values so the equality test is meaningful,
      // vec[vec.size() - 1] is also almost always invalidated.
      auto old_e = vec.size() - 1;
      auto val = vec[old_e];
      vec.insert(vec.end(), n, vec[old_e]);
      auto e = vec.begin() + old_e;
      EXPECT_TRUE(std::all_of(
          e, e + n,
          [&val](const NotTriviallyDestructible& x) { return x == val; }));
    }
  }
}

TEST(OverheadTest, Storage) {
  // Check for size overhead.
  // In particular, ensure that std::allocator doesn't cost anything to store.
  // The union should be absorbing some of the allocation bookkeeping overhead
  // in the larger vectors, leaving only the size_ field as overhead.

  struct T { void* val; };
  size_t expected_overhead = sizeof(T);

  EXPECT_EQ((2 * expected_overhead),
            sizeof(absl::InlinedVector<T, 1>) - sizeof(T[1]));
  EXPECT_EQ(expected_overhead,
            sizeof(absl::InlinedVector<T, 2>) - sizeof(T[2]));
  EXPECT_EQ(expected_overhead,
            sizeof(absl::InlinedVector<T, 3>) - sizeof(T[3]));
  EXPECT_EQ(expected_overhead,
            sizeof(absl::InlinedVector<T, 4>) - sizeof(T[4]));
  EXPECT_EQ(expected_overhead,
            sizeof(absl::InlinedVector<T, 5>) - sizeof(T[5]));
  EXPECT_EQ(expected_overhead,
            sizeof(absl::InlinedVector<T, 6>) - sizeof(T[6]));
  EXPECT_EQ(expected_overhead,
            sizeof(absl::InlinedVector<T, 7>) - sizeof(T[7]));
  EXPECT_EQ(expected_overhead,
            sizeof(absl::InlinedVector<T, 8>) - sizeof(T[8]));
}

TEST(IntVec, Clear) {
  for (int len = 0; len < 20; len++) {
    SCOPED_TRACE(len);
    IntVec v;
    Fill(&v, len);
    v.clear();
    EXPECT_EQ(0, v.size());
    EXPECT_EQ(v.begin(), v.end());
  }
}

TEST(IntVec, Reserve) {
  for (int len = 0; len < 20; len++) {
    IntVec v;
    Fill(&v, len);

    for (int newlen = 0; newlen < 100; newlen++) {
      const int* start_rep = v.data();
      v.reserve(newlen);
      const int* final_rep = v.data();
      if (newlen <= len) {
        EXPECT_EQ(start_rep, final_rep);
      }
      EXPECT_LE(newlen, v.capacity());

      // Filling up to newlen should not change rep
      while (v.size() < newlen) {
        v.push_back(0);
      }
      EXPECT_EQ(final_rep, v.data());
    }
  }
}

TEST(StringVec, SelfRefPushBack) {
  std::vector<std::string> std_v;
  absl::InlinedVector<std::string, 4> v;
  const std::string s = "A quite long string to ensure heap.";
  std_v.push_back(s);
  v.push_back(s);
  for (int i = 0; i < 20; ++i) {
    EXPECT_THAT(v, ElementsAreArray(std_v));

    v.push_back(v.back());
    std_v.push_back(std_v.back());
  }
  EXPECT_THAT(v, ElementsAreArray(std_v));
}

TEST(StringVec, SelfRefPushBackWithMove) {
  std::vector<std::string> std_v;
  absl::InlinedVector<std::string, 4> v;
  const std::string s = "A quite long string to ensure heap.";
  std_v.push_back(s);
  v.push_back(s);
  for (int i = 0; i < 20; ++i) {
    EXPECT_EQ(v.back(), std_v.back());

    v.push_back(std::move(v.back()));
    std_v.push_back(std::move(std_v.back()));
  }
  EXPECT_EQ(v.back(), std_v.back());
}

TEST(StringVec, SelfMove) {
  const std::string s = "A quite long string to ensure heap.";
  for (int len = 0; len < 20; len++) {
    SCOPED_TRACE(len);
    absl::InlinedVector<std::string, 8> v;
    for (int i = 0; i < len; ++i) {
      SCOPED_TRACE(i);
      v.push_back(s);
    }
    // Indirection necessary to avoid compiler warning.
    v = std::move(*(&v));
    // Ensure that the inlined vector is still in a valid state by copying it.
    // We don't expect specific contents since a self-move results in an
    // unspecified valid state.
    std::vector<std::string> copy(v.begin(), v.end());
  }
}

TEST(IntVec, Swap) {
  for (int l1 = 0; l1 < 20; l1++) {
    SCOPED_TRACE(l1);
    for (int l2 = 0; l2 < 20; l2++) {
      SCOPED_TRACE(l2);
      IntVec a = Fill(l1, 0);
      IntVec b = Fill(l2, 100);
      {
        using std::swap;
        swap(a, b);
      }
      EXPECT_EQ(l1, b.size());
      EXPECT_EQ(l2, a.size());
      for (int i = 0; i < l1; i++) {
        SCOPED_TRACE(i);
        EXPECT_EQ(i, b[i]);
      }
      for (int i = 0; i < l2; i++) {
        SCOPED_TRACE(i);
        EXPECT_EQ(100 + i, a[i]);
      }
    }
  }
}

TYPED_TEST_P(InstanceTest, Swap) {
  using Instance = TypeParam;
  using InstanceVec = absl::InlinedVector<Instance, 8>;
  for (int l1 = 0; l1 < 20; l1++) {
    SCOPED_TRACE(l1);
    for (int l2 = 0; l2 < 20; l2++) {
      SCOPED_TRACE(l2);
      InstanceTracker tracker;
      InstanceVec a, b;
      const size_t inlined_capacity = a.capacity();
      auto min_len = std::min(l1, l2);
      auto max_len = std::max(l1, l2);
      for (int i = 0; i < l1; i++) a.push_back(Instance(i));
      for (int i = 0; i < l2; i++) b.push_back(Instance(100 + i));
      EXPECT_EQ(tracker.instances(), l1 + l2);
      tracker.ResetCopiesMovesSwaps();
      {
        using std::swap;
        swap(a, b);
      }
      EXPECT_EQ(tracker.instances(), l1 + l2);
      if (a.size() > inlined_capacity && b.size() > inlined_capacity) {
        EXPECT_EQ(tracker.swaps(), 0);  // Allocations are swapped.
        EXPECT_EQ(tracker.moves(), 0);
      } else if (a.size() <= inlined_capacity && b.size() <= inlined_capacity) {
        EXPECT_EQ(tracker.swaps(), min_len);
        EXPECT_EQ((tracker.moves() ? tracker.moves() : tracker.copies()),
                  max_len - min_len);
      } else {
        // One is allocated and the other isn't. The allocation is transferred
        // without copying elements, and the inlined instances are copied/moved.
        EXPECT_EQ(tracker.swaps(), 0);
        EXPECT_EQ((tracker.moves() ? tracker.moves() : tracker.copies()),
                  min_len);
      }

      EXPECT_EQ(l1, b.size());
      EXPECT_EQ(l2, a.size());
      for (int i = 0; i < l1; i++) {
        EXPECT_EQ(i, b[i].value());
      }
      for (int i = 0; i < l2; i++) {
        EXPECT_EQ(100 + i, a[i].value());
      }
    }
  }
}

TEST(IntVec, EqualAndNotEqual) {
  IntVec a, b;
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);

  a.push_back(3);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);

  b.push_back(3);
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);

  b.push_back(7);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);

  a.push_back(6);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);

  a.clear();
  b.clear();
  for (int i = 0; i < 100; i++) {
    a.push_back(i);
    b.push_back(i);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);

    b[i] = b[i] + 1;
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);

    b[i] = b[i] - 1;  // Back to before
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
  }
}

TEST(IntVec, RelationalOps) {
  IntVec a, b;
  EXPECT_FALSE(a < b);
  EXPECT_FALSE(b < a);
  EXPECT_FALSE(a > b);
  EXPECT_FALSE(b > a);
  EXPECT_TRUE(a <= b);
  EXPECT_TRUE(b <= a);
  EXPECT_TRUE(a >= b);
  EXPECT_TRUE(b >= a);
  b.push_back(3);
  EXPECT_TRUE(a < b);
  EXPECT_FALSE(b < a);
  EXPECT_FALSE(a > b);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(b <= a);
  EXPECT_FALSE(a >= b);
  EXPECT_TRUE(b >= a);
}

TYPED_TEST_P(InstanceTest, CountConstructorsDestructors) {
  using Instance = TypeParam;
  using InstanceVec = absl::InlinedVector<Instance, 8>;
  InstanceTracker tracker;
  for (int len = 0; len < 20; len++) {
    SCOPED_TRACE(len);
    tracker.ResetCopiesMovesSwaps();

    InstanceVec v;
    const size_t inlined_capacity = v.capacity();
    for (int i = 0; i < len; i++) {
      v.push_back(Instance(i));
    }
    EXPECT_EQ(tracker.instances(), len);
    EXPECT_GE(tracker.copies() + tracker.moves(),
              len);  // More due to reallocation.
    tracker.ResetCopiesMovesSwaps();

    // Enlarging resize() must construct some objects
    tracker.ResetCopiesMovesSwaps();
    v.resize(len + 10, Instance(100));
    EXPECT_EQ(tracker.instances(), len + 10);
    if (len <= inlined_capacity && len + 10 > inlined_capacity) {
      EXPECT_EQ(tracker.copies() + tracker.moves(), 10 + len);
    } else {
      // Only specify a minimum number of copies + moves. We don't want to
      // depend on the reallocation policy here.
      EXPECT_GE(tracker.copies() + tracker.moves(),
                10);  // More due to reallocation.
    }

    // Shrinking resize() must destroy some objects
    tracker.ResetCopiesMovesSwaps();
    v.resize(len, Instance(100));
    EXPECT_EQ(tracker.instances(), len);
    EXPECT_EQ(tracker.copies(), 0);
    EXPECT_EQ(tracker.moves(), 0);

    // reserve() must not increase the number of initialized objects
    SCOPED_TRACE("reserve");
    v.reserve(len + 1000);
    EXPECT_EQ(tracker.instances(), len);
    EXPECT_EQ(tracker.copies() + tracker.moves(), len);

    // pop_back() and erase() must destroy one object
    if (len > 0) {
      tracker.ResetCopiesMovesSwaps();
      v.pop_back();
      EXPECT_EQ(tracker.instances(), len - 1);
      EXPECT_EQ(tracker.copies(), 0);
      EXPECT_EQ(tracker.moves(), 0);

      if (!v.empty()) {
        tracker.ResetCopiesMovesSwaps();
        v.erase(v.begin());
        EXPECT_EQ(tracker.instances(), len - 2);
        EXPECT_EQ(tracker.copies() + tracker.moves(), len - 2);
      }
    }

    tracker.ResetCopiesMovesSwaps();
    int instances_before_empty_erase = tracker.instances();
    v.erase(v.begin(), v.begin());
    EXPECT_EQ(tracker.instances(), instances_before_empty_erase);
    EXPECT_EQ(tracker.copies() + tracker.moves(), 0);
  }
}

TYPED_TEST_P(InstanceTest, CountConstructorsDestructorsOnCopyConstruction) {
  using Instance = TypeParam;
  using InstanceVec = absl::InlinedVector<Instance, 8>;
  InstanceTracker tracker;
  for (int len = 0; len < 20; len++) {
    SCOPED_TRACE(len);
    tracker.ResetCopiesMovesSwaps();

    InstanceVec v;
    for (int i = 0; i < len; i++) {
      v.push_back(Instance(i));
    }
    EXPECT_EQ(tracker.instances(), len);
    EXPECT_GE(tracker.copies() + tracker.moves(),
              len);  // More due to reallocation.
    tracker.ResetCopiesMovesSwaps();
    {  // Copy constructor should create 'len' more instances.
      InstanceVec v_copy(v);
      EXPECT_EQ(tracker.instances(), len + len);
      EXPECT_EQ(tracker.copies(), len);
      EXPECT_EQ(tracker.moves(), 0);
    }
    EXPECT_EQ(tracker.instances(), len);
  }
}

TYPED_TEST_P(InstanceTest, CountConstructorsDestructorsOnMoveConstruction) {
  using Instance = TypeParam;
  using InstanceVec = absl::InlinedVector<Instance, 8>;
  InstanceTracker tracker;
  for (int len = 0; len < 20; len++) {
    SCOPED_TRACE(len);
    tracker.ResetCopiesMovesSwaps();

    InstanceVec v;
    const size_t inlined_capacity = v.capacity();
    for (int i = 0; i < len; i++) {
      v.push_back(Instance(i));
    }
    EXPECT_EQ(tracker.instances(), len);
    EXPECT_GE(tracker.copies() + tracker.moves(),
              len);  // More due to reallocation.
    tracker.ResetCopiesMovesSwaps();
    {
      InstanceVec v_copy(std::move(v));
      if (len > inlined_capacity) {
        // Allocation is moved as a whole.
        EXPECT_EQ(tracker.instances(), len);
        EXPECT_EQ(tracker.live_instances(), len);
        // Tests an implementation detail, don't rely on this in your code.
        EXPECT_EQ(v.size(), 0);  // NOLINT misc-use-after-move
        EXPECT_EQ(tracker.copies(), 0);
        EXPECT_EQ(tracker.moves(), 0);
      } else {
        EXPECT_EQ(tracker.instances(), len + len);
        if (Instance::supports_move()) {
          EXPECT_EQ(tracker.live_instances(), len);
          EXPECT_EQ(tracker.copies(), 0);
          EXPECT_EQ(tracker.moves(), len);
        } else {
          EXPECT_EQ(tracker.live_instances(), len + len);
          EXPECT_EQ(tracker.copies(), len);
          EXPECT_EQ(tracker.moves(), 0);
        }
      }
      EXPECT_EQ(tracker.swaps(), 0);
    }
  }
}

TYPED_TEST_P(InstanceTest, CountConstructorsDestructorsOnAssignment) {
  using Instance = TypeParam;
  using InstanceVec = absl::InlinedVector<Instance, 8>;
  InstanceTracker tracker;
  for (int len = 0; len < 20; len++) {
    SCOPED_TRACE(len);
    for (int longorshort = 0; longorshort <= 1; ++longorshort) {
      SCOPED_TRACE(longorshort);
      tracker.ResetCopiesMovesSwaps();

      InstanceVec longer, shorter;
      for (int i = 0; i < len; i++) {
        longer.push_back(Instance(i));
        shorter.push_back(Instance(i));
      }
      longer.push_back(Instance(len));
      EXPECT_EQ(tracker.instances(), len + len + 1);
      EXPECT_GE(tracker.copies() + tracker.moves(),
                len + len + 1);  // More due to reallocation.

      tracker.ResetCopiesMovesSwaps();
      if (longorshort) {
        shorter = longer;
        EXPECT_EQ(tracker.instances(), (len + 1) + (len + 1));
        EXPECT_GE(tracker.copies() + tracker.moves(),
                  len + 1);  // More due to reallocation.
      } else {
        longer = shorter;
        EXPECT_EQ(tracker.instances(), len + len);
        EXPECT_EQ(tracker.copies() + tracker.moves(), len);
      }
    }
  }
}

TYPED_TEST_P(InstanceTest, CountConstructorsDestructorsOnMoveAssignment) {
  using Instance = TypeParam;
  using InstanceVec = absl::InlinedVector<Instance, 8>;
  InstanceTracker tracker;
  for (int len = 0; len < 20; len++) {
    SCOPED_TRACE(len);
    for (int longorshort = 0; longorshort <= 1; ++longorshort) {
      SCOPED_TRACE(longorshort);
      tracker.ResetCopiesMovesSwaps();

      InstanceVec longer, shorter;
      const int inlined_capacity = longer.capacity();
      for (int i = 0; i < len; i++) {
        longer.push_back(Instance(i));
        shorter.push_back(Instance(i));
      }
      longer.push_back(Instance(len));
      EXPECT_EQ(tracker.instances(), len + len + 1);
      EXPECT_GE(tracker.copies() + tracker.moves(),
                len + len + 1);  // More due to reallocation.

      tracker.ResetCopiesMovesSwaps();
      int src_len;
      if (longorshort) {
        src_len = len + 1;
        shorter = std::move(longer);
      } else {
        src_len = len;
        longer = std::move(shorter);
      }
      if (src_len > inlined_capacity) {
        // Allocation moved as a whole.
        EXPECT_EQ(tracker.instances(), src_len);
        EXPECT_EQ(tracker.live_instances(), src_len);
        EXPECT_EQ(tracker.copies(), 0);
        EXPECT_EQ(tracker.moves(), 0);
      } else {
        // Elements are all copied.
        EXPECT_EQ(tracker.instances(), src_len + src_len);
        if (Instance::supports_move()) {
          EXPECT_EQ(tracker.copies(), 0);
          EXPECT_EQ(tracker.moves(), src_len);
          EXPECT_EQ(tracker.live_instances(), src_len);
        } else {
          EXPECT_EQ(tracker.copies(), src_len);
          EXPECT_EQ(tracker.moves(), 0);
          EXPECT_EQ(tracker.live_instances(), src_len + src_len);
        }
      }
      EXPECT_EQ(tracker.swaps(), 0);
    }
  }
}

TEST(CountElemAssign, SimpleTypeWithInlineBacking) {
  for (size_t original_size = 0; original_size <= 5; ++original_size) {
    SCOPED_TRACE(original_size);
    // Original contents are [12345, 12345, ...]
    std::vector<int> original_contents(original_size, 12345);

    absl::InlinedVector<int, 2> v(original_contents.begin(),
                                  original_contents.end());
    v.assign(2, 123);
    EXPECT_THAT(v, AllOf(SizeIs(2), ElementsAre(123, 123)));
    if (original_size <= 2) {
      // If the original had inline backing, it should stay inline.
      EXPECT_EQ(2, v.capacity());
    }
  }
}

TEST(CountElemAssign, SimpleTypeWithAllocation) {
  for (size_t original_size = 0; original_size <= 5; ++original_size) {
    SCOPED_TRACE(original_size);
    // Original contents are [12345, 12345, ...]
    std::vector<int> original_contents(original_size, 12345);

    absl::InlinedVector<int, 2> v(original_contents.begin(),
                                  original_contents.end());
    v.assign(3, 123);
    EXPECT_THAT(v, AllOf(SizeIs(3), ElementsAre(123, 123, 123)));
    EXPECT_LE(v.size(), v.capacity());
  }
}

TYPED_TEST_P(InstanceTest, CountElemAssignInlineBacking) {
  using Instance = TypeParam;
  for (size_t original_size = 0; original_size <= 5; ++original_size) {
    SCOPED_TRACE(original_size);
    // Original contents are [12345, 12345, ...]
    std::vector<Instance> original_contents(original_size, Instance(12345));

    absl::InlinedVector<Instance, 2> v(original_contents.begin(),
                                       original_contents.end());
    v.assign(2, Instance(123));
    EXPECT_THAT(v, AllOf(SizeIs(2), ElementsAre(ValueIs(123), ValueIs(123))));
    if (original_size <= 2) {
      // If the original had inline backing, it should stay inline.
      EXPECT_EQ(2, v.capacity());
    }
  }
}

template <typename Instance>
void InstanceCountElemAssignWithAllocationTest() {
  for (size_t original_size = 0; original_size <= 5; ++original_size) {
    SCOPED_TRACE(original_size);
    // Original contents are [12345, 12345, ...]
    std::vector<Instance> original_contents(original_size, Instance(12345));

    absl::InlinedVector<Instance, 2> v(original_contents.begin(),
                                       original_contents.end());
    v.assign(3, Instance(123));
    EXPECT_THAT(v, AllOf(SizeIs(3), ElementsAre(ValueIs(123), ValueIs(123),
                                                ValueIs(123))));
    EXPECT_LE(v.size(), v.capacity());
  }
}
TEST(CountElemAssign, WithAllocationCopyableInstance) {
  InstanceCountElemAssignWithAllocationTest<CopyableOnlyInstance>();
}
TEST(CountElemAssign, WithAllocationCopyableMovableInstance) {
  InstanceCountElemAssignWithAllocationTest<CopyableMovableInstance>();
}

TEST(RangedConstructor, SimpleType) {
  std::vector<int> source_v = {4, 5, 6};
  // First try to fit in inline backing
  absl::InlinedVector<int, 4> v(source_v.begin(), source_v.end());
  EXPECT_EQ(3, v.size());
  EXPECT_EQ(4, v.capacity());  // Indication that we're still on inlined storage
  EXPECT_EQ(4, v[0]);
  EXPECT_EQ(5, v[1]);
  EXPECT_EQ(6, v[2]);

  // Now, force a re-allocate
  absl::InlinedVector<int, 2> realloc_v(source_v.begin(), source_v.end());
  EXPECT_EQ(3, realloc_v.size());
  EXPECT_LT(2, realloc_v.capacity());
  EXPECT_EQ(4, realloc_v[0]);
  EXPECT_EQ(5, realloc_v[1]);
  EXPECT_EQ(6, realloc_v[2]);
}

// Test for ranged constructors using Instance as the element type and
// SourceContainer as the source container type.
template <typename Instance, typename SourceContainer, int inlined_capacity>
void InstanceRangedConstructorTestForContainer() {
  InstanceTracker tracker;
  SourceContainer source_v = {Instance(0), Instance(1)};
  tracker.ResetCopiesMovesSwaps();
  absl::InlinedVector<Instance, inlined_capacity> v(source_v.begin(),
                                                    source_v.end());
  EXPECT_EQ(2, v.size());
  EXPECT_LT(1, v.capacity());
  EXPECT_EQ(0, v[0].value());
  EXPECT_EQ(1, v[1].value());
  EXPECT_EQ(tracker.copies(), 2);
  EXPECT_EQ(tracker.moves(), 0);
}

template <typename Instance, int inlined_capacity>
void InstanceRangedConstructorTestWithCapacity() {
  // Test with const and non-const, random access and non-random-access sources.
  // TODO(bsamwel): Test with an input iterator source.
  {
    SCOPED_TRACE("std::list");
    InstanceRangedConstructorTestForContainer<Instance, std::list<Instance>,
                                              inlined_capacity>();
    {
      SCOPED_TRACE("const std::list");
      InstanceRangedConstructorTestForContainer<
          Instance, const std::list<Instance>, inlined_capacity>();
    }
    {
      SCOPED_TRACE("std::vector");
      InstanceRangedConstructorTestForContainer<Instance, std::vector<Instance>,
                                                inlined_capacity>();
    }
    {
      SCOPED_TRACE("const std::vector");
      InstanceRangedConstructorTestForContainer<
          Instance, const std::vector<Instance>, inlined_capacity>();
    }
  }
}

TYPED_TEST_P(InstanceTest, RangedConstructor) {
  using Instance = TypeParam;
  SCOPED_TRACE("capacity=1");
  InstanceRangedConstructorTestWithCapacity<Instance, 1>();
  SCOPED_TRACE("capacity=2");
  InstanceRangedConstructorTestWithCapacity<Instance, 2>();
}

TEST(RangedConstructor, ElementsAreConstructed) {
  std::vector<std::string> source_v = {"cat", "dog"};

  // Force expansion and re-allocation of v.  Ensures that when the vector is
  // expanded that new elements are constructed.
  absl::InlinedVector<std::string, 1> v(source_v.begin(), source_v.end());
  EXPECT_EQ("cat", v[0]);
  EXPECT_EQ("dog", v[1]);
}

TEST(RangedAssign, SimpleType) {
  // Test for all combinations of original sizes (empty and non-empty inline,
  // and out of line) and target sizes.
  for (size_t original_size = 0; original_size <= 5; ++original_size) {
    SCOPED_TRACE(original_size);
    // Original contents are [12345, 12345, ...]
    std::vector<int> original_contents(original_size, 12345);

    for (size_t target_size = 0; target_size <= 5; ++target_size) {
      SCOPED_TRACE(target_size);

      // New contents are [3, 4, ...]
      std::vector<int> new_contents;
      for (size_t i = 0; i < target_size; ++i) {
        new_contents.push_back(i + 3);
      }

      absl::InlinedVector<int, 3> v(original_contents.begin(),
                                    original_contents.end());
      v.assign(new_contents.begin(), new_contents.end());

      EXPECT_EQ(new_contents.size(), v.size());
      EXPECT_LE(new_contents.size(), v.capacity());
      if (target_size <= 3 && original_size <= 3) {
        // Storage should stay inline when target size is small.
        EXPECT_EQ(3, v.capacity());
      }
      EXPECT_THAT(v, ElementsAreArray(new_contents));
    }
  }
}

// Returns true if lhs and rhs have the same value.
template <typename Instance>
static bool InstanceValuesEqual(const Instance& lhs, const Instance& rhs) {
  return lhs.value() == rhs.value();
}

// Test for ranged assign() using Instance as the element type and
// SourceContainer as the source container type.
template <typename Instance, typename SourceContainer>
void InstanceRangedAssignTestForContainer() {
  // Test for all combinations of original sizes (empty and non-empty inline,
  // and out of line) and target sizes.
  for (size_t original_size = 0; original_size <= 5; ++original_size) {
    SCOPED_TRACE(original_size);
    // Original contents are [12345, 12345, ...]
    std::vector<Instance> original_contents(original_size, Instance(12345));

    for (size_t target_size = 0; target_size <= 5; ++target_size) {
      SCOPED_TRACE(target_size);

      // New contents are [3, 4, ...]
      // Generate data using a non-const container, because SourceContainer
      // itself may be const.
      // TODO(bsamwel): Test with an input iterator.
      std::vector<Instance> new_contents_in;
      for (size_t i = 0; i < target_size; ++i) {
        new_contents_in.push_back(Instance(i + 3));
      }
      SourceContainer new_contents(new_contents_in.begin(),
                                   new_contents_in.end());

      absl::InlinedVector<Instance, 3> v(original_contents.begin(),
                                         original_contents.end());
      v.assign(new_contents.begin(), new_contents.end());

      EXPECT_EQ(new_contents.size(), v.size());
      EXPECT_LE(new_contents.size(), v.capacity());
      if (target_size <= 3 && original_size <= 3) {
        // Storage should stay inline when target size is small.
        EXPECT_EQ(3, v.capacity());
      }
      EXPECT_TRUE(std::equal(v.begin(), v.end(), new_contents.begin(),
                             InstanceValuesEqual<Instance>));
    }
  }
}

TYPED_TEST_P(InstanceTest, RangedAssign) {
  using Instance = TypeParam;
  // Test with const and non-const, random access and non-random-access sources.
  // TODO(bsamwel): Test with an input iterator source.
  SCOPED_TRACE("std::list");
  InstanceRangedAssignTestForContainer<Instance, std::list<Instance>>();
  SCOPED_TRACE("const std::list");
  InstanceRangedAssignTestForContainer<Instance, const std::list<Instance>>();
  SCOPED_TRACE("std::vector");
  InstanceRangedAssignTestForContainer<Instance, std::vector<Instance>>();
  SCOPED_TRACE("const std::vector");
  InstanceRangedAssignTestForContainer<Instance, const std::vector<Instance>>();
}

TEST(InitializerListConstructor, SimpleTypeWithInlineBacking) {
  EXPECT_THAT((absl::InlinedVector<int, 4>{4, 5, 6}),
              AllOf(SizeIs(3), CapacityIs(4), ElementsAre(4, 5, 6)));
}

TEST(InitializerListConstructor, SimpleTypeWithReallocationRequired) {
  EXPECT_THAT((absl::InlinedVector<int, 2>{4, 5, 6}),
              AllOf(SizeIs(3), CapacityIs(Gt(2)), ElementsAre(4, 5, 6)));
}

TEST(InitializerListConstructor, DisparateTypesInList) {
  EXPECT_THAT((absl::InlinedVector<int, 2>{-7, 8ULL}), ElementsAre(-7, 8));

  EXPECT_THAT((absl::InlinedVector<std::string, 2>{"foo", std::string("bar")}),
              ElementsAre("foo", "bar"));
}

TEST(InitializerListConstructor, ComplexTypeWithInlineBacking) {
  EXPECT_THAT((absl::InlinedVector<CopyableMovableInstance, 1>{
                  CopyableMovableInstance(0)}),
              AllOf(SizeIs(1), CapacityIs(1), ElementsAre(ValueIs(0))));
}

TEST(InitializerListConstructor, ComplexTypeWithReallocationRequired) {
  EXPECT_THAT(
      (absl::InlinedVector<CopyableMovableInstance, 1>{
          CopyableMovableInstance(0), CopyableMovableInstance(1)}),
      AllOf(SizeIs(2), CapacityIs(Gt(1)), ElementsAre(ValueIs(0), ValueIs(1))));
}

TEST(InitializerListAssign, SimpleTypeFitsInlineBacking) {
  for (size_t original_size = 0; original_size <= 4; ++original_size) {
    SCOPED_TRACE(original_size);

    absl::InlinedVector<int, 2> v1(original_size, 12345);
    const size_t original_capacity_v1 = v1.capacity();
    v1.assign({3});
    EXPECT_THAT(
        v1, AllOf(SizeIs(1), CapacityIs(original_capacity_v1), ElementsAre(3)));

    absl::InlinedVector<int, 2> v2(original_size, 12345);
    const size_t original_capacity_v2 = v2.capacity();
    v2 = {3};
    EXPECT_THAT(
        v2, AllOf(SizeIs(1), CapacityIs(original_capacity_v2), ElementsAre(3)));
  }
}

TEST(InitializerListAssign, SimpleTypeDoesNotFitInlineBacking) {
  for (size_t original_size = 0; original_size <= 4; ++original_size) {
    SCOPED_TRACE(original_size);
    absl::InlinedVector<int, 2> v1(original_size, 12345);
    v1.assign({3, 4, 5});
    EXPECT_THAT(v1, AllOf(SizeIs(3), ElementsAre(3, 4, 5)));
    EXPECT_LE(3, v1.capacity());

    absl::InlinedVector<int, 2> v2(original_size, 12345);
    v2 = {3, 4, 5};
    EXPECT_THAT(v2, AllOf(SizeIs(3), ElementsAre(3, 4, 5)));
    EXPECT_LE(3, v2.capacity());
  }
}

TEST(InitializerListAssign, DisparateTypesInList) {
  absl::InlinedVector<int, 2> v_int1;
  v_int1.assign({-7, 8ULL});
  EXPECT_THAT(v_int1, ElementsAre(-7, 8));

  absl::InlinedVector<int, 2> v_int2;
  v_int2 = {-7, 8ULL};
  EXPECT_THAT(v_int2, ElementsAre(-7, 8));

  absl::InlinedVector<std::string, 2> v_string1;
  v_string1.assign({"foo", std::string("bar")});
  EXPECT_THAT(v_string1, ElementsAre("foo", "bar"));

  absl::InlinedVector<std::string, 2> v_string2;
  v_string2 = {"foo", std::string("bar")};
  EXPECT_THAT(v_string2, ElementsAre("foo", "bar"));
}

TYPED_TEST_P(InstanceTest, InitializerListAssign) {
  using Instance = TypeParam;
  for (size_t original_size = 0; original_size <= 4; ++original_size) {
    SCOPED_TRACE(original_size);
    absl::InlinedVector<Instance, 2> v(original_size, Instance(12345));
    const size_t original_capacity = v.capacity();
    v.assign({Instance(3)});
    EXPECT_THAT(v, AllOf(SizeIs(1), CapacityIs(original_capacity),
                         ElementsAre(ValueIs(3))));
  }
  for (size_t original_size = 0; original_size <= 4; ++original_size) {
    SCOPED_TRACE(original_size);
    absl::InlinedVector<Instance, 2> v(original_size, Instance(12345));
    v.assign({Instance(3), Instance(4), Instance(5)});
    EXPECT_THAT(
        v, AllOf(SizeIs(3), ElementsAre(ValueIs(3), ValueIs(4), ValueIs(5))));
    EXPECT_LE(3, v.capacity());
  }
}

REGISTER_TYPED_TEST_CASE_P(InstanceTest, Swap, CountConstructorsDestructors,
                           CountConstructorsDestructorsOnCopyConstruction,
                           CountConstructorsDestructorsOnMoveConstruction,
                           CountConstructorsDestructorsOnAssignment,
                           CountConstructorsDestructorsOnMoveAssignment,
                           CountElemAssignInlineBacking, RangedConstructor,
                           RangedAssign, InitializerListAssign);

using InstanceTypes =
    ::testing::Types<CopyableOnlyInstance, CopyableMovableInstance>;
INSTANTIATE_TYPED_TEST_CASE_P(InstanceTestOnTypes, InstanceTest, InstanceTypes);

TEST(DynamicVec, DynamicVecCompiles) {
  DynamicVec v;
  (void)v;
}

TEST(AllocatorSupportTest, Constructors) {
  using MyAlloc = CountingAllocator<int>;
  using AllocVec = absl::InlinedVector<int, 4, MyAlloc>;
  const int ia[] = {0, 1, 2, 3, 4, 5, 6, 7};
  int64_t allocated = 0;
  MyAlloc alloc(&allocated);
  { AllocVec ABSL_ATTRIBUTE_UNUSED v; }
  { AllocVec ABSL_ATTRIBUTE_UNUSED v(alloc); }
  { AllocVec ABSL_ATTRIBUTE_UNUSED v(ia, ia + ABSL_ARRAYSIZE(ia), alloc); }
  { AllocVec ABSL_ATTRIBUTE_UNUSED v({1, 2, 3}, alloc); }

  AllocVec v2;
  { AllocVec ABSL_ATTRIBUTE_UNUSED v(v2, alloc); }
  { AllocVec ABSL_ATTRIBUTE_UNUSED v(std::move(v2), alloc); }
}

TEST(AllocatorSupportTest, CountAllocations) {
  using MyAlloc = CountingAllocator<int>;
  using AllocVec = absl::InlinedVector<int, 4, MyAlloc>;
  const int ia[] = {0, 1, 2, 3, 4, 5, 6, 7};
  int64_t allocated = 0;
  MyAlloc alloc(&allocated);
  {
    AllocVec ABSL_ATTRIBUTE_UNUSED v(ia, ia + 4, alloc);
    EXPECT_THAT(allocated, 0);
  }
  EXPECT_THAT(allocated, 0);
  {
    AllocVec ABSL_ATTRIBUTE_UNUSED v(ia, ia + ABSL_ARRAYSIZE(ia), alloc);
    EXPECT_THAT(allocated, v.size() * sizeof(int));
  }
  EXPECT_THAT(allocated, 0);
  {
    AllocVec v(4, 1, alloc);
    EXPECT_THAT(allocated, 0);

    int64_t allocated2 = 0;
    MyAlloc alloc2(&allocated2);
    AllocVec v2(v, alloc2);
    EXPECT_THAT(allocated2, 0);

    int64_t allocated3 = 0;
    MyAlloc alloc3(&allocated3);
    AllocVec v3(std::move(v), alloc3);
    EXPECT_THAT(allocated3, 0);
  }
  EXPECT_THAT(allocated, 0);
  {
    AllocVec v(8, 2, alloc);
    EXPECT_THAT(allocated, v.size() * sizeof(int));

    int64_t allocated2 = 0;
    MyAlloc alloc2(&allocated2);
    AllocVec v2(v, alloc2);
    EXPECT_THAT(allocated2, v2.size() * sizeof(int));

    int64_t allocated3 = 0;
    MyAlloc alloc3(&allocated3);
    AllocVec v3(std::move(v), alloc3);
    EXPECT_THAT(allocated3, v3.size() * sizeof(int));
  }
  EXPECT_EQ(allocated, 0);
  {
    // Test shrink_to_fit deallocations.
    AllocVec v(8, 2, alloc);
    EXPECT_EQ(allocated, 8 * sizeof(int));
    v.resize(5);
    EXPECT_EQ(allocated, 8 * sizeof(int));
    v.shrink_to_fit();
    EXPECT_EQ(allocated, 5 * sizeof(int));
    v.resize(4);
    EXPECT_EQ(allocated, 5 * sizeof(int));
    v.shrink_to_fit();
    EXPECT_EQ(allocated, 0);
  }
}

TEST(AllocatorSupportTest, SwapBothAllocated) {
  using MyAlloc = CountingAllocator<int>;
  using AllocVec = absl::InlinedVector<int, 4, MyAlloc>;
  int64_t allocated1 = 0;
  int64_t allocated2 = 0;
  {
    const int ia1[] = {0, 1, 2, 3, 4, 5, 6, 7};
    const int ia2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    MyAlloc a1(&allocated1);
    MyAlloc a2(&allocated2);
    AllocVec v1(ia1, ia1 + ABSL_ARRAYSIZE(ia1), a1);
    AllocVec v2(ia2, ia2 + ABSL_ARRAYSIZE(ia2), a2);
    EXPECT_LT(v1.capacity(), v2.capacity());
    EXPECT_THAT(allocated1, v1.capacity() * sizeof(int));
    EXPECT_THAT(allocated2, v2.capacity() * sizeof(int));
    v1.swap(v2);
    EXPECT_THAT(v1, ElementsAreArray(ia2));
    EXPECT_THAT(v2, ElementsAreArray(ia1));
    EXPECT_THAT(allocated1, v2.capacity() * sizeof(int));
    EXPECT_THAT(allocated2, v1.capacity() * sizeof(int));
  }
  EXPECT_THAT(allocated1, 0);
  EXPECT_THAT(allocated2, 0);
}

TEST(AllocatorSupportTest, SwapOneAllocated) {
  using MyAlloc = CountingAllocator<int>;
  using AllocVec = absl::InlinedVector<int, 4, MyAlloc>;
  int64_t allocated1 = 0;
  int64_t allocated2 = 0;
  {
    const int ia1[] = {0, 1, 2, 3, 4, 5, 6, 7};
    const int ia2[] = {0, 1, 2, 3};
    MyAlloc a1(&allocated1);
    MyAlloc a2(&allocated2);
    AllocVec v1(ia1, ia1 + ABSL_ARRAYSIZE(ia1), a1);
    AllocVec v2(ia2, ia2 + ABSL_ARRAYSIZE(ia2), a2);
    EXPECT_THAT(allocated1, v1.capacity() * sizeof(int));
    EXPECT_THAT(allocated2, 0);
    v1.swap(v2);
    EXPECT_THAT(v1, ElementsAreArray(ia2));
    EXPECT_THAT(v2, ElementsAreArray(ia1));
    EXPECT_THAT(allocated1, v2.capacity() * sizeof(int));
    EXPECT_THAT(allocated2, 0);
    EXPECT_TRUE(v2.get_allocator() == a1);
    EXPECT_TRUE(v1.get_allocator() == a2);
  }
  EXPECT_THAT(allocated1, 0);
  EXPECT_THAT(allocated2, 0);
}

TEST(AllocatorSupportTest, ScopedAllocatorWorksInlined) {
  using StdVector = std::vector<int, CountingAllocator<int>>;
  using Alloc = CountingAllocator<StdVector>;
  using ScopedAlloc = std::scoped_allocator_adaptor<Alloc>;
  using AllocVec = absl::InlinedVector<StdVector, 1, ScopedAlloc>;

  int64_t total_allocated_byte_count = 0;

  AllocVec inlined_case(ScopedAlloc(Alloc(+&total_allocated_byte_count)));

  // Called only once to remain inlined
  inlined_case.emplace_back();

  int64_t absl_responsible_for_count = total_allocated_byte_count;

  // MSVC's allocator preemptively allocates in debug mode
#if !defined(_MSC_VER)
  EXPECT_EQ(absl_responsible_for_count, 0);
#endif  // !defined(_MSC_VER)

  inlined_case[0].emplace_back();
  EXPECT_GT(total_allocated_byte_count, absl_responsible_for_count);

  inlined_case.clear();
  inlined_case.shrink_to_fit();
  EXPECT_EQ(total_allocated_byte_count, 0);
}

TEST(AllocatorSupportTest, ScopedAllocatorWorksAllocated) {
  using StdVector = std::vector<int, CountingAllocator<int>>;
  using Alloc = CountingAllocator<StdVector>;
  using ScopedAlloc = std::scoped_allocator_adaptor<Alloc>;
  using AllocVec = absl::InlinedVector<StdVector, 1, ScopedAlloc>;

  int64_t total_allocated_byte_count = 0;

  AllocVec allocated_case(ScopedAlloc(Alloc(+&total_allocated_byte_count)));

  // Called twice to force into being allocated
  allocated_case.emplace_back();
  allocated_case.emplace_back();

  int64_t absl_responsible_for_count = total_allocated_byte_count;
  EXPECT_GT(absl_responsible_for_count, 0);

  allocated_case[1].emplace_back();
  EXPECT_GT(total_allocated_byte_count, absl_responsible_for_count);

  allocated_case.clear();
  allocated_case.shrink_to_fit();
  EXPECT_EQ(total_allocated_byte_count, 0);
}

TEST(AllocatorSupportTest, SizeAllocConstructor) {
  constexpr int inlined_size = 4;
  using Alloc = CountingAllocator<int>;
  using AllocVec = absl::InlinedVector<int, inlined_size, Alloc>;

  {
    auto len = inlined_size / 2;
    int64_t allocated = 0;
    auto v = AllocVec(len, Alloc(&allocated));

    // Inline storage used; allocator should not be invoked
    EXPECT_THAT(allocated, 0);
    EXPECT_THAT(v, AllOf(SizeIs(len), Each(0)));
  }

  {
    auto len = inlined_size * 2;
    int64_t allocated = 0;
    auto v = AllocVec(len, Alloc(&allocated));

    // Out of line storage used; allocation of 8 elements expected
    EXPECT_THAT(allocated, len * sizeof(int));
    EXPECT_THAT(v, AllOf(SizeIs(len), Each(0)));
  }
}

TEST(InlinedVectorTest, MinimumAllocatorCompilesUsingTraits) {
  using T = int;
  using A = std::allocator<T>;
  using ATraits = absl::allocator_traits<A>;

  struct MinimumAllocator {
    using value_type = T;

    value_type* allocate(size_t n) {
      A a;
      return ATraits::allocate(a, n);
    }

    void deallocate(value_type* p, size_t n) {
      A a;
      ATraits::deallocate(a, p, n);
    }
  };

  absl::InlinedVector<T, 1, MinimumAllocator> vec;
  vec.emplace_back();
  vec.resize(0);
}

TEST(InlinedVectorTest, AbslHashValueWorks) {
  using V = absl::InlinedVector<int, 4>;
  std::vector<V> cases;

  // Generate a variety of vectors some of these are small enough for the inline
  // space but are stored out of line.
  for (int i = 0; i < 10; ++i) {
    V v;
    for (int j = 0; j < i; ++j) {
      v.push_back(j);
    }
    cases.push_back(v);
    v.resize(i % 4);
    cases.push_back(v);
  }

  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly(cases));
}

}  // anonymous namespace
