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

#include "tensorflow/compiler/xla/service/hlo_instruction.h"

#include <algorithm>
#include <deque>
#include <ostream>
#include <set>
#include <unordered_set>
#include <utility>

#include "tensorflow/compiler/xla/layout_util.h"
#include "tensorflow/compiler/xla/literal_util.h"
#include "tensorflow/compiler/xla/protobuf_util.h"
#include "tensorflow/compiler/xla/ptr_util.h"
#include "tensorflow/compiler/xla/service/dfs_hlo_visitor_with_default.h"
#include "tensorflow/compiler/xla/service/hlo_computation.h"
#include "tensorflow/compiler/xla/service/hlo_module.h"
#include "tensorflow/compiler/xla/service/name_uniquer.h"
#include "tensorflow/compiler/xla/shape_util.h"
#include "tensorflow/compiler/xla/status_macros.h"
#include "tensorflow/compiler/xla/types.h"
#include "tensorflow/compiler/xla/util.h"
#include "tensorflow/compiler/xla/window_util.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/gtl/flatmap.h"
#include "tensorflow/core/lib/strings/str_util.h"
#include "tensorflow/core/lib/strings/strcat.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/logging.h"

namespace xla {

using ::tensorflow::str_util::Join;
using ::tensorflow::strings::Printf;
using ::tensorflow::strings::StrAppend;
using ::tensorflow::strings::StrCat;

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateParameter(
    int64 parameter_number, const Shape& shape, const string& name) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kParameter, shape));
  instruction->parameter_number_ = parameter_number;
  instruction->parameter_name_ = name;
  instruction->name_ = "%" + name;
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateTrace(
    const string& tag, HloInstruction* operand) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kTrace, ShapeUtil::MakeNil()));
  instruction->operands_.push_back(operand);
  instruction->literal_.reset(new Literal);
  *instruction->literal_->mutable_u8s() += tag;
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateConstant(
    std::unique_ptr<Literal> literal) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kConstant, literal->shape()));
  instruction->literal_ = std::move(literal);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction>
HloInstruction::CreateGetTupleElement(const Shape& shape,
                                      HloInstruction* operand, int64 index) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kGetTupleElement, shape));
  instruction->tuple_index_ = index;
  instruction->AppendOperand(operand);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateRng(
    const Shape& shape, RandomDistribution distribution,
    tensorflow::gtl::ArraySlice<HloInstruction*> parameters) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kRng, shape));
  instruction->distribution_ = distribution;
  instruction->shape_ = shape;
  for (HloInstruction* param : parameters) {
    instruction->AppendOperand(param);
  }
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateNary(
    const Shape& shape, HloOpcode opcode,
    tensorflow::gtl::ArraySlice<HloInstruction*> operands) {
  if (opcode == HloOpcode::kCopy) {
    // It is impossible to copy an opaque shape, we don't know how big it is.
    CHECK(!ShapeUtil::IsOpaque(shape));
  }
  auto instruction = WrapUnique(new HloInstruction(opcode, shape));
  for (auto operand : operands) {
    instruction->AppendOperand(operand);
  }
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateUnary(
    const Shape& shape, HloOpcode opcode, HloInstruction* operand) {
  // Only certain opcodes are supported with CreateUnary: opcodes of unary
  // instructions with no auxiliary fields.
  switch (opcode) {
    case HloOpcode::kAbs:
    case HloOpcode::kBitcast:
    case HloOpcode::kCeil:
    case HloOpcode::kCopy:
    case HloOpcode::kExp:
    case HloOpcode::kFloor:
    case HloOpcode::kIsFinite:
    case HloOpcode::kLog:
    case HloOpcode::kLogicalNot:
    case HloOpcode::kNegate:
    case HloOpcode::kSign:
    case HloOpcode::kSort:
    case HloOpcode::kTanh:
      break;
    default:
      LOG(FATAL) << "Invalid unary instruction opcode "
                 << HloOpcodeString(opcode);
  }
  return CreateNary(shape, opcode, {operand});
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateBinary(
    const Shape& shape, HloOpcode opcode, HloInstruction* lhs,
    HloInstruction* rhs) {
  // Only certain opcodes are supported with CreateBinary: opcodes of binary
  // instructions with no auxiliary fields.
  switch (opcode) {
    case (HloOpcode::kAdd):
    case (HloOpcode::kDivide):
    case (HloOpcode::kDot):
    case (HloOpcode::kEq):
    case (HloOpcode::kGe):
    case (HloOpcode::kGt):
    case (HloOpcode::kLe):
    case (HloOpcode::kLt):
    case (HloOpcode::kMaximum):
    case (HloOpcode::kMinimum):
    case (HloOpcode::kMultiply):
    case (HloOpcode::kNe):
    case (HloOpcode::kPower):
    case (HloOpcode::kRemainder):
    case (HloOpcode::kSubtract):
    case (HloOpcode::kLogicalAnd):
    case (HloOpcode::kLogicalOr):
      break;
    default:
      LOG(FATAL) << "Invalid binary instruction opcode "
                 << HloOpcodeString(opcode);
  }
  return CreateNary(shape, opcode, {lhs, rhs});
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateTernary(
    const Shape& shape, HloOpcode opcode, HloInstruction* lhs,
    HloInstruction* rhs, HloInstruction* ehs) {
  // Only certain opcodes are supported with CreateTernary: opcodes of ternary
  // instructions with no auxiliary fields.
  switch (opcode) {
    case (HloOpcode::kClamp):
    case (HloOpcode::kSelect):
      break;
    default:
      LOG(FATAL) << "Invalid ternary instruction opcode "
                 << HloOpcodeString(opcode);
  }
  return CreateNary(shape, opcode, {lhs, rhs, ehs});
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateVariadic(
    const Shape& shape, HloOpcode opcode,
    tensorflow::gtl::ArraySlice<HloInstruction*> operands) {
  CHECK_EQ(HloOpcode::kTuple, opcode);
  return CreateNary(shape, opcode, operands);
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateMap(
    const Shape& shape, tensorflow::gtl::ArraySlice<HloInstruction*> operands,
    HloComputation* map_computation,
    tensorflow::gtl::ArraySlice<HloInstruction*> static_operands) {
  CHECK(static_operands.empty()) << "static_operands not yet supported";
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kMap, shape));
  for (auto operand : operands) {
    instruction->AppendOperand(operand);
  }
  instruction->called_computations_.push_back(map_computation);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateConvolve(
    const Shape& shape, HloInstruction* lhs, HloInstruction* rhs,
    const Window& window,
    const ConvolutionDimensionNumbers& dimension_numbers) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kConvolution, shape));
  if (window_util::HasBaseDilation(window)) {
    instruction->name_ = instruction->name() + "-base-dilated";
  }
  if (window_util::HasWindowDilation(window)) {
    instruction->name_ = instruction->name() + "-window-dilated";
  }
  instruction->AppendOperand(lhs);
  instruction->AppendOperand(rhs);
  instruction->window_ = MakeUnique<Window>(window);
  instruction->convolution_dimension_numbers_ =
      MakeUnique<ConvolutionDimensionNumbers>(dimension_numbers);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction>
HloInstruction::CreateCrossReplicaSum(const Shape& shape,
                                      HloInstruction* operand) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kCrossReplicaSum, shape));
  instruction->AppendOperand(operand);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateInfeed(
    const Shape& shape, const string& config) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kInfeed, shape));
  instruction->set_infeed_config(config);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateOutfeed(
    const Shape& shape, HloInstruction* operand,
    tensorflow::StringPiece outfeed_config) {
  std::unique_ptr<HloInstruction> instruction =
      WrapUnique(new HloInstruction(HloOpcode::kOutfeed, ShapeUtil::MakeNil()));
  instruction->AppendOperand(operand);
  instruction->outfeed_config_ = outfeed_config.ToString();
  instruction->outfeed_shape_ = shape;
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateSend(
    HloInstruction* operand, int64 channel_id) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kSend, ShapeUtil::MakeNil()));
  instruction->AppendOperand(operand);
  instruction->channel_id_ = channel_id;
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateRecv(
    const Shape& shape, int64 channel_id) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kRecv, shape));
  instruction->channel_id_ = channel_id;
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateReverse(
    const Shape& shape, HloInstruction* operand,
    tensorflow::gtl::ArraySlice<int64> dimensions) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kReverse, shape));
  instruction->AppendOperand(operand);
  instruction->dimensions_.assign(dimensions.begin(), dimensions.end());
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateWhile(
    const Shape& shape, HloComputation* condition, HloComputation* body,
    HloInstruction* init) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kWhile, shape));
  instruction->AppendOperand(init);
  // Body comes before condition computation in the vector.
  instruction->called_computations_.push_back(body);
  instruction->called_computations_.push_back(condition);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateSlice(
    const Shape& shape, HloInstruction* operand,
    tensorflow::gtl::ArraySlice<int64> start_indices,
    tensorflow::gtl::ArraySlice<int64> limit_indices) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kSlice, shape));
  instruction->AppendOperand(operand);
  instruction->slice_starts_.assign(start_indices.begin(), start_indices.end());
  instruction->slice_limits_.assign(limit_indices.begin(), limit_indices.end());
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateDynamicSlice(
    const Shape& shape, HloInstruction* operand, HloInstruction* start_indices,
    tensorflow::gtl::ArraySlice<int64> slice_sizes) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kDynamicSlice, shape));
  instruction->AppendOperand(operand);
  instruction->AppendOperand(start_indices);
  instruction->dynamic_slice_sizes_.assign(slice_sizes.begin(),
                                           slice_sizes.end());
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction>
HloInstruction::CreateDynamicUpdateSlice(const Shape& shape,
                                         HloInstruction* operand,
                                         HloInstruction* update,
                                         HloInstruction* start_indices) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kDynamicUpdateSlice, shape));
  instruction->AppendOperand(operand);
  instruction->AppendOperand(update);
  instruction->AppendOperand(start_indices);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateConcatenate(
    const Shape& shape, tensorflow::gtl::ArraySlice<HloInstruction*> operands,
    int64 dimension) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kConcatenate, shape));
  for (auto operand : operands) {
    instruction->AppendOperand(operand);
  }
  instruction->dimensions_.push_back(dimension);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateConvert(
    const Shape& shape, HloInstruction* operand) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kConvert, shape));
  instruction->AppendOperand(operand);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateReduce(
    const Shape& shape, HloInstruction* arg, HloInstruction* init_value,
    tensorflow::gtl::ArraySlice<int64> dimensions_to_reduce,
    HloComputation* reduce_computation) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kReduce, shape));
  instruction->AppendOperand(arg);
  instruction->AppendOperand(init_value);
  instruction->dimensions_.assign(dimensions_to_reduce.begin(),
                                  dimensions_to_reduce.end());
  instruction->called_computations_.push_back(reduce_computation);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateReduceWindow(
    const Shape& shape, HloInstruction* operand, HloInstruction* init_value,
    const Window& window, HloComputation* reduce_computation) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kReduceWindow, shape));
  instruction->AppendOperand(operand);
  instruction->AppendOperand(init_value);
  instruction->called_computations_.push_back(reduce_computation);
  instruction->window_ = MakeUnique<Window>(window);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction>
HloInstruction::CreateSelectAndScatter(
    const Shape& shape, HloInstruction* operand, HloComputation* select,
    const Window& window, HloInstruction* source, HloInstruction* init_value,
    HloComputation* scatter) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kSelectAndScatter, shape));
  instruction->AppendOperand(operand);
  instruction->AppendOperand(source);
  instruction->AppendOperand(init_value);
  // Select comes before scatter in the vector.
  instruction->called_computations_.push_back(select);
  instruction->called_computations_.push_back(scatter);
  instruction->window_ = MakeUnique<Window>(window);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateBroadcast(
    const Shape& shape, HloInstruction* operand,
    tensorflow::gtl::ArraySlice<int64> broadcast_dimensions) {
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kBroadcast, shape));
  instruction->AppendOperand(operand);
  instruction->dimensions_.assign(broadcast_dimensions.begin(),
                                  broadcast_dimensions.end());
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreatePad(
    const Shape& shape, HloInstruction* operand, HloInstruction* padding_value,
    const PaddingConfig& padding_config) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kPad, shape));
  instruction->AppendOperand(operand);
  instruction->AppendOperand(padding_value);
  instruction->padding_config_ = MakeUnique<PaddingConfig>(padding_config);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateReshape(
    const Shape& shape, HloInstruction* operand) {
  CHECK_EQ(ShapeUtil::ElementsIn(shape),
           ShapeUtil::ElementsIn(operand->shape()))
      << "shape: " << ShapeUtil::HumanString(shape)
      << " operand: " << ShapeUtil::HumanString(operand->shape());
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kReshape, shape));
  instruction->AppendOperand(operand);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateTranspose(
    const Shape& shape, HloInstruction* operand,
    tensorflow::gtl::ArraySlice<int64> dimensions) {
  CHECK_EQ(shape.dimensions().size(), dimensions.size());
  CHECK_EQ(shape.dimensions().size(), operand->shape().dimensions().size());
  CHECK(std::equal(operand->shape().dimensions().begin(),
                   operand->shape().dimensions().end(),
                   Permute(dimensions, shape.dimensions()).begin()));
  auto instruction =
      WrapUnique(new HloInstruction(HloOpcode::kTranspose, shape));
  instruction->AppendOperand(operand);
  instruction->dimensions_.assign(dimensions.begin(), dimensions.end());
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateFusion(
    const Shape& shape, FusionKind fusion_kind, HloInstruction* fused_root) {
  auto instruction = WrapUnique(new HloInstruction(HloOpcode::kFusion, shape));
  instruction->fusion_kind_ = fusion_kind;
  instruction->set_parent(fused_root->parent());
  instruction->set_metadata(fused_root->metadata());
  instruction->CloneAndFuseInternal(fused_root);
  instruction->CheckFusionInstruction();
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction>
HloInstruction::CreateFusionForBackwardConvolution(
    const Shape& shape, FusionKind fusion_kind, const Window& window,
    const ConvolutionDimensionNumbers& conv_dnums, HloInstruction* fused_root) {
  std::unique_ptr<HloInstruction> fusion =
      CreateFusion(shape, fusion_kind, fused_root);
  fusion->window_ = MakeUnique<Window>(window);
  fusion->convolution_dimension_numbers_ =
      MakeUnique<ConvolutionDimensionNumbers>(conv_dnums);
  return fusion;
}

void HloInstruction::MergeFusionInstruction(
    HloInstruction* instruction_to_merge) {
  CHECK_EQ(opcode_, HloOpcode::kFusion);
  CHECK_EQ(instruction_to_merge->opcode(), HloOpcode::kFusion);
  // Clone the instruction from which to merge fused instructions.
  std::unique_ptr<HloInstruction> clone = instruction_to_merge->Clone();
  // Replace uses of fused parameters with the corresponding operand of the
  // fusion.
  // Add all non-parameter fused instructions to 'unfused_instructions' to be
  // merged into 'this'.
  std::vector<HloInstruction*> unfused_instructions;
  for (auto& fused_instruction : clone->fused_instructions()) {
    if (fused_instruction->opcode() == HloOpcode::kParameter) {
      TF_CHECK_OK(fused_instruction->ReplaceAllUsesWith(
          clone->mutable_operand(fused_instruction->parameter_number())));
    } else {
      unfused_instructions.push_back(fused_instruction.get());
    }
  }
  CHECK(unfused_instructions.front() == clone->fused_expression_root());
  // Replace instruction_to_merge use of 'this' with unfused_root.
  TF_CHECK_OK(
      instruction_to_merge->ReplaceUseWith(this, unfused_instructions.front()));
  // Fuse 'unfused_instructions' into 'this'.
  for (auto& instruction : unfused_instructions) {
    FuseInstruction(instruction);
    instruction->DetachFromOperands();
  }
  CHECK_EQ(0, clone->user_count());
  clone->DetachFromOperands();
}

HloInstruction* HloInstruction::FuseInstruction(
    HloInstruction* instruction_to_fuse) {
  CHECK_EQ(opcode_, HloOpcode::kFusion);

  // This fusion instruction must be a user of instruction_to_fuse.
  CHECK(IsUserOf(instruction_to_fuse));
  HloInstruction* fused_instruction = CloneAndFuseInternal(instruction_to_fuse);
  CheckFusionInstruction();
  return fused_instruction;
}

HloInstruction* HloInstruction::CloneAndFuseInternal(
    HloInstruction* instruction_to_fuse) {
  CHECK_EQ(opcode_, HloOpcode::kFusion);
  CHECK(instruction_to_fuse->IsFusable());

  HloInstruction* clone = nullptr;
  if (fused_instructions_computation_ == nullptr) {
    // New fusion instruction.
    auto builder = HloComputation::Builder("fused_computation", true);
    builder.AddInstruction(instruction_to_fuse->Clone(/*suffix=*/""));
    fused_instructions_computation_ = builder.Build();
    clone = fused_expression_root();
    clone->parent_fusion_instruction_ = this;
  } else {
    CHECK(fused_instructions_computation_ != nullptr &&
          fused_instructions_computation_->IsFusionComputation());
    clone = fused_instructions_computation_->AddInstruction(
        instruction_to_fuse->Clone(/*suffix=*/""));
    clone->parent_fusion_instruction_ = this;
    // instruction_to_fuse is necessarily an operand of the fusion instruction.
    // After fusion this will no longer be the case. Remove the operand from the
    // operand list and remove its corresponding fused parameter
    // instruction. Renumber parameters as necessary to make parameter numbers
    // consistent with their index in the fused_parameter_ vector.
    CHECK(std::find(operands_.begin(), operands_.end(), instruction_to_fuse) !=
          operands_.end());
    const std::vector<HloInstruction*>& fused_parameters_ =
        fused_instructions_computation_->parameter_instructions();
    for (int64 operand_num = 0; operand_num < operand_count(); ++operand_num) {
      if (instruction_to_fuse == operands_[operand_num]) {
        // replace the fused parameter instruction's uses with the clone.
        HloInstruction* fused_parameter = fused_parameters_[operand_num];
        TF_CHECK_OK(fused_parameter->ReplaceAllUsesWith(clone));

        // Remove the corresponding fused parameter and operand from their
        // respective vectors.
        TF_CHECK_OK(
            fused_instructions_computation_->RemoveParameter(operand_num));
        operands_.erase(operands_.begin() + operand_num);
        break;
      }
    }
    // We've cloned instruction_to_fuse into this fusion instruction, so this
    // fusion instruction is no longer a use of instruction_to_fuse.
    instruction_to_fuse->RemoveUser(this);
  }

  // Reread the parameters in the computation.
  const std::vector<HloInstruction*>& fused_parameters_ =
      fused_instructions_computation_->parameter_instructions();

  // Add each operand of the clone as an operand of the fusion instruction. A
  // complication is that some clone operands may already be operands of the
  // fusion instruction.
  for (int64 operand_num = 0; operand_num < clone->operand_count();
       ++operand_num) {
    HloInstruction* operand = clone->mutable_operand(operand_num);

    // See if this operand is already an operand of the fusion node.
    CHECK_EQ(operands_.size(), fused_parameters_.size());
    HloInstruction* fused_param = nullptr;
    for (int64 i = 0; i < operands_.size(); ++i) {
      if (operands_[i] == operand) {
        fused_param = fused_parameters_[i];
        break;
      }
    }

    if (fused_param == nullptr) {
      // Clone's operand was not already an operand of the fusion
      // instruction. Add it as an operand and add a corresponding fused
      // parameter instruction.
      int64 param_no = fused_parameters_.size();
      // Name the parameter after the instruction it represents in the outer
      // (non-fusion) computation. Strip the leading "%" from the operand name
      // to avoid a double %%.
      string param_name =
          StrCat(operand->name().substr(1), ".param_", param_no);
      std::unique_ptr<HloInstruction> param_instruction =
          CreateParameter(param_no, operand->shape(), param_name);

      param_instruction->parent_fusion_instruction_ = this;
      fused_param = fused_instructions_computation_->AddParameter(
          std::move(param_instruction));
      AppendOperand(operand);
    }
    TF_CHECK_OK(clone->ReplaceOperandWith(operand_num, fused_param));
  }

  for (HloComputation* computation :
       instruction_to_fuse->called_computations()) {
    if (std::find(called_computations_.begin(), called_computations_.end(),
                  computation) == called_computations_.end()) {
      called_computations_.push_back(computation);
    }
  }

  return clone;
}

RandomDistribution HloInstruction::random_distribution() const {
  CHECK_EQ(opcode_, HloOpcode::kRng);
  return distribution_;
}

void HloInstruction::CheckFusionInstruction() const {
  CHECK_EQ(opcode_, HloOpcode::kFusion);
  CHECK(fused_instructions_computation_ != nullptr &&
        fused_instructions_computation_->IsFusionComputation());

  const std::list<std::unique_ptr<HloInstruction>>& fused_instructions_ =
      fused_instructions_computation_->instructions();
  // All instructions owned by this fusion instruction must be fused, and the
  // parent fusion instruction of the fused instructions must be 'this'.
  for (auto& instruction : fused_instructions_) {
    CHECK(instruction->IsFused());
    CHECK_EQ(this, instruction->fusion_instruction());
    CHECK_EQ(fused_instructions_computation_.get(), instruction->parent())
        << instruction->ToString();
  }

  // Fused root instruction and fused parameters must all be owned by the fusion
  // instruction.
  bool root_owned = false;
  const std::vector<HloInstruction*>& fused_parameters_ = fused_parameters();
  const HloInstruction* fused_root_ = fused_expression_root();
  std::vector<bool> parameter_owned(fused_parameters_.size(), false);
  for (auto& instruction : fused_instructions_) {
    if (fused_root_ == instruction.get()) {
      CHECK(!root_owned);
      root_owned = true;
    }
    for (int i = 0; i < fused_parameters_.size(); ++i) {
      if (fused_parameters_[i] == instruction.get()) {
        CHECK(!parameter_owned[i]);
        parameter_owned[i] = true;
      }
    }
  }
  CHECK(root_owned);
  // Make sure all the parameter_owned entries are set
  for (int i = 0; i < parameter_owned.size(); i++) {
    CHECK(parameter_owned[i]);
  }

  // Fused root must have no users.
  CHECK_EQ(0, fused_root_->user_count());

  // All uses of fused instructions must be in the fusion instruction, and every
  // non-root instruction must have at least one use.
  for (auto& instruction : fused_instructions_) {
    if (instruction.get() != fused_root_) {
      CHECK_GT(instruction->user_count(), 0);
      for (auto& user : instruction->users()) {
        CHECK(user->IsFused());
        CHECK_EQ(this, user->fusion_instruction());
      }
    }
  }

  // Fused parameter instructions must be numbered contiguously and match up
  // (shapes compatible) with their respective operand.
  CHECK_EQ(operands_.size(), fused_parameters_.size());
  std::vector<bool> parameter_numbers(fused_parameters_.size(), false);
  for (auto fused_param : fused_parameters_) {
    int64 param_no = fused_param->parameter_number();
    CHECK_GE(param_no, 0);
    CHECK_LT(param_no, fused_parameters_.size());
    CHECK(!parameter_numbers[param_no]);
    parameter_numbers[param_no] = true;
    CHECK(ShapeUtil::Compatible(fused_param->shape(),
                                operands_[param_no]->shape()));
  }
  // Make sure all the parameter_numbers entries were seen
  for (int i = 0; i < parameter_numbers.size(); i++) {
    CHECK(parameter_numbers[i]);
  }

  // Operands must be distinct.
  std::set<HloInstruction*> operand_set(operands_.begin(), operands_.end());
  CHECK_EQ(operand_set.size(), operands_.size());
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateCall(
    const Shape& shape, tensorflow::gtl::ArraySlice<HloInstruction*> operands,
    HloComputation* computation) {
  std::unique_ptr<HloInstruction> instruction =
      WrapUnique(new HloInstruction(HloOpcode::kCall, shape));
  for (auto operand : operands) {
    instruction->AppendOperand(operand);
  }
  instruction->called_computations_.push_back(computation);
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateCustomCall(
    const Shape& shape, tensorflow::gtl::ArraySlice<HloInstruction*> operands,
    tensorflow::StringPiece custom_call_target) {
  std::unique_ptr<HloInstruction> instruction =
      WrapUnique(new HloInstruction(HloOpcode::kCustomCall, shape));
  for (auto operand : operands) {
    instruction->AppendOperand(operand);
  }
  instruction->custom_call_target_ = custom_call_target.ToString();
  return instruction;
}

/* static */ std::unique_ptr<HloInstruction> HloInstruction::CreateTuple(
    tensorflow::gtl::ArraySlice<HloInstruction*> elements) {
  std::vector<Shape> element_shapes;
  for (auto element : elements) {
    element_shapes.push_back(element->shape());
  }
  Shape tuple_shape = ShapeUtil::MakeTupleShape(element_shapes);
  return CreateVariadic(tuple_shape, HloOpcode::kTuple, elements);
}

std::unique_ptr<HloInstruction> HloInstruction::CloneWithNewOperands(
    const Shape& shape,
    tensorflow::gtl::ArraySlice<HloInstruction*> new_operands) {
  // Explicitly call the factory for the instruction type. This is more robust
  // in the face of code changes than copying fields explicitly. This also
  // properly sets the user fields of the operands.
  switch (opcode_) {
    // Unary ops.
    case HloOpcode::kAbs:
    case HloOpcode::kBitcast:
    case HloOpcode::kCeil:
    case HloOpcode::kCopy:
    case HloOpcode::kExp:
    case HloOpcode::kIsFinite:
    case HloOpcode::kFloor:
    case HloOpcode::kLog:
    case HloOpcode::kLogicalNot:
    case HloOpcode::kNegate:
    case HloOpcode::kSign:
    case HloOpcode::kSort:
    case HloOpcode::kTanh:
      CHECK_EQ(new_operands.size(), 1);
      return CreateUnary(shape, opcode_, new_operands[0]);
    // Binary ops.
    case HloOpcode::kAdd:
    case HloOpcode::kDivide:
    case HloOpcode::kMultiply:
    case HloOpcode::kSubtract:
    case HloOpcode::kEq:
    case HloOpcode::kGe:
    case HloOpcode::kGt:
    case HloOpcode::kLe:
    case HloOpcode::kLt:
    case HloOpcode::kNe:
    case HloOpcode::kDot:
    case HloOpcode::kMaximum:
    case HloOpcode::kMinimum:
    case HloOpcode::kPower:
    case HloOpcode::kRemainder:
    case HloOpcode::kLogicalAnd:
    case HloOpcode::kLogicalOr:
      CHECK_EQ(new_operands.size(), 2);
      return CreateBinary(shape, opcode_, new_operands[0], new_operands[1]);
    // Ternary ops.
    case HloOpcode::kClamp:
    case HloOpcode::kSelect:
      CHECK_EQ(new_operands.size(), 3);
      return CreateTernary(shape, opcode_, new_operands[0], new_operands[1],
                           new_operands[2]);
    // Other supported ops.
    case HloOpcode::kBroadcast:
      CHECK_EQ(new_operands.size(), 1);
      return CreateBroadcast(shape, new_operands[0], dimensions_);
    case HloOpcode::kCall:
      return CreateCall(shape, new_operands, to_apply());
    case HloOpcode::kCustomCall:
      return CreateCustomCall(shape, new_operands, custom_call_target_);
    case HloOpcode::kConcatenate:
      return CreateConcatenate(shape, new_operands, dimensions(0));
    case HloOpcode::kConvert:
      CHECK_EQ(new_operands.size(), 1);
      return CreateConvert(shape, new_operands[0]);
    case HloOpcode::kConvolution:
      CHECK_EQ(new_operands.size(), 2);
      return CreateConvolve(shape, new_operands[0], new_operands[1], *window_,
                            *convolution_dimension_numbers_);
    case HloOpcode::kCrossReplicaSum:
      CHECK_EQ(new_operands.size(), 1);
      return CreateCrossReplicaSum(shape, new_operands[0]);
    case HloOpcode::kGetTupleElement:
      CHECK_EQ(new_operands.size(), 1);
      return CreateGetTupleElement(shape, new_operands[0], tuple_index());
    case HloOpcode::kMap:
      return CreateMap(shape, new_operands, to_apply());
    case HloOpcode::kPad:
      CHECK_EQ(new_operands.size(), 2);
      return CreatePad(shape, new_operands[0], new_operands[1],
                       *padding_config_);
    case HloOpcode::kReduce:
      CHECK_EQ(new_operands.size(), 2);
      return CreateReduce(shape, new_operands[0], new_operands[1], dimensions_,
                          to_apply());
    case HloOpcode::kReduceWindow:
      CHECK_EQ(new_operands.size(), 2);
      return CreateReduceWindow(shape, new_operands[0], new_operands[1],
                                *window_, to_apply());
    case HloOpcode::kSelectAndScatter:
      CHECK_EQ(new_operands.size(), 3);
      return CreateSelectAndScatter(shape, new_operands[0], select(), *window_,
                                    new_operands[1], new_operands[2],
                                    scatter());
    case HloOpcode::kReverse:
      CHECK_EQ(new_operands.size(), 1);
      return CreateReverse(shape, new_operands[0], dimensions_);
    case HloOpcode::kRng:
      return CreateRng(shape, distribution_, new_operands);
    case HloOpcode::kReshape:
      CHECK_EQ(new_operands.size(), 1);
      return CreateReshape(shape, new_operands[0]);
    case HloOpcode::kSlice:
      CHECK_EQ(new_operands.size(), 1);
      return CreateSlice(shape, new_operands[0], slice_starts_, slice_limits_);
    case HloOpcode::kDynamicSlice:
      return CreateDynamicSlice(shape, new_operands[0], new_operands[1],
                                dynamic_slice_sizes_);
    case HloOpcode::kDynamicUpdateSlice:
      CHECK_EQ(new_operands.size(), 3);
      return CreateDynamicUpdateSlice(shape, new_operands[0], new_operands[1],
                                      new_operands[2]);
    case HloOpcode::kTranspose:
      CHECK_EQ(new_operands.size(), 1);
      return CreateTranspose(shape, new_operands[0], dimensions_);
    case HloOpcode::kTuple:
      return CreateTuple(new_operands);
    case HloOpcode::kWhile:
      CHECK_EQ(new_operands.size(), 1);
      return CreateWhile(shape, while_condition(), while_body(),
                         new_operands[0]);
    case HloOpcode::kConstant:
      return CreateConstant(LiteralUtil::CloneToUnique(*literal_));
    case HloOpcode::kFusion:
      return CloneFusionWithNewOperands(shape, new_operands);
    case HloOpcode::kParameter:
      return CreateParameter(parameter_number_, shape, parameter_name_);
    // Unsupported ops for cloning.
    case HloOpcode::kRecv:
    case HloOpcode::kSend:
    case HloOpcode::kUpdate:
    case HloOpcode::kIndex:
    case HloOpcode::kInfeed:
    case HloOpcode::kOutfeed:
    case HloOpcode::kTrace:
      LOG(FATAL) << "Not yet implemented, clone: " << HloOpcodeString(opcode_);
  }
}

HloInstruction::~HloInstruction() {}

std::unique_ptr<HloInstruction> HloInstruction::Clone(const string& suffix) {
  std::unique_ptr<HloInstruction> clone =
      CloneWithNewOperands(shape_, operands_);
  if (suffix.empty()) {
    clone->name_ = name();
  } else {
    // If an instruction is cloned multiple times avoid names like
    // foo.suffix.suffix.suffix. Instead of repeating the suffix add a numeric
    // suffix. Specifically, the clone of foo.suffix is named foo.suffix2, the
    // clone of foo.suffix2 is named foo.suffix3 and so on.
    const string dot_suffix = "." + suffix;
    size_t index = name().rfind(dot_suffix);
    if (index == string::npos) {
      // Existing name does not include ".suffix".
      clone->name_ = name() + dot_suffix;
    } else {
      // Existing name includes ".suffix". Determine if substring after
      // ".suffix" is numeric and should be replaced with an incremented number.
      string after_suffix = name().substr(index + dot_suffix.size());
      if (after_suffix.empty()) {
        // Existing name ends in ".suffix". New name should end in ".suffix2".
        clone->name_ = name() + "2";
      } else {
        // If names ends with .suffix[0-9]+ then replace with a suffix with the
        // numeric value incremented.
        int64 numeric_suffix;
        if (tensorflow::strings::safe_strto64(after_suffix, &numeric_suffix)) {
          clone->name_ =
              StrCat(name().substr(0, index), dot_suffix, numeric_suffix + 1);
        } else {
          // Substring after ".suffix" is non-numeric.
          clone->name_ = name() + dot_suffix;
        }
      }
    }
  }
  clone->set_parent(parent());
  clone->set_metadata(metadata_);
  return clone;
}

std::unique_ptr<HloInstruction> HloInstruction::CloneFusionWithNewOperands(
    const Shape& shape, tensorflow::gtl::ArraySlice<HloInstruction*> operands) {
  CHECK_EQ(opcode_, HloOpcode::kFusion);
  CHECK(parent() != nullptr);
  CHECK(fused_instructions_computation_ != nullptr &&
        fused_instructions_computation_->IsFusionComputation());

  auto new_instruction =
      WrapUnique(new HloInstruction(HloOpcode::kFusion, shape));
  // Add the operands to our new fusion instruction.
  for (HloInstruction* new_operand : operands) {
    new_instruction->AppendOperand(new_operand);
  }
  // Clone all the fused instructions for the new fusion instruction.
  std::map<HloInstruction*, HloInstruction*> old_to_new;
  std::list<std::unique_ptr<HloInstruction>> new_fused_instructions;
  // Create the list of fused parameters by mapping through the cloned,
  // fused instructions.
  std::vector<HloInstruction*> new_fused_parameters;
  const std::vector<HloInstruction*>& fused_parameters_ =
      fused_instructions_computation_->parameter_instructions();
  const std::list<std::unique_ptr<HloInstruction>>& fused_instructions_ =
      fused_instructions_computation_->instructions();

  for (HloInstruction* old_fused_parameter : fused_parameters_) {
    new_fused_instructions.push_back(old_fused_parameter->Clone());
    HloInstruction* new_fusion_parameter = new_fused_instructions.back().get();
    new_fusion_parameter->parent_fusion_instruction_ = new_instruction.get();
    new_fused_parameters.push_back(new_fusion_parameter);
    InsertOrDie(&old_to_new, old_fused_parameter, new_fusion_parameter);
  }
  for (auto old_fused_instruction_iter = fused_instructions_.rbegin();
       old_fused_instruction_iter != fused_instructions_.rend();
       ++old_fused_instruction_iter) {
    HloInstruction* old_fused_instruction = old_fused_instruction_iter->get();
    if (old_fused_instruction->opcode() == HloOpcode::kParameter) {
      FindOrDie(old_to_new, old_fused_instruction);
      continue;
    }
    std::vector<HloInstruction*> new_operands;
    for (int64 operand_idx = 0;
         operand_idx < old_fused_instruction->operand_count(); ++operand_idx) {
      HloInstruction* old_operand =
          old_fused_instruction->mutable_operand(operand_idx);
      new_operands.push_back(FindOrDie(old_to_new, old_operand));
    }
    new_fused_instructions.push_back(
        old_fused_instruction->CloneWithNewOperands(
            old_fused_instruction->shape(), new_operands));
    HloInstruction* new_fused_instruction = new_fused_instructions.back().get();
    new_fused_instruction->set_parent(parent());
    new_fused_instruction->parent_fusion_instruction_ = new_instruction.get();
    InsertOrDie(&old_to_new, old_fused_instruction, new_fused_instruction);
  }
  new_instruction->fusion_kind_ = fusion_kind_;
  auto computation_builder = HloComputation::Builder(
      fused_instructions_computation_->name() + ".clone", true);
  // We iterated the fusion instructions in reverse post order which means
  // that we must reverse our new list of fusion instructions.
  for (auto new_fused_instruction_iter = new_fused_instructions.rbegin();
       new_fused_instruction_iter != new_fused_instructions.rend();
       ++new_fused_instruction_iter) {
    computation_builder.AddInstruction(std::move(*new_fused_instruction_iter));
  }
  auto fused_root_ = fused_expression_root();
  new_instruction->fused_instructions_computation_ =
      computation_builder.Build(FindOrDie(old_to_new, fused_root_));
  new_instruction->set_parent(parent());
  new_instruction->CheckFusionInstruction();
  return new_instruction;
}

const Literal& HloInstruction::literal() const {
  CHECK_EQ(HloOpcode::kConstant, opcode_);
  return *literal_;
}

bool HloInstruction::CanHaveDimensionsField() const {
  return (opcode() == HloOpcode::kReverse ||
          opcode() == HloOpcode::kConcatenate ||
          opcode() == HloOpcode::kReduce || opcode() == HloOpcode::kBroadcast ||
          opcode() == HloOpcode::kTranspose);
}

const std::vector<int64>& HloInstruction::dimensions() const {
  CHECK(CanHaveDimensionsField());
  return dimensions_;
}

int64 HloInstruction::dimensions(int64 index) const {
  return dimensions()[index];
}

int64 HloInstruction::concatenate_dimension() const {
  CHECK(opcode() == HloOpcode::kConcatenate);
  CHECK_EQ(1, dimensions_.size());
  return dimensions(0);
}

int64 HloInstruction::tuple_index() const {
  CHECK_EQ(HloOpcode::kGetTupleElement, opcode_);
  return tuple_index_;
}

const HloInstruction* HloInstruction::operand(int64 i) const {
  return operands_[i];
}

HloInstruction* HloInstruction::mutable_operand(int64 i) {
  CHECK(operands_[i] != nullptr);
  return operands_[i];
}

int64 HloInstruction::operand_index(const HloInstruction* target) const {
  for (int64 i = 0; i < operand_count(); ++i) {
    if (target == operand(i)) {
      return i;
    }
  }
  LOG(FATAL) << "target was not an operand";
}

Status HloInstruction::AddControlDependencyTo(HloInstruction* instruction) {
  TF_RET_CHECK(instruction->parent() == parent());
  if (std::find(control_successors_.begin(), control_successors_.end(),
                instruction) == control_successors_.end()) {
    control_successors_.push_back(instruction);
    TF_RET_CHECK(std::find(instruction->control_predecessors_.begin(),
                           instruction->control_predecessors_.end(),
                           this) == instruction->control_predecessors_.end());
    instruction->control_predecessors_.push_back(this);
  }
  return Status::OK();
}

Status HloInstruction::RemoveControlDependencyTo(HloInstruction* instruction) {
  auto succ_it = std::find(control_successors_.begin(),
                           control_successors_.end(), instruction);
  TF_RET_CHECK(succ_it != control_successors_.end());
  control_successors_.erase(succ_it);
  auto pred_it = std::find(instruction->control_predecessors_.begin(),
                           instruction->control_predecessors_.end(), this);
  TF_RET_CHECK(pred_it != instruction->control_predecessors_.end());
  instruction->control_predecessors_.erase(succ_it);

  return Status::OK();
}

void HloInstruction::AppendOperand(HloInstruction* operand) {
  operands_.push_back(operand);
  operand->AddUser(this);
}

void HloInstruction::AddUser(HloInstruction* user) {
  if (!ContainsKey(user_set_, user)) {
    user_set_.insert(user);
    users_.push_back(user);
  }
}

bool HloInstruction::IsConstant() const {
  return opcode_ == HloOpcode::kConstant;
}

bool HloInstruction::HasConstantOperand() const {
  for (const HloInstruction* operand : operands_) {
    if (operand->IsConstant()) {
      return true;
    }
  }
  return false;
}

bool HloInstruction::Identical(
    const HloInstruction& other,
    std::function<bool(const HloInstruction*, const HloInstruction*)>
        eq_operands,
    std::function<bool(const HloComputation*, const HloComputation*)>
        eq_computations) const {
  // An instruction is always identical to itself.
  if (this == &other) {
    return true;
  }

  // Identical instruction must have the same opcode and identical operands.  In
  // general, there is no need to check shape because shape is inferred from the
  // shape of the operands.
  if (opcode() != other.opcode() ||
      !ContainersEqual(operands(), other.operands(), std::move(eq_operands))) {
    return false;
  }

  // Perform opcode specific checks.
  switch (opcode()) {
    // The result of these instructions only depend upon their opcode and
    // operands.
    case HloOpcode::kAbs:
    case HloOpcode::kAdd:
    case HloOpcode::kCeil:
    case HloOpcode::kClamp:
    case HloOpcode::kCopy:
    case HloOpcode::kCrossReplicaSum:
    case HloOpcode::kDivide:
    case HloOpcode::kDot:
    case HloOpcode::kEq:
    case HloOpcode::kExp:
    case HloOpcode::kFloor:
    case HloOpcode::kGe:
    case HloOpcode::kGt:
    case HloOpcode::kIsFinite:
    case HloOpcode::kLe:
    case HloOpcode::kLog:
    case HloOpcode::kLogicalAnd:
    case HloOpcode::kLogicalNot:
    case HloOpcode::kLogicalOr:
    case HloOpcode::kLt:
    case HloOpcode::kMaximum:
    case HloOpcode::kMinimum:
    case HloOpcode::kMultiply:
    case HloOpcode::kNe:
    case HloOpcode::kNegate:
    case HloOpcode::kPower:
    case HloOpcode::kRemainder:
    case HloOpcode::kSelect:
    case HloOpcode::kSign:
    case HloOpcode::kSubtract:
    case HloOpcode::kTanh:
    case HloOpcode::kTuple:
      return true;

    // These opcodes have complex or special behavior so just return false.
    case HloOpcode::kFusion:
    case HloOpcode::kRng:
    case HloOpcode::kTrace:
    case HloOpcode::kWhile:
      return false;

    case HloOpcode::kParameter:
      return parameter_number() == other.parameter_number() &&
             // Check the shape too because `this` and `other` may be in
             // different HloComputations.
             ShapeUtil::Compatible(shape(), other.shape());

    // A constant is defined by the value in the literal.
    case HloOpcode::kConstant:
      return LiteralUtil::Equal(literal(), other.literal());

    // A convert result is determined by the primitive type that the operand is
    // converted into.
    case HloOpcode::kConvert:
      return shape().element_type() == other.shape().element_type();

    // Convolution has a window and dimensions.
    case HloOpcode::kConvolution:
      return protobuf_util::ProtobufEquals(window(), other.window()) &&
             protobuf_util::ProtobufEquals(
                 convolution_dimension_numbers(),
                 other.convolution_dimension_numbers());

    // Reduction results are determined by the reduction dimension and the
    // reduction computation.
    case HloOpcode::kReduce:
      return dimensions() == other.dimensions() &&
             eq_computations(to_apply(), other.to_apply());
    case HloOpcode::kReduceWindow:
      return eq_computations(to_apply(), other.to_apply()) &&
             protobuf_util::ProtobufEquals(window(), other.window());

    // SelectAndScatter is determined by both select and scatter
    // computation as well as the window configuration.
    case HloOpcode::kSelectAndScatter:
      return eq_computations(select(), other.select()) &&
             eq_computations(scatter(), other.scatter()) &&
             protobuf_util::ProtobufEquals(window(), other.window());

    case HloOpcode::kReshape:
      return ShapeUtil::Compatible(shape(), other.shape());

    // Transpose result is determined by the final shape and the permutation.
    case HloOpcode::kTranspose:
      return ShapeUtil::Compatible(shape(), other.shape()) &&
             dimensions() == other.dimensions();

    // Remaining instructions with special values.
    case HloOpcode::kBitcast:
      return ShapeUtil::Equal(shape(), other.shape());
    case HloOpcode::kBroadcast:
      return ShapeUtil::Compatible(shape(), other.shape()) &&
             dimensions() == other.dimensions();
    case HloOpcode::kConcatenate:
      return dimensions() == other.dimensions();
    case HloOpcode::kGetTupleElement:
      return tuple_index() == other.tuple_index();
    case HloOpcode::kPad:
      return protobuf_util::ProtobufEquals(padding_config(),
                                           other.padding_config());
    case HloOpcode::kSlice:
      return slice_starts_ == other.slice_starts_ &&
             slice_limits_ == other.slice_limits_;
    case HloOpcode::kDynamicSlice:
      return ShapeUtil::Compatible(shape(), other.shape()) &&
             dynamic_slice_sizes_ == other.dynamic_slice_sizes_;
    case HloOpcode::kDynamicUpdateSlice:
      return ShapeUtil::Compatible(shape(), other.shape());
    case HloOpcode::kCall:
    case HloOpcode::kMap:
      return eq_computations(to_apply(), other.to_apply());
    case HloOpcode::kCustomCall:
      return custom_call_target_ == other.custom_call_target_;
    case HloOpcode::kReverse:
      return dimensions() == other.dimensions();

    // These opcodes are not yet supported.
    case HloOpcode::kIndex:
    case HloOpcode::kInfeed:
    case HloOpcode::kOutfeed:
    case HloOpcode::kSort:
    case HloOpcode::kUpdate:
    case HloOpcode::kSend:
    case HloOpcode::kRecv:
      return false;
  }
}

bool HloInstruction::IsRank2Transpose() const {
  return (opcode_ == HloOpcode::kTranspose) &&
         dimensions_ == std::vector<int64>({1, 0}) &&
         shape_.dimensions_size() == 2 &&
         std::equal(shape_.dimensions().begin(), shape_.dimensions().end(),
                    operands_[0]->shape_.dimensions().rbegin());
}

void HloInstruction::RemoveUser(HloInstruction* user) {
  auto set_it = user_set_.find(user);
  CHECK(set_it != user_set_.end());
  user_set_.erase(set_it);
  // This is linear in the number of the users, but a vector provides a stable
  // iteration order and much faster traversal.
  auto vec_it = std::find(users_.begin(), users_.end(), user);
  CHECK(vec_it != users_.end());
  users_.erase(vec_it);
}

Status HloInstruction::ReplaceUseWith(HloInstruction* user,
                                      HloInstruction* new_producer) {
  TF_RET_CHECK(ShapeUtil::Compatible(shape(), new_producer->shape()))
      << "this shape: " << ShapeUtil::HumanString(shape())
      << ", replacement shape: "
      << ShapeUtil::HumanString(new_producer->shape());

  VLOG(3) << "Replacing uses of " << name() << " in " << user->name()
          << " with " << new_producer->name();

  RemoveUser(user);

  TF_RET_CHECK(
      std::count(user->operands_.begin(), user->operands_.end(), this) >= 0);
  std::replace(user->operands_.begin(), user->operands_.end(), this,
               new_producer);
  new_producer->AddUser(user);
  return Status::OK();
}

Status HloInstruction::ReplaceOperandWith(int64 operand_num,
                                          HloInstruction* new_operand) {
  TF_RET_CHECK(operand_num >= 0);
  TF_RET_CHECK(operand_num < operand_count());
  HloInstruction* old_operand = mutable_operand(operand_num);
  TF_RET_CHECK(
      ShapeUtil::Compatible(old_operand->shape(), new_operand->shape()))
      << old_operand->shape().ShortDebugString() << " is not compatible with "
      << new_operand->shape().ShortDebugString();
  operands_[operand_num] = new_operand;

  VLOG(3) << "Replacing operand " << operand_num << " of " << name() << " with "
          << new_operand->name() << ", was " << old_operand->name();

  if (std::find(operands_.begin(), operands_.end(), old_operand) ==
      operands_.end()) {
    old_operand->RemoveUser(this);
  }
  new_operand->AddUser(this);
  return Status::OK();
}

Status HloInstruction::ReplaceAllUsesWith(HloInstruction* new_producer) {
  bool new_producer_is_user = false;
  for (HloInstruction* user : users()) {
    if (user == new_producer) {
      // It's possible that new_producer is a user of this instruction as might
      // be the case when replacing an instruction with a kCopy of itself. In
      // this case, don't do the replacement to avoid creating a cycle in the
      // graph. new_producer remains the only user of this instruction.
      new_producer_is_user = true;
    } else {
      std::replace(user->operands_.begin(), user->operands_.end(), this,
                   new_producer);
      new_producer->AddUser(user);
    }
  }
  users_.clear();
  user_set_.clear();
  if (new_producer_is_user) {
    AddUser(new_producer);
  }

  return Status::OK();
}

void HloInstruction::DetachFromOperands() {
  CHECK_EQ(0, user_count());
  // An intruction may be repeated as an operand. To avoid calling RemoveUser
  // twice on the same operand, keep a set of already detached operands.
  std::set<HloInstruction*> detached_operands;
  for (int64 operand_num = 0; operand_num < operand_count(); ++operand_num) {
    HloInstruction* operand = operands_[operand_num];
    if (!ContainsKey(detached_operands, operand)) {
      operand->RemoveUser(this);
      detached_operands.insert(operand);
    }
    operands_[operand_num] = nullptr;
  }
}

HloComputation* HloInstruction::to_apply() const {
  switch (opcode_) {
    case HloOpcode::kCall:
    case HloOpcode::kMap:
    case HloOpcode::kReduceWindow:
    case HloOpcode::kReduce:
      CHECK_EQ(called_computations_.size(), 1);
      return called_computations_[0];
    default:
      LOG(FATAL) << "Invalid opcode for to_apply(): "
                 << HloOpcodeString(opcode());
  }
}

void HloInstruction::set_to_apply(HloComputation* computation) {
  // Don't allow changing the computation for fused instructions so we don't
  // have to recompute called_instructions for the entire fusion instruction.
  CHECK(!IsFused());
  switch (opcode_) {
    case HloOpcode::kCall:
    case HloOpcode::kMap:
    case HloOpcode::kReduceWindow:
    case HloOpcode::kReduce:
      CHECK_EQ(called_computations_.size(), 1);
      called_computations_[0] = computation;
      break;
    default:
      LOG(FATAL) << "Invalid opcode for to_apply(): "
                 << HloOpcodeString(opcode());
  }
}

const string& HloInstruction::custom_call_target() const {
  CHECK_EQ(opcode_, HloOpcode::kCustomCall);
  return custom_call_target_;
}

const string& HloInstruction::outfeed_config() const {
  CHECK_EQ(opcode_, HloOpcode::kOutfeed);
  return outfeed_config_;
}

HloComputation* HloInstruction::while_condition() const {
  CHECK_EQ(HloOpcode::kWhile, opcode_);
  return called_computations_[kConditionComputationIndex];
}

HloComputation* HloInstruction::while_body() const {
  CHECK_EQ(HloOpcode::kWhile, opcode_);
  return called_computations_[kBodyComputationIndex];
}

void HloInstruction::set_while_condition(HloComputation* computation) {
  // Don't allow changing the computation for fused instructions so we don't
  // have to recompute called_instructions for the entire fusion instruction.
  CHECK(!IsFused());
  CHECK_EQ(HloOpcode::kWhile, opcode_);
  called_computations_[kConditionComputationIndex] = computation;
}

void HloInstruction::set_while_body(HloComputation* computation) {
  // Don't allow changing the computation for fused instructions so we don't
  // have to recompute called_instructions for the entire fusion instruction.
  CHECK(!IsFused());
  CHECK_EQ(HloOpcode::kWhile, opcode_);
  called_computations_[kBodyComputationIndex] = computation;
}

HloComputation* HloInstruction::select() const {
  CHECK_EQ(HloOpcode::kSelectAndScatter, opcode_);
  return called_computations_[kSelectComputationIndex];
}

HloComputation* HloInstruction::scatter() const {
  CHECK_EQ(HloOpcode::kSelectAndScatter, opcode_);
  return called_computations_[kScatterComputationIndex];
}

void HloInstruction::set_select(HloComputation* computation) {
  // Don't allow changing the computation for fused instructions so we don't
  // have to recompute called_instructions for the entire fusion instruction.
  CHECK(!IsFused());
  CHECK_EQ(HloOpcode::kSelectAndScatter, opcode_);
  called_computations_[kSelectComputationIndex] = computation;
}

void HloInstruction::set_scatter(HloComputation* computation) {
  // Don't allow changing the computation for fused instructions so we don't
  // have to recompute called_instructions for the entire fusion instruction.
  CHECK(!IsFused());
  CHECK_EQ(HloOpcode::kSelectAndScatter, opcode_);
  called_computations_[kScatterComputationIndex] = computation;
}

string HloInstruction::SignatureString() const {
  string operands =
      Join(operands_, ", ", [](string* out, HloInstruction* operand) {
        StrAppend(out, ShapeUtil::HumanString(operand->shape()));
      });
  return StrCat("(", operands, ") -> ", ShapeUtil::HumanString(shape()));
}

string HloInstruction::ExtendedOpcodeStr() const {
  string opc_name = HloOpcodeString(opcode());
  HloOpcode opc = opcode();
  if (HloOpcode::kFusion == opc) {
    opc_name += ":" + xla::ToString(fusion_kind());
  }
  return opc_name;
}

string HloInstruction::ToString(bool compact_operands,
                                bool include_metadata) const {
  string operands;
  if (opcode() == HloOpcode::kConstant) {
    // For constants, show the actual value in place of an empty operand list.
    if (ShapeUtil::ElementsIn(shape()) <= 10) {
      // LiteralUtil::ToString emits multidimensional arrays over multiple
      // lines. Compact this into one line by stripping out white space.
      string tmp = LiteralUtil::ToString(literal());
      std::replace(tmp.begin(), tmp.end(), '\n', ' ');
      std::vector<string> v = tensorflow::str_util::Split(tmp, ' ');
      bool first = true;
      // Concatenate elements in "v" with spaces separating them, but ignoring
      // empty entries.
      for (const auto& s : v) {
        if (s.empty()) continue;
        StrAppend(&operands, (first ? "" : " "), s);
        first = false;
      }
    } else {
      // Do not show large constants.
      operands = "{...}";
    }
  } else if (opcode() == HloOpcode::kParameter) {
    operands = Printf("%lld", parameter_number_);
  } else {
    tensorflow::gtl::ArraySlice<HloInstruction*> slice(operands_);
    const int64 kMaxOperandsToShowIfCompact = 4;
    if (compact_operands && slice.size() > kMaxOperandsToShowIfCompact) {
      slice.remove_suffix(slice.size() - kMaxOperandsToShowIfCompact);
    }
    operands = Join(slice, ", ", [&](string* out, HloInstruction* operand) {
      *out += ShapeUtil::HumanStringWithLayout(operand->shape());
      if (!compact_operands) {
        StrAppend(out, " ", operand->name());
      }
    });
    const int64 remaining = operands_.size() - slice.size();
    if (slice.size() != operands_.size()) {
      StrAppend(&operands, ", ...(+", remaining, ")");
    }
  }
  string extra;
  if (CanHaveDimensionsField()) {
    StrAppend(&extra, ", dimensions={", Join(dimensions(), ","), "}");
  }
  if (window_ != nullptr) {
    StrAppend(&extra, ", ", window_util::ToString(*window_));
  }
  if (padding_config_ != nullptr) {
    StrAppend(&extra, ", padding=", padding_config_->ShortDebugString());
  }
  if (!slice_starts_.empty() && !slice_limits_.empty()) {
    std::vector<string> bounds;
    for (int i = 0; i < slice_starts_.size(); ++i) {
      bounds.push_back(
          StrCat("[", slice_starts_[i], ":", slice_limits_[i], "]"));
    }
    StrAppend(&extra, ", slice={", Join(bounds, ", "), "}");
  }

  if (convolution_dimension_numbers_ != nullptr) {
    StrAppend(&extra, ", ", ConvolutionDimensionNumbersToString());
  }

  if (opcode() == HloOpcode::kWhile) {
    StrAppend(&extra, ", condition=", while_condition()->name());
    StrAppend(&extra, ", body=", while_body()->name());
  } else if (opcode() == HloOpcode::kSelectAndScatter) {
    StrAppend(&extra, ", select=", select()->name());
    StrAppend(&extra, ", scatter=", scatter()->name());
  } else if (!called_computations().empty()) {
    StrAppend(&extra, ", calls=",
              Join(called_computations(), ", ",
                   [](string* out, const HloComputation* computation) {
                     StrAppend(out, computation->name());
                   }));
  }

  if (opcode() == HloOpcode::kGetTupleElement) {
    StrAppend(&extra, ", index=", tuple_index());
  }
  if (include_metadata &&
      (!metadata_.op_type().empty() || !metadata_.op_name().empty() ||
       !metadata_.source_file().empty())) {
    StrAppend(&extra, " # metadata=", metadata_.ShortDebugString());
  }

  return Printf("%s = %s %s(%s)%s", name().c_str(),
                ShapeUtil::HumanStringWithLayout(shape()).c_str(),
                ExtendedOpcodeStr().c_str(), operands.c_str(), extra.c_str());
}

string HloInstruction::ToShortString() const {
  return Printf("%s = %s(%s)", name().c_str(),
                HloOpcodeString(opcode()).c_str(),
                Join(operands_, ", ",
                     [](string* out, HloInstruction* operand) {
                       StrAppend(out, operand->name());
                     })
                    .c_str());
}

string HloInstruction::ToCategory() const {
  if (opcode() == HloOpcode::kTranspose || opcode() == HloOpcode::kCopy ||
      opcode() == HloOpcode::kReshape) {
    return "data formatting";
  }

  if (opcode() == HloOpcode::kConvolution) {
    string category = "convolution";
    if (window_util::HasBaseDilation(window())) {
      category += " base-dilated";
    }
    if (window_util::HasWindowDilation(window())) {
      category += " window-dilated";
    }
    return category;
  }

  if (opcode() == HloOpcode::kFusion) {
    if (operands().size() == 2) {
      bool saw_rank_1 = false;
      bool saw_higher_rank = false;
      for (const auto* operand : operands()) {
        saw_rank_1 |= ShapeUtil::Rank(operand->shape()) == 1;
        saw_higher_rank |= ShapeUtil::Rank(operand->shape()) > 1;
      }
      if (saw_rank_1 && saw_higher_rank) {
        return "rank-1-broadcast binary fusion";
      }
    }
    switch (fusion_kind()) {
      case FusionKind::kLoop:
        if (IsElementwise()) {
          return "elementwise fusion";
        } else {
          return "non-elementwise fusion";
        }
      case FusionKind::kInput:
        return "input fusion";
      case FusionKind::kOutput:
        return "output fusion";
      case FusionKind::kTransposeDot:
        return "dot fusion";
      case FusionKind::kConvBackwardFilter:
      case FusionKind::kConvBackwardInput:
        return "convolution fusion";
    }
  }

  if (IsElementwise() && opcode() != HloOpcode::kFusion) {
    return "non-fusion elementwise";
  }

  return HloOpcodeString(opcode());
}

string HloInstruction::FullyQualifiedName() const {
  if (IsFused()) {
    return StrCat(fusion_instruction()->parent()->name(),
                  "::", fusion_instruction()->name(), "::", name_);
  }
  return StrCat(parent_->name(), "::", name_);
}

HloInstruction* HloInstruction::tracing() const { return trace_instruction_; }

void HloInstruction::set_tracing(HloInstruction* trace_instruction) {
  trace_instruction_ = trace_instruction;
}

const string& HloInstruction::tracing_tag() const {
  CHECK_EQ(HloOpcode::kTrace, opcode());
  CHECK(literal_ != nullptr);
  return literal_->u8s();
}

bool HloInstruction::IsFused() const {
  return parent_fusion_instruction_ != nullptr;
}

bool HloInstruction::IsFusable() const {
  // Instructions which are traced should not be fused.
  if (tracing()) {
    return false;
  }

  // Some kinds of instructions don't make sense to fuse.
  switch (opcode_) {
    case HloOpcode::kInfeed:
    case HloOpcode::kOutfeed:
    case HloOpcode::kParameter:
    case HloOpcode::kTrace:
    case HloOpcode::kSend:
    case HloOpcode::kRecv:
      return false;
    // Only fuse Rng if it is used once, otherwise the random numbers generated
    // will be different in each fusion.
    case HloOpcode::kRng:
      return users_.size() == 1;
    default:
      return true;
  }
}

HloComputation* HloInstruction::fused_instructions_computation() const {
  CHECK_EQ(opcode_, HloOpcode::kFusion);
  return fused_instructions_computation_.get();
}

HloInstruction* HloInstruction::fusion_instruction() const {
  CHECK(IsFused());
  return parent_fusion_instruction_;
}

HloInstruction* HloInstruction::fused_expression_root() const {
  CHECK_EQ(opcode_, HloOpcode::kFusion);
  CHECK(fused_instructions_computation_ != nullptr &&
        fused_instructions_computation_->IsFusionComputation());
  return fused_instructions_computation_->root_instruction();
}

HloInstruction* HloInstruction::fused_parameter(int64 parameter_number) const {
  CHECK_EQ(opcode_, HloOpcode::kFusion);
  CHECK(fused_instructions_computation_ != nullptr &&
        fused_instructions_computation_->IsFusionComputation());
  return fused_instructions_computation_->parameter_instruction(
      parameter_number);
}

const std::vector<HloInstruction*>& HloInstruction::fused_parameters() const {
  CHECK_EQ(opcode_, HloOpcode::kFusion);
  CHECK(fused_instructions_computation_ != nullptr &&
        fused_instructions_computation_->IsFusionComputation());
  return fused_instructions_computation_->parameter_instructions();
}

const std::list<std::unique_ptr<HloInstruction>>&
HloInstruction::fused_instructions() const {
  CHECK_EQ(opcode_, HloOpcode::kFusion);
  CHECK(fused_instructions_computation_ != nullptr &&
        fused_instructions_computation_->IsFusionComputation());
  return fused_instructions_computation_->instructions();
}

HloInstruction::HloInstruction(HloOpcode opcode, const Shape& shape)
    : shape_(shape), opcode_(opcode), name_("%" + HloOpcodeString(opcode)) {
  TF_DCHECK_OK(ShapeUtil::ValidateShapeWithOptionalLayout(shape_));
}

Status HloInstruction::Visit(DfsHloVisitor* visitor) {
  switch (opcode_) {
    case HloOpcode::kAbs:
      return visitor->HandleAbs(this, operands_[0]);
    case HloOpcode::kSign:
      return visitor->HandleSign(this, operands_[0]);
    case HloOpcode::kConstant:
      return visitor->HandleConstant(this, *literal_);
    case HloOpcode::kGetTupleElement:
      return visitor->HandleGetTupleElement(this, operands_[0]);
    case HloOpcode::kParameter:
      return visitor->HandleParameter(this);
    case HloOpcode::kEq:
    case HloOpcode::kGe:
    case HloOpcode::kGt:
    case HloOpcode::kLe:
    case HloOpcode::kLt:
    case HloOpcode::kNe:
      return visitor->HandleCompare(this, opcode_, operands_[0], operands_[1]);
    case HloOpcode::kAdd:
      return visitor->HandleAdd(this, operands_[0], operands_[1]);
    case HloOpcode::kDivide:
      return visitor->HandleDivide(this, operands_[0], operands_[1]);
    case HloOpcode::kSubtract:
      return visitor->HandleSubtract(this, operands_[0], operands_[1]);
    case HloOpcode::kMaximum:
      return visitor->HandleMaximum(this, operands_[0], operands_[1]);
    case HloOpcode::kMinimum:
      return visitor->HandleMinimum(this, operands_[0], operands_[1]);
    case HloOpcode::kLogicalAnd:
      return visitor->HandleLogicalAnd(this, operands_[0], operands_[1]);
    case HloOpcode::kLogicalOr:
      return visitor->HandleLogicalOr(this, operands_[0], operands_[1]);
    case HloOpcode::kConcatenate:
      return visitor->HandleConcatenate(this, operands_);
    case HloOpcode::kConvert:
      return visitor->HandleConvert(this, operands_[0]);
    case HloOpcode::kCopy:
      return visitor->HandleCopy(this, operands_[0]);
    case HloOpcode::kMultiply:
      return visitor->HandleMultiply(this, operands_[0], operands_[1]);
    case HloOpcode::kDot:
      return visitor->HandleDot(this, operands_[0], operands_[1]);
    case HloOpcode::kPower:
      return visitor->HandlePower(this, operands_[0], operands_[1]);
    case HloOpcode::kRemainder:
      return visitor->HandleRemainder(this, operands_[0], operands_[1]);
    case HloOpcode::kSelect:
      return visitor->HandleSelect(this, operands_[0], operands_[1],
                                   operands_[2]);
    case HloOpcode::kConvolution:
      return visitor->HandleConvolution(this, operands_[0], operands_[1],
                                        window());
    case HloOpcode::kCrossReplicaSum:
      return visitor->HandleCrossReplicaSum(this);
    case HloOpcode::kTuple:
      return visitor->HandleTuple(this, operands_);
    case HloOpcode::kMap:
      return visitor->HandleMap(this, operands_, to_apply(), {});
    case HloOpcode::kClamp:
      return visitor->HandleClamp(this, operands_[0], operands_[1],
                                  operands_[2]);
    case HloOpcode::kReduce:
      return visitor->HandleReduce(this, operands_[0], operands_[1],
                                   dimensions_, to_apply());
    case HloOpcode::kReduceWindow:
      return visitor->HandleReduceWindow(this, operands_[0], window(),
                                         to_apply());
    case HloOpcode::kSelectAndScatter:
      return visitor->HandleSelectAndScatter(this);
    case HloOpcode::kNegate:
      return visitor->HandleNegate(this, operands_[0]);
    case HloOpcode::kExp:
      return visitor->HandleExp(this, operands_[0]);
    case HloOpcode::kFloor:
      return visitor->HandleFloor(this, operands_[0]);
    case HloOpcode::kCeil:
      return visitor->HandleCeil(this, operands_[0]);
    case HloOpcode::kLog:
      return visitor->HandleLog(this, operands_[0]);
    case HloOpcode::kTanh:
      return visitor->HandleTanh(this, operands_[0]);
    case HloOpcode::kIsFinite:
      return visitor->HandleIsFinite(this, operands_[0]);
    case HloOpcode::kLogicalNot:
      return visitor->HandleLogicalNot(this, operands_[0]);
    case HloOpcode::kBitcast:
      return visitor->HandleBitcast(this);
    case HloOpcode::kBroadcast:
      return visitor->HandleBroadcast(this);
    case HloOpcode::kPad:
      return visitor->HandlePad(this);
    case HloOpcode::kReshape:
      return visitor->HandleReshape(this);
    case HloOpcode::kTranspose:
      return visitor->HandleTranspose(this);
    case HloOpcode::kReverse:
      return visitor->HandleReverse(this, operands_[0]);
    case HloOpcode::kSlice:
      return visitor->HandleSlice(this, operands_[0]);
    case HloOpcode::kDynamicSlice:
      return visitor->HandleDynamicSlice(this, operands_[0], operands_[1]);
    case HloOpcode::kDynamicUpdateSlice:
      return visitor->HandleDynamicUpdateSlice(this, operands_[0], operands_[1],
                                               operands_[2]);
    case HloOpcode::kSort:
      return visitor->HandleSort(this, operands_[0]);
    case HloOpcode::kInfeed:
      return visitor->HandleInfeed(this);
    case HloOpcode::kOutfeed:
      return visitor->HandleOutfeed(this);
    case HloOpcode::kRng:
      return visitor->HandleRng(this, distribution_);
    case HloOpcode::kWhile:
      return visitor->HandleWhile(this);
    case HloOpcode::kFusion:
      return visitor->HandleFusion(this);
    case HloOpcode::kCall:
      return visitor->HandleCall(this);
    case HloOpcode::kCustomCall:
      return visitor->HandleCustomCall(this, operands_, custom_call_target_);
    case HloOpcode::kSend:
      return visitor->HandleSend(this);
    case HloOpcode::kRecv:
      return visitor->HandleRecv(this);

    // These opcodes are not handled here.
    case HloOpcode::kIndex:
    case HloOpcode::kTrace:
    case HloOpcode::kUpdate:
      break;
  }
  return Unimplemented("unhandled HloOpcode for DfsHloVisitor: %s",
                       HloOpcodeString(opcode_).c_str());
}

Status HloInstruction::AcceptInternal(DfsHloVisitor* visitor,
                                      const CompareFunction* operand_order,
                                      bool ignore_control_predecessors) {
  // Do not visit this HLO node again if it is already visited.
  if (visitor->DidVisit(*this)) {
    VLOG(3) << "Not visiting HLO " << name() << " as it was already visited.";
    return Status::OK();
  }

  // If the instruction is in the visiting state, it means a cycle.
  if (visitor->IsVisiting(*this)) {
    return FailedPrecondition(
        "A cycle is detected while visiting instruction %s",
        ToString().c_str());
  }
  visitor->SetVisiting(*this);

  // Sort operands, if an ordering was provided. 'temp_sorted_operands' must
  // live at this scope, since 'operands' will point to it if the operands are
  // sorted.  The purpose of the 'operands' pointer is to avoid copying the
  // operands in the common case where the operands are not sorted.
  std::vector<HloInstruction*>* operands = &operands_;
  std::vector<HloInstruction*> temp_sorted_operands;
  if (operand_order != nullptr) {
    temp_sorted_operands = operands_;
    std::sort(temp_sorted_operands.begin(), temp_sorted_operands.end(),
              *operand_order);
    operands = &temp_sorted_operands;
  }
  for (HloInstruction* operand : *operands) {
    VLOG(3) << "Going to visit HLO " << operand->name() << " as operand of HLO "
            << name();
    TF_RETURN_IF_ERROR(operand->AcceptInternal(visitor, operand_order,
                                               ignore_control_predecessors));
  }

  if (!ignore_control_predecessors) {
    // This uses the same pointer/vector sorting to avoid extra copies as above.
    std::vector<HloInstruction*>* predecessors = &control_predecessors_;
    std::vector<HloInstruction*> temp_sorted_predecessors;
    if (operand_order != nullptr) {
      temp_sorted_predecessors = control_predecessors_;
      std::sort(temp_sorted_predecessors.begin(),
                temp_sorted_predecessors.end(), *operand_order);
      predecessors = &temp_sorted_predecessors;
    }
    for (HloInstruction* control_predecessor : *predecessors) {
      VLOG(3) << "Going to visit HLO " << control_predecessor->name()
              << " as a control predecessor of HLO " << name();
      TF_RETURN_IF_ERROR(control_predecessor->AcceptInternal(
          visitor, operand_order, ignore_control_predecessors));
    }
  }

  TF_RETURN_IF_ERROR(visitor->Preprocess(this));
  VLOG(2) << "Visiting HLO " << name();
  TF_RETURN_IF_ERROR(Visit(visitor));
  visitor->SetVisited(*this);
  return visitor->Postprocess(this);
}

Status HloInstruction::Accept(DfsHloVisitor* visitor, bool call_finish_visit,
                              bool ignore_control_predecessors) {
  VLOG(2) << "HloInstruction::Accept(" << name() << ")";
  TF_RETURN_IF_ERROR(
      AcceptInternal(visitor, nullptr, ignore_control_predecessors));
  if (call_finish_visit) {
    TF_RETURN_IF_ERROR(visitor->FinishVisit(this));
  }
  return Status::OK();
}

Status HloInstruction::AcceptWithOperandOrder(
    DfsHloVisitor* visitor, const CompareFunction& operand_order,
    bool call_finish_visit) {
  VLOG(2) << "HloInstruction::AcceptWithOperandOrder(" << name() << ")";
  TF_RETURN_IF_ERROR(AcceptInternal(visitor, &operand_order,
                                    /*ignore_control_predecessors=*/false));
  if (call_finish_visit) {
    TF_RETURN_IF_ERROR(visitor->FinishVisit(this));
  }
  return Status::OK();
}

namespace {

// Returns true if the given order is a topological sort of the instructions it
// contains.
bool OrderIsTopologicalSort(const std::vector<const HloInstruction*>& order) {
  // Create a map from instruction to its position in 'order'.
  std::unordered_map<const HloInstruction*, int> order_position;
  for (int i = 0; i < order.size(); i++) {
    if (!order_position.insert({order[i], i}).second) {
      // Instruction order[i] is duplicated in the order.
      return false;
    }
  }
  // Verify that the operand of each instruction in the order is also in the
  // order *and* the operand's position is earlier (defs are before uses for all
  // ops).
  for (auto* instruction : order) {
    for (auto* operand : instruction->operands()) {
      if (!ContainsKey(order_position, operand) ||
          order_position.at(operand) >= order_position.at(instruction)) {
        return false;
      }
    }
  }

  return true;
}

}  // namespace

Status HloInstruction::Accept(
    const FunctionVisitor::VisitorFunction& visitor_func) {
  FunctionVisitor visitor(visitor_func);
  return this->Accept(&visitor);
}

Status HloInstruction::AcceptOrdered(
    DfsHloVisitor* visitor, const std::vector<const HloInstruction*>& order) {
  VLOG(2) << "HloInstruction::AcceptOrdered(" << name() << ")";
  TF_RET_CHECK(OrderIsTopologicalSort(order));

  // Compute the predecessors of this instruction.
  std::unordered_set<const HloInstruction*> predecessors;
  TF_RETURN_IF_ERROR(this->Accept([&predecessors](HloInstruction* instruction) {
    predecessors.insert(instruction);
    return Status::OK();
  }));

  for (auto* const_instruction : order) {
    if (!ContainsKey(predecessors, const_instruction)) {
      // Instruction is not a predecessors of 'this'.
      continue;
    }

    // The visitor can mark instructions as visited to skip particular
    // instructions.
    if (visitor->DidVisit(*const_instruction)) {
      VLOG(3) << "Not visiting HLO " << const_instruction->name()
              << " as it was already visited.";
      continue;
    }

    HloInstruction* instruction =
        const_cast<HloInstruction*>(const_instruction);

    TF_RETURN_IF_ERROR(visitor->Preprocess(instruction));
    VLOG(2) << "Visiting HLO " << instruction->name();
    TF_RETURN_IF_ERROR(instruction->Visit(visitor));
    visitor->SetVisited(*instruction);
    TF_RETURN_IF_ERROR(visitor->Postprocess(instruction));
  }

  return visitor->FinishVisit(this);
}

const Shape& HloInstruction::outfeed_shape() const {
  DCHECK_EQ(opcode_, HloOpcode::kOutfeed);
  TF_DCHECK_OK(ShapeUtil::ValidateShapeWithOptionalLayout(shape_));
  return outfeed_shape_;
}

const Shape& HloInstruction::shape() const {
  TF_DCHECK_OK(ShapeUtil::ValidateShapeWithOptionalLayout(shape_));
  return shape_;
}

std::vector<int64> HloInstruction::OperandIndices(
    const HloInstruction* operand) const {
  std::vector<int64> result;
  for (int64 i = 0; i < operand_count(); ++i) {
    if (this->operand(i) == operand) {
      result.push_back(i);
    }
  }
  return result;
}

bool HloInstruction::IsElementwise() const {
  switch (opcode_) {
    // Nullary elementwise operations.
    case HloOpcode::kConstant:
      return true;

    // Unary elementwise operations.
    case HloOpcode::kAbs:
    case HloOpcode::kCeil:
    case HloOpcode::kConvert:
    case HloOpcode::kCopy:
    case HloOpcode::kExp:
    case HloOpcode::kFloor:
    case HloOpcode::kIsFinite:
    case HloOpcode::kLog:
    case HloOpcode::kLogicalNot:
    case HloOpcode::kNegate:
    case HloOpcode::kSign:
    case HloOpcode::kTanh:
      return true;

    // Binary elementwise operations.
    case HloOpcode::kAdd:
    case HloOpcode::kDivide:
    case HloOpcode::kEq:
    case HloOpcode::kGe:
    case HloOpcode::kGt:
    case HloOpcode::kLe:
    case HloOpcode::kLt:
    case HloOpcode::kMaximum:
    case HloOpcode::kMinimum:
    case HloOpcode::kMultiply:
    case HloOpcode::kNe:
    case HloOpcode::kPower:
    case HloOpcode::kRemainder:
    case HloOpcode::kSubtract:
    case HloOpcode::kLogicalAnd:
    case HloOpcode::kLogicalOr:
      return true;

    // Ternary elementwise operations.
    case HloOpcode::kSelect:
      return !ShapeUtil::IsTuple(shape_);
    case HloOpcode::kClamp:
      return true;

    // Other operations.
    case HloOpcode::kRng:
    case HloOpcode::kMap:
      return true;
    case HloOpcode::kFusion:
      if (fusion_kind() != FusionKind::kLoop) {
        return false;
      }
      for (auto& fused : fused_instructions()) {
        if (fused->opcode() != HloOpcode::kParameter &&
            !fused->IsElementwise()) {
          return false;
        }
      }
      return true;

    default:
      return false;
  }
}

namespace {
bool IsInstructionElementwiseOnOperand(const HloInstruction* instruction,
                                       const HloInstruction* operand) {
  std::vector<int64> operand_indices = instruction->OperandIndices(operand);
  return std::all_of(
      operand_indices.begin(), operand_indices.end(),
      [instruction](int64 operand_index) {
        return instruction->IsElementwiseOnOperand(operand_index);
      });
}
}  // namespace

bool HloInstruction::IsElementwiseOnOperand(int64 operand_idx) const {
  // For all instructions other than kFusion, being elementwise on one of the
  // operands is equivalent to being elementwise on all the operands.
  if (opcode() != HloOpcode::kFusion) {
    return IsElementwise();
  }

  CHECK_EQ(HloOpcode::kFusion, opcode());
  if (fusion_kind() != FusionKind::kLoop) {
    return false;
  }

  // A loop-fusion is elementwise on an operand if all operations (computed
  // using BFS) between the operand and the fused root are elementwise.
  std::deque<HloInstruction*> worklist;
  std::unordered_set<const HloInstruction*> visited;
  worklist.push_back(fused_parameter(operand_idx));
  visited.insert(fused_parameter(operand_idx));
  while (!worklist.empty()) {
    HloInstruction* operand = worklist.front();
    worklist.pop_front();
    for (HloInstruction* user : operand->users()) {
      if (ContainsKey(visited, user)) {
        continue;
      }
      if (user->IsElementwise() ||
          IsInstructionElementwiseOnOperand(user, operand)) {
        worklist.push_back(user);
        visited.insert(user);
      } else {
        return false;
      }
    }
  }
  return true;
}

HloInstruction::UseKind HloInstruction::OperandElementUse(int64 i) const {
  switch (opcode_) {
    case HloOpcode::kBitcast:
    case HloOpcode::kConcatenate:
    case HloOpcode::kReshape:
    case HloOpcode::kReverse:
    case HloOpcode::kSlice:
    case HloOpcode::kTranspose:
      return UseKind::kUsePermutingElements;
    case HloOpcode::kPad:
    case HloOpcode::kReduce:
      // Pad reuses the padding value but not the padded array elements.
      // Reduce reuses the init value but not the operand array elements.
      return i > 0 ? UseKind::kReuse : UseKind::kUsePermutingElements;
    case HloOpcode::kFusion: {
      tensorflow::gtl::FlatMap<const HloInstruction*, UseKind> cache;
      // We could rather iterate backwards thru fused_instructions_ here, as it
      // is in reverse postorder, and compute whether each fused instruction
      // reuses the value of this parameter, which would save stack space but
      // not allow us to finish early if we find a reuse.
      std::function<UseKind(const HloInstruction&)> reuses_parameter_elements =
          [i, &cache, &reuses_parameter_elements](const HloInstruction& hlo) {
            auto plus = [](const UseKind& a, const UseKind& b) {
              if (a == UseKind::kNoUse) return b;
              if (b == UseKind::kNoUse) return a;
              if (a == UseKind::kReuse || b == UseKind::kReuse) {
                return UseKind::kReuse;
              }
              if (a == UseKind::kUsePermutingElements ||
                  b == UseKind::kUsePermutingElements) {
                return UseKind::kReuse;
              }
              CHECK(UseKind::kUse == a && UseKind::kUse == b);
              return UseKind::kUse;
            };

            if (hlo.opcode_ == HloOpcode::kParameter &&
                hlo.parameter_number_ == i) {
              return UseKind::kUse;
            }
            if (!ContainsKey(cache, &hlo)) {
              for (int64 j = 0; j < hlo.operands_.size(); ++j) {
                UseKind old = cache[&hlo];
                UseKind updated = plus(
                    old, std::min(hlo.OperandElementUse(j),
                                  reuses_parameter_elements(*hlo.operand(j))));
                cache[&hlo] = updated;
              }
            }
            return cache[&hlo];
          };
      return reuses_parameter_elements(*fused_expression_root());
    }
    default:
      return IsElementwise() ? UseKind::kUse : UseKind::kReuse;
  }
}

std::tuple<bool, std::vector<int64>, std::vector<int64>>
HloInstruction::ReshapeMerelyInsertsOrDeletes1SizedDimensions() const {
  if (HloOpcode::kReshape != opcode_) {
    return std::make_tuple(false, std::vector<int64>(), std::vector<int64>());
  }
  return ShapeUtil::InsertedOrDeleted1SizedDimensions(operand(0)->shape_,
                                                      shape_);
}

string ToString(HloInstruction::FusionKind kind) {
  switch (kind) {
    case HloInstruction::FusionKind::kLoop:
      return "kLoop";
    case HloInstruction::FusionKind::kInput:
      return "kInput";
    case HloInstruction::FusionKind::kOutput:
      return "kOutput";
    case HloInstruction::FusionKind::kTransposeDot:
      return "kTransposeDot";
    case HloInstruction::FusionKind::kConvBackwardFilter:
      return "kConvBackwardFilter";
    case HloInstruction::FusionKind::kConvBackwardInput:
      return "kConvBackwardInput";
  }
}

std::ostream& operator<<(std::ostream& os, HloInstruction::FusionKind kind) {
  return os << ToString(kind);
}

string HloInstruction::ConvolutionDimensionNumbersToString() const {
  string result;
  if (convolution_dimension_numbers_ == nullptr) {
    return result;
  }
  const ConvolutionDimensionNumbers& dnums = *convolution_dimension_numbers_;
  // Show the given dimension labels in order of major to minor based on the
  // shape's layout.
  const auto append_dims = [&](const std::vector<string>& dims,
                               const Shape& shape) {
    CHECK_EQ(dims.size(), ShapeUtil::Rank(shape));
    for (int64 logical = 0; logical < dims.size(); ++logical) {
      int64 physical = logical;
      if (!shape.layout().minor_to_major().empty()) {
        physical = LayoutUtil::Major(shape.layout(), logical);
      }
      result += dims[physical];
    }
  };

  // lhs_dims[i] is the symbol of the logical dimension i for the lhs
  // operand. E.g. if batch has dimension number 2, then lhs_dims[2] == "b".
  std::vector<string> lhs_dims(2 + dnums.spatial_dimensions().size());
  lhs_dims[dnums.batch_dimension()] = 'b';
  lhs_dims[dnums.feature_dimension()] = 'f';
  for (int64 i = 0; i < dnums.spatial_dimensions().size(); ++i) {
    lhs_dims[dnums.spatial_dimensions(i)] = StrCat(i);
  }

  std::vector<string> rhs_dims(2 + dnums.kernel_spatial_dimensions().size());
  rhs_dims[dnums.kernel_input_feature_dimension()] = "i";
  rhs_dims[dnums.kernel_output_feature_dimension()] = "o";
  for (int64 i = 0; i < dnums.spatial_dimensions().size(); ++i) {
    rhs_dims[dnums.kernel_spatial_dimensions(i)] = StrCat(i);
  }

  result += "dim_labels=";
  append_dims(lhs_dims, operand(0)->shape());
  result += "_";
  append_dims(rhs_dims, operand(1)->shape());
  result += "->";
  append_dims(lhs_dims, shape());
  return result;
}

bool HloInstruction::CouldBeBitcast() const {
  switch (opcode_) {
    case HloOpcode::kTranspose:
      return true;
    case HloOpcode::kReshape:
      return std::get<0>(ReshapeMerelyInsertsOrDeletes1SizedDimensions());
    default:
      return false;
  }
}

HloModule* HloInstruction::GetModule() const {
  if (parent_) {
    return parent_->parent();
  }
  return nullptr;
}

void HloInstruction::UniquifyName(NameUniquer* name_uniquer) {
  name_ = name_uniquer->GetUniqueName(name_);
}

}  // namespace xla
