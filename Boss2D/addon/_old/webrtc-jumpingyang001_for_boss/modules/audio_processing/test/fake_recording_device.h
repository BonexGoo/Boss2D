/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_TEST_FAKE_RECORDING_DEVICE_H_
#define MODULES_AUDIO_PROCESSING_TEST_FAKE_RECORDING_DEVICE_H_

#include <algorithm>
#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio__audio_frame_h //original-code:"api/audio/audio_frame.h"
#include BOSS_WEBRTC_U_common_audio__channel_buffer_h //original-code:"common_audio/channel_buffer.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {
namespace test {

class FakeRecordingDeviceWorker;

// Class for simulating a microphone with analog gain.
//
// The intended modes of operation are the following:
//
// FakeRecordingDevice fake_mic(255, 1);
//
// fake_mic.SetMicLevel(170);
// fake_mic.SimulateAnalogGain(buffer);
//
// When the mic level to undo is known:
//
// fake_mic.SetMicLevel(170);
// fake_mic.SetUndoMicLevel(30);
// fake_mic.SimulateAnalogGain(buffer);
//
// The second option virtually restores the unmodified microphone level. Calling
// SimulateAnalogGain() will first "undo" the gain applied by the real
// microphone (e.g., 30).
class FakeRecordingDevice final {
 public:
  FakeRecordingDevice(int initial_mic_level, int device_kind);
  ~FakeRecordingDevice();

  int MicLevel() const;
  void SetMicLevel(const int level);
  void SetUndoMicLevel(const int level);

  // Simulates the analog gain.
  // If |real_device_level| is a valid level, the unmodified mic signal is
  // virtually restored. To skip the latter step set |real_device_level| to
  // an empty value.
  void SimulateAnalogGain(AudioFrame* buffer);

  // Simulates the analog gain.
  // If |real_device_level| is a valid level, the unmodified mic signal is
  // virtually restored. To skip the latter step set |real_device_level| to
  // an empty value.
  void SimulateAnalogGain(ChannelBuffer<float>* buffer);

 private:
  // Fake recording device worker.
  std::unique_ptr<FakeRecordingDeviceWorker> worker_;
};

}  // namespace test
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_TEST_FAKE_RECORDING_DEVICE_H_
