#include <gtest/gtest.h>
#include BOSS_OPENH264_U_codec_def_h //original-code:"codec_def.h"
#include "utils/HashFunctions.h"
#include "utils/BufferedData.h"
#include "utils/InputStream.h"
#include "BaseDecoderTest.h"
#include "BaseEncoderTest.h"
#include <string>
static void UpdateHashFromFrame (const SFrameBSInfo& info, SHA1Context* ctx) {
  for (int i = 0; i < info.iLayerNum; ++i) {
    const SLayerBSInfo& layerInfo = info.sLayerInfo[i];
    int layerSize = 0;
    for (int j = 0; j < layerInfo.iNalCount; ++j) {
      layerSize += layerInfo.pNalLengthInByte[j];
    }
    SHA1Input (ctx, layerInfo.pBsBuf, layerSize);
  }
}

static void WritePlaneBuffer (BufferedData* buf, const uint8_t* plane,
                              int width, int height, int stride) {
  for (int i = 0; i < height; i++) {
    if (!buf->PushBack (plane, width)) {
      FAIL() << "unable to allocate memory";
    }
    plane += stride;
  }
}

struct DecodeEncodeFileParam {
  const char* fileName;
  const char* hashStr;
  int width;
  int height;
  float frameRate;
};

class DecodeEncodeTest : public ::testing::TestWithParam<DecodeEncodeFileParam>,
  public BaseDecoderTest, public BaseDecoderTest::Callback,
  public BaseEncoderTest , public BaseEncoderTest::Callback,
  public InputStream {
 public:
  virtual void SetUp() {
    BaseDecoderTest::SetUp();
    if (HasFatalFailure()) {
      return;
    }
    BaseEncoderTest::SetUp();
    if (HasFatalFailure()) {
      return;
    }
    SHA1Reset (&ctx_);
  }

  virtual void TearDown() {
    BaseDecoderTest::TearDown();
    BaseEncoderTest::TearDown();
  }

  virtual void onDecodeFrame (const Frame& frame) {
    const Plane& y = frame.y;
    const Plane& u = frame.u;
    const Plane& v = frame.v;
    WritePlaneBuffer (&buf_, y.data, y.width, y.height, y.stride);
    WritePlaneBuffer (&buf_, u.data, u.width, u.height, u.stride);
    WritePlaneBuffer (&buf_, v.data, v.width, v.height, v.stride);
  }

  virtual void onEncodeFrame (const SFrameBSInfo& frameInfo) {
    UpdateHashFromFrame (frameInfo, &ctx_);
  }

  virtual int read (void* ptr, size_t len) {
    while (buf_.Length() < len) {
      bool hasNext = DecodeNextFrame (this);
      if (HasFatalFailure()) {
        return -1;
      }
      if (!hasNext) {
        if (buf_.Length() == 0) {
          return -1;
        }
        break;
      }
    }
    return (int) buf_.PopFront (static_cast<uint8_t*> (ptr), len);
  }

 protected:
  SHA1Context ctx_;
  BufferedData buf_;
};

void DecEncFileParamToParamExt (DecodeEncodeFileParam* pDecEncFileParam, SEncParamExt* pEnxParamExt) {
  ASSERT_TRUE (NULL != pDecEncFileParam && NULL != pEnxParamExt);

  pEnxParamExt->iPicWidth      = pDecEncFileParam->width;
  pEnxParamExt->iPicHeight     = pDecEncFileParam->height;
  pEnxParamExt->fMaxFrameRate  = pDecEncFileParam->frameRate;

  //default value
  pEnxParamExt->iUsageType        = CAMERA_VIDEO_REAL_TIME;
  pEnxParamExt->iSpatialLayerNum  = 1;
  pEnxParamExt->bEnableDenoise    = false;
  pEnxParamExt->bIsLosslessLink   = false;
  pEnxParamExt->bEnableLongTermReference  = false;
  pEnxParamExt->iEntropyCodingModeFlag    = 0;

  for (int i = 0; i < pEnxParamExt->iSpatialLayerNum; i++) {
    pEnxParamExt->sSpatialLayers[i].sSliceArgument.uiSliceMode = SM_SINGLE_SLICE;
  }

}
TEST_P (DecodeEncodeTest, CompareOutput) {
  DecodeEncodeFileParam p = GetParam();
  SEncParamExt  EnxParamExt;
  DecEncFileParamToParamExt (&p, &EnxParamExt);

#if defined(ANDROID_NDK)
  std::string filename = std::string ("/sdcard/") + p.fileName;
  ASSERT_TRUE (Open (filename.c_str()));
#else
  ASSERT_TRUE (Open (p.fileName));
#endif
  EncodeStream (this, &EnxParamExt, this);
  unsigned char digest[SHA_DIGEST_LENGTH];
  SHA1Result (&ctx_, digest);
  if (!HasFatalFailure()) {
    CompareHash (digest, p.hashStr);
  }
}
static const DecodeEncodeFileParam kFileParamArray[] = {
  {"res/test_vd_1d.264", "cb3ec7a1bf37d0c08118f00009befb6f11dace3c", 320, 192, 12.0f},
  {"res/test_vd_rc.264", "f231547ee7a3e6e4f1a05425280c41f285df390c", 320, 192, 12.0f},
};


INSTANTIATE_TEST_CASE_P (DecodeEncodeFile, DecodeEncodeTest,
                         ::testing::ValuesIn (kFileParamArray));
