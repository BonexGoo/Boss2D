/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef VIDEO_TRANSPORT_ADAPTER_H_
#define VIDEO_TRANSPORT_ADAPTER_H_

#include <stddef.h>
#include <stdint.h>

#include <atomic>

#include BOSS_WEBRTC_U_api__call__transport_h //original-code:"api/call/transport.h"

namespace webrtc {
namespace internal {

class TransportAdapter : public Transport {
 public:
  explicit TransportAdapter(Transport* transport);
  ~TransportAdapter() override;

  bool SendRtp(const uint8_t* packet,
               size_t length,
               const PacketOptions& options) override;
  bool SendRtcp(const uint8_t* packet, size_t length) override;

  void Enable();
  void Disable();

 private:
  Transport* transport_;
  std::atomic<bool> enabled_;
};
}  // namespace internal
}  // namespace webrtc

#endif  // VIDEO_TRANSPORT_ADAPTER_H_
