/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__ice_transport_h //original-code:"pc/ice_transport.h"

#include <memory>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_api__ice_transport_factory_h //original-code:"api/ice_transport_factory.h"
#include BOSS_WEBRTC_U_p2p__base__fake_ice_transport_h //original-code:"p2p/base/fake_ice_transport.h"
#include BOSS_WEBRTC_U_p2p__base__fake_port_allocator_h //original-code:"p2p/base/fake_port_allocator.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

class IceTransportTest : public ::testing::Test {};

TEST_F(IceTransportTest, CreateNonSelfDeletingTransport) {
  auto cricket_transport =
      std::make_unique<cricket::FakeIceTransport>("name", 0, nullptr);
  auto ice_transport =
      rtc::make_ref_counted<IceTransportWithPointer>(cricket_transport.get());
  EXPECT_EQ(ice_transport->internal(), cricket_transport.get());
  ice_transport->Clear();
  EXPECT_NE(ice_transport->internal(), cricket_transport.get());
}

TEST_F(IceTransportTest, CreateSelfDeletingTransport) {
  std::unique_ptr<cricket::FakePortAllocator> port_allocator(
      std::make_unique<cricket::FakePortAllocator>(nullptr, nullptr));
  IceTransportInit init;
  init.set_port_allocator(port_allocator.get());
  auto ice_transport = CreateIceTransport(std::move(init));
  EXPECT_NE(nullptr, ice_transport->internal());
}

}  // namespace webrtc
