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
#include BOSS_OPENSSL_V_openssl__bn_h //original-code:<openssl/bn.h>
#include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
#include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
#include BOSS_OPENSSL_V_openssl__asn1_h //original-code:<openssl/asn1.h>
#include BOSS_OPENSSL_V_openssl__rsa_h //original-code:<openssl/rsa.h>
#include BOSS_OPENSSL_V_openssl__dsa_h //original-code:<openssl/dsa.h>
#include BOSS_OPENSSL_V_openssl__ec_h //original-code:<openssl/ec.h>

#include BOSS_OPENSSL_U_internal__evp_int_h //original-code:"internal/evp_int.h"

EVP_PKEY *d2i_PublicKey(int type, EVP_PKEY **a, const unsigned char **pp,
                        long length)
{
    EVP_PKEY *ret;

    if ((a == NULL) || (*a == NULL)) {
        if ((ret = EVP_PKEY_new()) == NULL) {
            ASN1err(ASN1_F_D2I_PUBLICKEY, ERR_R_EVP_LIB);
            return NULL;
        }
    } else
        ret = *a;

    if (!EVP_PKEY_set_type(ret, type)) {
        ASN1err(ASN1_F_D2I_PUBLICKEY, ERR_R_EVP_LIB);
        goto err;
    }

    switch (EVP_PKEY_id(ret)) {
#ifndef OPENSSL_NO_RSA
    case EVP_PKEY_RSA:
        if ((ret->pkey.rsa = d2i_RSAPublicKey(NULL, pp, length)) == NULL) {
            ASN1err(ASN1_F_D2I_PUBLICKEY, ERR_R_ASN1_LIB);
            goto err;
        }
        break;
#endif
#ifndef OPENSSL_NO_DSA
    case EVP_PKEY_DSA:
        /* TMP UGLY CAST */
        if (!d2i_DSAPublicKey(&ret->pkey.dsa, pp, length)) {
            ASN1err(ASN1_F_D2I_PUBLICKEY, ERR_R_ASN1_LIB);
            goto err;
        }
        break;
#endif
#ifndef OPENSSL_NO_EC
    case EVP_PKEY_EC:
        if (!o2i_ECPublicKey(&ret->pkey.ec, pp, length)) {
            ASN1err(ASN1_F_D2I_PUBLICKEY, ERR_R_ASN1_LIB);
            goto err;
        }
        break;
#endif
    default:
        ASN1err(ASN1_F_D2I_PUBLICKEY, ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE);
        goto err;
    }
    if (a != NULL)
        (*a) = ret;
    return ret;
 err:
    if (a == NULL || *a != ret)
        EVP_PKEY_free(ret);
    return NULL;
}
