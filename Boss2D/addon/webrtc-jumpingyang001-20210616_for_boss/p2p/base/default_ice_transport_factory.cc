/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_p2p__base__default_ice_transport_factory_h //original-code:"p2p/base/default_ice_transport_factory.h"

#include <utility>

#include BOSS_WEBRTC_U_p2p__base__basic_ice_controller_h //original-code:"p2p/base/basic_ice_controller.h"
#include BOSS_WEBRTC_U_p2p__base__ice_controller_factory_interface_h //original-code:"p2p/base/ice_controller_factory_interface.h"

namespace {

class BasicIceControllerFactory
    : public cricket::IceControllerFactoryInterface {
 public:
  std::unique_ptr<cricket::IceControllerInterface> Create(
      const cricket::IceControllerFactoryArgs& args) override {
    return std::make_unique<cricket::BasicIceController>(args);
  }
};

}  // namespace

namespace webrtc {

DefaultIceTransport::DefaultIceTransport(
    std::unique_ptr<cricket::P2PTransportChannel> internal)
    : internal_(std::move(internal)) {}

DefaultIceTransport::~DefaultIceTransport() {
  RTC_DCHECK_RUN_ON(&thread_checker_);
}

rtc::scoped_refptr<IceTransportInterface>
DefaultIceTransportFactory::CreateIceTransport(
    const std::string& transport_name,
    int component,
    IceTransportInit init) {
  BasicIceControllerFactory factory;
  return rtc::make_ref_counted<DefaultIceTransport>(
      cricket::P2PTransportChannel::Create(
          transport_name, component, init.port_allocator(),
          init.async_dns_resolver_factory(), init.event_log(), &factory));
}

}  // namespace webrtc
