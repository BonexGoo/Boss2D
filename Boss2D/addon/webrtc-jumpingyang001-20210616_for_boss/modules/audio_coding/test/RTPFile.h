/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_TEST_RTPFILE_H_
#define MODULES_AUDIO_CODING_TEST_RTPFILE_H_

#include <stdio.h>

#include <queue>

#include BOSS_WEBRTC_U_api__rtp_headers_h //original-code:"api/rtp_headers.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class RTPStream {
 public:
  virtual ~RTPStream() {}

  virtual void Write(const uint8_t payloadType,
                     const uint32_t timeStamp,
                     const int16_t seqNo,
                     const uint8_t* payloadData,
                     const size_t payloadSize,
                     uint32_t frequency) = 0;

  // Returns the packet's payload size. Zero should be treated as an
  // end-of-stream (in the case that EndOfFile() is true) or an error.
  virtual size_t Read(RTPHeader* rtp_Header,
                      uint8_t* payloadData,
                      size_t payloadSize,
                      uint32_t* offset) = 0;
  virtual bool EndOfFile() const = 0;

 protected:
  void MakeRTPheader(uint8_t* rtpHeader,
                     uint8_t payloadType,
                     int16_t seqNo,
                     uint32_t timeStamp,
                     uint32_t ssrc);

  void ParseRTPHeader(RTPHeader* rtp_header, const uint8_t* rtpHeader);
};

class RTPPacket {
 public:
  RTPPacket(uint8_t payloadType,
            uint32_t timeStamp,
            int16_t seqNo,
            const uint8_t* payloadData,
            size_t payloadSize,
            uint32_t frequency);

  ~RTPPacket();

  uint8_t payloadType;
  uint32_t timeStamp;
  int16_t seqNo;
  uint8_t* payloadData;
  size_t payloadSize;
  uint32_t frequency;
};

class RTPBuffer : public RTPStream {
 public:
  RTPBuffer() = default;

  ~RTPBuffer() = default;

  void Write(const uint8_t payloadType,
             const uint32_t timeStamp,
             const int16_t seqNo,
             const uint8_t* payloadData,
             const size_t payloadSize,
             uint32_t frequency) override;

  size_t Read(RTPHeader* rtp_header,
              uint8_t* payloadData,
              size_t payloadSize,
              uint32_t* offset) override;

  bool EndOfFile() const override;

 private:
  mutable Mutex mutex_;
  std::queue<RTPPacket*> _rtpQueue RTC_GUARDED_BY(&mutex_);
};

class RTPFile : public RTPStream {
 public:
  ~RTPFile() {}

  RTPFile() : _rtpFile(NULL), _rtpEOF(false) {}

  void Open(const char* outFilename, const char* mode);

  void Close();

  void WriteHeader();

  void ReadHeader();

  void Write(const uint8_t payloadType,
             const uint32_t timeStamp,
             const int16_t seqNo,
             const uint8_t* payloadData,
             const size_t payloadSize,
             uint32_t frequency) override;

  size_t Read(RTPHeader* rtp_header,
              uint8_t* payloadData,
              size_t payloadSize,
              uint32_t* offset) override;

  bool EndOfFile() const override { return _rtpEOF; }

 private:
  FILE* _rtpFile;
  bool _rtpEOF;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_TEST_RTPFILE_H_
