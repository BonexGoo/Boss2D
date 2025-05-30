/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef P2P_BASE_TRANSPORTFACTORYINTERFACE_H_
#define P2P_BASE_TRANSPORTFACTORYINTERFACE_H_

#include <memory>
#include <string>

#include BOSS_WEBRTC_U_p2p__base__dtlstransportinternal_h //original-code:"p2p/base/dtlstransportinternal.h"
#include BOSS_WEBRTC_U_p2p__base__icetransportinternal_h //original-code:"p2p/base/icetransportinternal.h"

namespace cricket {

// This interface is used to create DTLS/ICE transports. The external transports
// can be injected into the JsepTransportController through it. For example, the
// FakeIceTransport/FakeDtlsTransport can be injected by setting a
// FakeTransportFactory which implements this interface to the
// JsepTransportController.
class TransportFactoryInterface {
 public:
  virtual ~TransportFactoryInterface() {}

  virtual std::unique_ptr<IceTransportInternal> CreateIceTransport(
      const std::string& transport_name,
      int component) = 0;

  virtual std::unique_ptr<DtlsTransportInternal> CreateDtlsTransport(
      std::unique_ptr<IceTransportInternal> ice,
      const rtc::CryptoOptions& crypto_options) = 0;
};

}  // namespace cricket

#endif  // P2P_BASE_TRANSPORTFACTORYINTERFACE_H_
