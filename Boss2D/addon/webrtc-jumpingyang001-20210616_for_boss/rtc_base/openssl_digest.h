/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_OPENSSL_DIGEST_H_
#define RTC_BASE_OPENSSL_DIGEST_H_

#include BOSS_OPENSSL_V_openssl__ossl_typ_h //original-code:<openssl/ossl_typ.h>
#include <stddef.h>

#include <string>

#include BOSS_WEBRTC_U_rtc_base__message_digest_h //original-code:"rtc_base/message_digest.h"

namespace rtc {

// An implementation of the digest class that uses OpenSSL.
class OpenSSLDigest final : public MessageDigest {
 public:
  // Creates an OpenSSLDigest with |algorithm| as the hash algorithm.
  explicit OpenSSLDigest(const std::string& algorithm);
  ~OpenSSLDigest() override;
  // Returns the digest output size (e.g. 16 bytes for MD5).
  size_t Size() const override;
  // Updates the digest with |len| bytes from |buf|.
  void Update(const void* buf, size_t len) override;
  // Outputs the digest value to |buf| with length |len|.
  size_t Finish(void* buf, size_t len) override;

  // Helper function to look up a digest's EVP by name.
  static bool GetDigestEVP(const std::string& algorithm, const EVP_MD** md);
  // Helper function to look up a digest's name by EVP.
  static bool GetDigestName(const EVP_MD* md, std::string* algorithm);
  // Helper function to get the length of a digest.
  static bool GetDigestSize(const std::string& algorithm, size_t* len);

 private:
  EVP_MD_CTX* ctx_ = nullptr;
  const EVP_MD* md_;
};

}  // namespace rtc

#endif  // RTC_BASE_OPENSSL_DIGEST_H_
