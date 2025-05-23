/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/*
 * NOTE: this file was auto generated by the mkerr.pl script: any changes
 * made to it will be overwritten when the script next updates this file,
 * only reason strings will be preserved.
 */

#include <stdio.h>
#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include "e_ossltest_err.h"

/* BEGIN ERROR CODES */
#ifndef OPENSSL_NO_ERR

# define ERR_FUNC(func) ERR_PACK(0,func,0)
# define ERR_REASON(reason) ERR_PACK(0,0,reason)

static ERR_STRING_DATA OSSLTEST_str_functs[] = {
    {ERR_FUNC(OSSLTEST_F_BIND_OSSLTEST), "BIND_OSSLTEST"},
    {ERR_FUNC(OSSLTEST_F_OSSLTEST_AES128_INIT_KEY),
     "OSSLTEST_AES128_INIT_KEY"},
    {0, NULL}
};

static ERR_STRING_DATA OSSLTEST_str_reasons[] = {
    {ERR_REASON(OSSLTEST_R_INIT_FAILED), "init failed"},
    {0, NULL}
};

#endif

#ifdef OSSLTEST_LIB_NAME
static ERR_STRING_DATA OSSLTEST_lib_name[] = {
    {0, OSSLTEST_LIB_NAME},
    {0, NULL}
};
#endif

static int OSSLTEST_lib_error_code = 0;
static int OSSLTEST_error_init = 1;

static void ERR_load_OSSLTEST_strings(void)
{
    if (OSSLTEST_lib_error_code == 0)
        OSSLTEST_lib_error_code = ERR_get_next_error_library();

    if (OSSLTEST_error_init) {
        OSSLTEST_error_init = 0;
#ifndef OPENSSL_NO_ERR
        ERR_load_strings(OSSLTEST_lib_error_code, OSSLTEST_str_functs);
        ERR_load_strings(OSSLTEST_lib_error_code, OSSLTEST_str_reasons);
#endif

#ifdef OSSLTEST_LIB_NAME
        OSSLTEST_lib_name->error = ERR_PACK(OSSLTEST_lib_error_code, 0, 0);
        ERR_load_strings(0, OSSLTEST_lib_name);
#endif
    }
}

static void ERR_unload_OSSLTEST_strings(void)
{
    if (OSSLTEST_error_init == 0) {
#ifndef OPENSSL_NO_ERR
        ERR_unload_strings(OSSLTEST_lib_error_code, OSSLTEST_str_functs);
        ERR_unload_strings(OSSLTEST_lib_error_code, OSSLTEST_str_reasons);
#endif

#ifdef OSSLTEST_LIB_NAME
        ERR_unload_strings(0, OSSLTEST_lib_name);
#endif
        OSSLTEST_error_init = 1;
    }
}

static void ERR_OSSLTEST_error(int function, int reason, char *file, int line)
{
    if (OSSLTEST_lib_error_code == 0)
        OSSLTEST_lib_error_code = ERR_get_next_error_library();
    ERR_PUT_error(OSSLTEST_lib_error_code, function, reason, file, line);
}
