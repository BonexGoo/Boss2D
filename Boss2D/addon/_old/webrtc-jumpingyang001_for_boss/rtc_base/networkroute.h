/*
 *  Copyright 2016 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_NETWORKROUTE_H_
#define RTC_BASE_NETWORKROUTE_H_

#include BOSS_FAKEWIN_V_stdint_h //original-code:<stdint.h>

// TODO(honghaiz): Make a directory that describes the interfaces and structs
// the media code can rely on and the network code can implement, and both can
// depend on that, but not depend on each other. Then, move this file to that
// directory.
namespace rtc {

struct NetworkRoute {
  bool connected = false;
  uint16_t local_network_id = 0;
  uint16_t remote_network_id = 0;
  // Last packet id sent on the PREVIOUS route.
  int last_sent_packet_id = -1;
  // The overhead in bytes from IP layer and above.
  int packet_overhead = 0;

  // |last_sent_packet_id| and |packet_overhead| do not affect the NetworkRoute
  // comparison.
  bool operator==(const NetworkRoute& nr) const {
    return connected == nr.connected &&
           local_network_id == nr.local_network_id &&
           remote_network_id == nr.remote_network_id;
  }

  bool operator!=(const NetworkRoute& nr) const { return !(*this == nr); }
};
}  // namespace rtc

#endif  // RTC_BASE_NETWORKROUTE_H_
