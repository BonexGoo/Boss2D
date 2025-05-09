/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include BOSS_OPENSSL_V_openssl__comperr_h //original-code:<openssl/comperr.h>

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA COMP_str_functs[] = {
    {ERR_PACK(ERR_LIB_COMP, COMP_F_BIO_ZLIB_FLUSH, 0), "bio_zlib_flush"},
    {ERR_PACK(ERR_LIB_COMP, COMP_F_BIO_ZLIB_NEW, 0), "bio_zlib_new"},
    {ERR_PACK(ERR_LIB_COMP, COMP_F_BIO_ZLIB_READ, 0), "bio_zlib_read"},
    {ERR_PACK(ERR_LIB_COMP, COMP_F_BIO_ZLIB_WRITE, 0), "bio_zlib_write"},
    {ERR_PACK(ERR_LIB_COMP, COMP_F_COMP_CTX_NEW, 0), "COMP_CTX_new"},
    {0, NULL}
};

static const ERR_STRING_DATA COMP_str_reasons[] = {
    {ERR_PACK(ERR_LIB_COMP, 0, COMP_R_ZLIB_DEFLATE_ERROR),
    "zlib deflate error"},
    {ERR_PACK(ERR_LIB_COMP, 0, COMP_R_ZLIB_INFLATE_ERROR),
    "zlib inflate error"},
    {ERR_PACK(ERR_LIB_COMP, 0, COMP_R_ZLIB_NOT_SUPPORTED),
    "zlib not supported"},
    {0, NULL}
};

#endif

int ERR_load_COMP_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_func_error_string(COMP_str_functs[0].error) == NULL) {
        ERR_load_strings_const(COMP_str_functs);
        ERR_load_strings_const(COMP_str_reasons);
    }
#endif
    return 1;
}
