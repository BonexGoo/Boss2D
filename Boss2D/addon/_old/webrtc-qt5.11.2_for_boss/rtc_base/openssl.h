/*
 *  Copyright 2013 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_OPENSSL_H_
#define RTC_BASE_OPENSSL_H_

#include BOSS_OPENSSL_V_openssl__ssl_h //original-code:<openssl/ssl.h>

#if (OPENSSL_VERSION_NUMBER < 0x10000000L)
#error OpenSSL is older than 1.0.0, which is the minimum supported version.
#endif

#endif  // RTC_BASE_OPENSSL_H_
