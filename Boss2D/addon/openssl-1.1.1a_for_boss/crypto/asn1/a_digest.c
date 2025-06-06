/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <time.h>
#include <sys/types.h>

#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"

#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
#include BOSS_OPENSSL_V_openssl__buffer_h //original-code:<openssl/buffer.h>
#include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>

#ifndef NO_ASN1_OLD

int ASN1_digest(i2d_of_void *i2d, const EVP_MD *type, char *data,
                unsigned char *md, unsigned int *len)
{
    int i;
    unsigned char *str, *p;

    i = i2d(data, NULL);
    if ((str = OPENSSL_malloc(i)) == NULL) {
        ASN1err(ASN1_F_ASN1_DIGEST, ERR_R_MALLOC_FAILURE);
        return 0;
    }
    p = str;
    i2d(data, &p);

    if (!EVP_Digest(str, i, md, len, type, NULL)) {
        OPENSSL_free(str);
        return 0;
    }
    OPENSSL_free(str);
    return 1;
}

#endif

int ASN1_item_digest(const ASN1_ITEM *it, const EVP_MD *type, void *asn,
                     unsigned char *md, unsigned int *len)
{
    int i;
    unsigned char *str = NULL;

    i = ASN1_item_i2d(asn, &str, it);
    if (!str)
        return 0;

    if (!EVP_Digest(str, i, md, len, type, NULL)) {
        OPENSSL_free(str);
        return 0;
    }
    OPENSSL_free(str);
    return 1;
}
