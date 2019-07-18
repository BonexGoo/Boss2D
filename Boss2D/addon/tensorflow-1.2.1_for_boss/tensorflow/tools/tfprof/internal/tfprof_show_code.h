/* Copyright 2016 The TensorFlow Authors All Rights Reserved.

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

// Parent class and utilities for tfprof_code.

#ifndef THIRD_PARTY_TENSORFLOW_TOOLS_TFPROF_INTERNAL_TFPROF_SHOW_CODE_H_
#define THIRD_PARTY_TENSORFLOW_TOOLS_TFPROF_INTERNAL_TFPROF_SHOW_CODE_H_

#include <algorithm>
#include <string>
#include <vector>

#include "tensorflow/c/checkpoint_reader.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/tools/tfprof/internal/tfprof_constants.h"
#include "tensorflow/tools/tfprof/internal/tfprof_node.h"
#include "tensorflow/tools/tfprof/internal/tfprof_node_show.h"
#include "tensorflow/tools/tfprof/internal/tfprof_options.h"
#include "tensorflow/tools/tfprof/internal/tfprof_tensor.h"
#include "tensorflow/tools/tfprof/internal/tfprof_timeline.h"
#include "tensorflow/tools/tfprof/internal/tfprof_utils.h"
#include "tensorflow/tools/tfprof/tfprof_output.pb.h"

namespace tensorflow {
namespace tfprof {

class TFShowCode {
 public:
  explicit TFShowCode() {}
  virtual ~TFShowCode() {}
  virtual void AddNode(TFGraphNode* node) = 0;
  virtual void Build() = 0;
  const TFCodeNodeProto& Show(const Options& opts);

 protected:
  virtual const ShowCodeNode* ShowInternal(const Options& opts,
                                           Timeline* timeline) = 0;

  bool LookUpCheckPoint(const string& name,
                        std::unique_ptr<TFProfTensor>* tensor);

  // Overridden by subclass if extra requirements need to be met.
  virtual bool ShouldShowIfExtra(ShowCodeNode* node, const Options& opts,
                                 int depth) {
    return true;
  }

  bool ShouldShow(ShowCodeNode* node, const Options& opts, int depth);

  bool ShouldTrim(ShowCodeNode* node, const std::vector<string>& regexes);

  bool ShouldAccount(ShowCodeNode* node, const Options& opts);

  template <typename T>
  std::vector<T*> SortNodes(const std::vector<T*>& nodes, const Options& opts) {
    if (opts.order_by.empty() || nodes.empty()) {
      return nodes;
    }
    std::vector<T*> sorted_nodes = nodes;
    std::sort(sorted_nodes.begin(), sorted_nodes.end(),
              [&opts](const T* n1, const T* n2) {
                if (n1->name() == kTFProfRoot) return true;
                if (n2->name() == kTFProfRoot) return false;
                bool name_cmp = n1->name() < n2->name();
                if (opts.order_by == kOrderBy[0]) {
                  return name_cmp;
                } else if (opts.order_by == kOrderBy[1]) {
                  return n1->proto().total_requested_bytes() >
                         n2->proto().total_requested_bytes();
                } else if (opts.order_by == kOrderBy[2]) {
                  return n1->proto().total_exec_micros() >
                         n2->proto().total_exec_micros();
                } else if (opts.order_by == kOrderBy[3]) {
                  return n1->proto().total_parameters() >
                         n2->proto().total_parameters();
                } else if (opts.order_by == kOrderBy[4]) {
                  return n1->proto().total_float_ops() >
                         n2->proto().total_float_ops();
                }
                return name_cmp;
              });
    return sorted_nodes;
  }
};

}  // namespace tfprof
}  // namespace tensorflow

#endif  // THIRD_PARTY_TENSORFLOW_TOOLS_TFPROF_INTERNAL_TFPROF_SHOW_CODE_H_
