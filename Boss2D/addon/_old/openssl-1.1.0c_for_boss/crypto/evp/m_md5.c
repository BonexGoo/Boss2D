/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"

#ifndef OPENSSL_NO_MD5

# include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
# include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
# include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>
# include BOSS_OPENSSL_V_openssl__md5_h //original-code:<openssl/md5.h>
# include BOSS_OPENSSL_V_openssl__rsa_h //original-code:<openssl/rsa.h>
# include BOSS_OPENSSL_U_internal__evp_int_h //original-code:"internal/evp_int.h"

static int init(EVP_MD_CTX *ctx)
{
    return MD5_Init(EVP_MD_CTX_md_data(ctx));
}

static int update(EVP_MD_CTX *ctx, const void *data, size_t count)
{
    return MD5_Update(EVP_MD_CTX_md_data(ctx), data, count);
}

static int final(EVP_MD_CTX *ctx, unsigned char *md)
{
    return MD5_Final(md, EVP_MD_CTX_md_data(ctx));
}

static const EVP_MD md5_md = {
    NID_md5,
    NID_md5WithRSAEncryption,
    MD5_DIGEST_LENGTH,
    0,
    init,
    update,
    final,
    NULL,
    NULL,
    MD5_CBLOCK,
    sizeof(EVP_MD *) + sizeof(MD5_CTX),
};

const EVP_MD *EVP_md5(void)
{
    return (&md5_md);
}
#endif
