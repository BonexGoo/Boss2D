/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_SDPUTILS_H_
#define PC_SDPUTILS_H_

#include <functional>
#include <memory>
#include <string>

#include BOSS_WEBRTC_U_api__jsep_h //original-code:"api/jsep.h"
#include BOSS_WEBRTC_U_pc__sessiondescription_h //original-code:"pc/sessiondescription.h"

namespace webrtc {

// Returns a copy of the given session description.
std::unique_ptr<SessionDescriptionInterface> CloneSessionDescription(
    const SessionDescriptionInterface* sdesc);

// Returns a copy of the given session description with the type changed.
std::unique_ptr<SessionDescriptionInterface> CloneSessionDescriptionAsType(
    const SessionDescriptionInterface* sdesc,
    SdpType type);

// Function that takes a single session description content with its
// corresponding transport and produces a boolean.
typedef std::function<bool(const cricket::ContentInfo*,
                           const cricket::TransportInfo*)>
    SdpContentPredicate;

// Returns true if the predicate returns true for all contents in the given
// session description.
bool SdpContentsAll(SdpContentPredicate pred,
                    const cricket::SessionDescription* desc);

// Returns true if the predicate returns true for none of the contents in the
// given session description.
bool SdpContentsNone(SdpContentPredicate pred,
                     const cricket::SessionDescription* desc);

// Function that takes a single session description content with its
// corresponding transport and can mutate the content and/or the transport.
typedef std::function<void(cricket::ContentInfo*, cricket::TransportInfo*)>
    SdpContentMutator;

// Applies the mutator function over all contents in the given session
// description.
void SdpContentsForEach(SdpContentMutator fn,
                        cricket::SessionDescription* desc);

}  // namespace webrtc

#endif  // PC_SDPUTILS_H_
