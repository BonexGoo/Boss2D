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

// Native XLA implementations of XLA Elu Ops

#include "tensorflow/compiler/tf2xla/kernels/cwise_ops.h"
#include "tensorflow/compiler/tf2xla/xla_helpers.h"
#include "tensorflow/compiler/tf2xla/xla_op_registry.h"
#include "tensorflow/compiler/xla/client/computation_builder.h"
#include "tensorflow/compiler/xla/literal_util.h"
#include "tensorflow/core/framework/kernel_def_builder.h"
#include "tensorflow/core/framework/types.h"
#include "tensorflow/core/kernels/no_op.h"

namespace tensorflow {
namespace {

class EluOp : public XlaOpKernel {
 public:
  explicit EluOp(OpKernelConstruction* ctx) : XlaOpKernel(ctx) {}
  // Computes the max of the scalar input x and 0.
  void Compile(XlaOpKernelContext* ctx) override {
    xla::ComputationBuilder* b = ctx->builder();
    const auto zero = XlaHelpers::Zero(b, input_type(0));
    const auto one = XlaHelpers::One(b, input_type(0));
    const auto pred = b->Gt(ctx->Input(0), zero);
    const auto expm1 = b->Sub(b->Exp(ctx->Input(0)), one);
    ctx->SetOutput(0, b->Select(pred, ctx->Input(0), expm1));
  }
};

class EluGradOp : public XlaOpKernel {
 public:
  explicit EluGradOp(OpKernelConstruction* ctx) : XlaOpKernel(ctx) {}
  // Return the lhs (incoming gradient) if the rhs (input feature) > 0,
  // otherwise return lhs * (1 + rhs).
  void Compile(XlaOpKernelContext* ctx) override {
    xla::ComputationBuilder* b = ctx->builder();
    const auto zero = XlaHelpers::Zero(b, input_type(0));
    const auto one = XlaHelpers::One(b, input_type(0));
    const auto grad = ctx->Input(0);
    const auto activation = ctx->Input(1);
    const auto exp_grad = b->Mul(grad, b->Add(activation, one));
    const auto pred = b->Gt(activation, zero);
    ctx->SetOutput(0, b->Select(pred, grad, exp_grad));
  }
};

REGISTER_XLA_OP(Name("Elu"), EluOp);
REGISTER_XLA_OP(Name("EluGrad"), EluGradOp);

}  // namespace
}  // namespace tensorflow
