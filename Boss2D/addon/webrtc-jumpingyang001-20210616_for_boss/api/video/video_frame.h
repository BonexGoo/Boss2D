/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_VIDEO_VIDEO_FRAME_H_
#define API_VIDEO_VIDEO_FRAME_H_

#include <stdint.h>

#include <utility>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__rtp_packet_infos_h //original-code:"api/rtp_packet_infos.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__video__color_space_h //original-code:"api/video/color_space.h"
#include BOSS_WEBRTC_U_api__video__hdr_metadata_h //original-code:"api/video/hdr_metadata.h"
#include BOSS_WEBRTC_U_api__video__video_frame_buffer_h //original-code:"api/video/video_frame_buffer.h"
#include BOSS_WEBRTC_U_api__video__video_rotation_h //original-code:"api/video/video_rotation.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"

namespace webrtc {

class RTC_EXPORT VideoFrame {
 public:
  struct RTC_EXPORT UpdateRect {
    int offset_x;
    int offset_y;
    int width;
    int height;

    // Makes this UpdateRect a bounding box of this and other rect.
    void Union(const UpdateRect& other);

    // Makes this UpdateRect an intersection of this and other rect.
    void Intersect(const UpdateRect& other);

    // Sets everything to 0, making this UpdateRect a zero-size (empty) update.
    void MakeEmptyUpdate();

    bool IsEmpty() const;

    // Per-member equality check. Empty rectangles with different offsets would
    // be considered different.
    bool operator==(const UpdateRect& other) const {
      return other.offset_x == offset_x && other.offset_y == offset_y &&
             other.width == width && other.height == height;
    }

    bool operator!=(const UpdateRect& other) const { return !(*this == other); }

    // Scales update_rect given original frame dimensions.
    // Cropping is applied first, then rect is scaled down.
    // Update rect is snapped to 2x2 grid due to possible UV subsampling and
    // then expanded by additional 2 pixels in each direction to accommodate any
    // possible scaling artifacts.
    // Note, close but not equal update_rects on original frame may result in
    // the same scaled update rects.
    UpdateRect ScaleWithFrame(int frame_width,
                              int frame_height,
                              int crop_x,
                              int crop_y,
                              int crop_width,
                              int crop_height,
                              int scaled_width,
                              int scaled_height) const;
  };

  struct RTC_EXPORT ProcessingTime {
    TimeDelta Elapsed() const { return finish - start; }
    Timestamp start;
    Timestamp finish;
  };

  // Preferred way of building VideoFrame objects.
  class RTC_EXPORT Builder {
   public:
    Builder();
    ~Builder();

    VideoFrame build();
    Builder& set_video_frame_buffer(
        const rtc::scoped_refptr<VideoFrameBuffer>& buffer);
    Builder& set_timestamp_ms(int64_t timestamp_ms);
    Builder& set_timestamp_us(int64_t timestamp_us);
    Builder& set_timestamp_rtp(uint32_t timestamp_rtp);
    Builder& set_ntp_time_ms(int64_t ntp_time_ms);
    Builder& set_rotation(VideoRotation rotation);
    Builder& set_color_space(const absl::optional<ColorSpace>& color_space);
    Builder& set_color_space(const ColorSpace* color_space);
    Builder& set_id(uint16_t id);
    Builder& set_update_rect(const absl::optional<UpdateRect>& update_rect);
    Builder& set_packet_infos(RtpPacketInfos packet_infos);

   private:
    uint16_t id_ = 0;
    rtc::scoped_refptr<webrtc::VideoFrameBuffer> video_frame_buffer_;
    int64_t timestamp_us_ = 0;
    uint32_t timestamp_rtp_ = 0;
    int64_t ntp_time_ms_ = 0;
    VideoRotation rotation_ = kVideoRotation_0;
    absl::optional<ColorSpace> color_space_;
    absl::optional<UpdateRect> update_rect_;
    RtpPacketInfos packet_infos_;
  };

  // To be deprecated. Migrate all use to Builder.
  VideoFrame(const rtc::scoped_refptr<VideoFrameBuffer>& buffer,
             webrtc::VideoRotation rotation,
             int64_t timestamp_us);
  VideoFrame(const rtc::scoped_refptr<VideoFrameBuffer>& buffer,
             uint32_t timestamp_rtp,
             int64_t render_time_ms,
             VideoRotation rotation);

  ~VideoFrame();

  // Support move and copy.
  VideoFrame(const VideoFrame&);
  VideoFrame(VideoFrame&&);
  VideoFrame& operator=(const VideoFrame&);
  VideoFrame& operator=(VideoFrame&&);

  // Get frame width.
  int width() const;
  // Get frame height.
  int height() const;
  // Get frame size in pixels.
  uint32_t size() const;

  // Get frame ID. Returns 0 if ID is not set. Not guaranteed to be transferred
  // from the sender to the receiver, but preserved on the sender side. The id
  // should be propagated between all frame modifications during its lifetime
  // from capturing to sending as encoded image. It is intended to be unique
  // over a time window of a few minutes for the peer connection to which the
  // corresponding video stream belongs to.
  uint16_t id() const { return id_; }
  void set_id(uint16_t id) { id_ = id; }

  // System monotonic clock, same timebase as rtc::TimeMicros().
  int64_t timestamp_us() const { return timestamp_us_; }
  void set_timestamp_us(int64_t timestamp_us) { timestamp_us_ = timestamp_us; }

  // TODO(nisse): After the cricket::VideoFrame and webrtc::VideoFrame
  // merge, timestamps other than timestamp_us will likely be
  // deprecated.

  // Set frame timestamp (90kHz).
  void set_timestamp(uint32_t timestamp) { timestamp_rtp_ = timestamp; }

  // Get frame timestamp (90kHz).
  uint32_t timestamp() const { return timestamp_rtp_; }

  // For now, transport_frame_id and rtp timestamp are the same.
  // TODO(nisse): Must be handled differently for QUIC.
  uint32_t transport_frame_id() const { return timestamp(); }

  // Set capture ntp time in milliseconds.
  void set_ntp_time_ms(int64_t ntp_time_ms) { ntp_time_ms_ = ntp_time_ms; }

  // Get capture ntp time in milliseconds.
  int64_t ntp_time_ms() const { return ntp_time_ms_; }

  // Naming convention for Coordination of Video Orientation. Please see
  // http://www.etsi.org/deliver/etsi_ts/126100_126199/126114/12.07.00_60/ts_126114v120700p.pdf
  //
  // "pending rotation" or "pending" = a frame that has a VideoRotation > 0.
  //
  // "not pending" = a frame that has a VideoRotation == 0.
  //
  // "apply rotation" = modify a frame from being "pending" to being "not
  //                    pending" rotation (a no-op for "unrotated").
  //
  VideoRotation rotation() const { return rotation_; }
  void set_rotation(VideoRotation rotation) { rotation_ = rotation; }

  // Get color space when available.
  const absl::optional<ColorSpace>& color_space() const { return color_space_; }
  void set_color_space(const absl::optional<ColorSpace>& color_space) {
    color_space_ = color_space;
  }

  // max_composition_delay_in_frames() is used in an experiment of a low-latency
  // renderer algorithm see crbug.com/1138888.
  absl::optional<int32_t> max_composition_delay_in_frames() const {
    return max_composition_delay_in_frames_;
  }
  void set_max_composition_delay_in_frames(
      absl::optional<int32_t> max_composition_delay_in_frames) {
    max_composition_delay_in_frames_ = max_composition_delay_in_frames;
  }

  // Get render time in milliseconds.
  // TODO(nisse): Deprecated. Migrate all users to timestamp_us().
  int64_t render_time_ms() const;

  // Return the underlying buffer. Never nullptr for a properly
  // initialized VideoFrame.
  rtc::scoped_refptr<webrtc::VideoFrameBuffer> video_frame_buffer() const;

  void set_video_frame_buffer(
      const rtc::scoped_refptr<VideoFrameBuffer>& buffer);

  // TODO(nisse): Deprecated.
  // Return true if the frame is stored in a texture.
  bool is_texture() const {
    return video_frame_buffer()->type() == VideoFrameBuffer::Type::kNative;
  }

  bool has_update_rect() const { return update_rect_.has_value(); }

  // Returns update_rect set by the builder or set_update_rect() or whole frame
  // rect if no update rect is available.
  UpdateRect update_rect() const {
    return update_rect_.value_or(UpdateRect{0, 0, width(), height()});
  }

  // Rectangle must be within the frame dimensions.
  void set_update_rect(const VideoFrame::UpdateRect& update_rect) {
    RTC_DCHECK_GE(update_rect.offset_x, 0);
    RTC_DCHECK_GE(update_rect.offset_y, 0);
    RTC_DCHECK_LE(update_rect.offset_x + update_rect.width, width());
    RTC_DCHECK_LE(update_rect.offset_y + update_rect.height, height());
    update_rect_ = update_rect;
  }

  void clear_update_rect() { update_rect_ = absl::nullopt; }

  // Get information about packets used to assemble this video frame. Might be
  // empty if the information isn't available.
  const RtpPacketInfos& packet_infos() const { return packet_infos_; }
  void set_packet_infos(RtpPacketInfos value) {
    packet_infos_ = std::move(value);
  }

  const absl::optional<ProcessingTime> processing_time() const {
    return processing_time_;
  }
  void set_processing_time(const ProcessingTime& processing_time) {
    processing_time_ = processing_time;
  }

 private:
  VideoFrame(uint16_t id,
             const rtc::scoped_refptr<VideoFrameBuffer>& buffer,
             int64_t timestamp_us,
             uint32_t timestamp_rtp,
             int64_t ntp_time_ms,
             VideoRotation rotation,
             const absl::optional<ColorSpace>& color_space,
             const absl::optional<UpdateRect>& update_rect,
             RtpPacketInfos packet_infos);

  uint16_t id_;
  // An opaque reference counted handle that stores the pixel data.
  rtc::scoped_refptr<webrtc::VideoFrameBuffer> video_frame_buffer_;
  uint32_t timestamp_rtp_;
  int64_t ntp_time_ms_;
  int64_t timestamp_us_;
  VideoRotation rotation_;
  absl::optional<ColorSpace> color_space_;
  absl::optional<int32_t> max_composition_delay_in_frames_;
  // Updated since the last frame area. If present it means that the bounding
  // box of all the changes is within the rectangular area and is close to it.
  // If absent, it means that there's no information about the change at all and
  // update_rect() will return a rectangle corresponding to the entire frame.
  absl::optional<UpdateRect> update_rect_;
  // Information about packets used to assemble this video frame. This is needed
  // by |SourceTracker| when the frame is delivered to the RTCRtpReceiver's
  // MediaStreamTrack, in order to implement getContributingSources(). See:
  // https://w3c.github.io/webrtc-pc/#dom-rtcrtpreceiver-getcontributingsources
  RtpPacketInfos packet_infos_;
  // Processing timestamps of the frame. For received video frames these are the
  // timestamps when the frame is sent to the decoder and the decoded image
  // returned from the decoder.
  // Currently, not set for locally captured video frames.
  absl::optional<ProcessingTime> processing_time_;
};

}  // namespace webrtc

#endif  // API_VIDEO_VIDEO_FRAME_H_
