/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_system_wrappers__include__aligned_malloc_h //original-code:"system_wrappers/include/aligned_malloc.h"

#include <memory>

#ifdef _WIN32
#include BOSS_FAKEWIN_V_windows_h //original-code:<windows.h>
#else
#include BOSS_FAKEWIN_V_stdint_h //original-code:<stdint.h>
#endif

#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_typedefs_h //original-code:"typedefs.h"  // NOLINT(build/include)

namespace webrtc {

// Returns true if |size| and |alignment| are valid combinations.
bool CorrectUsage(size_t size, size_t alignment) {
  std::unique_ptr<char, AlignedFreeDeleter> scoped(
      static_cast<char*>(AlignedMalloc(size, alignment)));
  if (scoped.get() == NULL) {
    return false;
  }
  const uintptr_t scoped_address = reinterpret_cast<uintptr_t>(scoped.get());
  return 0u == scoped_address % alignment;
}

TEST(AlignedMalloc, GetRightAlign) {
  const size_t size = 100;
  const size_t alignment = 32;
  const size_t left_misalignment = 1;
  std::unique_ptr<char, AlignedFreeDeleter> scoped(
      static_cast<char*>(AlignedMalloc(size, alignment)));
  EXPECT_TRUE(scoped.get() != NULL);
  const uintptr_t aligned_address = reinterpret_cast<uintptr_t>(scoped.get());
  const uintptr_t misaligned_address = aligned_address - left_misalignment;
  const char* misaligned_ptr =
      reinterpret_cast<const char*>(misaligned_address);
  const char* realigned_ptr = GetRightAlign(misaligned_ptr, alignment);
  EXPECT_EQ(scoped.get(), realigned_ptr);
}

TEST(AlignedMalloc, IncorrectSize) {
  const size_t incorrect_size = 0;
  const size_t alignment = 64;
  EXPECT_FALSE(CorrectUsage(incorrect_size, alignment));
}

TEST(AlignedMalloc, IncorrectAlignment) {
  const size_t size = 100;
  const size_t incorrect_alignment = 63;
  EXPECT_FALSE(CorrectUsage(size, incorrect_alignment));
}

TEST(AlignedMalloc, AlignTo2Bytes) {
  size_t size = 100;
  size_t alignment = 2;
  EXPECT_TRUE(CorrectUsage(size, alignment));
}

TEST(AlignedMalloc, AlignTo32Bytes) {
  size_t size = 100;
  size_t alignment = 32;
  EXPECT_TRUE(CorrectUsage(size, alignment));
}

TEST(AlignedMalloc, AlignTo128Bytes) {
  size_t size = 100;
  size_t alignment = 128;
  EXPECT_TRUE(CorrectUsage(size, alignment));
}

}  // namespace webrtc
