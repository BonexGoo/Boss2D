/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__sslcertificate_h //original-code:"rtc_base/sslcertificate.h"

#include <ctime>
#include <string>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__opensslcertificate_h //original-code:"rtc_base/opensslcertificate.h"
#include BOSS_WEBRTC_U_rtc_base__sslfingerprint_h //original-code:"rtc_base/sslfingerprint.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__base64__base64_h //original-code:"rtc_base/third_party/base64/base64.h"

namespace rtc {

//////////////////////////////////////////////////////////////////////
// SSLCertificateStats
//////////////////////////////////////////////////////////////////////

SSLCertificateStats::SSLCertificateStats(
    std::string&& fingerprint,
    std::string&& fingerprint_algorithm,
    std::string&& base64_certificate,
    std::unique_ptr<SSLCertificateStats>&& issuer)
    : fingerprint(std::move(fingerprint)),
      fingerprint_algorithm(std::move(fingerprint_algorithm)),
      base64_certificate(std::move(base64_certificate)),
      issuer(std::move(issuer)) {}

SSLCertificateStats::~SSLCertificateStats() {}

//////////////////////////////////////////////////////////////////////
// SSLCertificate
//////////////////////////////////////////////////////////////////////

std::unique_ptr<SSLCertificateStats> SSLCertificate::GetStats() const {
  // TODO(bemasc): Move this computation to a helper class that caches these
  // values to reduce CPU use in |StatsCollector::GetStats|. This will require
  // adding a fast |SSLCertificate::Equals| to detect certificate changes.
  std::string digest_algorithm;
  if (!GetSignatureDigestAlgorithm(&digest_algorithm))
    return nullptr;

  // |SSLFingerprint::Create| can fail if the algorithm returned by
  // |SSLCertificate::GetSignatureDigestAlgorithm| is not supported by the
  // implementation of |SSLCertificate::ComputeDigest|. This currently happens
  // with MD5- and SHA-224-signed certificates when linked to libNSS.
  std::unique_ptr<SSLFingerprint> ssl_fingerprint(
      SSLFingerprint::Create(digest_algorithm, this));
  if (!ssl_fingerprint)
    return nullptr;
  std::string fingerprint = ssl_fingerprint->GetRfc4572Fingerprint();

  Buffer der_buffer;
  ToDER(&der_buffer);
  std::string der_base64;
  Base64::EncodeFromArray(der_buffer.data(), der_buffer.size(), &der_base64);

  return absl::make_unique<SSLCertificateStats>(std::move(fingerprint),
                                                std::move(digest_algorithm),
                                                std::move(der_base64), nullptr);
}

std::unique_ptr<SSLCertificate> SSLCertificate::GetUniqueReference() const {
  return absl::WrapUnique(GetReference());
}

//////////////////////////////////////////////////////////////////////
// SSLCertChain
//////////////////////////////////////////////////////////////////////

SSLCertChain::SSLCertChain(std::vector<std::unique_ptr<SSLCertificate>> certs)
    : certs_(std::move(certs)) {}

SSLCertChain::SSLCertChain(const std::vector<SSLCertificate*>& certs) {
  RTC_DCHECK(!certs.empty());
  certs_.resize(certs.size());
  std::transform(
      certs.begin(), certs.end(), certs_.begin(),
      [](const SSLCertificate* cert) -> std::unique_ptr<SSLCertificate> {
        return cert->GetUniqueReference();
      });
}

SSLCertChain::SSLCertChain(const SSLCertificate* cert) {
  certs_.push_back(cert->GetUniqueReference());
}

SSLCertChain::SSLCertChain(SSLCertChain&& rhs) = default;

SSLCertChain& SSLCertChain::operator=(SSLCertChain&&) = default;

SSLCertChain::~SSLCertChain() {}

SSLCertChain* SSLCertChain::Copy() const {
  std::vector<std::unique_ptr<SSLCertificate>> new_certs(certs_.size());
  std::transform(certs_.begin(), certs_.end(), new_certs.begin(),
                 [](const std::unique_ptr<SSLCertificate>& cert)
                     -> std::unique_ptr<SSLCertificate> {
                   return cert->GetUniqueReference();
                 });
  return new SSLCertChain(std::move(new_certs));
}

std::unique_ptr<SSLCertChain> SSLCertChain::UniqueCopy() const {
  return absl::WrapUnique(Copy());
}

std::unique_ptr<SSLCertificateStats> SSLCertChain::GetStats() const {
  // We have a linked list of certificates, starting with the first element of
  // |certs_| and ending with the last element of |certs_|. The "issuer" of a
  // certificate is the next certificate in the chain. Stats are produced for
  // each certificate in the list. Here, the "issuer" is the issuer's stats.
  std::unique_ptr<SSLCertificateStats> issuer;
  // The loop runs in reverse so that the |issuer| is known before the
  // certificate issued by |issuer|.
  for (ptrdiff_t i = certs_.size() - 1; i >= 0; --i) {
    std::unique_ptr<SSLCertificateStats> new_stats = certs_[i]->GetStats();
    if (new_stats) {
      new_stats->issuer = std::move(issuer);
    }
    issuer = std::move(new_stats);
  }
  return issuer;
}

// static
SSLCertificate* SSLCertificate::FromPEMString(const std::string& pem_string) {
  return OpenSSLCertificate::FromPEMString(pem_string);
}

}  // namespace rtc
