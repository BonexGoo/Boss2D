/*
 * Copyright 2015-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"

#ifndef OPENSSL_NO_CHACHA

# include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
# include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
# include "evp_locl.h"
# include BOSS_OPENSSL_U_internal__evp_int_h //original-code:"internal/evp_int.h"
# include BOSS_OPENSSL_U_internal__chacha_h //original-code:"internal/chacha.h"

typedef struct {
    union {
        double align;   /* this ensures even sizeof(EVP_CHACHA_KEY)%8==0 */
        unsigned int d[CHACHA_KEY_SIZE / 4];
    } key;
    unsigned int  counter[CHACHA_CTR_SIZE / 4];
    unsigned char buf[CHACHA_BLK_SIZE];
    unsigned int  partial_len;
} EVP_CHACHA_KEY;

#define data(ctx)   ((EVP_CHACHA_KEY *)(ctx)->cipher_data)

static int chacha_init_key(EVP_CIPHER_CTX *ctx,
                           const unsigned char user_key[CHACHA_KEY_SIZE],
                           const unsigned char iv[CHACHA_CTR_SIZE], int enc)
{
    EVP_CHACHA_KEY *key = data(ctx);
    unsigned int i;

    if (user_key)
        for (i = 0; i < CHACHA_KEY_SIZE; i+=4) {
            key->key.d[i/4] = CHACHA_U8TOU32(user_key+i);
        }

    if (iv)
        for (i = 0; i < CHACHA_CTR_SIZE; i+=4) {
            key->counter[i/4] = CHACHA_U8TOU32(iv+i);
        }

    key->partial_len = 0;

    return 1;
}

static int chacha_cipher(EVP_CIPHER_CTX * ctx, unsigned char *out,
                         const unsigned char *inp, size_t len)
{
    EVP_CHACHA_KEY *key = data(ctx);
    unsigned int n, rem, ctr32;

    if ((n = key->partial_len)) {
        while (len && n < CHACHA_BLK_SIZE) {
            *out++ = *inp++ ^ key->buf[n++];
            len--;
        }
        key->partial_len = n;

        if (len == 0)
            return 1;

        if (n == CHACHA_BLK_SIZE) {
            key->partial_len = 0;
            key->counter[0]++;
            if (key->counter[0] == 0)
                key->counter[1]++;
        }
    }

    rem = (unsigned int)(len % CHACHA_BLK_SIZE);
    len -= rem;
    ctr32 = key->counter[0];
    while (len >= CHACHA_BLK_SIZE) {
        size_t blocks = len / CHACHA_BLK_SIZE;
        /*
         * 1<<28 is just a not-so-small yet not-so-large number...
         * Below condition is practically never met, but it has to
         * be checked for code correctness.
         */
        if (sizeof(size_t)>sizeof(unsigned int) && blocks>(1U<<28))
            blocks = (1U<<28);

        /*
         * As ChaCha20_ctr32 operates on 32-bit counter, caller
         * has to handle overflow. 'if' below detects the
         * overflow, which is then handled by limiting the
         * amount of blocks to the exact overflow point...
         */
        ctr32 += (unsigned int)blocks;
        if (ctr32 < blocks) {
            blocks -= ctr32;
            ctr32 = 0;
        }
        blocks *= CHACHA_BLK_SIZE;
        ChaCha20_ctr32(out, inp, blocks, key->key.d, key->counter);
        len -= blocks;
        inp += blocks;
        out += blocks;

        key->counter[0] = ctr32;
        if (ctr32 == 0) key->counter[1]++;
    }

    if (rem) {
        memset(key->buf, 0, sizeof(key->buf));
        ChaCha20_ctr32(key->buf, key->buf, CHACHA_BLK_SIZE,
                       key->key.d, key->counter);
        for (n = 0; n < rem; n++)
            out[n] = inp[n] ^ key->buf[n];
        key->partial_len = rem;
    }

    return 1;
}

static const EVP_CIPHER chacha20 = {
    NID_chacha20,
    1,                      /* block_size */
    CHACHA_KEY_SIZE,        /* key_len */
    CHACHA_CTR_SIZE,        /* iv_len, 128-bit counter in the context */
    0,                      /* flags */
    chacha_init_key,
    chacha_cipher,
    NULL,
    sizeof(EVP_CHACHA_KEY),
    NULL,
    NULL,
    NULL,
    NULL
};

const EVP_CIPHER *EVP_chacha20(void)
{
    return (&chacha20);
}

# ifndef OPENSSL_NO_POLY1305
#  include BOSS_OPENSSL_U_internal__poly1305_h //original-code:"internal/poly1305.h"

typedef struct {
    EVP_CHACHA_KEY key;
    unsigned int nonce[12/4];
    unsigned char tag[POLY1305_BLOCK_SIZE];
    struct { uint64_t aad, text; } len;
    int aad, mac_inited, tag_len, nonce_len;
    size_t tls_payload_length;
} EVP_CHACHA_AEAD_CTX;

#  define NO_TLS_PAYLOAD_LENGTH ((size_t)-1)
#  define aead_data(ctx)        ((EVP_CHACHA_AEAD_CTX *)(ctx)->cipher_data)
#  define POLY1305_ctx(actx)    ((POLY1305 *)(actx + 1))

static int chacha20_poly1305_init_key(EVP_CIPHER_CTX *ctx,
                                      const unsigned char *inkey,
                                      const unsigned char *iv, int enc)
{
    EVP_CHACHA_AEAD_CTX *actx = aead_data(ctx);

    if (!inkey && !iv)
        return 1;

    actx->len.aad = 0;
    actx->len.text = 0;
    actx->aad = 0;
    actx->mac_inited = 0;
    actx->tls_payload_length = NO_TLS_PAYLOAD_LENGTH;

    if (iv != NULL) {
        unsigned char temp[CHACHA_CTR_SIZE] = { 0 };

        /* pad on the left */
        if (actx->nonce_len <= CHACHA_CTR_SIZE)
            memcpy(temp + CHACHA_CTR_SIZE - actx->nonce_len, iv, actx->nonce_len);

        chacha_init_key(ctx, inkey, temp, enc);

        actx->nonce[0] = actx->key.counter[1];
        actx->nonce[1] = actx->key.counter[2];
        actx->nonce[2] = actx->key.counter[3];
    } else {
        chacha_init_key(ctx, inkey, NULL, enc);
    }

    return 1;
}

static int chacha20_poly1305_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
                                    const unsigned char *in, size_t len)
{
    EVP_CHACHA_AEAD_CTX *actx = aead_data(ctx);
    size_t rem, plen = actx->tls_payload_length;
    static const unsigned char zero[POLY1305_BLOCK_SIZE] = { 0 };

    if (!actx->mac_inited) {
        actx->key.counter[0] = 0;
        memset(actx->key.buf, 0, sizeof(actx->key.buf));
        ChaCha20_ctr32(actx->key.buf, actx->key.buf, CHACHA_BLK_SIZE,
                       actx->key.key.d, actx->key.counter);
        Poly1305_Init(POLY1305_ctx(actx), actx->key.buf);
        actx->key.counter[0] = 1;
        actx->key.partial_len = 0;
        actx->len.aad = actx->len.text = 0;
        actx->mac_inited = 1;
    }

    if (in) {                                   /* aad or text */
        if (out == NULL) {                      /* aad */
            Poly1305_Update(POLY1305_ctx(actx), in, len);
            actx->len.aad += len;
            actx->aad = 1;
            return len;
        } else {                                /* plain- or ciphertext */
            if (actx->aad) {                    /* wrap up aad */
                if ((rem = (size_t)actx->len.aad % POLY1305_BLOCK_SIZE))
                    Poly1305_Update(POLY1305_ctx(actx), zero,
                                    POLY1305_BLOCK_SIZE - rem);
                actx->aad = 0;
            }

            actx->tls_payload_length = NO_TLS_PAYLOAD_LENGTH;
            if (plen == NO_TLS_PAYLOAD_LENGTH)
                plen = len;
            else if (len != plen + POLY1305_BLOCK_SIZE)
                return -1;

            if (ctx->encrypt) {                 /* plaintext */
                chacha_cipher(ctx, out, in, plen);
                Poly1305_Update(POLY1305_ctx(actx), out, plen);
                in += plen;
                out += plen;
                actx->len.text += plen;
            } else {                            /* ciphertext */
                Poly1305_Update(POLY1305_ctx(actx), in, plen);
                chacha_cipher(ctx, out, in, plen);
                in += plen;
                out += plen;
                actx->len.text += plen;
            }
        }
    }
    if (in == NULL                              /* explicit final */
        || plen != len) {                       /* or tls mode */
        const union {
            long one;
            char little;
        } is_endian = { 1 };
        unsigned char temp[POLY1305_BLOCK_SIZE];

        if (actx->aad) {                        /* wrap up aad */
            if ((rem = (size_t)actx->len.aad % POLY1305_BLOCK_SIZE))
                Poly1305_Update(POLY1305_ctx(actx), zero,
                                POLY1305_BLOCK_SIZE - rem);
            actx->aad = 0;
        }

        if ((rem = (size_t)actx->len.text % POLY1305_BLOCK_SIZE))
            Poly1305_Update(POLY1305_ctx(actx), zero,
                            POLY1305_BLOCK_SIZE - rem);

        if (is_endian.little) {
            Poly1305_Update(POLY1305_ctx(actx),
                            (unsigned char *)&actx->len, POLY1305_BLOCK_SIZE);
        } else {
            temp[0]  = (unsigned char)(actx->len.aad);
            temp[1]  = (unsigned char)(actx->len.aad>>8);
            temp[2]  = (unsigned char)(actx->len.aad>>16);
            temp[3]  = (unsigned char)(actx->len.aad>>24);
            temp[4]  = (unsigned char)(actx->len.aad>>32);
            temp[5]  = (unsigned char)(actx->len.aad>>40);
            temp[6]  = (unsigned char)(actx->len.aad>>48);
            temp[7]  = (unsigned char)(actx->len.aad>>56);

            temp[8]  = (unsigned char)(actx->len.text);
            temp[9]  = (unsigned char)(actx->len.text>>8);
            temp[10] = (unsigned char)(actx->len.text>>16);
            temp[11] = (unsigned char)(actx->len.text>>24);
            temp[12] = (unsigned char)(actx->len.text>>32);
            temp[13] = (unsigned char)(actx->len.text>>40);
            temp[14] = (unsigned char)(actx->len.text>>48);
            temp[15] = (unsigned char)(actx->len.text>>56);

            Poly1305_Update(POLY1305_ctx(actx), temp, POLY1305_BLOCK_SIZE);
        }
        Poly1305_Final(POLY1305_ctx(actx), ctx->encrypt ? actx->tag
                                                        : temp);
        actx->mac_inited = 0;

        if (in != NULL && len != plen) {        /* tls mode */
            if (ctx->encrypt) {
                memcpy(out, actx->tag, POLY1305_BLOCK_SIZE);
            } else {
                if (CRYPTO_memcmp(temp, in, POLY1305_BLOCK_SIZE)) {
                    memset(out - plen, 0, plen);
                    return -1;
                }
            }
        }
        else if (!ctx->encrypt) {
            if (CRYPTO_memcmp(temp, actx->tag, actx->tag_len))
                return -1;
        }
    }
    return len;
}

static int chacha20_poly1305_cleanup(EVP_CIPHER_CTX *ctx)
{
    EVP_CHACHA_AEAD_CTX *actx = aead_data(ctx);
    if (actx)
        OPENSSL_cleanse(ctx->cipher_data, sizeof(*ctx) + Poly1305_ctx_size());
    return 1;
}

static int chacha20_poly1305_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg,
                                  void *ptr)
{
    EVP_CHACHA_AEAD_CTX *actx = aead_data(ctx);

    switch(type) {
    case EVP_CTRL_INIT:
        if (actx == NULL)
            actx = ctx->cipher_data
                 = OPENSSL_zalloc(sizeof(*actx) + Poly1305_ctx_size());
        if (actx == NULL) {
            EVPerr(EVP_F_CHACHA20_POLY1305_CTRL, EVP_R_INITIALIZATION_ERROR);
            return 0;
        }
        actx->len.aad = 0;
        actx->len.text = 0;
        actx->aad = 0;
        actx->mac_inited = 0;
        actx->tag_len = 0;
        actx->nonce_len = 12;
        actx->tls_payload_length = NO_TLS_PAYLOAD_LENGTH;
        return 1;

    case EVP_CTRL_COPY:
        if (actx) {
            EVP_CIPHER_CTX *dst = (EVP_CIPHER_CTX *)ptr;

            dst->cipher_data =
                   OPENSSL_memdup(actx, sizeof(*actx) + Poly1305_ctx_size());
            if (dst->cipher_data == NULL) {
                EVPerr(EVP_F_CHACHA20_POLY1305_CTRL, EVP_R_COPY_ERROR);
                return 0;
            }
        }
        return 1;

    case EVP_CTRL_AEAD_SET_IVLEN:
        if (arg <= 0 || arg > CHACHA_CTR_SIZE)
            return 0;
        actx->nonce_len = arg;
        return 1;

    case EVP_CTRL_AEAD_SET_IV_FIXED:
        if (arg != 12)
            return 0;
        actx->nonce[0] = actx->key.counter[1]
                       = CHACHA_U8TOU32((unsigned char *)ptr);
        actx->nonce[1] = actx->key.counter[2]
                       = CHACHA_U8TOU32((unsigned char *)ptr+4);
        actx->nonce[2] = actx->key.counter[3]
                       = CHACHA_U8TOU32((unsigned char *)ptr+8);
        return 1;

    case EVP_CTRL_AEAD_SET_TAG:
        if (arg <= 0 || arg > POLY1305_BLOCK_SIZE)
            return 0;
        if (ptr != NULL) {
            memcpy(actx->tag, ptr, arg);
            actx->tag_len = arg;
        }
        return 1;

    case EVP_CTRL_AEAD_GET_TAG:
        if (arg <= 0 || arg > POLY1305_BLOCK_SIZE || !ctx->encrypt)
            return 0;
        memcpy(ptr, actx->tag, arg);
        return 1;

    case EVP_CTRL_AEAD_TLS1_AAD:
        if (arg != EVP_AEAD_TLS1_AAD_LEN)
            return 0;
        {
            unsigned int len;
            unsigned char *aad = ptr, temp[POLY1305_BLOCK_SIZE];

            len = aad[EVP_AEAD_TLS1_AAD_LEN - 2] << 8 |
                  aad[EVP_AEAD_TLS1_AAD_LEN - 1];
            if (!ctx->encrypt) {
                len -= POLY1305_BLOCK_SIZE;     /* discount attached tag */
                memcpy(temp, aad, EVP_AEAD_TLS1_AAD_LEN - 2);
                aad = temp;
                temp[EVP_AEAD_TLS1_AAD_LEN - 2] = (unsigned char)(len >> 8);
                temp[EVP_AEAD_TLS1_AAD_LEN - 1] = (unsigned char)len;
            }
            actx->tls_payload_length = len;

            /*
             * merge record sequence number as per
             * draft-ietf-tls-chacha20-poly1305-03
             */
            actx->key.counter[1] = actx->nonce[0];
            actx->key.counter[2] = actx->nonce[1] ^ CHACHA_U8TOU32(aad);
            actx->key.counter[3] = actx->nonce[2] ^ CHACHA_U8TOU32(aad+4);
            actx->mac_inited = 0;
            chacha20_poly1305_cipher(ctx, NULL, aad, EVP_AEAD_TLS1_AAD_LEN);
            return POLY1305_BLOCK_SIZE;         /* tag length */
        }

    case EVP_CTRL_AEAD_SET_MAC_KEY:
        /* no-op */
        return 1;

    default:
        return -1;
    }
}

static EVP_CIPHER chacha20_poly1305 = {
    NID_chacha20_poly1305,
    1,                  /* block_size */
    CHACHA_KEY_SIZE,    /* key_len */
    12,                 /* iv_len, 96-bit nonce in the context */
    EVP_CIPH_FLAG_AEAD_CIPHER | EVP_CIPH_CUSTOM_IV |
    EVP_CIPH_ALWAYS_CALL_INIT | EVP_CIPH_CTRL_INIT |
    EVP_CIPH_CUSTOM_COPY | EVP_CIPH_FLAG_CUSTOM_CIPHER,
    chacha20_poly1305_init_key,
    chacha20_poly1305_cipher,
    chacha20_poly1305_cleanup,
    0,          /* 0 moves context-specific structure allocation to ctrl */
    NULL,       /* set_asn1_parameters */
    NULL,       /* get_asn1_parameters */
    chacha20_poly1305_ctrl,
    NULL        /* app_data */
};

const EVP_CIPHER *EVP_chacha20_poly1305(void)
{
    return(&chacha20_poly1305);
}
# endif
#endif
