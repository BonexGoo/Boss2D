/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_ADAPTATION_RESOURCE_ADAPTATION_PROCESSOR_INTERFACE_H_
#define CALL_ADAPTATION_RESOURCE_ADAPTATION_PROCESSOR_INTERFACE_H_

#include <map>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__adaptation__resource_h //original-code:"api/adaptation/resource.h"
#include BOSS_WEBRTC_U_api__rtp_parameters_h //original-code:"api/rtp_parameters.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"
#include BOSS_WEBRTC_U_api__video__video_adaptation_counters_h //original-code:"api/video/video_adaptation_counters.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_call__adaptation__adaptation_constraint_h //original-code:"call/adaptation/adaptation_constraint.h"
#include BOSS_WEBRTC_U_call__adaptation__encoder_settings_h //original-code:"call/adaptation/encoder_settings.h"
#include BOSS_WEBRTC_U_call__adaptation__video_source_restrictions_h //original-code:"call/adaptation/video_source_restrictions.h"

namespace webrtc {

class ResourceLimitationsListener {
 public:
  virtual ~ResourceLimitationsListener();

  // The limitations on a resource were changed. This does not mean the current
  // video restrictions have changed.
  virtual void OnResourceLimitationChanged(
      rtc::scoped_refptr<Resource> resource,
      const std::map<rtc::scoped_refptr<Resource>, VideoAdaptationCounters>&
          resource_limitations) = 0;
};

// The Resource Adaptation Processor is responsible for reacting to resource
// usage measurements (e.g. overusing or underusing CPU). When a resource is
// overused the Processor is responsible for performing mitigations in order to
// consume less resources.
class ResourceAdaptationProcessorInterface {
 public:
  virtual ~ResourceAdaptationProcessorInterface();

  virtual void SetTaskQueue(TaskQueueBase* task_queue) = 0;

  virtual void AddResourceLimitationsListener(
      ResourceLimitationsListener* limitations_listener) = 0;
  virtual void RemoveResourceLimitationsListener(
      ResourceLimitationsListener* limitations_listener) = 0;
  // Starts or stops listening to resources, effectively enabling or disabling
  // processing. May be called from anywhere.
  // TODO(https://crbug.com/webrtc/11172): Automatically register and unregister
  // with AddResource() and RemoveResource() instead. When the processor is
  // multi-stream aware, stream-specific resouces will get added and removed
  // over time.
  virtual void AddResource(rtc::scoped_refptr<Resource> resource) = 0;
  virtual std::vector<rtc::scoped_refptr<Resource>> GetResources() const = 0;
  virtual void RemoveResource(rtc::scoped_refptr<Resource> resource) = 0;
};

}  // namespace webrtc

#endif  // CALL_ADAPTATION_RESOURCE_ADAPTATION_PROCESSOR_INTERFACE_H_
