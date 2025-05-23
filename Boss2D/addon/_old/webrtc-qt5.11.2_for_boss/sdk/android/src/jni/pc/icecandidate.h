/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_ANDROID_SRC_JNI_PC_ICECANDIDATE_H_
#define SDK_ANDROID_SRC_JNI_PC_ICECANDIDATE_H_

#include <vector>

#include BOSS_WEBRTC_U_api__datachannelinterface_h //original-code:"api/datachannelinterface.h"
#include BOSS_WEBRTC_U_api__jsep_h //original-code:"api/jsep.h"
#include BOSS_WEBRTC_U_api__jsepicecandidate_h //original-code:"api/jsepicecandidate.h"
#include BOSS_WEBRTC_U_api__peerconnectioninterface_h //original-code:"api/peerconnectioninterface.h"
#include BOSS_WEBRTC_U_api__rtpparameters_h //original-code:"api/rtpparameters.h"
#include BOSS_WEBRTC_U_rtc_base__sslidentity_h //original-code:"rtc_base/sslidentity.h"
#include "sdk/android/src/jni/jni_helpers.h"

namespace webrtc {
namespace jni {

cricket::Candidate JavaToNativeCandidate(JNIEnv* jni,
                                         const JavaRef<jobject>& j_candidate);

ScopedJavaLocalRef<jobject> NativeToJavaCandidate(
    JNIEnv* env,
    const cricket::Candidate& candidate);

ScopedJavaLocalRef<jobject> NativeToJavaIceCandidate(
    JNIEnv* env,
    const IceCandidateInterface& candidate);

ScopedJavaLocalRef<jobjectArray> NativeToJavaCandidateArray(
    JNIEnv* jni,
    const std::vector<cricket::Candidate>& candidates);

/*****************************************************
 * Below are all things that go into RTCConfiguration.
 *****************************************************/
PeerConnectionInterface::IceTransportsType JavaToNativeIceTransportsType(
    JNIEnv* jni,
    const JavaRef<jobject>& j_ice_transports_type);

PeerConnectionInterface::BundlePolicy JavaToNativeBundlePolicy(
    JNIEnv* jni,
    const JavaRef<jobject>& j_bundle_policy);

PeerConnectionInterface::RtcpMuxPolicy JavaToNativeRtcpMuxPolicy(
    JNIEnv* jni,
    const JavaRef<jobject>& j_rtcp_mux_policy);

PeerConnectionInterface::TcpCandidatePolicy JavaToNativeTcpCandidatePolicy(
    JNIEnv* jni,
    const JavaRef<jobject>& j_tcp_candidate_policy);

PeerConnectionInterface::CandidateNetworkPolicy
JavaToNativeCandidateNetworkPolicy(
    JNIEnv* jni,
    const JavaRef<jobject>& j_candidate_network_policy);

rtc::KeyType JavaToNativeKeyType(JNIEnv* jni,
                                 const JavaRef<jobject>& j_key_type);

PeerConnectionInterface::ContinualGatheringPolicy
JavaToNativeContinualGatheringPolicy(
    JNIEnv* jni,
    const JavaRef<jobject>& j_gathering_policy);

PeerConnectionInterface::TlsCertPolicy JavaToNativeTlsCertPolicy(
    JNIEnv* jni,
    const JavaRef<jobject>& j_ice_server_tls_cert_policy);

}  // namespace jni
}  // namespace webrtc

#endif  // SDK_ANDROID_SRC_JNI_PC_ICECANDIDATE_H_
