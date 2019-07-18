/*
 * Copyright 2007-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include BOSS_OPENSSL_V_openssl__opensslconf_h //original-code:<openssl/opensslconf.h>
#ifdef OPENSSL_NO_SEED
NON_EMPTY_TRANSLATION_UNIT
#else
# include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
# include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
# include <string.h>
# include <assert.h>
# include <openssl/seed.h>
# include BOSS_OPENSSL_U_internal__evp_int_h //original-code:"internal/evp_int.h"

static int seed_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
                         const unsigned char *iv, int enc);

typedef struct {
    SEED_KEY_SCHEDULE ks;
} EVP_SEED_KEY;

IMPLEMENT_BLOCK_CIPHER(seed, ks, SEED, EVP_SEED_KEY, NID_seed,
                       16, 16, 16, 128, EVP_CIPH_FLAG_DEFAULT_ASN1,
                       seed_init_key, 0, 0, 0, 0)

static int seed_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
                         const unsigned char *iv, int enc)
{
    SEED_set_key(key, &EVP_C_DATA(EVP_SEED_KEY,ctx)->ks);
    return 1;
}

#endif
