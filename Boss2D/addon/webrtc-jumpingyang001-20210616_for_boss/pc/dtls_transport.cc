/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__dtls_transport_h //original-code:"pc/dtls_transport.h"

#include <utility>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__dtls_transport_interface_h //original-code:"api/dtls_transport_interface.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_pc__ice_transport_h //original-code:"pc/ice_transport.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_certificate_h //original-code:"rtc_base/ssl_certificate.h"

namespace webrtc {

// Implementation of DtlsTransportInterface
DtlsTransport::DtlsTransport(
    std::unique_ptr<cricket::DtlsTransportInternal> internal)
    : owner_thread_(rtc::Thread::Current()),
      info_(DtlsTransportState::kNew),
      internal_dtls_transport_(std::move(internal)),
      ice_transport_(rtc::make_ref_counted<IceTransportWithPointer>(
          internal_dtls_transport_->ice_transport())) {
  RTC_DCHECK(internal_dtls_transport_.get());
  internal_dtls_transport_->SubscribeDtlsTransportState(
      [this](cricket::DtlsTransportInternal* transport,
             DtlsTransportState state) {
        OnInternalDtlsState(transport, state);
      });
  UpdateInformation();
}

DtlsTransport::~DtlsTransport() {
  // We depend on the signaling thread to call Clear() before dropping
  // its last reference to this object.
  RTC_DCHECK(owner_thread_->IsCurrent() || !internal_dtls_transport_);
}

DtlsTransportInformation DtlsTransport::Information() {
  MutexLock lock(&lock_);
  return info_;
}

void DtlsTransport::RegisterObserver(DtlsTransportObserverInterface* observer) {
  RTC_DCHECK_RUN_ON(owner_thread_);
  RTC_DCHECK(observer);
  observer_ = observer;
}

void DtlsTransport::UnregisterObserver() {
  RTC_DCHECK_RUN_ON(owner_thread_);
  observer_ = nullptr;
}

rtc::scoped_refptr<IceTransportInterface> DtlsTransport::ice_transport() {
  return ice_transport_;
}

// Internal functions
void DtlsTransport::Clear() {
  RTC_DCHECK_RUN_ON(owner_thread_);
  RTC_DCHECK(internal());
  bool must_send_event =
      (internal()->dtls_state() != DtlsTransportState::kClosed);
  // The destructor of cricket::DtlsTransportInternal calls back
  // into DtlsTransport, so we can't hold the lock while releasing.
  std::unique_ptr<cricket::DtlsTransportInternal> transport_to_release;
  {
    MutexLock lock(&lock_);
    transport_to_release = std::move(internal_dtls_transport_);
    ice_transport_->Clear();
  }
  UpdateInformation();
  if (observer_ && must_send_event) {
    observer_->OnStateChange(Information());
  }
}

void DtlsTransport::OnInternalDtlsState(
    cricket::DtlsTransportInternal* transport,
    DtlsTransportState state) {
  RTC_DCHECK_RUN_ON(owner_thread_);
  RTC_DCHECK(transport == internal());
  RTC_DCHECK(state == internal()->dtls_state());
  UpdateInformation();
  if (observer_) {
    observer_->OnStateChange(Information());
  }
}

void DtlsTransport::UpdateInformation() {
  RTC_DCHECK_RUN_ON(owner_thread_);
  MutexLock lock(&lock_);
  if (internal_dtls_transport_) {
    if (internal_dtls_transport_->dtls_state() ==
        DtlsTransportState::kConnected) {
      bool success = true;
      int ssl_cipher_suite;
      int tls_version;
      int srtp_cipher;
      success &= internal_dtls_transport_->GetSslVersionBytes(&tls_version);
      success &= internal_dtls_transport_->GetSslCipherSuite(&ssl_cipher_suite);
      success &= internal_dtls_transport_->GetSrtpCryptoSuite(&srtp_cipher);
      if (success) {
        info_ = DtlsTransportInformation(
            internal_dtls_transport_->dtls_state(), tls_version,
            ssl_cipher_suite, srtp_cipher,
            internal_dtls_transport_->GetRemoteSSLCertChain());
      } else {
        RTC_LOG(LS_ERROR) << "DtlsTransport in connected state has incomplete "
                             "TLS information";
        info_ = DtlsTransportInformation(
            internal_dtls_transport_->dtls_state(), absl::nullopt,
            absl::nullopt, absl::nullopt,
            internal_dtls_transport_->GetRemoteSSLCertChain());
      }
    } else {
      info_ = DtlsTransportInformation(internal_dtls_transport_->dtls_state());
    }
  } else {
    info_ = DtlsTransportInformation(DtlsTransportState::kClosed);
  }
}

}  // namespace webrtc
