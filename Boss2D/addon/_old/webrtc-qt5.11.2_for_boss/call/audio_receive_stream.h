/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_AUDIO_RECEIVE_STREAM_H_
#define CALL_AUDIO_RECEIVE_STREAM_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_factory_h //original-code:"api/audio_codecs/audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__call__transport_h //original-code:"api/call/transport.h"
#include BOSS_WEBRTC_U_api__optional_h //original-code:"api/optional.h"
#include BOSS_WEBRTC_U_api__rtpparameters_h //original-code:"api/rtpparameters.h"
#include BOSS_WEBRTC_U_api__rtpreceiverinterface_h //original-code:"api/rtpreceiverinterface.h"
#include BOSS_WEBRTC_U_call__rtp_config_h //original-code:"call/rtp_config.h"
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_rtc_base__scoped_ref_ptr_h //original-code:"rtc_base/scoped_ref_ptr.h"
#include BOSS_WEBRTC_U_typedefs_h //original-code:"typedefs.h"  // NOLINT(build/include)

namespace webrtc {
class AudioSinkInterface;

class AudioReceiveStream {
 public:
  struct Stats {
    uint32_t remote_ssrc = 0;
    int64_t bytes_rcvd = 0;
    uint32_t packets_rcvd = 0;
    uint32_t packets_lost = 0;
    float fraction_lost = 0.0f;
    std::string codec_name;
    rtc::Optional<int> codec_payload_type;
    uint32_t ext_seqnum = 0;
    uint32_t jitter_ms = 0;
    uint32_t jitter_buffer_ms = 0;
    uint32_t jitter_buffer_preferred_ms = 0;
    uint32_t delay_estimate_ms = 0;
    int32_t audio_level = -1;
    // Stats below correspond to similarly-named fields in the WebRTC stats
    // spec. https://w3c.github.io/webrtc-stats/#dom-rtcmediastreamtrackstats
    double total_output_energy = 0.0;
    uint64_t total_samples_received = 0;
    double total_output_duration = 0.0;
    uint64_t concealed_samples = 0;
    uint64_t concealment_events = 0;
    double jitter_buffer_delay_seconds = 0.0;
    // Stats below DO NOT correspond directly to anything in the WebRTC stats
    float expand_rate = 0.0f;
    float speech_expand_rate = 0.0f;
    float secondary_decoded_rate = 0.0f;
    float secondary_discarded_rate = 0.0f;
    float accelerate_rate = 0.0f;
    float preemptive_expand_rate = 0.0f;
    int32_t decoding_calls_to_silence_generator = 0;
    int32_t decoding_calls_to_neteq = 0;
    int32_t decoding_normal = 0;
    int32_t decoding_plc = 0;
    int32_t decoding_cng = 0;
    int32_t decoding_plc_cng = 0;
    int32_t decoding_muted_output = 0;
    int64_t capture_start_ntp_time_ms = 0;
  };

  struct Config {
    std::string ToString() const;

    // Receive-stream specific RTP settings.
    struct Rtp {
      std::string ToString() const;

      // Synchronization source (stream identifier) to be received.
      uint32_t remote_ssrc = 0;

      // Sender SSRC used for sending RTCP (such as receiver reports).
      uint32_t local_ssrc = 0;

      // Enable feedback for send side bandwidth estimation.
      // See
      // https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions
      // for details.
      bool transport_cc = false;

      // See NackConfig for description.
      NackConfig nack;

      // RTP header extensions used for the received stream.
      std::vector<RtpExtension> extensions;
    } rtp;

    Transport* rtcp_send_transport = nullptr;

    // TODO(solenberg): Remove once clients don't use it anymore.
    int voe_channel_id = -1;

    // NetEq settings.
    size_t jitter_buffer_max_packets = 50;
    bool jitter_buffer_fast_accelerate = false;

    // Identifier for an A/V synchronization group. Empty string to disable.
    // TODO(pbos): Synchronize streams in a sync group, not just one video
    // stream to one audio stream. Tracked by issue webrtc:4762.
    std::string sync_group;

    // Decoder specifications for every payload type that we can receive.
    std::map<int, SdpAudioFormat> decoder_map;

    rtc::scoped_refptr<AudioDecoderFactory> decoder_factory;
  };

  // Reconfigure the stream according to the Configuration.
  virtual void Reconfigure(const Config& config) = 0;

  // Starts stream activity.
  // When a stream is active, it can receive, process and deliver packets.
  virtual void Start() = 0;
  // Stops stream activity.
  // When a stream is stopped, it can't receive, process or deliver packets.
  virtual void Stop() = 0;

  virtual Stats GetStats() const = 0;
  // TODO(solenberg): Remove, once AudioMonitor is gone.
  virtual int GetOutputLevel() const = 0;

  // Sets an audio sink that receives unmixed audio from the receive stream.
  // Ownership of the sink is managed by the caller.
  // Only one sink can be set and passing a null sink clears an existing one.
  // NOTE: Audio must still somehow be pulled through AudioTransport for audio
  // to stream through this sink. In practice, this happens if mixed audio
  // is being pulled+rendered and/or if audio is being pulled for the purposes
  // of feeding to the AEC.
  virtual void SetSink(AudioSinkInterface* sink) = 0;

  // Sets playback gain of the stream, applied when mixing, and thus after it
  // is potentially forwarded to any attached AudioSinkInterface implementation.
  virtual void SetGain(float gain) = 0;

  virtual std::vector<RtpSource> GetSources() const = 0;

 protected:
  virtual ~AudioReceiveStream() {}
};
}  // namespace webrtc

#endif  // CALL_AUDIO_RECEIVE_STREAM_H_
