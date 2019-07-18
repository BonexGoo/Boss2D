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

#include <utility>

#include "tensorflow/c/checkpoint_reader.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/lib/hash/hash.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/test.h"
#include "tensorflow/core/protobuf/config.pb.h"
#include "tensorflow/tools/tfprof/internal/tfprof_constants.h"
#include "tensorflow/tools/tfprof/internal/tfprof_options.h"
#include "tensorflow/tools/tfprof/internal/tfprof_utils.h"
#include "tensorflow/tools/tfprof/tfprof_log.pb.h"
#include "tensorflow/tools/tfprof/tfprof_output.pb.h"

namespace tensorflow {
namespace tfprof {
class TFProfTimelineTest : public ::testing::Test {
 protected:
  TFProfTimelineTest() {
    string graph_path =
        io::JoinPath(testing::TensorFlowSrcRoot(),
                     "tools/tfprof/internal/testdata/graph.pbtxt");
    std::unique_ptr<tensorflow::GraphDef> graph_pb(new tensorflow::GraphDef());
    TF_CHECK_OK(ReadGraphDef(Env::Default(), graph_path, graph_pb.get()));

    std::unique_ptr<tensorflow::RunMetadata> run_meta_pb(
        new tensorflow::RunMetadata());
    string run_meta_path =
        io::JoinPath(testing::TensorFlowSrcRoot(),
                     "tools/tfprof/internal/testdata/run_meta");
    TF_CHECK_OK(
        ReadBinaryProto(Env::Default(), run_meta_path, run_meta_pb.get()));

    tf_stats_.reset(new TFStats(std::move(graph_pb), std::move(run_meta_pb),
                                nullptr, nullptr));
  }

  std::unique_ptr<TFStats> tf_stats_;
};

// Before adding test, first dump the json file and
// manually check it's correct
TEST_F(TFProfTimelineTest, GraphView) {
  string dump_file = io::JoinPath(testing::TmpDir(), "dump");
  Options opts(10000, 0, 0, 0, 0, {".*"}, "name",
               {".*"},  // accout_type_regexes
               {".*"}, {""}, {".*"}, {""}, false,
               {"params", "bytes", "micros", "float_ops", "num_hidden_ops"},
               "timeline", {{"outfile", dump_file}});
  tf_stats_->PrintGraph("graph", opts);

  string dump_str;
  TF_CHECK_OK(ReadFileToString(Env::Default(), dump_file, &dump_str));
  EXPECT_EQ(14171250174278825648ull, Hash64(dump_str));
}

TEST_F(TFProfTimelineTest, ScopeView) {
  string dump_file = io::JoinPath(testing::TmpDir(), "dump");
  Options opts(5, 0, 0, 0, 0, {".*"}, "name", {".*"},  // accout_type_regexes
               {".*"}, {""}, {".*"}, {""}, false,
               {"params", "bytes", "micros", "float_ops", "num_hidden_ops"},
               "timeline", {{"outfile", dump_file}});
  tf_stats_->PrintGraph("scope", opts);

  string dump_str;
  TF_CHECK_OK(ReadFileToString(Env::Default(), dump_file, &dump_str));
  EXPECT_EQ(2355241164346147404ull, Hash64(dump_str));
}

// TODO(xpan): tfprof_log is too large to include in testdata when adding
// code traces.

}  // namespace tfprof
}  // namespace tensorflow
