/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2017 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include "e_ossltest_err.h"

#ifndef OPENSSL_NO_ERR

static ERR_STRING_DATA OSSLTEST_str_functs[] = {
    {ERR_PACK(0, OSSLTEST_F_BIND_OSSLTEST, 0), "bind_ossltest"},
    {ERR_PACK(0, OSSLTEST_F_OSSLTEST_AES128_INIT_KEY, 0), ""},
    {0, NULL}
};

static ERR_STRING_DATA OSSLTEST_str_reasons[] = {
    {ERR_PACK(0, 0, OSSLTEST_R_INIT_FAILED), "init failed"},
    {0, NULL}
};

#endif

static int lib_code = 0;
static int error_loaded = 0;

static int ERR_load_OSSLTEST_strings(void)
{
    if (lib_code == 0)
        lib_code = ERR_get_next_error_library();

    if (!error_loaded) {
#ifndef OPENSSL_NO_ERR
        ERR_load_strings(lib_code, OSSLTEST_str_functs);
        ERR_load_strings(lib_code, OSSLTEST_str_reasons);
#endif
        error_loaded = 1;
    }
    return 1;
}

static void ERR_unload_OSSLTEST_strings(void)
{
    if (error_loaded) {
#ifndef OPENSSL_NO_ERR
        ERR_unload_strings(lib_code, OSSLTEST_str_functs);
        ERR_unload_strings(lib_code, OSSLTEST_str_reasons);
#endif
        error_loaded = 0;
    }
}

static void ERR_OSSLTEST_error(int function, int reason, char *file, int line)
{
    if (lib_code == 0)
        lib_code = ERR_get_next_error_library();
    ERR_PUT_error(lib_code, function, reason, file, line);
}
