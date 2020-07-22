// author BOSS

/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"

// removed by BOSS: #include "buildinf.h"

unsigned long OpenSSL_version_num(void)
{
    return OPENSSL_VERSION_NUMBER;
}

const char *OpenSSL_version(int t)
{
    switch (t) {
    case OPENSSL_VERSION:
        return OPENSSL_VERSION_TEXT;
    case OPENSSL_BUILT_ON:
        // removed by BOSS: return DATE;
        return ("built on: date not available"); // added by BOSS
    case OPENSSL_CFLAGS:
        // removed by BOSS: return compiler_flags;
        return ("compiler: information not available"); // added by BOSS
    case OPENSSL_PLATFORM:
        // removed by BOSS: return PLATFORM;
        return ("platform: information not available"); // added by BOSS
    case OPENSSL_DIR:
#ifdef OPENSSLDIR
        return "OPENSSLDIR: \"" OPENSSLDIR "\"";
#else
        return "OPENSSLDIR: N/A";
#endif
    case OPENSSL_ENGINES_DIR:
#ifdef ENGINESDIR
        return "ENGINESDIR: \"" ENGINESDIR "\"";
#else
        return "ENGINESDIR: N/A";
#endif
    }
    return "not available";
}
