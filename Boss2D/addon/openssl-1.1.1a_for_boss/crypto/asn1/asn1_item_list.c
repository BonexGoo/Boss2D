/*
 * Copyright 2000-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__asn1_h //original-code:<openssl/asn1.h>
#include BOSS_OPENSSL_V_openssl__asn1t_h //original-code:<openssl/asn1t.h>
#include BOSS_OPENSSL_V_openssl__cms_h //original-code:<openssl/cms.h>
#include BOSS_OPENSSL_V_openssl__dh_h //original-code:<openssl/dh.h>
#include BOSS_OPENSSL_V_openssl__ocsp_h //original-code:<openssl/ocsp.h>
#include BOSS_OPENSSL_V_openssl__pkcs7_h //original-code:<openssl/pkcs7.h>
#include BOSS_OPENSSL_V_openssl__pkcs12_h //original-code:<openssl/pkcs12.h>
#include BOSS_OPENSSL_V_openssl__rsa_h //original-code:<openssl/rsa.h>
#include BOSS_OPENSSL_V_openssl__x509v3_h //original-code:<openssl/x509v3.h>

#include "asn1_item_list.h"

const ASN1_ITEM *ASN1_ITEM_lookup(const char *name)
{
    size_t i;

    for (i = 0; i < OSSL_NELEM(asn1_item_list); i++) {
        const ASN1_ITEM *it = ASN1_ITEM_ptr(asn1_item_list[i]);

        if (strcmp(it->sname, name) == 0)
            return it;
    }
    return NULL;
}

const ASN1_ITEM *ASN1_ITEM_get(size_t i)
{
    if (i >= OSSL_NELEM(asn1_item_list))
        return NULL;
    return ASN1_ITEM_ptr(asn1_item_list[i]);
}
