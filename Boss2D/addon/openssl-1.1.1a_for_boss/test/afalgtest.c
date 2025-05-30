/*
 * Copyright 2016-2017 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_V_openssl__opensslconf_h //original-code:<openssl/opensslconf.h>

#include <string.h>
#include BOSS_OPENSSL_V_openssl__engine_h //original-code:<openssl/engine.h>
#include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
#include BOSS_OPENSSL_V_openssl__rand_h //original-code:<openssl/rand.h>
#include "testutil.h"

/* Use a buffer size which is not aligned to block size */
#define BUFFER_SIZE     17

#ifndef OPENSSL_NO_ENGINE
static ENGINE *e;
#endif


#ifndef OPENSSL_NO_AFALGENG
# include <linux/version.h>
# define K_MAJ   4
# define K_MIN1  1
# define K_MIN2  0
# if LINUX_VERSION_CODE < KERNEL_VERSION(K_MAJ, K_MIN1, K_MIN2)
/*
 * If we get here then it looks like there is a mismatch between the linux
 * headers and the actual kernel version, so we have tried to compile with
 * afalg support, but then skipped it in e_afalg.c. As far as this test is
 * concerned we behave as if we had been configured without support
 */
#  define OPENSSL_NO_AFALGENG
# endif
#endif

#ifndef OPENSSL_NO_AFALGENG
static int test_afalg_aes_cbc(int keysize_idx)
{
    EVP_CIPHER_CTX *ctx;
    const EVP_CIPHER *cipher;
    unsigned char key[] = "\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
                          "\x51\x2e\x03\xd5\x34\x12\x00\x06"
                          "\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
                          "\x51\x2e\x03\xd5\x34\x12\x00\x06";
    unsigned char iv[] = "\x3d\xaf\xba\x42\x9d\x9e\xb4\x30"
                         "\xb4\x22\xda\x80\x2c\x9f\xac\x41";
    /* input = "Single block msg\n"  17Bytes*/
    unsigned char in[BUFFER_SIZE] = "\x53\x69\x6e\x67\x6c\x65\x20\x62"
                                    "\x6c\x6f\x63\x6b\x20\x6d\x73\x67\x0a";
    unsigned char ebuf[BUFFER_SIZE + 32];
    unsigned char dbuf[BUFFER_SIZE + 32];
    unsigned char encresult_128[] = "\xe3\x53\x77\x9c\x10\x79\xae\xb8"
                                    "\x27\x08\x94\x2d\xbe\x77\x18\x1a\x2d";
    unsigned char encresult_192[] = "\xf7\xe4\x26\xd1\xd5\x4f\x8f\x39"
                                    "\xb1\x9e\xe0\xdf\x61\xb9\xc2\x55\xeb";
    unsigned char encresult_256[] = "\xa0\x76\x85\xfd\xc1\x65\x71\x9d"
                                    "\xc7\xe9\x13\x6e\xae\x55\x49\xb4\x13";
    unsigned char *enc_result = NULL;

    int encl, encf, decl, decf;
    int ret = 0;

    switch (keysize_idx) {
        case 0:
            cipher = EVP_aes_128_cbc();
            enc_result = &encresult_128[0];
            break;
        case 1:
            cipher = EVP_aes_192_cbc();
            enc_result = &encresult_192[0];
            break;
        case 2:
            cipher = EVP_aes_256_cbc();
            enc_result = &encresult_256[0];
            break;
        default:
            cipher = NULL;
    }
    if (!TEST_ptr(ctx = EVP_CIPHER_CTX_new()))
            return 0;

    if (!TEST_true(EVP_CipherInit_ex(ctx, cipher, e, key, iv, 1))
            || !TEST_true(EVP_CipherUpdate(ctx, ebuf, &encl, in, BUFFER_SIZE))
            || !TEST_true(EVP_CipherFinal_ex(ctx, ebuf+encl, &encf)))
        goto end;
    encl += encf;

    if (!TEST_mem_eq(enc_result, BUFFER_SIZE, ebuf, BUFFER_SIZE))
        goto end;

    if (!TEST_true(EVP_CIPHER_CTX_reset(ctx))
            || !TEST_true(EVP_CipherInit_ex(ctx, cipher, e, key, iv, 0))
            || !TEST_true(EVP_CipherUpdate(ctx, dbuf, &decl, ebuf, encl))
            || !TEST_true(EVP_CipherFinal_ex(ctx, dbuf+decl, &decf)))
        goto end;
    decl += decf;

    if (!TEST_int_eq(decl, BUFFER_SIZE)
            || !TEST_mem_eq(dbuf, BUFFER_SIZE, in, BUFFER_SIZE))
        goto end;

    ret = 1;

 end:
    EVP_CIPHER_CTX_free(ctx);
    return ret;
}
#endif

#ifndef OPENSSL_NO_ENGINE
int global_init(void)
{
    ENGINE_load_builtin_engines();
# ifndef OPENSSL_NO_STATIC_ENGINE
    OPENSSL_init_crypto(OPENSSL_INIT_ENGINE_AFALG, NULL);
# endif
    return 1;
}
#endif

int setup_tests(void)
{
#ifndef OPENSSL_NO_ENGINE
    if ((e = ENGINE_by_id("afalg")) == NULL) {
        /* Probably a platform env issue, not a test failure. */
        TEST_info("Can't load AFALG engine");
    } else {
# ifndef OPENSSL_NO_AFALGENG
        ADD_ALL_TESTS(test_afalg_aes_cbc, 3);
# endif
    }
#endif

    return 1;
}

#ifndef OPENSSL_NO_ENGINE
void cleanup_tests(void)
{
    ENGINE_free(e);
}
#endif
