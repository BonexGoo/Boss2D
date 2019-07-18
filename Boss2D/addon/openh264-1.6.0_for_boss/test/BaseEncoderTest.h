#ifndef __BASEENCODERTEST_H__
#define __BASEENCODERTEST_H__

#include BOSS_OPENH264_U_codec_api_h //original-code:"codec_api.h"
#include BOSS_OPENH264_U_codec_app_def_h //original-code:"codec_app_def.h"
#include "utils/InputStream.h"

class BaseEncoderTest {
 public:
  struct Callback {
    virtual void onEncodeFrame (const SFrameBSInfo& frameInfo) = 0;
  };

  BaseEncoderTest();
  void SetUp();
  void TearDown();
  void EncodeFile (const char* fileName, SEncParamExt* pEncParamExt, Callback* cbk);
  void EncodeStream (InputStream* in,  SEncParamExt* pEncParamExt, Callback* cbk);

  ISVCEncoder* encoder_;
 private:

};

#endif //__BASEENCODERTEST_H__
