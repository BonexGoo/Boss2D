/*
 * Copyright 2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdlib.h>
#include <string.h>
#include BOSS_OPENSSL_V_openssl__hmac_h //original-code:<openssl/hmac.h>
#include BOSS_OPENSSL_V_openssl__kdf_h //original-code:<openssl/kdf.h>
#include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_U_internal__evp_int_h //original-code:"internal/evp_int.h"

#define HKDF_MAXBUF 1024

static unsigned char *HKDF(const EVP_MD *evp_md,
                           const unsigned char *salt, size_t salt_len,
                           const unsigned char *key, size_t key_len,
                           const unsigned char *info, size_t info_len,
                           unsigned char *okm, size_t okm_len);

static unsigned char *HKDF_Extract(const EVP_MD *evp_md,
                                   const unsigned char *salt, size_t salt_len,
                                   const unsigned char *key, size_t key_len,
                                   unsigned char *prk, size_t *prk_len);

static unsigned char *HKDF_Expand(const EVP_MD *evp_md,
                                  const unsigned char *prk, size_t prk_len,
                                  const unsigned char *info, size_t info_len,
                                  unsigned char *okm, size_t okm_len);

typedef struct {
    const EVP_MD *md;
    unsigned char *salt;
    size_t salt_len;
    unsigned char *key;
    size_t key_len;
    unsigned char info[HKDF_MAXBUF];
    size_t info_len;
} HKDF_PKEY_CTX;

static int pkey_hkdf_init(EVP_PKEY_CTX *ctx)
{
    HKDF_PKEY_CTX *kctx;

    kctx = OPENSSL_zalloc(sizeof(*kctx));
    if (kctx == NULL)
        return 0;

    ctx->data = kctx;

    return 1;
}

static void pkey_hkdf_cleanup(EVP_PKEY_CTX *ctx)
{
    HKDF_PKEY_CTX *kctx = ctx->data;
    OPENSSL_clear_free(kctx->salt, kctx->salt_len);
    OPENSSL_clear_free(kctx->key, kctx->key_len);
    OPENSSL_cleanse(kctx->info, kctx->info_len);
    OPENSSL_free(kctx);
}

static int pkey_hkdf_ctrl(EVP_PKEY_CTX *ctx, int type, int p1, void *p2)
{
    HKDF_PKEY_CTX *kctx = ctx->data;

    switch (type) {
    case EVP_PKEY_CTRL_HKDF_MD:
        if (p2 == NULL)
            return 0;

        kctx->md = p2;
        return 1;

    case EVP_PKEY_CTRL_HKDF_SALT:
        if (p1 == 0 || p2 == NULL)
            return 1;

        if (p1 < 0)
            return 0;

        if (kctx->salt != NULL)
            OPENSSL_clear_free(kctx->salt, kctx->salt_len);

        kctx->salt = OPENSSL_memdup(p2, p1);
        if (kctx->salt == NULL)
            return 0;

        kctx->salt_len = p1;
        return 1;

    case EVP_PKEY_CTRL_HKDF_KEY:
        if (p1 < 0)
            return 0;

        if (kctx->key != NULL)
            OPENSSL_clear_free(kctx->key, kctx->key_len);

        kctx->key = OPENSSL_memdup(p2, p1);
        if (kctx->key == NULL)
            return 0;

        kctx->key_len  = p1;
        return 1;

    case EVP_PKEY_CTRL_HKDF_INFO:
        if (p1 == 0 || p2 == NULL)
            return 1;

        if (p1 < 0 || p1 > (int)(HKDF_MAXBUF - kctx->info_len))
            return 0;

        memcpy(kctx->info + kctx->info_len, p2, p1);
        kctx->info_len += p1;
        return 1;

    default:
        return -2;

    }
}

static int pkey_hkdf_ctrl_str(EVP_PKEY_CTX *ctx, const char *type,
                              const char *value)
{
    if (strcmp(type, "md") == 0)
        return EVP_PKEY_CTX_set_hkdf_md(ctx, EVP_get_digestbyname(value));

    if (strcmp(type, "salt") == 0)
        return EVP_PKEY_CTX_str2ctrl(ctx, EVP_PKEY_CTRL_HKDF_SALT, value);

    if (strcmp(type, "hexsalt") == 0)
        return EVP_PKEY_CTX_hex2ctrl(ctx, EVP_PKEY_CTRL_HKDF_SALT, value);

    if (strcmp(type, "key") == 0)
        return EVP_PKEY_CTX_str2ctrl(ctx, EVP_PKEY_CTRL_HKDF_KEY, value);

    if (strcmp(type, "hexkey") == 0)
        return EVP_PKEY_CTX_hex2ctrl(ctx, EVP_PKEY_CTRL_HKDF_KEY, value);

    if (strcmp(type, "info") == 0)
        return EVP_PKEY_CTX_str2ctrl(ctx, EVP_PKEY_CTRL_HKDF_INFO, value);

    if (strcmp(type, "hexinfo") == 0)
        return EVP_PKEY_CTX_hex2ctrl(ctx, EVP_PKEY_CTRL_HKDF_INFO, value);

    return -2;
}

static int pkey_hkdf_derive(EVP_PKEY_CTX *ctx, unsigned char *key,
                            size_t *keylen)
{
    HKDF_PKEY_CTX *kctx = ctx->data;

    if (kctx->md == NULL || kctx->key == NULL)
        return 0;

    if (HKDF(kctx->md, kctx->salt, kctx->salt_len, kctx->key, kctx->key_len,
             kctx->info, kctx->info_len, key, *keylen) == NULL)
    {
        return 0;
    }

    return 1;
}

const EVP_PKEY_METHOD hkdf_pkey_meth = {
    EVP_PKEY_HKDF,
    0,
    pkey_hkdf_init,
    0,
    pkey_hkdf_cleanup,

    0, 0,
    0, 0,

    0,
    0,

    0,
    0,

    0, 0,

    0, 0, 0, 0,

    0, 0,

    0, 0,

    0,
    pkey_hkdf_derive,
    pkey_hkdf_ctrl,
    pkey_hkdf_ctrl_str
};

static unsigned char *HKDF(const EVP_MD *evp_md,
                           const unsigned char *salt, size_t salt_len,
                           const unsigned char *key, size_t key_len,
                           const unsigned char *info, size_t info_len,
                           unsigned char *okm, size_t okm_len)
{
    unsigned char prk[EVP_MAX_MD_SIZE];
    size_t prk_len;

    if (!HKDF_Extract(evp_md, salt, salt_len, key, key_len, prk, &prk_len))
        return NULL;

    return HKDF_Expand(evp_md, prk, prk_len, info, info_len, okm, okm_len);
}

static unsigned char *HKDF_Extract(const EVP_MD *evp_md,
                                   const unsigned char *salt, size_t salt_len,
                                   const unsigned char *key, size_t key_len,
                                   unsigned char *prk, size_t *prk_len)
{
    unsigned int tmp_len;

    if (!HMAC(evp_md, salt, salt_len, key, key_len, prk, &tmp_len))
        return NULL;

    *prk_len = tmp_len;
    return prk;
}

static unsigned char *HKDF_Expand(const EVP_MD *evp_md,
                                  const unsigned char *prk, size_t prk_len,
                                  const unsigned char *info, size_t info_len,
                                  unsigned char *okm, size_t okm_len)
{
    HMAC_CTX *hmac;

    unsigned int i;

    unsigned char prev[EVP_MAX_MD_SIZE];

    size_t done_len = 0, dig_len = EVP_MD_size(evp_md);

    size_t n = okm_len / dig_len;
    if (okm_len % dig_len)
        n++;

    if (n > 255)
        return NULL;

    if ((hmac = HMAC_CTX_new()) == NULL)
        return NULL;

    if (!HMAC_Init_ex(hmac, prk, prk_len, evp_md, NULL))
        goto err;

    for (i = 1; i <= n; i++) {
        size_t copy_len;
        const unsigned char ctr = i;

        if (i > 1) {
            if (!HMAC_Init_ex(hmac, NULL, 0, NULL, NULL))
                goto err;

            if (!HMAC_Update(hmac, prev, dig_len))
                goto err;
        }

        if (!HMAC_Update(hmac, info, info_len))
            goto err;

        if (!HMAC_Update(hmac, &ctr, 1))
            goto err;

        if (!HMAC_Final(hmac, prev, NULL))
            goto err;

        copy_len = (done_len + dig_len > okm_len) ?
                       okm_len - done_len :
                       dig_len;

        memcpy(okm + done_len, prev, copy_len);

        done_len += copy_len;
    }

    HMAC_CTX_free(hmac);
    return okm;

 err:
    HMAC_CTX_free(hmac);
    return NULL;
}
