/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_NETEQ_SYNC_BUFFER_H_
#define MODULES_AUDIO_CODING_NETEQ_SYNC_BUFFER_H_

#include BOSS_WEBRTC_U_api__audio__audio_frame_h //original-code:"api/audio/audio_frame.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__audio_multi_vector_h //original-code:"modules/audio_coding/neteq/audio_multi_vector.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

class SyncBuffer : public AudioMultiVector {
 public:
  SyncBuffer(size_t channels, size_t length)
      : AudioMultiVector(channels, length),
        next_index_(length),
        end_timestamp_(0),
        dtmf_index_(0) {}

  // Returns the number of samples yet to play out from the buffer.
  size_t FutureLength() const;

  // Adds the contents of |append_this| to the back of the SyncBuffer. Removes
  // the same number of samples from the beginning of the SyncBuffer, to
  // maintain a constant buffer size. The |next_index_| is updated to reflect
  // the move of the beginning of "future" data.
  void PushBack(const AudioMultiVector& append_this) override;

  // Adds |length| zeros to the beginning of each channel. Removes
  // the same number of samples from the end of the SyncBuffer, to
  // maintain a constant buffer size. The |next_index_| is updated to reflect
  // the move of the beginning of "future" data.
  // Note that this operation may delete future samples that are waiting to
  // be played.
  void PushFrontZeros(size_t length);

  // Inserts |length| zeros into each channel at index |position|. The size of
  // the SyncBuffer is kept constant, which means that the last |length|
  // elements in each channel will be purged.
  virtual void InsertZerosAtIndex(size_t length, size_t position);

  // Overwrites each channel in this SyncBuffer with values taken from
  // |insert_this|. The values are taken from the beginning of |insert_this| and
  // are inserted starting at |position|. |length| values are written into each
  // channel. The size of the SyncBuffer is kept constant. That is, if |length|
  // and |position| are selected such that the new data would extend beyond the
  // end of the current SyncBuffer, the buffer is not extended.
  // The |next_index_| is not updated.
  virtual void ReplaceAtIndex(const AudioMultiVector& insert_this,
                              size_t length,
                              size_t position);

  // Same as the above method, but where all of |insert_this| is written (with
  // the same constraints as above, that the SyncBuffer is not extended).
  virtual void ReplaceAtIndex(const AudioMultiVector& insert_this,
                              size_t position);

  // Reads |requested_len| samples from each channel and writes them interleaved
  // into |output|. The |next_index_| is updated to point to the sample to read
  // next time. The AudioFrame |output| is first reset, and the |data_|,
  // |num_channels_|, and |samples_per_channel_| fields are updated.
  void GetNextAudioInterleaved(size_t requested_len, AudioFrame* output);

  // Adds |increment| to |end_timestamp_|.
  void IncreaseEndTimestamp(uint32_t increment);

  // Flushes the buffer. The buffer will contain only zeros after the flush, and
  // |next_index_| will point to the end, like when the buffer was first
  // created.
  void Flush();

  const AudioVector& Channel(size_t n) const { return *channels_[n]; }
  AudioVector& Channel(size_t n) { return *channels_[n]; }

  // Accessors and mutators.
  size_t next_index() const { return next_index_; }
  void set_next_index(size_t value);
  uint32_t end_timestamp() const { return end_timestamp_; }
  void set_end_timestamp(uint32_t value) { end_timestamp_ = value; }
  size_t dtmf_index() const { return dtmf_index_; }
  void set_dtmf_index(size_t value);

 private:
  size_t next_index_;
  uint32_t end_timestamp_;  // The timestamp of the last sample in the buffer.
  size_t dtmf_index_;       // Index to the first non-DTMF sample in the buffer.

  RTC_DISALLOW_COPY_AND_ASSIGN(SyncBuffer);
};

}  // namespace webrtc
#endif  // MODULES_AUDIO_CODING_NETEQ_SYNC_BUFFER_H_
