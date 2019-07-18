/*!
 * \copy
 *     Copyright (c)  2013, Cisco Systems
 *     All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions
 *     are met:
 *
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in
 *          the documentation and/or other materials provided with the
 *          distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *     "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *     COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *     BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *     CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *     ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *     POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef WELS_CODEC_TRACE
#define WELS_CODEC_TRACE

#include <stdarg.h>
#include BOSS_OPENH264_U_typedefs_h //original-code:"typedefs.h"
#include BOSS_OPENH264_U_utils_h //original-code:"utils.h"
#include BOSS_OPENH264_U_codec_app_def_h //original-code:"codec_app_def.h"
#include BOSS_OPENH264_U_codec_api_h //original-code:"codec_api.h"

class welsCodecTrace {
 public:
  welsCodecTrace();
  ~welsCodecTrace();

  void SetCodecInstance (void* pCodecInstance);
  void SetTraceLevel (const int32_t kiLevel);
  void SetTraceCallback (WelsTraceCallback func);
  void SetTraceCallbackContext (void* pCtx);

 private:
  static void StaticCodecTrace (void* pCtx, const int32_t kiLevel, const char* kpStrFormat, va_list vl);
  void CodecTrace (const int32_t kiLevel, const char* kpStrFormat, va_list vl);

  int32_t       m_iTraceLevel;
  WelsTraceCallback m_fpTrace;
  void*         m_pTraceCtx;
 public:

  SLogContext m_sLogCtx;
};

#endif //WELS_CODEC_TRACE
