/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_DTLS_TRANSPORT_H_
#define PC_DTLS_TRANSPORT_H_

#include <memory>

#include BOSS_WEBRTC_U_api__dtls_transport_interface_h //original-code:"api/dtls_transport_interface.h"
#include BOSS_WEBRTC_U_api__ice_transport_interface_h //original-code:"api/ice_transport_interface.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_p2p__base__dtls_transport_h //original-code:"p2p/base/dtls_transport.h"
#include BOSS_WEBRTC_U_p2p__base__dtls_transport_internal_h //original-code:"p2p/base/dtls_transport_internal.h"
#include BOSS_WEBRTC_U_pc__ice_transport_h //original-code:"pc/ice_transport.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class IceTransportWithPointer;

// This implementation wraps a cricket::DtlsTransport, and takes
// ownership of it.
class DtlsTransport : public DtlsTransportInterface {
 public:
  // This object must be constructed and updated on a consistent thread,
  // the same thread as the one the cricket::DtlsTransportInternal object
  // lives on.
  // The Information() function can be called from a different thread,
  // such as the signalling thread.
  explicit DtlsTransport(
      std::unique_ptr<cricket::DtlsTransportInternal> internal);

  rtc::scoped_refptr<IceTransportInterface> ice_transport() override;
  DtlsTransportInformation Information() override;
  void RegisterObserver(DtlsTransportObserverInterface* observer) override;
  void UnregisterObserver() override;
  void Clear();

  cricket::DtlsTransportInternal* internal() {
    MutexLock lock(&lock_);
    return internal_dtls_transport_.get();
  }

  const cricket::DtlsTransportInternal* internal() const {
    MutexLock lock(&lock_);
    return internal_dtls_transport_.get();
  }

 protected:
  ~DtlsTransport();

 private:
  void OnInternalDtlsState(cricket::DtlsTransportInternal* transport,
                           DtlsTransportState state);
  void UpdateInformation();

  DtlsTransportObserverInterface* observer_ = nullptr;
  rtc::Thread* owner_thread_;
  mutable Mutex lock_;
  DtlsTransportInformation info_ RTC_GUARDED_BY(lock_);
  std::unique_ptr<cricket::DtlsTransportInternal> internal_dtls_transport_
      RTC_GUARDED_BY(lock_);
  const rtc::scoped_refptr<IceTransportWithPointer> ice_transport_;
};

}  // namespace webrtc
#endif  // PC_DTLS_TRANSPORT_H_
