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
#ifndef OPENSSL_NO_BF
# include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
# include BOSS_OPENSSL_U_internal__evp_int_h //original-code:"internal/evp_int.h"
# include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
# include <openssl/blowfish.h>

static int bf_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
                       const unsigned char *iv, int enc);

typedef struct {
    BF_KEY ks;
} EVP_BF_KEY;

# define data(ctx)       EVP_C_DATA(EVP_BF_KEY,ctx)

IMPLEMENT_BLOCK_CIPHER(bf, ks, BF, EVP_BF_KEY, NID_bf, 8, 16, 8, 64,
                       EVP_CIPH_VARIABLE_LENGTH, bf_init_key, NULL,
                       EVP_CIPHER_set_asn1_iv, EVP_CIPHER_get_asn1_iv, NULL)

static int bf_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
                       const unsigned char *iv, int enc)
{
    BF_set_key(&data(ctx)->ks, EVP_CIPHER_CTX_key_length(ctx), key);
    return 1;
}

#endif
