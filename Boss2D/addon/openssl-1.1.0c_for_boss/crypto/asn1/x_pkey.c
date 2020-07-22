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
#include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
#include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
#include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>

X509_PKEY *X509_PKEY_new(void)
{
    X509_PKEY *ret = NULL;

    ret = OPENSSL_zalloc(sizeof(*ret));
    if (ret == NULL)
        goto err;

    ret->enc_algor = X509_ALGOR_new();
    ret->enc_pkey = ASN1_OCTET_STRING_new();
    if (ret->enc_algor == NULL || ret->enc_pkey == NULL)
        goto err;

    return ret;
err:
    X509_PKEY_free(ret);
    ASN1err(ASN1_F_X509_PKEY_NEW, ERR_R_MALLOC_FAILURE);
    return NULL;
}

void X509_PKEY_free(X509_PKEY *x)
{
    if (x == NULL)
        return;

    X509_ALGOR_free(x->enc_algor);
    ASN1_OCTET_STRING_free(x->enc_pkey);
    EVP_PKEY_free(x->dec_pkey);
    if (x->key_free)
        OPENSSL_free(x->key_data);
    OPENSSL_free(x);
}
