/*
 *  Copyright 2019 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_ADAPTATION_TEST_FAKE_RESOURCE_H_
#define CALL_ADAPTATION_TEST_FAKE_RESOURCE_H_

#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__adaptation__resource_h //original-code:"api/adaptation/resource.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"

namespace webrtc {

// Fake resource used for testing.
class FakeResource : public Resource {
 public:
  static rtc::scoped_refptr<FakeResource> Create(std::string name);

  explicit FakeResource(std::string name);
  ~FakeResource() override;

  void SetUsageState(ResourceUsageState usage_state);

  // Resource implementation.
  std::string Name() const override;
  void SetResourceListener(ResourceListener* listener) override;

 private:
  const std::string name_;
  ResourceListener* listener_;
};

}  // namespace webrtc

#endif  // CALL_ADAPTATION_TEST_FAKE_RESOURCE_H_
