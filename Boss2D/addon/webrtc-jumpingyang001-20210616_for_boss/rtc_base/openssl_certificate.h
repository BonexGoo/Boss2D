/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_OPENSSL_CERTIFICATE_H_
#define RTC_BASE_OPENSSL_CERTIFICATE_H_

#include BOSS_OPENSSL_V_openssl__ossl_typ_h //original-code:<openssl/ossl_typ.h>
#include <stddef.h>
#include <stdint.h>

#include <string>

#include BOSS_WEBRTC_U_rtc_base__buffer_h //original-code:"rtc_base/buffer.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_certificate_h //original-code:"rtc_base/ssl_certificate.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_identity_h //original-code:"rtc_base/ssl_identity.h"

namespace rtc {

class OpenSSLKeyPair;

// OpenSSLCertificate encapsulates an OpenSSL X509* certificate object,
// which is also reference counted inside the OpenSSL library.
class OpenSSLCertificate final : public SSLCertificate {
 public:
  // X509 object has its reference count incremented. So the caller and
  // OpenSSLCertificate share ownership.
  explicit OpenSSLCertificate(X509* x509);

  static std::unique_ptr<OpenSSLCertificate> Generate(
      OpenSSLKeyPair* key_pair,
      const SSLIdentityParams& params);
  static std::unique_ptr<OpenSSLCertificate> FromPEMString(
      const std::string& pem_string);

  ~OpenSSLCertificate() override;

  std::unique_ptr<SSLCertificate> Clone() const override;

  X509* x509() const { return x509_; }

  std::string ToPEMString() const override;
  void ToDER(Buffer* der_buffer) const override;
  bool operator==(const OpenSSLCertificate& other) const;
  bool operator!=(const OpenSSLCertificate& other) const;

  // Compute the digest of the certificate given algorithm
  bool ComputeDigest(const std::string& algorithm,
                     unsigned char* digest,
                     size_t size,
                     size_t* length) const override;

  // Compute the digest of a certificate as an X509 *
  static bool ComputeDigest(const X509* x509,
                            const std::string& algorithm,
                            unsigned char* digest,
                            size_t size,
                            size_t* length);

  bool GetSignatureDigestAlgorithm(std::string* algorithm) const override;

  int64_t CertificateExpirationTime() const override;

 private:
  X509* x509_;  // NOT OWNED
  RTC_DISALLOW_COPY_AND_ASSIGN(OpenSSLCertificate);
};

}  // namespace rtc

#endif  // RTC_BASE_OPENSSL_CERTIFICATE_H_
