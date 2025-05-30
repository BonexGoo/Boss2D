/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "sdk/android/src/jni/videoframe.h"

#include <memory>

#include BOSS_WEBRTC_U_common_video__include__video_frame_buffer_h //original-code:"common_video/include/video_frame_buffer.h"
#include BOSS_WEBRTC_U_rtc_base__bind_h //original-code:"rtc_base/bind.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__keep_ref_until_done_h //original-code:"rtc_base/keep_ref_until_done.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__scoped_ref_ptr_h //original-code:"rtc_base/scoped_ref_ptr.h"
#include BOSS_WEBRTC_U_rtc_base__timeutils_h //original-code:"rtc_base/timeutils.h"
#include "sdk/android/generated_video_jni/jni/VideoFrame_jni.h"
#include "sdk/android/src/jni/jni_helpers.h"
#include "sdk/android/src/jni/surfacetexturehelper.h"
#include "sdk/android/src/jni/wrapped_native_i420_buffer.h"
#include BOSS_WEBRTC_U_system_wrappers__include__aligned_malloc_h //original-code:"system_wrappers/include/aligned_malloc.h"
#include "third_party/libyuv/include/libyuv/scale.h"

namespace webrtc {
namespace jni {

namespace {

class AndroidVideoI420Buffer : public I420BufferInterface {
 public:
  // Adopts and takes ownership of the Java VideoFrame.Buffer. I.e. retain()
  // will not be called, but release() will be called when the returned
  // AndroidVideoBuffer is destroyed.
  static rtc::scoped_refptr<AndroidVideoI420Buffer> Adopt(
      JNIEnv* jni,
      int width,
      int height,
      const JavaRef<jobject>& j_video_frame_buffer);

 protected:
  // Should not be called directly. Adopts the buffer. Use Adopt() instead for
  // clarity.
  AndroidVideoI420Buffer(JNIEnv* jni,
                         int width,
                         int height,
                         const JavaRef<jobject>& j_video_frame_buffer);
  ~AndroidVideoI420Buffer();

 private:
  const uint8_t* DataY() const override { return data_y_; }
  const uint8_t* DataU() const override { return data_u_; }
  const uint8_t* DataV() const override { return data_v_; }

  int StrideY() const override { return stride_y_; }
  int StrideU() const override { return stride_u_; }
  int StrideV() const override { return stride_v_; }

  int width() const override { return width_; }
  int height() const override { return height_; }

  const int width_;
  const int height_;
  // Holds a VideoFrame.I420Buffer.
  const ScopedJavaGlobalRef<jobject> j_video_frame_buffer_;

  const uint8_t* data_y_;
  const uint8_t* data_u_;
  const uint8_t* data_v_;
  int stride_y_;
  int stride_u_;
  int stride_v_;
};

rtc::scoped_refptr<AndroidVideoI420Buffer> AndroidVideoI420Buffer::Adopt(
    JNIEnv* jni,
    int width,
    int height,
    const JavaRef<jobject>& j_video_frame_buffer) {
  return new rtc::RefCountedObject<AndroidVideoI420Buffer>(
      jni, width, height, j_video_frame_buffer);
}

AndroidVideoI420Buffer::AndroidVideoI420Buffer(
    JNIEnv* jni,
    int width,
    int height,
    const JavaRef<jobject>& j_video_frame_buffer)
    : width_(width),
      height_(height),
      j_video_frame_buffer_(jni, j_video_frame_buffer) {
  ScopedJavaLocalRef<jobject> j_data_y =
      Java_I420Buffer_getDataY(jni, j_video_frame_buffer);
  ScopedJavaLocalRef<jobject> j_data_u =
      Java_I420Buffer_getDataU(jni, j_video_frame_buffer);
  ScopedJavaLocalRef<jobject> j_data_v =
      Java_I420Buffer_getDataV(jni, j_video_frame_buffer);

  data_y_ =
      static_cast<const uint8_t*>(jni->GetDirectBufferAddress(j_data_y.obj()));
  data_u_ =
      static_cast<const uint8_t*>(jni->GetDirectBufferAddress(j_data_u.obj()));
  data_v_ =
      static_cast<const uint8_t*>(jni->GetDirectBufferAddress(j_data_v.obj()));

  stride_y_ = Java_I420Buffer_getStrideY(jni, j_video_frame_buffer);
  stride_u_ = Java_I420Buffer_getStrideU(jni, j_video_frame_buffer);
  stride_v_ = Java_I420Buffer_getStrideV(jni, j_video_frame_buffer);
}

AndroidVideoI420Buffer::~AndroidVideoI420Buffer() {
  JNIEnv* jni = AttachCurrentThreadIfNeeded();
  Java_Buffer_release(jni, j_video_frame_buffer_);
}

}  // namespace

int64_t GetJavaVideoFrameTimestampNs(JNIEnv* jni,
                                     const JavaRef<jobject>& j_video_frame) {
  return Java_VideoFrame_getTimestampNs(jni, j_video_frame);
}

Matrix::Matrix(JNIEnv* jni, const JavaRef<jfloatArray>& a) {
  RTC_CHECK_EQ(16, jni->GetArrayLength(a.obj()));
  jfloat* ptr = jni->GetFloatArrayElements(a.obj(), nullptr);
  for (int i = 0; i < 16; ++i) {
    elem_[i] = ptr[i];
  }
  jni->ReleaseFloatArrayElements(a.obj(), ptr, 0);
}

ScopedJavaLocalRef<jfloatArray> Matrix::ToJava(JNIEnv* jni) const {
  ScopedJavaLocalRef<jfloatArray> matrix(jni, jni->NewFloatArray(16));
  jni->SetFloatArrayRegion(matrix.obj(), 0, 16, elem_);
  return matrix;
}

void Matrix::Rotate(VideoRotation rotation) {
  // Texture coordinates are in the range 0 to 1. The transformation of the last
  // row in each rotation matrix is needed for proper translation, e.g, to
  // mirror x, we don't replace x by -x, but by 1-x.
  switch (rotation) {
    case kVideoRotation_0:
      break;
    case kVideoRotation_90: {
      const float ROTATE_90[16] =
          { elem_[4], elem_[5], elem_[6], elem_[7],
            -elem_[0], -elem_[1], -elem_[2], -elem_[3],
            elem_[8], elem_[9], elem_[10], elem_[11],
            elem_[0] + elem_[12], elem_[1] + elem_[13],
            elem_[2] + elem_[14], elem_[3] + elem_[15]};
      memcpy(elem_, ROTATE_90, sizeof(elem_));
    } break;
    case kVideoRotation_180: {
      const float ROTATE_180[16] =
          { -elem_[0], -elem_[1], -elem_[2], -elem_[3],
            -elem_[4], -elem_[5], -elem_[6], -elem_[7],
            elem_[8], elem_[9], elem_[10], elem_[11],
            elem_[0] + elem_[4] + elem_[12], elem_[1] + elem_[5] + elem_[13],
            elem_[2] + elem_[6] + elem_[14], elem_[3] + elem_[11]+ elem_[15]};
        memcpy(elem_, ROTATE_180, sizeof(elem_));
    } break;
    case kVideoRotation_270: {
      const float ROTATE_270[16] =
          { -elem_[4], -elem_[5], -elem_[6], -elem_[7],
            elem_[0], elem_[1], elem_[2], elem_[3],
            elem_[8], elem_[9], elem_[10], elem_[11],
            elem_[4] + elem_[12], elem_[5] + elem_[13],
            elem_[6] + elem_[14], elem_[7] + elem_[15]};
      memcpy(elem_, ROTATE_270, sizeof(elem_));
    } break;
  }
}

// Calculates result = a * b, in column-major order.
void Matrix::Multiply(const float a[16], const float b[16], float result[16]) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      float sum = 0;
      for (int k = 0; k < 4; ++k) {
        sum += a[k * 4 + j] * b[i * 4 + k];
      }
      result[i * 4 + j] = sum;
    }
  }
}

// Center crop by keeping xFraction of the width and yFraction of the height,
// so e.g. cropping from 640x480 to 640x360 would use
// xFraction=1, yFraction=360/480.
void Matrix::Crop(float xFraction,
                  float yFraction,
                  float xOffset,
                  float yOffset) {
  const float crop_matrix[16] =
      {xFraction, 0, 0, 0,
       0, yFraction, 0, 0,
       0, 0, 1, 0,
       xOffset, yOffset, 0, 1};
  const Matrix old = *this;
  Multiply(crop_matrix, old.elem_, this->elem_);
}

// Aligning pointer to 64 bytes for improved performance, e.g. use SIMD.
static const int kBufferAlignment = 64;

NativeHandleImpl::NativeHandleImpl(int id, const Matrix& matrix)
    : oes_texture_id(id), sampling_matrix(matrix) {}

NativeHandleImpl::NativeHandleImpl(
    JNIEnv* jni,
    jint j_oes_texture_id,
    const JavaRef<jfloatArray>& j_transform_matrix)
    : oes_texture_id(j_oes_texture_id),
      sampling_matrix(jni, j_transform_matrix) {}

AndroidTextureBuffer::AndroidTextureBuffer(
    int width,
    int height,
    const NativeHandleImpl& native_handle,
    const rtc::scoped_refptr<SurfaceTextureHelper>& surface_texture_helper)
    : width_(width),
      height_(height),
      native_handle_(native_handle),
      surface_texture_helper_(surface_texture_helper) {}

AndroidTextureBuffer::~AndroidTextureBuffer() {
  surface_texture_helper_->ReturnTextureFrame();
}

VideoFrameBuffer::Type AndroidTextureBuffer::type() const {
  return Type::kNative;
}

NativeHandleImpl AndroidTextureBuffer::native_handle_impl() const {
  return native_handle_;
}

int AndroidTextureBuffer::width() const {
  return width_;
}

int AndroidTextureBuffer::height() const {
  return height_;
}

rtc::scoped_refptr<I420BufferInterface> AndroidTextureBuffer::ToI420() {
  int uv_width = (width() + 7) / 8;
  int stride = 8 * uv_width;
  int uv_height = (height() + 1) / 2;
  size_t size = stride * (height() + uv_height);
  // The data is owned by the frame, and the normal case is that the
  // data is deleted by the frame's destructor callback.
  //
  // TODO(nisse): Use an I420BufferPool. We then need to extend that
  // class, and I420Buffer, to support our memory layout.
  // TODO(nisse): Depending on
  // system_wrappers/include/aligned_malloc.h violate current DEPS
  // rules. We get away for now only because it is indirectly included
  // by i420_buffer.h
  std::unique_ptr<uint8_t, AlignedFreeDeleter> yuv_data(
      static_cast<uint8_t*>(AlignedMalloc(size, kBufferAlignment)));
  // See YuvConverter.java for the required layout.
  uint8_t* y_data = yuv_data.get();
  uint8_t* u_data = y_data + height() * stride;
  uint8_t* v_data = u_data + stride / 2;

  rtc::scoped_refptr<I420BufferInterface> copy = webrtc::WrapI420Buffer(
      width(), height(), y_data, stride, u_data, stride, v_data, stride,
      rtc::Bind(&AlignedFree, yuv_data.release()));

  JNIEnv* jni = AttachCurrentThreadIfNeeded();

  // TODO(sakal): This call to a deperecated method will be removed when
  // AndroidTextureBuffer is removed.
  ScopedJavaLocalRef<jobject> byte_buffer =
      NewDirectByteBuffer(jni, y_data, size);
  SurfaceTextureHelperTextureToYUV(
      jni, surface_texture_helper_->GetJavaSurfaceTextureHelper(), byte_buffer,
      width(), height(), stride, native_handle_);
  return copy;
}

rtc::scoped_refptr<AndroidVideoBuffer> AndroidVideoBuffer::Adopt(
    JNIEnv* jni,
    const JavaRef<jobject>& j_video_frame_buffer) {
  return new rtc::RefCountedObject<AndroidVideoBuffer>(jni,
                                                       j_video_frame_buffer);
}

rtc::scoped_refptr<AndroidVideoBuffer> AndroidVideoBuffer::Create(
    JNIEnv* jni,
    const JavaRef<jobject>& j_video_frame_buffer) {
  Java_Buffer_retain(jni, j_video_frame_buffer);
  return Adopt(jni, j_video_frame_buffer);
}

AndroidVideoBuffer::AndroidVideoBuffer(
    JNIEnv* jni,
    const JavaRef<jobject>& j_video_frame_buffer)
    : width_(Java_Buffer_getWidth(jni, j_video_frame_buffer)),
      height_(Java_Buffer_getHeight(jni, j_video_frame_buffer)),
      j_video_frame_buffer_(jni, j_video_frame_buffer) {}

AndroidVideoBuffer::~AndroidVideoBuffer() {
  JNIEnv* jni = AttachCurrentThreadIfNeeded();
  Java_Buffer_release(jni, j_video_frame_buffer_);
}

const ScopedJavaGlobalRef<jobject>& AndroidVideoBuffer::video_frame_buffer()
    const {
  return j_video_frame_buffer_;
}

rtc::scoped_refptr<AndroidVideoBuffer> AndroidVideoBuffer::CropAndScale(
    JNIEnv* jni,
    int crop_x,
    int crop_y,
    int crop_width,
    int crop_height,
    int scale_width,
    int scale_height) {
  return Adopt(jni, Java_Buffer_cropAndScale(jni, j_video_frame_buffer_, crop_x,
                                             crop_y, crop_width, crop_height,
                                             scale_width, scale_height));
}

VideoFrameBuffer::Type AndroidVideoBuffer::type() const {
  return Type::kNative;
}

int AndroidVideoBuffer::width() const {
  return width_;
}

int AndroidVideoBuffer::height() const {
  return height_;
}

rtc::scoped_refptr<I420BufferInterface> AndroidVideoBuffer::ToI420() {
  JNIEnv* jni = AttachCurrentThreadIfNeeded();
  ScopedJavaLocalRef<jobject> j_i420_buffer =
      Java_Buffer_toI420(jni, j_video_frame_buffer_);

  // We don't need to retain the buffer because toI420 returns a new object that
  // we are assumed to take the ownership of.
  return AndroidVideoI420Buffer::Adopt(jni, width_, height_, j_i420_buffer);
}

VideoFrame JavaToNativeFrame(JNIEnv* jni,
                             const JavaRef<jobject>& j_video_frame,
                             uint32_t timestamp_rtp) {
  ScopedJavaLocalRef<jobject> j_video_frame_buffer =
      Java_VideoFrame_getBuffer(jni, j_video_frame);
  int rotation = Java_VideoFrame_getRotation(jni, j_video_frame);
  int64_t timestamp_ns = Java_VideoFrame_getTimestampNs(jni, j_video_frame);
  rtc::scoped_refptr<AndroidVideoBuffer> buffer =
      AndroidVideoBuffer::Create(jni, j_video_frame_buffer);
  return VideoFrame(buffer, timestamp_rtp,
                    timestamp_ns / rtc::kNumNanosecsPerMillisec,
                    static_cast<VideoRotation>(rotation));
}

static bool IsJavaVideoBuffer(rtc::scoped_refptr<VideoFrameBuffer> buffer) {
  if (buffer->type() != VideoFrameBuffer::Type::kNative) {
    return false;
  }
  AndroidVideoFrameBuffer* android_buffer =
      static_cast<AndroidVideoFrameBuffer*>(buffer.get());
  return android_buffer->android_type() ==
         AndroidVideoFrameBuffer::AndroidType::kJavaBuffer;
}

ScopedJavaLocalRef<jobject> NativeToJavaFrame(JNIEnv* jni,
                                              const VideoFrame& frame) {
  rtc::scoped_refptr<VideoFrameBuffer> buffer = frame.video_frame_buffer();

  if (IsJavaVideoBuffer(buffer)) {
    RTC_DCHECK(buffer->type() == VideoFrameBuffer::Type::kNative);
    AndroidVideoFrameBuffer* android_buffer =
        static_cast<AndroidVideoFrameBuffer*>(buffer.get());
    RTC_DCHECK(android_buffer->android_type() ==
               AndroidVideoFrameBuffer::AndroidType::kJavaBuffer);
    AndroidVideoBuffer* android_video_buffer =
        static_cast<AndroidVideoBuffer*>(android_buffer);

    return Java_VideoFrame_Constructor(
        jni, android_video_buffer->video_frame_buffer(),
        static_cast<jint>(frame.rotation()),
        static_cast<jlong>(frame.timestamp_us() *
                           rtc::kNumNanosecsPerMicrosec));
  } else {
    return Java_VideoFrame_Constructor(
        jni, WrapI420Buffer(jni, buffer->ToI420()),
        static_cast<jint>(frame.rotation()),
        static_cast<jlong>(frame.timestamp_us() *
                           rtc::kNumNanosecsPerMicrosec));
  }
}

static void JNI_VideoFrame_CropAndScaleI420(
    JNIEnv* jni,
    const JavaParamRef<jclass>&,
    const JavaParamRef<jobject>& j_src_y,
    jint src_stride_y,
    const JavaParamRef<jobject>& j_src_u,
    jint src_stride_u,
    const JavaParamRef<jobject>& j_src_v,
    jint src_stride_v,
    jint crop_x,
    jint crop_y,
    jint crop_width,
    jint crop_height,
    const JavaParamRef<jobject>& j_dst_y,
    jint dst_stride_y,
    const JavaParamRef<jobject>& j_dst_u,
    jint dst_stride_u,
    const JavaParamRef<jobject>& j_dst_v,
    jint dst_stride_v,
    jint scale_width,
    jint scale_height) {
  uint8_t const* src_y =
      static_cast<uint8_t*>(jni->GetDirectBufferAddress(j_src_y.obj()));
  uint8_t const* src_u =
      static_cast<uint8_t*>(jni->GetDirectBufferAddress(j_src_u.obj()));
  uint8_t const* src_v =
      static_cast<uint8_t*>(jni->GetDirectBufferAddress(j_src_v.obj()));
  uint8_t* dst_y =
      static_cast<uint8_t*>(jni->GetDirectBufferAddress(j_dst_y.obj()));
  uint8_t* dst_u =
      static_cast<uint8_t*>(jni->GetDirectBufferAddress(j_dst_u.obj()));
  uint8_t* dst_v =
      static_cast<uint8_t*>(jni->GetDirectBufferAddress(j_dst_v.obj()));

  // Perform cropping using pointer arithmetic.
  src_y += crop_x + crop_y * src_stride_y;
  src_u += crop_x / 2 + crop_y / 2 * src_stride_u;
  src_v += crop_x / 2 + crop_y / 2 * src_stride_v;

  bool ret = libyuv::I420Scale(
      src_y, src_stride_y, src_u, src_stride_u, src_v, src_stride_v, crop_width,
      crop_height, dst_y, dst_stride_y, dst_u, dst_stride_u, dst_v,
      dst_stride_v, scale_width, scale_height, libyuv::kFilterBox);
  RTC_DCHECK_EQ(ret, 0) << "I420Scale failed";
}

}  // namespace jni
}  // namespace webrtc
