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

#ifndef TENSORFLOW_COMPILER_XLA_CLIENT_LOCAL_CLIENT_H_
#define TENSORFLOW_COMPILER_XLA_CLIENT_LOCAL_CLIENT_H_

#include <memory>

#include "tensorflow/compiler/xla/client/client.h"
#include "tensorflow/compiler/xla/client/computation.h"
#include "tensorflow/compiler/xla/executable_run_options.h"
#include "tensorflow/compiler/xla/service/compiler.h"
#include "tensorflow/compiler/xla/service/device_memory_allocator.h"
#include "tensorflow/compiler/xla/service/executable.h"
#include "tensorflow/compiler/xla/service/local_service.h"
#include "tensorflow/compiler/xla/service/shaped_buffer.h"
#include "tensorflow/compiler/xla/statusor.h"
#include "tensorflow/compiler/xla/xla_data.pb.h"
#include "tensorflow/core/lib/gtl/array_slice.h"
#include "tensorflow/core/platform/stream_executor_no_cuda.h"

namespace xla {

// Class containing options for building an LocalExecutable with
// LocalClient::Compile.
class ExecutableBuildOptions {
 public:
  // If set, this is the platform to build the computation for. This must match
  // the underlying platform of the service. A value of nullptr indicates the
  // option has not been set.
  //
  // TODO(b/28616830): Support multiple platforms.
  ExecutableBuildOptions& set_platform(perftools::gputools::Platform* platform);
  perftools::gputools::Platform* platform() const;

  // If set, this is the device to build the computation for. Valid
  // device_ordinal values are: 0 to # of devices - 1. These values are
  // identical to the device ordinal values used by StreamExecutor. The built
  // executable will be executable on any device equivalent to the specified
  // device as determined by Backend::devices_equivalent(). A value of -1
  // indicates this option has not been set.
  ExecutableBuildOptions& set_device_ordinal(int device_ordinal);
  int device_ordinal() const;

  // If set, this specifies the layout of the result of the computation. If not
  // set, the service will chose the layout of the result. A Shape is used to
  // store the layout to accomodate tuple result shapes. A value of nullptr
  // indicates the option has not been set.
  ExecutableBuildOptions& set_result_layout(const Shape& shape_with_layout);
  const Shape* result_layout() const;

  // If set, the executable will be built to output a hybrid
  // ShapedBuffer with top-level tuple pointers in host memory and
  // result buffers in device memory.
  ExecutableBuildOptions& set_has_hybrid_result(bool has_hybrid_result);
  bool has_hybrid_result() const;

 private:
  perftools::gputools::Platform* platform_ = nullptr;
  int device_ordinal_ = -1;
  Shape result_layout_;
  bool result_layout_set_ = false;
  bool has_hybrid_result_ = true;
};

class LocalExecutable {
 public:
  // Run the compiled computation with the given arguments and options and
  // return the result.
  StatusOr<std::unique_ptr<ShapedBuffer>> Run(
      const tensorflow::gtl::ArraySlice<const ShapedBuffer*> arguments,
      const ExecutableRunOptions& options);

  // Return the layout (contained in a shape) of the result produced by the
  // computation.
  const Shape& result_layout() const {
    return executable_->module_config()
        .entry_computation_layout()
        .result_layout()
        .shape();
  }

  // Return the options used to build the executable.
  const ExecutableBuildOptions& build_options() const { return build_options_; }

  // Return the built executable.
  Executable* executable() const { return executable_.get(); }

 private:
  // Only a local client can construct these objects.
  friend class LocalClient;

  // Constructor invoked by LocalClient.
  LocalExecutable(std::unique_ptr<Executable> executable, Backend* backend,
                  int device_ordinal,
                  const ExecutableBuildOptions& build_options);

  // Validates that the given arguments and options satisfy various constraints
  // of the computation.
  tensorflow::Status ValidateExecutionOptions(
      const tensorflow::gtl::ArraySlice<const ShapedBuffer*> arguments,
      const ExecutableRunOptions& options, const Backend& backend);

  // Records the computation in a SessionModule proto with the arguments used to
  // invoke it, and the result. Enabled by flag: --tla_dump_executions_to.
  StatusOr<std::unique_ptr<ShapedBuffer>> ExecuteAndDump(
      const ServiceExecutableRunOptions* run_options,
      const tensorflow::gtl::ArraySlice<const ShapedBuffer*> arguments);

  // Records the arguments used to invoke the computation in a SessionModule
  // proto.
  tensorflow::Status RecordArguments(
      const tensorflow::gtl::ArraySlice<const ShapedBuffer*> arguments,
      SessionModule* session_module);

  // Records the result of the computation in a SessionModule proto.
  tensorflow::Status RecordResult(const ShapedBuffer* result,
                                  SessionModule* session_module);

  // Copies the contents of a ShapedBuffer into a Literal proto.
  tensorflow::Status LiteralFromShapedBuffer(const ShapedBuffer& shaped_buffer,
                                             Literal* literal);

  // Compiled computation.
  std::unique_ptr<Executable> executable_;

  // Execution backend.
  Backend* backend_;

  // The ordinal of the device which this executable was compiled for. The
  // executable can run on all equivalent devices (as determined by
  // Backend::devices_equivalent).
  int build_device_ordinal_;

  // Options used to build the executable.
  const ExecutableBuildOptions& build_options_;
};

// An XLA Client specialization for use when the client and service run in
// the same process.
class LocalClient : public Client {
 public:
  explicit LocalClient(LocalService* service)
      : Client(service), local_service_(service) {}

  LocalClient(const LocalClient&) = delete;
  void operator=(const LocalClient&) = delete;

  // Return a handle to a buffer large enough to hold shape, allocated
  // on device_ordinal on the local service. If
  // allocate_space_for_deep_copy, the buffer is large enough to hold
  // all sub-buffers of a tuple shape, otherwise it is only as large
  // as the top-level tuple pointer array.
  StatusOr<std::unique_ptr<GlobalData>> AllocateBufferOnDevice(
      const Shape& shape, int device_ordinal,
      bool allocate_space_for_deep_copy);

  // Build and return a LocalExecutable object. The executable is compiled using
  // the given argument layouts and options.
  StatusOr<std::unique_ptr<LocalExecutable>> Compile(
      const Computation& computation,
      const tensorflow::gtl::ArraySlice<const Shape*> argument_layouts,
      const ExecutableBuildOptions& options);

  // Returns the platform that the underlying service targets.
  perftools::gputools::Platform* platform() const;

  // Returns the number of devices on the system of the service platform
  // type. Not all devices may be supported by the service (see
  // device_ordinal_supported method).
  int device_count() const;

  // Returns the default device ordinal that the service will run computations
  // on if no device ordinal is specified in execute options.
  int default_device_ordinal() const;

  // Returns whether the device with the given ordinal can be used by the
  // service to execute computations. Not all devices of a particular platform
  // may be usable by the service (eg, a GPU with insufficient CUDA compute
  // capability).
  bool device_ordinal_supported(int device_ordinal) const;

  // Returns the backend used to execute computations.
  const Backend& backend() const;
  Backend* mutable_backend();

 private:
  LocalService* local_service_;
};

}  // namespace xla

#endif  // TENSORFLOW_COMPILER_XLA_CLIENT_LOCAL_CLIENT_H_
