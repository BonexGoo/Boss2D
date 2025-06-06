/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_NETEQ_TOOLS_ENCODE_NETEQ_INPUT_H_
#define MODULES_AUDIO_CODING_NETEQ_TOOLS_ENCODE_NETEQ_INPUT_H_

#include <memory>

#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_h //original-code:"api/audio_codecs/audio_encoder.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__tools__neteq_input_h //original-code:"modules/audio_coding/neteq/tools/neteq_input.h"

namespace webrtc {
namespace test {

// This class provides a NetEqInput that takes audio from a generator object and
// encodes it using a given audio encoder.
class EncodeNetEqInput : public NetEqInput {
 public:
  // Generator class, to be provided to the EncodeNetEqInput constructor.
  class Generator {
   public:
    virtual ~Generator() = default;
    // Returns the next num_samples values from the signal generator.
    virtual rtc::ArrayView<const int16_t> Generate(size_t num_samples) = 0;
  };

  // The source will end after the given input duration.
  EncodeNetEqInput(std::unique_ptr<Generator> generator,
                   std::unique_ptr<AudioEncoder> encoder,
                   int64_t input_duration_ms);
  ~EncodeNetEqInput() override;

  absl::optional<int64_t> NextPacketTime() const override;

  absl::optional<int64_t> NextOutputEventTime() const override;

  std::unique_ptr<PacketData> PopPacket() override;

  void AdvanceOutputEvent() override;

  bool ended() const override;

  absl::optional<RTPHeader> NextHeader() const override;

 private:
  static constexpr int64_t kOutputPeriodMs = 10;

  void CreatePacket();

  std::unique_ptr<Generator> generator_;
  std::unique_ptr<AudioEncoder> encoder_;
  std::unique_ptr<PacketData> packet_data_;
  uint32_t rtp_timestamp_ = 0;
  int16_t sequence_number_ = 0;
  int64_t next_packet_time_ms_ = 0;
  int64_t next_output_event_ms_ = 0;
  const int64_t input_duration_ms_;
};

}  // namespace test
}  // namespace webrtc
#endif  // MODULES_AUDIO_CODING_NETEQ_TOOLS_ENCODE_NETEQ_INPUT_H_
