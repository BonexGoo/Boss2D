//
// Copyright 2018 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include BOSS_ABSEILCPP_U_absl__debugging__internal__stack_consumption_h //original-code:"absl/debugging/internal/stack_consumption.h"

#ifdef ABSL_INTERNAL_HAVE_DEBUGGING_STACK_CONSUMPTION

#include <string.h>

#include "gtest/gtest.h"
#include BOSS_ABSEILCPP_U_absl__base__internal__raw_logging_h //original-code:"absl/base/internal/raw_logging.h"

namespace absl {
inline namespace lts_2018_06_20 {
namespace debugging_internal {
namespace {

static void SimpleSignalHandler(int signo) {
  char buf[100];
  memset(buf, 'a', sizeof(buf));

  // Never true, but prevents compiler from optimizing buf out.
  if (signo == 0) {
    ABSL_RAW_LOG(INFO, "%p", static_cast<void*>(buf));
  }
}

TEST(SignalHandlerStackConsumptionTest, MeasuresStackConsumption) {
  // Our handler should consume reasonable number of bytes.
  EXPECT_GE(GetSignalHandlerStackConsumption(SimpleSignalHandler), 100);
}

}  // namespace
}  // namespace debugging_internal
}  // inline namespace lts_2018_06_20
}  // namespace absl

#endif  // ABSL_INTERNAL_HAVE_DEBUGGING_STACK_CONSUMPTION
