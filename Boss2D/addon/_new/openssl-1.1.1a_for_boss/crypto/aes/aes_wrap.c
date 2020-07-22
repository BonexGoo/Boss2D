/*
 * Copyright 2008-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__aes_h //original-code:<openssl/aes.h>
#include BOSS_OPENSSL_V_openssl__modes_h //original-code:<openssl/modes.h>

int AES_wrap_key(AES_KEY *key, const unsigned char *iv,
                 unsigned char *out,
                 const unsigned char *in, unsigned int inlen)
{
    return CRYPTO_128_wrap(key, iv, out, in, inlen, (block128_f) AES_encrypt);
}

int AES_unwrap_key(AES_KEY *key, const unsigned char *iv,
                   unsigned char *out,
                   const unsigned char *in, unsigned int inlen)
{
    return CRYPTO_128_unwrap(key, iv, out, in, inlen,
                             (block128_f) AES_decrypt);
}
