/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// TODO(deadbeef): Move this out of api/; it's an implementation detail and
// shouldn't be used externally.

#ifndef API_JSEPICECANDIDATE_H_
#define API_JSEPICECANDIDATE_H_

#include <string>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_api__candidate_h //original-code:"api/candidate.h"
#include BOSS_WEBRTC_U_api__jsep_h //original-code:"api/jsep.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

// Implementation of IceCandidateInterface.
class JsepIceCandidate : public IceCandidateInterface {
 public:
  JsepIceCandidate(const std::string& sdp_mid, int sdp_mline_index);
  JsepIceCandidate(const std::string& sdp_mid,
                   int sdp_mline_index,
                   const cricket::Candidate& candidate);
  ~JsepIceCandidate() override;
  // |err| may be null.
  bool Initialize(const std::string& sdp, SdpParseError* err);
  void SetCandidate(const cricket::Candidate& candidate) {
    candidate_ = candidate;
  }

  std::string sdp_mid() const override;
  int sdp_mline_index() const override;
  const cricket::Candidate& candidate() const override;

  std::string server_url() const override;

  bool ToString(std::string* out) const override;

 private:
  std::string sdp_mid_;
  int sdp_mline_index_;
  cricket::Candidate candidate_;

  RTC_DISALLOW_COPY_AND_ASSIGN(JsepIceCandidate);
};

// Implementation of IceCandidateCollection which stores JsepIceCandidates.
class JsepCandidateCollection : public IceCandidateCollection {
 public:
  JsepCandidateCollection();
  // Move constructor is defined so that a vector of JsepCandidateCollections
  // can be resized.
  JsepCandidateCollection(JsepCandidateCollection&& o);
  ~JsepCandidateCollection() override;
  size_t count() const override;
  bool HasCandidate(const IceCandidateInterface* candidate) const override;
  // Adds and takes ownership of the JsepIceCandidate.
  // TODO(deadbeef): Make this use an std::unique_ptr<>, so ownership logic is
  // more clear.
  virtual void add(JsepIceCandidate* candidate);
  const IceCandidateInterface* at(size_t index) const override;
  // Removes the candidate that has a matching address and protocol.
  //
  // Returns the number of candidates that were removed.
  size_t remove(const cricket::Candidate& candidate);

 private:
  std::vector<JsepIceCandidate*> candidates_;

  RTC_DISALLOW_COPY_AND_ASSIGN(JsepCandidateCollection);
};

}  // namespace webrtc

#endif  // API_JSEPICECANDIDATE_H_
