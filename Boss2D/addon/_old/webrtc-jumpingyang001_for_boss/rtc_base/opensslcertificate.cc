/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__opensslcertificate_h //original-code:"rtc_base/opensslcertificate.h"

#include <memory>
#include <utility>
#include <vector>

#if defined(WEBRTC_WIN)
// Must be included first before openssl headers.
#include BOSS_WEBRTC_U_rtc_base__win32_h //original-code:"rtc_base/win32.h"  // NOLINT
#endif                       // WEBRTC_WIN

#include BOSS_OPENSSL_V_openssl__bio_h //original-code:<openssl/bio.h>
#include BOSS_OPENSSL_V_openssl__bn_h //original-code:<openssl/bn.h>
#include BOSS_OPENSSL_V_openssl__crypto_h //original-code:<openssl/crypto.h>
#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include BOSS_OPENSSL_V_openssl__pem_h //original-code:<openssl/pem.h>
#include BOSS_OPENSSL_V_openssl__rsa_h //original-code:<openssl/rsa.h>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_rtc_base__arraysize_h //original-code:"rtc_base/arraysize.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__helpers_h //original-code:"rtc_base/helpers.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__openssl_h //original-code:"rtc_base/openssl.h"
#include BOSS_WEBRTC_U_rtc_base__openssldigest_h //original-code:"rtc_base/openssldigest.h"
#include BOSS_WEBRTC_U_rtc_base__opensslidentity_h //original-code:"rtc_base/opensslidentity.h"
#include BOSS_WEBRTC_U_rtc_base__opensslutility_h //original-code:"rtc_base/opensslutility.h"
#ifdef WEBRTC_BUILT_IN_SSL_ROOT_CERTIFICATES
#include BOSS_WEBRTC_U_rtc_base__sslroots_h //original-code:"rtc_base/sslroots.h"
#endif

namespace rtc {

//////////////////////////////////////////////////////////////////////
// OpenSSLCertificate
//////////////////////////////////////////////////////////////////////

// We could have exposed a myriad of parameters for the crypto stuff,
// but keeping it simple seems best.

// Random bits for certificate serial number
static const int SERIAL_RAND_BITS = 64;

// Generate a self-signed certificate, with the public key from the
// given key pair. Caller is responsible for freeing the returned object.
static X509* MakeCertificate(EVP_PKEY* pkey, const SSLIdentityParams& params) {
  RTC_LOG(LS_INFO) << "Making certificate for " << params.common_name;
  X509* x509 = nullptr;
  BIGNUM* serial_number = nullptr;
  X509_NAME* name = nullptr;
  time_t epoch_off = 0;  // Time offset since epoch.

  if ((x509 = X509_new()) == nullptr)
    goto error;

  if (!X509_set_pubkey(x509, pkey))
    goto error;

  // serial number
  // temporary reference to serial number inside x509 struct
  ASN1_INTEGER* asn1_serial_number;
  if ((serial_number = BN_new()) == nullptr ||
      !BN_pseudo_rand(serial_number, SERIAL_RAND_BITS, 0, 0) ||
      (asn1_serial_number = X509_get_serialNumber(x509)) == nullptr ||
      !BN_to_ASN1_INTEGER(serial_number, asn1_serial_number))
    goto error;

  if (!X509_set_version(x509, 2L))  // version 3
    goto error;

  // There are a lot of possible components for the name entries. In
  // our P2P SSL mode however, the certificates are pre-exchanged
  // (through the secure XMPP channel), and so the certificate
  // identification is arbitrary. It can't be empty, so we set some
  // arbitrary common_name. Note that this certificate goes out in
  // clear during SSL negotiation, so there may be a privacy issue in
  // putting anything recognizable here.
  if ((name = X509_NAME_new()) == nullptr ||
      !X509_NAME_add_entry_by_NID(name, NID_commonName, MBSTRING_UTF8,
                                  (unsigned char*)params.common_name.c_str(),
                                  -1, -1, 0) ||
      !X509_set_subject_name(x509, name) || !X509_set_issuer_name(x509, name))
    goto error;

  if (!X509_time_adj(X509_get_notBefore(x509), params.not_before, &epoch_off) ||
      !X509_time_adj(X509_get_notAfter(x509), params.not_after, &epoch_off))
    goto error;

  if (!X509_sign(x509, pkey, EVP_sha256()))
    goto error;

  BN_free(serial_number);
  X509_NAME_free(name);
  RTC_LOG(LS_INFO) << "Returning certificate";
  return x509;

error:
  BN_free(serial_number);
  X509_NAME_free(name);
  X509_free(x509);
  return nullptr;
}

#if !defined(NDEBUG)
// Print a certificate to the log, for debugging.
static void PrintCert(X509* x509) {
  BIO* temp_memory_bio = BIO_new(BIO_s_mem());
  if (!temp_memory_bio) {
    RTC_DLOG_F(LS_ERROR) << "Failed to allocate temporary memory bio";
    return;
  }
  X509_print_ex(temp_memory_bio, x509, XN_FLAG_SEP_CPLUS_SPC, 0);
  BIO_write(temp_memory_bio, "\0", 1);
  char* buffer;
  BIO_get_mem_data(temp_memory_bio, &buffer);
  RTC_DLOG(LS_VERBOSE) << buffer;
  BIO_free(temp_memory_bio);
}
#endif

OpenSSLCertificate::OpenSSLCertificate(X509* x509) : x509_(x509) {
  AddReference();
}

OpenSSLCertificate* OpenSSLCertificate::Generate(
    OpenSSLKeyPair* key_pair,
    const SSLIdentityParams& params) {
  SSLIdentityParams actual_params(params);
  if (actual_params.common_name.empty()) {
    // Use a random string, arbitrarily 8chars long.
    actual_params.common_name = CreateRandomString(8);
  }
  X509* x509 = MakeCertificate(key_pair->pkey(), actual_params);
  if (!x509) {
    openssl::LogSSLErrors("Generating certificate");
    return nullptr;
  }
#if !defined(NDEBUG)
  PrintCert(x509);
#endif
  OpenSSLCertificate* ret = new OpenSSLCertificate(x509);
  X509_free(x509);
  return ret;
}

OpenSSLCertificate* OpenSSLCertificate::FromPEMString(
    const std::string& pem_string) {
  BIO* bio = BIO_new_mem_buf(const_cast<char*>(pem_string.c_str()), -1);
  if (!bio)
    return nullptr;
  BIO_set_mem_eof_return(bio, 0);
  X509* x509 =
      PEM_read_bio_X509(bio, nullptr, nullptr, const_cast<char*>("\0"));
  BIO_free(bio);  // Frees the BIO, but not the pointed-to string.

  if (!x509)
    return nullptr;

  OpenSSLCertificate* ret = new OpenSSLCertificate(x509);
  X509_free(x509);
  return ret;
}

// NOTE: This implementation only functions correctly after InitializeSSL
// and before CleanupSSL.
bool OpenSSLCertificate::GetSignatureDigestAlgorithm(
    std::string* algorithm) const {
  int nid = X509_get_signature_nid(x509_);
  switch (nid) {
    case NID_md5WithRSA:
    case NID_md5WithRSAEncryption:
      *algorithm = DIGEST_MD5;
      break;
    case NID_ecdsa_with_SHA1:
    case NID_dsaWithSHA1:
    case NID_dsaWithSHA1_2:
    case NID_sha1WithRSA:
    case NID_sha1WithRSAEncryption:
      *algorithm = DIGEST_SHA_1;
      break;
    case NID_ecdsa_with_SHA224:
    case NID_sha224WithRSAEncryption:
    case NID_dsa_with_SHA224:
      *algorithm = DIGEST_SHA_224;
      break;
    case NID_ecdsa_with_SHA256:
    case NID_sha256WithRSAEncryption:
    case NID_dsa_with_SHA256:
      *algorithm = DIGEST_SHA_256;
      break;
    case NID_ecdsa_with_SHA384:
    case NID_sha384WithRSAEncryption:
      *algorithm = DIGEST_SHA_384;
      break;
    case NID_ecdsa_with_SHA512:
    case NID_sha512WithRSAEncryption:
      *algorithm = DIGEST_SHA_512;
      break;
    default:
      // Unknown algorithm.  There are several unhandled options that are less
      // common and more complex.
      RTC_LOG(LS_ERROR) << "Unknown signature algorithm NID: " << nid;
      algorithm->clear();
      return false;
  }
  return true;
}

bool OpenSSLCertificate::ComputeDigest(const std::string& algorithm,
                                       unsigned char* digest,
                                       size_t size,
                                       size_t* length) const {
  return ComputeDigest(x509_, algorithm, digest, size, length);
}

bool OpenSSLCertificate::ComputeDigest(const X509* x509,
                                       const std::string& algorithm,
                                       unsigned char* digest,
                                       size_t size,
                                       size_t* length) {
  const EVP_MD* md;
  unsigned int n;

  if (!OpenSSLDigest::GetDigestEVP(algorithm, &md))
    return false;

  if (size < static_cast<size_t>(EVP_MD_size(md)))
    return false;

  X509_digest(x509, md, digest, &n);

  *length = n;

  return true;
}

OpenSSLCertificate::~OpenSSLCertificate() {
  X509_free(x509_);
}

OpenSSLCertificate* OpenSSLCertificate::GetReference() const {
  return new OpenSSLCertificate(x509_);
}

std::string OpenSSLCertificate::ToPEMString() const {
  BIO* bio = BIO_new(BIO_s_mem());
  if (!bio) {
    FATAL() << "unreachable code";
  }
  if (!PEM_write_bio_X509(bio, x509_)) {
    BIO_free(bio);
    FATAL() << "unreachable code";
  }
  BIO_write(bio, "\0", 1);
  char* buffer;
  BIO_get_mem_data(bio, &buffer);
  std::string ret(buffer);
  BIO_free(bio);
  return ret;
}

void OpenSSLCertificate::ToDER(Buffer* der_buffer) const {
  // In case of failure, make sure to leave the buffer empty.
  der_buffer->SetSize(0);

  // Calculates the DER representation of the certificate, from scratch.
  BIO* bio = BIO_new(BIO_s_mem());
  if (!bio) {
    FATAL() << "unreachable code";
  }
  if (!i2d_X509_bio(bio, x509_)) {
    BIO_free(bio);
    FATAL() << "unreachable code";
  }
  char* data;
  size_t length = BIO_get_mem_data(bio, &data);
  der_buffer->SetData(data, length);
  BIO_free(bio);
}

void OpenSSLCertificate::AddReference() const {
  RTC_DCHECK(x509_ != nullptr);
  X509_up_ref(x509_);
}

bool OpenSSLCertificate::operator==(const OpenSSLCertificate& other) const {
  return X509_cmp(x509_, other.x509_) == 0;
}

bool OpenSSLCertificate::operator!=(const OpenSSLCertificate& other) const {
  return !(*this == other);
}

// Documented in sslidentity.h.
int64_t OpenSSLCertificate::CertificateExpirationTime() const {
  ASN1_TIME* expire_time = X509_get_notAfter(x509_);
  bool long_format;

  if (expire_time->type == V_ASN1_UTCTIME) {
    long_format = false;
  } else if (expire_time->type == V_ASN1_GENERALIZEDTIME) {
    long_format = true;
  } else {
    return -1;
  }

  return ASN1TimeToSec(expire_time->data, expire_time->length, long_format);
}

}  // namespace rtc