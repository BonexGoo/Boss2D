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

#include "tensorflow/tools/tfprof/internal/tfprof_stats.h"

#include <stdio.h>
#include <utility>

#include "tensorflow/core/framework/step_stats.pb.h"
#include "tensorflow/core/lib/strings/str_util.h"
#include "tensorflow/tools/tfprof/internal/tfprof_timeline.h"

namespace tensorflow {
namespace tfprof {
TFStats::TFStats(std::unique_ptr<GraphDef> graph,
                 std::unique_ptr<RunMetadata> run_meta,
                 std::unique_ptr<OpLog> op_log,
                 std::unique_ptr<checkpoint::CheckpointReader> ckpt_reader)
    : graph_(std::move(graph)),
      run_meta_(std::move(run_meta)),
      op_log_(std::move(op_log)),
      ckpt_reader_(std::move(ckpt_reader)) {
  CHECK(graph_) << "Must at least have GraphDef";

  printf("Parsing GraphDef...\n");
  ParseGraph();
  if (run_meta_) {
    printf("Parsing RunMetadata...\n");
    ParseRunMeta();
  }
  if (op_log_) {
    printf("Parsing OpLog...\n");
    ParseOpLog();
  }

  if (ckpt_reader_) {
    printf("Parsing Checkpoint...\n");
    for (const auto& v : ckpt_reader_->GetVariableToShapeMap()) {
      auto node = nodes_map_.find(v.first);
      if (node != nodes_map_.end()) {
        node->second.AddOpType("_checkpoint_variables");
      }
    }
  }

  printf("Preparing Views...\n");
  scope_view_ = std::unique_ptr<TFScope>(new TFScope(ckpt_reader_.get()));
  graph_view_ = std::unique_ptr<TFGraph>(new TFGraph(ckpt_reader_.get()));
  code_view_ = std::unique_ptr<TFCode>(new TFCode());

  for (auto it = nodes_map_.begin(); it != nodes_map_.end(); it++) {
    scope_view_->AddNode(&it->second);
    graph_view_->AddNode(&it->second);
    code_view_->AddNode(&it->second);
  }
  scope_view_->Build();
  graph_view_->Build();
  code_view_->Build();
}

const TFGraphNodeProto& TFStats::PrintGraph(const string& cmd,
                                            const Options& opts) {
  if (cmd == kCmds[0]) {
    return scope_view_->Show(opts);
  } else if (cmd == kCmds[1]) {
    return graph_view_->Show(opts);
  } else {
    fprintf(stderr, "Unknown command: %s\n", cmd.c_str());
    return empty_graph_node_;
  }
}

const TFCodeNodeProto& TFStats::PrintCode(const Options& opts) {
  return code_view_->Show(opts);
}

void TFStats::ParseGraph() {
  for (const NodeDef& node : graph_->node()) {
    CHECK(nodes_map_.find(node.name()) == nodes_map_.end());
    nodes_map_[node.name()] = TFGraphNode(&node);
  }
  for (auto it = nodes_map_.begin(); it != nodes_map_.end(); it++) {
    const NodeDef* node_def = it->second.node_def();
    for (string node_input : node_def->input()) {
      int output_idx = 0;
      // input name format can be: "^node:src_output"
      auto prefix_pos = node_input.find(":");
      if (prefix_pos != node_input.npos) {
        std::vector<string> input_parts = str_util::Split(node_input, ":");
        CHECK(input_parts.size() == 2)
            << "Unknown NodeDef.input format: " << node_input;
        node_input = input_parts[0];
        CHECK(strings::safe_strto32(input_parts[1], &output_idx))
            << "Failed to parse integer: " << output_idx;
      }
      if (node_input.substr(0, 1) == "^") {
        node_input = node_input.substr(1);
      }
      auto input_node = nodes_map_.find(node_input);
      if (input_node == nodes_map_.end()) {
        continue;
      }
      it->second.AddInput(&input_node->second, output_idx);
    }
  }
}

void TFStats::ParseOpLog() {
  for (const OpLogEntry& entry : op_log_->log_entries()) {
    auto node = nodes_map_.find(entry.name());
    if (node == nodes_map_.end()) continue;
    for (const string& type : entry.types()) {
      node->second.AddOpType(type);
    }
    if (entry.float_ops()) {
      node->second.AddFloatOps(entry.float_ops());
    }
    if (entry.has_code_def()) {
      node->second.AddCode(&entry.code_def());
    }
  }
}

void TFStats::ParseRunMeta() {
  if (!run_meta_->has_step_stats()) return;

  for (const auto& dev_stat : run_meta_->step_stats().dev_stats()) {
    for (const auto& node_stat : dev_stat.node_stats()) {
      auto node = nodes_map_.find(node_stat.node_name());
      if (node == nodes_map_.end()) {
        continue;
      }
      node->second.AddStepStat(dev_stat.device(), &node_stat);
    }
  }
}
}  // namespace tfprof
}  // namespace tensorflow
