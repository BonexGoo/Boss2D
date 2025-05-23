/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_ANDROID_SRC_JNI_PC_PEERCONNECTION_H_
#define SDK_ANDROID_SRC_JNI_PC_PEERCONNECTION_H_

#include <pc/mediastreamobserver.h>
#include <map>
#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__peerconnectioninterface_h //original-code:"api/peerconnectioninterface.h"
#include "sdk/android/src/jni/jni_helpers.h"
#include "sdk/android/src/jni/pc/mediaconstraints.h"
#include "sdk/android/src/jni/pc/mediastream.h"
#include "sdk/android/src/jni/pc/rtpreceiver.h"

namespace webrtc {
namespace jni {

void JavaToNativeRTCConfiguration(
    JNIEnv* jni,
    const JavaRef<jobject>& j_rtc_config,
    PeerConnectionInterface::RTCConfiguration* rtc_config);

rtc::KeyType GetRtcConfigKeyType(JNIEnv* env,
                                 const JavaRef<jobject>& j_rtc_config);

// Adapter between the C++ PeerConnectionObserver interface and the Java
// PeerConnection.Observer interface.  Wraps an instance of the Java interface
// and dispatches C++ callbacks to Java.
class PeerConnectionObserverJni : public PeerConnectionObserver {
 public:
  PeerConnectionObserverJni(JNIEnv* jni, const JavaRef<jobject>& j_observer);
  virtual ~PeerConnectionObserverJni();

  // Implementation of PeerConnectionObserver interface, which propagates
  // the callbacks to the Java observer.
  void OnIceCandidate(const IceCandidateInterface* candidate) override;
  void OnIceCandidatesRemoved(
      const std::vector<cricket::Candidate>& candidates) override;
  void OnSignalingChange(
      PeerConnectionInterface::SignalingState new_state) override;
  void OnIceConnectionChange(
      PeerConnectionInterface::IceConnectionState new_state) override;
  void OnIceConnectionReceivingChange(bool receiving) override;
  void OnIceGatheringChange(
      PeerConnectionInterface::IceGatheringState new_state) override;
  void OnAddStream(rtc::scoped_refptr<MediaStreamInterface> stream) override;
  void OnRemoveStream(rtc::scoped_refptr<MediaStreamInterface> stream) override;
  void OnDataChannel(rtc::scoped_refptr<DataChannelInterface> channel) override;
  void OnRenegotiationNeeded() override;
  void OnAddTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver,
                  const std::vector<rtc::scoped_refptr<MediaStreamInterface>>&
                      streams) override;

 private:
  typedef std::map<MediaStreamInterface*, JavaMediaStream>
      NativeToJavaStreamsMap;
  typedef std::map<MediaStreamTrackInterface*, RtpReceiverInterface*>
      NativeMediaStreamTrackToNativeRtpReceiver;

  // If the NativeToJavaStreamsMap contains the stream, return it.
  // Otherwise, create a new Java MediaStream. Returns a global jobject.
  JavaMediaStream& GetOrCreateJavaStream(
      JNIEnv* env,
      const rtc::scoped_refptr<MediaStreamInterface>& stream);

  // Converts array of streams, creating or re-using Java streams as necessary.
  ScopedJavaLocalRef<jobjectArray> NativeToJavaMediaStreamArray(
      JNIEnv* jni,
      const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams);

  const ScopedJavaGlobalRef<jobject> j_observer_global_;

  // C++ -> Java remote streams.
  NativeToJavaStreamsMap remote_streams_;
  std::vector<JavaRtpReceiverGlobalOwner> rtp_receivers_;
};

// PeerConnection doesn't take ownership of the observer. In Java API, we don't
// want the client to have to manually dispose the observer. To solve this, this
// wrapper class is used for object ownership.
//
// Also stores reference to the deprecated PeerConnection constraints for now.
class OwnedPeerConnection {
 public:
  OwnedPeerConnection(
      rtc::scoped_refptr<PeerConnectionInterface> peer_connection,
      std::unique_ptr<PeerConnectionObserver> observer)
      : OwnedPeerConnection(peer_connection,
                            std::move(observer),
                            nullptr /* constraints */) {}
  // Deprecated. PC constraints are deprecated.
  OwnedPeerConnection(
      rtc::scoped_refptr<PeerConnectionInterface> peer_connection,
      std::unique_ptr<PeerConnectionObserver> observer,
      std::unique_ptr<MediaConstraintsInterface> constraints);
  ~OwnedPeerConnection();

  PeerConnectionInterface* pc() const { return peer_connection_.get(); }
  const MediaConstraintsInterface* constraints() const {
    return constraints_.get();
  }

 private:
  rtc::scoped_refptr<PeerConnectionInterface> peer_connection_;
  std::unique_ptr<PeerConnectionObserver> observer_;
  std::unique_ptr<MediaConstraintsInterface> constraints_;
};

}  // namespace jni
}  // namespace webrtc

#endif  // SDK_ANDROID_SRC_JNI_PC_PEERCONNECTION_H_
