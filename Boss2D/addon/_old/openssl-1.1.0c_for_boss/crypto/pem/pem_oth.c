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
#include BOSS_OPENSSL_V_openssl__buffer_h //original-code:<openssl/buffer.h>
#include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
#include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
#include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>
#include BOSS_OPENSSL_V_openssl__pem_h //original-code:<openssl/pem.h>

/* Handle 'other' PEMs: not private keys */

void *PEM_ASN1_read_bio(d2i_of_void *d2i, const char *name, BIO *bp, void **x,
                        pem_password_cb *cb, void *u)
{
    const unsigned char *p = NULL;
    unsigned char *data = NULL;
    long len;
    char *ret = NULL;

    if (!PEM_bytes_read_bio(&data, &len, NULL, name, bp, cb, u))
        return NULL;
    p = data;
    ret = d2i(x, &p, len);
    if (ret == NULL)
        PEMerr(PEM_F_PEM_ASN1_READ_BIO, ERR_R_ASN1_LIB);
    OPENSSL_free(data);
    return (ret);
}
