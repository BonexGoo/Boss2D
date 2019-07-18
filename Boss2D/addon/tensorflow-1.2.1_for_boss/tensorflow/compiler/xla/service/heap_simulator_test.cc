/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/compiler/xla/service/heap_simulator.h"

#include <memory>
#include <utility>
#include <vector>

#include "tensorflow/compiler/xla/literal_util.h"
#include "tensorflow/compiler/xla/service/hlo_computation.h"
#include "tensorflow/compiler/xla/service/hlo_instruction.h"
#include "tensorflow/compiler/xla/service/hlo_module.h"
#include "tensorflow/compiler/xla/service/hlo_ordering.h"
#include "tensorflow/compiler/xla/service/logical_buffer.h"
#include "tensorflow/compiler/xla/service/tuple_points_to_analysis.h"
#include "tensorflow/compiler/xla/status_macros.h"
#include "tensorflow/compiler/xla/tests/hlo_test_base.h"
#include "tensorflow/core/lib/gtl/flatmap.h"

namespace xla {
namespace {

const char kAlloc[] = "Alloc";
const char kFree[] = "Free";
const char kFinish[] = "Finish";

// CallSequence records a sequence of Alloc/Free/Finish calls.
using CallSequence = std::vector<std::pair<string, const LogicalBuffer*>>;

// HeapCallRecorder is a dummy heap algorithm that simply records its calls.
class HeapCallRecorder : public HeapAlgorithm {
 public:
  explicit HeapCallRecorder(CallSequence* calls) : calls_(calls) {}
  ~HeapCallRecorder() override {}

  void Alloc(const LogicalBuffer* buffer, int64 size) override {
    calls_->emplace_back(kAlloc, buffer);
    // Instead of assigning a real offset, we set the cardinality of the Alloc
    // call.  This isn't a valid assignment, but allows us to easily test for
    // buffer sharing.
    const int64 offset = result_.chunk_map.size();
    result_.chunk_map.emplace(buffer, Chunk{offset, size});
  }
  void Free(const LogicalBuffer* buffer, int64 size) override {
    calls_->emplace_back(kFree, buffer);
  }
  Result Finish() override {
    calls_->emplace_back(kFinish, nullptr);
    return result_;
  }

 private:
  CallSequence* calls_;
  Result result_;
};

// HeapSimulatorTracker runs the heap simulator, recording the sequence of calls
// made to the underlying heap algorithm.  Tests compare the actual call
// sequence against an expected sequence.
class HeapSimulatorTracker {
 public:
  // Constructor for testing a single entry computation.
  HeapSimulatorTracker(
      const string& name, std::unique_ptr<HloComputation> computation,
      const std::vector<const HloInstruction*>& instruction_sequence) {
    module_ = MakeUnique<HloModule>(name);
    module_->AddEntryComputation(std::move(computation));
    points_to_analysis_ =
        TuplePointsToAnalysis::Run(module_.get()).ConsumeValueOrDie();
    // Since we're only tracking the sequence of Alloc/Free calls, the actual
    // size of the buffers doesn't matter, so we always return 0.  We rely on
    // the secondary sorting criteria of DecreasingSizeRunsHeap to sort calls by
    // buffer id, for determinism in the tests.
    auto zero_size = [](const LogicalBuffer& buffer) { return 0; };
    auto algorithm = MakeUnique<DecreasingSizeRunsHeap>(
        MakeUnique<HeapCallRecorder>(&actual_calls_));
    result_ = HeapSimulator::Run(
                  std::move(algorithm), *module_->entry_computation(),
                  instruction_sequence, *points_to_analysis_, zero_size)
                  .ConsumeValueOrDie();
  }

  explicit HeapSimulatorTracker(const string& name) {
    module_ = MakeUnique<HloModule>(name);
  }

  // Similar to the single entry computation constructor above, but runs the
  // simulation over the entire module.
  void RunWholeModule(
      const std::vector<const HloInstruction*>& full_module_sequence) {
    points_to_analysis_ =
        TuplePointsToAnalysis::Run(module_.get()).ConsumeValueOrDie();

    // Construct the module sequence grouped by computation.
    SequentialHloOrdering::HloModuleSequence module_sequence;
    tensorflow::gtl::FlatMap<const HloInstruction*, int> reverse_position;
    for (int i = 0; i < full_module_sequence.size(); ++i) {
      const HloInstruction* instruction = full_module_sequence[i];
      module_sequence[instruction->parent()].push_back(instruction);
      reverse_position[instruction] = full_module_sequence.size() - i;
    }

    // Hack the size_fn so that it returns a decreasing value as we step through
    // the sequence. This lets us ensure the Alloc calls are in the sequence
    // order. The Free calls are sorted by LogicalBuffer.id, which is at least
    // deterministic.
    auto size_fn = [&reverse_position](const LogicalBuffer& buffer) {
      return reverse_position[buffer.instruction()];
    };
    auto algorithm = MakeUnique<DecreasingSizeRunsHeap>(
        MakeUnique<HeapCallRecorder>(&actual_calls_));
    result_ = HeapSimulator::Run(std::move(algorithm), *module_,
                                 module_sequence, *points_to_analysis_, size_fn)
                  .ConsumeValueOrDie();
  }

  HloModule* module() { return module_.get(); }

  // Returns the buffer defined at the given instruction and index.
  const LogicalBuffer* BufferAt(const HloInstruction* instruction,
                                const ShapeIndex& index) const {
    return points_to_analysis_->GetBufferDefinedAt(instruction, index)
        .ConsumeValueOrDie();
  }

  // Ensures the expected sequence of Alloc/Free/Finish calls was performed.
  void ExpectCallSequence(const CallSequence& expected) const {
    EXPECT_EQ(expected, actual_calls_);
  }

  // Ensures the buffers defined by the respective (instruction,index) pairs are
  // shared, relying on the unique offsets assigned in HeapCallRecorder::Alloc.
  void ExpectSharedBuffers(const HloInstruction* instruction_a,
                           const ShapeIndex& index_a,
                           const HloInstruction* instruction_b,
                           const ShapeIndex& index_b) {
    const LogicalBuffer* a = BufferAt(instruction_a, index_a);
    const LogicalBuffer* b = BufferAt(instruction_b, index_b);
    EXPECT_EQ(result_.chunk_map[a].offset, result_.chunk_map[b].offset)
        << *a << ", " << *b;
  }

 private:
  std::unique_ptr<HloModule> module_;
  std::unique_ptr<TuplePointsToAnalysis> points_to_analysis_;
  CallSequence actual_calls_;
  HeapSimulator::Result result_;
};

class HeapSimulatorTest : public HloTestBase {
 protected:
  HeapSimulatorTest() {}
  ~HeapSimulatorTest() override {}

  // Shapes for use in the examples.
  Shape f32scalar_ = ShapeUtil::MakeShape(xla::F32, {});
  Shape f32vec4_ = ShapeUtil::MakeShape(F32, {4});
};

TEST_F(HeapSimulatorTest, ScalarConstant) {
  auto builder = HloComputation::Builder(TestName());
  auto const0 = builder.AddInstruction(
      HloInstruction::CreateConstant(LiteralUtil::CreateR0<float>(1.0)));

  // Constants aren't assigned.  See b/32248867
  HeapSimulatorTracker tracker(TestName(), builder.Build(), {const0});
  tracker.ExpectCallSequence({{kFinish, nullptr}});
}

TEST_F(HeapSimulatorTest, OneParam) {
  auto builder = HloComputation::Builder(TestName());
  auto param0 = builder.AddInstruction(
      HloInstruction::CreateParameter(0, f32scalar_, "param0"));

  // A single parameter which is also the output.
  HeapSimulatorTracker tracker(TestName(), builder.Build(), {param0});
  tracker.ExpectCallSequence({
      {kAlloc, tracker.BufferAt(param0, {})},
      {kFree, tracker.BufferAt(param0, {})},
      {kFinish, nullptr},
  });
}

TEST_F(HeapSimulatorTest, Multiply) {
  auto builder = HloComputation::Builder(TestName());
  auto paramA = builder.AddInstruction(
      HloInstruction::CreateParameter(0, f32scalar_, "paramA"));
  auto paramX = builder.AddInstruction(
      HloInstruction::CreateParameter(1, f32vec4_, "paramX"));
  auto mul = builder.AddInstruction(HloInstruction::CreateBinary(
      f32vec4_, HloOpcode::kMultiply, paramA, paramX));

  // We must keep all parameters and outputs.
  HeapSimulatorTracker tracker(TestName(), builder.Build(),
                               {paramA, paramX, mul});
  tracker.ExpectCallSequence({
      {kAlloc, tracker.BufferAt(paramA, {})},
      {kAlloc, tracker.BufferAt(paramX, {})},
      {kAlloc, tracker.BufferAt(mul, {})},
      // All params and outputs are freed at the end.
      {kFree, tracker.BufferAt(paramA, {})},
      {kFree, tracker.BufferAt(paramX, {})},
      {kFree, tracker.BufferAt(mul, {})},
      {kFinish, nullptr},
  });
}

TEST_F(HeapSimulatorTest, MultiplyAdd) {
  auto builder = HloComputation::Builder(TestName());
  auto paramA = builder.AddInstruction(
      HloInstruction::CreateParameter(0, f32scalar_, "paramA"));
  auto paramX = builder.AddInstruction(
      HloInstruction::CreateParameter(1, f32vec4_, "paramX"));
  auto paramY = builder.AddInstruction(
      HloInstruction::CreateParameter(2, f32vec4_, "paramY"));
  auto mul = builder.AddInstruction(HloInstruction::CreateBinary(
      f32vec4_, HloOpcode::kMultiply, paramA, paramX));
  auto add = builder.AddInstruction(
      HloInstruction::CreateBinary(f32vec4_, HloOpcode::kAdd, mul, paramY));

  // The buffer for add is the output, and it's shared with the buffer for mul.
  HeapSimulatorTracker tracker(TestName(), builder.Build(),
                               {paramA, paramX, mul, paramY, add});
  tracker.ExpectCallSequence({
      {kAlloc, tracker.BufferAt(paramA, {})},
      {kAlloc, tracker.BufferAt(paramX, {})},
      {kAlloc, tracker.BufferAt(mul, {})},
      {kAlloc, tracker.BufferAt(paramY, {})},
      // All params and outputs are freed at the end.
      {kFree, tracker.BufferAt(paramA, {})},
      {kFree, tracker.BufferAt(paramX, {})},
      {kFree, tracker.BufferAt(mul, {})},
      {kFree, tracker.BufferAt(paramY, {})},
      {kFinish, nullptr},
  });
  tracker.ExpectSharedBuffers(add, {}, mul, {});
}

TEST_F(HeapSimulatorTest, MultiplyDot) {
  auto builder = HloComputation::Builder(TestName());
  auto paramA = builder.AddInstruction(
      HloInstruction::CreateParameter(0, f32scalar_, "paramA"));
  auto paramX = builder.AddInstruction(
      HloInstruction::CreateParameter(1, f32vec4_, "paramX"));
  auto paramY = builder.AddInstruction(
      HloInstruction::CreateParameter(2, f32scalar_, "paramY"));
  auto mul = builder.AddInstruction(HloInstruction::CreateBinary(
      f32vec4_, HloOpcode::kMultiply, paramA, paramX));
  auto dot = builder.AddInstruction(
      HloInstruction::CreateBinary(f32vec4_, HloOpcode::kDot, mul, paramY));

  // The buffer for dot is the output, and it cannot be shared with the buffer
  // for mul, since dot isn't elementwise.
  HeapSimulatorTracker tracker(TestName(), builder.Build(),
                               {paramA, paramX, mul, paramY, dot});
  tracker.ExpectCallSequence({
      {kAlloc, tracker.BufferAt(paramA, {})},
      {kAlloc, tracker.BufferAt(paramX, {})},
      {kAlloc, tracker.BufferAt(mul, {})},
      {kAlloc, tracker.BufferAt(paramY, {})},
      {kAlloc, tracker.BufferAt(dot, {})},
      // All params and outputs are freed at the end.
      {kFree, tracker.BufferAt(paramA, {})},
      {kFree, tracker.BufferAt(paramX, {})},
      {kFree, tracker.BufferAt(mul, {})},
      {kFree, tracker.BufferAt(paramY, {})},
      {kFree, tracker.BufferAt(dot, {})},
      {kFinish, nullptr},
  });
}

TEST_F(HeapSimulatorTest, MultiplyDotAdd) {
  auto builder = HloComputation::Builder(TestName());
  auto paramA = builder.AddInstruction(
      HloInstruction::CreateParameter(0, f32scalar_, "paramA"));
  auto paramX = builder.AddInstruction(
      HloInstruction::CreateParameter(1, f32vec4_, "paramX"));
  auto paramY = builder.AddInstruction(
      HloInstruction::CreateParameter(2, f32scalar_, "paramY"));
  auto mul = builder.AddInstruction(HloInstruction::CreateBinary(
      f32vec4_, HloOpcode::kMultiply, paramA, paramX));
  auto dot = builder.AddInstruction(
      HloInstruction::CreateBinary(f32vec4_, HloOpcode::kDot, mul, paramY));
  auto add = builder.AddInstruction(
      HloInstruction::CreateBinary(f32vec4_, HloOpcode::kAdd, dot, paramA));

  // The buffer for add is the output, and it's shared with the buffer for dot.
  HeapSimulatorTracker tracker(TestName(), builder.Build(),
                               {paramA, paramX, mul, paramY, dot, add});
  tracker.ExpectCallSequence({
      {kAlloc, tracker.BufferAt(paramA, {})},
      {kAlloc, tracker.BufferAt(paramX, {})},
      {kAlloc, tracker.BufferAt(mul, {})},
      {kAlloc, tracker.BufferAt(paramY, {})},
      {kAlloc, tracker.BufferAt(dot, {})},
      // All params and outputs are freed at the end.
      {kFree, tracker.BufferAt(paramA, {})},
      {kFree, tracker.BufferAt(paramX, {})},
      {kFree, tracker.BufferAt(mul, {})},
      {kFree, tracker.BufferAt(paramY, {})},
      {kFree, tracker.BufferAt(dot, {})},
      {kFinish, nullptr},
  });
  tracker.ExpectSharedBuffers(add, {}, dot, {});
}

TEST_F(HeapSimulatorTest, MultiplyDotDot) {
  auto builder = HloComputation::Builder(TestName());
  auto paramA = builder.AddInstruction(
      HloInstruction::CreateParameter(0, f32scalar_, "paramA"));
  auto paramX = builder.AddInstruction(
      HloInstruction::CreateParameter(1, f32vec4_, "paramX"));
  auto paramY = builder.AddInstruction(
      HloInstruction::CreateParameter(2, f32scalar_, "paramY"));
  auto mul = builder.AddInstruction(HloInstruction::CreateBinary(
      f32vec4_, HloOpcode::kMultiply, paramA, paramX));
  auto dot0 = builder.AddInstruction(
      HloInstruction::CreateBinary(f32vec4_, HloOpcode::kDot, mul, paramY));
  auto dot1 = builder.AddInstruction(
      HloInstruction::CreateBinary(f32vec4_, HloOpcode::kDot, dot0, paramY));

  // The buffer for dot1 is the output.  No buffers can be shared.  The buffer
  // for mul is freed before the end, since it's no longer used after dot0
  // finishes.
  HeapSimulatorTracker tracker(TestName(), builder.Build(),
                               {paramA, paramX, mul, paramY, dot0, dot1});
  tracker.ExpectCallSequence({
      {kAlloc, tracker.BufferAt(paramA, {})},
      {kAlloc, tracker.BufferAt(paramX, {})},
      {kAlloc, tracker.BufferAt(mul, {})},
      {kAlloc, tracker.BufferAt(paramY, {})},
      {kAlloc, tracker.BufferAt(dot0, {})},
      {kFree, tracker.BufferAt(mul, {})},  // mul no longer used
      {kAlloc, tracker.BufferAt(dot1, {})},
      // All params and outputs are freed at the end.
      {kFree, tracker.BufferAt(paramA, {})},
      {kFree, tracker.BufferAt(paramX, {})},
      {kFree, tracker.BufferAt(paramY, {})},
      {kFree, tracker.BufferAt(dot0, {})},
      {kFree, tracker.BufferAt(dot1, {})},
      {kFinish, nullptr},
  });
}

TEST_F(HeapSimulatorTest, MultiplyDotDotTuple) {
  auto builder = HloComputation::Builder(TestName());
  auto paramA = builder.AddInstruction(
      HloInstruction::CreateParameter(0, f32scalar_, "paramA"));
  auto paramX = builder.AddInstruction(
      HloInstruction::CreateParameter(1, f32vec4_, "paramX"));
  auto paramY = builder.AddInstruction(
      HloInstruction::CreateParameter(2, f32scalar_, "paramY"));
  auto mul = builder.AddInstruction(HloInstruction::CreateBinary(
      f32vec4_, HloOpcode::kMultiply, paramA, paramX));
  auto dot0 = builder.AddInstruction(
      HloInstruction::CreateBinary(f32vec4_, HloOpcode::kDot, mul, paramY));
  auto dot1 = builder.AddInstruction(
      HloInstruction::CreateBinary(f32vec4_, HloOpcode::kDot, dot0, paramY));
  auto tuple =
      builder.AddInstruction(HloInstruction::CreateTuple({dot0, dot1}));

  // The buffers for dot0, dot1 and tuple are the output.  No buffers can be
  // shared.  The buffer for mul is freed before the end, since it's no longer
  // used after dot0 finishes.
  HeapSimulatorTracker tracker(
      TestName(), builder.Build(),
      {paramA, paramX, mul, paramY, dot0, dot1, tuple});
  tracker.ExpectCallSequence({
      {kAlloc, tracker.BufferAt(paramA, {})},
      {kAlloc, tracker.BufferAt(paramX, {})},
      {kAlloc, tracker.BufferAt(mul, {})},
      {kAlloc, tracker.BufferAt(paramY, {})},
      {kAlloc, tracker.BufferAt(dot0, {})},
      {kFree, tracker.BufferAt(mul, {})},  // mul no longer used
      {kAlloc, tracker.BufferAt(dot1, {})},
      {kAlloc, tracker.BufferAt(tuple, {})},
      // All params and outputs are freed at the end.
      {kFree, tracker.BufferAt(paramA, {})},
      {kFree, tracker.BufferAt(paramX, {})},
      {kFree, tracker.BufferAt(paramY, {})},
      {kFree, tracker.BufferAt(dot0, {})},
      {kFree, tracker.BufferAt(dot1, {})},
      {kFree, tracker.BufferAt(tuple, {})},
      {kFinish, nullptr},
  });
}

TEST_F(HeapSimulatorTest, WholeModule) {
  HeapSimulatorTracker tracker(TestName());

  const Shape scalar_shape = ShapeUtil::MakeShape(xla::F32, {});
  const Shape tuple_shape =
      ShapeUtil::MakeTupleShape({scalar_shape, scalar_shape});

  auto cond_builder = HloComputation::Builder("WhileCond");
  HloInstruction* cond_param = cond_builder.AddInstruction(
      HloInstruction::CreateParameter(0, tuple_shape, "cond_param"));
  HloInstruction* cond_iter = cond_builder.AddInstruction(
      HloInstruction::CreateGetTupleElement(scalar_shape, cond_param, 0));
  HloInstruction* cond_data = cond_builder.AddInstruction(
      HloInstruction::CreateGetTupleElement(scalar_shape, cond_param, 1));
  HloInstruction* cond_lt = cond_builder.AddInstruction(
      HloInstruction::CreateBinary(ShapeUtil::MakeShape(PRED, {}),
                                   HloOpcode::kLt, cond_iter, cond_data));
  HloComputation* cond_computation =
      tracker.module()->AddEmbeddedComputation(cond_builder.Build());

  auto body_builder = HloComputation::Builder("WhileBody");
  HloInstruction* body_param = body_builder.AddInstruction(
      HloInstruction::CreateParameter(0, tuple_shape, "body_param"));
  HloComputation* body_computation =
      tracker.module()->AddEmbeddedComputation(body_builder.Build());

  auto builder = HloComputation::Builder(TestName());
  HloInstruction* param = builder.AddInstruction(
      HloInstruction::CreateParameter(0, tuple_shape, "param"));
  HloInstruction* while_op = builder.AddInstruction(HloInstruction::CreateWhile(
      tuple_shape, cond_computation, body_computation, param));
  tracker.module()->AddEntryComputation(builder.Build());

  tracker.RunWholeModule(
      {param, while_op, body_param, cond_param, cond_iter, cond_data, cond_lt});
  tracker.ExpectCallSequence({
      // The entry computation param and while_op are allocated first.
      {kAlloc, tracker.BufferAt(param, {})},
      {kAlloc, tracker.BufferAt(param, {0})},
      {kAlloc, tracker.BufferAt(param, {1})},
      {kAlloc, tracker.BufferAt(while_op, {})},
      {kAlloc, tracker.BufferAt(while_op, {0})},
      {kAlloc, tracker.BufferAt(while_op, {1})},

      // Now the while body param is allocated and freed.
      {kAlloc, tracker.BufferAt(body_param, {})},
      {kAlloc, tracker.BufferAt(body_param, {0})},
      {kAlloc, tracker.BufferAt(body_param, {1})},
      {kFree, tracker.BufferAt(body_param, {})},
      {kFree, tracker.BufferAt(body_param, {0})},
      {kFree, tracker.BufferAt(body_param, {1})},

      // Now the while cond param is allocated. The GTE instructions just alias
      // the param elements, so the param tuple can immediately be freed.
      {kAlloc, tracker.BufferAt(cond_param, {})},
      {kAlloc, tracker.BufferAt(cond_param, {0})},
      {kAlloc, tracker.BufferAt(cond_param, {1})},
      {kFree, tracker.BufferAt(cond_param, {})},

      // Now the final cond less-than buffer is allocated.
      {kAlloc, tracker.BufferAt(cond_lt, {})},

      // The order of the remaining Free calls is based on the LogicalBuffer.id,
      // which is deterministic, but not obvious.
      {kFree, tracker.BufferAt(param, {})},
      {kFree, tracker.BufferAt(param, {0})},
      {kFree, tracker.BufferAt(param, {1})},

      {kFree, tracker.BufferAt(while_op, {})},
      {kFree, tracker.BufferAt(while_op, {0})},
      {kFree, tracker.BufferAt(while_op, {1})},

      {kFree, tracker.BufferAt(cond_param, {0})},
      {kFree, tracker.BufferAt(cond_param, {1})},
      {kFree, tracker.BufferAt(cond_lt, {})},

      {kFinish, nullptr},
  });
}

// Base class for heap algorithm tests.
class HeapAlgorithmTestBase : public ::testing::Test {
 protected:
  HeapAlgorithmTestBase() {
    buffer_a_ = DummyLogicalBuffer();
    buffer_b_ = DummyLogicalBuffer();
    buffer_c_ = DummyLogicalBuffer();
    buffer_d_ = DummyLogicalBuffer();
    buffer_e_ = DummyLogicalBuffer();
    buffer_f_ = DummyLogicalBuffer();
    buffer_g_ = DummyLogicalBuffer();
    buffer_h_ = DummyLogicalBuffer();
    buffer_i_ = DummyLogicalBuffer();
  }
  ~HeapAlgorithmTestBase() override {}

  const LogicalBuffer* buffer_a_;
  const LogicalBuffer* buffer_b_;
  const LogicalBuffer* buffer_c_;
  const LogicalBuffer* buffer_d_;
  const LogicalBuffer* buffer_e_;
  const LogicalBuffer* buffer_f_;
  const LogicalBuffer* buffer_g_;
  const LogicalBuffer* buffer_h_;
  const LogicalBuffer* buffer_i_;

 private:
  // Create a dummy LogicalBuffer to pass to the heap algorithm.  Since the
  // algorithms only use the buffer as a handle, we don't need to fill in much
  // other than the id.
  const LogicalBuffer* DummyLogicalBuffer() {
    const LogicalBuffer::Id id = buffers_.size();
    buffers_.emplace_back(MakeUnique<LogicalBuffer>(nullptr, ShapeIndex{}, id));
    return buffers_.back().get();
  }

  std::vector<std::unique_ptr<LogicalBuffer>> buffers_;
};

class NoFragmentationStatsHeapTest : public HeapAlgorithmTestBase {};

TEST_F(NoFragmentationStatsHeapTest, Empty) {
  NoFragmentationStatsHeap heap;
  EXPECT_EQ(0, heap.Finish().heap_size);
}

TEST_F(NoFragmentationStatsHeapTest, Simple) {
  NoFragmentationStatsHeap heap;
  heap.Alloc(buffer_a_, 10);
  heap.Alloc(buffer_b_, 20);
  heap.Alloc(buffer_c_, 30);
  heap.Alloc(buffer_d_, 30);
  heap.Free(buffer_a_, 10);
  heap.Free(buffer_b_, 20);
  heap.Free(buffer_c_, 30);
  heap.Free(buffer_d_, 30);
  EXPECT_EQ(90, heap.Finish().heap_size);
}

TEST_F(NoFragmentationStatsHeapTest, Mixed) {
  NoFragmentationStatsHeap heap;
  heap.Alloc(buffer_a_, 10);  // max: A

  heap.Alloc(buffer_b_, 20);  // max: A+B
  heap.Free(buffer_b_, 20);

  heap.Alloc(buffer_c_, 30);  // max: A+C
  heap.Free(buffer_c_, 30);

  heap.Alloc(buffer_d_, 5);  // max: A+C
  heap.Free(buffer_d_, 5);

  heap.Free(buffer_a_, 10);
  EXPECT_EQ(40, heap.Finish().heap_size);
}

class DecreasingSizeRunsHeapTest : public HeapAlgorithmTestBase {};

TEST_F(DecreasingSizeRunsHeapTest, Empty) {
  CallSequence call_sequence;
  DecreasingSizeRunsHeap heap(MakeUnique<HeapCallRecorder>(&call_sequence));
  heap.Finish();
  EXPECT_EQ(call_sequence, CallSequence({
                               {kFinish, nullptr},
                           }));
}

TEST_F(DecreasingSizeRunsHeapTest, Simple) {
  CallSequence call_sequence;
  DecreasingSizeRunsHeap heap(MakeUnique<HeapCallRecorder>(&call_sequence));
  heap.Alloc(buffer_a_, 10);
  heap.Alloc(buffer_b_, 20);
  heap.Alloc(buffer_c_, 30);
  heap.Alloc(buffer_d_, 30);
  heap.Free(buffer_a_, 10);
  heap.Free(buffer_b_, 20);
  heap.Free(buffer_c_, 30);
  heap.Free(buffer_d_, 30);
  heap.Finish();
  // Runs of Allocs and Frees are sorted by decreasing size, with buffer id
  // tiebreaker.
  EXPECT_EQ(call_sequence, CallSequence({
                               {kAlloc, buffer_c_},
                               {kAlloc, buffer_d_},
                               {kAlloc, buffer_b_},
                               {kAlloc, buffer_a_},
                               {kFree, buffer_c_},
                               {kFree, buffer_d_},
                               {kFree, buffer_b_},
                               {kFree, buffer_a_},
                               {kFinish, nullptr},
                           }));
}

TEST_F(DecreasingSizeRunsHeapTest, Mixed) {
  CallSequence call_sequence;
  DecreasingSizeRunsHeap heap(MakeUnique<HeapCallRecorder>(&call_sequence));
  heap.Alloc(buffer_a_, 10);
  heap.Alloc(buffer_b_, 20);
  heap.Free(buffer_b_, 20);

  heap.Alloc(buffer_c_, 30);
  heap.Free(buffer_c_, 30);

  heap.Alloc(buffer_d_, 5);
  heap.Free(buffer_d_, 5);
  heap.Free(buffer_a_, 10);
  heap.Finish();
  // Runs of Allocs and Frees are sorted by decreasing size.
  EXPECT_EQ(call_sequence, CallSequence({
                               {kAlloc, buffer_b_},
                               {kAlloc, buffer_a_},
                               {kFree, buffer_b_},

                               {kAlloc, buffer_c_},
                               {kFree, buffer_c_},

                               {kAlloc, buffer_d_},
                               {kFree, buffer_a_},
                               {kFree, buffer_d_},
                               {kFinish, nullptr},
                           }));
}

class LazyBestFitHeapTest : public HeapAlgorithmTestBase {};

TEST_F(LazyBestFitHeapTest, Empty) {
  LazyBestFitHeap heap(/*alignment=*/1);
  const HeapSimulator::Result result = heap.Finish();
  EXPECT_EQ(0, result.heap_size);
  EXPECT_EQ(0, result.chunk_map.size());
}

TEST_F(LazyBestFitHeapTest, Simple) {
  LazyBestFitHeap heap(/*alignment=*/1);
  heap.Alloc(buffer_a_, 10);
  heap.Alloc(buffer_b_, 20);
  heap.Alloc(buffer_c_, 30);
  heap.Alloc(buffer_d_, 30);
  heap.Free(buffer_a_, 10);
  heap.Free(buffer_b_, 20);
  heap.Free(buffer_c_, 30);
  heap.Free(buffer_d_, 30);

  const HeapSimulator::Result result = heap.Finish();
  EXPECT_EQ(90, result.heap_size);
  EXPECT_EQ(10, result.chunk_map.at(buffer_a_).size);
  EXPECT_EQ(20, result.chunk_map.at(buffer_b_).size);
  EXPECT_EQ(30, result.chunk_map.at(buffer_c_).size);
  EXPECT_EQ(30, result.chunk_map.at(buffer_d_).size);

  EXPECT_EQ(0, result.chunk_map.at(buffer_a_).offset);
  EXPECT_EQ(10, result.chunk_map.at(buffer_b_).offset);
  EXPECT_EQ(30, result.chunk_map.at(buffer_c_).offset);
  EXPECT_EQ(60, result.chunk_map.at(buffer_d_).offset);
}

TEST_F(LazyBestFitHeapTest, Mixed) {
  LazyBestFitHeap heap(/*alignment=*/1);
  heap.Alloc(buffer_a_, 10);  // A lazy offset

  heap.Alloc(buffer_b_, 20);  // B lazy offset
  heap.Free(buffer_b_, 20);   // B range = [0, 20)  free = [0, 20)

  heap.Alloc(buffer_c_, 30);  // C range = [0, 30)
  heap.Free(buffer_c_, 30);   //                    free = [0, 30)

  heap.Alloc(buffer_d_, 5);  // D range = [0, 5)   free = [5, 30)
  heap.Free(buffer_d_, 5);   //                    free = [0, 30)

  heap.Free(buffer_a_, 10);  // A range = [30, 10) free = [0, 40)

  const HeapSimulator::Result result = heap.Finish();
  EXPECT_EQ(40, result.heap_size);
  EXPECT_EQ(10, result.chunk_map.at(buffer_a_).size);
  EXPECT_EQ(20, result.chunk_map.at(buffer_b_).size);
  EXPECT_EQ(30, result.chunk_map.at(buffer_c_).size);
  EXPECT_EQ(5, result.chunk_map.at(buffer_d_).size);

  EXPECT_EQ(30, result.chunk_map.at(buffer_a_).offset);
  EXPECT_EQ(0, result.chunk_map.at(buffer_b_).offset);
  EXPECT_EQ(0, result.chunk_map.at(buffer_c_).offset);
  EXPECT_EQ(0, result.chunk_map.at(buffer_d_).offset);
}

TEST_F(LazyBestFitHeapTest, BestFit) {
  LazyBestFitHeap heap(/*alignment=*/1);

  // First alloc/free buffer_a_, to force a big free chunk to appear.
  heap.Alloc(buffer_a_, 200);  // A lazy offset
  heap.Free(buffer_a_, 200);   // A range = [0, 200)   free = [0, 200)

  // Now alloc a bunch of buffers that are allocated out of the free chunk.
  heap.Alloc(buffer_b_, 30);  // B range = [0, 30)    free = [30, 200)
  heap.Alloc(buffer_c_, 30);  // C range = [30, 60)   free = [60, 200)
  heap.Alloc(buffer_d_, 20);  // D range = [60, 80)   free = [80, 200)
  heap.Alloc(buffer_e_, 20);  // E range = [80, 100)  free = [100, 200)
  heap.Alloc(buffer_f_, 10);  // F range = [100, 110) free = [110, 200)
  heap.Alloc(buffer_g_, 10);  // G range = [110, 120) free = [120, 200)
  heap.Alloc(buffer_h_, 80);  // H range = [120, 200)

  // Free buffers to create free chunks of different sizes.
  heap.Free(buffer_c_, 30);  // free = [30, 60)
  heap.Free(buffer_e_, 20);  // free = [30, 60), [80, 100)
  heap.Free(buffer_g_, 10);  // free = [30, 60), [80, 100), [110, 120)

  // The best fit is picked out of the existing free chunks.
  heap.Alloc(buffer_i_, 15);  // I range = [80, 95)

  // The frees here ensure the buffer-coalescing logic is exercised.
  heap.Free(buffer_b_, 30);
  heap.Free(buffer_d_, 20);
  heap.Free(buffer_f_, 10);
  heap.Free(buffer_h_, 80);
  heap.Free(buffer_i_, 15);

  const HeapSimulator::Result result = heap.Finish();
  EXPECT_EQ(200, result.heap_size);
  EXPECT_EQ(200, result.chunk_map.at(buffer_a_).size);
  EXPECT_EQ(30, result.chunk_map.at(buffer_b_).size);
  EXPECT_EQ(30, result.chunk_map.at(buffer_c_).size);
  EXPECT_EQ(20, result.chunk_map.at(buffer_d_).size);
  EXPECT_EQ(20, result.chunk_map.at(buffer_e_).size);
  EXPECT_EQ(10, result.chunk_map.at(buffer_f_).size);
  EXPECT_EQ(10, result.chunk_map.at(buffer_g_).size);
  EXPECT_EQ(80, result.chunk_map.at(buffer_h_).size);
  EXPECT_EQ(15, result.chunk_map.at(buffer_i_).size);

  EXPECT_EQ(0, result.chunk_map.at(buffer_a_).offset);
  EXPECT_EQ(0, result.chunk_map.at(buffer_b_).offset);
  EXPECT_EQ(30, result.chunk_map.at(buffer_c_).offset);
  EXPECT_EQ(60, result.chunk_map.at(buffer_d_).offset);
  EXPECT_EQ(80, result.chunk_map.at(buffer_e_).offset);
  EXPECT_EQ(100, result.chunk_map.at(buffer_f_).offset);
  EXPECT_EQ(110, result.chunk_map.at(buffer_g_).offset);
  EXPECT_EQ(120, result.chunk_map.at(buffer_h_).offset);
  EXPECT_EQ(80, result.chunk_map.at(buffer_i_).offset);
}

TEST_F(LazyBestFitHeapTest, Lazy) {
  LazyBestFitHeap heap(/*alignment=*/1);

  // First alloc some buffers, which are all lazily allocated offsets.
  heap.Alloc(buffer_a_, 10);
  heap.Alloc(buffer_b_, 5);
  heap.Alloc(buffer_c_, 10);

  // Now free some buffers, which forces offset assignment.
  heap.Free(buffer_a_, 10);  // A range = [0, 10)  free = [0, 10)
  heap.Free(buffer_c_, 10);  // C range = [10, 20) free = [0, 20)

  // If we hadn't lazily assigned offsets, the free chunk wouldn't be large
  // enough to hold the entire allocation.
  heap.Alloc(buffer_d_, 20);  // D range = [0, 20)

  heap.Free(buffer_b_, 5);  // B range = [20, 25)
  heap.Free(buffer_d_, 20);

  const HeapSimulator::Result result = heap.Finish();
  EXPECT_EQ(25, result.heap_size);
  EXPECT_EQ(10, result.chunk_map.at(buffer_a_).size);
  EXPECT_EQ(5, result.chunk_map.at(buffer_b_).size);
  EXPECT_EQ(10, result.chunk_map.at(buffer_c_).size);
  EXPECT_EQ(20, result.chunk_map.at(buffer_d_).size);

  EXPECT_EQ(0, result.chunk_map.at(buffer_a_).offset);
  EXPECT_EQ(20, result.chunk_map.at(buffer_b_).offset);
  EXPECT_EQ(10, result.chunk_map.at(buffer_c_).offset);
  EXPECT_EQ(0, result.chunk_map.at(buffer_d_).offset);
}

TEST_F(LazyBestFitHeapTest, ReuseLastFreeChunk) {
  LazyBestFitHeap heap(/*alignment=*/1);

  // First alloc/free buffer_a_, to force a big free chunk to appear.
  heap.Alloc(buffer_a_, 60);  // A lazy offset
  heap.Free(buffer_a_, 60);   // A range = [0, 60)   free = [0, 60)

  // Now alloc a bunch of buffers that are allocated out of the free chunk.
  heap.Alloc(buffer_b_, 10);  // B range = [0, 10)    free = [10, 60)
  heap.Alloc(buffer_c_, 20);  // C range = [10, 30)   free = [30, 60)
  heap.Alloc(buffer_d_, 30);  // D range = [30, 60)

  // Free buffers to create free chunks of different sizes.
  heap.Free(buffer_b_, 10);  // free = [0, 10)
  heap.Free(buffer_d_, 30);  // free = [0, 10), [30, 60)

  // No free chunks are large enough, but the last free chunk is adjacent to the
  // end of the heap, so we re-use that chunk.
  heap.Alloc(buffer_e_, 40);  // E range = [30, 70)

  heap.Free(buffer_c_, 20);
  heap.Free(buffer_e_, 40);

  const HeapSimulator::Result result = heap.Finish();
  EXPECT_EQ(70, result.heap_size);
  EXPECT_EQ(60, result.chunk_map.at(buffer_a_).size);
  EXPECT_EQ(10, result.chunk_map.at(buffer_b_).size);
  EXPECT_EQ(20, result.chunk_map.at(buffer_c_).size);
  EXPECT_EQ(30, result.chunk_map.at(buffer_d_).size);
  EXPECT_EQ(40, result.chunk_map.at(buffer_e_).size);

  EXPECT_EQ(0, result.chunk_map.at(buffer_a_).offset);
  EXPECT_EQ(0, result.chunk_map.at(buffer_b_).offset);
  EXPECT_EQ(10, result.chunk_map.at(buffer_c_).offset);
  EXPECT_EQ(30, result.chunk_map.at(buffer_d_).offset);
  EXPECT_EQ(30, result.chunk_map.at(buffer_e_).offset);
}

TEST_F(LazyBestFitHeapTest, Alignment) {
  LazyBestFitHeap heap(/*alignment=*/64);

  // First alloc some buffers, which are all lazily allocated offsets.
  heap.Alloc(buffer_a_, 10);
  heap.Alloc(buffer_b_, 5);
  heap.Alloc(buffer_c_, 10);

  // Now free some buffers, which forces offset assignment with alignment.
  heap.Free(buffer_a_, 10);  //  A range = [0, 10)    free = [0, 10)
  heap.Free(buffer_c_, 10);  //  C range = [64, 74)   free = [0, 74)

  // If we hadn't lazily assigned offsets, and accounted for alignment, the free
  // chunk wouldn't be large enough to hold the entire allocation.
  heap.Alloc(buffer_d_, 74);  // D range = [0, 74)    free = [)

  heap.Free(buffer_b_, 5);    // B range = [128, 133) free = [74, 133)
  heap.Alloc(buffer_e_, 23);  // E range = [128, 151) free = [74, 128)

  heap.Free(buffer_d_, 74);  //                       free = [0, 128)
  heap.Free(buffer_e_, 23);  //                       free = [0, 151)

  const HeapSimulator::Result result = heap.Finish();
  EXPECT_EQ(151, result.heap_size);
  EXPECT_EQ(10, result.chunk_map.at(buffer_a_).size);
  EXPECT_EQ(5, result.chunk_map.at(buffer_b_).size);
  EXPECT_EQ(10, result.chunk_map.at(buffer_c_).size);
  EXPECT_EQ(74, result.chunk_map.at(buffer_d_).size);
  EXPECT_EQ(23, result.chunk_map.at(buffer_e_).size);

  EXPECT_EQ(0, result.chunk_map.at(buffer_a_).offset);
  EXPECT_EQ(128, result.chunk_map.at(buffer_b_).offset);
  EXPECT_EQ(64, result.chunk_map.at(buffer_c_).offset);
  EXPECT_EQ(0, result.chunk_map.at(buffer_d_).offset);
  EXPECT_EQ(128, result.chunk_map.at(buffer_e_).offset);
}

}  // namespace
}  // namespace xla
